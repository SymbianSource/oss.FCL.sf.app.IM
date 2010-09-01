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
* Description:  Container class for chat group view
 *
*/
#include "CCAAppSettingsSAPExt.h"

// INCLUDE FILES
#include "CCAChatViewContainer.h"
#include "MCAGroupUtilsPC.h"
#include "CCAAppUi.h"
#include "CCAApp.h"
#include "CCAVariantFactory.h"
#include "CCAUISessionManager.h"
#include "CCAMessageEditor.h"
#include "CCAChatViewControl.h"
#include "CCAInviteEditor.h"
#include "CCAMessageExtensionsHandler.h"
#include "CCAGroupUtils.h"
#include "CCAPicture.h"
#include "CCAStatusPanehandler.h"
#include "CCAUiMessageUtils.h"
#include "cautils.h"
#include "CCAChatView.h"

#include "MCAConversationMessage.h"
#include "MCALayoutInfo.h"
#include "MCASkinVariant.h"
#include "MCARecipientObserver.h"
#include "CCAPCUtils.h"
#include "IMUtils.h"
#include "IMDialogUtils.h"
#include "IMNoteMapper.h"
#include "CAExternalInterface.h"
#include "chatdebugprint.h"
#include "chatdebugassert.h"
#include "CCAContactSelectionDialog.h"

#include "chatngclient.hrh"
#include "imnginternalcrkeys.h"
#include "imprivatecrkeys.h"
#include "imvariant.hrh"
//added for showing the editing indicator
#include "CCAEditIndicator.h"
#include "MCAProcessManager.h"
#include "MCAGroupPC.h"
#include "MCAServerContactsArrayPC.h"
#include <finditemengine.h>
#include <chatNG.mbg>
#include <chatNG.rsg>
#include <csxhelp/imng.hlp.hrh>
#include <gulicon.h>
#include <AknBidiTextUtils.h> // KAknBidiExtraSpacePerLine
#include <AknsUtils.h>
#include <aknenv.h>
#include <txtrich.h>
#include <impsgroupprops.h>
#include <stringloader.h>
#include <AknQueryDialog.h>
#include <AknsConstants.h>
#include <AppLayout.cdl.h> //cdl
#include <AknLayoutScalable_Apps.cdl.h>
#include <AknFepGlobalEnums.h> // for EAknFepSoftkeySpell
#include <barsread.h>

#include "CATouchUtils.h"
#include  <AknSettingCache.h>
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

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CCAChatViewContainer::CCAChatViewContainer
// Default constructor must not leave.
// ---------------------------------------------------------
//
CCAChatViewContainer::CCAChatViewContainer( MCAViewSwitcher& aViewSwitcher,
                                            MCAGroupPC* aGroupPC,
                                            MCARecipientObserver& aRecipientObserver,
                                            CCAMessageExtensionsHandler& aMessageHandler,
                                            CEikButtonGroupContainer* aCba,
                                            TInt& aTabId ,
                                            CCAChatView * aView )

        : iRecipientObserver( aRecipientObserver ),
        iViewSwitcher( aViewSwitcher ),
        iGroupPC( aGroupPC ),
        iMessageExtensionsHandler( aMessageHandler ),
        iCba( aCba ),
        iTabId( aTabId ),
        iView( aView )
    {
    }

// ---------------------------------------------------------
// CCAChatViewContainer::NewL
// Symbian constructor can leave.
// ---------------------------------------------------------
//
CCAChatViewContainer* CCAChatViewContainer::NewL( const TRect& aRect,
                                                  MCAViewSwitcher& aViewSwitcher,
                                                  MCAGroupPC* aGroupPC,
                                                  MCARecipientObserver& aRecipientObserver,
                                                  MObjectProvider* aParent,
                                                  CCAMessageExtensionsHandler& aMessageHandler,
                                                  CEikButtonGroupContainer* aCba,
                                                  const TBool aActivateFromTabSwitch, TInt& aTabId , CCAChatView * aView )
    {
    CCAChatViewContainer* self = new ( ELeave ) CCAChatViewContainer(
        aViewSwitcher, aGroupPC , aRecipientObserver,
        aMessageHandler, aCba, aTabId, aView );

    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent, aActivateFromTabSwitch );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CCAChatViewContainer::ConstructL
// Symbian OS two phased constructor
// ---------------------------------------------------------
//
void CCAChatViewContainer::ConstructL( const TRect& aRect,
                                       MObjectProvider* aParent,
                                       const TBool aActivateFromTabSwitch )
    {
    iAppUi = static_cast<CCAAppUi*>( iEikonEnv->AppUi() );

    // This must be done first.
    SetMopParent( aParent );

    CreateWindowL();
    CCAApp* app = static_cast<CCAApp*>(
                      iEikonEnv->EikAppUi()->Application() );
    iSkinVar = app->VariantFactory()->SkinVariantL();

    iGroupId = iGroupPC->ActiveGroupId().AllocL();

    iGroupUtilsPC = iAppUi->GetProcessManager().GetGroupUtilsInterface() ;

    //Get group interface
    iIsLoggedIn = iAppUi->UISessionManager().IsLoggedIn();

    AknEditUtils::SAknEditorParameters editorParams;
    editorParams.iCursorYesNo = ETrue;
    editorParams.iAllowedToMoveInsertionPoint = ETrue;
    editorParams.iCharacterCase = EAknEditorTextCase;
    editorParams.iEditingSpace = IMUtils::MaxMsgLength();
    editorParams.iEditingWindow = KMaxLines; // max 2 lines in the editor
    editorParams.iJustification = EAknEditorAlignBidi;
    editorParams.iOverflowYesNo = EFalse;
    iEditor = new ( ELeave ) CCAMessageEditor( iMessageExtensionsHandler );
    iEditor->SetMopParent( this );
    AknEditUtils::ConstructEditingL( iEditor, editorParams );
    iEditor->SetContainerWindowL( *this );
    iEditor->SetAllowPictures( ETrue );

    iEditor->SetEdwinSizeObserver( this );
    iEditor->SetMessageEditorObserver( this );

    // create chat view control


    LoadBitmapsL();

    MCAMsgAddedToRichTxtCtrlObserver* observer = NULL;

    if ( aActivateFromTabSwitch && !iAppUi->FocusFlag() )
        {
        observer = this;
        }

    iChatView = CCAChatViewControl::NewL( aRect, *this,
                                          *iGroupPC,
                                          *iAppUi->GetProcessManager().GetSettingsInterface(),
                                          iMessageExtensionsHandler,
                                          iPrivIcon, iToMe,
                                          iUnsupported,
                                          iCorrupted,
                                          ETrue,
                                          observer
                                        );

    iChatView->SetObserver( this );


    iGroupPC->SetGroupEventObserverL( this, *iGroupId  );

    iAppUi->LayoutInfo()->SetLayoutID( TChatListBoxLayout::EWithEditor );
    iAppUi->LayoutInfo()->SetEditorVisibility( ETrue );
    SetRect( aRect );

    iEditor->SetupSkinContextL();
    iEditor->SetCursorPosL( 0, EFalse );

    if ( !aActivateFromTabSwitch )
        {
        iEditor->SetFocus( ETrue );
        iChatView->SetFocus( EFalse );
        iAppUi->SetFocusFlag( ETrue );
        }
    else
        {
        iEditor->SetFocus( EFalse );
        iChatView->SetFocus( ETrue );
        }

    iEditor->SetObserver( this );

    iRecipients = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    iSentRecipients = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );

    iIsEditorStateChanged = EFalse;

    iSendkeyInMsgList = ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
                          & EIMFeatSendkeyInMsgList );

    ActivateL();
    TBool tempFlag = aActivateFromTabSwitch;
    if ( iAppUi->FocusFlag() && aActivateFromTabSwitch )
        {
        tempFlag = EFalse;
        }

    ResetAfterSendL( tempFlag );

    CHAT_DP( D_CHAT_LIT( "CursorPos(): %d" ), iEditor->CursorPos() );
    CHAT_DP( D_CHAT_LIT( "TextLength(): %d" ), iEditor->TextLength() );
    iAppUi->AddLayoutChangeObserver( this );

    // Observe tap events
    iChatView->SetTapObserver( this, EMessageList );
    iEditor->SetTapObserver( this, EMessageEditor );
    iEditIndicator = CCAEditIndicator::NewL( *iEditor ) ;
    ifocusFlag = EFalse;
    }

// Destructor
CCAChatViewContainer::~CCAChatViewContainer()
    {

    delete iEditIndicator;
    delete iEditor;
    delete iPrivIcon;
    delete iToMe;
    delete iUnsupported;
    delete iCorrupted;
    delete iPrivToMany;
    delete iChatView;

    if ( iRecipients )
        {
        iRecipients->Reset();
        }
    delete iRecipients;

    if ( iSentRecipients )
        {
        iSentRecipients->Reset();
        }
    delete iSentRecipients;

    if ( iGroupId && iGroupPC )
        {
        TRAP_IGNORE( iGroupPC->SetGroupEventObserverL( NULL, *iGroupId ) );
        }

    delete iGroupId;

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
// CCAChatViewContainer::IsObjectSelected
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::IsObjectSelected( TBool& aIsOpenable ,
                                              TBool& aSavePossible )
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
// CCAChatViewContainer::SelectedMessage
// ---------------------------------------------------------
//
MCAConversationMessage* CCAChatViewContainer::SelectedMessage()
    {
    return iChatView->SelectedMessage();
    }

// -----------------------------------------------------------------------------
// CCAChatViewContainer::SetItemHighlight
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAChatViewContainer::SetItemHighlight( TBool aHighlight )
    {
    iChatView->SetHighlight( aHighlight );
    iItemHighlight = aHighlight;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::FetchMessages
// ---------------------------------------------------------
//
void CCAChatViewContainer::FetchMessages( TBool aFetch )
    {
    iChatView->FetchMessages( aFetch );
    }


// ---------------------------------------------------------
// CCAChatViewContainer::ResizeIcons
// ---------------------------------------------------------
//
void CCAChatViewContainer::ResizeIcons( TRect aParentRect )
    {
    TSize size( aParentRect.Height(), aParentRect.Height() );
    AknIconUtils::SetSize( iPrivIcon->Bitmap(), size );
    AknIconUtils::SetSize( iPrivToMany->Bitmap(), size );
    AknIconUtils::SetSize( iToMe->Bitmap(), size );
    iMessageExtensionsHandler.SizeChanged( size );
    }

// ---------------------------------------------------------
// CCAChatViewContainer::SelectedItem
// ---------------------------------------------------------
//
HBufC* CCAChatViewContainer::SelectedItemL()
    {
    return iChatView->SelectedItemL();
    }

// -----------------------------------------------------------------------------
// CCAChatViewContainer::SelectedItemType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAChatViewContainer::SelectedItemType()
    {
    return iChatView->SelectedItemType();
    }

// ---------------------------------------------------------
// CCAChatViewContainer::IsInvitationAllowed
// Checks if user can invite friends to this group
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::IsInvitationAllowed() const
    {
    return iGroupUtilsPC->IsJoined( *iGroupId );
    }

// ---------------------------------------------------------
// CCAChatViewContainer::InviteFriendsToGroupL
// Start invite editor for current group
// ---------------------------------------------------------
//
void CCAChatViewContainer::InviteFriendsToGroupL()
    {

    //hide tabs
    iAppUi->CAStatusPane()->ClearNaviPaneL();

    CCAInviteEditor* editor = CCAInviteEditor::NewLC(
                                  *iGroupId );

    editor->CreateAndSendInvitation(  *( iAppUi->GroupUtils() ) );
    CleanupStack::PopAndDestroy( editor );

    //Show tabs
    iAppUi->CAStatusPane()->RestoreNaviPane();

    }

// ---------------------------------------------------------
// CCAChatViewContainer::IsStopped
// This method checks if scroll mode is on or off
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::IsStopped() const
    {
    return iChatView->Highlighted();
    }

// ---------------------------------------------------------
// CCAChatViewContainer::StartScrollingL()
// This method is called when scrolling is started again
// ---------------------------------------------------------
//
void CCAChatViewContainer::StartScrollingL( const TBool aActivateFromTabSwitch/*=EFalse*/ )
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
void CCAChatViewContainer::StopScrollingL()
    {
    // unfocus editor (highlighting is enabled
    // automatically in OfferKeyEvent of ChatView)
    iEditor->SetFocus( EFalse, EDrawNow );
    iEditor->DrawDeferred();
    // Set editor cursor pos to end of text
    iEditor->SetCursorPosL( iEditor->TextLength(), EFalse );
    iChatView->SetFocus( ETrue );
    UpdateCbaL();
    }

// ---------------------------------------------------------
// CCAChatViewContainer::UpdateCbaL()
// ---------------------------------------------------------
//
void CCAChatViewContainer::UpdateCbaL()
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
        TBool dummyArgument( EFalse );
        HBufC* item = iChatView->SelectedItemL();
        CleanupStack::PushL( item );

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
// CCAChatViewContainer::SetItemEmphasize( TBool aIsEmphasized )
// This method sets the item emphasization on
// ---------------------------------------------------------
//
void CCAChatViewContainer::SetItemEmphasize( const TBool aIsEmphasized )
    {
    iIsEmphasis = aIsEmphasized;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::IsEmphasized()
// This method tells if item is emphasized or not
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::IsEmphasized() const
    {
    return iIsEmphasis;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::Editor()
// This method returns pointer to CEikGlobalTextEditor
// ---------------------------------------------------------
//
CCAMessageEditor& CCAChatViewContainer::Editor() const
    {
    return *iEditor;
    }


// ---------------------------------------------------------
// CCAConversationsContainer::ForwardToGroupL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::ForwardToGroupL()
    {
    MCAConversationMessage* message = iChatView->SelectedMessage();
    if ( !message )
        {
        // nothing selected
        return;
        }

    MCAServerContactsArrayPC* pairsArray =
        iGroupPC->GetGroupPairArrayLC();

    HBufC* emptyText = iEikonEnv->AllocReadResourceLC(
                           R_CHAT_IBOX_FRIENDS_OFFLINE );
    HBufC* title = iEikonEnv->AllocReadResourceLC(
                       R_PRIVATE_CHAT_FORWARD_TITLE_GROUP );

    TInt selectedIndex( 0 );
    TInt retVal( IMDialogUtils::DisplayListQueryDialogL( &selectedIndex,
                                                         pairsArray,
                                                         R_PRIVATE_CHAT_LIST_SELECT_RECIP_DLG,
                                                         *title,
                                                         *emptyText,
                                                         ETrue ) );
    //Note: code scanner warning is ignored for using magic numbers
    CleanupStack::PopAndDestroy( 2, emptyText ); // title, emptyText

    if ( retVal == EAknSoftkeyOk || retVal == EAknSoftkeyDone
         || retVal == EAknSoftkeySelect )
        {

        // Check if there is highlighted item
        HBufC* item = SelectedItemL();
        CleanupStack::PushL( item );

        if ( item->Length() > 0 )
            {
            // Set forwarded message, ownership is transferred
            message->SetForwardPartL( item );
            }
        else
            {
            // Clear previous forward message
            message->SetForwardPartL( NULL );
            }

        CleanupStack::PopAndDestroy( item );

        TInt err = iAppUi->GroupUtils()->JoinGroupL(
                       ETrue,
                       pairsArray->WVIdL( selectedIndex )->iWVID,
                       KNullDesC, message );

        if ( err )
            {
            CActiveScheduler::Current()->Error( err );
            }
        if ( TEnumsPC::EUnregistered == iAppUi->RetForwardTo()  )
            {
            iAppUi->SetResetForwardTo( TEnumsPC::ERegister );
            iAppUi->GroupUtils()->PrepareToSwitchBackL( iGroupId->Des(),
                                                        KUidChatView,
                                                        KUidChatView );
            }
        else if ( TEnumsPC::ERegister == iAppUi->RetForwardTo() )
            {
            iAppUi->SetResetForwardTo( TEnumsPC::EAlreadyRegistered );
            }
        }
    //always dont use CleanupStack::PopAndDestroy(pairsArray)
    //because there might be a object slicing happening
    //when the pairsArray is transferred from the creator
    //to the caller. Hence one will end up deleting the wrong ptr.
    //Hence it is better to use CleanupStack::PopAndDestroy(), as
    //we dont know wat the creator has pushed onto the CleanupStack
    CleanupStack::PopAndDestroy();//pairsArray
    }


// ---------------------------------------------------------
// CCAConversationsContainer::ForwardToContactL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::ForwardToContactL()
    {
    MCAConversationMessage* message = iChatView->SelectedMessage();
    if ( !message )
        {
        // nothing selected
        return;
        }

    MCASkinVariant* skinVar = static_cast<CCAApp*>( iAppUi->Application() )
                              ->VariantFactory()->SkinVariantL();


    CDesCArray* selectedContacts = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( selectedContacts );

    CDesCArray* selectedIdentifications = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( selectedIdentifications );

    MCASettingsPC* settingsPC = iAppUi->GetProcessManager().GetSettingsInterface();
    MCAMainViewArrayPC* mainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();

    // show selection dialog
    TBool contacts( CCAContactSelectionDialog::ShowDialogL( *selectedContacts,
                                                            *mainViewArrayPC,
                                                            *skinVar, *settingsPC,
                                                            iAppUi->MbmFullPath(),
                                                            TEnumsPC::ESingleSelect,
                                                            R_CONTACT_SELECTION_DIALOG,
                                                            *iAppUi->CAStatusPane(),
                                                            selectedIdentifications ) );
    if ( !contacts )
        {
        // no contacts
        //Note: code scanner warning is ignored for using magic numbers
        CleanupStack::PopAndDestroy( 2, selectedContacts ); // selectedContacts, selectedIdentifications
        return;
        }

    // allow tabs - suppose user would have received new messages
    iAppUi->CAStatusPane()->RestoreNaviPane();

    // handle selected contact (if any)


    TInt count( selectedContacts->MdcaCount() );
    CHAT_DP( D_CHAT_LIT( "selection count %d" ), count );

    //if( selectedContacts->Count() == 1 )
    if ( count == 1 )
        {
        TPtrC wvid( selectedContacts->MdcaPoint( 0 ) );

        // User made selection -> set temporary identification for the wv id
        MCAMainViewArrayPC* mainViewArrayPC =
            iAppUi->GetProcessManager().GetArrayInterface();


        //Add the conversation item to open chats array before switching the view
        mainViewArrayPC->InsertConversationItemL( wvid,
                                                  selectedIdentifications->MdcaPoint( 0 ) );


        // Check if there is highlighted item
        HBufC* item = SelectedItemL();
        CleanupStack::PushL( item );

        if ( item->Length() > 0 )
            {
            // Set forwarded message
            message->SetForwardPartL( item );
            }
        else
            {
            message->SetForwardPartL( NULL );
            }

        CleanupStack::PopAndDestroy( item );

        // Store message, in case it gets destroyed.
        iAppUi->SetForwardMessageL( message );

        if ( TEnumsPC::EUnregistered == iAppUi->RetForwardTo()  )
            {
            iAppUi->SetResetForwardTo( TEnumsPC::ERegister );
            iAppUi->GroupUtils()->PrepareToSwitchBackL( iGroupId->Des(),
                                                        KUidChatView,
                                                        KUidConversationsView );
            }
        else if ( TEnumsPC::ERegister == iAppUi->RetForwardTo() )
            {
            iAppUi->SetResetForwardTo( TEnumsPC::EAlreadyRegistered );
            }
        TCADnlConvView dnlConView;
        dnlConView.iWVID = wvid;
        dnlConView.iIsForwarded = ETrue;
        dnlConView.iForwardedMessage = iAppUi->ForwardMessage();
        dnlConView.iSAPChanged = EFalse;
        dnlConView.iSwitchTab = EFalse;

        TCADnlConvViewBuf convMsgBuf( dnlConView );
        CHAT_DP( D_CHAT_LIT( "**forward \"%S\" to contact %S" ),
                 &dnlConView.iForwardedMessage, &dnlConView.iWVID );

        iAppUi->ActivateLocalViewL( KUidConversationsView, KUidConvViewMsgId, convMsgBuf );


        }
    //Note: code scanner warning is ignored
    CleanupStack::PopAndDestroy( 2, selectedContacts ); // selectedContacts,selectedIdentifications
    }


// ---------------------------------------------------------
// CCAChatViewContainer::SendMessageL()
// This method handles message sending to engine
// ---------------------------------------------------------
//
void CCAChatViewContainer::SendMessageL()
    {
    if ( iAppUi->UISessionManager().IsLoggedIn() )
        {
        TInt textLength( iEditor->TextLength() );
        if ( textLength == 0 )
            {
            // setting focus back to editor window
            iEditor->SetFocus( ETrue );
            iChatView->SetFocus( EFalse );
            return;
            }

        HBufC* msgBuf = iMessageExtensionsHandler.ProcessMessageLC( *iEditor );
        TPtr msgPtr( msgBuf->Des() );
        TInt offset( 0 );

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

        // check if the message still is a whisper message,
        // did the user erase the recipient
        TBool isWhisper = ETrue;

        // the pics are in different place with mirrored layout
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            offset = msgPtr.FindC( KSemicolon );
            // the rest are privmsg header
            if ( offset >= 0 )	// if found
                {
                //1 is space
                msgPtr.Delete( 0, offset + 1 );
                }
            else
                {
                // user erased the recipient
                isWhisper = EFalse;
                }
            }
        else
            {
            if ( msgPtr[0] == CEditableText::EPictureCharacter &&
                 msgPtr[2] == CEditableText::EPictureCharacter )
                {
                offset = msgPtr.FindC( KSemicolon );
                msgPtr.Delete( 0, offset + KPictureChar );
                }
            else if ( msgPtr[0] == CEditableText::EPictureCharacter )
                {
                offset = msgPtr.FindC( KSemicolon );
                msgPtr.Delete( 0, offset + KPictureChar );
                }
            else
                {
                // user erased the recipient
                isWhisper = EFalse;
                }
            }

        // send message
        if ( msgPtr.Length() > 0 )
            {


            if ( ! isWhisper )
                {
                // no longer a whisper message, address this message to all
                iRecipients->Reset();
                }


            iGroupPC->SendMessageL( msgPtr, *iRecipients );

            TInt editorFlags = ( iEditor->AknEdwinFlags() | EAknEditorFlagNoEditIndicators );
            iEditor->SetAknEditorFlags( editorFlags );

            ResetAfterSendL();


            }

        CleanupStack::PopAndDestroy( msgBuf );
        }
    else
        {
        iAppUi->UISessionManager().LoginL( MCAUiLoginCmdCB::ELogin );
        if ( iAppUi->UISessionManager().IsLoggedIn() )
            {
            if ( iGroupUtilsPC->IsJoined( *iGroupId ) )
                {
                SendMessageL();
                }
            else
                {
                iAppUi->GroupUtils()->JoinGroupL( EFalse, *iGroupId );
                if ( iGroupUtilsPC->IsJoined( *iGroupId ) )
                    {
                    SendMessageL();
                    }
                }
            }
        }

    // for some reason we're not redrawn, so force it
    DrawNow();
    }

// ---------------------------------------------------------
// CCAChatViewContainer::SendImageL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::SendObjectL( TBool aNewFile,
                                        TNewServiceFileType aFileType /*= ENewFileServiceImage */ )
    {
    if ( iAppUi->UISessionManager().IsLoggedIn() )
        {


        // get members
        CDesCArray* members = new( ELeave )CDesCArrayFlat( 1 );
        CleanupStack::PushL( members );



        TPtrC groupid = iGroupPC->ActiveGroupId();
        TBool isGroupFound = iGroupUtilsPC->FindGroup( groupid );

        if ( isGroupFound )
            {
            // get joined members
            iGroupUtilsPC->GetParticipantsL( *members, groupid );
            }

        TInt index;
        if ( members->Find( *iAppUi->ScreenNamePtr(), index ) == KErrNone )
            {
            members->Delete( index );
            members->Compress();
            }

        members->Sort( ECmpCollated );

        CCAUiMessageUtils* utils = CCAUiMessageUtils::NewLC();


        if ( aNewFile )
            {
            // create new
            utils->SendNewFileL( aFileType, *iGroupPC,
                                 iGroupPC->ScreenName(), members, IsWhisperAllowedL() );
            }
        else
            {
            // from photos
            utils->SendImageL( *iGroupPC,
                               iGroupPC->ScreenName(), members, IsWhisperAllowedL() );
            }
        //Note: code scanner warning is ignored
        CleanupStack::PopAndDestroy( 2, members ); // utils, members
        }
    else
        {
        iAppUi->UISessionManager().LoginL( MCAUiLoginCmdCB::ELogin );
        if ( iAppUi->UISessionManager().IsLoggedIn() )
            {
            if ( iGroupUtilsPC->IsJoined( *iGroupId ) )
                {
                SendObjectL( aNewFile, aFileType );
                }
            else
                {
                iAppUi->GroupUtils()->JoinGroupL( EFalse, *iGroupId );
                if ( iGroupUtilsPC->IsJoined( *iGroupId ) )
                    {
                    SendObjectL( aNewFile, aFileType );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// CCAChatViewContainer::InitPrivateMessageL()
// Initialises privatemessage. Sets the icons to the editor
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::InitPrivateMessageL( const TDesC& aNickName,
                                                const TBool aGroupMsg )
    {
    iCurrentNickInPrivMsg = aNickName;
    iIsGroupMsgInPrivMsg = aGroupMsg;

    HBufC* nickHb =
        HBufC::NewLC( aNickName.Length() + KAknBidiExtraSpacePerLine );

    TPtr nickVisPtr( nickHb->Des() );
    nickVisPtr.Append( aNickName );

    HBufC* msg = iEditor->GetTextInHBufL();
    CleanupStack::PushL( msg );


    if ( msg )
        {
        TPtr msgPtr( msg->Des() );
        TInt scIndex( msgPtr.FindC( KSemicolon ) );
        CHAT_DP( D_CHAT_LIT( "scIndex = %d" ), scIndex );
        if ( scIndex >= 0 )
            {
            //KPictureChar is for space and icon
            TPtrC tmpMsg( msgPtr.Mid( scIndex + KPictureChar ) );
            CHAT_DP( D_CHAT_LIT( "tmpMsg:%S" ), &tmpMsg );
            *msg = tmpMsg;
            CHAT_DP( D_CHAT_LIT( "msg:%S" ), msg );
            }
        }

    TInt textLength( iEditor->TextLength() );
    //Read picture char from position zero to one
    TPtrC pictureChar( iEditor->Text()->Read( 0, 1 ) );

    CCAPicture* privIconPic = new ( ELeave ) CCAPicture(
        *CCoeEnv::Static()->SystemGc().Device(),
        iPrivIcon, -1 );

    CleanupStack::PushL( privIconPic );

    TPictureHeader privIconHeader;
    privIconHeader.iPicture = TSwizzle<CPicture>( privIconPic );

    if ( textLength > 0 && pictureChar[0] != CEditableText::EPictureCharacter )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            CreatePrivMsgAHHeaderL( privIconHeader, aGroupMsg, nickVisPtr, msg );
            }
        else
            {
            CreatePrivMsgHeaderL( privIconHeader, aGroupMsg, nickVisPtr, msg );
            }

        }
    else if ( ( pictureChar[0] == CEditableText::EPictureCharacter )  ||
              ( textLength == 0 ) )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            CreatePrivMsgAHHeaderL( privIconHeader, aGroupMsg,
                                    nickVisPtr, msg );
            }
        else
            {
            CreatePrivMsgHeaderL( privIconHeader, aGroupMsg, nickVisPtr, msg );
            }
        }

    // apply formatting for private message icon
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormat.iFontPresentation.iPictureAlignment =
        TFontPresentation::EAlignCentered;
    charFormatMask.SetAttrib( EAttFontPictureAlignment );
    iEditor->RichText()->ApplyCharFormatL( charFormat, charFormatMask,
                                           0, iEditor->TextLength() );

    iEditor->HandleTextChangedL();

    TInt nickLengthInPixels( 0 );

    TAknLayoutId layoutId;
    CAknEnv::Static()->GetCurrentLayoutId( layoutId );

    const CFont* font;
    if ( layoutId == EAknLayoutIdAPAC )
        {
        font = ApacPlain12();
        }
    else
        {
        font = LatinPlain12();
        }

    if ( !aGroupMsg )
        {
        nickLengthInPixels =  font->TextWidthInPixels( nickVisPtr );
        }

    nickLengthInPixels += ( font->TextWidthInPixels( KSpace ) ) +
                          font->TextWidthInPixels( KSemicolon );

    iEditor->SetCursorPosL( iEditor->Text()->DocumentLength(), EFalse );

    CleanupStack::Pop( privIconPic );
    //Note: code scanner warning is ignored
    CleanupStack::PopAndDestroy( 2, nickHb ); // msg, nickHb
    }

// ---------------------------------------------------------
// CCAChatViewContainer::Recipients()
// Returns recipients array
// (other items were commented in a header).
// ---------------------------------------------------------
//
CDesCArrayFlat& CCAChatViewContainer::Recipients() const
    {
    return *iRecipients;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::SentRecipients()
// Returns handle to sent recipients
// (other items were commented in a header).
// ---------------------------------------------------------
//
CDesCArrayFlat* CCAChatViewContainer::SentRecipients() const
    {
    return iSentRecipients;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::ResetAfterSendL()
// Resets arrays, lines and nick length after send
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::ResetAfterSendL( const TBool aActivateFromTabSwitch/*=EFalse*/ )
    {
    iEditor->ResetL();

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

    iIsPrivateMsgActive = EFalse;
    iCurrentNickInPrivMsg.Zero();
    iCurrentTextLenght = 0;
    iNickLength = 0;
    iRecipients->Reset();
    iEditor->EnableCcpuSupportL( ETrue );
    iEditor->SetTextLimit( IMUtils::MaxMsgLength() );
    iEditor->SetAknEditorCurrentCase( EAknEditorTextCase );
    StartScrollingL( aActivateFromTabSwitch );  // Call to update CBA
    }

// ---------------------------------------------------------
// CCAChatViewContainer::DisplayInfoNoteL()
// Displays info note for corresponding resource id
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::DisplayInfoNoteL( const TInt aResourceId )
    {
    CAknNoteDialog* infoNote = new ( ELeave ) CAknNoteDialog( CAknNoteDialog::
                                                              EConfirmationTone, CAknNoteDialog::EShortTimeout );

    infoNote->ExecuteLD( aResourceId );
    }

// ---------------------------------------------------------
// CCAChatViewContainer::ClearSelectionL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::ClearSelectionL( const TInt aSelectionLength )
    {
    HBufC* tmpMsg = iEditor->GetTextInHBufL();
    TPtr tmpPtr( tmpMsg->Des() );
    CleanupStack::PushL( tmpMsg );
    tmpPtr.Delete( 0, aSelectionLength );
    iEditor->ResetL();
    iEditor->SetTextL( &*tmpMsg );
    CleanupStack::PopAndDestroy( tmpMsg );
    iEditor->DrawNow();
    iIsPrivateMsgActive = EFalse;
    iNickLength = 0;
    iRecipients->Reset();
    }

// ---------------------------------------------------------
// CCAChatViewContainer::CreatePrivMsgHeaderL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::CreatePrivMsgHeaderL( TPictureHeader& aIconHeader,
                                                 const TBool& aGroupMsg,
                                                 TDesC& aNick,
                                                 HBufC* aMsg )
    {
    //Richtext pointer cannot be put to local variable and that's why it is used
    //via methods

    // if already exist the prefix of private message, delete it.
    if ( iNickLength > 0 && iEditor->Text()->DocumentLength() >= iNickLength )
        {
        iEditor->RichText()->DeleteL( 0, iNickLength );
        }

    // reset the prefix of private message
    TInt originalLength = iEditor->Text()->DocumentLength();

    iEditor->RichText()->InsertL( 0, KSemicolon );

    if ( aGroupMsg )
        {
        CCAPicture* privToMany = new ( ELeave ) CCAPicture(
            *CCoeEnv::Static()->SystemGc().Device(),
            iPrivToMany );

        CleanupStack::PushL( privToMany );

        TPictureHeader groupIconHeader;
        groupIconHeader.iPicture = TSwizzle<CPicture>( privToMany );

        iEditor->RichText()->InsertL( 0, groupIconHeader );
        CleanupStack::Pop( privToMany );
        }
    else
        {
        iEditor->RichText()->InsertL( 0, aNick );
        }

    iEditor->RichText()->InsertL( 0, KSpace );
    iEditor->RichText()->InsertL( 0, aIconHeader );

    iNickLength = iEditor->Text()->DocumentLength() - originalLength;
    }


// ---------------------------------------------------------
// CCAChatViewContainer::CreatePrivMsgAHHeaderL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::CreatePrivMsgAHHeaderL( TPictureHeader& aIconHeader,
                                                   const TBool& aGroupMsg,
                                                   TDesC& aNick,
                                                   HBufC* aMsg )
    {
    //Richtext pointer cannot be put to local variable and that's why it is used
    //via methods

    // if already exist the prefix of private message, delete it.
    if ( iNickLength > 0 && iEditor->Text()->DocumentLength() >= iNickLength )
        {
        iEditor->RichText()->DeleteL( 0, iNickLength );
        }

    // reset the prefix of private message
    TInt originalLength = iEditor->Text()->DocumentLength();

    iEditor->RichText()->InsertL( 0, KSemicolon );
    iEditor->RichText()->InsertL( 0, KArabicSign );

    if ( aGroupMsg )
        {
        CCAPicture* privToMany = new ( ELeave ) CCAPicture(
            *CCoeEnv::Static()->SystemGc().Device(),
            iPrivToMany );

        CleanupStack::PushL( privToMany );

        TPictureHeader groupIconHeader;
        groupIconHeader.iPicture = TSwizzle<CPicture>( privToMany );

        CleanupStack::Pop();
        iEditor->RichText()->InsertL( 0, groupIconHeader );
        }
    else
        {
        iEditor->RichText()->InsertL( 0, KArabicSign );
        iEditor->RichText()->InsertL( 0, aNick );
        }

    iEditor->RichText()->InsertL( 0, KSpace );
    iEditor->RichText()->InsertL( 0, KArabicSign );
    iEditor->RichText()->InsertL( 0, aIconHeader );
    iEditor->RichText()->InsertL( 0, KArabicSign );

    iNickLength = iEditor->Text()->DocumentLength() - originalLength;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::LoadBitmapsL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::LoadBitmapsL()
    {
    //Removed the ownership of iConnUI from this file and made it a reference
    //APPUI owns this connection UI instance and shares the same with this component.
    // get user-defined colors from SAP settings
    CCAAppSettingsSAPExt* SAPExtension = CCAAppSettingsSAPExt::NewLC( iAppUi->ConnectionUI() );
    TInt ownCol( SAPExtension->IntValueL( KIMOwnMessageColourKey ) );
    TInt otherCol( SAPExtension->IntValueL( KIMOthersMessageColourKey ) );
    CleanupStack::PopAndDestroy( SAPExtension );

    TRgb* ownRgbCol = NULL;
    TRgb* otherRgbCol = NULL;

    TBool useCustomOtherMsgColor = EFalse;
    if ( otherCol != KErrNotFound )
        {
        otherRgbCol = new ( ELeave ) TRgb( otherCol );
        CleanupStack::PushL( otherRgbCol );
        useCustomOtherMsgColor = ETrue;
        }

    TBool useCustomOwnMsgColor = EFalse;
    if ( ownCol != KErrNotFound )
        {
        ownRgbCol = new ( ELeave ) TRgb( ownCol );
        CleanupStack::PushL( ownRgbCol );
        useCustomOwnMsgColor = ETrue;
        }

    // check resource colours
    // The color settings are fetched from Resource file, and not from the Cenrep, that is the reason for using IntResourceValueFromRssL instead of IntResourceValueL
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

    CGulIcon* iconTemp;

    // if custom colors are defined, use them
    if ( useCustomOwnMsgColor )
        {
        iconTemp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_im_tome,
                                          EMbmChatngQgn_prop_im_tome_mask,
                                          iAppUi->MbmFullPath(), EFalse, ownRgbCol );
        }
    else
        {
        iconTemp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_im_tome,
                                          EMbmChatngQgn_prop_im_tome_mask,
                                          iAppUi->MbmFullPath() );
        }

    CleanupStack::PushL( iconTemp );
    IMUtils::UpdateIcon( iPrivIcon, iconTemp );
    //iconTemp is removed from CleanupStack and does not have valid value

    //it can be safely reused
    iconTemp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_im_many,
                                      EMbmChatngQgn_prop_im_many_mask,
                                      iAppUi->MbmFullPath() );
    CleanupStack::PushL( iconTemp );
    IMUtils::UpdateIcon( iPrivToMany, iconTemp );
    //iconTemp is removed from CleanupStack and does not have valid value

    //it can be safely reused
    // if custom colors are defined, use them
    if ( useCustomOtherMsgColor )
        {
        iconTemp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_im_tome,
                                          EMbmChatngQgn_prop_im_tome_mask,
                                          iAppUi->MbmFullPath(), EFalse, otherRgbCol );
        }
    else
        {
        iconTemp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_im_tome,
                                          EMbmChatngQgn_prop_im_tome_mask,
                                          iAppUi->MbmFullPath() );
        }
    CleanupStack::PushL( iconTemp );
    IMUtils::UpdateIcon( iToMe, iconTemp );
    //iconTemp is removed from CleanupStack and does not have valid value

    //it can be safely reused
    iconTemp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_image_corrupted,
                                      EMbmChatngQgn_prop_image_corrupted_mask,
                                      iAppUi->MbmFullPath() );
    CleanupStack::PushL( iconTemp );
    IMUtils::UpdateIcon( iCorrupted, iconTemp );
    //iconTemp is removed from CleanupStack and does not have valid value

    //it can be safely reused
    iconTemp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_unknown,
                                      EMbmChatngQgn_prop_unknown_mask,
                                      iAppUi->MbmFullPath() );
    CleanupStack::PushL( iconTemp );
    IMUtils::UpdateIcon( iUnsupported, iconTemp );
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
// CCAChatViewContainer::ApplicationUserSentThis() const
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::ApplicationUserSentThis() const
    {
    TBool ret( EFalse );
    MCAConversationMessage* msg = iChatView->SelectedMessage();
    if ( msg )
        {
        ret = ( msg->MessagerType() == TEnumsPC::EMessageSent );
        }
    return ret;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::Received() const
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::Received() const
    {
    TBool ret( EFalse );
    MCAConversationMessage* msg = iChatView->SelectedMessage();
    if ( msg )
        {
        ret = ( msg->MessagerType() == TEnumsPC::EMessageReceived );
        }
    return ret;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::SaveChatL()
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCAChatViewContainer::SaveChatL()
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

    logFileNamePtr.Copy( iGroupPC->GroupName().Left( KMaxLogFileName ) );

    TInt result( IMDialogUtils::DisplayTextQueryDialogL( logFileNamePtr,
                                                         RSC_CHATNG_LOGDIALOG_PROMPT, R_CHATCLIENT_LOG_NAME_QUERY,
                                                         ETrue // T9
                                                       ) );

    if ( result == EAknSoftkeyOk || result == EAknSoftkeyDone )
        {
#ifdef RD_MULTIPLE_DRIVE // show note
        TInt error = KErrNone;
        TRAP( error , iGroupPC->SaveChatL( *logFileName ) );

        if ( error != KErrNotSaved && error != KErrDiskFull )
#else
        iGroupPC->SaveChatL( *logFileName );
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

            CleanupStack::PushL( cenrep );
            CleanupStack::PopAndDestroy( cenrep );
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
// CCAChatViewContainer::SetLoggingOffL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::SetLoggingOffL()
    {
    if ( iGroupPC->IsLoggingOn() )
        {
        iAppUi->ShowWaitDialogL( R_QTN_GEN_NOTE_SAVING );


        TRAPD( err, iGroupPC->EndLoggingL() );

        iAppUi->DismissWaitDialogL( err );
        if ( err == KErrNone )
            {
            IMDialogUtils::DisplayNoteDialogL( R_QTN_RECORDING_ENDED );
            }
        }
    }

// ---------------------------------------------------------
// CCAChatViewContainer::IsWhisperAllowedL() const
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::IsWhisperAllowedL() const
    {
    CHAT_DP( D_CHAT_LIT( "CCAChatViewContainer::IsWhisperAllowedL()" ) );

    if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_GROUP_WHISPER ) )
        {
        return iWhisperingAllowed;
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------
// CCAChatViewContainer::IsUserAdminL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::IsUserAdmin() const
    {
    CHAT_DP( D_CHAT_LIT( "CCAChatViewContainer::IsUserAdminL()" ) );
    return( iGroupUtilsPC->IsAdmin( *iGroupId ) || iGroupUtilsPC->IsOwnGroup( *iGroupId ) );
    }

// ---------------------------------------------------------
// CCAChatViewContainer::IsGroupOpen()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::IsGroupOpen() const
    {

    // In error situation let the group be closed i.e. EFalse
    CHAT_DP( D_CHAT_LIT( "CCAChatViewContainer::IsGroupOpen()" ) );
    CHAT_DP( D_CHAT_LIT( "%d = iChatGroup->IsOpenL()" ), iIsOpen );
    return iIsOpen;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::ExtensionFits()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::ExtensionFits( TInt aLength )
    {
    TBool retVal( ETrue );
    if ( iEditor->MaxLength() - iEditor->TextLength() < aLength )
        {
        retVal = EFalse;
        }
    return retVal;
    }

// logic:
// 1. no groups/groups not supported --> hide "to group"
// 2. no contacts --> hide "to contact"
// 3. 1 and 2 --> hide forward menu completely
// 4. contacts, but no online contacts, show All contacts are Offline
// 5. skip always the current contact we're conversing with

// ---------------------------------------------------------
// CCAChatViewContainer::FwdContactVisible
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::FwdContactVisible()
    {
    // contacts
    TInt numContacts = iGroupPC->ContactCount();

    TBool showFwdToContact = numContacts > 0;

    CHAT_DP( D_CHAT_LIT(
                 "CCAChatViewContainer::FwdContactVisible - contacts %d, show %d" ),
             numContacts, showFwdToContact );

    return showFwdToContact;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::FwdGroupVisible
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::FwdGroupVisible()
    {
    // groups
    TBool groupsSupported = iAppUi->UISessionManager().IsSupported( CCAUISessionManager::EGroup );
    TInt numGroups = iGroupPC->GroupCount( ETrue );

    // we have to have groups supported and visible to show the forward-menu
    // If there is only one group we have to be in it so does not need to show menuitem.
    TBool showFwdToGroup = groupsSupported && ( numGroups > 1 );

    CHAT_DP( D_CHAT_LIT( "CCAChatViewContainer::FwdGroupVisible - fwd show group: %d" ), ( TInt ) showFwdToGroup );

    return showFwdToGroup;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::ReplyL()
// Reply function
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::ReplyL()
    {
    MCAConversationMessage* msg = iChatView->SelectedMessage();
    if ( msg )
        {
        TPtrC sender( msg->Sender() );
        iRecipients->AppendL( sender );
        InitPrivateMessageL( sender, EFalse );

        StartScrollingL();
        iAppUi->SetFocusFlag(ETrue);
        }
    }

// ---------------------------------------------------------
// CCAChatViewContainer::EditChatGroupPropertiesL()
// Edits chat group properties. Launches setting dialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::EditChatGroupPropertiesL()
    {

    TBool highlight = iChatView->Highlighted();

    if ( !highlight )
        {
        iEditor->SetFocus( EFalse );
        }
    
    // Prohibit the tab group at this moment
    //iAppUi->CAStatusPane()->ProhibitTabGroup( ETrue );
    
    TInt result = iAppUi->GroupUtils()->EditChatGroupPropertiesL( *iGroupId );

    //iAppUi->CAStatusPane()->RestoreNaviPane();

    if ( !highlight && result != EAknSoftkeyExit )
        {
        iEditor->SetFocus( ETrue );
        }
    }

// ---------------------------------------------------------
// CCAChatViewContainer::LeaveGroupL()
// This function starts leaving procedure
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::LeaveGroupL()
    {
    //Save group conversation if logging is on
    SetLoggingOffL();

    iAppUi->GroupUtils()->LeaveGroupL( *iGroupId );

    // Deregister group from switch back views
    iAppUi->DeRegisterSwitchBack( KUidChatView, *iGroupId );
    }

// ---------------------------------------------------------
// CCAChatViewContainer::DisplayChatInfoL()
// Displays chat info
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::DisplayChatInfoL()
    {
    iAppUi->GroupUtils()->DisplayChatInfoL( *iGroupId );
    }


// ---------------------------------------------------------
// CCAChatViewContainer::HandleEdwinSizeEventL( CEikEdwin* aEdwin,
//			TEdwinSizeEvent aEventType, TSize aDesirableEdwinSize )
// This method is called when editor is extended to two lines
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::HandleEdwinSizeEventL( CEikEdwin* /*aEdwin*/,
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
// CCAChatViewContainer::HandleTapEventL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::HandleTapEventL( TTapEvent aEvent,
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
                TBool dummyArgument( EFalse );
                if ( IsObjectSelected( ignore, dummyArgument ) )
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
            // controlId. Check the implementation.
            __CHAT_ASSERT_DEBUG( EFalse );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCAChatViewContainer::CheckIfPrivateMessageL()
// Checks if written message has private icons
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::CheckIfPrivateMessageL()
    {
    TInt cursosPos( iEditor->CursorPos() );
    //KPictureChar stands for for space and ':' character
    cursosPos -= KPictureChar;

    if ( cursosPos >= 0 && cursosPos <= iNickLength )
        {
        TPtrC text( iEditor->Text()->Read( cursosPos, 1 ) );

        if ( text[0] == KColon )
            {
            TInt selectionLength( iEditor->SelectionLength() );
            if ( selectionLength > 0 )
                {
                ClearSelectionL( selectionLength );
                iEditor->EnableCcpuSupportL( ETrue );
                }
            else
                {
                iEditor->SetSelectionL( iEditor->CursorPos(), 0 );
                iEditor->EnableCcpuSupportL( EFalse );
                }
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::DoHandleGroupPropertiesChangedL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::DoHandleGroupPropertiesChangedL()
    {
    TBool isGroupProps( EFalse );
    iGroupPC->GetPropertiesL( isGroupProps );
    if ( isGroupProps )
        {
        TPtrC groupName = iGroupPC->GroupName();
        if ( groupName.Length() > 0 )
            {
            // use name when possible
            iViewSwitcher.CAStatusPane()->SetTitleL( groupName );
            }
        else if ( iGroupId )
            {
            // GroupId is the second option
            iViewSwitcher.CAStatusPane()->SetTitleL(
                CCAPCUtils::DisplayId( *iGroupId ) );
            }
        iWhisperingAllowed = ( TEnumsPC::EImpsPropYes == iGroupPC->IsPrivateAllowed() );
        iIsOpen = ( TEnumsPC::EImpsPropYes == iGroupPC->IsOpen() );
        }
    else
        {
        // We don't know the states so assume the worst
        iWhisperingAllowed = EFalse;
        iIsOpen = EFalse;
        }


    }

// ---------------------------------------------------------
// CCAChatViewContainer::DoHandleGroupLeftL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::DoHandleGroupLeftL( TInt aReason )
    {
    // don't redraw the list box.otherwise we might crash after background to
    // foreground switch
    IMNoteMapper::ShowNoteL( aReason );
    //SwitchView and DeleteTab is called here as there can be group leave
    //because the creater has removed the group or banned the user from the group

    iAppUi->CAStatusPane()->SynchroniseAndRemoveTabGroup();
    iViewSwitcher.SwitchViewL( KUidFriendsListView );
    }

// ---------------------------------------------------------
// CCAChatViewContainer::LayoutChangedL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::LayoutChangedL( TInt aType )
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
// CCAChatViewContainer::HandleGroupLeft
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::HandleGroupLeft( TInt aReason )
    {
    CHAT_DP_TXT( "         !! CCAChatViewContainer::HandleGroupLeft !!!" );
    TRAPD( err, DoHandleGroupLeftL( aReason ) );

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// ---------------------------------------------------------
// CCAChatViewContainer::HandleGroupPropertiesChanged()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::HandleGroupPropertiesChanged()
    {
    TRAPD( err, DoHandleGroupPropertiesChangedL() );
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }


// ---------------------------------------------------------
// CCAChatViewContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCAChatViewContainer::SizeChanged()
    {

    TInt lines( iEditor->NumberOfLines() );

    if ( lines > KMaxLines ) // ignore events for more than 2 lines
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


    TAknLayoutRect wpl;
    wpl.LayoutRect( Rect(), AknLayoutScalable_Apps::im_writing_pane( 1 - lines ) );

    TAknWindowLineLayout myWPLayout( AknLayoutScalable_Apps::im_writing_pane( 1 - lines ) );

    iOutLine.LayoutRect( iAppUi->LayoutInfo()->MainPaneRect(), myWPLayout );

    iVertLine.LayoutRect( wpl.Rect(), AppLayout::IM_writing_field_elements_Line_1( lines ) );

    iHoriline.LayoutRect( wpl.Rect(), AppLayout::IM_writing_field_elements_Line_2() );

    TAknMultiLineTextLayout mlwpt = AppLayout::Multiline_IM_writing_pane_texts_Line_1( lines + 1 );

    AknLayoutUtils::LayoutEdwin( iEditor , wpl.Rect(), mlwpt, EAknsCIQsnTextColorsCG25 );
		
	FocusChanged( EDrawNow );

    DrawNow();
    }

// ---------------------------------------------------------
// CCAChatViewContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CCAChatViewContainer::CountComponentControls() const
    {
    //chatview, iEditor
    return KControlCount;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CCAChatViewContainer::ComponentControl( TInt aIndex ) const
    {
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
// CCAChatViewContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CCAChatViewContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();

    // Draw skinned background
    iSkinVar->ClearChatListBoxBackGround(
        gc, aRect,
        iSkinVar->MainPaneBackgroundContext() );

    TRect editorRect( iEditor->Rect() );
    TRect outlineRect( iOutLine.Rect() );
    TRect parent( Rect() );

    TBool skinnedDraw = iSkinVar->DrawSkinnedEditor( this, gc, parent, editorRect, outlineRect );

    if ( ! skinnedDraw )
        {
        DrawUtils::ClearBetweenRects( gc, Rect(), iEditor->Rect() );

        iOutLine.DrawOutLineRect( gc );
        iHoriline.DrawRect( gc );
        iVertLine.DrawRect( gc );
        }
    }


// ---------------------------------------------------------
// CCAChatViewContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
//												  TEventCode aType )
// Handles keyevents
// ---------------------------------------------------------
//
TKeyResponse CCAChatViewContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                   TEventCode aType )
    {
    CHAT_DP( D_CHAT_LIT( "CCAChatViewContainer::OfferKeyEventL key %d" ), aKeyEvent.iCode );

    if ( ( IsStopped() || !iEditor->IsFocused() ) )
        {
        if ( aKeyEvent.iScanCode >= '0' && aKeyEvent.iScanCode <= '9' )
            {
            // stop the automatic scrolling  and start manual
            // if user presses any of the number keys (0-9)
            StartScrollingL();
            // Key event gets passed to editor after this
            }
        }

    if ( aType == EEventKeyDown )
        {
        //if "#" is pressed start and timer and show the editing indicator
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
            if ( iView->GetIfMenuPopup() )
                {
                return EKeyWasConsumed;
                }
            if ( iEditor->IsFocused() )
                {
                iEditor->SetFocus( EFalse );
                // trapping error so that focus can be returned to editor
                TRAPD( err, SendMessageL() );
                if ( err )
                    {
                    CActiveScheduler::Current()->Error( err );
                    }
                iEditor->SetFocus( ETrue );
                }
            else if ( aKeyEvent.iCode == EKeyEnter
                      || aKeyEvent.iCode == EKeyOK
                      && iChatView->Highlighted() )
                {
                TBool ignore;
                TBool dummyArgument( EFalse );

                if ( IsObjectSelected( ignore, dummyArgument ) )
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

            if ( CheckIfPrivateMessageL() )	// if there are private icons, don't go back
                {
                return EKeyWasConsumed;
                }
            else
                {
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

                //If cursor pos is less than 2 then it is set to 2
                //KPictureChar for picture characters. (Two icons)
                if ( cursorPos < KPictureChar )
                    {
                    cursorPos = 2;
                    }

                CHAT_DP( D_CHAT_LIT( "CursorPos(): %d" ), cursorPos );
                }
            break;
            }

        case EKeyLeftArrow:
            {
            if ( !iEditor->IsFocused() ||
                 ( iEditor->CursorPos() <= iNickLength &&
                   iNickLength > 0 ) )
                {
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

        case EKeyRightArrow:
            {
            if ( !iEditor->IsFocused() )
                {
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

            TInt cursorPos( iEditor->CursorPos() );

            if ( ( cursorPos >= 0 ) &&
                 ( cursorPos < iNickLength ) &&
                 ( iNickLength > 0 ) )
                {
                TPtrC text( iEditor->Text()->Read( cursorPos, 1 ) );

                if ( text[0] == CEditableText::EPictureCharacter )
                    {
                    iEditor->SetCursorPosL( iNickLength, EFalse );
                    return EKeyWasConsumed;
                    }
                }
            else if ( cursorPos == iEditor->TextLength() )
                {
                iEditor->SetCursorPosL( iNickLength, EFalse );
                return EKeyWasConsumed;
                }
            break;
            }

        case EKeyDownArrow: // flowthrough
        case EKeyUpArrow:
            {
            if ( iEditor->IsFocused() )
                {
                // editor is focused, try to offer the key
                if ( iEditor->OfferKeyEventL( aKeyEvent, aType ) == EKeyWasConsumed )
                    {
                    // editor took it
                    return EKeyWasConsumed;
                    }
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
        }

    if ( iEditor->IsFocused() )
        {
        TKeyResponse resp = iEditor->OfferKeyEventL( aKeyEvent, aType );

        TInt cursorPos( iEditor->CursorPos() );
        if ( ( cursorPos >= 0 ) &&
             ( cursorPos < iNickLength ) &&
             ( iNickLength > 0 ) )
            {
            TPtrC text( iEditor->Text()->Read( cursorPos, 1 ) );

            if ( text[0] == CEditableText::EPictureCharacter )
                {
                iEditor->SetCursorPosL( iNickLength, EFalse );
                }
            }

        // Update CBA only when there is not a shift held down
        if ( !( aKeyEvent.iModifiers & EModifierShift ) )
            {
            UpdateCbaL();
            }
        TInt inputLang = iAvkonEnv->SettingCache().InputLanguage();         // when the language has candidate
        if ( ( inputLang != ELangPrcChinese ) && ( inputLang != ELangHongKongChinese )// pane then switching between tabs shouldnot
             && ( inputLang != ELangTaiwanChinese ) && ( inputLang != ELangJapanese ) ) // happen, when editor is empty.
            {
            if ( resp == EKeyWasNotConsumed && iEditor->TextLength() == 0
                 && aType == EEventKeyDown
                 && ( aKeyEvent.iScanCode == EStdKeyLeftArrow
                      || aKeyEvent.iScanCode == EStdKeyRightArrow ) )
                {
                StopScrollingL();
                ifocusFlag = ETrue;
                iAppUi->SetFocusFlag( ETrue );
                }
            }
        return resp;
        }

    return EKeyWasNotConsumed;
    }


// ---------------------------------------------------------
// CCAChatViewContainer::HandleMessageEditorEventL
// ---------------------------------------------------------
//
void CCAChatViewContainer::HandleMessageEditorEventL( TEditorEvent aEvent )
    {
    switch ( aEvent )
        {
        case EEditorContentCleared: // Flowthrough
            {
            // Call StartScrollingL to update CBA if needed
            StartScrollingL();
            break;
            }
        case EEditorFirstContentAdded:
            {
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
// CCAChatViewContainer::GetHelpContext
// This function is called when Help application is launched
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAChatViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
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
void CCAChatViewContainer::FocusChanged(TDrawNow aDrawNow)
	{
	if ( iEditor && iAppUi->FocusFlag() )
		{
		iEditor->SetFocus( IsFocused() );
		}
	
	CCoeControl::FocusChanged( aDrawNow );
	}

// ---------------------------------------------------------
// CCAChatViewContainer::RefreshEditorL
// ---------------------------------------------------------
//
void CCAChatViewContainer::RefreshEditorL()
    {
    iEditor->RefreshEditorL();
    }

// ---------------------------------------------------------
// CCAChatViewContainer::IsItemHighlight
// ---------------------------------------------------------
//
TBool CCAChatViewContainer::IsItemHighlight() const
    {
    return iItemHighlight;
    }

// ---------------------------------------------------------
// CCAChatViewContainer::TextRect
// ---------------------------------------------------------
//
const TRect CCAChatViewContainer::TextRect() const
    {
    TInt lines( iEditor->NumberOfLines() );

    if ( lines > KMaxLines ) // ignore events for more than 2 lines
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

// ---------------------------------------------------------
// CCAChatViewContainer::HandleControlEventL
// ---------------------------------------------------------
//
void CCAChatViewContainer::HandleControlEventL( CCoeControl* aControl,
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
// CCAChatViewContainer::SwitchTabL
// ---------------------------------------------------------
//
void CCAChatViewContainer::SwitchTabL()
    {

    if ( iGroupId )
        {

        iGroupPC->SetGroupEventObserverL( NULL, *iGroupId  );

        delete 	iGroupId;
        iGroupId = NULL;
        }

    iGroupId = iGroupPC->ActiveGroupId().AllocL();

    HandleGroupPropertiesChanged();

    iGroupPC->SetGroupEventObserverL( this, *iGroupId  );

    delete iChatView;
    iChatView = NULL;

    MCAMsgAddedToRichTxtCtrlObserver* observer = NULL;

    if ( !iAppUi->FocusFlag() )
        {
        observer = this;
        }

    iChatView = CCAChatViewControl::NewL( Rect(), *this,
                                          *iGroupPC,
                                          *iAppUi->GetProcessManager().GetSettingsInterface(),
                                          iMessageExtensionsHandler,
                                          iPrivIcon, iToMe,
                                          iUnsupported,
                                          iCorrupted,
                                          ETrue,
                                          observer
                                        );

    iChatView->SetObserver( this );

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
// CCAChatViewContainer::HandleMessageAddedL
// ---------------------------------------------------------
//
void CCAChatViewContainer::HandleMessageAddedL( const TInt aMessageCount )
    {
    //called by tabswitch. hence set the foxus to the last message
    if ( iGroupPC->MessageCount() == aMessageCount )
        {
        iChatView->MoveHighlightL( CCARichTextContainer::EPrevious );

        iChatView->UnRegisterRichTxtCtrlObserver();

        UpdateCbaL();
        }

    }

#ifdef RD_MULTIPLE_DRIVE
// -----------------------------------------------------------------------------
// CCAChatViewContainer::ShowMemorySelectionDialogL
// New actual filename for new history file.
// -----------------------------------------------------------------------------
//
TBool CCAChatViewContainer::ShowMemorySelectionDialogL()
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
        }
    
    // cenrep create error
    return EFalse;
    }


#endif

// End of File
