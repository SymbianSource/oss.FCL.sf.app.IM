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
* Description:  Handle to NWSessionSlot.
*
*/

#ifndef __CPENGNWSESSIONSLOT2_H
#define __CPENGNWSESSIONSLOT2_H

//  INCLUDES
#include <E32Base.h>
#include <BaDesCa.h>
#include <PEngPresenceEngineConsts2.h>




//FORWARD DECLARATIONS
class MPEngNWSessionOperationObserver2;
class CPEngNWSessionSlot2Imp;
class CPEngNWSessionSlotID2;
class CWVSAPSettings;
class CIMPSSAPSettings;





/**
 * NWSessionSlot option level.
 * Presence transport query.
 * @since 3.0
 */
const TUint KPEngNWSessionSlotTransportQuery   = 0x2000;




// CLASS DECLARATION

/**
 * Handle to NWSessionSlot.
 *
 * NWSessionSlot ties together:
 * 1. Network presence session
 *    - network presence service is identified by user id
 *      and service address
 *    - NWSessionSlot holds also other SAP settings needed
 *      to connect to network service (IAP ID, user password, etc)
 *
 * 2. Network session presence data
 *    - NWSessionSlot is context for certain network
 *      session's presence data
 *    - Presence data consists from
 *          - contact list setings
 *          - contact list data
 *          - attribute list data
 *          - subscribed / fetched presence attributes
 *          - user own attributes
 *
 *
 * NWSessionSlot has a state and event information
 * describing its current state.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngNWSessionSlot2 : public CBase
    {

        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates NWSessionSlot object.
         *
         * Instantiates NWSessionSlot handle object and connects it to
         * identified Presence Engine side NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * Errors:
         *  - Requested NWSessionSlot not found: KErrNotFound
         *  - Given NWSessionSlotID malformed: KErrArgument
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for CPEngNWSessionSlot2 client
         * side active objects. These active objects are used when
         * asynchronously delivering events to observers.
         *
         * @return New CPEngNWSessionSlot2 instance.
         */
        IMPORT_C static CPEngNWSessionSlot2* NewL(
            const CPEngNWSessionSlotID2& aNwSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngNWSessionSlot2* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngNWSessionSlot2();



    private:

        CPEngNWSessionSlot2();




        //-----------------------------------------------------------------------
    public: /* NW Session Slot configuration */


        /**
         * Gets the NWSessionSlotID.
         *
         * @since 3.0
         * @param aNWSessionSlotID On the return contains the NWSessionSlotID
         * of connected NWSessionSlot.
         * @return Result from NWSessionSlotID get. One of a system
         * wide error codes.
         */
        IMPORT_C TInt GetNWSessionSlotID(
            CPEngNWSessionSlotID2& aNWSessionSlotID ) const;



        /**
         * Generic getter to access NWSessionSlot options.
         *
         * Generic getters to access NWSessionSlot options.
         * Used for example to get underlying network session
         * capabilities. See PEngWVServices2.h for more info about
         * WV network capabilities.
         *
         * @since 3.0
         * @param aOptionName Constant which identifies an option.
         * @param aOptionLevel Constant which identifies level of an option.
         * @param aOption On the return contains actual option value.
         * @return Result from option get. Might be a system
         * wide error code or error code specific to option.
         */
        IMPORT_C TInt GetOption( TUint aOptionName,
                                 TUint aOptionLevel,
                                 TDes8& aOption );



        //-----------------------------------------------------------------------
    public: /* Network session start and ownership gaining */


        /**
         * Tests is the NWPresenceSession open already active.
         *
         * @since 3.0
         * @return ETrue NWPresenceSession open is active.
         * Else EFalse.
         */
        IMPORT_C TBool IsOpenNWPresenceSessionActive() const;


        /**
         * Opens application network presence session.
         *
         * Asynchronously opens network presence session for
         * application and connects it to given presence network
         * service. Result from open is notified to given observer.
         *
         * - If underlying network CSP session is already open,
         *   joins the NWSessionSlot to existing CSP session if SAP
         *   settings match.
         * - At successful login the client CPEngNWSessionSlot2 instance
         *   becomes a NWPresenceSession ownership handle. If client
         *   dies or the handle is deleted, the opened NWPresenceSession
         *   is automaticly closed, if there isn't other registered
         *   session owners. Client can "store" the ownership to system
         *   if it doesn't want the automatic close behaviour.
         *   See StoreNWPresenceSessionOwnership().
         * - Performs network session capability query. Capabilities
         *   can be asked from NWSessionSlot after login.
         *
         * Possible errors:
         *   - KErrAlreadyExist - NWPresenceSession already open
         *   - Errors specific for real transport
         *   - System wide errors
         *
         * @since 3.0
         * @param aSAP The SAP settings to use for NWPresenceSession
         * Server address and user ID must match to NWSessionSlotID.
         * Overloaded version for CWVSAPSettings and CIMPSSAPSettings.
         *
         * @param aObserver The observer to notify from session open results.
         *
         * @return Result from initiating session open.
         *  - KErrNone if session open successfully initiated.
         *  - KErrInUse if session open already on going from object.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt OpenNWPresenceSession(
            const CWVSAPSettings& aSAP,
            MPEngNWSessionOperationObserver2& aObserver );

        IMPORT_C TInt OpenNWPresenceSession(
            const CIMPSSAPSettings& aSAP,
            MPEngNWSessionOperationObserver2& aObserver );

        IMPORT_C void CancelOpenNWPresenceSession();



        /**
         * Gains the network session ownership to this
         * CPEngNWSessionSlot2 handle object.
         *
         * With this method client can get active NWPresenceSession
         * ownership to additional handle object. Can be used
         * in cases where several applications want to control
         * NWPresenceSession lifetime with their own lifetime.
         *
         * @since 3.0
         * @return Result:
         * - KErrNone if ownership gaining successfully done.
         * - KErrNotSupported if NWPresenceSession not active.
         * - Else one of system wide error codes
         */
        IMPORT_C TInt OpenNWPresenceSessionOwnership();




        //-----------------------------------------------------------------------
    public: /* Network session stop and ownership closing */


        /**
         * Tests is the NWPresenceSession close already active.
         *
         * @since 3.0
         * @return ETrue NWPresenceSession close is active.
         * Else EFalse.
         */
        IMPORT_C TBool IsCloseNWPresenceSessionActive() const;


        /**
         * Closes certain application network presence session.
         *
         * Asynchronously closes network presence session.
         * Result from close is notified to given observer.
         *
         * - Active network operations from NWSessionSlot are terminated
         *   with KErrCancel.
         * - Canceling NW session close doesn't stop the actual network
         *   session close operation, but merely informs Presence Engine
         *   that the client isn't anymore interested from disconnect
         *   end status.
         *
         * Possible errors:
         *   - KErrNotFound - NWPresenceSession not open.
         *   - Errors specific for real transport
         *   - System wide errors
         *
         *
         * Two overloads:
         * - CloseNWPresenceSession()
         *   Closes the NWPresenceSession if there isn't any other
         *   reqistered session owners than the calling client.
         *   Note the client CPEngNWSessionSlot2 obejct must have
         *   the NWPresenceSession ownership before it can issue this.
         *
         * - ForceCloseNWPresenceSession()
         *   Closes the NWPresenceSession in all cases, event there is
         *   other reqistered session owners.
         *
         * @since 3.0
         * @param aObserver The observer to notify from session close results.
         *
         * @return Result from initiating session close.
         *  - KErrNone if session close successfully initiated.
         *  - KErrInUse if session close already on going from object.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt CloseNWPresenceSession(
            MPEngNWSessionOperationObserver2& aObserver );

        IMPORT_C TInt ForceCloseNWPresenceSession(
            MPEngNWSessionOperationObserver2& aObserver );

        IMPORT_C void CancelCloseNWPresenceSession();



        /**
         * Releases the network session ownership from this
         * CPEngNWSessionSlot2 handle object.
         *
         * With this method client can release immiadtely
         * active NWPresenceSession ownership. If the released
         * ownership is the last one, Presence Engine closes
         * down the NWPresenceSession like in case of holding
         * application dies.
         *
         * Similar behaviour takes place if the client
         * deletes CPEngNWSessionSlot2 object holding the
         * NWSessionOwnership.
         *
         * @since 3.0
         * @return Result:
         * - KErrNone if ownership successfully closed.
         * - KErrNotSupported if NWPresenceSession not active.
         * - In practise clients can ignore errors as the
         *   close requests itself succeeds always.
         */
        IMPORT_C void CloseNWPresenceSessionOwnership();




        //-----------------------------------------------------------------------
    public: /* NWPresenceSession ownership storing */

        /**
         * Stores the network session ownership from this
         * CPEngNWSessionSlot2 handle to Presence Engine.
         *
         * With this method client can store active NWPresenceSession
         * ownership from the client object to Presence Engine
         * and thus make the NWPresenceSession to stay open untill
         * it is explicitly closed with the ForceCloseNWPresenceSession().
         *
         * @since 3.0
         * @param aOwnerID The NWPresenceSession ownership ID.
         * Client can use this ID to later restore the ownership back from
         * Presence Engine to client CPEngNWSessionSlot2 object.
         *
         * @return Result:
         * - KErrNone if storing successfully done.
         * - KErrNotSupported if NWPresenceSession not active.
         * - KErrAlreadyExist if the ownership id exists already.
         * - Else one of system wide error codes
         */
        IMPORT_C TInt StoreNWPresenceSessionOwnership(
            const TDesC16& aOwnerID );




        /**
         * Restores the network session ownership from Presence Engine
         * to this CPEngNWSessionSlot2 handle object.
         *
         * With this method client restore active NWPresenceSession
         * ownership from Presence Engine to CPEngNWSessionSlot2
         * handle object and thus make the NWPresenceSession live
         * to follow the client live.
         *
         * @since 3.0
         * @param aOwnerID The NWPresenceSession ownership ID.
         * Which NWPresenceSession ownership to restore.
         *
         * @return Result:
         * - KErrNone if restoring successfully done.
         * - KErrNotSupported if NWPresenceSession not active.
         * - KErrNotFound if the ownership id not found.
         * - Else one of system wide error codes
         */
        IMPORT_C TInt RestoreNWPresenceSessionOwnership(
            const TDesC16& aOwnerID );






        //-----------------------------------------------------------------------
    public: /* NWSessionSlot current state */


        /**
         * Gets the network presence session's SAP settings.
         *
         * Gets the SAP settings as used in presence network
         * session open.
         *
         * @since 3.0
         * @param aSAP On the return contains network presence
         * session SAP settings. Resets the SAP container
         * before adding the settings.
         *
         * @return Result:
         * - KErrNone if settings successfully get.
         * - KErrNotFound if no active network presence session.
         * - Else one of system wide error codes
         */
        IMPORT_C TInt GetNWPresenceSessionSap( CWVSAPSettings& aSAP ) const;
        IMPORT_C TInt GetNWPresenceSessionSap( CIMPSSAPSettings& aSAP ) const;


        /**
         * Gets NWSessionSlots state.
         *
         * @since 3.0
         * @param aState Last state reported from NWSessionSlot.
         * @return  Result:
         *  - KErrNone if state successfully get.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt GetNWSessionSlotState(
            TPEngNWSessionSlotState& aState ) const;




        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngNWSessionSlot2Imp*     iImp;


    };


#endif //__CPENGNWSESSIONSLOT2_H

//End of file




