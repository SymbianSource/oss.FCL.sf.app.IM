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
* Description:  Contact editor / viewer
*  Interface   : Private
*
*/


// INCLUDE FILES

#include	"chatngclient.hrh"			// application resources
#include	"CCAContactEditor.h"		// header for this .cpp...
#include	"CCAStatusPaneHandler.h"	// For title pane changes
#include	"ChatDebugPrint.h"
#include    "CAExternalInterface.h"
#include    "IMNoteMapper.h"
#include    "IMDialogUtils.h"
#include    "chatdefinitions.h"
#include    "CCAAppUi.h"
#include    "CCAEngine.h"
#include    "CCAUISessionManager.h"
#include    "ImpsCSPAllErrors.h"
#include    "CCAUINGUtils.h"
#include    "CAUtils.h"
#include    "ccasyncchecker.h"
#include    "ccafetchwaiter.h"
#include    "MCAError.h"
#include	"MCASettingsPC.h"
#include	"TEnumsPC.h"
#include	"IMUtils.h"
#include    "CCAPCUtils.h"


#include	<eikcapc.h>				    // CEikCaptionedControl
#include	<eikbtgpc.h>				// CEikButtonGroupContainer
#include	<eikedwin.h>				// Edwin
#include	<eikmenup.h>				// menu pane handling
#include	<ChatNG.rsg>					// application resources
#include	<aknnotewrappers.h>
#include	<aknquerydialog.h>			// AVKON's query dialog
#include	<StringLoader.h>            // StringLoader
#include    <hlplch.h>
#include    <csxhelp/imng.hlp.hrh>
#include    <featmgr.h>
#include    <cntdb.h>
#include    <EscapeUtils.h>

#include    <akndef.h>

#include 	"CCACommandManagerFactory.h"
#include 	"CCACommandManager.h"
#include 	"CCAContactEditCmd.h"
#include 	"MCAContactEditPC.h"
#include 	"MCAProcessManager.h"
#include	"MCABlockingPC.h"

#include    "imnginternalcrkeys.h"
#include    "IMPrivateCRKeys.h"
#include    "IMVariant.hrh"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

// CONSTANTS
const TInt KAlphaCodeA = 0x61;
const TInt KAlphaCodeZ = 0x7A;
const TInt KNumCode0 = 0x2F;
const TInt KNumCode9 = 0x39;


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAContactEditor::CCAContactEditor(
    TBool& aSaved,
    const TBool aContactPCInitialise )
        :
        iAllFieldsEmpty( ETrue ),
        iContactWasSaved( aSaved ),
        iOwnStatus( EFalse ),
        iInitialFocus( EChatEditWV ),
        iContactEditPCInitialised( aContactPCInitialise )/*,
	iNewContactID(aNewContactID),
    iNewListID(aNewContactID)*/
    {
    iContactWasSaved = EFalse;
    }


// Destructor
CCAContactEditor::~CCAContactEditor()
    {
    // reset the iContact and iOwnData
    //only release resources if this has been initialised within contact editor
    if ( iContactEditPCInitialised && iCCAContactEditPC )
        {
        iCCAContactEditPC->ReleaseResources();
        }


    delete iUserId;
    delete iOrigId;
    delete iAlias;
    delete iNick;
    delete iListId;
    delete iIdentification;
    delete iCCAContactEditCmd;
    delete iNewUserId;

    }


// ---------------------------------------------------------
// CCAContactEditor::NewL
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCAContactEditor* CCAContactEditor::NewL(
    TInt aIndex,
    TBool& aSaved,
    const TBool aContactPCInitialise /*= ETrue*/ )
    {
    CCAContactEditor* temp = CCAContactEditor::NewLC( aIndex, aSaved, aContactPCInitialise );
    CleanupStack::Pop( temp );
    return temp;
    }




// ---------------------------------------------------------
// CCAContactEditor::NewLC
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCAContactEditor* CCAContactEditor::NewLC(
    TInt aIndex,
    TBool& aSaved,
    const TBool aContactPCInitialise /*= ETrue*/ )
    {

    CCAContactEditor* temp = new( ELeave )CCAContactEditor( aSaved, aContactPCInitialise );
    CleanupStack::PushL( temp );
    temp->ConstructL( aIndex );

    return temp;
    }

// ---------------------------------------------------------
// CCAContactEditor::ConstructL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::ConstructL( TInt aIndex )
    {
    CAknForm::ConstructL( R_CHAT_CEDITOR_MENUBAR );  // superclass

    // finalize construction
    iAliasEnabled = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVE_ALIAS );
    iNicknameEnabled = IMUtils::IntResourceValueL(
                           RSC_CHAT_VARIATION_HAVE_NICKNAME );
    iTextualInputMode = IMUtils::IntResourceValueL(
                            RSC_CHAT_VARIATION_USERID_TEXTUAL_INPUTMODE );
    iUserIDNotEditable = IMUtils::IntResourceValueL(
                             RSC_CHAT_VARIATION_UNEDITABLE_USERID );

    iAppUI = static_cast <CCAAppUi*> ( CCoeEnv::Static()->AppUi() );


    iCCAContactEditPC = iAppUI->GetProcessManager().GetContactEditInterface();

    //only initialise if iContactEditPCInitialised = ETrue
    //also initialise only if we have a valid index
    if ( ( -1 != aIndex ) && ( iContactEditPCInitialised ) )
        {
        iCCAContactEditPC->InitialiseL(
            *iAppUI->GetProcessManager().GetArrayInterface(),
            aIndex, iOwnStatus );
        }

    if ( !iContactEditPCInitialised )
        {
        iOwnStatus = iCCAContactEditPC->IsOwnData();
        }

    MCACommandFactory* commandManager =  CCACommandManagerFactory::InstanceL()->GetCommandManager();


    iCCAContactEditCmd = static_cast< CCAContactEditCmd* > ( commandManager->NewCommandL( EChatClientDetailsEdit, iAppUI->GetProcessManager(), *this ) );


    }

// ---------------------------------------------------------
// CCAContactEditor::EditContactLD
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAContactEditor::EditContactLD( TEnumsPC::TContactEditorMode aCreateNew,
                                      const TDesC& aListId,
                                      const TDesC& aWVID /* = KNullDesC */,
                                      const TDesC& aNickName /* = KNullDesC */,
                                      TBool aLoadFromStore /* = ETrue */,
                                      TBool /*aFetchAlias  = ETrue */,
                                      TBool aCursorToBeginning /* = EFalse */ )
    {
    TInt retval = 0;

    CleanupStack::PushL( this );
    TPtrC dummy( KNullDesC() );

    CCAStatusPaneHandler& titlePane( *iAppUI->CAStatusPane() );


    // Check disk space before editing
    IMUtils::FFSSpaceBelowCriticalLevelL( KContactSizeEstimate );

    switch ( aCreateNew )
        {
        case TEnumsPC::EAddToContact:
            {
            iInternalMode = TEnumsPC::EAddToContact;
            iWVUID = &aWVID;
            iNickName = &aNickName;
            break;
            }
        case TEnumsPC::EEditContact:
            {
            iInternalMode = TEnumsPC::EEditContact;
            iWVUID = &aWVID;
            iNickName = &aNickName;
            break;
            }
        case TEnumsPC::ECreateNew:
            {
            iInternalMode = TEnumsPC::EAddToContact;
            iWVUID = &dummy;
            iNickName = &dummy;
            break;
            }
        default:
            {
            iInternalMode = TEnumsPC::EUnknownMode;
            }
        }

    // set the mode and get default values for user id and nick name

    if ( ( !aCreateNew ) && ( aLoadFromStore ) && ( !( iCCAContactEditPC->IsContact() ) ) )
        {
        if ( ( aWVID.Length() != 0 ) && ( aListId.Length() != 0 ) )
            {
            iCCAContactEditPC->InitialiseL( aWVID, aListId, iOwnStatus );
            //cross verification
            if ( !iCCAContactEditPC->IsContact() )
                {
                //some serious issue dont open contact editor
                CleanupStack::PopAndDestroy( this );
                return retval;
                }
            }
        }

    if ( !( ( iCCAContactEditPC->IsContact() ) && ( aLoadFromStore ) ) )
        {
        if ( aWVID.Length() != 0 )
            {
            iUserId = aWVID.AllocL();
            }
        }


    iCursorToBeginning = aCursorToBeginning;

    if ( ( aListId.Length() == 0 ) && ( !iOwnStatus ) )
        {
        // and go to contact list creation
        iListId = GetListIdL( aWVID, 0 );
        if ( !iListId )
            {
            CleanupStack::PopAndDestroy( this );
            return retval;
            }
        }
    else
        {
        iListId = aListId.AllocL();
        }

    // We have to use stacking decorators, because we can have stacked dialogs.
    // REMEMBER to pop out the pushed decorator
    titlePane.ClearNaviPaneL( ETrue );

    // execute the actual form
    CleanupStack::Pop( this );

    retval = ExecuteLD( R_CHAT_CONTACT_EDITOR_DLG );

    // Restore navipane(show tabs id available)
    // fix for when add to im contacts is called from chatView,ConversationView
    // doesnt affect the rest
    titlePane.PopNaviPane();

    return retval;

    }

// ---------------------------------------------------------
// CCAContactEditor::EditContactLD
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAContactEditor::EditContactLD(
    TInt aInitialFocus,
    TEnumsPC::TContactEditorMode aCreateNew,
    const TDesC& aListId )
    {
    iInitialFocus = aInitialFocus;
    TInt retval = EditContactLD( aCreateNew, aListId );
    return retval;
    }

// ---------------------------------------------------------
// CCAContactEditor::LoadFormDataL
// Loads all data displayed in form
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::LoadFormDataL()
    {

    if ( iInternalMode == TEnumsPC::EAddToContact )
        {
        // adding new contact, fetch the default values if they exist
        if ( iWVUID )
            {

            // get a copy of original id
            delete iOrigId;
            iOrigId = NULL;
            iOrigId = iWVUID->AllocL();

            TPtrC displayId( CCAAppUi::DisplayId( *iOrigId ) );
            GetFormEdwin( EChatEditWV )->SetTextL( &displayId );

            // set numeric as default, if requested
            GetFormEdwin( EChatEditWV )->SetAknEditorInputMode(
                iTextualInputMode ?
                EAknEditorTextInputMode :
                EAknEditorNumericInputMode );

            if ( iCursorToBeginning )
                {
                GetFormEdwin( EChatEditWV )->AddFlagToUserFlags(
                    CEikEdwin::ENoAutoSelection );
                }

            SetChangesPending( ETrue );
            }

        if ( iNicknameEnabled )
            {
            if ( iNickName )
                {
                GetFormEdwin( EChatEditNick )->SetTextL( iNickName );
                SetChangesPending( ETrue );
                }
            }

        if ( iAliasEnabled )
            {

            delete iAlias;

            //iAlias is assigned NULL as GetAliasFromPresenceL may leave
            iAlias = NULL;
            iAlias = iCCAContactEditPC->GetAliasFromPresenceL( *iWVUID );

            // set the alias field as read-only, as we are now editing
            if ( iAlias )
                {
                GetFormEdwin( EChatEditAlias )->SetTextL( iAlias );
                }
            GetFormEdwin( EChatEditAlias )->SetReadOnly( ETrue );
            GetFormEdwin( EChatEditAlias )->
            AddFlagToUserFlags( CEikEdwin::EAvkonDisableCursor |
                                CEikEdwin::ENoAutoSelection );
            SetChangesPending( ETrue );
            }

        HideEmptyFieldsL();
        }
    else
        {

        iContactEditOperation = MCAUiContactEditCmdCB::TContactEditParams::ELoadContactDetails;

        iCCAContactEditCmd->ExecuteCommandL();


        // viewing or editing
        if ( iUserId )
            {

            // editing or viewing existing contact, load nick name and wvid
                {

                TPtrC displayId( CCAAppUi::DisplayId( *iOrigId ) );
                GetFormEdwin( EChatEditWV )->SetTextL( &displayId );
                // Make the text not highlighted and cursor at the end
                GetFormEdwin( EChatEditWV )->
                AddFlagToUserFlags( CEikEdwin::ENoAutoSelection |
                                    CEikEdwin::EJustAutoCurEnd );

                if ( iUserIDNotEditable || iOwnStatus )
                    {
                    // we can't edit WVID of existing contact.
                    GetFormEdwin( EChatEditWV )->SetReadOnly( ETrue );
                    GetFormEdwin( EChatEditWV )->
                    AddFlagToUserFlags( CEikEdwin::EAvkonDisableCursor |
                                        CEikEdwin::ENoAutoSelection );
                    }

                // set numeric as default, if requested
                GetFormEdwin( EChatEditWV )->SetAknEditorInputMode(
                    iTextualInputMode ?
                    EAknEditorTextInputMode :
                    EAknEditorNumericInputMode );

                // alias
                if ( ! iAliasEnabled )
                    {
                    // no alias
                    // 1. hide the alias entry completely
                    DeleteLine( EChatEditAlias, EFalse );
                    }
                else
                    {
                    // alias
                    // 1. put alias to alias field
                    // 2. make alias field not editable (in OfferKeyEventL display a note)
                    if ( iAlias )
                        {
                        TPtrC aliasPtr( *iAlias );
                        GetFormEdwin( EChatEditAlias )->SetTextL( &aliasPtr );

                        // User can only edit his/her own alias
                        if ( !iOwnStatus )
                            {
                            GetFormEdwin( EChatEditAlias )->SetReadOnly( ETrue );
                            GetFormEdwin( EChatEditAlias )->
                            AddFlagToUserFlags( CEikEdwin::EAvkonDisableCursor |
                                                CEikEdwin::ENoAutoSelection );
                            }
                        }
                    }
                // nickname
                if ( iNicknameEnabled )
                    {
                    // nickname
                    if ( iNick )
                        {
                        GetFormEdwin( EChatEditNick )->SetTextL( iNick );
                        GetFormEdwin( EChatEditNick )->
                        AddFlagToUserFlags( CEikEdwin::ENoAutoSelection |
                                            CEikEdwin::EJustAutoCurEnd );
                        }
                    }
                }

            HideEmptyFieldsL();
            }

        }

    // Hide the nick if it is not enabled
    if ( !iNicknameEnabled || iOwnStatus )
        {
        DeleteLine( EChatEditNick, EFalse );
        }

    UpdateTitleBarL();
    }

// ---------------------------------------------------------
// CCAContactEditor::GetFormEdwin
// Fetches handler to edwin control pointed by aId
// (other items were commented in a header).
// ---------------------------------------------------------
//
CEikEdwin* CCAContactEditor::GetFormEdwin( TInt aId )
    {
    return static_cast<CEikEdwin*>( Control( aId ) ) ;
    }

// ---------------------------------------------------------
// CCAContactEditor::GetEdwinTextBufferL
// Copies text to buffer and returns handle to it
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CCAContactEditor::GetEdwinTextBufferL( TInt aId )
    {
    return GetFormEdwin( aId )->GetTextInHBufL();
    }

// ---------------------------------------------------------
// CCAContactEditor::GetEdwinTextBufferLC
// Copies edwin's text to buffer and returns handle to it
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CCAContactEditor::GetEdwinTextBufferLC( TInt aId )
    {
    HBufC* text = GetEdwinTextBufferL( aId );
    CleanupStack::PushL( text );
    return text;
    }


// ---------------------------------------------------------
// CCAContactEditor::SetMenuBarTitleResourceL
// Update CBA-container with resource pointed by aResourceId
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::SetMenuBarTitleResourceL( TInt aResourceId )
    {
    // Get handle to current cba-button container and update container with
    // resource pointed by aResourceId (RESOURCE CBA)
    CEikButtonGroupContainer* container = CEikButtonGroupContainer::Current();
    container->SetCommandSetL( aResourceId );
    container->DrawNow();
    }

// ---------------------------------------------------------
// CCAContactEditor::UpdateTitleBarL
// Updates title bar according to current data edited
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::UpdateTitleBarL()
    {

    CCAStatusPaneHandler* statusPane = iAppUI->CAStatusPane();

    if ( iInternalMode == TEnumsPC::EAddToContact )
        {
        HBufC* title = iEikonEnv->AllocReadResourceLC( R_CHAT_CD_NEW_ENTRY );
        statusPane->SetTitleL( *title );
        CleanupStack::PopAndDestroy( title );
        }
    else if ( iOwnStatus )
        {
        HBufC* title = iEikonEnv->AllocReadResourceLC(
                           R_QTN_CHAT_CD_OWN_DATA_TITLE );

        statusPane->SetTitleL( *title );
        CleanupStack::PopAndDestroy( title );
        }
    else
        {
        // editing existing
        HBufC* title;

        // Identification of contact is one of the following
        // in this order:
        // 1. Nick name
        // 2. Alias
        // 3. WV ID
        if ( iNicknameEnabled )
            {
            title = GetEdwinTextBufferL( EChatEditNick );
            if ( title )
                {
                CleanupStack::PushL( title );
                statusPane->SetTitleL( *title );
                CleanupStack::PopAndDestroy( title );
                return;
                }
            }

        if ( iAliasEnabled && iAlias )
            {
            statusPane->SetTitleL( *iAlias );
            return;
            }

        title = GetEdwinTextBufferL( EChatEditWV );
        if ( title )
            {
            CleanupStack::PushL( title );
            statusPane->SetTitleL( *title );
            CleanupStack::PopAndDestroy( title );
            return;
            }
        }
    }

// ---------------------------------------------------------
// CCAContactEditor::HideEmptyFieldsL
// Hides fields that have no data
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::HideEmptyFieldsL()
    {
    if ( iInternalMode == TEnumsPC::EAddToContact )
        {
        // in this case empty alias should be hidden
        if ( ControlOrNull( EChatEditAlias ) )
            {
            if ( GetFormEdwin( EChatEditAlias )->TextLength() == 0 )
                {
                DeleteLine( EChatEditAlias );
                }
            }
        }
    }

// ---------------------------------------------------------
// CCAContactEditor::SetFocusToAvailableFieldL
// Sets focus to field that is available
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::SetFocusToAvailableFieldL()
    {
    TBool readOnly = ETrue;
    TInt controlId = EChatEditWV;
    TBool aliasField = EFalse, nickField( EFalse ), wvField( EFalse );

    if ( ControlOrNull( EChatEditNick ) )
        {
        nickField = ETrue;
        }

    if ( ControlOrNull( EChatEditAlias ) )
        {
        aliasField = ETrue;
        }

    if ( ControlOrNull( EChatEditWV ) )
        {
        wvField = ETrue;
        }

    // When opening the editing view from the details view, the cursor is in the end of
    // the text field that was in focus on details view.
    // Check if focus must be changed to get it visible
    if ( ControlOrNull( iInitialFocus ) )
        {
        switch ( iInitialFocus )
            {
            case EChatEditNick:
                {
                if ( nickField &&
                     ( !GetFormEdwin( EChatEditNick )->IsReadOnly() )
                   )
                    {
                    readOnly = EFalse;
                    controlId = EChatEditNick;
                    }
                break;
                }

            case EChatEditAlias:
                {
                if ( aliasField &&
                     ( !GetFormEdwin( EChatEditAlias )->IsReadOnly() )
                   )
                    {
                    readOnly = EFalse;
                    controlId = EChatEditAlias;
                    }
                break;
                }

            case EChatEditWV:
                {
                if ( wvField &&
                     ( !GetFormEdwin( EChatEditWV )->IsReadOnly() )
                   )
                    {
                    readOnly = EFalse;
                    }
                break;
                }

            default:
                {
                break;
                }
            }

        //In case the focused field cannot be edited, then focus is
        //in the first editable field from the top of the list.
        //Identify from top which field is readOnly
        if ( readOnly )
            {

            if ( wvField
                 && ( !GetFormEdwin( EChatEditWV )->IsReadOnly() )
               )
                {
                controlId = EChatEditWV;
                }
            else if ( aliasField
                      && ( !GetFormEdwin( EChatEditAlias )->IsReadOnly() )
                    )
                {
                controlId = EChatEditAlias;
                }
            else if ( nickField
                      && ( !GetFormEdwin( EChatEditNick )->IsReadOnly() )
                    )
                {
                controlId = EChatEditNick ;
                }
            }

        if ( ControlOrNull( controlId ) )
            {
            TryChangeFocusToL( controlId );
            GetFormEdwin( controlId )->ClearSelectionL();
            }

        }


    }

// ---------------------------------------------------------
// CCAContactEditor::SetInitialCurrentLine
// Set initial line to be in edit-state when edit or add-state is chosen
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::SetInitialCurrentLine()
    {
    CAknForm::SetInitialCurrentLine();

    // try to set the focus to what it was in contact
    // details viewer, if the caller gave us that
    if ( iInitialFocus != EChatEditDummy )
        {
        TRAPD( error,

               // the field might not be available, this call
               // takes care of moving to a correct field
               SetFocusToAvailableFieldL(); );

        if ( error )
            {
            CActiveScheduler::Current()->Error( error );
            }
        }
    }

// ---------------------------------------------------------
// CCAContactEditor::OfferKeyEventL
// Overridden method for giving selection-key a default method (edit)
// (other items were commented in a header).
// ---------------------------------------------------------
//

TKeyResponse CCAContactEditor::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                               TEventCode aType )
    {
    CHAT_DP( D_CHAT_LIT( "aKeyEvent.iScanCode == %d" ), aKeyEvent.iScanCode );
    CHAT_DP( D_CHAT_LIT( "aKeyEvent.iModifiers == %d" ), aKeyEvent.iModifiers );
    CHAT_DP( D_CHAT_LIT( "aKeyEvent.iRepeats == %d" ), aKeyEvent.iRepeats );
    CHAT_DP( D_CHAT_LIT( "IsEditAble() == %d" ), IsEditable() );



    // Show note when user is trying to edit WV ID
    if ( IsEditable() &&
         IsEditingKey( aKeyEvent ) &&
         IdOfFocusControl() == EChatEditWV &&
         aType == EEventKey &&
         ( iOwnStatus || ( iUserIDNotEditable && iInternalMode != TEnumsPC::EAddToContact ) ) )
        {
        IMDialogUtils::DisplayInformationNoteL(
            R_QTN_CHAT_UNABLE_MODIFY_USERID,
            KNullDesC );

        return EKeyWasConsumed;
        }

    // Show note when user is trying to edit contacts alias
    if ( !iOwnStatus &&
         IsEditable() &&
         IsEditingKey( aKeyEvent ) &&
         IdOfFocusControl() == EChatEditAlias &&
         aType == EEventKey )
        {
        IMDialogUtils::DisplayInformationNoteL(
            R_QTN_CHAT_UNABLE_MODIFY_ALIAS,
            KNullDesC );

        return EKeyWasConsumed;
        }

    if ( aKeyEvent.iCode == EKeyPhoneSend )
        {
        return EKeyWasConsumed;
        }

    TKeyResponse retVal =  CAknForm::OfferKeyEventL( aKeyEvent, aType );

    // Show domain selection query if variated so
    if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_DOMAIN_SELECTION )
         && IsEditable()
         && IdOfFocusControl() == EChatEditWV
         && aType == EEventKey
         && ( aKeyEvent.iCode == EKeyBackspace || aKeyEvent.iCode == EKeyDelete ) )
        {
        CEikEdwin* edwin = GetFormEdwin( EChatEditWV );
        if ( edwin->TextLength() == 0 )
            {
            HBufC* userName = HBufC::NewLC( KServerWVUserIdMaxLength );
            TPtr userPtr( userName->Des() );


            if ( iAppUI->
                 UISessionManager().DisplayDomainSelectionQueryL( userPtr ) )
                {
                // Domain selected
                edwin->SetTextL( userName );
                edwin->SetCursorPosL( 0, EFalse );
                }

            CleanupStack::PopAndDestroy( userName );
            }
        }

    return retVal;
    }

// ---------------------------------------------------------
// CCAContactEditor::DynInitMenuPaneL
// Gets called before form is displayed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::DynInitMenuPaneL( TInt aResourceId,
                                         CEikMenuPane* aMenuPane )
    {
    // Execute base class's DynInitMenuPaneL
    CAknForm::DynInitMenuPaneL( aResourceId, aMenuPane );

    // if we're displaying general menu and help feature is not supported..
    if ( aResourceId == R_CHATCLIENT_GENERAL_MENU &&
         !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        // dimm the help menu item
        aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
        }
    else if ( aResourceId == R_AVKON_FORM_MENUPANE )
        {
        // Deleting is not needed when editing...
        // nor when viewing own status
        if ( IsEditable() || iOwnStatus )
            {
            aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdDelete,
                                      ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EChatClientFriendsListViewCmdDelete,
                                      EFalse );
            }

        if ( !iAliasEnabled && iOwnStatus )
            {
            aMenuPane->SetItemDimmed( EAknFormCmdEdit, ETrue );
            }

        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
            }
        // Disable unnecessary items, that CAknForm adds...
        aMenuPane->SetItemDimmed( EAknFormCmdDelete, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdAdd, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdLabel, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdSave, ETrue );
        }
    }

// ---------------------------------------------------------
// CCAContactEditor::PreLayoutDynInitL
// Gets called before form is displayed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::PreLayoutDynInitL()
    {
    //SetMenuBarTitleResourceL( R_AVKON_SOFTKEYS_DONE_CANCEL );
    SetMenuBarTitleResourceL( R_CHAT_SOFTKEYS_DONE_CANCEL__EMPTY );

    LoadFormDataL();

    SetInitialCurrentLine();

    CAknForm::PreLayoutDynInitL();

    // set own status, but for landscape mode only
    iAppUI->UISessionManager().SetStatusPaneIconsL();
    }

// ---------------------------------------------------------
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::PostLayoutDynInitL()
    {
    CAknForm::PostLayoutDynInitL();
    SetEditableL( ETrue );
    }

// ---------------------------------------------------------
// CCAContactEditor::SaveFormDataL
// Gets called when user chooses to save data
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAContactEditor::SaveFormDataL()
    {
    // we need to be logged in
    if ( !iAppUI->UISessionManager().LoginL( MCAUiLoginCmdCB::EQueryLogin ) )
        {
        return EFalse;
        }


    // wait for background fetch
    if ( !CCASyncChecker::CheckSyncStateL() )
        {
        // canceled or not logged in
        return EFalse;
        }
    MCABlockingPC* blockingInterface = iAppUI->GetProcessManager().GetBlockingInterface();

    iAllFieldsEmpty = ETrue;
    iContactWasSaved = EFalse;
    iSaveStarted = ETrue;

    if ( iOwnStatus )
        {
        if ( iAliasEnabled )
            {
            HBufC* alias = GetEdwinTextBufferLC( EChatEditAlias );
            if ( !alias )
                {
                CleanupStack::PopAndDestroy( alias );
                if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_ALIAS_MANDATORY ) )
                    {
                    IMDialogUtils::DisplayQueryDialogL( R_QTN_CHAT_NO_ALIAS_DEF );
                    iAllFieldsEmpty = EFalse;
                    iSaveStarted = EFalse;
                    return EFalse;
                    }
                //it should never be empty
                alias = KNullDesC().AllocLC();
                }

            //call function tat will UpdateOwnAliasL( alias->Des() );
            iAppUI->ShowWaitDialogLC( R_QTN_GEN_NOTE_SAVING, ETrue );

            MCASettingsPC* settingsPC = iAppUI->GetProcessManager().GetSettingsInterface();

            iContactEditOperationError = iCCAContactEditPC->UpdateOwnAliasL( *alias );

            CleanupStack::Pop(); //wait dialog

            CleanupStack::PopAndDestroy( alias );

            if ( iContactEditOperationError )
                {
                iAppUI->DismissWaitDialogL( KErrNone, EFalse, KNullDesC );
                iAliasNotOk = ETrue;
                if ( iContactEditOperationError == ECSPInvalidPresenceValue )
                    {
                    //iAliasNotOk = ETrue;
                    iAllFieldsEmpty = EFalse;
                    HBufC* text = StringLoader::LoadLC( R_QTN_CHAT_TOO_LONG_ALIAS_TEXT );
                    IMDialogUtils::DisplayErrorNoteL( *text );
                    CleanupStack::PopAndDestroy( text );
                    }
                iSaveStarted = EFalse;
                return EFalse;
                }
            else
                {
                iAppUI->DismissWaitDialogL( KErrNone, EFalse, KNullDesC );
                // Alias saved OK, show confirmation note
                IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_CNOTE_SAVED );
                iAllFieldsEmpty = EFalse;
                iContactWasSaved = ETrue;
                }
            }
        iSaveStarted = EFalse;
        return ETrue;
        }

    //delete the old iNewUserId  if exists
    if ( iNewUserId )
        {
        delete iNewUserId;
        iNewUserId = NULL;
        }

    //delete the old iNick  if exists
    if ( iNick )
        {
        delete iNick;
        iNick = NULL;
        }

    // Get nickname
    if ( iNicknameEnabled )
        {
        iNick = GetEdwinTextBufferL( EChatEditNick );
        if ( iNick )
            {
            iAllFieldsEmpty = EFalse;
            }
        }

    // make sure we have some kind of nick
    if ( !iNick )
        {
        iNick = HBufC::NewL( 0 );
        }

    iNewUserId = GetEdwinTextBufferL( EChatEditWV );

    if ( !iNewUserId ) // if the field is empty
        {

        // Create empty buffer
        iNewUserId = HBufC::NewL( 0 );

        if ( !iAllFieldsEmpty )
            {
            CAknQueryDialog* dlg =
                new ( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );
            TInt ret( dlg->ExecuteLD(
                          R_CHAT_CEDITOR_ERROR_WVIDMISSING_QUERY ) );
            if ( ( ret != EAknSoftkeyOk ) && ( ret != EAknSoftkeyYes ) )
                {
                if ( iInternalMode == TEnumsPC::EAddToContact )
                    {
                    // just get out of here, nothing to delete
                    iContactDeleted = ETrue;
                    iSaveStarted = EFalse;
                    return ETrue;
                    }
                else
                    {

                    TInt err = DeleteCurrentContactL( EFalse );
                    if ( err == KErrNone )
                        {
                        iContactDeleted = ETrue;
                        CHAT_DP_TXT( "contact should NOT be saved.." );
                        iSaveStarted = EFalse;
                        return ETrue;
                        }
                    else
                        {
                        // let user out of contact editor but report the error
                        iContactDeleted = ETrue;
                        CActiveScheduler::Current()->Error( err );
                        iSaveStarted = EFalse;
                        return ETrue;
                        }
                    }
                }

            CHAT_DP_TXT( "wvid missing, changing focus to editor.." );
            TryChangeFocusToL( EChatEditWV );
            iSaveStarted = EFalse;
            return EFalse;
            }
        }
    else
        {
        iAllFieldsEmpty = EFalse;

        if ( iOrigId )
            {
            // we have some user id, so reconstruct it (add removed "wv:" and "@something")
            HBufC* newId = CAUtils::ReconstructIdL( *iOrigId, *iNewUserId );

            // replace the id with new one
            delete iNewUserId;
            iNewUserId = newId;
            }
        }

    if ( !CAUtils::ValidLoginIdL( *iNewUserId ) )
        {
        HBufC* text = StringLoader::LoadLC( R_CHAT_ERROR_WRONG_WVID, *iNewUserId );
        IMDialogUtils::DisplayErrorNoteL( *text );
        CleanupStack::PopAndDestroy( text ); // text
        iSaveStarted = EFalse;
        return EFalse;
        }

    // If all fields are empty then we can go back without saving contact
    if ( iAllFieldsEmpty )
        {
        iSaveStarted = EFalse;
        return ETrue;
        }


    // check if there's already a contact with same wvid

    TBool contactInList( EFalse );
    TDesC* tmpUsrId = NULL;

    //Check whether a new contact/existing contact
    TBool createNew = ( iInternalMode == TEnumsPC::EAddToContact ) ? ETrue : EFalse;

    TBool emptyNick = ( iNick->CompareC( KNullDesC ) == 0 );
    TBool sameNick = emptyNick ? EFalse
                     : iCCAContactEditPC->FindAnyContactByNickL( *iNick );


    if ( sameNick )
        {
        // Contact with same nick was found from stored contacts,
        // ask if user wants to edit nick
        TInt ret = IMDialogUtils::DisplayYesNoConfirmationDialogL(
                       R_QTN_CHAT_SAME_NICK_NAME );

        // If user wants to edit nick move focus to nick and highlight it.
        // Otherwise continue saving with current nick
        if ( ret == EAknSoftkeyOk || ret == EAknSoftkeyYes )
            {
            FocusEdwinL( EChatEditNick, ETrue );
            iSaveStarted = EFalse;
            return EFalse;
            }
        }

    if ( blockingInterface->IsContactBlocked( *iNewUserId )
         && ( 1 == IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING ) ) )
        {
        iCCAContactEditPC->DeleteContactL( *iNewUserId );
        }

    if ( iCCAContactEditPC->IsWVIDAvailableAndValidL( createNew, *iListId, *iNewUserId,
                                                      contactInList, tmpUsrId ) )
        {
        if ( contactInList )
            {

            TInt ret( IMDialogUtils::DisplayYesNoConfirmationDialogL(
                          R_QTN_CHAT_CREATE_CONFQ_EX_WVID ) );

            if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
                {
                iInternalMode = TEnumsPC::EEditContact;
                // Change user id to match new target contact.
                delete iUserId;
                iUserId = NULL;
                iUserId = tmpUsrId->AllocL();
                }
            else
                {
                // move focus to wvid and highlight it
                FocusEdwinL( EChatEditWV, ETrue );
                iSaveStarted = EFalse;
                return EFalse;
                }
            }

        }

    else
        {
        iInternalMode = TEnumsPC::EAddToContact;
        }


    // check if unblock query enabled and blocked contacts
    // are not shown in the contact list
    TBool showUnblockQuery =
        ( ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
            & EIMFeatAddBlockedContactQuery )
          && ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING ) != 2 ) );

    if ( showUnblockQuery )
        {


        // check if the wvid to be added is blocked
        if ( blockingInterface->IsContactBlocked( *iNewUserId ) )
            {
            // show query
            HBufC* prompt = NULL;
            prompt = StringLoader::LoadLC( R_QTN_CHAT_CONTACT_ADD_BLOCKED,
                                           CCAPCUtils::DisplayId( *iNewUserId ) );

            TInt ret( IMDialogUtils::DisplayYesNoConfirmationDialogL(
                          *prompt ) );

            if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
                {
                //We have to create a list of users to be unblocked
                CDesCArrayFlat* unBlockList =
                    new ( ELeave ) CDesCArrayFlat( 1 );
                CleanupStack::PushL( unBlockList );
                unBlockList->AppendL( *iNewUserId );

                // unblock
                iAppUI->ShowWaitDialogL( R_CHAT_BLOCK_REMOVING );
                TInt errorCode( KErrNone );
                TRAPD( err, errorCode =
                           blockingInterface->RemoveL( unBlockList, NULL ) );
                iAppUI->DismissWaitDialogL( err );

                if ( errorCode != KErrNone )
                    {
                    IMDialogUtils::DisplayErrorNoteL( errorCode );
                    }
                CleanupStack::PopAndDestroy( 2, prompt ); // unBlockList, prompt
                }
            else
                {
                CleanupStack::PopAndDestroy( prompt );
                prompt = StringLoader::LoadLC( R_QTN_CHAT_CONTACT_ADD_BLOCKED_NOT );
                IMDialogUtils::DisplayErrorNoteL( *prompt );
                CleanupStack::PopAndDestroy( prompt ); // prompt
                // Return to place where adding was initiated
                iContactWasSaved = ETrue;
                iSaveStarted = EFalse;
                return ETrue;
                }
            }
        }

    HBufC* oldUserId = NULL;

    // proceed with adding or saving
    if ( iInternalMode == TEnumsPC::EAddToContact )
        {
        iAppUI->ShowWaitDialogLC( R_QTN_CHAT_FRIEND_ADDPB );

        //*******************// add called
        iContactEditOperation = MCAUiContactEditCmdCB::TContactEditParams::EAddContact;
        iCCAContactEditCmd->ExecuteCommandL();

        CleanupStack::PopAndDestroy(); // iAppUI->DismissWaitDialog
        }
    else
        {
        // editing a contact
        if ( iUserId )
            {
            oldUserId = iUserId->AllocLC();
            }
        else
            {
            // in this case it's NULL, more nicer cleanup later
            CleanupStack::PushL( oldUserId );
            }

        iAppUI->ShowWaitDialogLC( R_QTN_GEN_NOTE_SAVING );


        if ( iUserId && ( CAUtils::NeutralCompare( *iNewUserId, *iUserId ) == 0 ) )
            {
            //call for update	   ******************

            iContactEditOperation = MCAUiContactEditCmdCB::TContactEditParams::EUpdateContact;
            iCCAContactEditCmd->ExecuteCommandL();

            if ( iContactEditOperationError == KErrNone )
                {
                delete iUserId;
                iUserId = NULL;
                iUserId = iNewUserId->AllocL();
                }
            }
        else
            {
            iCCAContactEditPC->SetAnyContactL( *iNewUserId );
            //*******************// add called
            iContactEditOperation = MCAUiContactEditCmdCB::TContactEditParams::EAddContact;
            iCCAContactEditCmd->ExecuteCommandL();

            }
        CleanupStack::PopAndDestroy(); // iAppUI->DismissWaitDialog

        }

    // handle possible error cases
    CHAT_DP( D_CHAT_LIT( "*UpdateContactL error %d" ), iContactEditOperationError );

    iSaveStarted = EFalse;

    HBufC* text = NULL;

    switch ( iContactEditOperationError )
        {
        case ECSPAccountLost:   // this error occurs when the list is full
        case ECSPContactListFull: // flowthrough
            {
            text = iEikonEnv->AllocReadResourceLC(
                       R_QTN_CHAT_CONTACT_LIST_FULL );
            IMDialogUtils::DisplayErrorNoteL( *text );
            CleanupStack::PopAndDestroy( text );
            break;
            }

        case ECSPUnknownUserId: // this error occurs when the userID is unknown
            {
            text = iEikonEnv->AllocReadResourceLC(
                       R_QTN_CHAT_VALIDATING_CONTACT_FAILED );
            IMDialogUtils::DisplayErrorNoteL( *text );
            CleanupStack::PopAndDestroy( text );

            // move focus to wvid and highlight it
            FocusEdwinL( EChatEditWV );

            //iCCAContactEditPC->SetContactL( *iListId, *iNewUserId );

            if ( iInternalMode != TEnumsPC::EAddToContact )
                {
                CleanupStack::PopAndDestroy( oldUserId ); // oldUserId
                }

            return EFalse;
            }


        case KErrNone:  // contact added
            {
            iContactWasSaved = ETrue;
            break;
            }

        default:
            {
            // show error code
            IMNoteMapper::ShowNoteL( iContactEditOperationError, *iNewUserId );
            break;
            }
        }

    if ( iInternalMode != TEnumsPC::EAddToContact )
        {
        // editing a contact

        // got some error, so restore userid
        if ( iContactEditOperationError != KErrNone )
            {
            if ( oldUserId )
                {
                delete iUserId;
                iUserId = oldUserId;
                }
            CleanupStack::Pop( oldUserId );
            }
        else
            {
            // it could be NULL, doesn't matter
            CleanupStack::PopAndDestroy( oldUserId );
            }
        }

    if ( iContactEditOperationError != KErrNone )
        {
        return EFalse;
        }


    return ETrue;
    }

// ---------------------------------------------------------
// CCAContactEditor::DoNotSaveFormDataL
// Gets called when user chooses not to save data
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::DoNotSaveFormDataL()
    {
    // Refresh screen data
    if ( iInternalMode == TEnumsPC::EEditAfterView )
        {
        iInternalMode = TEnumsPC::EViewContact;
        LoadFormDataL();
        SetEditableL( EFalse );
        }
    }

// ---------------------------------------------------------
// CCAContactEditor::OkToExitL
// Button that was pressed for dismissing dialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAContactEditor::OkToExitL( TInt aButtonId )
    {
    CHAT_DP_TXT( "CCAContactEditor::OkToExitL" );
    if ( iSaveStarted )
        {
        return EFalse;
        }

    HideMenu();

    if ( aButtonId == EAknSoftkeyEmpty )
        {
        return EFalse;
        }
    // If user chose exit, then dialog can be dismissed from any state
    if ( aButtonId == EEikCmdExit || aButtonId == EAknSoftkeyExit || aButtonId == EAknSoftkeyCancel )
        {
        return ETrue;
        }


    // Functionality of EAknSoftkeyDone is the same as EAknSoftkeyBack
    if ( aButtonId == EAknSoftkeyDone )
        {
        aButtonId = EAknSoftkeyBack;
        }
          
    if ( !iAppUI->UISessionManager().IsLoggedIn() )
    	{
    	iAppUI->UISessionManager().LoginL(MCAUiLoginCmdCB::EQueryLogin);
    	SaveFormDataL();
    	}
    	
    // Call base class to handle the event
    TBool okToExit( CAknForm::OkToExitL( aButtonId ) );

    if ( aButtonId != EAknSoftkeyBack )
        {
        return okToExit;
        }

    if ( !iAppUI->UISessionManager().IsLoggedIn() )
        {
        return EFalse;
        }

    // iAllFieldsEmpty is ETrue if
    // - Nothing has been changed.
    // - All fields are empty

    // iContactDeleted is ETrue if
    // - Contact is deleted
    // - Wvid is empty and user wants to close edit mode.

    // iContactWasAdded is ETrue if
    // - Contact is added successfully

    // None of these can be set at the same time if situation is ok to exit.

    if ( ( iAllFieldsEmpty ^ iContactDeleted ^ iContactWasSaved ) )
        {
        if ( iInternalMode == EEditAfterView )
            {
            // Change focus if needed
            SetFocusToAvailableFieldL();
            //Set the internal state
            DoNotSaveFormDataL();

            // Reset flags
            iAllFieldsEmpty = ETrue;
            iContactDeleted = EFalse;
            iContactWasSaved = EFalse;
            return EFalse;
            }
        else
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CCAContactEditor::LaunchHelpL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAContactEditor::LaunchHelpL( const TDesC& aContext )
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        //make context array
        //granularity 1 is ok cos there is added just one item
        CArrayFix< TCoeHelpContext >* cntx = new( ELeave ) CArrayFixFlat <
        TCoeHelpContext > ( 1 );
        CleanupStack::PushL( cntx );

        cntx->AppendL( TCoeHelpContext( KUidChatClient, aContext ) );
        CleanupStack::Pop( cntx );

        //and launch help - takes ownership of context array
        HlpLauncher::LaunchHelpApplicationL(
            CEikonEnv::Static()->WsSession(), cntx );
        }
    }

// ---------------------------------------------------------
// CCAContactEditor::QuerySaveChangesL
// Do not ask about saving the contact
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAContactEditor::QuerySaveChangesL()
    {
    return ETrue;
    }



// -----------------------------------------------------------------------------
// CCAContactEditor::IsNonEditingKey()
// -----------------------------------------------------------------------------
//
TBool CCAContactEditor::IsEditingKey( const TKeyEvent &aKeyEvent )
    {
    switch ( aKeyEvent.iScanCode )
        {
            // list of editing keys
        case EStdKeyForwardSlash:       // fallthrough
        case EStdKeyBackSlash:
        case EStdKeySemiColon:
        case EStdKeySingleQuote:
        case EStdKeyHash:
        case EStdKeySquareBracketLeft:
        case EStdKeySquareBracketRight:
        case EStdKeyMinus:
        case EStdKeyEquals:
        case EStdKeyNkpForwardSlash:
        case EStdKeyNkpAsterisk:
        case EStdKeyNkpMinus:
        case EStdKeyNkpPlus:
        case EStdKeyNkpEnter:
        case EStdKeyNkp1:
        case EStdKeyNkp2:
        case EStdKeyNkp3:
        case EStdKeyNkp4:
        case EStdKeyNkp5:
        case EStdKeyNkp6:
        case EStdKeyNkp7:
        case EStdKeyNkp8:
        case EStdKeyNkp9:
        case EStdKeyNkp0:
        case EStdKeyBackspace:
        case EStdKeyTab:
        case EStdKeyEnter:
        case EStdKeySpace:
            {
            return ETrue;
            }
        }

    // qwerty keyboard: alphapetic keys 'a'-'z'
    if ( aKeyEvent.iCode >= KAlphaCodeA && aKeyEvent.iCode <= KAlphaCodeZ )
        {
        return ETrue;
        }

    // numeric keys '0'-'9'
    if ( aKeyEvent.iCode >= KNumCode0 && aKeyEvent.iCode <= KNumCode9 )
        {
        return ETrue;
        }

    // non editing key
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CCAContactEditor::FocusEdwinL
// -----------------------------------------------------------------------------
//
void CCAContactEditor::FocusEdwinL( TInt aId, TBool aHighlight /*= EFalse*/ )
    {
    TryChangeFocusToL( aId );
    CEikEdwin* wvField = GetFormEdwin( aId );
    if ( aHighlight )
        {
        // Set field to be highlighted
        wvField->SelectAllL();
        }
    wvField->SetCursorPosL( wvField->TextLength(), aHighlight );
    }


// -----------------------------------------------------------------------------
// CCAContactEditor::GetParameters
// this function is called by the contact edit object
// this function is called by the contact edit object
// based on the iContactEditOperation - The Command object
// gets the required data from UI
// -----------------------------------------------------------------------------
//

void CCAContactEditor::GetParameters( MCAUiContactEditCmdCB::TContactEditParams& aParam )
    {

    aParam.iContactEditOperation = iContactEditOperation;

    switch ( aParam.iContactEditOperation )
        {
        case MCAUiContactEditCmdCB::TContactEditParams::ELoadContactDetails :
            //load exisiting contact details
            {
            aParam.iAliasEnabled = iAliasEnabled;
            aParam.iNicknameEnabled = iNicknameEnabled;
            break;
            }

        case MCAUiContactEditCmdCB::TContactEditParams::EAddContact :
            //add a new contact
            {
            aParam.iListId = iListId;
            aParam.iUserId = iNewUserId;
            aParam.iNickname = iNick;
            aParam.iAlias = iAlias;
            aParam.iNewContact = ETrue;
            break;
            }
        case MCAUiContactEditCmdCB::TContactEditParams::EUpdateContact :
            //update an existing contact
            {
            aParam.iListId = iListId;
            aParam.iUserId = iNewUserId;
            aParam.iNickname = iNick;
            aParam.iAlias = iAlias;
            aParam.iNewContact = EFalse;
            break;
            }
        case MCAUiContactEditCmdCB::TContactEditParams::EMoveContact :
            //move a exisiting contact from one list to another
            {
            aParam.iListId = iListId;
            aParam.iUserId = iUserId;
            break;
            }
        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CCAContactEditor::SetParameters
// this function is called by the contact edit object
// based on the iContactEditOperation - command object sets the
// data required by UI.
// The ownership of pointer varibles is also transferred to the UI.
// Hence UI must delete the allocated data.
// -----------------------------------------------------------------------------
//
void CCAContactEditor::SetParameters( MCAUiContactEditCmdCB::TContactEditParams& aParam )
    {
    switch ( aParam.iContactEditOperation )
        {
        case MCAUiContactEditCmdCB::TContactEditParams::ELoadContactDetails :
            {
            iOwnStatus = aParam.iOwnData;
            iOrigId = aParam.iOrigId;
            iUserId = aParam.iUserId;
            iAlias = aParam.iAlias;
            iIdentification = aParam.iIdentification;
            iNick = aParam.iNickname;
            break;
            }

        case MCAUiContactEditCmdCB::TContactEditParams::EAddContact :
        case MCAUiContactEditCmdCB::TContactEditParams::EUpdateContact :
        case MCAUiContactEditCmdCB::TContactEditParams::EMoveContact :
            {
            iContactEditOperationError = aParam.iContactEditOperationError;
            break;
            }
        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CCAContactEditor::DeleteCurrentContactL
// -----------------------------------------------------------------------------
//
TInt CCAContactEditor::DeleteCurrentContactL( TBool aAskConfirmation /*= ETrue*/ )
    {
    // can't delete self
    if ( iOwnStatus )
        {
        return KErrAccessDenied;
        }

    // Get the name of the friend to be deleted
    TPtrC name( *iIdentification );

    // Load and format confirmation string from resource
    HBufC* prompt = StringLoader::LoadLC( RSC_CHAT_DELETE_CONFIRMATION, name );

    TInt ret = aAskConfirmation ?
               IMDialogUtils::DisplayQueryDialogL(
                   R_GENERIC_YES_NO_CONFIRMATION_QUERY, *prompt ) :
               EAknSoftkeyOk;
    CleanupStack::PopAndDestroy( prompt );

    TInt error( KErrCancel );

    // Check what the user responded
    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        // accepted, lets delete the contact
        iAppUI->ShowWaitDialogLC( R_QTN_CHAT_DELETING_CONTACT );

        error = iCCAContactEditPC->DeleteCurrentContactL( *iListId, *iOrigId );

        CleanupStack::PopAndDestroy(); //iAppUI->DismissWaitDialogL()
        if ( error <= Imps_ERROR_BASE )
            {
            // server error
            IMNoteMapper::ShowNoteL( error );
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CCAContactEditor::GetListIdL()
// -----------------------------------------------------------------------------
//

HBufC* CCAContactEditor::GetListIdL( const TDesC& aId, TInt aNoListsResource )
    {

    TInt itemCount( 0 );
    CDesCArrayFlat* listIdArray = NULL;

    RArray <TInt> indexArray;

    HBufC* listId = NULL;

    listId = iCCAContactEditPC->GetListIdL( aId, aNoListsResource, itemCount,
                                            listIdArray, indexArray,
                                            ( iInternalMode == TEnumsPC::EMoveContact ) ? ETrue : EFalse
                                          );

    if ( !listId )
        {
        if ( itemCount == 0 )
            {
            if ( aNoListsResource != 0 )
                {
                IMDialogUtils::DisplayInformationNoteL( aNoListsResource );
                }
            else
                {
                // Show note with OK softkey, return value can be ignored
                IMDialogUtils::DisplayQueryDialogL( R_QTN_CHAT_CEDITOR_NOCONTACTLISTS );
                TInt initValue = KErrNotFound;
                // Advance to list creation
                TInt ret = CCAUINGUtils::NewContactListL( initValue );

                if ( ret == KErrNone )
                    {
                    // List created succesfully, call this recursively to get list id
                    return iCCAContactEditPC->GetListIdL( aId, aNoListsResource, itemCount,
                                                          listIdArray, indexArray,
                                                          ( iInternalMode == TEnumsPC::EMoveContact ) ? ETrue : EFalse );
                    }
                else
                    {
                    CActiveScheduler::Current()->Error( ret );
                    }
                }
            }
        else if ( itemCount > 1 )
            {
            //this means thats indexArray, listIdArray has been allocated by GetListIdL function.
            //so we need to push the same in cleanupstack again
            CleanupClosePushL( indexArray );
            CleanupStack::PushL( listIdArray ) ;

            TInt selectedIndex( 0 );
            HBufC* titleText = CCoeEnv::Static()->AllocReadResourceLC(
                                   R_QTN_CHAT_SERVCONT_SELLIST );
            // launch the list query
            TInt ret( IMDialogUtils::DisplayListQueryDialogL( &selectedIndex, listIdArray,
                                                              R_PRIVATE_CHAT_LIST_SELECT_RECIP_DLG,
                                                              *titleText, KNullDesC, ETrue ) );
            CleanupStack::PopAndDestroy( 2, listIdArray ); // listIdArray, titleText

            if ( ( ret == EAknSoftkeyOk || ret == EAknSoftkeyDone || ret ==
                   EAknSoftkeySelect ) && ( iAppUI->UISessionManager().IsLoggedIn() ) )
                {
                HBufC* listId = iCCAContactEditPC->GetListIdAtIndexL( indexArray[selectedIndex] );
                CleanupStack::PopAndDestroy( &indexArray ); //indexArray
                return listId;
                }
            else
                {
                CleanupStack::PopAndDestroy( &indexArray ); //indexArray
                }
            }
        }

    return listId;
    }


// ---------------------------------------------------------
// CCAContactEditor::MoveContactL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAContactEditor::MoveContactL( const TDesC& aListId,
                                      HBufC*& aMovedListId, TBool& aCollapsed )
    {
    CHAT_DP_TXT( "CCAContactEditor::MoveContactL() start" );
    aMovedListId = NULL;
    if ( !iCCAContactEditPC->IsContact() )
        {
        User::Leave( KErrNotFound );
        }

    if ( !CCASyncChecker::CheckSyncStateL() )
        {
        return EFalse;
        }

    iInternalMode = TEnumsPC::EMoveContact;

    // select list
    HBufC* listId = GetListIdL( aListId, R_QTN_CHAT_NO_LISTS );

    if ( !listId )
        {
        // no other lists available
        return EFalse;
        }

    // copy the listid, where the contacts has to be moved,
    // Ownership is transfered to the caller.
    aMovedListId = listId->AllocL();

    // Get the status of the list(collapsed/expanded)
    // before performing the actual move.
    aCollapsed = iCCAContactEditPC->IsCollapsedL( *listId );

    CleanupStack::PushL( listId );

    /* In case the contact is already in the selected list, it is
    removed from the current list, but not moved to the selected list anymore.
    */
    // check if there's already a contact with same wvid
    TBool contactInList( EFalse );
    TDesC* tmpUsrId = NULL;
    const TDesC& userId = iCCAContactEditPC->GetCurrentContactUserId();

    if ( iCCAContactEditPC->IsWVIDAvailableAndValidL( ETrue, *listId,
                                                      userId,
                                                      contactInList, tmpUsrId ) )
        {
        if ( contactInList )
            {
            TInt error = iCCAContactEditPC->DeleteCurrentContactL( aListId, userId );
            CleanupStack::PopAndDestroy();
            return EFalse;
            }
        }


    iListId = aListId.AllocL();

    iUserId = listId->AllocL();

    CCAWaitNote* note = CCAWaitNote::ShowWaitNoteLC(
                            R_QTN_CHAT_MOVING_CONTACT );

    iContactEditOperation = MCAUiContactEditCmdCB::TContactEditParams::EMoveContact;
    iCCAContactEditCmd->ExecuteCommandL();

    CleanupStack::PopAndDestroy( 2, listId ); //note, listid

    switch ( iContactEditOperationError )
        {

        case ECSPContactListFull: // flowthrough
            {
            HBufC* text = iEikonEnv->AllocReadResourceLC(
                              R_QTN_CHAT_CONTACT_LIST_FULL );
            IMDialogUtils::DisplayErrorNoteL( *text );
            CleanupStack::PopAndDestroy( text );
            break;
            }
        case KErrNone:
            {
            // show note
            IMDialogUtils::DisplayConfirmationNoteL( R_QTN_CHAT_CONTACT_MOVED );
            return ETrue;// if the control comes here..move was successful...
            }
        case ECSPPartiallySuccessful:
            {
            CCAUINGUtils::HandlePartialSuccessL();
            break;
            }
        default:
            {
            IMNoteMapper::ShowNoteL( iContactEditOperationError );
            break;
            }
        }

    CHAT_DP_TXT( "CCAContactEditor::MoveContactL() done" );
    return EFalse;// if the control comes here..move was failed...
    }


//  End of File
