/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  All CSP errors, symbolically
 *
*/


#ifndef _IMPSCSPALLERRORS_H
#define _IMPSCSPALLERRORS_H

// suck in error base and other errors too
#include <ImpsErrors.h>

enum
    {
    // Please refer to The Wireless Village initiative
    // Client-Server Protocol document (WV-022), e.g. WV_CSP_v1.1.pdf

    // 1xx - INFORMATIONAL
    ECSPContinue                    = Imps_ERROR_BASE - 100,
    ECSPQueued                      = Imps_ERROR_BASE - 101,
    ECSPStarted                     = Imps_ERROR_BASE - 102,

    // 2xx - SUCCESSFUL
    ECSPSuccessful                  = Imps_ERROR_BASE - 200,
    ECSPPartiallySuccessful         = Imps_ERROR_BASE - 201,
    ECSPAccepted                    = Imps_ERROR_BASE - 202,

    // 3xx are not listed in WV_CSP_v1.1

    // 4xx - CLIENT ERROR
    ECSPBadRequest                  = Imps_ERROR_BASE - 400,
    ECSPUnauthorized                = Imps_ERROR_BASE - 401,
    ECSPInvalidParameter            = Imps_ERROR_BASE - 402,
    ECSPAccountLost                 = Imps_ERROR_BASE - 403,
    ECSPNotFound                    = Imps_ERROR_BASE - 404,
    ECSPNotSupported                = Imps_ERROR_BASE - 405,
    ECSPRequestTimeout              = Imps_ERROR_BASE - 406,
    ECSPWrongPassword               = Imps_ERROR_BASE - 409,
    ECSPCannotDeliver               = Imps_ERROR_BASE - 410,
    ECSPUnsupportedMediaType        = Imps_ERROR_BASE - 415,
    ECSPInvalidTransactionOrId      = Imps_ERROR_BASE - 420,
    ECSPUserIdClientIdMismatch      = Imps_ERROR_BASE - 422,
    ECSPInvalidInvitationId         = Imps_ERROR_BASE - 423,
    ECSPInvalidSearchId             = Imps_ERROR_BASE - 424,
    ECSPInvalidSearchIndex          = Imps_ERROR_BASE - 425,
    ECSPInvalidMessageId            = Imps_ERROR_BASE - 426,
    ECSPUnauthorizedGroupMembership = Imps_ERROR_BASE - 431,

    // 5xx - SERVER ERROR
    ECSPInternalOrNetworkError      = Imps_ERROR_BASE - 500,
    ECSPNotImplemented              = Imps_ERROR_BASE - 501,
    ECSPServiceUnavailable          = Imps_ERROR_BASE - 503,
    ECSPTimeout                     = Imps_ERROR_BASE - 504,
    ECSPUnsupportedVersion          = Imps_ERROR_BASE - 505,
    ECSPServiceNotAgreed            = Imps_ERROR_BASE - 506,
    ECSPMessageQueueFull            = Imps_ERROR_BASE - 507,
    ECSPDomainNotSupported          = Imps_ERROR_BASE - 516,
    ECSPUnrespondedPresenceRequest  = Imps_ERROR_BASE - 521,
    ECSPUnrespondedGroupRequest     = Imps_ERROR_BASE - 522,
    ECSPUnknownUserId               = Imps_ERROR_BASE - 531,
    ECSPRecipientBlockedSender      = Imps_ERROR_BASE - 532,
    ECSPRecipientNotLoggedIn        = Imps_ERROR_BASE - 533,
    ECSPRecipientUnauthorized       = Imps_ERROR_BASE - 534,
    ECSPSearchTimeout               = Imps_ERROR_BASE - 535,
    ECSPTooManyHits                 = Imps_ERROR_BASE - 536,
    ECSPTooBroadSearchCriteria      = Imps_ERROR_BASE - 537,
    ECSPMessageIsRejected           = Imps_ERROR_BASE - 538,
    ECSPUnsupportedHeaderEncoding   = Imps_ERROR_BASE - 540,

    // 6xx - SESSION
    ECSPSessionExpired              = Imps_ERROR_BASE - 600,
    ECSPForcedLogout                = Imps_ERROR_BASE - 601,
    ECSPAlreadyLoggedIn             = Imps_ERROR_BASE - 603,
    ECSPNotLoggedIn                 = Imps_ERROR_BASE - 604,
    ECSPTimeoutValueNotAccepted     = Imps_ERROR_BASE - 605,
    ECSPInsufficientRights          = Imps_ERROR_BASE - 616, // not in spec

    // 7xx - PRESENCE AND CONTACT LIST
    ECSPContactListDoesNotExist     = Imps_ERROR_BASE - 700,
    ECSPContactListAlreadyExists    = Imps_ERROR_BASE - 701,
    ECSPInvalidUserProperties       = Imps_ERROR_BASE - 702,
    ECSPInvalidPresenceAttribute    = Imps_ERROR_BASE - 750,
    ECSPInvalidPresenceValue        = Imps_ERROR_BASE - 751,
    ECSPInvalidContactProperty      = Imps_ERROR_BASE - 752,
    ECSPContactListFull             = Imps_ERROR_BASE - 754, // from 1.2 spec

    // 8xx - GROUPS
    ECSPGroupDoesNotExist           = Imps_ERROR_BASE - 800,
    ECSPGroupAlreadyExists          = Imps_ERROR_BASE - 801,
    ECSPGroupIsOpen                 = Imps_ERROR_BASE - 802,
    ECSPGroupIsRestricted           = Imps_ERROR_BASE - 803,
    ECSPGroupIsPublic               = Imps_ERROR_BASE - 804,
    ECSPGroupIsPrivate              = Imps_ERROR_BASE - 805,
    ECSPInvalidGroupProperties      = Imps_ERROR_BASE - 806,
    ECSPGroupAlreadyJoined          = Imps_ERROR_BASE - 807,
    ECSPGroupNotJoined              = Imps_ERROR_BASE - 808,
    ECSPCannotJoinRejected          = Imps_ERROR_BASE - 809,
    ECSPNotAGroupMember             = Imps_ERROR_BASE - 810,
    ECSPScreennameInUse             = Imps_ERROR_BASE - 811,
    ECSPWhisperDisabledForGroup     = Imps_ERROR_BASE - 812,
    ECSPWhisperDisabledForUser      = Imps_ERROR_BASE - 813,
    ECSPMaxNumberOfGroupsForUser    = Imps_ERROR_BASE - 814,
    ECSPMaxNumberOfGroupsForServer  = Imps_ERROR_BASE - 815,
    ECSPInsufficientGroupPrivileges = Imps_ERROR_BASE - 816,
    ECSPMaxNumberOfJoinedUsers      = Imps_ERROR_BASE - 817,
    ECSPHistoryNotSupported         = Imps_ERROR_BASE - 821,
    ECSPGroupMissingNameOrTopic     = Imps_ERROR_BASE - 822,

    // 9xx - GENERAL ERRORS
    ECSPMultipleErrors              = Imps_ERROR_BASE - 900
    };

#endif // _IMPSCSPALLERRORS_H
