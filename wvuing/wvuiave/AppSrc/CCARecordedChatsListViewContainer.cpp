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
* Description:  Container for recorded chats list view.
*
*/


// INCLUDE FILES
#include    "MCARecordedChatsPC.h"
#include    "MCARecordedChatsArrayPC.h"
#include    "MCAProcessManager.h"

#include    "CCARecordedChatsListViewContainer.h"
#include    "CCARecordedChatsArray.h"
#include    "CCAAppUi.h"
#include    "CCAApp.h"
#include    "ChatDefinitions.h"
#include    "CCAStatusPaneHandler.h"
#include    "CAExternalInterface.h"
#include    "MCASkinVariant.h"
#include    "CCAVariantFactory.h"
#include    "ChatDebugPrint.h"

#include    <chatNG.mbg>
#include    <chatNG.rsg>

#include    <csxhelp/imng.hlp.hrh>
#include    <eikmenub.h>
#include    <StringLoader.h>
#include    <eikprogi.h>
#include    <aknlists.h>
#include    <aknnotewrappers.h>
#include    <akniconarray.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

#include "MCARecipientObserver.h"

// CONST VARIABLES
const TInt KRecordedChatDeletionIsReady = 0;
const TInt KRecordedChatDeletionIsNotReady = 1;
const TInt KProgressIncrement = 1;
const TInt KProgressInterval = 1;
const TInt KGranularity = 2;

// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCARecordedChatsListViewContainer* CCARecordedChatsListViewContainer::NewL(
    MCARecipientObserver& aMenuObserver,
    const TRect& aRect,
    CCAAppUi& aAppUi,
    MCAViewSwitcher& aViewSwitcher,
    CCARecordedChatsArray& aGroupArray,
    TInt aLastIndex,
    TBool& aContextFlag  )
    {
    CCARecordedChatsListViewContainer* self =
        new ( ELeave ) CCARecordedChatsListViewContainer( aMenuObserver, aAppUi, aViewSwitcher,
                                                          aGroupArray, aContextFlag );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aLastIndex );

    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::CCARecordedChatsListViewContainer
// Default constructor
// ---------------------------------------------------------
//
CCARecordedChatsListViewContainer::CCARecordedChatsListViewContainer(
    MCARecipientObserver& aMenuObserver,
    CCAAppUi& aAppUi,
    MCAViewSwitcher& aViewSwitcher,
    CCARecordedChatsArray& aGroupArray,
    TBool& aContextFlag  )
        :	iMenuObserver( aMenuObserver ),
        iRecordedChatsArray( aGroupArray ),
        iAppUi( aAppUi ),
        iViewSwitcher( aViewSwitcher ),
        iProgressCount( aGroupArray.ProgressCount() ),
        iContextFlag( aContextFlag ),
        iRecordedChatsPC( *( aAppUi.GetProcessManager().GetRecordedChatsPC() ) )
    {
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::ConstructL
// Symbian OS two phased constructor
// ---------------------------------------------------------
//
void CCARecordedChatsListViewContainer::ConstructL(
    const TRect& aRect,
    TInt aLastIndex )
    {
    // This is done for MTM send to work. It is not done when array is created
    // because it takes about 1 second to prepare sending and array is
    // created when application starts.
    iRecordedChatsArray.PrepareArrayForSendingMtmL();

    CreateWindowL();

    iAppUi.AddLayoutChangeObserver( this );

    // Create and construct listbox
    iListBox = new ( ELeave ) CAknDoubleGraphicStyleListBox;
    iListBox->ConstructL( this, EAknListBoxMarkableList );
    iListBox->SetListBoxObserver( this );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    iListBox->Model()->SetItemTextArray( &iRecordedChatsArray );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    iRecordedChatsPC.SetSelectedItems( iListBox->SelectionIndexes() );

    LoadBitmapsL();

    if ( iRecordedChatsArray.CurrentChatDeleteStatus() )
        {
        DeleteHistoryFilesL();
        }

    SetRect( aRect );

    if ( aLastIndex >= 0 && aLastIndex < iListBox->Model()->NumberOfItems() )
        {
        iListBox->SetCurrentItemIndexAndDraw( aLastIndex );
        }

    UpdateCbaL();

    ActivateL();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsListViewContainer::~CCARecordedChatsListViewContainer
// destructor
// -----------------------------------------------------------------------------
//
CCARecordedChatsListViewContainer::~CCARecordedChatsListViewContainer()
    {
    if ( !iAppUi.IsUnderDestruction() )
        {
        iAppUi.RemoveLayoutChangeObserver( this );
        }

    // Remove pointer from group array.
    iRecordedChatsPC.SetSelectedItems( NULL );

    delete iListBox;
    delete iIdle;

    TInt ignore;
    TRAP( ignore, DismissProgressDialogL() );
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::ListBox()
// Returns handle to listbox
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAknDoubleGraphicStyleListBox* CCARecordedChatsListViewContainer::ListBox()
    {
    return iListBox;
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::SetDefaultTitleL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsListViewContainer::SetDefaultTitleL()
    {
    iViewSwitcher.CAStatusPane()->SetTitleL( iEikonEnv,
                                             R_CHATCLIENT_RECORDEDCHATLIST_VIEW_TITLE );
    iViewSwitcher.CAStatusPane()->ShowTabGroupL( KUidRecordedChatListView );
    }


// -----------------------------------------------------------------------------
// CCARecordedChatsListViewContainer::LoadBitmapsL
// Load bitmaps.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsListViewContainer::LoadBitmapsL()
    {
    // Create array for listbox graphics
    CArrayPtr<CGulIcon>* icons =
        new ( ELeave ) CAknIconArray( KArrayGranularity );

    CleanupStack::PushL( icons );

    // Add icons. Marked first, so we can use automatic marking feature
    MCASkinVariant* skinVar =
        static_cast<CCAApp*>( iAppUi.Application() )->
        VariantFactory()->SkinVariantL();

    CGulIcon* markedItem =
        skinVar->LoadBitmapL(   EMbmChatngQgn_indi_marked_add,
                                EMbmChatngQgn_indi_marked_add_mask,
                                iAppUi.MbmFullPath() );

    CleanupStack::PushL( markedItem );

    CGulIcon* savedConv =
        skinVar->LoadBitmapL(   EMbmChatngQgn_prop_im_saved_conv,
                                EMbmChatngQgn_prop_im_saved_conv_mask,
                                iAppUi.MbmFullPath() );

    CleanupStack::PushL( savedConv );
    CGulIcon* savedGroup =
        skinVar->LoadBitmapL(   EMbmChatngQgn_prop_im_saved_chat,
                                EMbmChatngQgn_prop_im_saved_chat_mask,
                                iAppUi.MbmFullPath() );
    CleanupStack::PushL( savedGroup );


    icons->AppendL( markedItem );
    icons->AppendL( savedConv );
    icons->AppendL( savedGroup );

    CArrayPtr<CGulIcon>* oldIcons =
        iListBox->ItemDrawer()->ColumnData()->IconArray();
    if ( oldIcons )
        {
        oldIcons->ResetAndDestroy();
        delete oldIcons;
        }

    // Transfer ownership to listbox
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );

    CleanupStack::Pop( 4, icons ); // savedConv, savedGroup, markedItem
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsListViewContainer::LayoutChangedL
// Úpdate when layout is changed
// -----------------------------------------------------------------------------
//
void CCARecordedChatsListViewContainer::LayoutChangedL( TInt aType )
    {
    // update this view's bitmaps
    LoadBitmapsL();
    // updates tabgroup
    iViewSwitcher.CAStatusPane()->ShowTabGroupL( KUidRecordedChatListView );

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    SetRect( mainPaneRect );
    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::HandleListBoxEventL
// Handles list box events
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsListViewContainer::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    if ( aEventType == EEventEnterKeyPressed ||
         aEventType == EEventItemDoubleClicked )
        {
        TInt markedCount( iListBox->SelectionIndexes()->Count() );
        if ( markedCount == 0 )
            {
            iRecordedChatsArray.SetCurrentItemIndex( iListBox->CurrentItemIndex() );
            iViewSwitcher.SwitchViewL( KUidRecordedChatView );
            }
        // If there is item been marked, context menu is enabled.
        if ( markedCount > 0 )
            {
            iContextFlag = ETrue;
            UpdateCbaL();
            iMenuObserver.ShowPopUpMenuL();
            }
        iContextFlag = EFalse;
        }
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCARecordedChatsListViewContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::CountComponentControls
// ---------------------------------------------------------
//
TInt CCARecordedChatsListViewContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::ComponentControl
// Return component by index.
// ---------------------------------------------------------
//
CCoeControl* CCARecordedChatsListViewContainer::ComponentControl(
    TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return iListBox;
            }
        default:
            {
            return NULL;
            }
        }
    }


// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::OfferKeyEventL
// Handles key events
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCARecordedChatsListViewContainer::OfferKeyEventL(
    const TKeyEvent& aEvent, TEventCode aType )
    {
    CHAT_DP( D_CHAT_LIT(
                 "CCARecordedChatsListViewContainer::OfferKeyEventL %d %d" ),
             aEvent,
             aType );

    CCoeEnv* iEnv = CCoeEnv::Static();
    if (  iEnv->AppUi()->IsDisplayingMenuOrDialog() || aEvent.iScanCode == EStdKeyDial )
        {
        return EKeyWasConsumed;
        }
    //No action with arrow keys to left and right
    if ( aEvent.iCode == EKeyLeftArrow || aEvent.iCode == EKeyRightArrow )
        {
        return EKeyWasNotConsumed;
        }
    if ( aEvent.iCode == EKeyBackspace )
        {
        DeleteHistoryFilesL();
        return EKeyWasConsumed;
        }

    if ( aEvent.iScanCode == EStdKeyDevice3 ) // ok key
        {
        iContextFlag = ETrue;
        }
    else
        {
        iContextFlag = EFalse;
        }

    TKeyResponse res = iListBox->OfferKeyEventL( aEvent, aType );

    // If hash key is pressed down, do not change MSK title (AVKON sets MARK text)
    // IM also receives some interesting key events. Ignore these.
    if ( !( aEvent.iScanCode == EStdKeyHash && ( aType == EEventKeyDown || aType == EEventKey ) )
         && aEvent.iScanCode != EStdKeyF21
         && aEvent.iScanCode != EStdKeyLeftShift 
         && IsFocused() )
        {
        UpdateCbaL();
        }
    return res;
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::GetHelpContext
// This function is called when Help application is launched
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsListViewContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_SAVED_LIST ;
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::DeleteHistoryFilesL()
// Handles item removal
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsListViewContainer::DeleteHistoryFilesL()
    {
    if ( !( iRecordedChatsArray.MdcaCount() > iListBox->CurrentItemIndex() &&
            iRecordedChatsArray.MdcaCount() > 0 ) ) //index out of range.
        {
        return;
        }

    if ( iRecordedChatsArray.DeleteHistoryFilesL( iListBox->CurrentItemIndex() ) )
        {
        SetMovingFlag( ETrue );
        // Show progressnote and delete items.
        ShowProgressDialogL();

        delete iIdle;
        iIdle = NULL;
        iIdle = CIdle::NewL( CActive::EPriorityLow );
        TCallBack callback( IdleDeleteCallback, this );
        iIdle->Start( callback );
        }
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsListViewContainer::OpenRecordL
// Open record if there is one.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsListViewContainer::OpenRecordL()
    {
    if ( iMovingFlag )
    	{
    	return;
    	}
    if ( iRecordedChatsArray.MdcaCount() > iListBox->CurrentItemIndex() &&
         iRecordedChatsArray.MdcaCount() > 0 )
        {
        iRecordedChatsArray.SetCurrentItemIndex( iListBox->CurrentItemIndex() );
        iViewSwitcher.SwitchViewL( KUidRecordedChatView );
        }
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::IdleDeleteCallback
// Callback method for idle.
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsListViewContainer::IdleDeleteCallback( TAny* aThis )
    {
    CCARecordedChatsListViewContainer* self =
        static_cast< CCARecordedChatsListViewContainer* >( aThis );
	self->SetMovingFlag( EFalse );                

    TBool ret( EFalse );
    TRAPD( err, ret = self->iRecordedChatsArray.DeleteHistoryFileL() );

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }

    TRAP( err, self->UpdateProgressTextL(); );

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }

    if ( !ret )
        {

        TRAP( err, self->DismissProgressDialogL(); );
        if ( err )
            {
            CActiveScheduler::Current()->Error( err );
            }

        self->iListBox->SetCurrentItemIndex(
            self->iRecordedChatsArray.CurrentItemIndex() );
        self->iListBox->ClearSelection();

        self->DrawNow();

        return KRecordedChatDeletionIsReady;
        }

    return KRecordedChatDeletionIsNotReady;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsListViewContainer::UpdateCbaL
// Update CBA
// -----------------------------------------------------------------------------
//
void CCARecordedChatsListViewContainer::UpdateCbaL()
    {
    TInt cbaRes = R_CHAT_SOFTKEYS_OPTIONS_BACK__EMPTY;  // Default: Options/Back

    if ( iListBox->Model()->NumberOfItems() > 0
         && iListBox->SelectionIndexes()->Count() == 0 )
        {
        // Options/Back/Open
        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_BACK__OPEN;
        }
    else if ( iListBox->Model()->NumberOfItems() > 0
              && iListBox->SelectionIndexes()->Count() > 0 )
        {
        // Options/Back/Context menu icon
        cbaRes = R_CHAT_SOFTKEYS_OPTIONS_BACK__CONTEXTOPTIONS;
        }

    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->SetCommandSetL( cbaRes );
    cba->DrawNow();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::ShowProgressDialogL
// Progress dialog show
// -----------------------------------------------------------------------------
//
void CCARecordedChatsListViewContainer::ShowProgressDialogL()
    {
    if ( iProgressDialog )
        {
        //Hide old one if exists
        DismissProgressDialogL();
        }

    // show the dialog
	if ( iListBox->SelectionIndexes()->Count() > KProgressIncrement )
		{
    iProgressDialog = new ( ELeave ) CAknProgressDialog(
        iListBox->SelectionIndexes()->Count(),
        KProgressIncrement, KProgressInterval,
        reinterpret_cast< CEikDialog** > ( &iProgressDialog ) );
		}
	else 
		{
		iProgressDialog = new (ELeave) CAknProgressDialog(
				reinterpret_cast< CEikDialog** > ( &iProgressDialog ));
		}
	
    iProgressDialog->SetCallback( this );
    iProgressDialog->SetTone( CAknNoteDialog::ENoTone );
    // CodeScanner warning ignored because iProgressDialog
    // is not destructed in destructor, ExecuteLD destroys it automatically
    iProgressDialog->ExecuteLD( R_CHAT_PROGRESS_NOTE_TEMPLATE ); // CSI: 50 # See comment above
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    UpdateProgressTextL();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::DismissProgressDialogL
// Shutdown progressdialog if running.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsListViewContainer::DismissProgressDialogL()
    {
    if ( iProgressDialog )
        {
        TRAPD( err, iProgressDialog->ProcessFinishedL();
               UpdateCbaL(); );
        if ( err != KErrNone )
            {
            // don't know for sure that ProcessFinishedL deletes the dialog
            // in all cases, so let's delete it
            delete iProgressDialog;
            iProgressDialog = NULL;
            User::Leave( err );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::UpdateProgressTextL
// This takes some time. Deletion is quick rutine without this progress
// dialog update.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsListViewContainer::UpdateProgressTextL()
    {
    HBufC* progressText;

    TInt markedCount( iListBox->SelectionIndexes()->Count() );
    if ( markedCount == 0 )
        {
        progressText = StringLoader::LoadLC( R_QTN_CHAT_REC_DEL_CHAT );
        }
    else
        {
        CArrayFix<TInt>* intArray = NULL;
        intArray = new ( ELeave ) CArrayFixFlat<TInt>( KGranularity );
        CleanupStack::PushL( intArray );
        intArray->AppendL( iProgressCount );
        intArray->AppendL( markedCount );
        progressText = StringLoader::LoadL( R_QTN_CHAT_REC_DEL_CHAT_MANY,
                                            *intArray );
        CleanupStack::PopAndDestroy( intArray );
        CleanupStack::PushL( progressText );
        }

    if ( iProgressDialog )
        {
        iProgressDialog->SetTextL( *progressText );
        iProgressInfo->IncrementAndDraw( 1 );
        }

    CleanupStack::PopAndDestroy( progressText );
    }

// ---------------------------------------------------------
// CCARecordedChatsListViewContainer::FocusChanged
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCARecordedChatsListViewContainer::FocusChanged( TDrawNow /* aDrawNow */ )
    {
    if ( iListBox )
        {
        // give focus to list box so that highlight animations
        // are done properly
        iListBox->SetFocus( IsFocused() );
        }
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::DialogDismissedL
// Callback method for progress dialog to inform us when dialog is dismissed.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsListViewContainer::DialogDismissedL( TInt /*aButtonId*/ )
    {
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsListViewContainer::MarkAllL
// mark/unmark all items
// -----------------------------------------------------------------------------
//
void CCARecordedChatsListViewContainer::MarkAllL( TBool aMark )
    {
    iListBox->ClearSelection();

    if ( aMark )
        {
        TInt itemCount( iRecordedChatsArray.MdcaCount() );

        CArrayFixFlat<TInt>* array =
            new( ELeave ) CArrayFixFlat< TInt >( itemCount );
        CleanupStack::PushL( array );
        array->SetReserveL( itemCount );
        for ( TInt index( 0 ); index < itemCount; ++index )
            {
            array->AppendL( index );
            }
        iListBox->SetSelectionIndexesL( array );
        CleanupStack::PopAndDestroy( array );
        }

    UpdateCbaL();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsListViewContainer::MarkL
// Mark/unmark indexed item
// -----------------------------------------------------------------------------
//
void CCARecordedChatsListViewContainer::MarkL( TInt aIndex, TBool aMark )
    {
    if ( aMark )
        {
        iListBox->View()->SelectItemL( aIndex );
        }
    else
        {
        iListBox->View()->DeselectItem( aIndex );
        }

    UpdateCbaL();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsListViewContainer::HighlightedChatName()
// -----------------------------------------------------------------------------
//
const TDesC& CCARecordedChatsListViewContainer::HighlightedChatName() const
    {
    TInt index = iListBox->CurrentItemIndex();
    if ( index < 0 )
        {
        return KNullDesC;
        }
    MCARecordedChatsArrayPC& recChatsArrayPC = *( iAppUi.GetProcessManager().GetRecordedChatsArrayPC() );
    return recChatsArrayPC.Name( index );
    }
    
void CCARecordedChatsListViewContainer::SetMovingFlag( TBool aFlag )
	{
	iMovingFlag = aFlag;
	}
// End of File
