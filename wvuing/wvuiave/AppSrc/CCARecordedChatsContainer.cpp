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
* Description:  Container for recorded chat view
*
*/



// INCLUDE FILES
#include    "MCARecordedChatsPC.h"
#include    "MCARecordedChatsArrayPC.h"
#include    "IMUtils.h"
#include	"MCAConversationMessage.h"
#include	"CCAGroupUtils.h"

#include	"CCARecordedChatsContainer.h"
#include    "CCAChatViewControl.h"
#include	"MCARecordedChatProvider.h"
#include	"CCAContactSelectionDialog.h"
#include	"CCAUISessionManager.h"
#include	"MCAProcessManager.h"
#include    "MCASettingsPC.h"
#include    "MCAServerContactsArrayPC.h"

#include    "CCAMessageExtensionsHandler.h"
#include	"CAExternalInterface.h"
#include	"CCAAppUi.h"
#include	"CCAApp.h"
#include	"CCAStatusPaneHandler.h"
#include	"MCASkinVariant.h"
#include	"CCAVariantFactory.h"
#include	"TCAChatListBoxLayout.h"
#include    "MCALayoutInfo.h"
#include    "ChatDebugPrint.h"
#include	"IMDialogUtils.h"
#include    "mcarecipientobserver.h"
#include    "chatngclient.hrh"
#include    "imnginternalcrkeys.h"
#include    "IMPrivateCRKeys.h"
#include    "IMVariant.hrh"


#include    <finditemengine.h>
#include	<eikenv.h>
#include	<chatNG.rsg>
#include    <chatNG.mbg>
#include	<csxhelp/imng.hlp.hrh>
#include	<eikmenub.h>
#include	<AknLayout.cdl.h> //cdl 
#include	<AppLayout.cdl.h> //cdl 
#include	<AknsDrawUtils.h>
#include    <gulicon.h>
#include    <AknLayoutScalable_Apps.cdl.h>

#include 	"CATouchUtils.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"


// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CCARecordedChatsContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCARecordedChatsContainer* CCARecordedChatsContainer::NewL(
    const TRect& aRect,
    MCAViewSwitcher& aViewSwitcher,
    MCARecordedChatProvider& aRecChatArrayProvider,
    MCARecipientObserver& aRecipientObserver,
    MObjectProvider& aObjectProvider,
    MCASettingsPC& aSettingsPC,
    CCAAppUi* aAppUi,
    MCARecordedChatsPC& aRecordedChatsPC )
    {
    CCARecordedChatsContainer* self =
        new ( ELeave ) CCARecordedChatsContainer(   aRecChatArrayProvider,
                                                    aViewSwitcher,
                                                    aRecipientObserver,
                                                    aSettingsPC,
                                                    aAppUi,
                                                    aRecordedChatsPC );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aObjectProvider );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::CCARecordedChatsContainer
// Symbian OS default constructor
// ---------------------------------------------------------
//
CCARecordedChatsContainer::CCARecordedChatsContainer(
    MCARecordedChatProvider& aRecChatArrayProvider,
    MCAViewSwitcher& aViewSwitcher,
    MCARecipientObserver& aRecipientObserver,
    MCASettingsPC& aSettingsPC,
    CCAAppUi* aAppUi,
    MCARecordedChatsPC& aRecordedChatsPC
)
        :   iRecordedChatArrayProvider( aRecChatArrayProvider ),
        iRecordedChatsPC( aRecordedChatsPC ),
        iViewSwitcher( aViewSwitcher ),
        iRecipientObserver( aRecipientObserver ),
        iSettingsPC( aSettingsPC )
    {
    iAppUI = aAppUi ;
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::ConstructL(const TRect& aRect)
// Symbian OS two phased constructor
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::ConstructL( const TRect& aRect,
                                            MObjectProvider& aObjectProvider )
    {
    SetMopParent( &aObjectProvider );

    CreateWindowL();

    CCAApp* myApp = static_cast<CCAApp*>( iAppUI->Application() );
    iSkinVar = myApp->VariantFactory()->SkinVariantL();

    iAppUI->AddLayoutChangeObserver( this );

    // Create chat view
    LoadBitmapsL();

    iChatView = CCAChatViewControl::NewL( aRect , *this,
                                          iRecordedChatsPC,
                                          iSettingsPC,
                                          iAppUI->MessageHandler(),
                                          iFromMe,
                                          iToMe,
                                          iUnsupported,
                                          iCorrupted, EFalse, this );

    iChatView->SetObserver( this );

    iChatView->SetTapObserver( this, 0 );

    iAppUI->LayoutInfo()->SetLayoutID( TChatListBoxLayout::EWithoutEditor );
    iAppUI->LayoutInfo()->SetEditorVisibility( EFalse );

    iSendkeyInMsgList = ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
                          & EIMFeatSendkeyInMsgList );

    SetRect( aRect );
    UpdateCbaL();
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsContainer::LayoutChangedL
//
// -----------------------------------------------------------------------------
//
void CCARecordedChatsContainer::LayoutChangedL( TInt aType )
    {
    LoadBitmapsL();
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    SetRect( mainPaneRect );
    CCoeControl::HandleResourceChange( aType );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsContainer::~CCARecordedChatsListViewContainer
// destructor
// -----------------------------------------------------------------------------
//
CCARecordedChatsContainer::~CCARecordedChatsContainer()
    {
    delete iFromMe;
    delete iToMe;
    delete iUnsupported;
    delete iCorrupted;
    if ( iAppUI && !iAppUI->IsUnderDestruction() )
        {
        iAppUI->RemoveLayoutChangeObserver( this );
        }
    delete iChatView;
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::SetDefaultTitleL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::SetDefaultTitleL()
    {
    iViewSwitcher.CAStatusPane()->SetTitleL( iEikonEnv,
                                             R_CHATCLIENT_RECORDEDCHATLIST_VIEW_TITLE );
    iViewSwitcher.CAStatusPane()->ShowTabGroupL( KUidRecordedChatListView );
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::SizeChanged()
    {
    TAknLayoutRect rp;

    rp.LayoutRect( Rect(), AknLayoutScalable_Apps::im_reading_pane(
                       TChatListBoxLayout::EWithoutEditor ) );

    // set rect for chat view
    if ( iChatView )
        {
        iChatView->SetRect( rp.Rect() );
        }
    DrawNow();
    }

// ---------------------------------------------------------
// CCAConversationsContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::Draw( const TRect& aRect ) const
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    CWindowGc& myGc = SystemGc();

    if ( !AknsDrawUtils::Background( skin,
                                     iSkinVar->MainPaneBackgroundContext(),
                                     myGc, aRect ) )
        {
        myGc.Clear( aRect );
        }
    }


// ---------------------------------------------------------
// CCARecordedChatsContainer::CountComponentControls() const
// Number of components
// ---------------------------------------------------------
//
TInt CCARecordedChatsContainer::CountComponentControls() const
    {
    return iChatView ? 1 : 0; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::ComponentControl(TInt aIndex) const
// return component by index
// ---------------------------------------------------------
//
CCoeControl* CCARecordedChatsContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return iChatView;
            }
        default:
            {
            return NULL;
            }
        }
    }


// ---------------------------------------------------------
// CCARecordedChatsContainer::OfferKeyEventL()
// Handles key events
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCARecordedChatsContainer::OfferKeyEventL(
    const TKeyEvent& aEvent, TEventCode aType )
    {
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }

    switch ( aEvent.iCode )
        {
            // flowthrough
        case EKeyLeftArrow:
        case EKeyRightArrow:
            {
            return EKeyWasConsumed;
            }
        case EKeyBackspace:
            {
            if ( iRecordedChatArrayProvider.DeleteChatL() )
                {
                iViewSwitcher.SwitchViewL( KUidRecordedChatListView );
                }
            return EKeyWasConsumed;
            }

        // flowthrough
        case EKeyEnter:
        case EKeyOK:
            {
            ShowContextMenuL( EKeyActivation );
            return EKeyWasConsumed;
            }
        case EKeyPhoneSend:
            {
            if ( iSendkeyInMsgList )
                {
                if ( aEvent.iCode == EKeyPhoneSend
                     && iChatView->Highlighted()
                     && iChatView->SelectedItemType()
                     == CFindItemEngine::EFindItemSearchPhoneNumberBin )
                    {
                    // Phone number highlighted, send key pressed -> call
                    iAppUI->ProcessCommandL( EChatClientCall );
                    }
                }
            return EKeyWasConsumed;
            }
        default:
            {
            TKeyResponse res = EKeyWasNotConsumed;
            if ( iChatView )
                {
                res = static_cast<CCoeControl*>( iChatView )->OfferKeyEventL( aEvent, aType );
                }

            if ( aEvent.iCode == EKeyDownArrow || aEvent.iCode == EKeyUpArrow )
                {
                UpdateCbaL();
                }

            return res;
            }
        }
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::GetHelpContext
// This function is called when Help application is launched
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_SAVED_VIEW ;
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::HandleTapEventL
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::HandleTapEventL( TTapEvent aEvent,
                                                 TUint /*aControlId*/ )
    {
    // Forward taps to ShowContextMenuL()
    if ( aEvent == MCATapEventObserver::EDoubleTap )
        {
        ShowContextMenuL( EDoubleClick );
        }
    else if ( aEvent == MCATapEventObserver::ESingleTap )
        {
        ShowContextMenuL( ESingleClick );
        }
    }


// ---------------------------------------------------------
// CCARecordedChatsContainer::LoadBitmapsL
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::LoadBitmapsL()
    {
    //load "from me" icon
    TBool overrideBrand( AknLayoutUtils::LayoutMirrored() );

    CGulIcon* iconTemp;

    iconTemp = iSkinVar->LoadBitmapL(  EMbmChatngQgn_prop_im_from,
                                       EMbmChatngQgn_prop_im_from_mask,
                                       iAppUI->MbmFullPath(),
                                       overrideBrand );
    CleanupStack::PushL( iconTemp );
    IMUtils::UpdateIcon( iFromMe, iconTemp );
    //iconTemp is removed from CleanupStack and does not have valid value

    //it can be safely reused
    iconTemp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_im_tome,
                                      EMbmChatngQgn_prop_im_tome_mask,
                                      iAppUI->MbmFullPath(),
                                      overrideBrand );
    CleanupStack::PushL( iconTemp );
    IMUtils::UpdateIcon( iToMe, iconTemp );
    //iconTemp is removed from CleanupStack and does not have valid value


    //it can be safely reused
    iconTemp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_image_corrupted,
                                      EMbmChatngQgn_prop_image_corrupted_mask,
                                      iAppUI->MbmFullPath() );
    CleanupStack::PushL( iconTemp );
    IMUtils::UpdateIcon( iCorrupted, iconTemp );
    //iconTemp is removed from CleanupStack and does not have valid value

    //it can be safely reused
    iconTemp = iSkinVar->LoadBitmapL( EMbmChatngQgn_prop_unknown,
                                      EMbmChatngQgn_prop_unknown_mask,
                                      iAppUI->MbmFullPath() );
    CleanupStack::PushL( iconTemp );
    IMUtils::UpdateIcon( iUnsupported, iconTemp );
    //iconTemp is removed from CleanupStack and does not have valid value

    TAknLayoutRect rp;

    /*The argument 2: TChatListBoxLayout::EWithoutEditor*/
    rp.LayoutRect( Rect(), AknLayoutScalable_Apps::im_reading_pane( 2 ) );

    TAknLayoutText text;
    text.LayoutText( rp.Rect(),
                     AknLayoutScalable_Apps::im_reading_pane_t1( 1 ).LayoutLine() );

    TSize size( text.TextRect().Height(), text.TextRect().Height() );
    AknIconUtils::SetSize( iFromMe->Bitmap(), size );
    AknIconUtils::SetSize( iToMe->Bitmap(), size );
    iAppUI->MessageHandler().SizeChanged( size );
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::IsStopped()
// This method checks if scroll mode is on or off
// ---------------------------------------------------------
//
TBool CCARecordedChatsContainer::IsStopped() const
    {
    return iChatView ? iChatView->Highlighted() : EFalse;
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::SelectedItem()
// ---------------------------------------------------------
//
HBufC* CCARecordedChatsContainer::SelectedItemL()
    {
    return iChatView ? iChatView->SelectedItemL() : KNullDesC().AllocL();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsContainer::SelectedItemType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsContainer::SelectedItemType()
    {
    return iChatView ? iChatView->SelectedItemType() : KErrNotFound;
    }

// logic:
// 1. no groups/groups not supported --> hide "to group"
// 2. no contacts --> hide "to contact"
// 3. 1 and 2 --> hide forward menu completely
// 4. contacts, but no online contacts, show All contacts are Offline
// 5. skip always the current contact we're conversing with

// ---------------------------------------------------------
// CCARecordedChatsContainer::FwdContactVisible
// ---------------------------------------------------------
//
TBool CCARecordedChatsContainer::FwdContactVisible()
    {
    TInt numContacts( iRecordedChatsPC.StoredContactsCount() );

    TBool showFwdToContact = numContacts > 0;

    CHAT_DP( D_CHAT_LIT( "CCARecordedChatsContainer::FwdContactVisible\
                           - contacts %d, show %d" ),
             numContacts, showFwdToContact );

    return showFwdToContact;
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::FwdGroupVisible
// ---------------------------------------------------------
//
TBool CCARecordedChatsContainer::FwdGroupVisible()
    {
    // groups
    TBool groupsSupported = iAppUI->UISessionManager().IsSupported(
                                CCAUISessionManager::EGroup );

    TInt numGroups( 0 );
    TRAPD( err, numGroups = iRecordedChatsPC.GroupCountL( ETrue ) );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }

    // we have to have groups supported and visible to show the forward-menu
    TBool showFwdToGroup = groupsSupported && ( numGroups > 0 );

    CHAT_DP( D_CHAT_LIT( "CCARecordedChatsContainer::\
                          FwdGroupVisible - fwd show group: %d" ),
             ( TInt ) showFwdToGroup );

    return showFwdToGroup;
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::IsObjectSelected
// ---------------------------------------------------------
//
TBool CCARecordedChatsContainer::IsObjectSelected( TBool& aIsOpenable, TBool& aSavePossible )
    {
    MCAConversationMessage* msg = iChatView ? iChatView->SelectedMessage() : NULL;
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
// CCARecordedChatsContainer::SelectedMessage
// ---------------------------------------------------------
//
MCAConversationMessage* CCARecordedChatsContainer::SelectedMessage()
    {
    return iChatView ? iChatView->SelectedMessage() : NULL;
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::ForwardToGroupL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::ForwardToGroupL()
    {
    MCAConversationMessage* message = SelectedMessage();
    if ( !message )
        {
        // nothing selected
        return;
        }


    /**
    * 1) PairsArray is Owned. Ownership is transferred from
    * 2) There's no need to skip groups in this case. but we do need to make sure
         the pairs are in correct order after sorting
    * 3) We can use CCAServerContactsArray, as we deal with descriptor tuple
         just as with contacts. the class naming is a bit misleading
    */
    MCAServerContactsArrayPC* pairsArray
    = iRecordedChatsPC.GetPairArrayForServerContactsArrayLC();


    HBufC* emptyText = iEikonEnv->AllocReadResourceLC(
                           R_CHAT_IBOX_FRIENDS_OFFLINE );
    HBufC* title = iEikonEnv->AllocReadResourceLC(
                       R_PRIVATE_CHAT_FORWARD_TITLE_GROUP );

    TInt selectedIndex( 0 );
    TInt retVal( IMDialogUtils::DisplayListQueryDialogL( &selectedIndex,
                                                         pairsArray,
                                                         R_PRIVATE_CHAT_LIST_SELECT_RECIP_DLG,
                                                         *title,
                                                         *emptyText, ETrue ) );

    CleanupStack::PopAndDestroy( 2, emptyText ); // title, emptyText

    if ( retVal == EAknSoftkeyOk || retVal == EAknSoftkeyDone
         || retVal == EAknSoftkeySelect )
        {
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
            // Clear previous forward part
            message->SetForwardPartL( NULL );
            }

        CleanupStack::PopAndDestroy( item ); //item
        //Bound checking on pairsArray
        if ( selectedIndex >= 0 && selectedIndex < pairsArray->MdcaCount() )
            {
            TInt err( iAppUI->GroupUtils()->JoinGroupL( EFalse,
                                                        pairsArray->WVIdL( selectedIndex )->iWVID,
                                                        KNullDesC, message ) );

            if ( TEnumsPC::EUnregistered == iAppUI->RetForwardTo()  )
                {
                iAppUI->SetResetForwardTo( TEnumsPC::ERegister );
                iAppUI->GroupUtils()->PrepareToSwitchBackL( iRecordedChatArrayProvider.RecordedChatName(),
                                                            KUidRecordedChatView,
                                                            KUidChatView );
                }
            else if ( TEnumsPC::ERegister == iAppUI->RetForwardTo() )
                {
                iAppUI->SetResetForwardTo( TEnumsPC::EAlreadyRegistered );
                }
            if ( err )
                {
                CActiveScheduler::Current()->Error( err );
                }
            }

        }

    //always dont use CleanupStack::PopAndDestroy(pairsArray)
    //because there might be a object slicing happening
    //when the pairsArray is transferred from the creator
    //to the caller. Hence one will end up deleting the wrong ptr.
    //Hence it is better to use CleanupStack::PopAndDestroy(), as
    //we dont know wat the creator has pushed onto the CleanupStack
    CleanupStack::PopAndDestroy(); //pairsArray
    }


// ---------------------------------------------------------
// CCARecordedChatsContainer::ForwardToContactL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::ForwardToContactL()
    {
    MCAConversationMessage* message = SelectedMessage();
    if ( !message )
        {
        // nothing selected
        return;
        }
    CDesCArray* selectedContacts = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( selectedContacts );

    CDesCArray* selectedIdentifications = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( selectedIdentifications );

    MCASkinVariant* skinVar = static_cast<CCAApp*>( iAppUI->Application() )
                              ->VariantFactory()->SkinVariantL();

    // show selection dialog
    if ( !CCAContactSelectionDialog::ShowDialogL(
             *selectedContacts,
             *( iAppUI->GetProcessManager().GetArrayInterface() ),
             *skinVar,
             *( iAppUI->GetProcessManager().GetSettingsInterface() ),
             iAppUI->MbmFullPath(),
             TEnumsPC::ESingleSelect,
             R_CONTACT_SELECTION_DIALOG,
             *iViewSwitcher.CAStatusPane(),
             selectedIdentifications ) )
        {
        // no contacts
        CleanupStack::PopAndDestroy( 2, selectedContacts ); // selectedContacts,selectedIdentifications
        return;
        }


    // handle selected contact (if any)
    if ( selectedContacts->MdcaCount() == 1 )
        {
        TPtrC wvid( selectedContacts->MdcaPoint( 0 ) );//User ID

        // User made selection -> set temporary identification for the wv id
        // Get identification through temp identification handler
        // code merge 14 nov, 2006
        MCAMainViewArrayPC* mainViewArrayPC = iAppUI->GetProcessManager().GetArrayInterface();


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
            // Clear previous forward part
            message->SetForwardPartL( NULL );
            }

        CleanupStack::PopAndDestroy( item );

        // Store message, in case it gets deleted.
        iAppUI->SetForwardMessageL( message );

        TCADnlConvView dnlConView;
        dnlConView.iWVID = wvid;
        dnlConView.iIsForwarded = ETrue;
        dnlConView.iForwardedMessage = iAppUI->ForwardMessage();
        dnlConView.iSAPChanged = EFalse;
        dnlConView.iSwitchTab = EFalse;
        TCADnlConvViewBuf convMsgBuf( dnlConView );
        CHAT_DP( D_CHAT_LIT( "**forward \"%S\" to contact %S" ),
                 &dnlConView.iForwardedMessage, &dnlConView.iWVID );

        // prevent AppShell from flashing during view switch,
        // flash the main view instead
        // if refresh is required refreshview can be called.
        iViewSwitcher.SwitchViewL( KUidConversationsView, KUidConvViewMsgId,
                                   convMsgBuf );


        if ( TEnumsPC::EUnregistered == iAppUI->RetForwardTo()  )
            {
            iAppUI->SetResetForwardTo( TEnumsPC::ERegister );
            iAppUI->GroupUtils()->PrepareToSwitchBackL( iRecordedChatArrayProvider.RecordedChatName(),
                                                        KUidRecordedChatView,
                                                        KUidConversationsView );
            }
        else if ( TEnumsPC::ERegister == iAppUI->RetForwardTo() )
            {
            iAppUI->SetResetForwardTo( TEnumsPC::EAlreadyRegistered );
            }

        }

    CleanupStack::PopAndDestroy( 2, selectedContacts ); // selectedContacts, selectedIdentifications
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsContainer::SetItemHighlightL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCARecordedChatsContainer::SetItemHighlightL( TBool aHighlight )
    {
    if ( iChatView )
        {
        iChatView->SetHighlight( aHighlight );
        }
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::FreeMemoryForViewSwitch
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::FreeMemoryForViewSwitch()
    {
    delete iChatView;
    iChatView = NULL;
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::UpdateCbaL
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::UpdateCbaL()
    {
    TInt cbaRes = R_CHAT_SOFTKEYS_OPTIONS_BACK__EMPTY;  // Default: Options/Back
    TBool ignore;
    TBool unUsedParamSave = EFalse;
    HBufC* item = iChatView ? iChatView->SelectedItemL() : NULL;
    CleanupStack::PushL( item );

    if ( IsObjectSelected( ignore , unUsedParamSave ) )
        {
        // Options/Back/Open
        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_BACK__OPEN;
        }
    else if ( item && item->Length() > 0 )
        {
        // automatic find item selected
        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_BACK__CONTEXTOPTIONS;
        }
    CleanupStack::PopAndDestroy( item );

    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->SetCommandSetL( cbaRes );
    cba->DrawNow();
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::OpenContextMenuL
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::ShowContextMenuL(
    TContextLaunchEvent aLaunchEvent )
    {
    if ( IMUtils::CRKeyL( KCRUidIMVariation,
                          KIMVariationKey ) & EIMFeatRecordedChatOptions )
        {
        // New options
        TBool ignore;
        TBool unUsedParamSave( EFalse );
        if ( IsObjectSelected( ignore , unUsedParamSave ) )
            {
            // Single click don't open objects
            if ( aLaunchEvent != ESingleClick )
                {
                iAppUI->ProcessCommandL( EChatClientOpenObject );
                }
            }
        else
            {
            // Double click don't open find menu
            if ( SelectedItemType() != KErrNotFound &&
                 aLaunchEvent != EDoubleClick )
                {
                iRecipientObserver.ShowPopUpMenuL();
                }
            }
        }
    else if ( aLaunchEvent == EDoubleClick ||
              aLaunchEvent == EKeyActivation )
        {
        // Old options
        iRecipientObserver.ShowPopUpMenuL();
        }
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::HandleControlEventL
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::HandleControlEventL( CCoeControl* aControl,
                                                     TCoeEvent aEventType )
    {
    if ( CATouchUtils::PenEnabled() )
        {

        if ( EEventRequestFocus == aEventType )
            {
            if ( aControl == ( CCoeControl* )iChatView )
                {
                // Focus to editor
                UpdateCbaL();
                }
            }
        }
    }

// ---------------------------------------------------------
// CCARecordedChatsContainer::HandleMessageAddedL
// ---------------------------------------------------------
//
void CCARecordedChatsContainer::HandleMessageAddedL( const TInt /*aMessageCount*/ )
    {
    UpdateCbaL();

    iChatView->UnRegisterRichTxtCtrlObserver();
    }

// End of File

