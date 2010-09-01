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
* Description:  Blocking manager handles the blocking and granting
 *
*/


// INCLUDES

#include "CCABlockingManager.h"
#include "CCARequest.h"
#include "CCARequestMapper.h"
#include "PrivateEngineDefinitions.h"
#include "ChatDebugPrint.h"
#include "CCAStorageManagerFactory.h"
#include "MCASettings.h"
#include "MCAStoredContact.h"
#include "MCAStoredContacts.h"
#include "MCAImpsFactory.h"

#include "impsbuilddefinitions.h"

#include "TStorageManagerGlobals.h"
#include "CAUtils.h"

#include "MCAPresence.h"
#include "MCAContactLists.h"
#include "CAPresenceManager.h"

#include "ccasyncchecker.h"
#include "mcacontactlist.h"

#include <badesca.h>

const TInt KBlockingMgrListGranularity = 10;


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//

CCABlockingManager::CCABlockingManager(
    MCAImpsFactory* aIMPSFactory,
    CCARequestMapper* aRequestMapper,
    MCASettings* aSettingsManager )
        :   iImpsFactory( aIMPSFactory ),
        iRequestMapper( aRequestMapper ),
        iSettingsManager( aSettingsManager ),
        iStrategy( MCASettings::EAll ),
        iMgrState( EStateNormal )
    {
    }

// Destructor
EXPORT_C CCABlockingManager::~CCABlockingManager()
    {
    if ( iBlockedList )
        {
        iBlockedList->Reset();
        delete iBlockedList;
        }

    if ( iGrantedList )
        {
        iGrantedList->Reset();
        delete iGrantedList;
        }
    }

// Two-phased constructor.
EXPORT_C CCABlockingManager* CCABlockingManager::NewL(
    MCAImpsFactory* aIMPSFactory,
    CCARequestMapper* aRequestMapper,
    MCASettings* aSettingsManager )
    {
    CCABlockingManager* self = new ( ELeave ) CCABlockingManager(
        aIMPSFactory,
        aRequestMapper,
        aSettingsManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }



// Symbian OS default constructor can leave.
void CCABlockingManager::ConstructL()
    {
    iStoredContacts = CCAStorageManagerFactory::ContactListInterfaceL();

    iBlockedList = new ( ELeave ) CDesCArrayFlat( KBlockingMgrListGranularity );
    iGrantedList = new ( ELeave ) CDesCArrayFlat( KBlockingMgrListGranularity );

    // after this the strategy gets updated via observing mechanism
    iStrategy = ( MCASettings::TCASettingValues )
                iSettingsManager->Value( MCASettings::EReceiveIMessages );
    }

// ---------------------------------------------------------
// CCABlockingManager::GetBlockedListRequestL()
// GetBlocked list
// ---------------------------------------------------------
//
TInt CCABlockingManager::GetBlockedListFromServerL()
    {
    CHAT_DP_TXT( "CCABlockingManager::GetBlockedListFromServerL...starts" );

    iGetBlockedListOpId =  iImpsFactory->CreateImClientL()->GetBlockedListRequestL();

    // owned by request mapper
    CCARequest* request = iRequestMapper->CreateRequestL( iGetBlockedListOpId );    // CSI: 35 # Ownership is not transferred to caller.

    CHAT_DP_FUNC_DP( "GetBlockedListResponseL", "Going to HandleRequest" );
    iRequestMapper->HandleRequest( iGetBlockedListOpId, KErrNone );

    // set to 0 so we know that there is no request hanging.
    iGetBlockedListOpId = 0;

    TInt errorCode( request->ErrorCode() );
    iRequestMapper->RemoveRequest( request );

    if ( iMgrState == EStateNormal )
        {
        if ( iStrategy != MCASettings::EAll && iCanUseGrant )
            {
            CHAT_DP_FUNC_DP( "GetBlockedListResponseL",
                             "Copying grantlist" );
            switch ( iStrategy )
                {
                case MCASettings::EFriends:
                    {
#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                    if( iAppUiExit )
                    	{
                    	return errorCode;
                    	}
                    // Can't grant friends if contact list
                    // sync is not done, wait for it if needed
                    if ( !CCASyncChecker::CheckSyncStateL() )
                        {
                        User::Leave( KErrNotReady );
                        }
#endif // IMPS_CONTACT_FETCH_BACKGROUND
                    CDesCArray* clone = CloneArrayLC( *iGrantedList );
                    GrantFriendsL();
                    // Check new ids to be granted
                    CDesCArray* diff = CreateNeutralDiffLC( *iGrantedList,
                                                            *clone );
                    // Check if clone contains more ids than we have friends
                    CDesCArray* diff2 = CreateNeutralDiffLC( *clone,
                                                             *iGrantedList );
                    iGrantedList->Reset();
                    // Remove grant from excess ids
                    RemoveL( NULL, diff2, ETrue );
                    // Add grant for new ids
                    InsertL( NULL, diff, ETrue );
                    GrantFriendsL(); // reset situation
                    CleanupStack::PopAndDestroy( 3, clone ); // clone, diff, diff2
                    break;
                    }
                case MCASettings::ENobody:
                    {
                    CDesCArray* clone = CloneArrayLC( *iGrantedList );
                    RemoveL( NULL, clone, ETrue );
                    CleanupStack::PopAndDestroy( clone );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            }
        else if ( !iCanUseGrant )
            // This is done to get rid of grant list usage.
            {
            //if iGrantedList is empty, nothing to clean
            if ( iGrantedList && iGrantedList->MdcaCount() > 0 )
                {
                CDesCArray* clone = CloneArrayLC( *iGrantedList );
                RemoveL( NULL, clone, ETrue );
                CleanupStack::PopAndDestroy( clone );
                }
            }
        else if ( iCanUseGrant && iStrategy == MCASettings::EAll )
            {
            if( iAppUiExit )
            	{
            	return errorCode;
            	}
            InsertL( iBlockedList, NULL, ETrue );
            }
        }

    CHAT_DP_TXT( "CCABlockingManager::GetBlockedListFromServerL...over" );

    return errorCode;
    }

// ---------------------------------------------------------
// CCABlockingManager::BlockedList()
// ---------------------------------------------------------
//
const CDesCArray* CCABlockingManager::BlockedList()
    {
    return iBlockedList;
    }

// check if grant list in use according to the
// currently chosen strategy
TBool CCABlockingManager::GrantListInUse()
    {
    CHAT_DP_FUNC_ENTER( "GrantListInUse" );

    // grant list is always in use if we're not
    // accepting IMessages from all
    TBool retval( ETrue );

    if ( iStrategy == MCASettings::EAll || !iCanUseGrant )
        {
        // grant list not in use, everyone is accepted (except
        // that the blocking list overrides)
        retval = EFalse;
        CHAT_DP_TXT( "Grant list not in use!" );
        }

    CHAT_DP_FUNC_DONE( "GrantListInUse" );

    return retval;
    }

// ---------------------------------------------------------
// CCABlockingManager::InsertL()
// Insert users to block or grant lists at server
// ---------------------------------------------------------
//
TInt CCABlockingManager::InsertL(
    const CDesCArray* aBlockList,
    const CDesCArray* aGrantList,
    TBool aForceUpdate /* = EFalse */ )
    {
    if ( aGrantList )
        {
        CDesCArray* diff = CreateNeutralDiffLC( *aGrantList, *iGrantedList );

        TInt err( KErrNone );
        if ( NeedUpdate( aBlockList, diff, aForceUpdate ) && !iUpdateInProgress )
            {
            err = DoBlockRequestL( aBlockList, NULL, diff, NULL, aBlockList,
                                   EBlockRequest );
            if ( err == KErrNone )
                {
                AddToGrantedListL( *aGrantList );
                }
            }
        CleanupStack::PopAndDestroy( diff );

        return err;
        }
    else if ( NeedUpdate( aBlockList, aGrantList, aForceUpdate ) )
        {
        return DoBlockRequestL( aBlockList, NULL, aGrantList, NULL, aBlockList,
                                EBlockRequest );
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// CCABlockingManager::RemoveL()
// Remove users from block or grant lists at server
// ---------------------------------------------------------
//
TInt CCABlockingManager::RemoveL(
    const CDesCArray* aBlockList,
    const CDesCArray* aGrantList,
    TBool aForceUpdate /* = EFalse */ )
    {
    if ( aGrantList )
        {
        CDesCArray* clone = CloneArrayLC( *aGrantList );
        RemoveFromGrantedList( *clone );
        TInt err( KErrNone );
        if ( NeedUpdate( aBlockList, clone, aForceUpdate )
             && !iUpdateInProgress )
            {
            err = DoBlockRequestL( NULL, aBlockList, NULL, clone, aBlockList,
                                   EUnBlockRequest );
            }
        CleanupStack::PopAndDestroy( clone );
        return err;
        }

    if ( NeedUpdate( aBlockList, aGrantList, aForceUpdate ) )
        {
        return DoBlockRequestL( NULL, aBlockList, NULL, aGrantList, aBlockList,
                                EUnBlockRequest );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCABlockingManager::NeedUpdate
// If update is needed, return True
// -----------------------------------------------------------------------------
//
TBool CCABlockingManager::NeedUpdate( const CDesCArray* aBlockList,
                                      const CDesCArray* aGrantList,
                                      TBool aForceUpdate )
    {
    if ( !aForceUpdate )
        {
        if ( aGrantList )
            {
            if ( aGrantList->MdcaCount() == 0 )
                {
                return EFalse;
                }
            }
        else if ( aBlockList )
            {
            if ( aBlockList->MdcaCount() == 0 )
                {
                return EFalse;
                }
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CCABlockingManager::GrantFriendsL()
// Insert friends to grant lists at server
// ---------------------------------------------------------
//
void CCABlockingManager::GrantFriendsL(
    CDesCArray* aDoNotIncludeList /*= NULL*/ )
    {
    if( iAppUiExit )
    	{
    	return ;
    	}
    // add all friends to Grant list
    // the blocked list overrides this list anyway
    iGrantedList->Reset();

    // fetch the contacts from all lists in server
    CPtrCArray* wvids = new( ELeave )CPtrCArray( KArrayGranularity );
    CleanupStack::PushL( wvids );

    MCAPresence* presence = CAPresenceManager::InstanceL();
    MCAContactLists* listInterface = presence->ContactLists();
    TContactListsWrapper contactListIds;

    if ( listInterface )
        {
        contactListIds = listInterface->FetchServerContactListsL();
        }
    TInt count( contactListIds.MdcaCount() );
    TInt i;
    for ( i = 0; i < count; ++i )
        {
        listInterface->FetchServerContactsL( contactListIds.MdcaPoint( i ),
                                             *wvids );
        }
    count = wvids->MdcaCount();
    TBool doNotIncludeFlag( EFalse );
    for ( i = 0; i < count; ++i )
        {
        TPtrC id( wvids->MdcaPoint( i ) );
        if ( aDoNotIncludeList )
            {
            TInt count2( aDoNotIncludeList->MdcaCount() );
            for ( TInt j( 0 ); j < count2 && !doNotIncludeFlag; ++j )
                {
                if ( CAUtils::NeutralCompare( id,
                                              aDoNotIncludeList->MdcaPoint( j ) ) == 0 )
                    {
                    doNotIncludeFlag = ETrue;
                    aDoNotIncludeList->Delete( j );
                    }
                }
            }
        if ( !doNotIncludeFlag )
            {
            iGrantedList->AppendL( wvids->MdcaPoint( i ) );
            }
        else
            {
            doNotIncludeFlag = EFalse;
            }
        }

    CleanupStack::PopAndDestroy( wvids ); // wvids
    }

// ---------------------------------------------------------
// CCABlockingManager::DoBlockRequestL()
// Execute block entity request with given arguments.
// ---------------------------------------------------------
//
TInt CCABlockingManager::DoBlockRequestL( const CDesCArray* aInsertBlockList,
                                          const CDesCArray* aRemoveBlockList,
                                          const CDesCArray* aInsertGrantList,
                                          const CDesCArray* aRemoveGrantList,
                                          const CDesCArray* aAdditionalData,
                                          TOperationRequests aRequest )
    {
    // check argument validity
    if ( aRequest != EBlockRequest && aRequest != EUnBlockRequest )
        {
        User::Leave( KErrArgument );
        }

    TBool grantUsed = GrantListInUse();

    // Create the request but do not start the synchronous wait yet ( EFalse )
    CCARequest* request = iRequestMapper->CreateRequestL(
                              iImpsFactory->CreateImClientL()->BlockEntityRequestL(
                                  aInsertBlockList,
                                  aRemoveBlockList,
                                  ETrue,
                                  aInsertGrantList,
                                  aRemoveGrantList,
                                  grantUsed ),
                              EFalse );

    request->SetRequestType( aRequest );

    // empty array, so granularity of one
    CDesCArrayFlat* emptyArray = new( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( emptyArray );

    // set correct array based on request to additional data
    const CDesCArray *tempArrayPtr = NULL;
    tempArrayPtr = emptyArray;

    if ( aAdditionalData )
        {
        tempArrayPtr = aAdditionalData;
        }

    request->SetAdditionalData( *tempArrayPtr );

    // Now when the request type and additional data is set we can
    // activate the synchronous wait
    request->StartWait();

    // Execution continues from here
    TInt errorCode( request->ErrorCode() );

    CHAT_DP( D_CHAT_LIT( "   Error code in DoBlockRequestL is %d" ), errorCode );

    iRequestMapper->RemoveRequest( request );
    CleanupStack::PopAndDestroy( emptyArray );

    return errorCode;

    }

// ---------------------------------------------------------
// CCABlockingManager::IsContactBlocked
// ---------------------------------------------------------
//
TBool CCABlockingManager::IsContactBlocked(
    const TDesC& aContact,
    TBool aUseGrant /*= EFalse*/ ) const
    {
    CHAT_DP_FUNC_ENTER( "IsContactBlocked" );
    CHAT_DP( D_CHAT_LIT(
                 "CCABlockingManager::IsContactBlocked - Checking contact %S" ),
             &aContact );

    MCAStoredContact* contact = iStoredContacts->FindAnyContact( aContact );

    // default to allowing the contact if it gets through
    // the switch-case
    TBool retVal( EFalse );

    MCASettings::TCASettingValues strategy =
        iCanUseGrant ? iStrategy : MCASettings::EAll;
    switch ( strategy )
        {
        case MCASettings::EAll:
            CHAT_DP_FUNC_DP( "IsContactBlocked", "Strategy is Grant all" );
                {
                // grant list not in use, allow all
                // but note the blocklist
                if ( !contact )
                    {
                    CHAT_DP_FUNC_DP( "IsContactBlocked", "Contact is not a friend" );
                    retVal = InternallyBlocked( aContact );
                    }
                else
                    {
                    CHAT_DP_FUNC_DP( "IsContactBlocked", "Contact is a friend" );
                    retVal = contact->IsBlocked();
                    }
                break;
                }

        case MCASettings::EFriends:
            {
            CHAT_DP_FUNC_DP( "IsContactBlocked", "Strategy is Grant friends" );
            // friends are granted according to block status
            if ( contact )
                {
                CHAT_DP_FUNC_DP( "IsContactBlocked", "Contact is a friend" );
                retVal = contact->IsBlocked();
                }
            else
                {
                if ( aUseGrant )
                    {
                    // Check grantlist, because there might be some
                    // contacts in grant list.
                    CHAT_DP_FUNC_DP( "IsContactBlocked",
                                     "Contact is not friend but might in grant list" );
                    // Other than friends could be in grant list
                    // because temporary grant.
                    retVal = !IsContactGranted( aContact );
                    }
                else
                    {
                    retVal = InternallyBlocked( aContact );
                    }
                }
            break;
            }
        case MCASettings::ENobody:
            {
            CHAT_DP_FUNC_DP( "IsContactBlocked", "Strategy is Grant none, but \
                         someone could be in grant list." );
            // Some one could be in grant list because temporary grant.
            if ( aUseGrant )
                {
                retVal = !IsContactGranted( aContact );
                }
            else
                {
                retVal = InternallyBlocked( aContact );
                }
            break;
            }

        default:
            {
            // bogus value!
            CHAT_DP_FUNC_DP( "IsContactBlocked",
                             "Bogus grant strategy, defaulting to allow all" );
            retVal = EFalse;
            break;
            }
        }

    CHAT_DP_FUNC_DONE( "IsContactBlocked" );
    return retVal;
    }

// ---------------------------------------------------------
// CCABlockingManager::ContactListChangedL
// ---------------------------------------------------------
//
void CCABlockingManager::ContactListChangeL( TBool aUpdateInProgress )
    {
    iUpdateInProgress = aUpdateInProgress;

    if ( iUpdateInProgress )
        {
        return;
        }

    TInt count( iBlockedList->MdcaCount() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        iStoredContacts->SetBlocked( iBlockedList->MdcaPoint( a ), ETrue );
        }
    }

// ---------------------------------------------------------
// CCABlockingManager::HandleCompleteL
// ---------------------------------------------------------
//
void CCABlockingManager::HandleCompleteL(
    TInt aOperationId,
    TImpsCspIdentifier& /* aCspId */ )
    {
    TInt status( KErrNone );

    TRAPD( err, DoHandleCompleteL( aOperationId, status ) );
    if ( err != KErrNone )
        {
        status = err;
        }
    iRequestMapper->HandleRequest( aOperationId, status );

    User::LeaveIfError( err );
    }

// ---------------------------------------------------------
// CCABlockingManager::DoHandleCompleteL
// ---------------------------------------------------------
//
void CCABlockingManager::DoHandleCompleteL(
    TInt aOperationId,
    TInt aStatus )
    {
    CHAT_DP_TXT( "CCABlockingManager::HandleCompleteL...starts" );
    CHAT_DP( D_CHAT_LIT( "   aStatus is %d" ), aStatus );

    // Find the request
    const CCARequest* request = iRequestMapper->FindRequest( aOperationId );

    if ( request )
        {
        // check if this was block request or unblock request
        TBool blockRequest = ETrue;
        if ( request->RequestType() == EUnBlockRequest )
            {
            blockRequest = EFalse;
            }
        // set requested contacts as blocked
        TInt itemInArray( request->AdditionalData()->MdcaCount() );

        for ( TInt i( 0 ); i < itemInArray; i++ )
            {
            TPtrC userId =
                TPtrC( request->AdditionalData()->MdcaPoint( i ) );

            // block and save state in persistent storage
            iStoredContacts->SetBlocked( userId, blockRequest );

            if ( blockRequest )
                {
                AddToBlockListL( userId );
                }
            else
                {
                RemoveFromBlockList( userId );
                }
            }
        }

    CHAT_DP_TXT( "CCABlockingManager::HandleCompleteL...over" );
    }



// ---------------------------------------------------------
// CCABlockingManager::GetBlockedListResponseL
// ---------------------------------------------------------
//
void CCABlockingManager::GetBlockedListResponseL(
    const MDesCArray* aBlockedList,
    TBool aBlockedListInUse,
    const MDesCArray* aGrantedList,
    TBool aGrantedListInUse,
    TImpsCspIdentifier& /* aCspId */ )
    {
    TRAPD( err, DoGetBlockedListResponseL( aBlockedList, aBlockedListInUse,
                                           aGrantedList, aGrantedListInUse ) );

    if ( iGetBlockedListOpId != 0 )
        {
        CHAT_DP_FUNC_DP( "GetBlockedListResponseL", "Going to HandleRequest" ) ;
        iRequestMapper->HandleRequest( iGetBlockedListOpId, err );
        iGetBlockedListOpId = 0;
        }

    User::LeaveIfError( err );
    }

// ---------------------------------------------------------
// CCABlockingManager::DoGetBlockedListResponseL
// ---------------------------------------------------------
//
void CCABlockingManager::DoGetBlockedListResponseL(
    const MDesCArray* aBlockedList,
    TBool /* aBlockedListInUse */,
    const MDesCArray* aGrantedList,
    TBool /* aGrantedListInUse */ )
    {
    CHAT_DP_FUNC_ENTER( "GetBlockedListResponseL" );

    switch ( iMgrState )
        {
        case EStateNormal :
            {
            CHAT_DP_FUNC_DP( "GetBlockedListResponseL", "EStateNormal" );

            if ( aBlockedList )
                {
                // nuke the internal list
                iBlockedList->Reset();
                if ( aBlockedList->MdcaCount() > 0 )
                    {
                    // now block the selected contacts, if there are any
                    CHAT_DP_FUNC_DP( "GetBlockedListResponseL",
                                     "Updating blocking status" );

                    TInt count( aBlockedList->MdcaCount() );
                    CHAT_DP( D_CHAT_LIT(
                                 "CCABlockingManager::GetBlockedListResponseL \
                    - Blocked count %d" ), count );

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                    TBool needToObserve = EFalse;
                    TBool syncReady = EFalse;
#endif // IMPS_CONTACT_FETCH_BACKGROUND                    
                    for ( TInt i( 0 ); i < count; ++i )
                        {
                        TPtrC userId = TPtrC( aBlockedList->MdcaPoint( i ) );

                        CHAT_DP( D_CHAT_LIT(
                                     "CCABlockingManager::GetBlockedListResponseL \
                        - Contact %S" ), &userId );

                        // it's blocked, so remember it
                        AddToBlockListL( userId );

#ifndef IMPS_CONTACT_FETCH_BACKGROUND
                        iStoredContacts->SetBlocked( userId, ETrue );
#else
                        // Contact list sync state must be checked.
                        // If list sync is not finished mark blocked ids
                        // while sync progresses.
                        if ( !needToObserve )
                            {
                            MCAContactLists* listInterface =
                                CAPresenceManager::InstanceL()->ContactLists();
                            TBool syncDone = EFalse;
                            if ( listInterface )
                                {
                                syncDone = listInterface->IsSyncDone();
                                }
                            TBool ignoreFailed = ETrue;
                            if ( !iStoredContacts->IsAllSynchronised( ignoreFailed )
                                 || !syncDone )
                                {
                                // Sync not done, observe it through stored
                                // contacts interface
                                needToObserve = ETrue;
                                }
                            else
                                {
                                // Sync done
                                syncReady = ETrue;
                                }
                            }
                        if ( syncReady )
                            {
                            // Sync done, ids can be blocked directly
                            iStoredContacts->SetBlocked( userId, ETrue );
                            }
#endif // IMPS_CONTACT_FETCH_BACKGROUND
                        }

#ifdef IMPS_CONTACT_FETCH_BACKGROUND
                    if ( needToObserve )
                        {
                        // Observe fetching.
                        // Blocked ids are marked in HandleChange method.
                        iStoredContacts->AddObserverL( this );
                        }
#endif // IMPS_CONTACT_FETCH_BACKGROUND                    
                    }
                }
            iGrantedList->Reset();
            if ( aGrantedList )
                {
                // copy the granted list
                CHAT_DP_FUNC_DP( "GetBlockedListResponseL",
                                 "Copying grantlist" );
                const TInt count( aGrantedList->MdcaCount() );
                for ( TInt i = 0; i < count; i++ )
                    {
                    iGrantedList->AppendL( aGrantedList->MdcaPoint( i ) );
                    }
                }
            break;
            }

        case EStateDeletingGrantList :
            {
            CHAT_DP_FUNC_DP( "GetBlockedListResponseL",
                             "EStateDeletingGrantList" );
            // copy the returned grant list into an internal structure
            iGrantedList->Reset();
            if ( aGrantedList )
                {
                // copy the granted list
                CHAT_DP_FUNC_DP( "GetBlockedListResponseL",
                                 "Copying grantlist" );
                const TInt count( aGrantedList->MdcaCount() );
                for ( TInt i = 0; i < count; i++ )
                    {
                    iGrantedList->AppendL( aGrantedList->MdcaPoint( i ) );
                    }
                }
            else
                {
                // we should have gotten it
                CHAT_DP_FUNC_DP( "GetBlockedListResponseL",
                                 "Grantlist was NULL, assuming it's empty" );
                }
            break;
            }

        default:
            {
            CHAT_DP_FUNC_DP( "GetBlockedListResponseL", "Invalid state" );
            break;
            }
        }

    CHAT_DP_FUNC_DONE( "GetBlockedListResponseL" );
    }

// clear the grant list
void CCABlockingManager::ClearGrantListL( TBool aForceUpdate /* = EFalse */ )
    {
    // fetch the lists
    iMgrState = EStateDeletingGrantList;
    GetBlockedListFromServerL();

    // ungrant all of the returned grant list people,
    // effectively clearing the grant list
    CDesCArray* clone = CloneArrayLC( *iGrantedList );
    iGrantedList->Reset();
    RemoveL( NULL, clone, aForceUpdate );
    CleanupStack::PopAndDestroy( clone );

    // RemoveL return value ignored, because if remove fails
    // engine gets callback HandleErrorL, if removeL is succeed
    // CCABlockingManager gets callback HandleCompleteL

    // that's it grant list is cleared
    iMgrState = EStateNormal;
    }

// ---------------------------------------------------------
// CCABlockingManager::HandleSettingsChangeL
// ---------------------------------------------------------
//
void CCABlockingManager::HandleSettingsChangeL( TInt aChangedSettingEnum )
    {
    CHAT_DP_FUNC_ENTER( "HandleSettingsChangeL" );

    if ( aChangedSettingEnum != MCASettings::EReceiveIMessages )
        {
        // it's not for us
        CHAT_DP_FUNC_DP( "HandleSettingsChangeL",
                         "Got event but it was not for me" );
        return;
        }

    MCASettings::TCASettingValues newValue = ( MCASettings::TCASettingValues )
                                             iSettingsManager->Value( MCASettings::EReceiveIMessages );

    CHAT_DP( D_CHAT_LIT( "CCABlockingManager::HandleSettingsChangeL - \
                        Current %d, new %d" ), iStrategy, newValue );   // CSI: 62 # Not valid, new inside debug print.

    if ( newValue == iStrategy )
        {
        CHAT_DP_FUNC_DP( "HandleSettingsChangeL",
                         "Unchanged setting state for grantlist strategy" );
        return;
        }

    CHAT_DP( D_CHAT_LIT( "CCABlockingManager::HandleSettingsChangeL - \
                        BackGroundTaskReady %d" ), iBackgroundTaskReady );
    if ( !iBackgroundTaskReady ) // haven't runned the backgroundtask yet.
        {
        return;
        }

    iStrategy = newValue;
    if ( !iLoggedIn )
        {
        // not logged in -> return
        return;
        }

    // the changed settings effects us
    MCASettings::TCASettingValues strategy =
        iCanUseGrant ? newValue : MCASettings::EAll;

    switch ( strategy )
        {
        case MCASettings::EAll :
            {
            CHAT_DP_FUNC_DP( "HandleSettingsChangeL", "Blocklist only" );
            // only blocklist active. it should be up-to-date.
            InsertL( iBlockedList, NULL, ETrue );
            break;
            }

        case MCASettings::EFriends :
            {
            CHAT_DP_FUNC_DP( "HandleSettingsChangeL",
                             "Blocklist with grantlist" );
            // blocklist with grantlist
            CDesCArray* clone = CloneArrayLC( *iGrantedList );
            GrantFriendsL();
            // Check new ids to be granted
            CDesCArray* diff = CreateNeutralDiffLC( *iGrantedList,
                                                    *clone );
            // Check if clone contains more ids than we have friends
            CDesCArray* diff2 = CreateNeutralDiffLC( *clone,
                                                     *iGrantedList );
            iGrantedList->Reset();
            // Remove grant from excess ids
            RemoveL( NULL, diff2, ETrue );
            // Add grant for new ids
            InsertL( NULL, diff, ETrue );
            CleanupStack::PopAndDestroy( 3, clone ); // clone, diff, diff2
            break;
            }

        case MCASettings::ENobody :
            {
            CHAT_DP_FUNC_DP( "HandleSettingsChangeL", "Granting nobody" );
            // grantlist empty == decline all messages

            // empty the grant list
            ClearGrantListL( ETrue );
            break;
            }

        default :
            {
            // bogus
            break;
            }
        }

    CHAT_DP_FUNC_DONE( "HandleSettingsChangeL" );
    }

// ---------------------------------------------------------
// CCABlockingManager::AddToBlockListL
// add to internal blocking list
// ---------------------------------------------------------
//
void CCABlockingManager::AddToBlockListL( const TDesC& aId )
    {
    CHAT_DP_FUNC_ENTER( "AddToBlockListL" );

    TInt position( KErrNotFound );
    if ( iBlockedList->Find( aId, position, ECmpCollated ) == 0 )
        {
        // found it, don't add a duplicate
        CHAT_DP_FUNC_DP( "AddToBlockListL", "Contact already in blocked list" );
        return;
        }

    // didn't find it, so add it
    iBlockedList->AppendL( aId );

    CHAT_DP_FUNC_DONE( "AddToBlockListL" );
    }


// ---------------------------------------------------------
// CCABlockingManager::RemoveFromBlockList
// remove from internal blocking list
// ---------------------------------------------------------
//
void CCABlockingManager::RemoveFromBlockList( const TDesC& aId )
    {
    CHAT_DP_FUNC_ENTER( "RemoveFromBlockListL" );

#ifdef _DEBUG
    TPtrC myDebug( aId );
    CHAT_DP( D_CHAT_LIT( "CCABlockingManager::RemoveFromBlockListL\
	                      looking for %S" ), &myDebug );
#endif

    TInt count( iBlockedList->MdcaCount() );
    for ( TInt a( 0 ); a < count; ++a )
        {
#ifdef _DEBUG
        myDebug.Set( iBlockedList->MdcaPoint( a ) );
        CHAT_DP( D_CHAT_LIT( "CCABlockingManager::RemoveFromBlockListL \
		                     we have %S" ), &myDebug );
#endif
        if ( 0 == CAUtils::NeutralCompare( iBlockedList->MdcaPoint( a ), aId ) )
            {
            // found it, remove it
            CHAT_DP_FUNC_DP( "RemoveFromBlockListL",
                             "Found internally blocked contact" );
            iBlockedList->Delete( a );
            iBlockedList->Compress();
            CHAT_DP_FUNC_DONE( "RemoveFromBlockListL" );
            return;
            }
        }
    CHAT_DP_FUNC_DONE( "RemoveFromBlockListL" );
    }


// ---------------------------------------------------------
// CCABlockingManager::InternallyBlocked
// see if contact is internally blocked or not
// ---------------------------------------------------------
//
TBool CCABlockingManager::InternallyBlocked( const TDesC& aId ) const
    {
    CHAT_DP_FUNC_ENTER( "InternallyBlocked" );

#ifdef _DEBUG
    TPtrC myDebug( aId );
    CHAT_DP( D_CHAT_LIT( "CCABlockingManager::InternallyBlocked \
	                      looking for %S" ), &myDebug );
#endif

    TInt count( iBlockedList->MdcaCount() );
    for ( TInt a( 0 ); a < count; ++a )
        {
#ifdef _DEBUG
        myDebug.Set( iBlockedList->MdcaPoint( a ) );
        CHAT_DP( D_CHAT_LIT( "CCABlockingManager::InternallyBlocked we have %S" ),
                 &myDebug );
#endif
        if ( 0 == CAUtils::NeutralCompare( iBlockedList->MdcaPoint( a ), aId ) )
            {
            CHAT_DP_FUNC_DONE( "InternallyBlocked" );
            return ETrue;
            }
        }

    CHAT_DP_FUNC_DONE( "InternallyBlocked" );
    return EFalse;
    }


// ---------------------------------------------------------
// CCABlockingManager::HandleNetworkStateChange
// ---------------------------------------------------------
//
void CCABlockingManager::HandleNetworkStateChange( TNetworkState aState,
                                                   TBool aCanUseGrant )
    {
    CHAT_DP_FUNC_ENTER( "HandleNetworkState" );
    CHAT_DP( D_CHAT_LIT( "New state is: %d" ), aState );

    iCanUseGrant = aCanUseGrant;

    if ( aState == ELoggedIn )
        {
        iLoggedIn = ETrue;
        //just make sure its not started yet
        if ( iRequestMapper->BackgroundTaskStatus(
                 MCABackgroundInterface::EBlockGrantListFetch ) &
             ( MCABackgroundInterface::EUnknown |
               MCABackgroundInterface::ECompleted |
               MCABackgroundInterface::ECancelled |
               MCABackgroundInterface::EFailed
             ) )
            {
            CHAT_DP_TXT( "CCABlockingManager::HandleNetworkStateChange \
			              registering block list fetch" );

#ifdef RD_SETTINGS_FACELIFT
            // Make sure we have the correct settings value.
            // With settings facelift enabled it is possible to
            // edit settings of any server. If user edits settings
            // for some other server than the default server, notification
            // is not observed here -> getting value here is needed.
            iStrategy = ( MCASettings::TCASettingValues )
                        iSettingsManager->Value( MCASettings::EReceiveIMessages );
#endif

            iRequestMapper->RegisterBackgroundTask(
                this, MCABackgroundInterface::EBlockGrantListFetch );
            iBackgroundTaskReady = EFalse;
            }
        }
    if ( aState == ELoggedOut )
        {
        iLoggedIn = EFalse;
        }
    CHAT_DP_FUNC_DONE( "HandleNetworkState" );
    }


// ---------------------------------------------------------
// CCABlockingManager::HandleBackgroundTaskL
// ---------------------------------------------------------
//
void CCABlockingManager::HandleBackgroundTaskL( TInt aSubTask )
    {
    CHAT_DP_FUNC_ENTER( "HandleBackgroundTaskL" );
    CHAT_DP( D_CHAT_LIT( "CCABlockingManager::HandleBackgroundTaskL \
	                      Subtask is: %d" ), aSubTask );


    TInt error = GetBlockedListFromServerL();

    iBackgroundTaskReady = ETrue;

    CHAT_DP_FUNC_DONE( "HandleBackgroundTaskL" );

    if ( error )
        {
        User::Leave( error );
        }

    }
// ---------------------------------------------------------
// CCABlockingManager::HandleContactDelete
// ---------------------------------------------------------
//
void CCABlockingManager::HandleContactDelete( const TDesC& /*aContactId*/ )
    {
    // Not used
    }

// ---------------------------------------------------------
// CCABlockingManager::HandleAddition
// ---------------------------------------------------------
//
void CCABlockingManager::HandleAddition( MCAContactList& /*aList*/,
                                         MCAStoredContact& /*aContact*/ )
    {
    // Not used
    }

// ---------------------------------------------------------
// CCABlockingManager::HandleChange
// ---------------------------------------------------------
//
void CCABlockingManager::HandleChange(
    MCAContactList* /*aList*/,
    MCAStoredContact* /*aContact*/,
    TStorageManagerGlobals::TCAObserverEventType aEventType,
    TBool /*aUserIdChanged*/ )
    {
    if ( !iLoggedIn )
        {
        // Logout event received, stop observing
        iStoredContacts->RemoveObserver( this );
        return;
        }

    // Mark contacts blocked as they are fetched
    if ( aEventType == TStorageManagerGlobals::EStorageEventMultipleChanges )
        {
        MCAContactList* currentList = NULL;
        TRAPD( err, currentList = &iStoredContacts->ListInSyncProcessL() );
        if ( err == KErrNone && currentList )
            {
            // Sync not yet ready means that contact adding is going on
            // -> check list for blocked ids
            if ( currentList->Synchronised() == MCAContactList::ESynchroniseNotDone )
                {
                TInt blockCount = iBlockedList->Count();
                for ( TInt i = 0; i < blockCount; i++ )
                    {
                    TPtrC userId = iBlockedList->MdcaPoint( i );
                    TPtrC listId = currentList->ListId();
                    iStoredContacts->SetBlocked( userId, listId, ETrue );
                    }
                }
            }
        }

    // Check if sync has ended
    TBool failed = EFalse;
    TBool syncDone = EFalse;

    TRAP_IGNORE(
        MCAContactLists* listInterface =
            CAPresenceManager::InstanceL()->ContactLists();
        syncDone = listInterface->IsSyncDone();
    );  // TRAP_IGNORE

    if ( iStoredContacts->IsAllSynchronised( failed ) &&
         syncDone )
        {
        if ( !failed )
            {
            // Sync done and no failed lists, no need to observe anymore
            iStoredContacts->RemoveObserver( this );
            }
        // If there were failed lists keep on observing for
        // the possible re-fetching of the failed lists.
        }
    }

// -----------------------------------------------------------------------------
// CCABlockingManager::SetTemporaryGrantL
// Temporary grant, permanent unblock.
// -----------------------------------------------------------------------------
//
TInt CCABlockingManager::SetTemporaryGrantL( const TDesC& aContact, TBool aOn )
    {
    //Creating list of unblocked users
    CDesCArrayFlat* unBlockList =
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( unBlockList );

    // unBlockList is those persons who are going to be unblocked
    unBlockList->AppendL( aContact );

    TInt errorCode( KErrNone );
    if ( aOn )
        {
        if ( GrantListInUse() )
            {
            errorCode = InsertL( NULL, unBlockList );
            }
        TInt temp( RemoveL( unBlockList, NULL ) );
        if ( temp != KErrNone && errorCode == KErrNone )
            {
            errorCode = temp;
            }
        }
    else
        {
        MCAStoredContact* contact = iStoredContacts->FindAnyContact( aContact );
        if ( !( contact && iStrategy == MCASettings::EFriends ) )
            {
            errorCode = RemoveL( NULL, unBlockList );
            }
        }

    CleanupStack::PopAndDestroy( unBlockList );

    return errorCode;
    }


// -----------------------------------------------------------------------------
// CCABlockingManager::IsContactGranted
// Check if contact is granted or not.
// -----------------------------------------------------------------------------
//
TBool CCABlockingManager::IsContactGranted( const TDesC& aContact ) const
    {
    TInt count( iGrantedList->MdcaCount() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        if ( 0 == CAUtils::NeutralCompare( iGrantedList->MdcaPoint( a ),
                                           aContact ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCABlockingManager::CloneArrayLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CDesCArray* CCABlockingManager::CloneArrayLC( const MDesCArray& aSource )
    {
    TInt count( aSource.MdcaCount() );
    CDesCArray* array = new( ELeave )CDesCArrayFlat( count == 0 ? 1 : count );
    CleanupStack::PushL( array );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC text( aSource.MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( " CloneArrayLC [%d] %S" ), i, &text );
        array->AppendL( text );
        }
    CHAT_DP_TXT( "----------------------[endClone]----------------------" );
    return array;
    }

// -----------------------------------------------------------------------------
// CCABlockingManager::CreateNeutralDiffLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CDesCArray* CCABlockingManager::CreateNeutralDiffLC(
    const MDesCArray& aArrayDiff,
    const MDesCArray& aArrayTest )
    {
    CDesCArray* array = new( ELeave )CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( array );
    TInt count( aArrayDiff.MdcaCount() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC id( aArrayDiff.MdcaPoint( i ) );
        TInt count2( aArrayTest.MdcaCount() );
        TInt flag( EFalse );

        for ( TInt j( 0 ); j < count2 && !flag; ++j )
            {
            if ( CAUtils::NeutralCompare( id, aArrayTest.MdcaPoint( j ) ) == 0 )
                {
                flag = ETrue;
                }
            }
        if ( !flag )
            {
            // item that is in aArrayDiff, but not in aArrayTest
            // add to resulting array if it does not exist in resulting array
            TInt count3( array->MdcaCount() );
            for ( TInt k( 0 ); k < count3 && !flag; ++k )
                {
                if ( CAUtils::NeutralCompare( id, array->MdcaPoint( k ) ) == 0 )
                    {
                    flag = ETrue;
                    }
                }

            if ( !flag )
                {
                CHAT_DP( D_CHAT_LIT( " CreateDiffLC [%d] %S" ), i, &id );
                array->AppendL( id );
                }
            }
        }
    CHAT_DP_TXT( "-----------------------[endDiff]----------------------" );
    return array;
    }

// -----------------------------------------------------------------------------
// CCABlockingManager::AddToGrantedListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABlockingManager::AddToGrantedListL( const MDesCArray& aInsertArray )
    {
    TInt count( aInsertArray.MdcaCount() );
    for ( TInt a( 0 ); a < count && !iUpdateInProgress; ++a )
        {
        iGrantedList->AppendL( aInsertArray.MdcaPoint( a ) );
        }
    }

// -----------------------------------------------------------------------------
// CCABlockingManager::RemoveFromGrantedListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABlockingManager::RemoveFromGrantedList( CDesCArray& aRemoveArray )
    {
    TInt count( aRemoveArray.MdcaCount() );
    for ( TInt a( --count ); a >= 0 && !iUpdateInProgress; --a )
        {
        TInt grantedCount( iGrantedList->MdcaCount() );
        TBool foundFlag( EFalse );
        TBool foundOther( EFalse );
        TPtrC id( aRemoveArray.MdcaPoint( a ) );

        for ( TInt b( --grantedCount ); b >= 0 && !foundOther; --b )
            {
            TPtrC id2( iGrantedList->MdcaPoint( b ) );
            if ( CAUtils::NeutralCompare( id, id2 ) == 0 )
                {
                if ( !foundFlag )
                    {
                    // Delete current position
                    iGrantedList->Delete( b );
                    foundFlag = ETrue;
                    }
                else
                    {
                    foundOther = ETrue;
                    }
                }
            }
        if ( foundOther )
            {
            aRemoveArray.Delete( a );
            }
        }
    }
// -----------------------------------------------------------------------------
// CCABlockingManager::SetAppUiExitFlag
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABlockingManager::SetAppUiExitFlag( TBool aFlag )
	{
	iAppUiExit = aFlag;
	}
// end of file
