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
* Description:  NWSessionSlotEvent
*
*/

#ifndef __CPENGNWSESSIONSLOTEVENT2_H
#define __CPENGNWSESSIONSLOTEVENT2_H

//  INCLUDES
#include <E32Base.h>
#include <PEngPresenceEngineConsts2.h>


// FORWARD DECLARATIONS
class CPEngNWSessionSlotEvent2Copy;
class CPEngNWSessionSlotEventEntry;
class CPEngNWSessionSlotID2;



// CLASS DECLARATION
/**
 * NWSessionSlotEvent.
 *
 * Presents the NWSessionSlotEvent.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngNWSessionSlotEvent2 : public CBase
    {
        //-----------------------------------------------------------------------
    protected: /* Construction */
        CPEngNWSessionSlotEvent2();
        ~CPEngNWSessionSlotEvent2();



    public: // Cloning support

        /**
         * Creates a new deep copy from NWSessionSlotEvent object.
         *
         * Creates a deep copy from NWSessionSlotEvent object and
         * returns it ownership to client. Created object is placed
         * on the CleanupStack.
         *
         * Created copy is derived from CPEngNWSessionSlotEvent2Copy
         * interface to allow client to delete created object.
         *
         * @since 3.0
         * @return New copy from CPEngNWSessionSlotEvent2.
         * Ownership of created object is returned to calling client.
         */
        IMPORT_C CPEngNWSessionSlotEvent2Copy* CloneLC() const;



        //-----------------------------------------------------------------------
    public: /* NWSessionSlotEvent data */


        /**
         * NWSessionSlotID.
         * ID of changed NWSessionSlot.
         *
         * @since 3.0
         * @return NWSessionSlotID.
         */
        IMPORT_C const CPEngNWSessionSlotID2& NWSessionSlotID() const;


        /**
         * The event.
         *
         * @since 3.0
         * @return The event
         */
        IMPORT_C TPEngNWSessionSlotEvent Event() const;


        /**
         * The NWSessionSlot state after event.
         * This is the AppID specific state.
         * (Compare to CSPSessionState() )
         *
         * @since 3.0
         * @return NWSessionSlot state after event.
         */
        IMPORT_C TPEngNWSessionSlotState NWSessionSlotState() const;


        /**
         * The CSP session state after event.
         * This is global CSP session state.
         *
         * @since 3.0
         * @return CSP session state after event.
         */
        IMPORT_C TPEngNWSessionSlotState CSPSessionState() const;


        /**
         * Additional event integer.
         * Content depends from actual TPEngNWSessionSlotEvent.
         *
         * @since 3.0
         * @return Additional event integer.
         */
        IMPORT_C TInt EventInt() const;


        /**
         * Additional event descriptor.
         * Content meaning depends from actual TPEngNWSessionSlotEvent event
         * and EventInt.
         *
         * @since 3.0
         * @return Additional event descriptor.
         */
        IMPORT_C TPtrC EventDesc16() const;



        //-----------------------------------------------------------------------
    public: /* Non exported access to implementation */

        /**
        * Presence Engine internal access
        * to CPEngNWSessionSlotEvent2 implementation.
        *
        * @return Implementation.
        */
        CPEngNWSessionSlotEventEntry* Implementation();
        const CPEngNWSessionSlotEventEntry* Implementation() const;



        //-----------------------------------------------------------------------
    protected: /* Data */


        //OWN: Implementation
        CPEngNWSessionSlotEventEntry*    iImp;

    };




/**
 * This interface offers possibility to client to
 * control cloned CPEngNWSessionSlotEvent2 object
 * lifetime.
 *
 * @since 3.0
 */
class CPEngNWSessionSlotEvent2Copy : public CPEngNWSessionSlotEvent2
    {
    public: /* Construction */

        /**
         * Instantiates CPEngNWSessionSlotEvent2 object.
         *
         * @return New CPEngNWSessionSlotEvent2 instance.
         */
        static CPEngNWSessionSlotEvent2Copy* NewL();


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngNWSessionSlotEvent2Copy();



    protected: /* Construction */

        CPEngNWSessionSlotEvent2Copy();
    };




#endif      // __CPENGNWSESSIONSLOTEVENT2_H

// End of File


