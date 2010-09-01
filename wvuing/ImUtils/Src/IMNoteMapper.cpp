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
* Description:  Utility class for mapping error codes to notes
*
*/


// INCLUDE FILES
#include "IMNoteMapper.h"
#include "IMDialogUtils.h"
#include "ChatDebugPrint.h"
#include <stringloader.h>
#include <ChatNG.rsg>
#include <eikenv.h>
#include "ImpsCSPAllErrors.h"
#include "PublicEngineDefinitions.h"
#include "imutils.h"

// ---------------------------------------------------------
// IMNoteMapper::ShowNoteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void IMNoteMapper::ShowNoteL( const TInt aErrorCode, const TDesC& aAdditionalData )
    {
    TBool ignoreAdditionalData( EFalse ); // Some logical strings are without parameters...
    TBool errorNote( ETrue );
    TBool defaultNote( EFalse );
    TInt resourceID( 0 );
    TBool confirmationNote( EFalse );
    TBool softkeyOk( EFalse );

    switch ( aErrorCode )
        {
        case KErrNoMemory:
            {
            errorNote = ETrue;
            resourceID = R_CHAT_CEDITOR_MEMLO;
            ignoreAdditionalData = ETrue;
            break;
            }
        case KErrDiskFull:
            {
            errorNote = ETrue;
            resourceID = R_CHAT_CEDITOR_MEMFULL;
            ignoreAdditionalData = ETrue;
            break;
            }

        // flowthrough
        case ECSPPartiallySuccessful:
        case ECSPSuccessful:
        case KErrNone:
            {
            CHAT_DP_TXT( "IMNoteMapper::ShowNoteL - not an error" );
            return; // don't show any notes
            }

        case ECSPRecipientBlockedSender: // recipient has blocked sender
            {
            resourceID = R_CHAT_RECIPIENT_BLOCKED_MESSAGES;
            ignoreAdditionalData = EFalse;
            errorNote = ETrue;
            softkeyOk = ETrue;
            break;
            }

        // flowthrough
        case ESearchNotSupported:
        case ECSPNotSupported: // Service not supported
        case ECSPServiceNotAgreed: // Service not agreed
            {
            resourceID = R_CHAT_ERROR_SERVICE_ERR;
            ignoreAdditionalData = ETrue;
            break;
            }

        case ECSPWrongPassword: // Invalid password
            {
            resourceID = R_CHAT_LOGIN_PASSER_NOTE;
            ignoreAdditionalData = ETrue;
            break;
            }

        case ECSPCannotDeliver: // Unable to deliver
            {
            resourceID = R_CHAT_INVITE_SEND_ERROR;
            ignoreAdditionalData = ETrue;
            break;
            }

        // flowthrough
        case ECSPServiceUnavailable: // Service unavailable
        case ECSPMessageQueueFull: // Message queue is full
            {
            resourceID = R_CHAT_ERROR_SERV_BUSY;
            ignoreAdditionalData = ETrue;
            break;
            }

        case ECSPUnsupportedVersion: // Version not supported
            {
            resourceID = R_CHAT_ERROR_GEN_ERROR;
            ignoreAdditionalData = ETrue;
            break;
            }
        case ECSPInvalidSearchId: // Invalid search id. Search timed out.
        case ECSPSearchTimeout: // Search timed out
            {
            resourceID = R_CHAT_ERROR_SEARCH_FAIL;
            ignoreAdditionalData = ETrue;
            break;
            }

        // flowthrough
        case ECSPTooManyHits: // Too many hits
        case ECSPTooBroadSearchCriteria: // Too broad search criteria
            {
            errorNote = EFalse;
            resourceID = R_CHAT_INOTE_TOO_MANY_HITS;
            ignoreAdditionalData = ETrue;
            break;
            }

        // flowthrough
        case KErrTimedOut: //Time out for requests
        case ECSPSessionExpired: // Session expired
        case ECSPForcedLogout: // Forced logout
            {
            IMDialogUtils::DisplayQueryDialogL( R_CHAT_CONERROR_NOTE );
            break;
            }

        case ECSPGroupDoesNotExist: // Group does not exist
            {
            resourceID = R_CHAT_ERROR_NO_GROUP;
            ignoreAdditionalData = ETrue;
            break;
            }

        case ECSPInvalidParameter + Imps_ERROR_BASE:    // Flowthrough
        case ECSPGroupDoesNotExist + Imps_ERROR_BASE:
            {
            // Manually entered group id was not found from server
            resourceID = R_CHAT_INOTE_CNOTFOUND;
            ignoreAdditionalData = ETrue;
            break;
            }

        // flowthrough
        case ECSPGroupNotJoined: // Group is not joined

            //start
        case ECSPCannotJoinRejected: // Rejected
            //end
        case ECSPNotAGroupMember: // Access removed
        case ECSPGroupIsRestricted: // Kicked out
            {
            errorNote = EFalse;
            resourceID = R_CHAT_INOTE_KICKED;
            ignoreAdditionalData = ETrue;
            break;
            }

        case ECSPScreennameInUse: // Screen name already in use
            {
            errorNote = EFalse;
            resourceID = R_CHAT_JOIN_SAME_N;
            ignoreAdditionalData = ETrue;
            break;
            }

        // flowthrough
        case ECSPMaxNumberOfGroupsForUser: // The maximum number of groups has been reached for the user
        case ECSPMaxNumberOfGroupsForServer: // The maximum number of groups has been reached for the server
            {
            errorNote = EFalse;
            resourceID = R_CHAT_INOTE_GROUP_FULL;
            ignoreAdditionalData = ETrue;
            break;
            }

        case ECSPInsufficientGroupPrivileges: // Insufficient group privileges
            {
            resourceID = R_CHAT_JOIN_NOTE_ERROR;
            ignoreAdditionalData = ETrue;
            break;
            }

        case ECSPMaxNumberOfJoinedUsers: // The maximum number of joined users has been reached
            {
            errorNote = EFalse;
            resourceID = R_CHAT_INOTE_CHAT_FULL;
            ignoreAdditionalData = ETrue;
            break;
            }

        // flowthrough
        case ECSPDomainNotSupported: // Domain not supported
        case ECSPRecipientNotLoggedIn: // Message recipient not logged in
            {
            errorNote = ETrue;
            resourceID = R_CHAT_ERROR_NOT_REACH;
            break;
            }

        case ECSPUnknownUserId: // Unknown user
            {
            errorNote = ETrue;
            resourceID = R_CHAT_ERROR_WRONG_WVID;
            if ( aAdditionalData == KNullDesC )
                {
                resourceID = R_CHAT_SERVERCONT_IDS_EXIST_SERVER;
                ignoreAdditionalData = ETrue;
                }
            break;
            }

        case ECSPUnknownUserId + Imps_ERROR_BASE:
            {
            // User has started conversation by entering user id
            // manually -> user not found from server
            errorNote = ETrue;
            resourceID = R_CHAT_INOTE_NAME_NOTFOUND;
            break;
            }

        case EServerContactListFull:    // contact list on the server is full
            {
            errorNote = ETrue;
            resourceID = R_QTN_CHAT_SERVER_FULL;
            break;
            }

        case ECorruptedContent:
            {
            confirmationNote = ETrue;
            resourceID = R_QTN_MMS_UPLOAD_INFO_CORRUPTED;
            ignoreAdditionalData = ETrue;
            break;
            }

        case EGeneralServerError:   // flowthrough
        default: // Unknown error -> show general note
            {
            defaultNote = ETrue;
            errorNote = ETrue;
            ignoreAdditionalData = ETrue;
            resourceID = R_CHAT_ERROR_GEN_ERROR;
            break;
            }
        }

    if ( resourceID != 0 )
        {
        if ( confirmationNote )
            {
            IMDialogUtils::DisplayQueryDialogL( resourceID );
            }
        else
            {
            HBufC* prompt = NULL;

            if ( aAdditionalData == KNullDesC || defaultNote || ignoreAdditionalData )
                {
                prompt = CEikonEnv::Static()->AllocReadResourceLC( resourceID );
                }
            else
                {
                prompt = StringLoader::LoadLC( resourceID, aAdditionalData );
                }

            if ( errorNote )
                {
                IMDialogUtils::DisplayErrorNoteL( *prompt, softkeyOk );
                }
            else
                {
                IMDialogUtils::DisplayNoteDialogL( *prompt );
                }

            CleanupStack::PopAndDestroy( prompt );
            }
        }
    }

//  End of File
