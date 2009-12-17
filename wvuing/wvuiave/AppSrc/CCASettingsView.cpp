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
* Description:  View class for settings view
*
*/

// INCLUDE FILES
#include	"chatngclient.hrh"
#include	"CCASettingsView.h"
#include	"CCAAppUi.h"
#include	"CCAStatusPaneHandler.h"
#include	"CAExternalInterface.h"
#include	"CCASettingsViewContainer.h"
#include	"CCAAppSettingsDialog.h"
#include    "CAExternalInterface.h"
#include	"ChatDefinitions.h"
#include    "ChatDebugPrint.h"

#include	<aknviewappui.h>
#include	<chatNG.rsg>
#include    <bldvariant.hrh>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

// ================= MEMBER FUNCTIONS =======================

// Symbian OS default constructor can leave.
void CCASettingsView::ConstructL( TInt aAvkonViewResourceId,
                                  TUid aViewId )
    {
    BaseConstructL( aAvkonViewResourceId, aViewId );

    }

// Destructor
CCASettingsView::~CCASettingsView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    }

// -----------------------------------------------------------------------------
// CCASettingsView::DoActivateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASettingsView::DoActivateL( const TVwsViewId& aPrevViewId,
                                   TUid /*aCustomMessageId*/,
                                   const TDesC8& /*aCustomMessage*/ )
    {
    CHAT_DP_TXT( "CCASettingsView::DoActivateL" );

    // To prevent looping between this view and refresh view
    if ( aPrevViewId.iViewUid != KUidRefreshView )
        {
        iPrevViewId = aPrevViewId;
        }



    iContainer = new ( ELeave ) CCASettingsViewContainer(
        *( static_cast<MCAViewSwitcher*>( CAAppUi() ) ),
        *( CAAppUi()->GetProcessManager().GetSettingsInterface( ) ) //( CAModel().GetInternalSettingsInterface() )
    );
    iContainer->SetMopParent( this );
    iContainer->ConstructL( ClientRect() );

    AppUi()->AddToViewStackL( *this, iContainer );
    CAAppUi()->CAStatusPane()->ClearNaviPaneL();

    CAAppUi()->CAStatusPane()->SetTitleL( iEikonEnv, R_CHATCLIEN_SETTINGS_VIEW_TITLE );

    if ( aPrevViewId.iViewUid == KUidRefreshView )
        {
        iContainer->Listbox()->SetCurrentItemIndexAndDraw(
            CCASettingsViewContainer::EServiceSettings );
        }

#ifdef RD_SERVICE_ENHANCEMENTS
    if ( iServSettingsDialogWasActive )
        {
        iContainer->LaunchServSettingsDialogL( ETrue );
        }
#endif  // RD_SERVICE_ENHANCEMENTS
    }

// -----------------------------------------------------------------------------
// CCASettingsView::DoDeactivate
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASettingsView::DoDeactivate()
    {
    CHAT_DP_TXT( "CCASettingsView::DoDeactivate" );

    if ( iContainer )
        {
#ifdef RD_SERVICE_ENHANCEMENTS
        iServSettingsDialogWasActive = iContainer->IsServSettingsDialogActive();
#endif  // RD_SERVICE_ENHANCEMENTS

        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    iContainer = NULL;
    }

// -----------------------------------------------------------------------------
// CCASettingsView::HandleCommandL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASettingsView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack: // Flow through
        case EChatClientCmdBack:
            {
            CAAppUi()->SwitchViewL( iPrevViewId.iViewUid );
            break;
            }
        case EAknSoftkeyOpen:   // Flowthrough, MSK command
        case EChatClientSettingsOpen:
            {
            TInt index( iContainer->Listbox()->CurrentItemIndex() );

            if ( index == CCASettingsViewContainer::EApplicationSettings )
                {
                iContainer->LaunchAppSettingsDialogL();
                }

            if ( index == CCASettingsViewContainer::EServiceSettings )
                {
                iContainer->LaunchServSettingsDialogL();
                }
            break;
            }

        case EChatClientCmdHelp:
            {
            LaunchHelpL();
            break;
            }

        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCASettingsView::DynInitMenuPaneL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASettingsView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    CCAView::DynInitMenuPaneL( aResourceId, aMenuPane );
    }

//  End of File
