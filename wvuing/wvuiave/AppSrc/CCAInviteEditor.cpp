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
* Description:  Editor for invites
*
*/


// INCLUDE FILES
#include	"CCAInviteEditor.h"		// header for this .cpp.
#include	"IMDialogUtils.h"
#include	"ChatDefinitions.h"
#include    "CCAApp.h"
#include    "CCAAppUi.h"
#include	"CCAVariantFactory.h"
#include    "ChatDebugPrint.h"
#include    "IMPSBuildDefinitions.h"
#include    "CCAContactSelectionDialog.h"
#include    "MCAInviteConflictResolver.h"
#include    "MCASettingsPC.h"
#include    "MCAMainViewArrayPC.h"
#include    "MCAProcessManager.h"
#include    "MCAInviteEditorPC.h"
#include    "MCAServerContactsArrayPC.h"
#include 	"ImpsCSPAllErrors.h"
#include 	"CCAPCUtils.h"

#include	<eikenv.h>
#include	<chatNG.rsg>
#include    <stringloader.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCAInviteEditor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAInviteEditor* CCAInviteEditor::NewL( const TDesC& aGroup )
    {
    CCAInviteEditor* self = new ( ELeave ) CCAInviteEditor( );

    CleanupStack::PushL( self );
    self->ConstructL( aGroup );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCAInviteEditor::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAInviteEditor* CCAInviteEditor::NewLC( const TDesC& aGroup )
    {
    CCAInviteEditor* self = new ( ELeave ) CCAInviteEditor( );

    CleanupStack::PushL( self );
    self->ConstructL( aGroup );
    return self;
    }

// Destructor
CCAInviteEditor::~CCAInviteEditor()
    {
    delete iGroupID;
    delete iInviteEditorPC;
    }

// -----------------------------------------------------------------------------
// CCAInviteEditor::CreateAndSendInvitation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAInviteEditor::CreateAndSendInvitation( MCAInviteConflictResolver& aResolver,
                                                const TDesC& aReceiver )
    {
    TRAPD( err, CreateAndSendInvitationL( aResolver, aReceiver ) );

    if ( err != KErrNone )
        {
        TRAPD( ign, ShowErrorNoteL() );

        if ( ign )
            {
            // propagate system errors to current active scheduler,
            CActiveScheduler::Current()->Error( ign );
            }
        }

    return ( err == KErrNone );
    }


// -----------------------------------------------------------------------------
// CCAInviteEditor::PopulateInviteListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAInviteEditor::PopulateInviteListL( CDesCArray& aInviteList,
                                           const MCAServerContactsArrayPC& aRawOnlineList,
                                           const CArrayFixFlat<TInt>& aSelectionArray )
    {
    iInviteEditorPC->PopulateInviteListL( aInviteList,
                                          aRawOnlineList,
                                          aSelectionArray );

    }

// -----------------------------------------------------------------------------
// CCAInviteEditor::QueryInviteMessageL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCAInviteEditor::QueryInviteMessageL()
    {
    HBufC* message = HBufC::NewLC( KInviteMessageMaxLength );
    TPtr messagePtr( message->Des() );

    if ( !IMDialogUtils::DisplayTextQueryDialogL( messagePtr,
                                                  R_CHAT_INVITE_MSG_DQ, R_CHAT_INVITE_MESSAGE_QUERY,
                                                  ETrue /* T9 */,
                                                  EFalse, // Show LSK only when needed
                                                  IMDialogUtils::ESend
                                                ) )
        {
        CleanupStack::PopAndDestroy( message );
        return NULL;
        }
    else
        {
        CleanupStack::Pop( message );
        return message;
        }
    }

// -----------------------------------------------------------------------------
// CCAInviteEditor::ShowErrorNoteL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAInviteEditor::ShowErrorNoteL()
    {
    HBufC* prompt = CEikonEnv::Static()->AllocReadResourceLC(
                        R_CHAT_INVITE_SEND_ERROR );
    IMDialogUtils::DisplayErrorNoteL( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }


// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAInviteEditor::CCAInviteEditor(  )
    {
    }

// -----------------------------------------------------------------------------
// CCAInviteEditor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAInviteEditor::ConstructL( const TDesC& aGroup )
    {
    iGroupID = aGroup.AllocL();
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    iInviteEditorPC = appUi->GetProcessManager().CreateInviteEditorPCL();
    }

// -----------------------------------------------------------------------------
// CCAInviteEditor::CreateAndSendInvitationL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAInviteEditor::CreateAndSendInvitationL( MCAInviteConflictResolver& aResolver,
                                                const TDesC& aReceiver
                                              )
    {

    CDesCArray* inviteList = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( inviteList );


    if ( aReceiver.Length() == 0 )
        {
        CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
        CDesCArray* selectedContacts = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
        CleanupStack::PushL( selectedContacts );
        MCASkinVariant* skinVar = static_cast<CCAApp*>( appUi->Application() )
                                  ->VariantFactory()->SkinVariantL();


        MCASettingsPC* settingsPC = appUi->GetProcessManager().GetSettingsInterface();
        MCAMainViewArrayPC* mainViewArrayPC = appUi->GetProcessManager().GetArrayInterface();
        if ( ! CCAContactSelectionDialog::ShowDialogL( *selectedContacts,
                                                       *mainViewArrayPC,
                                                       *skinVar, *settingsPC,
                                                       appUi->MbmFullPath(),
                                                       TEnumsPC::EMultiSelect,
                                                       R_CONTACT_SELECTION_DIALOG,
                                                       *appUi->CAStatusPane(),
                                                       NULL ) )
            {
            // no contacts
            CleanupStack::PopAndDestroy( 2 ); // selectedContacts.Close(), inviteList
            return;
            }

        // handle selected contacts (if any)
        TInt count( selectedContacts->MdcaCount() );
        CHAT_DP( D_CHAT_LIT( "selection count %d" ), count );
        for ( TInt i( 0 ); i < count; ++i )
            {

            TPtrC wvid( selectedContacts->MdcaPoint( i ) );

            CHAT_DP( D_CHAT_LIT( "..adding %S to invite list" ), &wvid );
            // Prevent duplicates and don't add if user canceled offline notification
            TInt pos( KErrNone );
            if ( inviteList->Find( wvid, pos ) != KErrNone )
                {
                inviteList->AppendL( wvid );
                }
            }

        CleanupStack::PopAndDestroy(); // selectedContacts.Close()
        }
    else
        {
        inviteList->AppendL( aReceiver );
        }

    // If user selected something, then proceed...
    if ( inviteList->MdcaCount() > 0 )
        {
        // Ask for invite message
        HBufC* message = QueryInviteMessageL();

        if ( message )
            {
            CleanupStack::PushL( message );

            // Resolve invite conflicts
            aResolver.ResolveInviteConflictL( *iGroupID, *inviteList );

            TInt inviteeCount = inviteList->MdcaCount();
            if ( inviteeCount > 0 )
                {
                // Create invitation
                // Show wait-note
                CCAAppUi* appUi =
                    static_cast< CCAAppUi* >( CCoeEnv::Static()->AppUi() );

                // Show wait-note. Error state dismisses it.
                appUi->ShowWaitDialogL( inviteList->MdcaCount() > 1 ?
                                        R_QTN_CHAT_INVITE_SENDING_MANY : R_QTN_CHAT_INVITE_SENDING,
                                        ETrue );
                // Send invitation
                TRAPD( retval, iInviteEditorPC->SendInvitationL( *inviteList,
                                                                 *iGroupID,
                                                                 *message ) );

                CleanupStack::PopAndDestroy( message );
                // Get rid of wait-note
                if ( retval == ECSPRecipientBlockedSender )
                    {
                    for ( TInt cnt = 0; cnt < inviteeCount; cnt++ )
                        {
                        appUi->DismissWaitDialogL( retval, EFalse,
                                                   CCAPCUtils::DisplayId( inviteList->MdcaPoint( cnt ) ) );
                        }
                    }
                else
                    {
                    appUi->DismissWaitDialogL( retval );
                    }

                }
            else
                {
                CleanupStack::PopAndDestroy( message );
                }

            }
        else
            {
            // User cancelled invite message query,
            // return to contact selection view
            CleanupStack::PopAndDestroy( inviteList );
            return;
            }
        }
    CleanupStack::PopAndDestroy( inviteList );

    }

// -----------------------------------------------------------------------------
// CCAInviteEditor::GetOnlineFriendsLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAServerContactsArrayPC* CCAInviteEditor::GetOnlineFriendsLC()
    {
    MCAServerContactsArrayPC* pairsArray = iInviteEditorPC->GetOnlineFriendsLC( KIconTabulator );
    return pairsArray;
    }

//  End of File

