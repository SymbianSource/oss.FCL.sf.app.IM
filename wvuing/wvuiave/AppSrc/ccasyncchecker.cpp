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
#include "ccaappui.h"
#include "ccauisessionmanager.h"
#include "MCAGlobalNotificationObserverPC.h"
#include "MCAProcessManager.h"
#include "impsbuilddefinitions.h"
#include "chatdebugprint.h"
// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

#include <chatNG.rsg>

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
// CCASyncChecker::CCASyncChecker()
// Called when a contact is deleted
// ---------------------------------------------------------
//
CCASyncChecker::CCASyncChecker()
        : iAppUi( static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() ) )
    {
    iGlobalNotificationPC = iAppUi->GetProcessManager().GetGlobalNotificationInterface();
    delete iWaitNote;
    }


// ---------------------------------------------------------
// CCASyncChecker::~CCASyncChecker()
// Called when a contact is deleted
// ---------------------------------------------------------
//
CCASyncChecker::~CCASyncChecker()
    {
    delete iWaitNote;
    }

// ---------------------------------------------------------
// CCASyncChecker::WaitForSyncCompleteL()
// Called when a contact is deleted
// ---------------------------------------------------------
//
TBool CCASyncChecker::WaitForSyncCompleteL()
    {
    if ( !LoggedIn() )
        {
        // Not logged in. No reason to wait
        return EFalse;
        }

    TBool failed;
    if ( !iGlobalNotificationPC->StoredContactsIsAllSynchronised( failed ) ||
         !iGlobalNotificationPC->IsContactListsSyncDoneL() )
        {
        iWaitNoteCanceled = EFalse;
        iGlobalNotificationPC->AddGlobalObserverL( this );
        iWaitNote = CCAWaitNote::ShowWaitNoteL(
                        R_QTN_CHAT_FETCHING_LISTS_ONGOING,
                        ETrue, ETrue, this );

        if ( ! iWait.IsStarted() )
            {
            iWait.Start();  // CSI: 10 # iWait is not an active object
            }
        delete iWaitNote;
        iGlobalNotificationPC->RemoveGlobalObserver( this );
        iWaitNote = NULL;
        if ( iWaitNoteCanceled )
            {
            return EFalse;
            }
        }
    TBool allSync = iGlobalNotificationPC->StoredContactsIsAllSynchronised( failed );
    TBool syncDone = iGlobalNotificationPC->IsContactListsSyncDoneL();
    return ( allSync && !failed && syncDone );
    }


// ---------------------------------------------------------
// CCASyncChecker::LoggedIn()
// Called when a contact is deleted
// ---------------------------------------------------------
//
TBool CCASyncChecker::LoggedIn()
    {
    return iAppUi->UISessionManager().IsLoggedIn();
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
    //Code scanner warning ignored, since this is a static method
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    MCAGlobalNotificationObserverPC* globalNotificationPC =
        appUi->GetProcessManager().GetGlobalNotificationInterface();
    if ( globalNotificationPC->IsValidListInterfaceL() == EFalse )
        {
        // not logged in yet, so sync state is ok
        return ETrue;
        }

    TBool syncDone = globalNotificationPC->IsContactListsSyncDoneL();
    TBool ignoreFailed;
    if ( !globalNotificationPC->StoredContactsIsAllSynchronised( ignoreFailed ) ||
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

void CCASyncChecker::HandleDelete( const TDesC& /*aContactId*/ , TEnumsPC::TItem /*type*/ )
    {
    // Not used
    }

// ---------------------------------------------------------
// CCASyncChecker::HandleAddition()
// Called when a contact is added
// ---------------------------------------------------------
//
void CCASyncChecker::HandleAddition(  TEnumsPC::TItem /*type*/ )
    {
    // Not used
    }

// ---------------------------------------------------------
// CCASyncChecker::HandleChange()
// Called when a contact is changed
// ---------------------------------------------------------
//
void CCASyncChecker::HandleChange( TEnumsPC::TItem /*aType*/, TEnumsPC::TChange /*aChangeType*/ )
    {
    CHAT_DP_FUNC_ENTER( "CCASyncChecker::HandleChange" );

    if ( !iGlobalNotificationPC->IsValidStoredContactsInterface() )
        {
        return;
        }

    TBool ignoreFailed;
    TBool syncDone = EFalse;
    TRAP_IGNORE( syncDone = iGlobalNotificationPC->IsContactListsSyncDoneL() );
    if ( iWaitNote &&
         iGlobalNotificationPC->StoredContactsIsAllSynchronised( ignoreFailed ) &&
         syncDone )
        {
        iWaitNote->DismissDialog();
        iWaitNoteCanceled = EFalse;
        }

    CHAT_DP_FUNC_DONE( "CCASyncChecker::HandleChange" );
    }

// ---------------------------------------------------------
// CCASyncChecker::NoteCanceled
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASyncChecker::NoteCanceled( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyCancel )
        {
        iWaitNoteCanceled = ETrue;
        }
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// end of file
