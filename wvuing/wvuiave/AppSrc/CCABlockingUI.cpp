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
* Description:  UI side blocker class
 *
*/


// INCLUDE FILES
#include    "CCABlockingUI.h"
#include    "IMNoteMapper.h"
#include	"IMDialogUtils.h"
#include    "ChatDefinitions.h"
#include    "chatdebugprint.h"
#include    "avkon.hrh"
#include    "ImpsCSPAllErrors.h"
#include    "CCAAppUi.h"
#include    "MCAServerContactsArrayPC.h"
#include    "CAArrayUtils.h"
#include    "IMUtils.h"
#include    "CAUtils.h"
#include    "CCAPCUtils.h"

#include	"CCABlockingDialog.h"
#include	"chatngclient.hrh"
#include	"CCAAppUi.h"
#include    "CCAApp.h"
#include	"CCAStatusPaneHandler.h"

#include    "CCAContactEditor.h"
#include	"CCAVariantFactory.h"
#include	"CCAContactSelectionDialog.h"

#include    "ccasyncchecker.h"
#include    "impsbuilddefinitions.h"

#include    <chatNG.rsg>
#include    <eikenv.h>
#include	<hlplch.h>
#include    <StringLoader.h>
#include    <aknnotewrappers.h>
#include    "CCAServerContactsArrayPC.h"
#include    "MCASettingsPC.h"
#include 	"MCAProcessManager.h"
#include  	"MCABlockingPC.h"

#include <csxhelp/imng.hlp.hrh>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

// ============================ MEMBER FUNCTIONS ===============================
_LIT( KMyTab, "\t" );

// C++ default constructor can NOT contain any code, that
// might leave.
CCABlockingUI::CCABlockingUI( MCABlockingPC* aBlockingPC )
        : iBlockingPC( aBlockingPC ),
        iDialogShutter( EFalse )
    {

    }



// -----------------------------------------------------------------------------
// CCABlockingUI::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCABlockingUI::ConstructL()
    {
    iAppUi = static_cast< CCAAppUi* >( CCoeEnv::Static()->AppUi() );

    iMainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();

    iBlockingPC->RegisterBackgroundObserver( this );

    iTextualInputMode = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_USERID_TEXTUAL_INPUTMODE );
    }

// -----------------------------------------------------------------------------
// CCABlockingUI::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCABlockingUI* CCABlockingUI::NewL( MCABlockingPC* aBlockingPC )
    {
    CCABlockingUI* self = new( ELeave ) CCABlockingUI( aBlockingPC );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCABlockingUI::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCABlockingUI* CCABlockingUI::NewLC( MCABlockingPC* aBlockingPC )
    {
    CCABlockingUI* self = new( ELeave ) CCABlockingUI( aBlockingPC );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCABlockingUI::~CCABlockingUI
// -----------------------------------------------------------------------------
//
CCABlockingUI::~CCABlockingUI()
    {

    CHAT_DP( D_CHAT_LIT( "CCABlockingUI::~CCABlockingUI \
                                     before delete iBlockingDialog:%d" ),
             iBlockingDialog );

    if ( iBlockingPC )
        {
        iBlockingPC->UnRegisterBackgroundObserver( );
        }
    delete iOldTitle;
    }

// ---------------------------------------------------------
// CCABlockingUI::DisplayBlockedListL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCABlockingUI::DisplayBlockedListL( TBool aDontShowTabGroup /*= EFalse */ )
    {
    CHAT_DP_FUNC_ENTER( "DisplayBlockedListL" );

    switch ( iBlockingPC->GetBackgroundStatus() )
        {
        case TEnumsPC::ECompleted:
            {
            //we have a list
            break;
            }


        case TEnumsPC::EFailedStatus:
        case TEnumsPC::EUnknownStatus:
        case TEnumsPC::ECancelled:
            {
            //we dont have a list, but we can try again
            //show the wait dialog immediately
            iAppUi->ShowWaitDialogL( R_CHAT_BLOCK_FETCHING, ETrue );
            // Getting blocked members
            // We have to trap the get-method, because we need to propagate the
            // CSP errors forward to the wait dialog dismissal logic.

            TRAPD( err, iBlockingPC->GetBlockedListFromServerL() );
            iAppUi->DismissWaitDialogL( err );
            break;
            }

        case KErrNotFound:
        case TEnumsPC::EIdleStarted:
        case TEnumsPC::EStarting:
        case TEnumsPC::EWaiting:
            {
            //we have are getting the list, its time to wait
            //show the wait dialog immediately
            iAppUi->ShowWaitDialogL( R_CHAT_BLOCK_FETCHING, ETrue );

#ifdef _DEBUG
            if ( iFetchOperation != CCABlockingUI::EUnknown )
                {
                CHAT_DP( D_CHAT_LIT( "CCABlockingUI::DisplayBlockedListL \
                                 previous operation is waiting:%d" ),
                         iFetchOperation );
                }
#endif // _DEBUG

            iFetchOperation = EShowBlockList;
            iWaiting = ETrue;
            return;

            }
        default:
            {
            //we should not be here
            break;
            }
        }

    // dismiss the wait dialog
    iAppUi->DismissWaitDialogL( KErrNone );
    iWaiting = EFalse;

    iBlockingPC->GetBlockedMembersL( KMyTab );

    iSelectedItem = KErrNotFound;

    if ( iBlockingDialog )
        {
        delete iBlockingDialog;
        iBlockingDialog = NULL;
        }

    //preserve title pane text
    BackupTitleL();
    CleanupStack::PushL( TCleanupItem( CCABlockingUI::RestoreTitle, this ) );



    iBlockingArray = &( iBlockingPC->GetServerContactsArray() );
    iBlockingDialog = CCABlockingDialog::NewL( iSelectedItem, iBlockingArray,
                                               R_CHATCLIENT_MENUBAR_BLOCKING_DIALOG,
                                               R_CHATCLIENT_MENUBAR_BLOCKING_DIALOG2,
                                               this,
                                               aDontShowTabGroup
                                             );

    // CodeScanner warning ignored because iBlockingDailog is not owned by us
    // and it is destroyed automatically in ExecuteLD
    if ( iBlockingDialog->ExecuteLD( R_BLOCK_DIALOG ) == EChatClientCmdExit ) // CSI: 50 # See comment above
        {
        // Handle exit command
        iBlockingDialog = NULL;
        iAppUi->HandleCommandL( EChatClientCmdExit );
        }

    iBlockingDialog = NULL;
    CleanupStack::PopAndDestroy();

    CHAT_DP_FUNC_DONE( "DisplayBlockedListL" );
    }

// ---------------------------------------------------------
// CCABlockingUI::DisplayUnblockListQueryL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCABlockingUI::DisplayUnblockListQueryL()
    {
    CHAT_DP_FUNC_ENTER( "DisplayUnblockListQueryL" );

    switch ( iBlockingPC->GetBackgroundStatus( ) )
        {
        case TEnumsPC::ECompleted:
            {
            //we have a list
            break;
            }

        case TEnumsPC::EFailedStatus:
        case TEnumsPC::EUnknownStatus:
        case TEnumsPC::ECancelled:
            {
            //we dont have a list, but we can try again
            //display the wait dialog immediately
            iAppUi->ShowWaitDialogL( R_CHAT_BLOCK_FETCHING, ETrue );
            //Getting blocked members
            // We have to trap the get-method, because we need to propagate the
            // CSP errors forward to the wait dialog dismissal logic.

            TRAPD( err, iBlockingPC->GetBlockedListFromServerL() );
            iAppUi->DismissWaitDialogL( err );
            break;
            }

        case KErrNotFound:
        case TEnumsPC::EIdleStarted:
        case TEnumsPC::EStarting:
        case TEnumsPC::EWaiting:
            {
            //we are getting the list, its time to wait
            //display the wait dialog immediately
            iAppUi->ShowWaitDialogL( R_CHAT_BLOCK_FETCHING, ETrue );

#ifdef _DEBUG
            if ( iFetchOperation != CCABlockingUI::EUnknown )
                {
                CHAT_DP( D_CHAT_LIT( "CCABlockingUI::DisplayUnblockListQueryL \
                                 previous operation is waiting:%d" ),
                         iFetchOperation );
                }
#endif // _DEBUG

            iFetchOperation = EShowUnblockList;
            iWaiting = ETrue;
            return;


            }
        default:
            {
            //we should not be here
            break;
            }
        }

    // dismiss the wait dialog
    iAppUi->DismissWaitDialogL( KErrNone );
    iWaiting = EFalse;

    MCAServerContactsArrayPC* blockedMembers = iBlockingPC->GetBlockedMembersLC( KIconTabulator );

    //Creating index array that tells what indexes are selected or not
    CArrayFixFlat<TInt>* indexArray =
        new ( ELeave ) CArrayFixFlat<TInt>( KArrayGranularity );
    CleanupStack::PushL( indexArray );

    //Creating list of unblocked users
    CDesCArrayFlat* unBlockList =
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( unBlockList );

    //Display pop up list
    TInt result( IMDialogUtils::DisplayListQueryDialogL( indexArray,
                                                         blockedMembers,
                                                         R_CHAT_UNBLOCK_LIST_QUERY ) );

    if ( result == EAknSoftkeyOk )
        {
        TInt blockedMembersCount( blockedMembers->MdcaCount() );
        TInt count( indexArray->Count() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            TInt deletedIndex( indexArray->At( i ) );
            if ( deletedIndex >= blockedMembersCount )
                {
                deletedIndex = blockedMembersCount - 1;
                }
            // unBlockList is those persons who are going to be unblocked
            unBlockList->AppendL( iBlockingPC->GetWvIdL( KIconTabulator,
                                                         deletedIndex ) );
            }

        // just a check
        if ( unBlockList->MdcaCount() > 0 )
            {
            iAppUi->ShowWaitDialogL( R_CHAT_BLOCK_REMOVING );
            TInt errorCode( KErrNone );
            TRAPD( err, errorCode = iBlockingPC->RemoveL( unBlockList, NULL ) );

            iAppUi->DismissWaitDialogL( err );
            if ( errorCode != KErrNone )
                {
                IMDialogUtils::DisplayErrorNoteL( errorCode );
                }
            }
        }

    CleanupStack::PopAndDestroy( 3 ); // unBlockList, indexArray, blockedMembers
    CHAT_DP_FUNC_DONE( "DisplayUnblockListQueryL" );
    }

// ---------------------------------------------------------
// CCABlockingUI::DisplayBlockTextQueryL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCABlockingUI::DisplayBlockTextQueryL()
    {
    //Data pointer
    HBufC* data = HBufC::NewLC( KMaxWVIDLength );
    TPtr dataPtr( data->Des() );

    //Displaying text query dialog
    TInt result( IMDialogUtils::DisplayTextQueryDialogL( dataPtr,
                                                         R_CHATCLIENT_BLOCK_ENTER_PROMPT,
                                                         iTextualInputMode ? R_CHATCLIENT_BLOCK_ENTER : R_CHATCLIENT_BLOCK_ENTER2,
                                                         ETrue // T9
                                                       ) );

    if ( result == EAknSoftkeyOk || result == EAknSoftkeyDone )
        {
        // don't add a user which is already in our list

        const CDesCArray* blockList = iBlockingPC->GetBlockedList();
        TInt count = blockList->Count();
        for ( TInt i = 0; i < count; i++ )
            {
            if ( 0 == CAUtils::NeutralCompare( *data,
                                               blockList->MdcaPoint( i ) ) )
                {
                CleanupStack::PopAndDestroy(); // data
                return;
                }
            }

        BlockUserL( *data );

        }
    CleanupStack::PopAndDestroy( data );
    }

// ---------------------------------------------------------
// CCABlockingUI::BlockUserL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCABlockingUI::BlockUserL( const TDesC& aUserId )
    {
    // this cannot be called when the user is already on the list, because the
    // menu option will not be visible in such a case
    CDesCArray* blockArray = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( blockArray );
    blockArray->AppendL( aUserId );

    BlockMultipleUsersL( blockArray );

    CleanupStack::PopAndDestroy( blockArray );

    }
// ---------------------------------------------------------
// CCABlockingUI::BlockUserL
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCABlockingUI::BlockMultipleUsersL( const CDesCArray* aUserIds )
    {
    // If we go to block someone, we can not close the dialog
    if ( iBlockingDialog )
        iBlockingDialog->SetExitEnabled( EFalse );

    if ( !aUserIds )
        {
        // Set the value back when operation failed
        if ( iBlockingDialog )
            iBlockingDialog->SetExitEnabled( ETrue );

        User::Leave( KErrArgument );
        }

    TInt errorCode( KErrNone );

    // If contact list synch is not done. Wait for it.
    TInt removeBlocked = ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING ) == 0 );

    if ( removeBlocked && !CCASyncChecker::CheckSyncStateL() )
        {
        if ( iBlockingDialog )
            iBlockingDialog->SetExitEnabled( ETrue );

        return;
        }

    iAppUi->ShowWaitDialogL( R_CHAT_BLOCK_ADDING );

    if ( removeBlocked )
        {
        //we have to remove ids from the list first.
        TRAPD( err, RemoveIDsL( aUserIds ) );

        if ( err != KErrNone )
            {
            iAppUi->DismissWaitDialogL( err );

            if ( iBlockingDialog )
                iBlockingDialog->SetExitEnabled( ETrue );

            User::Leave( err );
            }
        }

    TRAPD( err, errorCode = iBlockingPC->InsertL( aUserIds, NULL ) );
    iAppUi->DismissWaitDialogL( err );

    // Set the value back when operation finished
    if ( iBlockingDialog )
        iBlockingDialog->SetExitEnabled( ETrue );

    CHAT_DP( D_CHAT_LIT( "CCABlockinManager::BlockUserL() err==%d" ), errorCode );

    if ( errorCode == KErrNone )
        {
        IMDialogUtils::DisplayNoteDialogL( R_CHATCLIENT_CNOTE_BLOCK );
        }
    else
        {
        if ( errorCode == ECSPInvalidParameter )
            {
            errorCode = ECSPUnknownUserId;
            }
        if ( aUserIds->MdcaCount() == 1 )
            {
            IMNoteMapper::ShowNoteL( errorCode, CCAPCUtils::DisplayId( aUserIds->MdcaPoint( 0 ) ) );
            }
        else
            {
            IMNoteMapper::ShowNoteL( errorCode );
            }
        }

    iBlockingPC->GetBlockedMembersL( KMyTab );

    if ( iBlockingDialog )
        {
        iBlockingDialog->HandleItemAdditionL();
        SetIndexAfterBlocking( aUserIds->Count() > 1,
                               aUserIds->MdcaPoint( 0 ) );
        }
    }

// -----------------------------------------------------------------------------
// CCAAppUi::CloseDialog
// -----------------------------------------------------------------------------
//
void CCABlockingUI::RestoreTitle( TAny* aInstance )
    {

    TRAPD( leave, static_cast<CCABlockingUI*>( aInstance )->DoRestoreTitleL() );
    if ( leave != KErrNone )
        {
        CActiveScheduler::Current()->Error( leave );
        }

    }

// -----------------------------------------------------------------------------
// CCAAppUi::CloseDialog
// -----------------------------------------------------------------------------
//
void CCABlockingUI::DoRestoreTitleL()
    {
    if ( iOldTitle )
        {
        CCAStatusPaneHandler* statusPane = iAppUi->CAStatusPane();

        statusPane->SetTitleL( *iOldTitle );
        delete iOldTitle;
        iOldTitle = NULL;
        }
    }
// ---------------------------------------------------------
// CCABlockingUI::HandleBackgroundEventPC
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCABlockingUI::HandleBackgroundEventPC(
    TEnumsPC::TCABackgroundTasks aEventSource,
    TEnumsPC::TCABackgroundStatus aStatus,
    TInt aSubTaskNumber,
    TInt aLeaveCode
)
    {
    CHAT_DP_FUNC_ENTER( "HandleBackgroundEvent" );
    CHAT_DP( D_CHAT_LIT( "CCABlockingUI::HandleBackgroundEvent() source:%d \
                        status:%d subtask:%d leave:%d" ),
             aEventSource, aStatus, aSubTaskNumber, aLeaveCode );

    switch ( iFetchOperation )
        {
        case CCABlockingUI::EShowBlockList:
            {
            if ( iWaiting )
                {
                switch ( aStatus )
                    {
                    case TEnumsPC::ECompleted:
                        {
                        TRAPD( err, DisplayBlockedListL() );

                        if ( err != KErrNone )
                            {
                            // just to make sure.
                            TInt ignore;
                            TRAP( ignore,
                                  iAppUi->DismissWaitDialogL( KErrNone ) );

                            // let the activescheduler handle it
                            CActiveScheduler::Current()->Error( err );
                            }
                        break;
                        }

                    default:
                        {
                        TRAPD( err, iAppUi->DismissWaitDialogL( KErrNone ) );
                        iWaiting = EFalse;
                        if ( err != KErrNone )
                            {
                            // let the activescheduler handle it
                            CActiveScheduler::Current()->Error( err );
                            }
                        break;
                        }
                    }
                }
            break;
            }
        case CCABlockingUI::EShowUnblockList:
            {
            if ( iWaiting )
                {
                switch ( aStatus )
                    {
                    case TEnumsPC::ECompleted:
                        {
                        TRAPD( err, DisplayUnblockListQueryL() );

                        if ( err != KErrNone )
                            {
                            // just to make sure..
                            TInt ignore;
                            TRAP( ignore,
                                  iAppUi->DismissWaitDialogL( KErrNone ) );

                            // let the activescheduler handle it
                            CActiveScheduler::Current()->Error( err );
                            }
                        break;
                        }

                    default:
                        {
                        TRAPD( err, iAppUi->DismissWaitDialogL( KErrNone ) );
                        iWaiting = EFalse;

                        if ( err != KErrNone )
                            {
                            // let the activescheduler handle it
                            CActiveScheduler::Current()->Error( err );
                            }
                        break;
                        }
                    }
                }
            break;
            }

        case CCABlockingUI::EUnknown:
            {
            break;
            }

        default:
            {
            //we shouldnt be here
            }
        }

    iFetchOperation = CCABlockingUI::EUnknown;

    CHAT_DP_FUNC_DONE( "HandleBackgroundEvent" );
    }

// ---------------------------------------------------------
// CCABlockingUI::ProcessCommandL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCABlockingUI::ProcessCommandL( TInt aCommandId )
    {
    switch ( aCommandId )
        {
        case EChatClientBlockingUnblock:
            {
            UnBlockL();

            if ( !iDialogShutter )
                {
                iBlockingDialog->RefreshDialogTitleL();
                }
            else
                {
                iDialogShutter = EFalse;
                }

            break;
            }

        case EChatClientBlockingBlock:
            {
            DisplayBlockListQueryL();
            break;
            }

        case EChatClientBlockingBlockEnter:
            {
            DisplayBlockTextQuery2L();
            break;
            }

        case EChatClientCmdExit :
            {
            iDialogShutter = ETrue;
            break;
            }

        // Help key support EChatClientCmdHelp is assigned EAknCmdHelp in
        // chatngclient.hrh
        case EChatClientCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(
                CEikonEnv::Static()->WsSession(),
                iAppUi->AppHelpContextL() );
            break;
            }

        default:
            {
            iBlockingDialog->SelectionListProcessCommandL( aCommandId );
            break;
            }
        }

    }

// ---------------------------------------------------------
// CCABlockingUI::CreateCustomCommandControlL
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCoeControl* CCABlockingUI::CreateCustomCommandControlL( TInt //aControlType
                                                       )
    {
    //we don`t have any
    return NULL;
    }

// ---------------------------------------------------------
// CCABlockingUI::UnBlockL
// ---------------------------------------------------------
//
void CCABlockingUI::UnBlockL()
    {
    if ( iBlockingDialog && iBlockingArray && iSelectedItem >= 0 &&
         iSelectedItem < iBlockingArray->MdcaCount() )
        {
        iBlockingDialog->SetExitEnabled( EFalse );

        const TDesC& findPaneTxt ( iBlockingDialog->GetFindBoxTextL() );

        //Creating list of unblocked users
        CDesCArrayFlat* unBlockList = iBlockingPC->GetUnBlockListL( iSelectedItem,
                                                                    findPaneTxt );

        CleanupStack::PushL( unBlockList );

        // unBlockList is those persons who are going to be unblocked*/
        // Simulate the findPane functionality when getting the iSelectedItem
        // from the blockingArray structure
        const TCAServerContact* unblocked = iBlockingArray->WVIdL( iSelectedItem, findPaneTxt );

        iAppUi->ShowWaitDialogLC( R_CHAT_BLOCK_REMOVING );

        TInt errorCode( KErrNone );

        if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING ) == 1 )
            {
            errorCode = iBlockingPC->DeleteContactL( iSelectedItem, findPaneTxt );
            }
        HBufC* title = CEikonEnv::Static()->AllocReadResourceLC( R_QTN_CHAT_BLOCKED_CONTACTS_TITLE );
        iAppUi->CAStatusPane()->SetTitleL( *title );
        CleanupStack::PopAndDestroy( title );

        if ( errorCode ) // Some error happened -> Unblock failed.
            {
            iAppUi->DismissWaitDialogL( errorCode );

            iBlockingDialog->SetExitEnabled( ETrue );

            CleanupStack::Pop(); //wait dialog
            CleanupStack::PopAndDestroy( unBlockList );
            return;
            }

        TRAPD( err, errorCode =
                   iBlockingPC->RemoveL( unBlockList, NULL ) );

        iAppUi->DismissWaitDialogL( err );

        iBlockingDialog->SetExitEnabled( ETrue );

        CleanupStack::Pop(); //wait dialog
        if ( errorCode != KErrNone )
            {
            IMNoteMapper::ShowNoteL( errorCode );
            CleanupStack::PopAndDestroy( unBlockList );
            return;
            }

        IMDialogUtils::DisplayNoteDialogL( R_QTN_CHAT_UNBLOCKING_CONFNOTE2 );

        if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_BLOCKING ) != 2 )
            {

            HBufC* prompt = StringLoader::LoadLC( R_CHAT_UNBLOCKING_QUERY,
                                                  unblocked->iIdentification );

            // Show confirmation
            CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog(
                CAknQueryDialog::ENoTone );

            CleanupStack::PushL( dlg );
            dlg->SetPromptL( prompt->Des() );
            CleanupStack::Pop( dlg );
            TInt ret( dlg->ExecuteLD( R_GENERIC_YES_NO_CONFIRMATION_QUERY ) );
            CleanupStack::PopAndDestroy( prompt );

            if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes )  )
                {
                //we have to add id to some list.
                TPtrC listId( KNullDesC() );
                //-1 is item fix for index related to mainviewarray.
                TBool contactSaved( EFalse );
                CCAContactEditor* editor =
                    CCAContactEditor::NewL( -1 , contactSaved );

                editor->EditContactLD( TEnumsPC::EAddToContact, listId, unblocked->iWVID, unblocked->iNickName );
                }
            }

        CleanupStack::PopAndDestroy( unBlockList );

        if ( !iDialogShutter )
            {
            iBlockingPC->GetBlockedMembersL( KMyTab );
            iBlockingDialog->HandleItemRemovalL();
            }
        }

    }

// ---------------------------------------------------------
// CCABlockingUI::BackupTitleL
// ---------------------------------------------------------
//
void CCABlockingUI::BackupTitleL()
    {
    CCAStatusPaneHandler* statusPane = iAppUi->CAStatusPane();

    HBufC* tempTitle = statusPane->Title()->AllocL();
    delete iOldTitle;
    iOldTitle = NULL;
    iOldTitle = tempTitle;
    }

// ---------------------------------------------------------
// CCABlockingUI::DisplayBlockTextQuery2L
// ---------------------------------------------------------
//
void CCABlockingUI::DisplayBlockTextQuery2L()
    {
    DisplayBlockTextQueryL();
    iBlockingPC->GetBlockedMembersL( KMyTab );
    }

// ---------------------------------------------------------
// CCABlockingUI::RemoveIDsL
// ---------------------------------------------------------
//
void CCABlockingUI::RemoveIDsL( const CDesCArray* aUserIds ) const
    {
    iBlockingPC->RemoveIDsL( aUserIds );
    }

// ---------------------------------------------------------
// CCABlockingUI::SetIndexAfterBlocking
// ---------------------------------------------------------
//
void CCABlockingUI::SetIndexAfterBlocking( TBool aMultipleBlocked /*= EFalse*/,
                                           const TDesC& aBlockedId /*= KNullDesC*/ )
    {
    if ( !iBlockingDialog )
        {
        // No dialog
        return;
        }

    if ( aMultipleBlocked )
        {
        // Multiple contacts blocked, set focus to first item
        iBlockingDialog->ListBox()->SetCurrentItemIndexAndDraw( 0 );
        return;
        }

    if ( aBlockedId.Length() > 0 )
        {
        // One id blocked, set focus to that
        if ( iBlockingArray )
            {
            TInt index = iBlockingArray->Find( aBlockedId );
            if ( index != KErrNotFound )
                {
                iBlockingDialog->ListBox()->SetCurrentItemIndexAndDraw( index );
                }
            else
                {
                // Handle errors by setting focus to first item
                iBlockingDialog->ListBox()->SetCurrentItemIndexAndDraw( 0 );
                }
            }
        }
    }
// ---------------------------------------------------------
// CCABlockingUI::DisplayBlockListQueryL
// ---------------------------------------------------------
//
void CCABlockingUI::DisplayBlockListQueryL()
    {
    CDesCArray* blockArray = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( blockArray );
    MCASkinVariant* skinVar = static_cast<CCAApp*>( iAppUi->Application() )
                              ->VariantFactory()->SkinVariantL();


    CCAStatusPaneHandler* titlePane = iAppUi->CAStatusPane();


    MCASettingsPC& settings = *iAppUi->GetProcessManager().GetSettingsInterface();

    // show selection dialog
    TBool contacts( CCAContactSelectionDialog::ShowDialogL( *blockArray, *iMainViewArrayPC,
                                                            *skinVar, settings,
                                                            iAppUi->MbmFullPath(),
                                                            TEnumsPC::EMultiSelect,
                                                            R_CONTACT_SELECTION_DIALOG,
                                                            *iAppUi->CAStatusPane(),
                                                            NULL,
                                                            TEnumsPC::EFilterNotDefined,
                                                            TEnumsPC::EFilterNotDefined,
                                                            ETrue ) );

    // contact selection dialog allowed tabs..
    titlePane->ProhibitTabGroup( ETrue );
    if ( !contacts )
        {
        // no contacts
        CleanupStack::PopAndDestroy();
        return;
        }

    // handle selected contacts (if any)
    TInt count( blockArray->MdcaCount() );
    CHAT_DP( D_CHAT_LIT( "selection count %d" ), count );


    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC wvid( blockArray->MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( "..adding %S to blocking list" ), &wvid );
        }

    BlockMultipleUsersL( blockArray );

    CleanupStack::PopAndDestroy( blockArray );
    }

// ---------------------------------------------------------
// CCABlockingUI::UnblockUserL
// ---------------------------------------------------------
//
void CCABlockingUI::UnblockUserL( const TDesC& aUserId )
    {
    // 1 id is going to be unblocked, 1 for granularity
    CDesCArrayFlat* unBlockList = new( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( unBlockList );
    unBlockList->AppendL( aUserId );

    iAppUi->ShowWaitDialogL( R_CHAT_BLOCK_REMOVING );

    TInt errorCode = KErrNone;
    TRAPD( err, errorCode = iBlockingPC->RemoveL( unBlockList, NULL ) );
    iAppUi->DismissWaitDialogL( err );
    if ( errorCode != KErrNone )
        {
        IMDialogUtils::DisplayErrorNoteL( errorCode );
        }

    CleanupStack::PopAndDestroy( unBlockList );
    }

/**
 * Help Key Support
 * This function is called by the Framework to get the context to launch
 * Help
 *
 * @param aContext	The context that contains the appid and the help id.
 */
void CCABlockingUI::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_BLOCKED_VIEW ;
    }
//  End of File
