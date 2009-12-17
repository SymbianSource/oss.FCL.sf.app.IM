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
* Description:  Reactive authorization handling utilities
*
*/


//INCLUDES
#include "CCAReactiveAuthorization.h"
#include "CCAAppUi.h"
#include "IMDialogUtils.h"
#include "StringLoader.h"
#include "IMUtils.h"
#include "CCAContactEditor.h"
#include "CAUtils.h"
#include "CCABlockingUI.h"
#include "ChatDebugAssert.h"
#include "MCAReactiveAuthPC.h"
#include "MCASettingsPC.h"
#include "MCAProcessManager.h"
#include "MCABlockingPC.h"
#include <chatNG.rsg>
#include <chatNG.mbg>
#include "CCAUINGUtils.h"
#include "CCAPCUtils.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::CCAReactiveAuthorization
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAReactiveAuthorization::CCAReactiveAuthorization( CCAAppUi& aAppUi,
                                                    MCAReactiveAuthPC* aReactiveAuthPC )
        : iAppUi( aAppUi ),
        iReactiveAuthPC( aReactiveAuthPC )
    {
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAReactiveAuthorization* CCAReactiveAuthorization::NewLC( CCAAppUi& aAppUi,
                                                           MCAReactiveAuthPC* aReactiveAuthPC )
    {
    CCAReactiveAuthorization* temp =
        new( ELeave )CCAReactiveAuthorization( aAppUi, aReactiveAuthPC );
    CleanupStack::PushL( temp );
    temp->ConstructL();
    return temp;
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAReactiveAuthorization* CCAReactiveAuthorization::NewL( CCAAppUi& aAppUi,
                                                          MCAReactiveAuthPC* aReactiveAuthPC )
    {
    CCAReactiveAuthorization* temp = CCAReactiveAuthorization::NewLC( aAppUi, aReactiveAuthPC );
    CleanupStack::Pop( temp );
    return temp;
    }

// Destructor
CCAReactiveAuthorization::~CCAReactiveAuthorization()
    {
    delete iIdle;
    delete iBlockingUI;
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthorization::ConstructL()
    {
    iIdle = CIdle::NewL( CActive::EPriorityIdle );

    // iBlockingUI is constructed when it's needed,
    // because of the login speedup
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::HandleRAPendingReqL
// New pending request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthorization::HandleRAPendingReqL()
    {
    if ( ! IMUtils::IntResourceValueL(
             RSC_CHAT_VARIATION_REACTIVE_PRESENCE_AUTH ) )
        {
        // no RA active
        return;
        }

    // launch consumer method via CIdle
    if ( !iIdle->IsActive() && !iRequestPending )
        {
        // not active, launch it again
        iIdle->Start( TCallBack( ConsumerCallback, this ) );
        iRequestPending = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::HandleRAStatusL
// New RA status message
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthorization::HandleRAStatusL()
    {
    if ( ! IMUtils::IntResourceValueL(
             RSC_CHAT_VARIATION_REACTIVE_PRESENCE_AUTH ) )
        {
        // no RA active
        return;
        }

    // launch consumer via CIdle
    if ( ! iIdle->IsActive() && !iRequestPending )
        {
        // not active, launch it again
        iIdle->Start( TCallBack( ConsumerCallback, this ) );
        iRequestPending = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::HandleRAError
// Error case, some of the RA observer methods left.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthorization::HandleRAError( TInt aError )
    {
    if ( aError )
        {
        CActiveScheduler::Current()->Error( aError );
        }
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::ConsumerCallback
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAReactiveAuthorization::ConsumerCallback( TAny* aInstance )
    {
    return static_cast<CCAReactiveAuthorization*>( aInstance )->Consumer();
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::Consumer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAReactiveAuthorization::Consumer()
    {
    TRAPD( err, iRequestPending = DoConsumerL() );

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        iRequestPending = EFalse;
        }

    return iRequestPending;
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::DoConsumerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAReactiveAuthorization::DoConsumerL()
    {
    // 1. get pending messages from IM Presence
    //   - handle queue
    // 2. Get pending statuses from IM Presence
    //   - handle queue
    // 3. If both queues are empty, get out



    HBufC* userId = HBufC::NewLC( KMaxWVIDLength );

    // since this method got called, there has to be at least one (1) in either
    // array
    TPtr userIdPtr( userId->Des() );
    TInt id = 0;
    TInt lastPendingPos = 0;

    // eat up the request queue
    // can get the whole list at once on pc side( like have been done for
    // status queue) n then handle here
    // can do later if it is a performance issue
    while ( iReactiveAuthPC->GetNextPendingRARequestL( id, userIdPtr,
                                                       lastPendingPos ) )
        {
        // keep on emptying the queue
        TBool response = DoHandleRAPendingReqL( *userId );
        iReactiveAuthPC->SendReactiveAuthResponseL( id, response );
        }


    // eat up the status queue
    CDesCArray* userIdsArray =  new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( userIdsArray );

    iReactiveAuthPC->GetListOfRAStatusL( userIdPtr, *userIdsArray );
    TInt count = userIdsArray->Count();
    for ( TInt i( 0 ); i < count; i++ )
        {
        // the user rejected us
        DoHandleRAStatusL( userIdsArray->MdcaPoint( i ) );
        }

    CleanupStack::PopAndDestroy( userIdsArray );
    CleanupStack::PopAndDestroy( userId );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::DoHandleRAPendingReqL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAReactiveAuthorization::DoHandleRAPendingReqL( const TDesC& aId )
    {
    // reactive authorization pending request

    TBool accepted = EFalse;


    TInt authorizePresence = iAppUi.GetProcessManager().GetSettingsInterface()
                             ->GetIntValuePC( TEnumsPC::EAuthorizeIMPresence, NULL );
    if ( !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVEPRESENCEAUTH ) )
        {
        // if VARIATION_HAVEPRESENCEAUTH is disabled, we always show RA query
        authorizePresence = TEnumsPC::EFriends;
        }

    switch ( authorizePresence )
        {
        case TEnumsPC::EAllValue:
            {
            accepted = ETrue;
            break;
            }
        case TEnumsPC::ENobody:
            {
            accepted = EFalse;
            break;
            }
        case TEnumsPC::EFriends:
            {
            // check if we have that contact

            TBool isContactFound( iReactiveAuthPC->FindAnyContactL( aId ) );
            MCABlockingPC* blockingPC = iAppUi.GetProcessManager().GetBlockingInterface();
            TBool isContactBlocked = blockingPC->IsContactBlocked( aId );

            if ( isContactFound && !isContactBlocked )
                {
                // we have the user in our contactlist
                accepted = ETrue;
                }
            else if ( isContactBlocked )
                {
                // we have the user in our blocked list
                accepted = EFalse;
                }
            else
                {
                TInt aliasErr( 0 );
                HBufC* aliasTxt = NULL;
                HBufC* queryTxt = NULL;

                // Get alias

                TRAP( aliasErr, aliasTxt = iReactiveAuthPC->GetAliasL( aId ) );
                CleanupStack::PushL( aliasTxt );

                TBool validAlias( EFalse );
                if ( aliasTxt )
                    {
                    validAlias = TBool( aliasTxt->Length() );
                    }

                if ( aliasErr == KErrNotFound || !validAlias )
                    {
                    // no alias, use wvid
                    queryTxt = StringLoader::LoadLC(
                                   R_QTN_CHAT_REACTIVE_AUTHORIZE_QUERY,
                                   CCAPCUtils::DisplayId( aId ) );
                    }
                else
                    {
                    if ( aliasErr == KErrNone )
                        {
                        // alias found
                        queryTxt = StringLoader::LoadLC( R_QTN_CHAT_REACTIVE_AUTHORIZE_QUERY,
                                                         *aliasTxt );
                        }
                    else
                        {
                        // all other errors
                        User::Leave( aliasErr );
                        }
                    }

                // Ask from user to accept authorization
                TInt result = IMDialogUtils::DisplayQueryDialogL(
                                  R_GENERIC_YES_NO_CONFIRMATION_QUERY,
                                  *queryTxt );

                CleanupStack::PopAndDestroy( queryTxt );
                CleanupStack::PopAndDestroy( aliasTxt );

                if ( ( result == EAknSoftkeyOk ) || ( result == EAknSoftkeyYes ) )
                    {
                    // recipient has accepted authorization
                    accepted = ETrue;

                    // we don't have the contact,
                    // so ask if the user wants to add it
                    queryTxt = StringLoader::LoadLC( R_QTN_CHAT_REACTIVE_ADD_LIST,
                                                     CCAPCUtils::DisplayId( aId ) );
                    result = IMDialogUtils::DisplayQueryDialogL(
                                 R_GENERIC_YES_NO_CONFIRMATION_QUERY,
                                 *queryTxt );

                    CleanupStack::PopAndDestroy( queryTxt );

                    if ( ( result == EAknSoftkeyOk ) || ( result == EAknSoftkeyYes ) )
                        {
                        // verify that we have a contact list
                        if ( CCAUINGUtils::VerifyContactlistL() ==
                             CCAUINGUtils::EChatNoContactList )
                            {
                            // Could not create contact list
                            break;
                            }

                        // add the contact to friends list
                        TBool contactSaved = EFalse;

                        CCAContactEditor* contactEditor =
                            CCAContactEditor::NewL( -1, contactSaved );
                        TInt err;
                        TRAP( err, contactEditor->EditContactLD(
                                  TEnumsPC::EAddToContact, KNullDesC(), aId ) );
                        }
                    }
                else
                    {
                    // user did not auth, ask whether to block
                    queryTxt = StringLoader::LoadLC( R_QTN_CHAT_REACTIVE_BLOCK,
                                                     CCAPCUtils::DisplayId( aId ) );
                    result = IMDialogUtils::DisplayQueryDialogL(
                                 R_GENERIC_YES_NO_CONFIRMATION_QUERY, *queryTxt );
                    CleanupStack::PopAndDestroy( queryTxt );

                    if ( ( result == EAknSoftkeyOk ) ||
                         ( result == EAknSoftkeyYes ) )
                        {
                        // make sure that blocking interface exists
                        iAppUi.FinalizeChatConstructionL();

                        // block the contact
                        if ( !iBlockingUI )
                            {

                            iBlockingUI = CCABlockingUI::NewL( blockingPC );

                            }

                        iBlockingUI->BlockUserL( aId );
                        }

                    accepted = EFalse;
                    }
                }
            break;
            }
        default:
            {
            // We should not get here... some strange value
            __CHAT_ASSERT_DEBUG( EFalse );
            break;
            }
        }

    return accepted;
    }

// -----------------------------------------------------------------------------
// CCAReactiveAuthorization::DoHandleRAStatusL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAReactiveAuthorization::DoHandleRAStatusL( const TDesC& aId )
    {
    // reactive authorization status
    HBufC* text = StringLoader::LoadLC(
                      R_QTN_CHAT_REACTIVE_NOT_AUTHORIZED, CCAPCUtils::DisplayId( aId ) );
    IMDialogUtils::DisplayNoteDialogL( *text, R_AVKON_SOFTKEYS_OK_EMPTY__OK,
                                       CAknNoteDialog::ENoTimeout );
    CleanupStack::PopAndDestroy( text );
    }

// End of file

