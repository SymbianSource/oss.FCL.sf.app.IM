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
* Description:  Wireless Village CSP protocol error codes.
*
*/

#ifndef __IMPSPLUGINWVPRESENCEERRORS2_H
#define __IMPSPLUGINWVPRESENCEERRORS2_H


/**
 * Wireless Village CSP status code range.
 * Range is inclusive.
 *
 * @since 3.0
 */

#define KImpsPluginWVStatusCodeBase 100	// CSI: 8 #
#define KImpsPluginWVStatusCodeTop 999	// CSI: 8 #



/**
 * Wireless Village CSP protocol error codes.
 *
 * WV CSP status codes received from network transactions
 * are translated into Wireless Village CSP error code range
 * with following mapping:
 * <WV-CSP-error-code> = KImpsPluginErrorWVServerResponseBase - <WV-CSP-status-code>
 * WV CSP status code 200, succesfull is handled specially and
 * mapped to KErrNone.
 *
 *
 * NOTE!! Mapping algorithm above and error symbols defined
 * below are in sync with algorithm used by the WV Engine.
 * (See WV Engine error header: impserrors.h)
 * (With exeption of 200 - KErrNone)
 *
 * @since 3.0
 */
enum	// CSI: 28 #
    {
    KImpsPluginErrorWVServerResponseBase = -34000,


    //Successful
    KImpsPluginNwErrSuccessful = KImpsPluginErrorWVServerResponseBase - 200,
    KImpsPluginNwErrPartiallySuccessful = KImpsPluginErrorWVServerResponseBase - 201,
    KImpsPluginNwErrAccepted = KImpsPluginErrorWVServerResponseBase - 202,


    // Client errors
    KImpsPluginNwErrBadRequest = KImpsPluginErrorWVServerResponseBase - 400,
    KImpsPluginNwErrUnauthorized = KImpsPluginErrorWVServerResponseBase - 401,
    KImpsPluginNwErrBadParameter = KImpsPluginErrorWVServerResponseBase - 402,
    KImpsPluginNwErrForbidden = KImpsPluginErrorWVServerResponseBase - 403,
    KImpsPluginNwErrNotFound = KImpsPluginErrorWVServerResponseBase - 404,
    KImpsPluginNwErrServiceNotSupported = KImpsPluginErrorWVServerResponseBase - 405,
    KImpsPluginNwErrRequestTimeout = KImpsPluginErrorWVServerResponseBase - 408,
    KImpsPluginNwErrInvalidPassword = KImpsPluginErrorWVServerResponseBase - 409,
    KImpsPluginNwErrUnableToDeliver = KImpsPluginErrorWVServerResponseBase - 410,
    KImpsPluginNwErrUnsupportedMediaType = KImpsPluginErrorWVServerResponseBase - 415,
    KImpsPluginNwErrInvalidTransaction = KImpsPluginErrorWVServerResponseBase - 420,
    KImpsPluginNwErrUserIDAndClientIDDoNotMatch = KImpsPluginErrorWVServerResponseBase - 422,
    KImpsPluginNwErrInvalidInvitationID = KImpsPluginErrorWVServerResponseBase - 423,
    KImpsPluginNwErrInvalidSearchID = KImpsPluginErrorWVServerResponseBase - 424,
    KImpsPluginNwErrInvalidSearchIndex = KImpsPluginErrorWVServerResponseBase - 425,
    KImpsPluginNwErrInvalidMessageID = KImpsPluginErrorWVServerResponseBase - 426,
    KImpsPluginNwErrUnauthorizedGroupMembership = KImpsPluginErrorWVServerResponseBase - 431,
    KImpsPluginNwErrResponseTooLarge = KImpsPluginErrorWVServerResponseBase - 432,


    //Server errors
    KImpsPluginNwErrInternalServerOrNetworkError = KImpsPluginErrorWVServerResponseBase - 500,
    KImpsPluginNwErrNotImplemented = KImpsPluginErrorWVServerResponseBase - 501,
    KImpsPluginNwErrServiceUnavailable = KImpsPluginErrorWVServerResponseBase - 503,
    KImpsPluginNwErrTimeout = KImpsPluginErrorWVServerResponseBase - 504,
    KImpsPluginNwErrVersionNotSupported = KImpsPluginErrorWVServerResponseBase - 505,
    KImpsPluginNwErrServiceNotAgreed = KImpsPluginErrorWVServerResponseBase - 506,
    KImpsPluginNwErrMessageQueueIsFull = KImpsPluginErrorWVServerResponseBase - 507,
    KImpsPluginNwErrDomainNotSupported = KImpsPluginErrorWVServerResponseBase - 516,
    KImpsPluginNwErrUnrespondedPresenceRequest = KImpsPluginErrorWVServerResponseBase - 521,
    KImpsPluginNwErrUnrespondedGroupRequest = KImpsPluginErrorWVServerResponseBase - 522,
    KImpsPluginNwErrUnknownUser = KImpsPluginErrorWVServerResponseBase - 531,
    KImpsPluginNwErrMessageRecipientNotLoggedIn = KImpsPluginErrorWVServerResponseBase - 533,
    KImpsPluginNwErrMessageRecipientUnauthorized = KImpsPluginErrorWVServerResponseBase - 534,
    KImpsPluginNwErrSearchTimedOut = KImpsPluginErrorWVServerResponseBase - 535,
    KImpsPluginNwErrTooManyHits = KImpsPluginErrorWVServerResponseBase - 536,
    KImpsPluginNwErrTooBroadSearchCriteria = KImpsPluginErrorWVServerResponseBase - 537,
    KImpsPluginNwErrMessageHasBeenRejected = KImpsPluginErrorWVServerResponseBase - 538,
    KImpsPluginNwErrHeaderEncodingNotSupported = KImpsPluginErrorWVServerResponseBase - 540,


    //Session errors
    KImpsPluginNwErrSessionExpired = KImpsPluginErrorWVServerResponseBase - 600,
    KImpsPluginNwErrForcedLogout = KImpsPluginErrorWVServerResponseBase - 601,
    KImpsPluginNwErrAlreadyLoggedIn = KImpsPluginErrorWVServerResponseBase - 603,
    KImpsPluginNwErrInvalidSessionNotLoggedIn = KImpsPluginErrorWVServerResponseBase - 604,
    KImpsPluginNwErrNewValueNotAccepted = KImpsPluginErrorWVServerResponseBase - 605,


    //Presence and contact list errors
    KImpsPluginNwErrContactListDoesNotExist = KImpsPluginErrorWVServerResponseBase - 700,
    KImpsPluginNwErrContactListAlreadyExists = KImpsPluginErrorWVServerResponseBase - 701,
    KImpsPluginNwErrInvalidOrUnSupportedUserProperties = KImpsPluginErrorWVServerResponseBase - 702,
    KImpsPluginNwErrInvalidOrUnSupportedPresenceAttribute = KImpsPluginErrorWVServerResponseBase - 750,
    KImpsPluginNwErrInvalidOrUnSupportedPresenceValue = KImpsPluginErrorWVServerResponseBase - 751,
    KImpsPluginNwErrInvalidOrUnSupportedContactListProperty = KImpsPluginErrorWVServerResponseBase - 752,
    KImpsPluginNwErrMaxNumberOfContactListsReached = KImpsPluginErrorWVServerResponseBase - 753,
    KImpsPluginNwErrMaxNumberOfContactsReached = KImpsPluginErrorWVServerResponseBase - 754,
    KImpsPluginNwErrMaxNumberOfAttributeListsReached = KImpsPluginErrorWVServerResponseBase - 755,
    KImpsPluginNwErrAutomaticSubscriptionNotSupported = KImpsPluginErrorWVServerResponseBase - 760,


    //General errors
    KImpsPluginNwErrMultipleErrors = KImpsPluginErrorWVServerResponseBase - 900,
    KImpsPluginNwErrGeneralAddressError = KImpsPluginErrorWVServerResponseBase - 901,
    KImpsPluginNwErrNotEnoughCreditsForOperation = KImpsPluginErrorWVServerResponseBase - 902,
    KImpsPluginNwErrOperationRequiresHigherServiceClass = KImpsPluginErrorWVServerResponseBase - 903,


    KImpsPluginErrorWVServerResponseLow = -34999
    };

#endif // __ImpsPluginWVPRESENCEERRORS2_H

//  End of File


