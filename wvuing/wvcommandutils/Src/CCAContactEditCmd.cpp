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
* Description:  This component is for contact editing
*
*/



// ============================ HEADER FILES ===============================
//  INCLUDE FILES
#include    "CCAContactEditCmd.h"
#include    "MCAProcessManager.h"
#include	"TEnumsPC.h"
#include	"ImpsCSPAllErrors.h"
#include 	"IMNoteMapper.h"
#include 	"IMDialogUtils.h"

#include	<chatNG.rsg>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCAContactEditCmd::~CCAContactEditCmd()
    {

    }

// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//
CCAContactEditCmd::CCAContactEditCmd(
    MCAUiContactEditCmdCB& aUiContactEditCmdCB,
    MCAContactEditPC & aCCAContactEditPC )
        :
        iUiContactEditCmdCB( aUiContactEditCmdCB ),
        iCCAContactEditPC( static_cast <CCAContactEditPC&>( aCCAContactEditPC ) )
    {

    }


// -----------------------------------------------------------------------------
// CCAContactEditCmd::NewL
// -----------------------------------------------------------------------------
//
CCAContactEditCmd* CCAContactEditCmd::NewL( MCAProcessManager& aProcessManager,
                                            MCAUiContactEditCmdCB& aUiContactEditCmdCB )
    {
    CCAContactEditCmd* self = CCAContactEditCmd::NewLC( aProcessManager ,
                                                        aUiContactEditCmdCB );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CCAContactEditCmd::NewLC
// -----------------------------------------------------------------------------
//
CCAContactEditCmd* CCAContactEditCmd::NewLC( MCAProcessManager& aProcessManager,
                                             MCAUiContactEditCmdCB& aUiContactEditCmdCB )
    {
    CCAContactEditCmd* self = new( ELeave ) CCAContactEditCmd(
        aUiContactEditCmdCB,
        *aProcessManager.GetContactEditInterface() );
    CleanupStack::PushL( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CCAContactEditCmd::SetObserver
// -----------------------------------------------------------------------------
//
void CCAContactEditCmd::SetObserver( MCACommandObserver& /*aObserver*/ )
    {

    }


// -----------------------------------------------------------------------------
// CCAContactEditCmd::CancelCommand
// -----------------------------------------------------------------------------
//
void CCAContactEditCmd::CancelCommand()
    {

    }

// -----------------------------------------------------------------------------
// CCAContactEditCmd::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void CCAContactEditCmd::ExecuteCommandL()
    {
    MCAUiContactEditCmdCB::TContactEditParams params;

    iUiContactEditCmdCB.GetParameters( params );

    switch ( params.iContactEditOperation )
        {
        case MCAUiContactEditCmdCB::TContactEditParams::ELoadContactDetails :
            {
            LoadCCAContactDetailsL( params ) ;
            break;
            }

        case MCAUiContactEditCmdCB::TContactEditParams::EAddContact :
            {
            SaveCCAContactDetailsL( params ) ;
            break;
            }
        case MCAUiContactEditCmdCB::TContactEditParams::EUpdateContact :
            {
            SaveCCAContactDetailsL( params ) ;
            break;
            }
        case MCAUiContactEditCmdCB::TContactEditParams::EMoveContact :
            {
            MoveContactL( params ) ;
            break;
            }

        default:
            {
            break;
            }
        }

    iUiContactEditCmdCB.SetParameters( params );

    }

// -----------------------------------------------------------------------------
// CCAContactEditCmd::IsAsynchronous
// -----------------------------------------------------------------------------
//
TBool CCAContactEditCmd::IsAsynchronous()
    {
    return ETrue;
    }



// -----------------------------------------------------------------------------
// CCAContactEditCmd::LoadCCAContactDetails
// -----------------------------------------------------------------------------
//
void CCAContactEditCmd::LoadCCAContactDetailsL(
    MCAUiContactEditCmdCB::TContactEditParams &aParams )
    {

    iCCAContactEditPC.GetContactDetailsL(
        aParams.iAliasEnabled,
        aParams.iNicknameEnabled,
        aParams.iOwnData,
        aParams.iOrigId,
        aParams.iAlias,
        aParams.iNickname,
        aParams.iUserId,
        aParams.iIdentification );


    }



// -----------------------------------------------------------------------------
// CCAContactEditCmd::SaveCCAContactDetails
// -----------------------------------------------------------------------------
//
void CCAContactEditCmd::SaveCCAContactDetailsL
( MCAUiContactEditCmdCB::TContactEditParams &aParams )
    {

    aParams.iContactEditOperationError = KErrNone ;

    if ( aParams.iNewContact )
        {
        aParams.iContactEditOperationError =
            iCCAContactEditPC.AddContactL( *aParams.iListId,
                                           *aParams.iUserId,
                                           *aParams.iNickname,
                                           *aParams.iAlias );
        }
    else
        {
        aParams.iContactEditOperationError =
            iCCAContactEditPC.UpdateContactL( *aParams.iListId,
                                              *aParams.iUserId,
                                              *aParams.iNickname );
        }

    switch ( aParams.iContactEditOperationError )
        {
        case KErrNone:
            {
            TInt noteId( aParams.iNewContact ?
                         R_QTN_CHAT_CONTACT_ADD_NOTE : R_QTN_CHAT_CNOTE_SAVED );
            // find the contact in storage
            // If the wvid was edited, the contact is different and we
            // must search it from the storage
            iCCAContactEditPC.SetContactL( *aParams.iListId, *aParams.iUserId ) ;

                {
                // check nickname
                HBufC* newNick =
                    iCCAContactEditPC.GetCurrentContactNickname().AllocL();

                if ( newNick )
                    {
                    // contact found and it has a nickname
                    CleanupStack::PushL( newNick );
                    TPtrC newNickPtr( newNick->Des() );

                    // compare it to the one we tried to add
                    if ( newNickPtr.CompareC( *aParams.iNickname ) != 0
                         && aParams.iNicknameEnabled )
                        {
                        // nickname was changed
                        noteId = R_QTN_CHAT_SERVER_ALTERED_NICK;
                        }
                    CleanupStack::PopAndDestroy( newNick );
                    }
                }
            IMDialogUtils::DisplayConfirmationNoteL( noteId );

            break;
            }

        case ECSPPartiallySuccessful:
            {
            // show the detailed error codes
            const RPointerArray<MCAPresenceError>& errorArray =
                iCCAContactEditPC.LastContactEditOperationResultL();

            TInt count( errorArray.Count() );


            //for partially successful.

            if ( count == 0 )
                {
                IMNoteMapper::ShowNoteL( ECSPUnknownUserId, *aParams.iUserId );
                }

            for ( TInt i( 0 ); i < count; i++ )
                {
                TInt error = errorArray[ i ]->ErrorCode();
                const TDesC& userId = errorArray[i]->UserId();
                if ( userId.Length() > 0 )
                    {
                    IMNoteMapper::ShowNoteL( error, userId );
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }

    }


// -----------------------------------------------------------------------------
// CCAContactEditCmd::MoveContactL
// -----------------------------------------------------------------------------
//

void CCAContactEditCmd::MoveContactL(
    MCAUiContactEditCmdCB::TContactEditParams &aParams )
    {



    // make a copy of ids so that instances are
    // valid when storage contacts disappear
    HBufC* userId = iCCAContactEditPC.GetCurrentContactUserId().AllocLC();
    HBufC* nickName = iCCAContactEditPC.GetCurrentContactNickname().AllocLC();

    // We have to remove first, because some servers
    // don't support same nicknames on different lists.
    // Yes, nicknames.
    aParams.iContactEditOperationError = iCCAContactEditPC.RemoveContactL(
                                             *aParams.iListId,
                                             *userId ) ;
    if ( aParams.iContactEditOperationError == KErrNone )
        {

        aParams.iContactEditOperationError = iCCAContactEditPC.AddContactL(
                                                 *aParams.iUserId,
                                                 *userId,
                                                 *nickName,
                                                 KNullDesC() );
        if ( aParams.iContactEditOperationError != KErrNone )
            {
            // adding failed, rollback
            // we can ignore the return value, because in this case
            // we will show the error from previous AddContactL
            iCCAContactEditPC.AddContactL( *aParams.iListId,
                                           *userId,
                                           *nickName,
                                           KNullDesC() );
            }
        }

    CleanupStack::PopAndDestroy( 2, userId ); // nickName, userId

    }


