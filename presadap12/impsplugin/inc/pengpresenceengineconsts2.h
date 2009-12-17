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
* Description:  Presence Engine consts to be used by the clients.
*
*/

#ifndef __PENGPRESENCEENGINECONSTS2_H
#define __PENGPRESENCEENGINECONSTS2_H

//  INCLUDES
#include <E32Def.h>



// CONSTANTS

/**
 * Max length definitions for NWSessionSlotID
 * sub-parts and other related ID's.
 * @since 3.0
 */
_LIT( KPengWVIDPrefix, "wv:" );
const TInt KPEngAttrWVIdPrefixLength = 3;

const TInt KWVRequestResponseResultCode = 200;
const TInt KPEngMaxServiceAddressLength = 256;
const TInt KPEngMaxUserIdLength = 50;
const TInt KPEngMaxAppIdLength = 10;
const TInt KPEngMaxNWSessionOwnerID = 10;



/**
 * Unknown search element.
 * @since 3.0
 */
const TInt KPEngNullSearchElement = -1;



/**
 * NULL attribute type.
 *
 * @since 3.0
 */
const TUint32 KPEngNullAttributeType = 0x0;




/**
 * User own presence ID.
 *
 * Following lit is used to identify user own
 * presence attributes, e.g. when requesting
 * attribute notifications.
 *
 * @since 3.0
 */
_LIT( KPEngUserOwnPresenceId, "" );


/**
 * NWSessionSlot Application ID: IM
 *
 * This is used with CPEngNWSessionSlotID2 to
 * identify that IM application is using the
 * NWSessionSlot.
 *
 * @since 3.0
 */
_LIT( KPEngAppIdIM, "IM" );


/**
 * NWSessionSlot Application ID: PEC
 *
 * This is used with CPEngNWSessionSlotID2 to
 * identify that PEC applications are using the
 * NWSessionSlot.
 *
 * @since 3.0
 */
_LIT( KPEngAppIdPEC, "PEC" );






// EVENT AND OPERATION ENUMERATIONS


/**
 * NWSessionSlot event enumeration.
 *
 * @since 3.0
 */
enum TPEngNWSessionSlotEvent
    {
    //Pseudo empty event
    EPEngEventNone = 0,

    //New slot created
    EPEngEventNWSessionSlotCreated = 1,

    //Slot configured
    EPEngEventNWSessionSlotChanged = 2,

    //Slot removed
    EPEngEventNWSessionSlotRemoved = 3,

    //Network Presence Session opened from the slot
    EPEngEventAppNWPresenceSessionOpened = 4,

    //Network Presence Session closed from the slot
    EPEngEventAppNWPresenceSessionClosed = 5,

    //Network Server closed session
    EPEngEventNWSessionClosedByServer = 6,

    //Network Presence Session transport signalled event
    //EventInt and EventDes describe further the actual event
    EPEngEventNWSessionTransport = 7
    };





/**
 * NWSessionSlot state enumeration.
 *
 * @since 3.0
 */
enum TPEngNWSessionSlotState
    {
    // No network presence session open from NWSessionSlot.
    EPEngNWPresenceSessionClosed = 1,

    // Application network presence session open from NWSessionSlot.
    EPEngNWPresenceSessionOpen = 2
    };







/**
 * NWSessionSlot operation ID's.
 *
 * These NWSessionSlot operation IDs are visible for clients in:
 * 1) Transaction result entries, see
 *    - MPEngTransactionStatus2
 *
 * 2) Operation event callbacks, see
 *    - MPEngNWSessionOperationObserver2
 *
 * Internally Presence Engine uses these ID's
 * to manage NWSessionSlot operations.
 *
 * @since 3.0
 */
enum TPEngNWSessionSlotOperations
    {
    //Presence network session opening operation
    EPEngNWSSOpNWSessionOpen = 1,


    //Presence network session closing operation
    EPEngNWSSOpNWSessionClose = 2
    };







/**
 * Transaction operation ID's.
 *
 * These transaction operation IDs are visible for clients in:
 * 1) Transaction result entries, see
 *    - MPEngTransactionStatus2
 *
 * 2) Operation event callbacks, see
 *    - MPEngContactListTransactionObserver2
 *    - MPEngAttributeListTransactionObserver2
 *    - MPEngAttributeTransactionObserver2
 *
 * Internally Presence Engine uses these ID's
 * to manage transaction operations.
 *
 * @since 3.0
 */
enum TPEngTransactionOperation
    {
    //Publish own presence attributes
    EPEngTransOpOwnAttributePublish = 51,

    //Fetch Presence Attributes to cache
    EPEngTransOpAttributeFetchToCache = 52,

    //Fetch Presence Attributes to objects
    EPEngTransOpAttributeFetchToObjects = 53,



    // Synchronize Attribute base things, default attribute list
    EPEngTransOpAttrListBaseSynchronize = 101,

    // Sychronize attribut elist for contact list
    EPEngTransOpAttrListSyncForCntList = 102,

    //Publish Attribute lists
    EPEngTransOpAttrListPublish = 103,


    //Synchronize Contact List base things (list of lists)
    EPEngTransOpCntListBaseSync = 150,

    //Update contact list
    EPEngTransOpCntListUpdateList = 151,

    //Synchronize contact list
    EPEngTransOpCntListSyncList = 152,

    //Network search start
    EPEngTransOpNetworkSearchStart = 200,

    //Network continue search
    EPEngTransOpNetworkSearchContinue = 201,

    //Network search stop
    EPEngTransOpNetworkSearchStop = 202,

    // Respond to Autorization Requests
    EPEngTransOpAuthorizationResponse = 250
    };





#endif //__PENGPRESENCEENGINECONSTS2_H


