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
* Description:  Container class for conversations view
*
*/

#include    <AknSettingCache.h>

#include    "CAPresenceManager.h"
#include    "MCAPresence.h"
#include    "ccafetchwaiter.h"
#include    "ccasyncchecker.h"
#include    "ImpsCSPAllErrors.h"

// INCLUDE FILES
#include	"CCAConversationsContainer.h"
#include    "MCAViewSwitcher.h"
#include    "CCAMessageEditor.h"
#include    "ChatDefinitions.h"
#include    "ChatDebugPrint.h"
#include    "CCAAppUi.h"
#include    "CCAContactEditor.h"
#include    "CCAStatusPaneHandler.h"
#include    "MCABlockingPC.h"
#include    "IMDialogUtils.h"
#include    "MCARecipientObserver.h"
#include    "CAExternalinterface.h"
#include    "IMNoteMapper.h"
#include    "CCABlockingUI.h"
#include	"chatngclient.hrh"
#include	"CCAApp.h"
#include	"CCAVariantFactory.h"
#include	"MCASkinVariant.h"
#include    "CCAUISessionManager.h"
#include    "CCAMessageExtensionsHandler.h"
#include    "CCAChatViewControl.h"
#include    "IMDialogUtils.h"

#include	"TCAChatListBoxLayout.h"
#include    "IMUtils.h"
#include    "CAUtils.h"
#include    "ccawaitnote.h"
#include    "chatdebugassert.h"
#include    "CCAPCUtils.h"

#include    "MCAMessageErrorInformer.h"
#include    "MCAMessageUtils.h"
#include    "MCAMessageCreator.h"
#include    "MCAMessageContainerInfo.h"
#include    "MCAMessageUtils.h"
#include    "MCAContentProcessor.h"
#include    "MCASettingsPC.h"
#include    "MCAConversationMessage.h"
#include	"MCALayoutInfo.h"


#include    "imnginternalcrkeys.h"
#include    "imprivatecrkeys.h"
#include    "imvariant.hrh"
#include    "CCAAppSettingsSAPExt.h"


#include	"MCAUiLoginCmdCB.h"
#include	"MCAConversationPC.h"
#include	"CCAEditIndicator.h"
#include	"MCAProcessManager.h"
#include	"CCAUiMessageUtils.h"
#include    "MCALoginPC.h"
#include "MCALoginRefreshObserverPC.h"

#include    <finditemengine.h>
#include    <apgcli.h>
#include    <S32FILE.H>
#include	<npdapi.h>
#include    <chatNG.mbg>
#include    <chatNG.rsg>
#include    <aknnotedialog.h>
#include    <StringLoader.h>
#include	<csxhelp/imng.hlp.hrh>
#include	<impserrors.h>
#include 	<txtrich.h>
#include    <gulicon.h>
#include    <AknLayout.cdl.h> //cdl 
#include    <AppLayout.cdl.h> //cdl 
#include    <AknsUtils.h>
#include    <AknLayoutScalable_Apps.cdl.h>
#include    <aknnotewrappers.h>
#include    <AknFepGlobalEnums.h> // for EAknFepSoftkeySpell
#include    <barsread.h>

#include	"CATouchUtils.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
#ifdef RD_MULTIPLE_DRIVE
#include <centralrepository.h>
#include <IMPSServiceSettingsUINGInternalCRKeys.h>
#include <E32std.h>
#include <EIKAPP.H>
#include <eikappui.h>
#include <eikenv.h>
#include <eikbtgpc.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
//CONSTANTS
const TInt KErrNotSaved = -999;
#endif

// CONSTANTS
// Maximum amount of lines in the editor
const TInt KChatConvEditorMaxLines = 2;


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCAConversationsContainer::NewL
// Symbian constructor can leave.
// -----------------------------------------------------------------------------
//
CCAConversationsContainer* CCAConversationsContainer::NewL(
    const TRect& aRect,
    MCAViewSwitcher& aViewSwitcher,
    MCAConversationPC* aCCAConversationManagerPC,
    MCARecipientObserver& aRecipientObserver,
    MObjectProvider* aParent,
    CCAMessageExtensionsHandler& aMessageHandler,
    MCATitleUpdater& aTitleUpdater,
    MCAEditorContentStorage& aEditorContentStorage,
    CEikButtonGroupContainer* aCba,
    TBool aActivateFromTabSwitch,
    TInt& aTabId )
    {
    CCAConversationsContainer* self = new ( ELeave ) CCAConversationsContainer(
        aViewSwitcher, aCCAConversationManagerPC, aRecipientObserver,
        aMessageHandler, aTitleUpdater, aEditorContentStorage, aCba, aTabId );

    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent, aActivateFromTabSwitch );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAConversationsContainer::CCAConversationsContainer
// Default constructor must not leave
// -----------------------------------------------------------------------------
//
CCAConversationsContainer::CCAConversationsContainer(
    MCAViewSwitcher& aViewSwitcher,
    MCAConversationPC* aCCAConversationManagerPC,
    MCARecipientObserver& aRecipientObserver,
    CCAMessageExtensionsHandler& aMessageHandler,
    MCATitleUpdater& aTitleUpdater,
    MCAEditorContentStorage& aEditorContentStorage,
    CEikButtonGroupContainer* aCba,
    TInt& aTabId )

        : iViewSwitcher( aViewSwitcher ),
        iRecipientObserver( aRecipientObserver ),
        iIsChangeToOnlineAskedOnce( EFalse ),
        iMessageExtensionsHandler( aMessageHandler ),
        iTitleUpdater( aTitleUpdater ),
        iEditorContentStorage( aEditorContentStorage ),
        iCba( aCba ),
        iConversationPC( aCCAConversationManagerPC ),
        iTabId( aTabId )
    {
    }

// -----------------------------------------------------------------------------
// CCAConversationsContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAConversationsContainer::ConstructL( const TRect& aRect,
                                            MObjectProvider* aParent,
                                            TBool aActivateFromTabSwitch )
    {
    // for getting mbm path from CCAAppUi
    iAppUi = static_cast<CCAAppUi*>( CEikonEnv::Static()->AppUi() );
    iAppUi->AddLayoutChangeObserver( this );

    // This must be done first.
    SetMopParent( aParent );

    CreateWindowL();
    CCAApp* app = static_cast<CCAApp*>(
                      CEikonEnv::Static()->EikAppUi()->Application() );
    iSkinVar = app->VariantFactory()->SkinVariantL();

    AknEditUtils::SAknEditorParameters editorParams;
    editorParams.iCursorYesNo = ETrue;
    editorParams.iAllowedToMoveInsertionPoint = ETrue;
    editorParams.iCharacterCase = EAknEditorTextCase;
    editorParams.iEditingSpace = IMUtils::MaxMsgLength();
    editorParams.iEditingWindow = KChatConvEditorMaxLines;
    editorParams.iJustification = EAknEditorAlignBidi;
    editorParams.iOverflowYesNo = EFalse;

    iEditor = new ( ELeave ) CCAMessageEditor( iMessageExtensionsHandler );
    iEditor->SetMopParent( this );
    AknEditUtils::ConstructEditingL( iEditor, editorParams );
    iEditor->SetContainerWindowL( *this );
    iEditor->SetAllowPictures( ETrue );

    iEditor->SetEdwinSizeObserver( this );
    iEditor->SetMessageEditorObserver( this );

    LoadBitmapsL();

    MCAMsgAddedToRichTxtCtrlObserver* observer = NULL;

    if ( aActivateFromTabSwitch && !iAppUi->FocusFlag() )
        {
        observer = this;
        }

    iChatView = CCAChatViewControl::NewL( aRect, *this,
                                          *iConversationPC,
                                          *iAppUi->GetProcessManager().GetSettingsInterface(),
                                          iMessageExtensionsHandler,
                                          iFromMe,
                                          iToMe,
                                          iUnsupported,
                                          iCorrupted,
                                          ETrue,
                                          observer );

    iChatView->SetObserver( this );

    iAppUi->LayoutInfo()->SetLayoutID( TChatListBoxLayout::EWithEditor );
    iAppUi->LayoutInfo()->SetEditorVisibility( ETrue );

    iEditor->SetupSkinContextL();
    iEditor->SetCursorPosL( 0, EFalse );

    if ( !aActivateFromTabSwitch )
        {
        iEditor->SetFocus( ETrue );
        iAppUi->SetFocusFlag( ETrue );
        iChatView->SetFocus( EFalse );
        }
    else
        {
        iEditor->SetFocus( EFalse );
        iChatView->SetFocus( ETrue );
        }

    iEditor->SetObserver( this );

    iBlockingUI = CCABlockingUI::NewL( iAppUi->GetProcessManager().GetBlockingInterface() );

    iSendkeyInMsgList = ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
                          & EIMFeatSendkeyInMsgList );

    SetRect( aRect );

    ActivateL();
    TBool tempFlag = aActivateFromTabSwitch;
    if ( iAppUi->FocusFlag() && aActivateFromTabSwitch )
        {
        tempFlag = EFalse;
        }

    ResetAfterSendL( tempFlag );

    CHAT_DP( D_CHAT_LIT( "CursorPos(): %d" ), iEditor->CursorPos() );
    CHAT_DP( D_CHAT_LIT( "TextLength(): %d" ), iEditor->TextLength() );

    // Observe tap events
    iChatView->SetTapObserver( this, EMessageList );
    iEditor->SetTapObserver( this, EMessageEditor );

    iEditIndicator = CCAEditIndicator::NewL( *iEditor ) ;
    ifocusFlag = EFalse;
    }

// Destructor
CCAConversationsContainer::~CCAConversationsContainer()
    {

    delete iFromMe;
    delete iToMe;
    delete iUnsupported;
    delete iCorrupted;
    delete iChatView;

    delete iEditIndicator;
    delete iEditor;
    delete iBlockingUI;
    if ( iAppUi )
        {
        if ( !iAppUi->IsUnderDestruction() )
            {
            iAppUi->LayoutInfo()->SetEditorVisibility( EFalse );
            iAppUi->RemoveLayoutChangeObserver( this );

            }
        }
    }


// ---------------------------------------------------------
// CCAConversationsContainer::IsObjectSelected
// ---------------------------------------------------------
//
TBool CCAConversationsContainer::IsObjectSelected( TBool& aIsOpenable , TBool& aSavePossible )
    {
    MCAConversationMessage* msg = iChatView->SelectedMessage();
    if ( !msg )
        {
        // Nothing selected
        return EFalse;
        }

    TBool object = EFalse;

    if ( msg->ContentType() == TEnumsPC::EContentPicture ||
         msg->ContentType() == TEnumsPC::EContentOther )
        {
        // We have object
        object = ETrue;
        }


    switch ( msg->ContentProcessState() )
        {
        case TEnumsPC::EContentNotSupported:
        case TEnumsPC::EContentCorrupted:
        case TEnumsPC::EContentNotSupportedDrm:
            {
            // Not supported or corrupted, can't open
            aIsOpenable = EFalse;
            break;
            }
        case TEnumsPC::EThumbnailReady:
        default:
            {
            if ( msg->ContentData().Length() == 0 )
                {
                // No content, don't allow opening
                aIsOpenable = EFalse;
                }
            else
                {
                // Allow opening
                aIsOpenable = ETrue;

                //fix - refer Ui Spec Approved Version 1.0
                //(Instant Messaging NG 001 151006.pdf)
                //Section 10.2.10 Pg 131 -
                //"In case user has saved the image already or sent it
                //by himself, this option(save) is not available."
                aSavePossible = ( ( msg->MessagerType() == TEnumsPC::EMessageSent )
                                  || ( msg->IsImageSaved() ) )
                                ? EFalse : ETrue ;
                }
            }
        }

    return object;
    }

// ---------------------------------------------------------
// CCAConversationsContainer::SelectedMessage
// ---------------------------------------------------------
//
MCAConversationMessage* CCAConversationsContainer::SelectedMessage()
    {
    return iChatView->SelectedMessage();
    }

// ---------------------------------------------------------
// CCAConversationsContainer::FetchMessages
// ---------------------------------------------------------
//
void CCAConversationsContainer::FetchMessages( TBool aFetch )
    {
    iChatView->FetchMessages( aFetch );
    }

// ---------------------------------------------------------
// CCAConversationsContainer::SelectedItem()
// ---------------------------------------------------------
//
HBufC* CCAConversationsContainer::SelectedItemL()
    {
    return iChatView->SelectedItemL();
    }

// -----------------------------------------------------------------------------
// CCAConversationsContainer::SelectedItemType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAConversationsContainer::SelectedItemType()
    {
    return iChatView->SelectedItemType();
    }

// ---------------------------------------------------------
// CCAConversationsContainer::IsStopped()
// This method checks if scroll mode is on or off
// ---------------------------------------------------------
//
TBool CCAConversationsContainer::IsStopped() const
    {
    return iChatView->Highlighted();
    }

// -----------------------------------------------------------------------------
// CCAConversationsContainer::SetItemHighlightL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAConversationsContainer::SetItemHighlightL( TBool aHighlight )
    {
    iChatView->SetHighlight( aHighlight );
    iItemHighlight = aHighlight;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::StartScrollingL()
// This method is called when scrolling is started again
// ---------------------------------------------------------
//
void CCAConversationsContainer::StartScrollingL( TBool aActivateFromTabSwitch/*=EFalse*/ )
    {
    // disable highlight and set focus to editor
    if ( iChatView->Highlighted() )
        {
        iChatView->DisableHighlightL();
        }
    if ( !aActivateFromTabSwitch )
        {
        iEditor->SetFocus( ETrue, EDrawNow );
        iChatView->SetFocus( EFalse, EDrawNow );
        }
    else
        {
        iEditor->SetFocus( EFalse, EDrawNow );
        iChatView->SetFocus( ETrue, EDrawNow );
        }

    iEditor->DrawDeferred();
    UpdateCbaL();
    }

// ---------------------------------------------------------
// CCAChatViewContainer::StopScrollingL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsContainer::StopScrollingL()
    {
    // unfocus editor (highlighting is enabled
    // automatically in OfferKeyEvent of ChatView)
    iEditor->SetFocus( EFalse, EDrawNow );
    iChatView->SetFocus( ETrue, EDrawNow );
    iEditor->DrawDeferred();
    // Set editor cursor pos to end of text
    iEditor->SetCursorPosL( iEditor->TextLength(), EFalse );
    UpdateCbaL();
    }

// ---------------------------------------------------------
// CCAChatViewContainer::UpdateCbaL()
// ---------------------------------------------------------
//
void CCAConversationsContainer::UpdateCbaL()
    {
    TInt cbaRes( R_CHAT_SOFTKEYS_OPTIONS_CONTINUE__EMPTY );
    if ( !iCba )
        {
        return;
        }

    if ( !iChatView->GetMessageCount() )
        {
        //if the message list is empty
        if ( !iEditor->IsFocused() )
            {
            //if editor is not in focus then the rightSK must be "continue"
            iCba->SetCommandSetL( cbaRes );
            iCba->DrawNow();
            return;
            }
        }

    if ( iChatView->Highlighted() )
        {
        // focus on chatview
        TBool ignore;
        HBufC* item = iChatView->SelectedItemL();
        CleanupStack::PushL( item );
        TBool dummyArgument( EFalse );
        if ( IsObjectSelected( ignore, dummyArgument ) )
            {
            // openable object selected
            cbaRes = R_CHAT_SOFTKEYS_OPTIONS_CONTINUE__OPEN;
            }
        else if ( item && item->Length() > 0 )
            {
            // automatic find item selected
            cbaRes = R_CHAT_SOFTKEYS_OPTIONS_CONTINUE__CONTEXTOPTIONS;
            }
        CleanupStack::PopAndDestroy( item );
        }
    else
        {
        // focus on editor
        cbaRes = iEditor->TextLength() > 0 ?
                 R_CHAT_SOFTKEYS_OPTIONS_BACK__SEND :
                 R_CHAT_SOFTKEYS_OPTIONS_BACK__EMPTY;
        }

    // Check that there is not "Spell" in the left button
    // ControlOrNull() is needed for 3.0 backport.
    if ( !( iCba->ControlOrNull( EAknFepSoftkeySpell ) &&
            iCba->IsCommandVisible( EAknFepSoftkeySpell ) ) )
        {
        iCba->SetCommandSetL( cbaRes );
        iCba->DrawNow();
        }
    }

// ---------------------------------------------------------
// CCAConversationsContainer::Editor()
// This method returns pointer to CEikGlobalTextEditor
// ---------------------------------------------------------
//
CCAMessageEditor& CCAConversationsContainer::Editor() const
    {
    return *iEditor;
    }

TBool CCAConversationsContainer::Sending() const
    {
    return iSending;
    }

void CCAConversationsContainer::ResetSending()
    {
    iSending = EFalse;
    }


// ---------------------------------------------------------
// CCAConversationsContainer::CheckBlockedL
// ---------------------------------------------------------
//
void CCAConversationsContainer::CheckBlockedL()
    {

    MCABlockingPC* blockinInterface = iAppUi->GetProcessManager().GetBlockingInterface();

    TPtrC wvid( iConversationPC->GetReadInterfaceId() );

    // Check if target id is blocked
    if ( blockinInterface->IsContactBlocked( wvid, ETrue ) )
        {
        iAppUi->ShowWaitDialogL( R_CHAT_BLOCK_REMOVING );
        TInt errorCode = KErrNone;
        TRAPD( err, errorCode = blockinInterface->SetTemporaryGrantL(
                                    wvid, ETrue ) );
        iAppUi->DismissWaitDialogL( err );
        if ( errorCode == KErrNone )
            {
            IMDialogUtils::DisplayNoteDialogL( R_CHATCLIENT_CNOTE_UNBLOCK );
            }
        else
            {
            IMNoteMapper::ShowNoteL( errorCode );
            }
        }
    }


// ---------------------------------------------------------
// CCAConversationsContainer::SendMessageL()
// This method handles message sending to engine
// ---------------------------------------------------------
//
void CCAConversationsContainer::SendMessageL()
    {
    if ( iAppUi->UISessionManager().IsLoggedIn() )
        {
        TInt textLength( iEditor->TextLength() );
        if ( textLength == 0 )
            {
            return;
            }

        // Check status, and change status to "online" if it's "away"
        MCAPresence::TPresenceStatus curStatus =
            CAPresenceManager::InstanceL()->StatusL();

        TBool statusChangeNeeded =
            ( curStatus == MCAPresence::EAway ||
              curStatus == MCAPresence::EBusy );

        if ( statusChangeNeeded &&
             IMUtils::IntResourceValueL(
                 RSC_CHAT_VARIATION_AUTOMATIC_STATUS_CHANGE ) )
            {
            CCAWaitNote* note = CCAWaitNote::ShowWaitNoteLC(
                                    R_QTN_CHAT_CHANGE_STATUS_CHANGING );

            TInt error = CAPresenceManager::InstanceL()->ChangeStatusL(
                             MCAPresence::EOnline, KNullDesC );

            CleanupStack::PopAndDestroy(); // waitnote

            if ( error == KErrNone )
                {
                // Change status pane icon
                iAppUi->UISessionManager().SetStatusPaneIconsL();
                // show information note
                IMDialogUtils::DisplayInformationNoteL(
                    R_CHAT_STATUS_CHANGED_AUTOMATIC );
                }
            else
                {
                // change status failed
                IMDialogUtils::DisplayErrorNoteL(
                    R_QTN_CHAT_CHANGE_STATUS_FAILED );
                }
            }

        CheckReplyL();

        HBufC* msgBuf = iMessageExtensionsHandler.ProcessMessageLC( *iEditor );
        TPtr msgPtr( msgBuf->Des() );

        // Check message if there is other than empty chars
        TInt len( msgPtr.Length() );
        TBool contaisOnlyEmptyChars( ETrue );

        for ( TInt i( 0 ); ( i < len ) && contaisOnlyEmptyChars; ++i )
            {
            TPtrC c = msgPtr.Mid( i, 1 );  // next character
            TInt position = ( KEmptyCharacters() ).FindC( c );

            if ( position == KErrNotFound )
                {
                // c not found from empty characters --> c is not empty char
                contaisOnlyEmptyChars = EFalse;
                }
            }

        if ( contaisOnlyEmptyChars )
            {
            // message contains only empty character
            // don't send message and setting focus back to editor window
            iEditor->SetFocus( ETrue );
            iChatView->SetFocus( EFalse );
            CleanupStack::PopAndDestroy( msgBuf );
            return;
            }


        // Check grant list
        CheckBlockedL();

        iConversationPC->SendMessageL( msgBuf->Des() );

        TInt editorFlags = ( iEditor->AknEdwinFlags() | EAknEditorFlagNoEditIndicators );
        iEditor->SetAknEditorFlags( editorFlags );

        ResetAfterSendL();
        CleanupStack::PopAndDestroy( msgBuf ); // msgBuf
        }
    else
        {
        // Not logged in
        // 1. Start login procedure
        // 2. Wait for contact list fetching to complete
        //    before proceeding with sending (unless bgfetch is enabled)
        if ( !iLogingFlag )
            {
            iLogingFlag = ETrue;
            MCALoginPC *loginPC = iAppUi->GetProcessManager().GetLogInInterface();
            loginPC->AddObserver( this );

            TBool isloggedIn = iAppUi->UISessionManager().LoginL( MCAUiLoginCmdCB::EQueryLogin );

            if ( isloggedIn )
                {
                iWvId.Copy( iConversationPC->GetReadInterfaceId().Left(
                                iWvId.MaxLength() ) );
                iSending = ETrue; // set the sending flag, so we don't get deleted
                // Login
                }
            else
                { // Login failed, return
                ResetSending();
                loginPC->RemoveObserver();
                }
            iLogingFlag = EFalse;
            }
        }
    }

// ---------------------------------------------------------
// CCAConversationsContainer::AddToFriendsL()
// Adds to friend
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsContainer::AddToFriendsL()
    {

    CCAStatusPaneHandler& statusPane = *iViewSwitcher.CAStatusPane();

    HBufC* titleText = statusPane.Title()->AllocLC();

    // creating a new contact
    TBool contactSaved( EFalse );

    CCAContactEditor* contactEditor =
        CCAContactEditor::NewL( -1, contactSaved );
    TPtrC wvid( iConversationPC->GetReadInterfaceId() );

    contactEditor->EditContactLD( TEnumsPC::EAddToContact, KNullDesC, wvid, KNullDesC );

    statusPane.SetTitleL( *titleText );
    CleanupStack::PopAndDestroy( titleText );

    iTitleUpdater.UpdateTitleL();

    statusPane.RestoreNaviPane();

    }


// ---------------------------------------------------------
// CCAConversationsContainer::BlockUserL()
// ---------------------------------------------------------
//
void CCAConversationsContainer::BlockUserL()
    {
    TPtrC wvid( iConversationPC->GetReadInterfaceId() );

    iBlockingUI->BlockUserL( wvid );
    }

// ---------------------------------------------------------
// CCAConversationsContainer::IsBlocked()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAConversationsContainer::IsBlocked() const
    {
    TPtrC wvid( iConversationPC->GetReadInterfaceId() );
    return iAppUi->GetProcessManager().GetBlockingInterface()->IsContactBlocked( wvid );
    }

// ---------------------------------------------------------
// CCAConversationsContainer::BlockUserWVIdL()
// ---------------------------------------------------------
void CCAConversationsContainer::BlockUserWVIdL()
    {
    iBlockingUI->DisplayBlockTextQueryL();
    }
// ---------------------------------------------------------
// CCAConversationsContainer::UnBlockL()
// ---------------------------------------------------------
void CCAConversationsContainer::UnBlockL()
    {
    iBlockingUI->DisplayUnblockListQueryL();
    }
// ---------------------------------------------------------
// CCAConversationsContainer::DisplayBlockedListL()
// ---------------------------------------------------------
//
void CCAConversationsContainer::DisplayBlockedListL()
    {
    iBlockingUI->DisplayBlockedListL();
    }

// ---------------------------------------------------------
// CCAConversationsContainer::ApplicationUserSentThis()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAConversationsContainer::ApplicationUserSentThis() const
    {
    TBool ret( EFalse );
    MCAConversationMessage* message = iChatView->SelectedMessage();
    if ( message )
        {
        ret = ( message->MessagerType() == TEnumsPC::EMessageSent );
        }
    return ret;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::SetLoggingOnL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsContainer::RecordChatL()
    {
    TBool isMemorySelected = ETrue;
#ifdef RD_MULTIPLE_DRIVE // show note	
    isMemorySelected = ShowMemorySelectionDialogL();
#endif
    // if the location has not been set, don't do record
    if ( !isMemorySelected )
    	{
    	return;
    	}
    
    HBufC* logFileName = HBufC::NewLC( KMaxLogFileName );
    TPtr logFileNamePtr( logFileName->Des() );

    iConversationPC->GetLogFileName( logFileNamePtr ) ;

    TInt result( IMDialogUtils::DisplayTextQueryDialogL( logFileNamePtr,
                                                         RSC_CHATNG_LOGDIALOG_PROMPT, R_CHATCLIENT_LOG_NAME_QUERY,
                                                         ETrue // T9
                                                       ) );

    if ( result == EAknSoftkeyOk || result == EAknSoftkeyDone )
        {
        //CR ID : 10139730 Show warning when saving a chat
        ShowRecordSavingWarningL() ;
#ifdef RD_MULTIPLE_DRIVE // show note
        TInt error = KErrNone;
        TRAP( error , iConversationPC->RecordChatL( *logFileName ) );

        if ( error != KErrNotSaved && error != KErrDiskFull )
#else
        iConversationPC->RecordChatL( *logFileName );
#endif
            {
            IMDialogUtils::DisplayConfirmationNoteL( R_QTN_CHAT_SAVING_SAVED );
            }
#ifdef RD_MULTIPLE_DRIVE
        else if ( error == KErrDiskFull )
            {
            TBuf< KMaxPath > defaultDrive;

            CRepository* cenrep = CRepository::NewL( KWVSettingsCenRepUid );

            TInt err = cenrep->Get( KIMPSCRIMDefaultMemoryDrive, defaultDrive );

            delete cenrep;
            cenrep = NULL;

            if ( err != KErrNone )
                {
                User::Leave( err );
                }
            TInt driveNo = 0;
            err = RFs::CharToDrive( defaultDrive[0], driveNo );
            if ( err != KErrNone )
                {
                User::Leave( err );
                }
            TDriveNumber driveNumber = TDriveNumber( driveNo );

            RFs& fs = CCoeEnv::Static()->FsSession();

            TDriveInfo driveInfo;
            err = fs.Drive( driveInfo, driveNumber );
            if ( err != KErrNone )
                {
                User::Leave( err );
                }
            if ( driveInfo.iDriveAtt & KDriveAttRemovable )
                {
                HBufC* prompt = iEikonEnv->AllocReadResourceLC( R_CHAT_MEMLO_MMC_NOT_ENOUGH_MEMORY );
                IMDialogUtils::DisplayErrorNoteL( *prompt, ETrue );
                CleanupStack::PopAndDestroy( prompt );
                }
            else
                {
                HBufC* prompt = iEikonEnv->AllocReadResourceLC( R_CHAT_MEMLO_NOT_ENOUGH_MEMORY );
                IMDialogUtils::DisplayErrorNoteL( *prompt, ETrue );
                CleanupStack::PopAndDestroy( prompt );
                }

            }
#endif
        }
    CleanupStack::PopAndDestroy( logFileName );
    }



// ---------------------------------------------------------
// CCAConversationsContainer::ExtensionFits()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAConversationsContainer::ExtensionFits( TInt aLength )
    {
    TBool retVal( ETrue );
    if ( iEditor->MaxLength() - iEditor->TextLength() < aLength )
        {
        retVal = EFalse;
        }
    return retVal;
    }

// ---------------------------------------------------------
// CCAConversationsContainer::ResetAfterSendL()
// Resets arrays, lines and nick length after send
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsContainer::ResetAfterSendL( TBool aActivateFromTabSwitch/*=EFalse*/  )
    {
    iEditor->ResetL();
    iEditor->SetTextLimit( IMUtils::MaxMsgLength() );
    iEditor->SetAknEditorCurrentCase( EAknEditorTextCase );
    StartScrollingL( aActivateFromTabSwitch );  // Call to update CBA
    }

// ---------------------------------------------------------
// CCAConversationsContainer::DisplayInfoNoteL()
// Displays info note for corresponding resource id
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsContainer::DisplayInfoNoteL( const TInt aResourceId )
    {
    CAknNoteDialog* infoNote = new ( ELeave ) CAknNoteDialog( CAknNoteDialog::
                                                              EConfirmationTone, CAknNoteDialog::EShortTimeout );

    infoNote->ExecuteLD( aResourceId );
    }


// ---------------------------------------------------------
// CCAConversationsContainer::LayoutChangedL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsContainer::LayoutChangedL( TInt aType )
    {
    LoadBitmapsL();
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    SetRect( mainPaneRect );

    // Update skin context of editor
    if ( iEditor )
        {
        iEditor->SetupSkinContextL( ETrue );
        }

    CCoeControl::HandleResourceChange( aType );
    }


// ---------------------------------------------------------
// CCAConversationsContainer::HandleEdwinSizeEventL( CEikEdwin* aEdwin,
//			TEdwinSizeEvent aEventType, TSize aDesirableEdwinSize )
// This method is called when editor is extended to two lines
// ---------------------------------------------------------
//
TBool CCAConversationsContainer::HandleEdwinSizeEventL( CEikEdwin* /*aEdwin*/,
                                                        TEdwinSizeEvent aEventType, TSize /*aDesirableEdwinSize*/ )
    {
    if ( aEventType == EEventSizeChanging )
        {
        SizeChanged();

        DrawNow();
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CCAConversationsContainer::HandleTapEventL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsContainer::HandleTapEventL( TTapEvent aEvent,
                                                 TUint aControlId )
    {
    // Handle the tap events which occur in our child controls:
    // Message list and Message editor.
    switch ( aControlId )
        {
        case EMessageList:
            {
            // Focused message was double tapped
            if ( aEvent == MCATapEventObserver::EDoubleTap )
                {
                // Activate tapped item
                TBool ignore;
                TBool dummyArgument;
                if ( IsObjectSelected( ignore , dummyArgument ) )
                    {
                    iAppUi->ProcessCommandL( EChatClientOpenObject );
                    }

                }
            // Message or item was tapped
            else if ( aEvent == MCATapEventObserver::ESingleTap )
                {
                // Find objects activate with single tap
                iRecipientObserver.ShowPopUpMenuL();
                }
            break;
            }
        case EMessageEditor:
            {
            // Nothing to do here ATM.
            // Focus transition is handled in HandleControlEventL
            break;
            }
        default:
            {
            // There is an unhandled object or the object returned wrong
            // controlId. Check the implementation
            __CHAT_ASSERT_DEBUG( EFalse );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCAConversationsContainer::HandleControlEventL
// ---------------------------------------------------------
//
void CCAConversationsContainer::HandleControlEventL( CCoeControl* aControl,
                                                     TCoeEvent aEventType )
    {
    if ( EEventRequestFocus == aEventType )
        {
        if ( aControl == ( CCoeControl* )iEditor )
            {
            // Focus to editor
            StartScrollingL();
            iAppUi->SetFocusFlag( ETrue );
            }
        else // aControl == iChatView
            {
            // Focus to messagelist
            StopScrollingL();
            iAppUi->SetFocusFlag( EFalse );
            }
        }

    if ( CATouchUtils::PenEnabled() )
        {

        if ( EEventStateChanged == aEventType )
            {
            if ( aControl == ( CCoeControl* )iEditor )
                {
                // Update MSK - When using virtual keyboard
                // on touch enabled devices, one is bound to receive
                // EEventStateChanged. Hence handle the same
                UpdateCbaL();
                }
            }

        }

    }

// ---------------------------------------------------------
// CCAConversationsContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCAConversationsContainer::SizeChanged()
    {
    TInt lines( iEditor->NumberOfLines() );

    if ( lines > 2 ) // ignore events for more than 2 lines
        {
        lines = 2;
        }
    lines--;	// convert to index

    TAknLayoutRect rp;
    if ( lines == 1 )
        {
        rp.LayoutRect( Rect(), AknLayoutScalable_Apps::im_reading_pane(
                           TChatListBoxLayout::EWithEditor2Lines ) );
        }
    else
        {
        rp.LayoutRect( Rect(), AknLayoutScalable_Apps::im_reading_pane(
                           TChatListBoxLayout::EWithEditor ) );
        }

    // Set icon sizes, do this before setting the rect for
    // chat view to prevent icon sizes from changing after chat
    // view rect is set.
    ResizeIcons( TextRect() );

    // set rect for chat view
    iChatView->SetRect( rp.Rect() );

    // and editor
    TAknLayoutRect wpl;
    wpl.LayoutRect( Rect(), AknLayoutScalable_Apps::im_writing_pane( 1 - lines ) );

    TAknWindowLineLayout myWPLayout( AknLayoutScalable_Apps::im_writing_pane( 1 - lines ) );

    iOutLine.LayoutRect( iAppUi->LayoutInfo()->MainPaneRect(), myWPLayout );

    iVertLine.LayoutRect( wpl.Rect(),
                          iAppUi->LayoutInfo()->IM_writing_field_elements_Line_1( lines ) );

    iHoriline.LayoutRect( wpl.Rect(),
                          iAppUi->LayoutInfo()->IM_writing_field_elements_Line_2() );

    TAknMultiLineTextLayout mlwpt =
        iAppUi->LayoutInfo()->Multiline_IM_writing_pane_texts_Line_1( lines + 1 );

    AknLayoutUtils::LayoutEdwin( iEditor, wpl.Rect(), mlwpt, EAknsCIQsnTextColorsCG25 );
    
	FocusChanged( EDrawNow );

    DrawNow();
    }

// ---------------------------------------------------------
// CCAConversationsContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CCAConversationsContainer::CountComponentControls() const
    {
    // We have 2 controls
    return 2;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CCAConversationsContainer::ComponentControl( TInt aIndex ) const
    {
    //returns control with aIndex
    switch ( aIndex )
        {
        case 0:
            {
            return iChatView;
            }
        case 1:
            {
            return iEditor;
            }
        default:
            {
            return NULL;
            }
        }
    }

// ---------------------------------------------------------
// CCAConversationsContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CCAConversationsContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();

    // Draw skinned background
    iSkinVar->ClearChatListBoxBackGround(
        gc, aRect,
        iSkinVar->MainPaneBackgroundContext() );

    TRect editorRect( iEditor->Rect() );
    TRect outlineRect( iOutLine.Rect() );
    TRect parent( Rect() );

    TBool skinnedDraw = iSkinVar->DrawSkinnedEditor(
                            this, gc, parent, editorRect, outlineRect );

    if ( ! skinnedDraw )
        {
        DrawUtils::ClearBetweenRects( gc, Rect(), iEditor->Rect() );

        iOutLine.DrawOutLineRect( gc );
        iHoriline.DrawRect( gc );
        iVertLine.DrawRect( gc );
        }

    }

// ---------------------------------------------------------
// CCAConversationsContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
//												  TEventCode aType )
// Handles keyevents
// ---------------------------------------------------------
//
TKeyResponse CCAConversationsContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    CHAT_DP( D_CHAT_LIT( "CCAConversationsContainer::OfferKeyEventL key %d" ),
             aKeyEvent.iCode );
    
    if ( aKeyEvent.iScanCode == EStdKeyYes && aType != EEventKey )
        {
        return EKeyWasConsumed;
        }
    
    if ( ( IsStopped() || !iEditor->IsFocused() ) )
        {
        if ( aKeyEvent.iScanCode >= '0' && aKeyEvent.iScanCode <= '9' )
            {
            // stop the automatic scrolling and start manual
            // if user presses any of the number keys (0-9)
            StartScrollingL();
            // Key event gets passed to editor after this
            }
        }

    if ( aType == EEventKeyDown )
        {
        if ( ( aKeyEvent.iScanCode == KHashKey ) &&
             ( iEditor->TextLength() == 0 ) )
            {

            if ( !iEditIndicator->IsIndicatorActive() )
                {
                iEditIndicator->StartTimer();
                }

            }
        }
    switch ( aKeyEvent.iCode )
        {
            // flowthrough
        case EKeyEnter:
        case EKeyOK:
        case EKeyPhoneSend:
            {
            if ( iEditor->IsFocused() )
                {
                // trapping error so that focus can be returned to editor
                TRAPD( err, SendMessageL() );
                if ( err )
                    {
                    CActiveScheduler::Current()->Error( err );
                    }
                iEditor->SetFocus( ETrue );
                iChatView->SetFocus( EFalse );
                }
            else if ( aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK
                      && iChatView->Highlighted() )
                {
                TBool ignore;
                TBool dummyArgument;
                if ( IsObjectSelected( ignore , dummyArgument ) )
                    {
                    iAppUi->ProcessCommandL( EChatClientOpenObject );
                    }
                else
                    {
                    iRecipientObserver.ShowPopUpMenuL();
                    }
                }
            else if ( iSendkeyInMsgList )
                {
                if ( aKeyEvent.iCode == EKeyPhoneSend
                     && iChatView->Highlighted()
                     && iChatView->SelectedItemType()
                     == CFindItemEngine::EFindItemSearchPhoneNumberBin )
                    {
                    // Phone number highlighted, send key pressed -> call
                    iAppUi->ProcessCommandL( EChatClientCall );
                    }
                }
            return EKeyWasConsumed;
            }

        case EKeyBackspace:
            {
            // if bitmap is selected, it is deleted "normally" in
            // iEditor's OfferKeyEvent
            if ( iEditor->SelectionLength() > 0 )
                {
                break;
                }

            TInt cursorPos( iEditor->CursorPos() );
            if ( cursorPos > 0 ) // at least one char in editor
                {
                TPtrC ptr = iEditor->RichText()->Read( ( cursorPos - 1 ), 1 );
                // if a char to be "backspaced" is a picture
                if ( ptr[ 0 ] == CEditableText::EPictureCharacter )
                    {
                    iMessageExtensionsHandler.DeleteExtensionL( *iEditor,
                                                                EMessageExtensionSmiley, cursorPos - 1 );
                    return EKeyWasConsumed;
                    }
                }

            if ( cursorPos < 2 )
                {
                cursorPos = 2;
                }

            CHAT_DP( D_CHAT_LIT( "CursorPos(): %d" ), cursorPos );
            break;
            }

        // flowthrough
        case EKeyLeftArrow:
        case EKeyRightArrow:
            {
            if ( !iEditor->IsFocused() )
                {
                //return EKeyWasConsumed;
                TKeyResponse res = iAppUi->OfferTabEventL( aKeyEvent, aType );
                if ( ifocusFlag )
                    {
                    StartScrollingL();
                    ifocusFlag = EFalse;
                    }
                else
                    {
                    iAppUi->SetFocusFlag( EFalse );
                    }
                return res;
                }
            break;
            }

        case EKeyDownArrow: // flowthrough
        case EKeyUpArrow:
            {
            if ( iEditor->IsFocused() )
                {
                // editor is focused, try to offer the key
                if ( iEditor->OfferKeyEventL( aKeyEvent, aType )
                     == EKeyWasConsumed )
                    {
                    // editor took it
                    return EKeyWasConsumed;
                    }
                }
            TInt inputLang = iAvkonEnv->SettingCache().InputLanguage();   // when the editor is empty and language has candidate pane
            if ( !iChatView->GetMessageCount() && ( ( inputLang == ELangPrcChinese )// focus should move to messagelistpane, helps in tab swithching.
                                                    || ( inputLang == ELangHongKongChinese )
                                                    || ( inputLang == ELangTaiwanChinese )
                                                    || ( inputLang == ELangJapanese ) ) )
                {
                //when message list is empty
                if ( iEditor->IsFocused() )
                    {
                    //if editor in focus, then remove the focus from
                    // editor and move the focus to message list
                    StopScrollingL();
                    iAppUi->SetFocusFlag( EFalse );
                    }
                else
                    {
                    //if editor not in focus, then remove the focus from
                    //msg list and move the focus to editor
                    StartScrollingL();
                    iAppUi->SetFocusFlag( ETrue );
                    }


                return EKeyWasConsumed;
                }
            // offer the key to our chat control
            TKeyResponse res = static_cast<CCoeControl*>( iChatView )
                               ->OfferKeyEventL( aKeyEvent, aType );

            if ( iChatView->Highlighted() )
                {
                // change the cba
                StopScrollingL();
                iAppUi->SetFocusFlag( EFalse );
                }
            else
                {
                // change the cba
                StartScrollingL();
                iAppUi->SetFocusFlag( ETrue );
                }
            return res;
            }

        default:
            {
            break;
            }
        }


    if ( iEditor->IsFocused() )
        {
        TKeyResponse res = iEditor->OfferKeyEventL( aKeyEvent, aType );

        // Update CBA only when there is not a shift held down
        if ( !( aKeyEvent.iModifiers & EModifierShift ) )
            {
            UpdateCbaL();
            }
        TInt inputLang = iAvkonEnv->SettingCache().InputLanguage();         // when the language has candidate
        if ( ( inputLang != ELangPrcChinese ) && ( inputLang != ELangHongKongChinese )//pane  then switching between tabs shouldnot
             && ( inputLang != ELangTaiwanChinese ) && ( inputLang != ELangJapanese ) ) // happen,when editor is empty.
            {

            if ( res == EKeyWasNotConsumed && iEditor->TextLength() == 0
                 && aType == EEventKeyDown
                 && ( aKeyEvent.iScanCode == EStdKeyLeftArrow
                      ||  aKeyEvent.iScanCode == EStdKeyRightArrow )
               )
                {
                StopScrollingL();
                ifocusFlag = ETrue;
                iAppUi->SetFocusFlag( ETrue );
                }
            }
        return res;
        }

    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CCAConversationsContainer::GetHelpContext
// This function is called when Help application is launched
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_CONVERSATIONS ;
    }

// ---------------------------------------------------------
// CCAConversationsContainer::FocusChanged
// This function is called when focus has been changed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsContainer::FocusChanged(TDrawNow aDrawNow)
	{
	if ( iEditor && iAppUi->FocusFlag() )
		{
		iEditor->SetFocus( IsFocused() );
		}
	
	CCoeControl::FocusChanged( aDrawNow );
	}

// logic:
// 1. no groups/groups not supported --> hide "to group"
// 2. no contacts --> hide "to contact"
// 3. 1 and 2 --> hide forward menu completely
// 4. contacts, but no online contacts, show All contacts are Offline
// 5. skip always the current contact we're conversing with

// ---------------------------------------------------------
// CCAConversationsContainer::FwdContactVisible
// ---------------------------------------------------------
//
TBool CCAConversationsContainer::FwdContactVisible()
    {
    // contacts
    TInt numContacts( iConversationPC->ContactCount() );
    TBool showFwdToContact = numContacts > 0;

    // but check if the only contact is OTHER than the contact
    // we are having a conversation with
    if ( numContacts == 1 )
        {
        TPtrC wvid( iConversationPC->GetReadInterfaceId() );
        if ( iConversationPC->FindAnyContact( wvid ) )
            {
            // found it, skip the current contact
            // in list hiding logic
            showFwdToContact = numContacts == 1 ? EFalse : ETrue;
            }
        }

    CHAT_DP( D_CHAT_LIT( "CCAConversationsContainer::FwdContactVisible\
                           - contacts %d, show %d" ),
             numContacts, showFwdToContact );

    return showFwdToContact;
    }

// ---------------------------------------------------------
// CCAConversationsContainer::FwdGroupVisible
// ---------------------------------------------------------
//
TBool CCAConversationsContainer::FwdGroupVisible()
    {
    // groups
    TBool groupsSupported = iAppUi->UISessionManager().IsSupported(
                                CCAUISessionManager::EGroup );
    TInt numGroups( 0 );

    numGroups = iConversationPC->GroupCount( ETrue ) ;

    // we have to have groups supported and visible to show the forward-menu
    TBool showFwdToGroup = groupsSupported && ( numGroups > 0 );

    CHAT_DP( D_CHAT_LIT( "CCAConversationsContainer::\
                          FwdGroupVisible - fwd show group: %d" ),
             ( TInt ) showFwdToGroup );

    return showFwdToGroup;
    }



// ---------------------------------------------------------
// CCAConversationsContainer::CheckReplyL
// ---------------------------------------------------------
//
void CCAConversationsContainer::CheckReplyL()
    {

    TPtrC wvid;
    TBool contactInlist = ETrue;

    iConversationPC->CheckReply( wvid, contactInlist );
    if ( !contactInlist )
        {
        //To Remove wv and domain part from "WV User Id"
        //if RSC_CHAT_VARIATION_HIDE_WV flag is enabled
        HBufC* text = StringLoader::LoadLC( R_QTN_CHAT_FRIEND_ADD_UNKNOWN,
                                            CCAPCUtils::DisplayId( wvid ) );
        TInt ret = IMDialogUtils::DisplayYesNoConfirmationDialogL( *text );
        CleanupStack::PopAndDestroy( text );
        if ( ret == EAknSoftkeyOk || ret == EAknSoftkeyYes )
            {
            // Launch add contact.
            AddToFriendsL();
            }
        }
    }

// ---------------------------------------------------------
// CCAChatViewContainer::ResizeIcons
// ---------------------------------------------------------
//
void CCAConversationsContainer::ResizeIcons( TRect aParentRect )
    {
    TSize size( aParentRect.Height(), aParentRect.Height() );
    AknIconUtils::SetSize( iFromMe->Bitmap(), size );
    AknIconUtils::SetSize( iToMe->Bitmap(), size );

    iMessageExtensionsHandler.SizeChanged( size );
    }


// ---------------------------------------------------------
// CCAConversationsContainer::HandleMessageEditorEventL
// ---------------------------------------------------------
//
void CCAConversationsContainer::HandleMessageEditorEventL( TEditorEvent aEvent )
    {
    switch ( aEvent )
        {
        case EEditorContentCleared:
            {
            // Call StartScrollingL to update CBA if needed
            StartScrollingL();
            break;
            }


        case EEditorFirstContentAdded:
            {
            // Call StartScrollingL to update CBA if needed
            if ( !iEditor->IsFocused() )
                {
                StartScrollingL();
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }


// ---------------------------------------------------------
// CCAConversationsContainer::LoadBitmapsL
// ---------------------------------------------------------
//
void CCAConversationsContainer::LoadBitmapsL()
    {
    //Removed the ownership of iConnUI from this file and made it a reference
    //APPUI owns this connection UI instance and shares the same with this component.
    // get user-defined colors from SAP settings
    CCAAppSettingsSAPExt* SAPExtension = CCAAppSettingsSAPExt::NewLC( iAppUi->ConnectionUI() );
    TInt ownCol( SAPExtension->IntValueL( KIMOwnMessageColourKey ) );
    TInt otherCol( SAPExtension->IntValueL( KIMOthersMessageColourKey ) );
    CleanupStack::PopAndDestroy( SAPExtension );

    // These are pointers, because it has to be given as a pointer parameter
    // to skin variant when loading icons
    TRgb* ownRgbCol = NULL;
    TRgb* otherRgbCol = NULL;

    // check if custom color is defined for incoming messages arrow
    TBool useCustomOtherMsgColor = EFalse;
    if ( otherCol != KErrNotFound )
        {
        otherRgbCol = new ( ELeave ) TRgb( otherCol );
        CleanupStack::PushL( otherRgbCol );
        useCustomOtherMsgColor = ETrue;
        }

    // check if custom color is defined for outgoing messages arrow
    TBool useCustomOwnMsgColor = EFalse;
    if ( ownCol != KErrNotFound )
        {
        ownRgbCol = new ( ELeave ) TRgb( ownCol );
        CleanupStack::PushL( ownRgbCol );
        useCustomOwnMsgColor = ETrue;
        }

    // check resource colours
    // The color settings is fetched directly from the REsource file, and its not part of Cenrep, that is the reason for using IntResourceValueFromRssL
    TBool defaultColors = IMUtils::IntResourceValueFromRssL(
                              RSC_CHAT_DEFAULT_MESSAGE_COLORS );

    if ( !useCustomOwnMsgColor && !defaultColors )
        {
        // user-defined color not defined and resource colour in use
        TResourceReader reader;
        CCoeEnv::Static()->CreateResourceReaderLC( reader,
                                                   RSC_CHAT_MESSAGE_COLOR );
        TUint8 red( reader.ReadUint8() );
        TUint8 green( reader.ReadUint8() );
        TUint8 blue( reader.ReadUint8() );

        CleanupStack::PopAndDestroy(); // reader

        if ( !ownRgbCol )
            {
            ownRgbCol = new ( ELeave ) TRgb( red, green, blue );
            CleanupStack::PushL( ownRgbCol );
            }
        else
            {
            *ownRgbCol = TRgb( red, green, blue );
            }

        useCustomOwnMsgColor = ETrue;
        }

    if ( !useCustomOtherMsgColor && !defaultColors )
        {
        // user-defined color not defined and resource colour in use
        TResourceReader reader;
        CCoeEnv::Static()->CreateResourceReaderLC( reader,
                                                   RSC_CHAT_OWN_MESSAGE_COLOR );
        TUint8 red( reader.ReadUint8() );
        TUint8 green( reader.ReadUint8() );
        TUint8 blue( reader.ReadUint8() );

        CleanupStack::PopAndDestroy(); // reader

        if ( !otherRgbCol )
            {
            otherRgbCol = new ( ELeave ) TRgb( red, green, blue );
            CleanupStack::PushL( otherRgbCol );
            }
        else
            {
            *otherRgbCol = TRgb( red, green, blue );
            }

        useCustomOwnMsgColor = ETrue;
        }

    TBool overrideBrand( AknLayoutUtils::LayoutMirrored() );

    CGulIcon* temp;

    // if custom colors are defined, use them
    if ( useCustomOwnMsgColor )
        {
        temp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_im_from,
                                      EMbmChatngQgn_prop_im_from_mask,
                                      iAppUi->MbmFullPath(),
                                      overrideBrand, ownRgbCol );
        }
    else
        {
        temp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_im_from,
                                      EMbmChatngQgn_prop_im_from_mask,
                                      iAppUi->MbmFullPath() );
        }

    CleanupStack::PushL( temp );
    IMUtils::UpdateIcon( iFromMe, temp );
    //iconTemp is removed from CleanupStack and does not have valid value

    //it can be safely reused
    // if custom colors are defined, use them
    if ( useCustomOtherMsgColor )
        {
        temp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_im_tome,
                                      EMbmChatngQgn_prop_im_tome_mask,
                                      iAppUi->MbmFullPath(),
                                      overrideBrand, otherRgbCol );
        }
    else
        {
        temp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_im_tome,
                                      EMbmChatngQgn_prop_im_tome_mask,
                                      iAppUi->MbmFullPath() );
        }

    CleanupStack::PushL( temp );
    IMUtils::UpdateIcon( iToMe, temp );
    //iconTemp is removed from CleanupStack and does not have valid value


    //it can be safely reused
    temp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_image_corrupted,
                                  EMbmChatngQgn_prop_image_corrupted_mask,
                                  iAppUi->MbmFullPath() );
    CleanupStack::PushL( temp );
    IMUtils::UpdateIcon( iCorrupted, temp );
    //iconTemp is removed from CleanupStack and does not have valid value

    //it can be safely reused
    temp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_unknown,
                                  EMbmChatngQgn_prop_unknown_mask,
                                  iAppUi->MbmFullPath() );
    CleanupStack::PushL( temp );
    IMUtils::UpdateIcon( iUnsupported, temp );
    //iconTemp is removed from CleanupStack and does not have valid value

    if ( useCustomOwnMsgColor )
        {
        CleanupStack::PopAndDestroy(); // ownRgbCol
        }

    if ( useCustomOtherMsgColor )
        {
        CleanupStack::PopAndDestroy(); // otherRgbCol
        }

    }

// ---------------------------------------------------------
// CCAConversationsContainer::RefreshEditorL
// ---------------------------------------------------------
//
void CCAConversationsContainer::RefreshEditorL()
    {
    iEditor->RefreshEditorL();
    }

// ---------------------------------------------------------
// CCAConversationsContainer::IsItemHighlight
// ---------------------------------------------------------
//
TBool CCAConversationsContainer::IsItemHighlight() const
    {
    return iItemHighlight;
    }

// ---------------------------------------------------------
// CCAConversationsContainer::TextRect
// ---------------------------------------------------------
//
const TRect CCAConversationsContainer::TextRect() const
    {
    TInt lines( iEditor->NumberOfLines() );

    if ( lines > 2 ) // ignore events for more than 2 lines
        {
        lines = 2;
        }
    lines--;	// convert to index

    TAknLayoutRect rp;
    if ( lines == 1 )
        {
        rp.LayoutRect( Rect(), AknLayoutScalable_Apps::im_reading_pane( 0
                                                                        /*TChatListBoxLayout::EWithEditor2Lines*/ ) );
        }
    else
        {
        rp.LayoutRect( Rect(), AknLayoutScalable_Apps::im_reading_pane( 1
                                                                        /*TChatListBoxLayout::EWithEditor*/ ) );
        }

    TAknLayoutText text;
    text.LayoutText( rp.Rect(),
                     AknLayoutScalable_Apps::im_reading_pane_t1( 1 ).LayoutLine() );

    return text.TextRect();
    }


//added for multitab support

// ---------------------------------------------------------
// CCAConversationsContainer::SwitchViewL
// ---------------------------------------------------------
//
void CCAConversationsContainer::SwitchViewL( )
    {
    delete iChatView;
    //iChatView is assigned NULL as NewL may leave
    iChatView = NULL;

    //when user navigate among tab.
    MCAMsgAddedToRichTxtCtrlObserver* observer = NULL;

    if ( !iAppUi->FocusFlag() )
        {
        observer = this;
        }

    iChatView = CCAChatViewControl::NewL( Rect(), *this,
                                          *iConversationPC,
                                          *iAppUi->GetProcessManager().GetSettingsInterface(),
                                          iMessageExtensionsHandler,
                                          iFromMe,
                                          iToMe,
                                          iUnsupported,
                                          iCorrupted,
                                          ETrue,
                                          observer );

    iChatView->SetObserver( this );

    UpdateCbaL();

    SizeChanged();

    // Observe tap events
    iChatView->SetTapObserver( this, EMessageList );

    iAppUi->LayoutInfo()->SetLayoutID( TChatListBoxLayout::EWithEditor );
    iAppUi->LayoutInfo()->SetEditorVisibility( ETrue );

    if ( iAppUi->FocusFlag() )
        {
        StartScrollingL();
        }
    }

// ---------------------------------------------------------
// CCAConversationsContainer::HandleLoginRefreshCompleteL
// ---------------------------------------------------------
//
void CCAConversationsContainer::HandleLoginRefreshCompleteL( )
    {
    // Check if chat is deleted, happens when user
    // selected to login to different server.
    // Use case:
    // - user was logged in to server 1
    // - user had on going conversation with another user in server 1
    // - user logged out from server 1
    // - user opens the conversation and tries to send a message to the another user
    // - user selects server 2 to login
    // ==> all open conversations are deleted during login procedure
    if ( iChatView->IsDeleted() )
        {
        iConversationPC->SetActiveConversationL( iWvId );

        HBufC* msgBuf = iMessageExtensionsHandler.ProcessMessageLC( *iEditor );
        iEditorContentStorage.StoreContent( msgBuf ); // Ownership changed
        CleanupStack::Pop( msgBuf );

        iTabId = iAppUi->CAStatusPane()->ResetAndShowCurrentTabL( TEnumsPC::EConversationItem,
                                                                  iWvId ) ;
        ResetSending();

        SwitchViewL();

        SendMessageL();

        }
    else
        {
        // Contacts are now available, update titlebar
        iTitleUpdater.UpdateTitleL();
        // Proceed with message sending
        SendMessageL();
        ResetSending();
        }
    MCALoginPC *loginPC = iAppUi->GetProcessManager().GetLogInInterface();
    loginPC->RemoveObserver();
    }

// ---------------------------------------------------------
// CCAConversationsContainer::ShowRecordSavingWarningL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAConversationsContainer::ShowRecordSavingWarningL() const
    {
    if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_RECORD_WARNING ) )
        {
        HBufC* warningText = CCoeEnv::Static()->AllocReadResourceLC(
                                 R_CHAT_RECORD_WARNING );

        HBufC* warningHeading = CCoeEnv::Static()->AllocReadResourceLC(
                                    R_CHAT_RECORD_WARNING_HEADING );

        IMDialogUtils::DisplayMessageQueryDialogL(
            R_CHATCLIENT_MESSAGE_QUERY_DIALOG,
            *warningText,
            *warningHeading
        );

        CleanupStack::PopAndDestroy( 2, warningText );
        }

    }

// ---------------------------------------------------------
// CCAConversationsContainer::HandleMessageAddedL
// ---------------------------------------------------------
//
void CCAConversationsContainer::HandleMessageAddedL( const TInt aMessageCount )
    {
    //called by tabswitch. hence set the foxus to the last message
    if ( iConversationPC->MessageCount() == aMessageCount )
        {
        iChatView->MoveHighlightL( CCARichTextContainer::EPrevious );

        iChatView->UnRegisterRichTxtCtrlObserver();

        UpdateCbaL();
        }

    }

#ifdef RD_MULTIPLE_DRIVE
// -----------------------------------------------------------------------------
// CCAConversationsContainer::ShowMemorySelectionDialogL
// New actual filename for new history file.
// -----------------------------------------------------------------------------
//
TBool CCAConversationsContainer::ShowMemorySelectionDialogL()
    {
    CHAT_DP_TXT( "CCALoggerManager::ShowMemorySelectionDialogL" );

    ///////// read settings from cenrep ///////////////

    TBuf< KMaxPath > defaultDrive;
    TBuf< KMaxPath > pathTempWithoutDrive;
    TBuf< KMaxPath > driveAndPath;

    CRepository* cenrep = NULL;
    TRAPD( err, cenrep = CRepository::NewL( KWVSettingsCenRepUid ) );

    if ( err == KErrNone )
        {
        // creation of cenrep OK
        CleanupStack::PushL( cenrep );
        err = cenrep->Get( KIMPSCRIMDefaultMemoryDrive, defaultDrive );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy( cenrep );
            cenrep = NULL;
            User::Leave( err );
            }
        // end of reading from cenrep

        TInt driveNo = 0;
        err = RFs::CharToDrive( defaultDrive[0], driveNo );
        TDriveNumber driveNumber = TDriveNumber( driveNo );

        // Check drive's media status
        TBool isDriveOk = iAppUi->GetDriveStatusL( driveNumber );
        if ( !isDriveOk )
            {

            // prompt the user to save into a different drive.......
            CAknMemorySelectionDialogMultiDrive* dlg = CAknMemorySelectionDialogMultiDrive::NewL(
                                                           ECFDDialogTypeSave,
                                                           EFalse );

            HBufC* rootBuffer = HBufC::NewLC( KMaxPath );
            TPtr rootBufferPtr ( rootBuffer->Des() );

            HBufC* defaultFolderBuffer = HBufC::NewLC( KMaxPath );
            TPtr defaultFolderBufferPtr ( defaultFolderBuffer->Des() );

            CleanupStack::PushL( dlg );
            TBool ret( dlg->ExecuteL( driveNumber, &rootBufferPtr, &defaultFolderBufferPtr ) );
            CleanupStack::PopAndDestroy( dlg );

            CleanupStack::PopAndDestroy( defaultFolderBuffer );

            if ( ret )
                {
                // update the centrep sheet also..as user changed the default folder...
                cenrep->Set( KIMPSCRIMDefaultMemoryDrive, rootBufferPtr.Left( 3 ) );
                driveAndPath.Append( rootBufferPtr.Left( 2 ) ); // append the drive root here with user selected drive...
                CleanupStack::PopAndDestroy( rootBuffer );
                CleanupStack::PopAndDestroy( cenrep );
                cenrep = NULL;
                }
            else
                {
                CleanupStack::PopAndDestroy( rootBuffer );
                CleanupStack::PopAndDestroy( cenrep );
                cenrep = NULL;
                }
    		
    		// use selected memory
    		return ret;
            }
        else

            {

            CleanupStack::PopAndDestroy( cenrep );

            cenrep = NULL;

            // use the default memory
            return ETrue;
            }
	
        }// end of cenrep error

    // cenrep create error
    return EFalse;
    }

#endif

//  End of File
