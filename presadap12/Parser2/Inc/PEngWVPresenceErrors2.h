/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef __PENGWVPRESENCEERRORS2_H
#define __PENGWVPRESENCEERRORS2_H


/**
 * Wireless Village CSP status code range.
 * Range is inclusive.
 *
 * @since 3.0
 */
#define KPEngWVStatusCodeBase 100	// CSI: 8 #
#define KPEngWVStatusCodeTop 999	// CSI: 8 #



/**
 * Wireless Village CSP protocol error codes.
 *
 * WV CSP status codes received from network transactions
 * are translated into Wireless Village CSP error code range
 * with following mapping:
 * <WV-CSP-error-code> = KPEngErrorWVServerResponseBase - <WV-CSP-status-code>
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
    KPEngErrorWVServerResponseBase = -34000,


    //Successful
    KPEngNwErrSuccessful = KPEngErrorWVServerResponseBase - 200,
    KPEngNwErrPartiallySuccessful = KPEngErrorWVServerResponseBase - 201,
    KPEngNwErrAccepted = KPEngErrorWVServerResponseBase - 202,


    // Client errors
    KPEngNwErrBadRequest = KPEngErrorWVServerResponseBase - 400,
    KPEngNwErrUnauthorized = KPEngErrorWVServerResponseBase - 401,
    KPEngNwErrBadParameter = KPEngErrorWVServerResponseBase - 402,
    KPEngNwErrForbidden = KPEngErrorWVServerResponseBase - 403,
    KPEngNwErrNotFound = KPEngErrorWVServerResponseBase - 404,
    KPEngNwErrServiceNotSupported = KPEngErrorWVServerResponseBase - 405,
    KPEngNwErrRequestTimeout = KPEngErrorWVServerResponseBase - 408,
    KPEngNwErrInvalidPassword = KPEngErrorWVServerResponseBase - 409,
    KPEngNwErrUnableToDeliver = KPEngErrorWVServerResponseBase - 410,
    KPEngNwErrUnsupportedMediaType = KPEngErrorWVServerResponseBase - 415,
    KPEngNwErrInvalidTransaction = KPEngErrorWVServerResponseBase - 420,
    KPEngNwErrUserIDAndClientIDDoNotMatch = KPEngErrorWVServerResponseBase - 422,
    KPEngNwErrInvalidInvitationID = KPEngErrorWVServerResponseBase - 423,
    KPEngNwErrInvalidSearchID = KPEngErrorWVServerResponseBase - 424,
    KPEngNwErrInvalidSearchIndex = KPEngErrorWVServerResponseBase - 425,
    KPEngNwErrInvalidMessageID = KPEngErrorWVServerResponseBase - 426,
    KPEngNwErrUnauthorizedGroupMembership = KPEngErrorWVServerResponseBase - 431,
    KPEngNwErrResponseTooLarge = KPEngErrorWVServerResponseBase - 432,


    //Server errors
    KPEngNwErrInternalServerOrNetworkError = KPEngErrorWVServerResponseBase - 500,
    KPEngNwErrNotImplemented = KPEngErrorWVServerResponseBase - 501,
    KPEngNwErrServiceUnavailable = KPEngErrorWVServerResponseBase - 503,
    KPEngNwErrTimeout = KPEngErrorWVServerResponseBase - 504,
    KPEngNwErrVersionNotSupported = KPEngErrorWVServerResponseBase - 505,
    KPEngNwErrServiceNotAgreed = KPEngErrorWVServerResponseBase - 506,
    KPEngNwErrMessageQueueIsFull = KPEngErrorWVServerResponseBase - 507,
    KPEngNwErrDomainNotSupported = KPEngErrorWVServerResponseBase - 516,
    KPEngNwErrUnrespondedPresenceRequest = KPEngErrorWVServerResponseBase - 521,
    KPEngNwErrUnrespondedGroupRequest = KPEngErrorWVServerResponseBase - 522,
    KPEngNwErrUnknownUser = KPEngErrorWVServerResponseBase - 531,
    KPEngNwErrMessageRecipientNotLoggedIn = KPEngErrorWVServerResponseBase - 533,
    KPEngNwErrMessageRecipientUnauthorized = KPEngErrorWVServerResponseBase - 534,
    KPEngNwErrSearchTimedOut = KPEngErrorWVServerResponseBase - 535,
    KPEngNwErrTooManyHits = KPEngErrorWVServerResponseBase - 536,
    KPEngNwErrTooBroadSearchCriteria = KPEngErrorWVServerResponseBase - 537,
    KPEngNwErrMessageHasBeenRejected = KPEngErrorWVServerResponseBase - 538,
    KPEngNwErrHeaderEncodingNotSupported = KPEngErrorWVServerResponseBase - 540,


    //Session errors
    KPEngNwErrSessionExpired = KPEngErrorWVServerResponseBase - 600,
    KPEngNwErrForcedLogout = KPEngErrorWVServerResponseBase - 601,
    KPEngNwErrAlreadyLoggedIn = KPEngErrorWVServerResponseBase - 603,
    KPEngNwErrInvalidSessionNotLoggedIn = KPEngErrorWVServerResponseBase - 604,
    KPEngNwErrNewValueNotAccepted = KPEngErrorWVServerResponseBase - 605,


    //Presence and contact list errors
    KPEngNwErrContactListDoesNotExist = KPEngErrorWVServerResponseBase - 700,
    KPEngNwErrContactListAlreadyExists = KPEngErrorWVServerResponseBase - 701,
    KPEngNwErrInvalidOrUnSupportedUserProperties = KPEngErrorWVServerResponseBase - 702,
    KPEngNwErrInvalidOrUnSupportedPresenceAttribute = KPEngErrorWVServerResponseBase - 750,
    KPEngNwErrInvalidOrUnSupportedPresenceValue = KPEngErrorWVServerResponseBase - 751,
    KPEngNwErrInvalidOrUnSupportedContactListProperty = KPEngErrorWVServerResponseBase - 752,
    KPEngNwErrMaxNumberOfContactListsReached = KPEngErrorWVServerResponseBase - 753,
    KPEngNwErrMaxNumberOfContactsReached = KPEngErrorWVServerResponseBase - 754,
    KPEngNwErrMaxNumberOfAttributeListsReached = KPEngErrorWVServerResponseBase - 755,
    KPEngNwErrAutomaticSubscriptionNotSupported = KPEngErrorWVServerResponseBase - 760,


    //General errors
    KPEngNwErrMultipleErrors = KPEngErrorWVServerResponseBase - 900,
    KPEngNwErrGeneralAddressError = KPEngErrorWVServerResponseBase - 901,
    KPEngNwErrNotEnoughCreditsForOperation = KPEngErrorWVServerResponseBase - 902,
    KPEngNwErrOperationRequiresHigherServiceClass = KPEngErrorWVServerResponseBase - 903,


    KPEngErrorWVServerResponseLow = -34999
    };

#endif // __PENGWVPRESENCEERRORS2_H

//  End of File


