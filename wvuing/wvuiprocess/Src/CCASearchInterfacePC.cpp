/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for PC Search Interface
*
*/



// INCLUDE FILES
#include "CCASearchInterfacePC.h"
#include "MCASearchBackgroundObserverPC.h"
#include "MCASearchInterface.h"
#include "MCASearchObserverInterfacePC.h"
#include "MCAStoredContacts.h"
#include "CCAStorageManagerFactory.h"
#include "MCAGroupManagerInterface.h"
#include "MCAExtendedStoredGroup.h"
#include "MCAGroupOperations.h"
#include "MCAStoredGroups.h"
#include "ImpsCSPAllErrors.h"
#include "TEnumsPC.h"
#include "MCAServerContactsArrayPC.h"
#include "CCAServerContactsArrayPC.h"



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCASearchInterfacePC: Constructor
// -----------------------------------------------------------------------------
//
CCASearchInterfacePC::CCASearchInterfacePC( MCASearchInterface* aSearchInterface,
                                            MCABackgroundInterface& aBackgroundInterface,
                                            MCAGroupManagerInterface& aGroupMgrInterface )
        : iSearchInterface( aSearchInterface ),
        iBackgroundInterface( aBackgroundInterface ),
        iGroupMgrInterface( aGroupMgrInterface )
    {
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC: ConstructL
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::ConstructL(  )
    {
    iStoredGroupsInterface = CCAStorageManagerFactory::GroupListInterfaceL();
    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC: NewL
// -----------------------------------------------------------------------------
//
CCASearchInterfacePC* CCASearchInterfacePC::NewL( MCASearchInterface* aSearchInterface,
                                                  MCABackgroundInterface& aBackgroundInterface,
                                                  MCAGroupManagerInterface& aGroupMgrInterface )
    {


    CCASearchInterfacePC* self = new ( ELeave ) CCASearchInterfacePC( aSearchInterface,
                                                                      aBackgroundInterface,
                                                                      aGroupMgrInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC: Destructor
// -----------------------------------------------------------------------------
//
CCASearchInterfacePC::~CCASearchInterfacePC()
    {
    if ( iBackgroundObserver )
        {
        iBackgroundInterface.UnRegisterBackgroundObserver( this );
        }
    // delete searchpairs

    if ( iSearchPairs )
        {
        iSearchPairs->ResetAndDestroy();
        delete iSearchPairs;
        iSearchPairs = NULL;
        }

    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC: StartSearchL
// -----------------------------------------------------------------------------
//
TInt CCASearchInterfacePC::StartSearchL( TInt aSearchLimit,
                                         MCASearchObserverInterfacePC* aObserver, CSearchPairs* aSearchPairs )
    {
    //This code is not yet useful for the existing chat.
    //the current calls for searching to the engine are synchronous calls.
    AddObserver( aObserver );

    //For the existing chat application with synchronous calls
    //to search, pass NULL as the observer
    if ( !aSearchPairs )
        {
        aSearchPairs = SearchPairs();
        }

    return iSearchInterface->StartSearchL( *aSearchPairs, aSearchLimit, NULL );
    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC: EndSearchL
// -----------------------------------------------------------------------------
//
TInt CCASearchInterfacePC::EndSearchL()
    {
    RemoveObserver();
    return iSearchInterface->EndSearchL();
    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC: SearchNextL
// -----------------------------------------------------------------------------
//
TInt CCASearchInterfacePC::SearchNextL( TInt aIndex, MCASearchObserverInterfacePC* aObserver )
    {
    //This code is not yet useful for the existing chat.
    //the current calls for searching to the engine are synchronous calls.
    AddObserver( aObserver );

    //For the existing chat application with synchronous calls
    //to search, pass NULL as the observer
    return iSearchInterface->SearchNextL( aIndex, NULL );
    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC: AddObserver
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::AddObserver( MCASearchObserverInterfacePC* aObserver )
    {
    iSearchObserverPC = aObserver;
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC: RemoveObserver
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::RemoveObserver()
    {
    //Not the owner of observer. so dont delete
    iSearchObserverPC = NULL;
    }



// -----------------------------------------------------------------------------
// CCASearchInterfacePC: HandleSearchError
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::HandleSearchError( TInt aErrorCode )
    {
    if ( iSearchObserverPC )
        {
        iSearchObserverPC->HandleSearchError( aErrorCode );
        }
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC: HandleSearchFinished
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::HandleSearchFinished( )
    {
    if ( iSearchObserverPC )
        {
        iSearchObserverPC->HandleSearchFinished();
        }

    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC::BackgroundTaskStatus
// -----------------------------------------------------------------------------
//
TInt CCASearchInterfacePC::BackgroundTaskStatus( TEnumsPC::TCABackgroundTasks aTaskID )
    {
    MCABackgroundInterface::TCABackgroundTasks eventSource = ConvertToTCABackgroundTasks( aTaskID );
    return iBackgroundInterface.BackgroundTaskStatus( eventSource );
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC::RegisterBackgroundObserver
// -----------------------------------------------------------------------------
//
TInt CCASearchInterfacePC::RegisterBackgroundObserver(
    MCASearchBackgroundObserverPC* aBackgroundObserver,
    TInt aTaskMask,
    TInt aEventMask	)
    {
    iBackgroundObserver = aBackgroundObserver;
    TInt status = iBackgroundInterface.RegisterBackgroundObserver( this,
                                                                   aTaskMask, aEventMask );
    ASSERT( aBackgroundObserver );
    return status;
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC::UnRegisterBackgroundObserver
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::UnRegisterBackgroundObserver(
    MCASearchBackgroundObserverPC* aBackgroundObserver )

    {
    if ( iBackgroundObserver == aBackgroundObserver )
        {
        iBackgroundInterface.UnRegisterBackgroundObserver( this );
        }
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC::HandleBackgroundEvent
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::HandleBackgroundEvent(
    MCABackgroundInterface::TCABackgroundTasks aEventSource,
    MCABackgroundInterface::TCABackgroundStatus aStatus,
    TInt aSubTaskNumber,
    TInt aLeaveCode
)
    {
    TEnumsPC::TCABackgroundTasks eventSource = ConvertTCABackgroundTasksToTEnumsPC( aEventSource );
    TEnumsPC::TCABackgroundStatus status = ConvertTCABackgroundStatus( aStatus );
    iBackgroundObserver->HandleBackgroundEventPC( eventSource,
                                                  status, aSubTaskNumber, aLeaveCode );
    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC::ConvertToTCABackgroundTasks
// -----------------------------------------------------------------------------
//
MCABackgroundInterface::TCABackgroundTasks CCASearchInterfacePC::
ConvertToTCABackgroundTasks( TEnumsPC::TCABackgroundTasks aSource )
    {
    MCABackgroundInterface::TCABackgroundTasks eventSource;
    switch ( aSource )
        {
        case TEnumsPC::EGroupFetch:
            {
            eventSource = MCABackgroundInterface::EGroupFetch;
            break;
            }
        case TEnumsPC::EPresenceTask:
            {
            eventSource = MCABackgroundInterface::EPresence;
            break;
            }
        case TEnumsPC::EWhisperSync:
            {
            eventSource = MCABackgroundInterface::EWhisperSync;
            break;
            }
        case TEnumsPC::EBlockGrantListFetch:
        default:
            {
            eventSource = MCABackgroundInterface::EBlockGrantListFetch;
            break;
            }
        }

    return eventSource;
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC::ConvertTCABackgroundTasksToTEnumsPC
// -----------------------------------------------------------------------------
//
TEnumsPC::TCABackgroundTasks CCASearchInterfacePC::ConvertTCABackgroundTasksToTEnumsPC
( MCABackgroundInterface::TCABackgroundTasks aEventSource )
    {
    TEnumsPC::TCABackgroundTasks eventSource;
    switch ( aEventSource )
        {
        case MCABackgroundInterface::EGroupFetch:
            {
            eventSource = TEnumsPC::EGroupFetch;
            break;
            }
        case MCABackgroundInterface::EPresence:
            {
            eventSource = TEnumsPC::EPresenceTask;
            break;
            }
        case MCABackgroundInterface::EWhisperSync:
            {
            eventSource = TEnumsPC::EWhisperSync;
            break;
            }
        case MCABackgroundInterface::EBlockGrantListFetch:
            {
            eventSource = TEnumsPC::EBlockGrantListFetch;
            break;
            }
        default:
            {
            eventSource = TEnumsPC::EFailed;
            break;
            }
        }

    return eventSource;
    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC::ConvertTCABackgroundStatus
// -----------------------------------------------------------------------------
//
TEnumsPC::TCABackgroundStatus CCASearchInterfacePC::ConvertTCABackgroundStatus
( MCABackgroundInterface::TCABackgroundStatus aStatus )
    {
    TEnumsPC::TCABackgroundStatus status;

    switch ( aStatus )
        {
        case MCABackgroundInterface::EUnknown:
            {
            status = TEnumsPC::EUnknownStatus;
            break;
            }
        case MCABackgroundInterface::EIdleStarted:
            {
            status = TEnumsPC::EIdleStarted;
            break;
            }
        case MCABackgroundInterface::EStarting:
            {
            status = TEnumsPC::EStarting;
            break;
            }
        case MCABackgroundInterface::EWaiting:
            {
            status = TEnumsPC::EWaiting;
            break;
            }
        case MCABackgroundInterface::ECompleted:
            {
            status = TEnumsPC::ECompleted;
            break;
            }
        case MCABackgroundInterface::ECancelled:
            {
            status = TEnumsPC::ECancelled;
            break;
            }
        case MCABackgroundInterface::EFailed:
        default:
            {
            status = TEnumsPC::EFailedStatus;
            break;
            }
        }

    return status;
    }



// -----------------------------------------------------------------------------
// CCASearchInterfacePC::SetRequestL
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::SetRequestL(  RArray<TEnumsPC::TSearchCriteria>&  aSearchCriteriaArray,
                                         CDesCArray& aSearchStringArray )
    {
    TInt count = aSearchCriteriaArray.Count();
    ASSERT( count );
    SetSearchPairsL( count );

    for ( TInt i( 0 ); i < count; i++ )
        {
        CImpsSearchRequest* request = CImpsSearchRequest::NewL();
        CleanupStack::PushL( request );
        request->SetRequestL( ConvertToTImpsSearchElement(
                                  aSearchCriteriaArray[i] ), aSearchStringArray[i] );
        iSearchPairs->AppendL( request ); //Takes the ownership
        CleanupStack::Pop( request );
        }

    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC::ConvertToTImpsSearchElement
// -----------------------------------------------------------------------------
//
TImpsSearchElement CCASearchInterfacePC::
ConvertToTImpsSearchElement( TEnumsPC::TSearchCriteria aElement )
    {
    TImpsSearchElement searchElement( EImpsUserID );

    switch ( aElement )
        {
        case TEnumsPC::ESearchGroupName:
            {
            searchElement = EImpsGroupName;
            break;
            }
        case TEnumsPC::ESearchGroupTopic:
            {
            searchElement = EImpsGroupTopic;
            break;
            }
        case TEnumsPC::ESearchGroupUserIDJoined:
            {
            searchElement = EImpsGroupUserIDJoined;
            break;
            }
        case TEnumsPC::ESearchUserName:
            {
            searchElement = EImpsUserFirstName;
            break;
            }

        case TEnumsPC::ESearchUserLastName:
            {
            searchElement = EImpsUserLastName;
            break;
            }
        case TEnumsPC::ESearchUserID:
            {
            searchElement = EImpsUserID;
            break;
            }
        case TEnumsPC::ESearchUserMobileNumber:
            {
            searchElement = EImpsUserMobileNumber;
            break;
            }
        case TEnumsPC::ESearchUserEmailAddress:
            {
            searchElement = EImpsUserEmailAddress;
            break;
            }
        default:
            {
            break;
            }
        }

    return searchElement;
    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC::SearchPairs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSearchPairs* CCASearchInterfacePC::SearchPairs()
    {
    return iSearchPairs;
    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC::SetSearchPairsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::SetSearchPairsL( TInt aCount )
    {
    if ( iSearchPairs )
        {
        iSearchPairs->ResetAndDestroy();
        delete iSearchPairs;
        iSearchPairs = NULL;
        }

    if ( aCount > 0 )
        {
        iSearchPairs = new ( ELeave ) CSearchPairs( aCount );
        }

    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC::IsSearchPairsSet
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASearchInterfacePC::IsSearchPairsSet()
    {
    if ( SearchPairs() )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC::GetSearchStringLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCASearchInterfacePC::GetSearchStringLC()
    {
    HBufC* searchString = HBufC::NewLC( 0 );

    TInt count = iSearchPairs->Count();

    for ( TInt i = 0; i < count; i++ )
        {
        TPtrC newValue = ( *iSearchPairs )[i]->Value();

        TInt space = 0;
        if ( i < count - 1 )
            {
            // Memory needed for space
            space = KSpace().Length();
            }

        // Reallocate for adding
        searchString = searchString->ReAllocL(
                           searchString->Length() + newValue.Length() + space );

        // Update cleanup stack
        // we are deliberately calling pop and pushL for
        // because we are poping old value and pushing new value
        // Codescanner warning ignored
        CleanupStack::Pop( ); //searchString
        CleanupStack::PushL( searchString );

        // Append data
        TPtr searchStringPtr( searchString->Des() );
        searchStringPtr.Append( newValue );
        if ( i < count - 1 )
            {
            // Append space
            searchStringPtr.Append( KEmptyDesC );
            }
        }
    return searchString;
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC: FindAnyContactL
// -----------------------------------------------------------------------------
//
TBool CCASearchInterfacePC::FindAnyContactL( const TDesC& aContactId )
    {
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    if ( contacts->FindAnyContact( aContactId ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC: GetPropertiesL
// -----------------------------------------------------------------------------
//
TEnumsPC::TImpsCSPAllErrors CCASearchInterfacePC::GetPropertiesL( const TDesC& aGroupId,
                                                                  CImpsCommonGroupProps*& aCommonProps,
                                                                  CImpsPrivateGroupProps*& aPrivateProps )
    {
    MCAGroupOperations* grOps = iGroupMgrInterface.GroupOperationsL( aGroupId );

    TInt err( KErrNone );

    err = grOps->GetPropertiesL( aCommonProps, aPrivateProps );
    TEnumsPC::TImpsCSPAllErrors impsErr = ( TEnumsPC::TImpsCSPAllErrors )err;
    return impsErr;
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC::GetGroupName
// -----------------------------------------------------------------------------
//
TPtrC CCASearchInterfacePC::GetGroupNameL( const TDesC& aWvid, TBool& aHasCommonProps,
                                           TEnumsPC::TImpsCSPAllErrors& aImpsError )
    {
    // we have the group so we can fetch the properties for it
    CImpsPrivateGroupProps* privateProps = NULL;
    CImpsCommonGroupProps* commonProps = NULL;
    MCAGroupOperations* grOps =
        iGroupMgrInterface.GroupOperationsL( aWvid );
    if ( grOps )
        {
        aImpsError = ( TEnumsPC::TImpsCSPAllErrors )grOps->GetPropertiesL(
                         commonProps, privateProps ) ;
        if ( commonProps )
            {
            aHasCommonProps = ETrue;
            return commonProps->GroupName();
            }
        else
            {
            aHasCommonProps = EFalse;
            }

        }
    return KNullDesC(); //Error condition
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC: SetGroupNameL
// -----------------------------------------------------------------------------
//
TEnumsPC::TImpsCSPAllErrors CCASearchInterfacePC::ConvertImpsErrorToUiEnums( TInt aImpsError )
    {
    TEnumsPC::TImpsCSPAllErrors impsErr;
    switch ( aImpsError )
        {
        case ECSPInsufficientGroupPrivileges:
            {
            impsErr = TEnumsPC::ECSPInsufficientGroupPrivileges;
            break;
            }
        case ECSPNotSupported:
            {
            impsErr = TEnumsPC::ECSPNotSupported;
            break;
            }
        case ECSPInvalidParameter:
            {
            impsErr =  TEnumsPC::ECSPInvalidParameter;
            break;
            }
        case 0:
            {
            impsErr =  TEnumsPC::EImpsErrNone;
            break;
            }
        default:
            {
            impsErr = TEnumsPC::ECSPMultipleErrors; //general errors.
            break;
            }
        }
    return impsErr;
    }
// -----------------------------------------------------------------------------
// CCASearchInterfacePC: SetGroupNameL
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::SetGroupNameL( const TDesC& aGroupId, const TDesC& aGroupName )
    {
    MCAStoredGroups* storedGroups =
        CCAStorageManagerFactory::GroupListInterfaceL();
    MCAExtendedStoredGroup* storedGrp =
        static_cast<MCAExtendedStoredGroup*>( storedGroups->FindGroup( aGroupId ) );
    storedGrp->SetGroupNameL( aGroupName );
    }
// -----------------------------------------------------------------------------
// CCASearchInterfacePC: SaveChangesL
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::SaveChangesL( const TDesC& aGroupId )
    {
    if ( iStoredGroupsInterface )
        {
        MCAExtendedStoredGroup* storedGrp =
            static_cast<MCAExtendedStoredGroup*>( iStoredGroupsInterface->FindGroup( aGroupId ) );
        if ( storedGrp )
            {
            storedGrp->SaveChangesL();
            }
        }
    }
// -----------------------------------------------------------------------------
// CCASearchInterfacePC: CreateGroupL
// -----------------------------------------------------------------------------
//
void CCASearchInterfacePC::CreateGroupL( const TDesC& aGroupId )
    {
    if ( iStoredGroupsInterface )
        {
        // Ownership not transferred to caller. Hence CodeScanner occasionally gives
        //false positives for this issue and the warning is ignored.
        MCAExtendedStoredGroup* group = iStoredGroupsInterface->CreateGroupL();

        if ( group )
            {
            CleanupStack::PushL( group );
            group->SetGroupIdL( aGroupId );
            CleanupStack::Pop();
            }

        }
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC::GetPairArrayForServerContactsArrayL
// -----------------------------------------------------------------------------
//
MCAServerContactsArrayPC* CCASearchInterfacePC::GetPairArrayForServerContactsArrayLC()
    {

    CCAServerContactsArrayPC* pairsArray = NULL;

    if ( iStoredGroupsInterface )
        {

        RPointerArray<MCAStoredGroup> groupList;

        CDesCArray* groupNames = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
        CleanupStack::PushL( groupNames );

        CDesCArray* groupIds = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
        CleanupStack::PushL( groupIds );

        CleanupClosePushL( groupList );


        iStoredGroupsInterface->PopulateGroupsListL( groupList );

        TInt count( groupList.Count() );
        //For each item in the group list get the groupname and group id
        for ( TInt index( 0 ); index < count; index++ )
            {
            groupNames->AppendL( groupList[ index ]->GroupName() );
            groupIds->AppendL( groupList[ index ]->GroupId() );
            }

        //Codescanner warning ignored since we have used CleanupClosePushL
        //to push the groupList
        CleanupStack::PopAndDestroy(); // groupList.Close()

        // servercontactsarray takes the ownership. Codescanner warning ignored
        CleanupStack::Pop( 2 ); // groupIds,groupNames

        pairsArray = CCAServerContactsArrayPC::NewL( groupNames, groupIds, KNullDesC );

        CleanupStack::PushL( pairsArray );

        }


    //Not owned. Transfer ownership of pairsArray to the caller.
    return pairsArray;

    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC::FindGroup
// -----------------------------------------------------------------------------
//
TBool CCASearchInterfacePC::FindGroup( const TDesC& aWvid )
    {
    if ( iStoredGroupsInterface )
        {
        MCAExtendedStoredGroup* group =
            static_cast <MCAExtendedStoredGroup*> (
                iStoredGroupsInterface->FindGroup( aWvid ) );

        if ( group )
            {
            return ETrue;
            }
        }
    return EFalse;//error condition or did not find a group by wvid
    }


// -----------------------------------------------------------------------------
// CCASearchInterfacePC::DeleteGroupL
// -----------------------------------------------------------------------------
//
TInt CCASearchInterfacePC::DeleteGroupL( const TDesC& aGroupId, TBool aDeleteFromNetwork )
    {
    return iGroupMgrInterface.DeleteGroupL( aGroupId, aDeleteFromNetwork );
    }

// -----------------------------------------------------------------------------
// CCASearchInterfacePC::IsFavouriteChatGroup
// -----------------------------------------------------------------------------
//
TBool CCASearchInterfacePC::IsFavouriteChatGroup( const TDesC& aGroupId ) const
    {
    TBool isFavourite( EFalse );

    MCAStoredGroup* group = iStoredGroupsInterface->FindGroup( aGroupId );
    if ( group )
        {
        isFavourite = ( group->StorageType() ==
                        TStorageManagerGlobals::EStoragePersistent );
        }
    return isFavourite;
    }
// -----------------------------------------------------------------------------
// CCASearchInterfacePC::JoinedGroups
// -----------------------------------------------------------------------------
//
TInt CCASearchInterfacePC::JoinedGroups()
    {
    return iGroupMgrInterface.JoinedGroups();
    }
// End of File

