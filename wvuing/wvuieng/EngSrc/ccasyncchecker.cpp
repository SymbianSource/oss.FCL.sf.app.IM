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
* Description:  Helper class for checking synch state.
*
*/



#include "ccasyncchecker.h"

#include "mcastoredcontacts.h"
#include "mcapresence.h"

#include "ccastoragemanagerfactory.h"
#include "capresencemanager.h"

#include "impsbuilddefinitions.h"
#include "chatdebugprint.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------
// CCASyncChecker::NewLC()
// Called when a contact is deleted
// ---------------------------------------------------------
//
CCASyncChecker* CCASyncChecker::NewLC()
    {
    CCASyncChecker* self = new ( ELeave ) CCASyncChecker();
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------
// CCASyncChecker::~CCASyncChecker()
// Called when a contact is deleted
// ---------------------------------------------------------
//
CCASyncChecker::~CCASyncChecker()
    {
    }

// ---------------------------------------------------------
// CCASyncChecker::WaitForSyncCompleteL()
// Called when a contact is deleted
// ---------------------------------------------------------
//
TBool CCASyncChecker::WaitForSyncCompleteL()
    {
    MCAStoredContacts* contacts = CCAStorageManagerFactory::ContactListInterfaceL();

    TBool failed = EFalse;

    if ( !contacts->IsAllSynchronised( failed ) ||
         !IsSyncDoneL() )
        {
        contacts->AddObserverL( this );

        if ( !iWait.IsStarted() )
            {
            iWait.Start();  // CSI: 10 # iWait is not an active object
            }

        contacts->RemoveObserver( this );
        }
    TBool allSync = contacts->IsAllSynchronised( failed );
    TBool syncDone = IsSyncDoneL();
    return ( allSync && !failed && syncDone );
    }

// ---------------------------------------------------------
// CCASyncChecker::IsSyncDoneL()
// Called when a contact is deleted
// ---------------------------------------------------------
//
TBool CCASyncChecker::IsSyncDoneL()
    {
    MCAContactLists* listInterface =
        CAPresenceManager::InstanceL()->ContactLists();

    return listInterface->IsSyncDone();
    }

// ---------------------------------------------------------
// CCASyncChecker::CheckSyncStateL()
// Called when a contact is deleted
// ---------------------------------------------------------
//
TBool CCASyncChecker::CheckSyncStateL()
    {
    TBool retVal = ETrue;
#ifdef IMPS_CONTACT_FETCH_BACKGROUND

    MCAStoredContacts* contactList =
        CCAStorageManagerFactory::ContactListInterfaceL();

    MCAContactLists* listInterface =
        CAPresenceManager::InstanceL()->ContactLists();
    if ( !listInterface )
        {
        // not logged in yet, so sync state is ok
        return ETrue;
        }

    TBool syncDone = listInterface->IsSyncDone();

    TBool ignoreFailed;
    if ( !contactList->IsAllSynchronised( ignoreFailed ) ||
         !syncDone )
        {
        CCASyncChecker* checker = NewLC();
        retVal = checker->WaitForSyncCompleteL();
        CleanupStack::PopAndDestroy(); //checker
        }

#endif //IMPS_CONTACT_FETCH_BACKGROUND
    return retVal;
    }

// ---------------------------------------------------------
// CCASyncChecker::HandleDelete()
// Called when a contact is deleted
// ---------------------------------------------------------
//

void CCASyncChecker::HandleContactDelete( const TDesC& /* aContact */ )
    {
    // Not used
    }

// ---------------------------------------------------------
// CCASyncChecker::HandleAddition()
// Called when a contact is added
// ---------------------------------------------------------
//
void CCASyncChecker::HandleAddition( MCAContactList& /*aList*/,
                                     MCAStoredContact& /*aContact*/ )
    {
    // Not used
    }

// ---------------------------------------------------------
// CCASyncChecker::HandleChange()
// Called when a contact is changed
// ---------------------------------------------------------
//
void CCASyncChecker::HandleChange(
    MCAContactList* /*aList*/,
    MCAStoredContact* /*aContact*/,
    TStorageManagerGlobals::TCAObserverEventType /*aEventType*/,
    TBool /*aUserIdChanged*/ )
    {
    CHAT_DP_FUNC_ENTER( "CCASyncChecker::HandleChange" );

    MCAStoredContacts* storage = NULL;
    TRAPD( error, storage = CCAStorageManagerFactory::ContactListInterfaceL() );

    if ( error != KErrNone )
        {
        return;
        }

    TBool ignoreFailed = EFalse;
    TBool syncDone = EFalse;
    TRAP_IGNORE( syncDone = IsSyncDoneL() );
    if ( storage->IsAllSynchronised( ignoreFailed ) &&
         syncDone &&
         iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }

    CHAT_DP_FUNC_DONE( "CCASyncChecker::HandleChange" );
    }

// end of file
