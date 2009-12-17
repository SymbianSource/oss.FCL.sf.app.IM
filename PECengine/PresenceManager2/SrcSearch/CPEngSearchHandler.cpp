/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: this class handls the search operation
*
*/

// INCLUDE FILES
#include "CPEngSearchHandler.h"
#include "CPEngSearchResultEntry.h"
#include "PEngWVSearchPanics.h"
#include "PresenceDebugPrint.h"

#include <ImpsFundamental.h>
#include <CPEngSearchCriteria2.h>
#include <PEngWVPresenceSearch2.h>


//MACROS

#define RETURN_IF_NOT_PENDING( aId ) \
        {                                \
        if( !IsRequestPending( aId ) )   \
            {                            \
            return;                      \
            }                            \
        }                                \
     




// ============================ LOCAL FUNCTIONS ================================

// -----------------------------------------------------------------------------
// PEngElement2WVElementL()
//
//
//
// -----------------------------------------------------------------------------
TImpsSearchElement PEngElement2WVElementL( TInt aElement )
    {
    TImpsSearchElement wvElement( EImpsUserAlias );
    switch ( aElement )
        {
        case EPEngWVSearchUserAlias:
            {
            wvElement = EImpsUserAlias;
            break;
            }

        case EPEngWVSearchUserOnlineStatus:
            {
            wvElement = EImpsUserOnlineStatus;
            break;
            }

        case EPEngWVSearchUserEmailAddress:
            {
            wvElement = EImpsUserEmailAddress;
            break;
            }

        case EPEngWVSearchUserFirstName:
            {
            wvElement = EImpsUserFirstName;
            break;
            }

        case EPEngWVSearchUserID:
            {
            wvElement = EImpsUserID;
            break;
            }

        case EPEngWVSearchUserLastName:
            {
            wvElement = EImpsUserLastName;
            break;
            }

        case EPEngWVSearchUserMobileNumber:
            {
            wvElement = EImpsUserMobileNumber;
            break;
            }

        default:
            {
            SearchPanicOrLeaveL( EPEngSearchElementUnknown, KErrUnknown );
            break;
            }
        }

    return wvElement;
    }



// -----------------------------------------------------------------------------
// PEngCriteria2WVReqLC()
// -----------------------------------------------------------------------------
CImpsSearchRequest* PEngCriteria2WVReqLC( const CPEngSearchCriteria2& aCriteria )
    {
    CImpsSearchRequest* req = CImpsSearchRequest::NewL();
    CleanupStack::PushL( req );

    TImpsSearchElement wvElement = PEngElement2WVElementL( aCriteria.Element() );
    TPtrC searchValue( aCriteria.SearchValue() );

    req->SetRequestL( wvElement, searchValue );
    return req;
    }




// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CPEngSearchHandler::NewL()
// -----------------------------------------------------------------------------
CPEngSearchHandler* CPEngSearchHandler::NewL( TInt aPriority,
                                              RImpsEng& aImpsEngine,
                                              CPEngSearchResultEntry& aResultContainer,
                                              const RPointerArray< CPEngSearchCriteria2 >& aCriterias,
                                              TInt aSearchLimit )
    {
    CPEngSearchHandler* self = new ( ELeave ) CPEngSearchHandler( aResultContainer, aSearchLimit );
    CleanupStack::PushL( self );
    self->ConstructL( aPriority, aImpsEngine, aCriterias );
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngSearchHandler::~CPEngSearchHandler()
// -----------------------------------------------------------------------------
CPEngSearchHandler::~CPEngSearchHandler()
    {
    if ( iSearchId != KErrNotFound )
        {
        TRAP_IGNORE( iFundClient.StopSearchL( iSearchId ) )
        }

    iFundClient.Unregister();

    if ( iSearchPairs )
        {
        iSearchPairs->ResetAndDestroy();
        }

    delete iSearchPairs;


    if ( iRequest )
        {
        User::RequestComplete( iRequest, KErrCancel );
        }
    }


// -----------------------------------------------------------------------------
// CPEngSearchHandler::CPEngSearchHandler()
// -----------------------------------------------------------------------------
CPEngSearchHandler::CPEngSearchHandler( CPEngSearchResultEntry& aResultContainer,
                                        TInt aSearchLimit )
        : iResultContainer( aResultContainer ),
        iSearchLimit( aSearchLimit ),
        iOpId( KErrNotFound )
    {
    }


// -----------------------------------------------------------------------------
// CPEngSearchHandler::ConstructL()
// -----------------------------------------------------------------------------
void CPEngSearchHandler::ConstructL( TInt aPriority,
                                     RImpsEng& aImpsEngine,
                                     const RPointerArray< CPEngSearchCriteria2 >& aCriterias )
    {
    iFundClient.RegisterL( aImpsEngine, this, NULL, KNullDesC, EFalse, aPriority );
    iFundClient.RegisterErrorObserverL( *this );

    iSearchPairs = new ( ELeave ) CSearchPairs( 5 );

    const TInt count = aCriterias.Count();
    for ( TInt ix = 0; ix < count; ix++ )
        {
        CPEngSearchCriteria2* pengCriteria = aCriterias[ ix ];
        CImpsSearchRequest* impsReg = PEngCriteria2WVReqLC( *pengCriteria );
        iSearchPairs->AppendL( impsReg );
        CleanupStack::Pop( impsReg );
        }
    }



// -----------------------------------------------------------------------------
// CPEngSearchHandler::SearchL()
// -----------------------------------------------------------------------------
void CPEngSearchHandler::SearchL( TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iRequest, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( iOpId == KErrNotFound, User::Leave( KErrAlreadyExists ) );

    iOpId = iFundClient.SearchFirstL( *iSearchPairs, iSearchLimit );
    iSearchId = KErrNotFound;

    iRequest = &aStatus;
    *iRequest = KRequestPending;
    }


// -----------------------------------------------------------------------------
// CPEngSearchHandler::ContinueSearchL()
// -----------------------------------------------------------------------------
void CPEngSearchHandler::ContinueSearchL( TInt aContinueIndex,
                                          TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iRequest, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( iOpId == KErrNotFound, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( iSearchId != KErrNotFound, User::Leave( KErrNotReady ) );

    iOpId = iFundClient.SearchNextL( iSearchId, aContinueIndex );

    iRequest = &aStatus;
    *iRequest = KRequestPending;
    }


// -----------------------------------------------------------------------------
// CPEngSearchHandler::StopSearchL()
// -----------------------------------------------------------------------------
void CPEngSearchHandler::StopSearchL( TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iRequest, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( iOpId == KErrNotFound, User::Leave( KErrAlreadyExists ) );

    if ( iSearchId != KErrNotFound )
        {
        iOpId = iFundClient.StopSearchL( iSearchId );

        iRequest = &aStatus;
        *iRequest = KRequestPending;
        }
    else
        {
        //If no WV engine search pending - complete stop directly without errors
        TRequestStatus* tmpStatus = &aStatus;
        User::RequestComplete( tmpStatus, KErrNone );
        }
    }



// -----------------------------------------------------------------------------
// CPEngSearchHandler::IsRequestPending()
// -----------------------------------------------------------------------------
TBool CPEngSearchHandler::IsRequestPending( TInt aId )
    {
    if ( ( aId == iOpId ) && iRequest )
        {
        return ETrue;
        }

    return EFalse;
    }



// -----------------------------------------------------------------------------
// CPEngSearchHandler::HandleSearchL()
// From MImpsSearchHandler2
// -----------------------------------------------------------------------------
void CPEngSearchHandler::HandleSearchL( TInt aOpId,
                                        TInt aSearchId,
                                        TInt aIndex,
                                        TInt aResultsFound,
                                        TBool aCompleted,
                                        TImpsSearchResultType aType,
                                        MDesCArray* aResults ,
                                        TImpsCspIdentifier& /*aCspId*/ )
    {
    PENG_DP( D_PENG_LIT( "CPEngSearchHandler::HandleSearchL( aOpId(%d), aSearchId(%d), aIndex(%d), aResultsFound(%d), aCompleted(%d), aType(%i), results(%d) )" ),
             aOpId, aSearchId, aIndex, aResultsFound, aCompleted, aType, aResults ? aResults->MdcaCount() : 0 );

    RETURN_IF_NOT_PENDING( aOpId );

    //Init operation & search ID's
    iOpId = KErrNotFound;
    if ( iSearchId == KErrNotFound )
        {
        iSearchId = aSearchId;
        }

    //Fill result container
    iResultContainer.Reset();
    iResultContainer.SetDetails( aResultsFound, aIndex, aCompleted );

    TInt err = KErrNone;
    if ( ( aType == EImpsUserResult ) && aResults )
        {
        TRAP( err,
            {
            const TInt resultCount = aResults->MdcaCount();
            for ( TInt ix = 0; ix < resultCount; ix++ )
                {
                iResultContainer.UserResults().AppendL( aResults->MdcaPoint( ix ) );
                }
            } );
        }


    User::RequestComplete( iRequest, err );
    }


// -----------------------------------------------------------------------------
// CPEngSearchHandler::HandleSearchStoppedL()
// From MImpsSearchHandler2
// -----------------------------------------------------------------------------
void CPEngSearchHandler::HandleSearchStoppedL( TInt aOpId,
                                               TInt aSearchId,
                                               TImpsCspIdentifier& /*aCspId*/ )
    {

    PENG_DP( D_PENG_LIT( "CPEngSearchHandler::HandleSearchStoppedL( aOpId(%d), aStatus(%d), aSearchId(%d) )" ),
             aOpId, aSearchId );

    RETURN_IF_NOT_PENDING( aOpId );

    iOpId = KErrNotFound;
    iSearchId = KErrNotFound;
    iResultContainer.Reset();
    User::RequestComplete( iRequest, KErrNone );
    }


// -----------------------------------------------------------------------------
// CPEngSearchHandler::HandleErrorL()
// From MImpsErrorHandler2
// -----------------------------------------------------------------------------
void CPEngSearchHandler::HandleErrorL( TInt aStatus,
                                       TInt aOpId,
                                       const TDesC* /*aDescription*/,
                                       const CImpsDetailed* /*aDetailedRes*/,
                                       TImpsCspIdentifier& /*aCspId*/ )
    {
    PENG_DP( D_PENG_LIT( "CPEngSearchHandler::HandleErrorL( aStatus(%i), aOpId(%i) )" ), aStatus, aOpId );

    RETURN_IF_NOT_PENDING( aOpId );

    iOpId = KErrNotFound;
    iSearchId = KErrNotFound;

    iResultContainer.Reset();
    User::RequestComplete( iRequest, aStatus );
    }



// End of file


