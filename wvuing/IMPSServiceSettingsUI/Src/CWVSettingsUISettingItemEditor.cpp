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
* Description:  Setting item editor
*
*/


// INCLUDE FILES
#include "CWVSettingsUISettingItemEditor.h"
#include "CWVSettingsUIDialogImp.h"
#include "WVSettingsUIPanics.h"
#include "CWVSettingsUIDefs.h"
#include "WVSettingsSharedDataNG.h"
#include "CWVSettingsUICheckboxSettingPage.h"

#include	<cwvsettingsUING.rsg>
#include	<aknsettingitemlist.h>
#include	<aknsettingpage.h>
#include	<cimpssapsettings.h>
#include	<cimpssapsettingsstore.h>
#include	<stringloader.h>
#include	<aknnotewrappers.h>
#include	<AknCheckBoxSettingPage.h>
#include	<aknpopupsettingpage.h>

#include    <cimpspresenceconnectionuing.h>
#include    <cpengnwsessionslotid2.h>

// Defines whether first login to server has been done
_LIT( KIMFirstLoginToServer, "IMFirstLoginToServer" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWVSettingsUISettingItemEditor::EditSettingItemL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUISettingItemEditor::EditSettingItemL( TBool aIsPopUp,
                                                       TInt aListBoxIndex,
                                                       CAknSettingItemArray& aSettingItemList,
                                                       CIMPSSAPSettingsStore& aSAPSettingsStore,
                                                       TInt& aIMLoginType,
                                                       TBool& aIMLoginTypeEdited,
                                                       CSelectionItemList* aSelectedDaysIM,
                                                       TInt& aConnectHoursIMInt,
                                                       TBool& aIMConnHoursEdited,
                                                       TBool& aCancelled,
                                                       TInt aSettingItem,
                                                       CIMPSPresenceConnectionUi* aConnectionUI, /*= NULL */
                                                       TBool* aForcedLoginMade /*= NULL */ )
    {
    iSettingItemArray = &aSettingItemList;
    iSAPSettingsStore = &aSAPSettingsStore;

    if ( aConnectionUI )
        {
        iConnectionUI = aConnectionUI;
        }
    if ( aForcedLoginMade )
        {
        iForcedLoginMade = aForcedLoginMade;
        }

    switch ( aSettingItem )
        {
        case CWVSettingsUIDialogImp::EWVSettingsIMLoginType:
            {
            EditLoginTypeL( aIsPopUp,
                            aListBoxIndex,
                            aSettingItemList,
                            aSAPSettingsStore,
                            aIMLoginType,
                            aIMLoginTypeEdited,
                            aSelectedDaysIM,
                            aConnectHoursIMInt,
                            aIMConnHoursEdited,
                            aCancelled,
                            aSettingItem );
            break;
            }
        case CWVSettingsUIDialogImp::EWVSettingsIMConnDays:
            {
            // this is IM Connection days
            EditConnectedDaysL( aSelectedDaysIM, aCancelled );
            break;
            }
        case CWVSettingsUIDialogImp::EWVSettingsIMConnHours:
            {
            // this is IM connection hours
            EditConnectedHoursL( aConnectHoursIMInt, aCancelled );
            aIMConnHoursEdited = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }

    iSettingItemArray->At( aListBoxIndex )->StoreL();
    }

// ---------------------------------------------------------
// CWVSettingsUISettingItemEditor::ShowMissingInfoErrorNoteL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CWVSettingsUISettingItemEditor::ShowMissingInfoErrorNoteL()
    {
    // load the note text
    HBufC* prompt = StringLoader::LoadLC( R_WVSETTINGSVIEW_ERROR_INFORMATION_MISSING );
    // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
    // PrepareLC of the dialog pushes the dialog into cleanupstack
    CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog( // CSI: 35 # See above
        CAknNoteDialog::EErrorTone , CAknNoteDialog::ELongTimeout );
    dlg->PrepareLC( R_WVSETTINGSVIEW_ERROR_NOTE_TEMPLATE );
    dlg->SetTextL( *prompt );
    // run the note
    dlg->RunLD();
    CleanupStack::PopAndDestroy( prompt );
    }


// ---------------------------------------------------------
// CWVSettingsUISettingItemEditor::EditConnectedDaysL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUISettingItemEditor::EditConnectedDaysL( CSelectionItemList* aSelectionItemList,
                                                         TBool& aCancelled )
    {
    if ( aSelectionItemList )
        {
        CWVSettingsUICheckboxSettingPage* dlg = CWVSettingsUICheckboxSettingPage::NewL(
                                                    R_WVSETTINGSVIEW_SET_SCHEDULING, aSelectionItemList );
        aCancelled = !dlg->ExecuteLD();
        }
    }

// ---------------------------------------------------------
// CWVSettingsUISettingItemEditor::EditConnectedHoursL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUISettingItemEditor::EditConnectedHoursL( TInt& aConnectHoursInt,
                                                          TBool& aCancelled )
    {
    aCancelled = EFalse;
    aConnectHoursInt = CWVSettingsUIDialogImp::EWVSettingsBetweenHours;
    }


// ---------------------------------------------------------
// CWVSettingsUISettingItemEditor::HandleSettingPageEventL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUISettingItemEditor::HandleSettingPageEventL( CAknSettingPage* /*aSettingPage*/,
                                                              TAknSettingPageEvent /*aEventType*/ )
    {
    // no need to do anything here
    }


// ---------------------------------------------------------
// CWVSettingsUISettingItemEditor::EditLoginTypeL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUISettingItemEditor::EditLoginTypeL( TBool aIsPopUp,
                                                     TInt aListBoxIndex,
                                                     CAknSettingItemArray& aSettingItemList,
                                                     CIMPSSAPSettingsStore& aSAPSettingsStore,
                                                     TInt& aIMLoginType,
                                                     TBool& aIMLoginTypeEdited,
                                                     CSelectionItemList* aSelectedDaysIM,
                                                     TInt& aConnectHoursIMInt,
                                                     TBool& aIMConnHoursEdited,
                                                     TBool& aCancelled,
                                                     TInt aSettingItem )
    {
    HBufC* userId = NULL;
    HBufC* password = NULL;
    CIMPSSAPSettings* sapSettings = CIMPSSAPSettings::NewLC();

    TRAPD( err, iSAPSettingsStore->GetDefaultL( sapSettings, EIMPSIMAccessGroup ) );

    if ( err == KErrNone )
        {
        userId = sapSettings->SAPUserId().AllocLC();
        password = sapSettings->SAPUserPassword().AllocLC();
        }
    else if ( err == KErrNotFound )
        {
        // we do not want to leave if there is no default SAP
        }
    else
        {
        User::Leave( err );
        }

    TInt oldIMLoginType = aIMLoginType;
    iSettingItemArray->At( aListBoxIndex )->EditItemL( aIsPopUp );
    iSettingItemArray->At( aListBoxIndex )->StoreL();
    aIMLoginTypeEdited = ETrue;

    // check if the login type is automatic
    if ( ( aIMLoginType == EWVSettingsChatLoginAutoAlways ) ||
         ( aIMLoginType == EWVSettingsChatLoginAutoInHomeNW ) ||
         ( aIMLoginType == EWVSettingsChatLoginApplicationLaunch ) )
        {
        // check if the userid and password are defined
        if ( !userId || !password )
            {
            // show a note about the missing items return to editing
            aIMLoginType = oldIMLoginType;
            iSettingItemArray->At( aListBoxIndex )->LoadL();
            ShowMissingInfoErrorNoteL();

            EditSettingItemL ( aIsPopUp,
                               aListBoxIndex,
                               aSettingItemList,
                               aSAPSettingsStore,
                               aIMLoginType,
                               aIMLoginTypeEdited,
                               aSelectedDaysIM,
                               aConnectHoursIMInt,
                               aIMConnHoursEdited,
                               aCancelled,
                               aSettingItem );
            }
        // check that user id has some data
        else
            {
            TBool errorShown( EFalse );
            if ( userId )
                {
                if ( 0 == userId->Length() )
                    {
                    // show a note about the missing items return to editing
                    aIMLoginType = oldIMLoginType;
                    iSettingItemArray->At( aListBoxIndex )->LoadL();
                    ShowMissingInfoErrorNoteL();
                    errorShown = ETrue;

                    EditSettingItemL ( aIsPopUp,
                                       aListBoxIndex,
                                       aSettingItemList,
                                       aSAPSettingsStore,
                                       aIMLoginType,
                                       aIMLoginTypeEdited,
                                       aSelectedDaysIM,
                                       aConnectHoursIMInt,
                                       aIMConnHoursEdited,
                                       aCancelled,
                                       aSettingItem );
                    }
                }
            // check that password has some data
            if ( password )
                {
                if ( !errorShown )
                    {
                    // no need to check for password if user id already failed
                    if ( 0 == password->Length() )
                        {
                        // show a note about the missing items return to editing
                        aIMLoginType = oldIMLoginType;
                        iSettingItemArray->At( aListBoxIndex )->LoadL();
                        ShowMissingInfoErrorNoteL();
                        errorShown = ETrue;

                        EditSettingItemL ( aIsPopUp,
                                           aListBoxIndex,
                                           aSettingItemList,
                                           aSAPSettingsStore,
                                           aIMLoginType,
                                           aIMLoginTypeEdited,
                                           aSelectedDaysIM,
                                           aConnectHoursIMInt,
                                           aIMConnHoursEdited,
                                           aCancelled,
                                           aSettingItem );
                        }
                    }
                }

            // UserId and password ok, force login before activating
            // automatic login type if user has not logged in
            // to current server before
            if ( !errorShown )
                {
                if ( !ForceLoginL( *sapSettings ) )
                    {
                    // Query not accepted or login failed, restore old login type
                    aIMLoginType = oldIMLoginType;
                    iSettingItemArray->At( aListBoxIndex )->LoadL();

                    EditSettingItemL ( aIsPopUp,
                                       aListBoxIndex,
                                       aSettingItemList,
                                       aSAPSettingsStore,
                                       aIMLoginType,
                                       aIMLoginTypeEdited,
                                       aSelectedDaysIM,
                                       aConnectHoursIMInt,
                                       aIMConnHoursEdited,
                                       aCancelled,
                                       aSettingItem );
                    }
                }
            }
        }

    if ( password )
        {
        CleanupStack::PopAndDestroy( password ); // password
        }
    if ( userId )
        {
        CleanupStack::PopAndDestroy( userId ); //
        }
    CleanupStack::PopAndDestroy( sapSettings ); // sapSettings
    }

// -----------------------------------------------------------------------------
// CWVSettingsUISettingItemEditor::ForceLoginL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUISettingItemEditor::ForceLoginL( CIMPSSAPSettings& aSapSettings )
    {
    TInt firstLogin = 0;
    TInt sapError = -1;
    sapError = aSapSettings.GetOpaqueInt( KIMFirstLoginToServer, firstLogin );

    if ( sapError != KErrNotFound )
        {
        User::LeaveIfError( sapError );
        }

    // First login
    if ( firstLogin == 0 )
        {
        // Init to EFalse
        *iForcedLoginMade = EFalse;

        // Ask confirmation about logging out
        CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog(
            CAknQueryDialog::ENoTone );
        CleanupStack::PushL( dlg );

        HBufC* text = StringLoader::LoadLC(
                          R_WVSETTINGSVIEW_CANNOT_ACTIVATE_QUERY,
                          aSapSettings.SAPName(),
                          CEikonEnv::Static() );

        dlg->SetPromptL( *text );
        CleanupStack::PopAndDestroy( text );
        CleanupStack::Pop( dlg );

        TInt ret = dlg->ExecuteLD( R_FORCE_LOGIN_CONFIRMATION_QUERY );

        if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
            {
            CPEngNWSessionSlotID2* sessionSlotID =
                CPEngNWSessionSlotID2::NewL();
            CleanupStack::PushL( sessionSlotID );

            TInt loginError = iConnectionUI->LoginL( EIMPSConnClientIM,
                                                     EIMPSApplicationLaunch,
                                                     *sessionSlotID );

            CleanupStack::PopAndDestroy( sessionSlotID );

            // Login failed
            if ( loginError != KErrNone )
                {
                return EFalse;
                }

            *iForcedLoginMade = ETrue;
            }
        else
            {
            // User cancelled force login query
            return EFalse;
            }
        }
    else
        {
        *iForcedLoginMade = EFalse;
        }

    // Query accepted, login completed
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUISettingItemEditor::CWVSettingsUISettingItemEditor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWVSettingsUISettingItemEditor::CWVSettingsUISettingItemEditor()
    {
    }

// Destructor
CWVSettingsUISettingItemEditor::~CWVSettingsUISettingItemEditor()
    {
    }

//  End of File
