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
* Description:  General utility class
*
*/



// INCLUDE FILES
#include    "CCAUINGUtils.h"
#include	"CCAAppUi.h"
#include 	"MCAProcessManager.h"
#include 	"MCAMainViewArrayPC.h"
#include    "CCAWaitNote.h"
#include    "IMDialogUtils.h"
#include    "IMNoteMapper.h"
#include    "PublicEngineDefinitions.h"
#include    "CCAPCUtils.h"
#include    "aknnotewrappers.h"
#include	"CCAAppUi.h"
#include    "CCAStatusPaneHandler.h"

#include    <ChatNG.rsg>
#include    <hlplch.h>
#include    <csxhelp/imng.hlp.hrh>
#include    <eikmenup.h>

#include	<StringLoader.h>
#include    "ImpsCSPAllErrors.h"
#include    <PEngWVPresenceErrors2.h>   // wv error codes
// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

#include    "IMUtils.h"
#include    "CAUtils.h"

// -----------------------------------------------------------------------------
// CCAUINGUtils::NewContactlistL
// Creates a new contact list and shows the appropriate dialogs
// -----------------------------------------------------------------------------
//
TInt CCAUINGUtils::NewContactListL( TInt& aIndex )
    {
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    MCAMainViewArrayPC* mainViewArrayPC = appUi->GetProcessManager().GetArrayInterface();
    TInt capitalizeListname( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_CONTACT_LISTS_CAPITALIZED ) );
    // ask name for new list
    HBufC* name = HBufC::NewLC( KMaxWVIDLength );
    TPtr namePtr( name->Des() );
    TInt err( KErrCancel );

    // We loop until we get a valid name or user cancels
    do
        {

        TInt result( IMDialogUtils::DisplayTextQueryDialogL( namePtr,
                                                             R_QTN_CHAT_CONTACT_LIST_NAME_QUERY,
                                                             R_CHATCLIENT_CONVLIST_RECIP_QUERY,
                                                             ETrue // T9
                                                           ) );

        if ( result == EAknSoftkeyOk || result == EAknSoftkeyDone )
            {
            // show wait note and create list

            TInt contactListsExists = mainViewArrayPC->ContactListsExistL();
            if ( contactListsExists == KErrGeneral )
                {
                //can not proceed
                HBufC* text = StringLoader::LoadLC( R_QTN_CHAT_CONTACT_LIST_FAILED );
                IMDialogUtils::DisplayErrorConfirmationNoteL( *text );
                CleanupStack::PopAndDestroy( text );
                return KErrGeneral;
                }

            err = mainViewArrayPC->ListAlreadyExistsL( namePtr );


            if ( err == KErrNone )
                {
                CCAWaitNote* waitNote =
                    CCAWaitNote::ShowWaitNoteLC( R_QTN_GEN_NOTE_SAVING );

                // UI CR  ID: 101-39727 : capitalizing contact listname

                if ( capitalizeListname )
                    {
                    HBufC* capitalizedList = CAUtils::CapitalizeListNameL( *name );
                    CleanupStack::PushL( capitalizedList );
                    err = err  = mainViewArrayPC->CreateContactListL( *capitalizedList , aIndex );
                    CleanupStack::PopAndDestroy();
                    }
                else
                    {
                    err  = mainViewArrayPC->CreateContactListL( *name , aIndex );
                    }
                CleanupStack::PopAndDestroy(); // waitnote
                }

            if ( err != KErrNone )
                {
                HBufC* text = NULL;

                // Max number of contact lists exeeded
                if ( err == KPEngNwErrMaxNumberOfContactListsReached )
                    {
                    text = StringLoader::LoadLC( R_QTN_CHAT_CONTACT_LISTS_FULL );
                    IMDialogUtils::DisplayErrorConfirmationNoteL( *text );
                    CleanupStack::PopAndDestroy( text );
                    }

                // contact list already exists
                else if ( err == KErrAlreadyExists ||
                          err == KPEngNwErrContactListAlreadyExists )
                    {
                    text = StringLoader::LoadLC(
                               R_QTN_FLDR_NAME_ALREADY_USED, namePtr );
                    IMDialogUtils::DisplayInformationNoteL( *text );
                    CleanupStack::PopAndDestroy( text );
                    }

                // default error message
                else
                    {
                    text = StringLoader::LoadLC( R_QTN_CHAT_CONTACT_LIST_FAILED );
                    IMDialogUtils::DisplayErrorConfirmationNoteL( *text );
                    CleanupStack::PopAndDestroy( text );
                    }
                }
            }
        else
            {
            // User canceled the dialog
            err = KErrCancel;
            }


        } while ( err != KErrNone && err != KErrCancel
              && err != KPEngNwErrMaxNumberOfContactListsReached );

    CleanupStack::PopAndDestroy( name );
    return err;
    }


// ---------------------------------------------------------
// CCAUINGUtils::VerifyContactlistL()
// Verifies the we have at least one contact list
// else tries to create one.
// ---------------------------------------------------------
//
CCAUINGUtils::TChatContactListCreationType CCAUINGUtils::VerifyContactlistL()
    {
    // Check that we have even one contact list where to add the contact
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    MCAMainViewArrayPC* mainViewArrayPC = appUi->GetProcessManager().GetArrayInterface();

    if ( mainViewArrayPC->ListCountL() == 0 )
        {
        IMDialogUtils::DisplayQueryDialogL( R_CHAT_NO_CONTACT_LISTS_QUERY );
        TInt initValue = KErrNotFound;
        if ( NewContactListL( initValue ) != KErrNone )
            {
            // contact list was not created
            return EChatNoContactList;
            }
        else
            {
            // new contact list was created
            return EChatContactListWasCreated;
            }
        }

    return EChatContactListExists;
    }


// -----------------------------------------------------------------------------
// CCAUINGUtils::HandleErrorL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAUINGUtils::HandleErrorL( TInt aError )
    {
    if ( aError == ECSPPartiallySuccessful )
        {
        HandlePartialSuccessL();
        }
    else if ( aError != KErrNone )
        {
        User::Leave( aError );
        }
    }

// -----------------------------------------------------------------------------
// CCAUINGUtils::HandlePartialSuccessL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAUINGUtils::HandlePartialSuccessL()
    {
    TInt errorCode( 0 );
    for ( TInt i( 0 ); i < CCAPCUtils::PartialSucessCountL(); i++ )
        {
        errorCode = CCAPCUtils::GetErrorCodeL( i );
        if ( errorCode == ECSPInvalidParameter || errorCode == ECSPUnknownUserId )
            {
            errorCode = ECSPUnknownUserId;
            IMNoteMapper::ShowNoteL( errorCode );
            }
        }
    if ( CCAPCUtils::PartialSucessCountL() == 0 )
        {
        IMNoteMapper::ShowNoteL( EGeneralServerError );
        }
    }

// -----------------------------------------------------------------------------
// CCAUINGUtils::SetTitleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAUINGUtils::SetTitleL( TInt aResourceId, const TUid aViewId )
    {
    HBufC* chatListViewTitle =
        CCoeEnv::Static()->AllocReadResourceLC( aResourceId );
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    CCAStatusPaneHandler* statusPane = appUi->CAStatusPane();

    statusPane->SetTitleL( *chatListViewTitle );
    CleanupStack::PopAndDestroy( chatListViewTitle );
    statusPane->ShowTabGroupL( aViewId );
    }

// -----------------------------------------------------------------------------
// CCAUINGUtils::DisplayErrorNoteL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAUINGUtils::DisplayErrorNoteL( TInt aResourceId )
    {
    HBufC* noteBuf = CCoeEnv::Static()->AllocReadResourceLC( aResourceId );
    CAknErrorNote* errorNote = new( ELeave ) CAknErrorNote();
    errorNote->ExecuteLD( *noteBuf );
    CleanupStack::PopAndDestroy( noteBuf );
    }
