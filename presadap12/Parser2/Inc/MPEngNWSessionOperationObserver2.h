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
* Description:  Observer interface to listen NWSessionSlot operation events.
*
*/

#ifndef __MPENGNWSESSIONOPERATIONOBSERVER2_H
#define __MPENGNWSESSIONOPERATIONOBSERVER2_H

//  INCLUDES
#include <E32Std.h>

// FORWARD DECLARATIONS
class CPEngNWSessionSlot2;
class MPEngTransactionStatus2;




// CLASS DECLARATION
/**
 * Observer interface to receive NWSessionSlot
 * operation completion events.
 *
 * @since 3.0
 */
class MPEngNWSessionOperationObserver2
    {
    public: //Observer methods

        /**
         * Operation complete handler.
         *
         * Called by CPEngNWSessionSlot2 to handle NWSessionSlot
         * operation complete. If this method leaves, error code is
         * reported back to HandleNWSessionOperationNotifyError().
         *
         * @since 3.0
         * @param aStatus Transaction status container holding result from
         *        operation. Container ownership remains on the NWSessionSlot object.
         * @param aNWSessionSlot The NWSessionSlot object where notification
         *        is coming from. No ownership transferred.
         * @param aNWSessionSlotOperation ID of the completed NWSessionSlot
         *        operation. Operation IDs are listed in
         *        PEngPresenceEngineConsts2.h TPEngNWSessionSlotOperations
         *        enumeration. Client can use operation ID to figure
         *        out what operation completed.
         */
        virtual void HandleNWSessionOperationCompleteL(
            MPEngTransactionStatus2& aStatus,
            CPEngNWSessionSlot2& aNWSessionSlot,
            TInt aNWSessionSlotOperation ) = 0;


        /**
         * Failure handler.
         *
         * Called to handle leave errors from
         * HandleNWSessionOperationCompleteL().
         *
         * @since 3.0
         * @param aError The leave error from operation complete handling.
         * @param aNWSessionSlot The NWSessionSlot object which event
         *        handling failed.
         * @param aNWSessionSlotOperation ID of the completed NWSessionSlot
         *        operation. Operation IDs are listed in
         *        PEngPresenceEngineConsts2.h TPEngNWSessionSlotOperations
         *        enumeration. Client can use operation ID to figure
         *        out what operation completed.
         */
        virtual void HandleNWSessionOperationNotifyError(
            TInt aError,
            CPEngNWSessionSlot2& aNWSessionSlot,
            TInt aNWSessionSlotOperation ) = 0;



    private: //Extensions

        /**
         * Reserved virtual table slots for future use.
         * Do not implement these.
         *
         * @since 3.0
         */
        virtual TInt NWSessionOperationReserved() {
            return KErrNotSupported;
            }




    protected:  //Destructor

        /**
         * Protected destructor.
         * Observers can't be destroyed via this interface.
         */
        virtual ~MPEngNWSessionOperationObserver2() { }

    };



#endif //__MPENGNWSESSIONOPERATIONOBSERVER2_H



//End of file

