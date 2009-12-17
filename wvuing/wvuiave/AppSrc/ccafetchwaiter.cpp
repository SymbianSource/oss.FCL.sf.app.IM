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
* Description:  Helper class for waiting fetch process.
*
*/



#include "ccafetchwaiter.h"

#include "ccaappui.h"
#include "CCAUISessionManager.h"

#include "impsbuilddefinitions.h"
#include "chatdebugprint.h"

#include "MCASessionHandlerCmd.h"
#include "CCACommandManagerFactory.h"
#include "CCACommandManager.h"
#include <chatNG.rsg>

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------
// CCAFetchWaiter::NewLC()
// ---------------------------------------------------------
//
CCAFetchWaiter* CCAFetchWaiter::NewLC()
    {
    CCAFetchWaiter* self = new ( ELeave ) CCAFetchWaiter();
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------
// CCAFetchWaiter::~CCAFetchWaiter()
// ---------------------------------------------------------
//
CCAFetchWaiter::~CCAFetchWaiter()
    {
    delete iWaitNote;
    }

// ---------------------------------------------------------
// CCAFetchWaiter::WaitForSyncCompleteL()
// ---------------------------------------------------------
//
TBool CCAFetchWaiter::WaitForFetchCompleteL()
    {
    if ( !LoggedIn() )
        {
        // Not logged in. No reason to wait
        return EFalse;
        }

    if ( !IsFetchDone() )
        {
        iWaitNoteCanceled = EFalse;
        CCACommandManager* commandManager =  CCACommandManagerFactory::InstanceL()->GetCommandManager();
        MCASessionHandlerCmd* sessHandlerCmd =  commandManager->GetSessionHandlerCmd() ;
        if ( sessHandlerCmd )
            {
            sessHandlerCmd->AddContactFetchObserverL( this );
            }



        iWaitNote = CCAWaitNote::ShowWaitNoteL(
                        R_QTN_GEN_CHAT_NOTE_PROCESSING,
                        ETrue, EFalse, this );

        CHAT_DP_TXT( "CCAFetchWaiter::WaitForFetchCompleteL - start wait" );
        if ( !iWait.IsStarted() )
            {
            iWait.Start();  // CSI: 10 # iWait is not an active object
            }
        CHAT_DP_TXT( "CCAFetchWaiter::WaitForFetchCompleteL - end wait" );
        delete iWaitNote;

        if ( sessHandlerCmd )
            {
            sessHandlerCmd->RemoveContactFetchObserver( this );
            }
        iWaitNote = NULL;
        if ( iWaitNoteCanceled )
            {
            return EFalse;
            }
        }

    return ETrue;
    }


// ---------------------------------------------------------
// CCAFetchWaiter::LoggedIn()
// ---------------------------------------------------------
//
TBool CCAFetchWaiter::LoggedIn()
    {
    return UISessionManager().IsLoggedIn();
    }

// ---------------------------------------------------------
// CCAFetchWaiter::UISessionManager()
// ---------------------------------------------------------
//
CCAUISessionManager& CCAFetchWaiter::UISessionManager()
    {
    return static_cast<CCAAppUi*>
           ( CCoeEnv::Static()->AppUi() )->UISessionManager();
    }

// ---------------------------------------------------------
// CCAFetchWaiter::IsFetchDone()
// ---------------------------------------------------------
//
TBool CCAFetchWaiter::IsFetchDone()
    {
    CCACommandManager* commandManager( NULL );
    TRAPD( err, commandManager = CCACommandManagerFactory::InstanceL()->GetCommandManager() );
    if ( err != KErrNone )
        {
        return EFalse;
        }
    MCASessionHandlerCmd* sessHandlerCmd =  commandManager->GetSessionHandlerCmd() ;
    if ( sessHandlerCmd )
        {
        return sessHandlerCmd->IsFetchDone();
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CCAFetchWaiter::CheckFetchStateL()
// ---------------------------------------------------------
//
TBool CCAFetchWaiter::CheckFetchStateL()
    {
    TBool retVal = ETrue;

#ifndef IMPS_CONTACT_FETCH_BACKGROUND

    CCAFetchWaiter* waiter = NewLC();
    retVal = waiter->WaitForFetchCompleteL();
    CleanupStack::PopAndDestroy(); //waiter

#endif //IMPS_CONTACT_FETCH_BACKGROUND

    return retVal;
    }

// ---------------------------------------------------------
// CCAFetchWaiter::HandleFetchCompleteL()
// ---------------------------------------------------------
//
void CCAFetchWaiter::HandleFetchCompleteL( TInt aError )
    {
    if ( iWait.IsStarted() )
        {
        CHAT_DP_TXT( "CCAFetchWaiter::HandleFetchCompleteL - stop wait" );
        iWait.AsyncStop();
        iWaitNoteCanceled = ( aError == KErrNone ) ? EFalse : ETrue;
        }
    }

// ---------------------------------------------------------
// CCAFetchWaiter::NoteCanceled
// ---------------------------------------------------------
//
void CCAFetchWaiter::NoteCanceled( TInt /*aButtonId*/ )
    {
    if ( iWait.IsStarted() )
        {
        CHAT_DP_TXT( "CCAFetchWaiter::NoteCanceled - stop wait" );
        iWait.AsyncStop();
        iWaitNoteCanceled = ETrue;
        }
    }

// end of file
