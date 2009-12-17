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
* Description:  Notifier API to listen NWSessionSlot changes.
*
*/

#ifndef __CPENGNWSESSIONSLOTNOTIFIER2_H
#define __CPENGNWSESSIONSLOTNOTIFIER2_H

//  INCLUDES
#include <E32Base.h>
#include <PEngPresenceEngineConsts2.h>



// FORWARD DECLARATIONS
class MPEngNWSessionSlotObserver2;
class CPEngNWSessionSlotNotifier2Imp;
class CPEngNWSessionSlotID2;




// CLASS DECLARATION
/**
 * Notifier to receive notifications from
 * NWSessionSlot events and state changes.
 *
 * Notifies from:
 *  - creation / removal of NWSessionSlots
 *  - individual NWSessionSlots events
 *
 * For full list of notified events,
 * see TPEngNWSessionSlotEvent enumeration.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngNWSessionSlotNotifier2 : public CBase
    {

        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates NWSessionSlotNotifier object.
         *
         * Instantiates NWSessionSlotNotifier object and connects it to
         * PresenceEngine subsystem. If needed PresenceEngine servers
         * aren't yet running, starts those before returning.
         *
         * @param aPriority The priority for
         * CPEngNWSessionSlotNotifier2 client side active objects.
         * These active objects are used asynhronously deliver
         * events to observers.
         *
         * @return New CPEngNWSessionSlotNotifier2 instance.
         */
        IMPORT_C static CPEngNWSessionSlotNotifier2* NewL(
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngNWSessionSlotNotifier2* NewLC(
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         *
         * Destruction
         *  - stops automaticly event notification
         *  - unregisters observers
         */
        ~CPEngNWSessionSlotNotifier2();


    private:

        CPEngNWSessionSlotNotifier2();



        //-----------------------------------------------------------------------
    public: /* NWSessionSlot notifications */


        /**
         * Starts delivering NWSessionSlot notifications.
         *
         * Reports either notifications from all NWSessionSlot
         * events or only those events which match given criterias.
         *
         * Possible criterias are:
         *  - NWSessionSlotID:
         *    Reports only those events which NWSessionSlotID
         *    matches given criteria. Match pattern can be
         *    full NWSessionSlotID or wild one.
         *
         *  - SlotEvent:
         *    Reports only those events which are requested.
         *
         * @param aMatchPattern The NWSessionSlotID match pattern to use.
         * @param aEvents The events to deliver
         * @return Result:
         *  - KErrNone if succesfully started
         *  - KErrArgument if empty criterias provided
         *  - KErrInUse if notifier already started
         *  - Else one of system wide error codes
         */
        IMPORT_C TInt Start();
        IMPORT_C TInt Start( const CPEngNWSessionSlotID2& aMatchPattern );
        IMPORT_C TInt Start( const TArray<TPEngNWSessionSlotEvent>& aEvents );


        /**
         * Stops listening sessions slot changes.
         */
        IMPORT_C void Stop();



        /**
         * Checks is the slot notifier started or not.
         *
         * @return ETrue if the slot notifier is already started.
         *         EFalse if the slot notifier isn't started.
         */
        IMPORT_C TBool IsActive() const;




        //-----------------------------------------------------------------------
    public: /* NWSessionSlot notifications */


        /**
         * Notifies synchronously registered observers
         * from NWSessionSlot states.
         *
         * State notifications are delivered synchronously from all
         * NWSessionSlots which NWSessionSlotID matches given criteria.
         * Match string can be full NWSessionSlotID or wild one.
         * If no matching NWSessionSlot found, returns KErrNotFound.
         *
         * Only NWSessionSlot state information is delivered.
         * Dynamic event and data parts are delivered as empty.
         *
         * @param aMatchPattern The NWSessionSlotID match pattern to use.
         * @return Result:
         *  - KErrNone if succesfully notified observers
         *  - KErrNotFound if no matching NWSessionSlot found
         *  - Else one of system wide error codes
         */
        IMPORT_C TInt PushNWSessionSlotStateToObservers(
            const CPEngNWSessionSlotID2& aMatchPattern );



        /**
         * Gets NWSessionSlots state.
         *
         * @since 3.0
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aState Last state reported from NWSessionSlot.
         * @return  Result:
         *  - KErrNone if state succesfully get.
         *  - Else one of system wide error codes.
         */
        IMPORT_C TInt GetNWSessionSlotState(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TPEngNWSessionSlotState& aState ) const;






        //-----------------------------------------------------------------------
    public: /* NWSessionSlot event observers */


        /**
         * Registers event observer to be notified from slot changes.
         *
         * Registered event observers are notified from
         * NWSessionSlot events as requested with StartL().
         *
         * @param aObserver The observer to be notified.
         * @return KErrNone is observer added succesfully.
         *         Else one of the system wide error codes.
         */
        IMPORT_C TInt AddObserver(
            MPEngNWSessionSlotObserver2& aObserver );



        /**
         * Unregisters session slot event observer.
         *
         * @param aObserver The observer to remove.
         * @return KErrNone is observer removed succesfully.
         *         KErrNotFound if the observer wasn't registered.
         */
        IMPORT_C TInt RemoveObserver(
            MPEngNWSessionSlotObserver2& aObserver );



        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngNWSessionSlotNotifier2Imp*     iImp;
    };



#endif //__CPENGNWSESSIONSLOTNOTIFIER2_H

//End of file




