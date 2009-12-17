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
* Description:  Main view class
*
*/


// INCLUDE FILES
#include	"CCARefreshView.h"
#include    "ChatDebugPrint.h"
#include    "CCAEmptyControl.h"
#include    "CCAUISessionManager.h"
#include    "CAExternalInterface.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCARefreshView::~CCARefreshView
// Destructor
// -----------------------------------------------------------------------------
//
CCARefreshView::~CCARefreshView()
    {
    delete iContainer;
    }

// -----------------------------------------------------------------------------
// CCARefreshView::CCARefreshView
// Constructor
// -----------------------------------------------------------------------------
//
CCARefreshView::CCARefreshView( TInt& aRefreshFlag )
        : iRefreshFlag( aRefreshFlag )
    {
    }


// ---------------------------------------------------------
// CCARefreshView::DoActivateL(
//   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
//   const TDesC8& /*aCustomMessage*/)
// From CAknView, Gets called from framework when activating this view
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARefreshView::DoActivateL(
    const TVwsViewId& aPrevViewId, TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    CHAT_DP_TXT( "CCARefreshView::DoActivateL" );
    iRefreshFlag = ETrue;

    iContainer = CCAEmptyControl::NewL( ClientRect() );
    iContainer->SetMopParent( this );

    AppUi()->AddToViewStackL( *this, iContainer );

    CAAppUi()->CaptureEventsL();

    TUid id;

    if ( aPrevViewId.iAppUid != CAAppUi()->Application()->AppDllUid() )
        {
        id = KUidFriendsListView;
        }
    else
        {
        id = CAAppUi()->AfterRefreshViewId( aPrevViewId.iViewUid );
        }

    CAViewSwitcher()->SwitchViewL( id );
    }

// ---------------------------------------------------------
// CCARefreshView::DoDeactivate()
// From AknView, Gets called from framework when deactivating this view
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARefreshView::DoDeactivate()
    {
    CHAT_DP_TXT( "CCARefreshView::DoDeactivate" );
    iRefreshFlag = EFalse;

    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    iContainer = NULL;

    TRAPD( err, CAAppUi()->ReleaseCapturingL() );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }


// End of File

