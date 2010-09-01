/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This class offers the search services
*
*/


// INCLUDE FILES
#include "CCARequest.h"
#include "CCARequestMapper.h"
#include "CCASearchData.h"
#include "CCASearchManager.h"
#include "MCASearchObserverInterface.h"
#include "PublicEngineDefinitions.h"
#include "PrivateEngineDefinitions.h"
#include "ImpsCSPAllErrors.h"
#include "MCAImpsFactory.h"

#include "ChatDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCASearchManager* CCASearchManager::NewL(
    MCAImpsFactory* aIMPSFactory,
    CCARequestMapper*   aRequestMapper )
    {
    CCASearchManager* self = new ( ELeave ) CCASearchManager(
        aIMPSFactory,
        aRequestMapper );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCASearchManager::~CCASearchManager()
    {
    delete iOwnGrouplistCreatorId;
    delete iResults;
    delete iCreatedGroups;
    }

// ---------------------------------------------------------
// CCASearchManager::SetSearchSupported
// ---------------------------------------------------------
//
void CCASearchManager::SetSearchSupported( TBool aSearchSupported )
    {
    iSearchSupported = aSearchSupported;
    }

// ---------------------------------------------------------
// CCASearchManager::StartSearchL
// ---------------------------------------------------------
//
EXPORT_C TInt CCASearchManager::StartSearchL(
    const CSearchPairs& aPairs,
    TInt          aSearchLimit,
    MCASearchObserverInterface* aObserver )
    {
    CHAT_DP_TXT( "CCASearchManager::StartSearchL()...starts" );

    if ( !iSearchSupported )
        {
        User::Leave( ESearchNotSupported );
        }

    // Set data
    iSearchLimit = aSearchLimit;
    iState = ESearchStartRequested;

    // Create the request
    CHAT_DP_FUNC_DP( "StartSearchL", "Creating request" );
    TInt opId = iImpsFundClient->SearchFirstL( aPairs,
                                               aSearchLimit );
    CCARequest* request = iRequestMapper->CreateRequestL( opId, EFalse );

    // remember the observer (synchronous, so only one observer used)
    iObserver = aObserver;

    CHAT_DP_FUNC_DP( "StartSearchL", "Waiting for completion" );

    request->StartWait();

    // execution continues from here
    TInt errorCode( KErrNone );
    errorCode = request->ErrorCode();

    iRequestMapper->RemoveRequest( request );
    iObserver = NULL;

    CHAT_DP_TXT( "CCASearchManager::StartSearchL()...over" );

    return errorCode;
    }


// ---------------------------------------------------------
// CCASearchManager::EndSearchL
// ---------------------------------------------------------
//
EXPORT_C TInt CCASearchManager::EndSearchL()
    {
    CHAT_DP_TXT( "CCASearchManager::EndSearchL()...starts" );

    if ( !iSearchSupported )
        {
        User::Leave( ESearchNotSupported );
        }

    // Create the request and start waiting
    CCARequest* request = iRequestMapper->CreateRequestL(
                              iImpsFundClient->StopSearchL( iSearchID ),
                              EFalse );

    request->StartWait();

    TInt errorCode( KErrNone );

    // Execution continues from here
    errorCode = request->ErrorCode();
    CHAT_DP( D_CHAT_LIT( "EndSearchL() error code %d" ), errorCode );

    iRequestMapper->RemoveRequest( request );
    iObserver = NULL;

    CHAT_DP_TXT( "CCASearchManager::EndSearchL()...over" );

    return errorCode;
    }

// ---------------------------------------------------------
// CCASearchManager::SearchNextL
// ---------------------------------------------------------
//
EXPORT_C TInt CCASearchManager::SearchNextL( TInt aIndex,
                                             MCASearchObserverInterface* aObserver )
    {
    CHAT_DP_FUNC_ENTER( "SearchNextL" );
    CHAT_DP( D_CHAT_LIT( "SearchNextL index is %d" ), aIndex );

    if ( !iSearchSupported )
        {
        User::Leave( ESearchNotSupported );
        }

    if ( aIndex >= SearchDataInterface()->HowManyResultsAvailable() &&
         iResults->NoMoreResultsAvailable() )
        {
        CHAT_DP_TXT( "!!!NO MORE RESULTS!!!" );
        iObserver = NULL;
        User::Leave( ERequestedResultsButNotAvailable );
        }

    if ( iResults->WeHaveData( aIndex ) )
        {
        CHAT_DP_TXT( "   We have results" );
        CHAT_DP_FUNC_DONE( "SearchNextL" );
        iObserver = NULL;
        return KErrNone;
        }
    else
        {
        CHAT_DP_TXT( "   We do not have results" );
        CHAT_DP( D_CHAT_LIT( "Next index %d" ), iResults->NextSearchIndex() );

        // Start waiting
        TInt opId = iImpsFundClient->SearchNextL( iSearchID,
                                                  iResults->NextSearchIndex() );
        CCARequest* request = iRequestMapper->CreateRequestL( opId, EFalse );

        // remember the observer for this operation
        iObserver = aObserver;

        request->StartWait();

        // execution continues from here
        TInt errorCode( KErrNone );
        errorCode = request->ErrorCode();

        iRequestMapper->RemoveRequest( request );

        CHAT_DP_FUNC_DONE( "SearchNextL" );
        iObserver = NULL;

        return errorCode;
        }
    }

// ---------------------------------------------------------
// CCASearchManager::SearchDataInterface
// ---------------------------------------------------------
//
EXPORT_C MCASearchData* CCASearchManager::SearchDataInterface() const
    {
    return iResults;
    }

// ---------------------------------------------------------
// CCASearchManager::HandleSearchL
// ---------------------------------------------------------
//
void CCASearchManager::HandleSearchL( TInt aId,
                                      TInt aSearchId,
                                      TInt aIndex,
                                      TInt aResultsFound,
                                      TBool aCompleted,
                                      TImpsSearchResultType aType,
                                      MDesCArray* aResults ,
                                      TImpsCspIdentifier& /* aCspId */ )
    {
    // NOTES about response API
    // 1. After the search has been started 2nd search and from that point on
    //    the aSearchId is not available (see SCR)
    CHAT_DP_FUNC_ENTER( "HandleSearchL" );
    CHAT_DP( D_CHAT_LIT( "   aIndex = %d" ), aIndex );
    CHAT_DP( D_CHAT_LIT( "   aSearchId = %d" ), aSearchId );
    CHAT_DP( D_CHAT_LIT( "   iSearchID = %d" ), iSearchID );

    TInt leave( KErrNone );

    // Find the request
    CCARequest* request =
        const_cast<CCARequest*>( iRequestMapper->FindRequest( aId ) );

    if ( request )
        {
        if ( iState == ESearchStartRequested )
            {
            iResults->ClearResults();
            }

        iResults->UpdateMaximumPossibleResults( aResultsFound );

        TRAP( leave, SaveResultsL( aIndex, aCompleted, aType, aResults ) );
        if ( leave != KErrNone )
            {
            CHAT_DP_FUNC_DP( "HandleSearchL", "Got an error" );

            if ( iObserver )
                {
                iObserver->HandleSearchError( EECouldntSaveSearchResults );
                }
            }
        else
            {
            if ( iState == ESearchStartRequested )
                {
                // Update search ID for the first and only time
                iSearchID = aSearchId;

                // Update state
                iState = ESearchGoingOn;
                }
            }
        }

    // If code added/changed before HandleRequest, then make sure that
    // code does not Leave before HandleRequest, because request
    // response waiting does not stop (and waitnote) before calling
    // HandleRequest
    CHAT_DP_FUNC_DP( "HandleSearchL", "Handling request" );
    TInt status( KErrNone );
    if ( leave )
        {
        status = leave;
        }
    iRequestMapper->HandleRequest( aId, status );

    if ( iObserver )
        {
        CHAT_DP_FUNC_DP( "HandleSearchL", "Invoking observer" );
        iObserver->HandleSearchFinished();
        }

    // leave on system errors
    if ( leave > Imps_ERROR_BASE )
        {
        User::LeaveIfError( leave );
        }

    CHAT_DP_FUNC_DONE( "HandleSearchL" );
    }

// ---------------------------------------------------------
// CCASearchManager::HandleSearchStoppedL
// ---------------------------------------------------------
//
void CCASearchManager::HandleSearchStoppedL( TInt aId,
                                             TInt /* aSearchId */,
                                             TImpsCspIdentifier& /* aCspId */ )
    {
    CHAT_DP_FUNC_ENTER( "HandleSearchStoppedL" );

    iResults->ClearResults();

    iState = ENoActivity;

    // If code added/changed before HandleRequest, then make sure that
    // code does not Leave before HandleRequest, because request
    // response waiting does not stop (and waitnote) before calling
    // HandleRequest

    iRequestMapper->HandleRequest( aId, KErrNone );

    iObserver = NULL;
    CHAT_DP_FUNC_DONE( "HandleSearchStoppedL" );
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCASearchManager::CCASearchManager(
    MCAImpsFactory* aIMPSFactory,
    CCARequestMapper*   aRequestMapper )
        :   iImpsFactory( aIMPSFactory ),
        iState( ENoActivity ),
        iRequestMapper( aRequestMapper )
    {
    }

// Symbian OS default constructor can leave.
void CCASearchManager::ConstructL()
    {
    CHAT_DP_TXT( "CCASearchManager::ConstructL...starts" );

    iImpsFundClient = iImpsFactory->CreateFundClientL();

    iResults = CCASearchData::NewL();
    iCreatedGroups = new( ELeave ) CDesCArrayFlat( KArrayGranularity );

    CHAT_DP_TXT( "CCASearchManager::ConstructL...ends" );
    }

// ---------------------------------------------------------
// CCASearchManager::SaveResultsL
// ---------------------------------------------------------
//
void CCASearchManager::SaveResultsL( TInt aIndex,
                                     TBool aCompleted,
                                     TImpsSearchResultType aType,
                                     MDesCArray* aResults )
    {
    CHAT_DP_TXT( "CCASearchManager::SaveResultsL...starts" );

    iResults->UpdateDataL(
        aIndex,
        aCompleted,
        aType,
        aResults,
        iSearchLimit );

    CHAT_DP_TXT( "CCASearchManager::SaveResultsL...over" );
    }

//  End of File
