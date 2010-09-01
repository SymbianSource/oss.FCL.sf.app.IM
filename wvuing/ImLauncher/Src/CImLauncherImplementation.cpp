/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ImLauncher plugin implementation
*
*/



// INCLUDE FILES
#include "CImLauncherImplementation.h"
#include "ChatDebugPrint.h"
#include "CCAAudioManager.h"
#include "iminternalpskeys.h"

#include <e32base.h>
#include <AknSoftNotifier.h>

#include <e32property.h>

#include "impsbuilddefinitions.h"
#ifndef RD_30_COMPATIBILITY_MODE
#include    <coreapplicationuisdomainpskeys.h>
#else
#include    <coreapplicationuisinternalpskeys.h>
#endif //RD_30_COMPATIBILITY_MODE


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImLauncherImplementation::CImLauncherImplementation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImLauncherImplementation::CImLauncherImplementation()
        : iStatus( NULL ),
        iRequestErr( KErrNone )
    {
    }

// -----------------------------------------------------------------------------
// CImLauncherImplementation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImLauncherImplementation::ConstructL()
    {
    CHAT_DP_FUNC_ENTER( "ConstructL" );
    iConnUI = CIMPSPresenceConnectionUi::NewL( EIMPSConnClientIM );
    iAudioManager = CCAAudioManager::NewL( *iConnUI, ETrue );
    iAudioManager->AddObserverL( this );
    CHAT_DP_FUNC_DONE( "ConstructL" );
    }

// -----------------------------------------------------------------------------
// CImLauncherImplementation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImLauncherImplementation* CImLauncherImplementation::NewL()
    {
    CImLauncherImplementation* self = new( ELeave ) CImLauncherImplementation;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CImLauncherImplementation::~CImLauncherImplementation()
    {
    CHAT_DP_FUNC_ENTER( "~CImLauncherImplementation" );
    delete iAudioManager;
    delete iConnUI;
    CHAT_DP_FUNC_DONE( "~CImLauncherImplementation" );
    }


// -----------------------------------------------------------------------------
// CImLauncherImplementation::StartApplicationL( TRequestStatus& aStatus )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImLauncherImplementation::StartApplicationL( TRequestStatus& aStatus,
                                                   const TDesC& /* aSap */, const TDesC& /*a UserId */ )
    {
    CHAT_DP_FUNC_ENTER( "StartApplicationL" );
    iStatus = &aStatus;
    iRequestErr = KErrNone;

    // Show IM message indicator
    SetIMUIPIconL();

    // Add unread message count for IM message indicator
    AddMessageCounterL();

    // display soft notification for new chat messages (invitation)
    TRAP( iRequestErr, ShowSoftNotificationL() );

    CHAT_DP( D_CHAT_LIT( "StartApplicationL - softnotifier left with: %d" ),
             iRequestErr );

    if ( iRequestErr )
        {
        CActiveScheduler::Current()->Error( iRequestErr );
        }

    iAudioManager->PlayL();

    CHAT_DP_FUNC_DONE( "StartApplicationL" );
    }

// -----------------------------------------------------------------------------
// CImLauncherImplementation::CancelStartApplication()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImLauncherImplementation::CancelStartApplication()
    {
    // Do nothing as we don't use the active scheduler
    }

// -----------------------------------------------------------------------------
// CImLauncherImplementation::PlayCompleted()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImLauncherImplementation::PlayCompleted()
    {
    CHAT_DP_FUNC_ENTER( "PlayCompleted" );

    if ( iStatus )
        {
        CHAT_DP_TXT( "PlayCompleted - iStatus good" );
        if ( iStatus->Int() == KRequestPending )
            {
            CHAT_DP_TXT( "PlayCompleted - iStatus was KRequestPending, completing request" );
            User::RequestComplete( iStatus, iRequestErr );
            }
        }
    }

// -----------------------------------------------------------------------------
// CImLauncherImplementation::SetIMUIPIcon()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImLauncherImplementation::SetIMUIPIconL()
    {
    CHAT_DP_FUNC_ENTER( "SetIMUIPICon" );

    TInt err = RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsUipInd,
                               ECoreAppUIsShow );

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    CHAT_DP_FUNC_DONE( "SetIMUIPICon" );
    }

// -----------------------------------------------------------------------------
// CImLauncherImplementation::ShowSoftNotificationL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImLauncherImplementation::ShowSoftNotificationL()
    {
    CAknSoftNotifier* softNotifier = CAknSoftNotifier::NewLC();
    softNotifier->AddNotificationL( EChatMessageNotification );
    CleanupStack::PopAndDestroy( softNotifier );
    }

// -----------------------------------------------------------------------------
// CImLauncherImplementation::AddMessageCounterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImLauncherImplementation::AddMessageCounterL()
    {
    // Update unread message count to PS
    // Value is used by the ImIndicatorPlugin
    TInt err = RProperty::Define(
                   KPSUidIMUI, KIMUnreadMsgKey, RProperty::EInt,
                   KIMUnreadMsgReadPolicy, KIMUnreadMsgWritePolicy  );
    TInt value;
    err = RProperty::Get( KPSUidIMUI, KIMUnreadMsgKey, value );
    if ( err == KErrNone )
        {
        err = RProperty::Set( KPSUidIMUI, KIMUnreadMsgKey, ++value );
        }
    }

//  End of File
