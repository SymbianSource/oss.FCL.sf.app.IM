/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles title pane changes for service settings UI
*
*/


// INCLUDE FILES
#include <E32std.H>
#include <akntitle.h>
#include <aknlists.h>
#include <aknnavi.h>
#include "CWVSettingsUIDialogTitlePaneHandler.h"

// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
CWVSettingsUIDialogTitlePaneHandler::CWVSettingsUIDialogTitlePaneHandler()
    {
    }


// Destructor
CWVSettingsUIDialogTitlePaneHandler::~CWVSettingsUIDialogTitlePaneHandler()
    {
    delete iOldTitleText;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogTitlePaneHandler::SetTitlePaneTextL
// Set title pane text from given text
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogTitlePaneHandler::SetTitlePaneTextL( const TDesC& aTitleText ) const
    {
    CAknTitlePane* title = GetTitlePaneInstanceL();
    title->SetTextL( aTitleText );
    }


void CWVSettingsUIDialogTitlePaneHandler::SetTitlePaneDefaultTextL() const
    {
    CAknTitlePane* title = GetTitlePaneInstanceL();
    title->SetTextL( KNullDesC );
    }
// ---------------------------------------------------------
// CWVSettingsUIDialogTitlePaneHandler::GetTitlePaneInstanceL
// Gets titlepane instance from CEikonEnv
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAknTitlePane* CWVSettingsUIDialogTitlePaneHandler::GetTitlePaneInstanceL() const
    {
    CAknTitlePane* title = static_cast< CAknTitlePane* >
                           ( CEikonEnv::Static()->AppUiFactory()->StatusPane()->ControlL(
                                 TUid::Uid( EEikStatusPaneUidTitle ) ) );
    if ( !title )
        {
        User::Leave( KErrNotSupported );
        }
    return title;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogTitlePaneHandler::StoreTitlePaneTextL
// Stores current titlepanetext, so it can be later restored.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogTitlePaneHandler::StoreTitlePaneTextL()
    {
    CAknTitlePane* title = GetTitlePaneInstanceL();

    HBufC* tmp = title->Text()->AllocL();
    delete iOldTitleText;
    iOldTitleText = tmp;

    // Clear the navi pane
    // Get pointer to status-pane
    CEikStatusPane* statusPane = CEikonEnv::Static()->AppUiFactory()->
                                 StatusPane();
    // Get pointer to navi-pane
    CAknNavigationControlContainer* naviPane = static_cast<CAknNavigationControlContainer*>(
                                                   statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    naviPane->PushDefaultL( ETrue );
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogTitlePaneHandler::RestoreTitlePaneText
// Restores titlepanetext to previously stored one.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogTitlePaneHandler::RestoreTitlePaneText() const
    {
    // we can (and have to) ignore this error
    // no real harm done even if this fails
    TInt ignore;
    TRAP( ignore, DoRestoreTitlePaneTextL() );
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogTitlePaneHandler::DoRestoreTitlePaneTextL() const
// Does the actual restoring.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogTitlePaneHandler::DoRestoreTitlePaneTextL() const
    {
    CAknTitlePane* title = GetTitlePaneInstanceL();

    if ( iOldTitleText )
        {
        title->SetTextL( *iOldTitleText );
        }
    else
        {
        title->SetTextToDefaultL();
        }

    // restore the navi pane
    // Get pointer to status-pane
    CEikStatusPane* statusPane = CEikonEnv::Static()->AppUiFactory()->
                                 StatusPane();
    // Get pointer to navi-pane
    CAknNavigationControlContainer* naviPane = static_cast<CAknNavigationControlContainer*>(
                                                   statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    naviPane->Pop();
    }
//  End of File


