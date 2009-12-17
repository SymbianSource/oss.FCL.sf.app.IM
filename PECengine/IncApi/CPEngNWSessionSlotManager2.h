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
* Description:  API to manage NWSessionSlots.
*
*/

#ifndef __CPENGNWSESSIONSLOTMANAGER2_H
#define __CPENGNWSESSIONSLOTMANAGER2_H

//  INCLUDES
#include <E32Base.h>
#include <PEngPresenceEngineConsts2.h>




// FORWARD DECLARATIONS
class CPEngNWSessionSlotManager2Imp;
class CPEngNWSessionSlotID2;


// CLASS DECLARATION
/**
 * CPEngNWSessionSlotManager2 to manage NWSessionSlots.
 *
 * Services:
 * - Create, remove and list NWSessionSlots
 * - Query certain NWSessionSlot's state
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngNWSessionSlotManager2 : public CBase
    {

        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates NWSessionSlotManager object.
         *
         * Instantiates NWSessionSlotManager object and connects it to
         * PresenceEngine subsystem. If needed PresenceEngine servers
         * aren't yet running, starts those before returning from method.
         *
         * @param aPriority The priority for Presence Engine client side active
         * objects. These active objects are used when asynchronously delivering
         * events to observers or maintaining Presence Cache state.
         *
         * @return New CPEngNWSessionSlotManager2 instance.
         */
        IMPORT_C static CPEngNWSessionSlotManager2* NewL(
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngNWSessionSlotManager2* NewLC(
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngNWSessionSlotManager2();


    private:

        CPEngNWSessionSlotManager2();



        //-----------------------------------------------------------------------
    public: /* NWSessionSlot management */


        /**
         * Creates a new NWSessionSlot to Presence Engine.
         *
         * Creates a new NWSessionSlot to Presence Engine.
         * After creation NWSessionSlot can be used:
         * - it can be connected to network server
         * - its data can be edited
         * - its data can be published and synchronized with network server
         * - data can be fetched from network server to local storage
         *
         * NWSessionSlots are identified with NWSessionSlotID.
         * One NWSessionSlot is tightly coupled to certain network service,
         * and in practice NWSessionSlot can be connected only to service
         * defined in its NWSessionSlotID.
         *
         * When a new NWSessionSlot is created, it is
         *  - in offline state
         *  - it has no session SAP settings defined
         *  - it has no contact lists defined nor any contact list data
         *  - it has no attribute lists defined
         *  - user own attributes are in their default state
         *  - and no subscribed presence attributes
         *
         * @param aNWSessionSlotID The session slot ID to use to
         * identify the new NWSessionSlot.
         *
         * @return Result:
         *  - KErrNone if creation succeeds.
         *  - KErrAlreadyExist if NWSessionSlot with given ID exists already
         *  - KErrArgument if NWSessionSlotID is malformed
         *  - Or one of the system wide error codes
         */
        IMPORT_C TInt CreateNWSessionSlot(
            const CPEngNWSessionSlotID2& aNWSessionSlotID );



        /**
         * Removes existing NWSessionSlot from Presence Engine.
         *
         * Removes existing NWSessionSlot from Presence Engine.
         * Remove fails if there is somewhere in system a open handle
         * to NWSessionSlot.
         * ==> - Client responsible for NW Session slot creation and
         *       removal must be prepared to do the garbage collect for
         *       NWSessionSlot's at suitable point. This could be done
         *       for example at boot up, when there isn't yet
         *       clients connected to NWSessionSlots.
         *     - All Presence Engine API objects which construction
         *       take NWSessionSlotID parameter (e.g. CPEngAttributeStore
         *       and CPEngAttributePublisher) have in practise a
         *       open handle to NWSessionSlot. Also some presence model
         *       objects (e.g MPEngContactList2 and MPEngPresenceAttrModel2)
         *       are having such handles also.
         *
         * @param aNWSessionSlotID The NWSessionSlot to remove.
         *
         * @return Result:
         *  - KErrNone if remove succeeds.
         *  - KErrNotFound if NWSessionSlot with given ID isn't found
         *  - KErrArgument if given NWSessionSlotID malformed
         *  - KErrInUse if NWSessionSlot is in use
         *  - Or one of the system wide error codes
         */
        IMPORT_C TInt RemoveNWSessionSlot(
            const CPEngNWSessionSlotID2& aNWSessionSlotID );





        //-----------------------------------------------------------------------
    public: /* NWSessionSlot information access */

        /**
         * Gets a list of all existing NWSessionSlots.
         *
         * @param aNWSessionSlotIDs On the return contains ID's of all existing
         *        NWSessionSlots. Appends the ID's in the end of array.
         *        Possibly existing objects in the array are not touched.
         *        Ownership of created objects is returned to caller. In case
         *        of error, returns ID's retrieved before the error.
         *
         * @return KErrNone or one of the system wide error codes.
         */
        IMPORT_C TInt GetNWSessionSlots(
            RPointerArray< CPEngNWSessionSlotID2 >& aNWSessionSlotIDs ) const;


        /**
         * Gets a list of existing NWSessionSlots.
         *
         * Overloaded version for getting NWSessionSlots which
         * NWSessionSlotID matches the given match pattern and
         * which state matches given state.
         *
         * @param aNWSessionSlotIDs On the return contains ID's of matching
         *        NWSessionSlots. Appends the ID's in the end of array.
         *        Possibly existing objects in the array are not touched.
         *        Ownership of created objects is returned to caller. In case
         *        of error, returns ID's retrieved before the error.
         *
         * @param aMatchPattern The match pattern to use
         *        to filter the returned NWSessionSlotIDs. Only those
         *        slots which ID match given match pattern, are returned.
         *
         * @param aMatchState The state match pattern to use to filter
         *        the returned NWSessionSlotIDs. Only those slots which
         *        state is same as given state are returned.
         *
         * @return KErrNone or one of the system wide error codes.
         */
        IMPORT_C TInt GetNWSessionSlots(
            RPointerArray< CPEngNWSessionSlotID2 >& aNWSessionSlotIDs,
            const CPEngNWSessionSlotID2& aMatchPattern,
            TPEngNWSessionSlotState aMatchState ) const;



        /**
         * Tests does the given NWSessionSlot exist.
         *
         * Tests does there exist a given NWSessionSlot.
         * NWSessionSlotId must be fully defined.
         *
         * @param aNWSessionSlotID NWSessionSlotID to check.
         * @return Result:
         *  - KErrNone if matching NWSessionSlot exist.
         *  - KErrNotFound if matching NWSessionSlot not found.
         *  - KErrArgument if given NWSessionSlotID malformed
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt ExistsNWSessionSlot(
            const CPEngNWSessionSlotID2& aNWSessionSlotID ) const;



        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngNWSessionSlotManager2Imp*     iImp;


    };


#endif //__CPENGNWSESSIONSLOTMANAGER2_H


//End of file








