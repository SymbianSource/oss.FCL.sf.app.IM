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
* Description:  NWSessionOperationOwner interface.
*
*/

#ifndef MPENGNWSESSIONOPERATIONOWNER_H
#define MPENGNWSESSIONOPERATIONOWNER_H


// FORWARD DECLARATIONS
class CPEngNWSessionOperation;


// CLASS DECLARATION

/**
 * Operation owner interface to receive operation
 * death etc. events.
 *
 * @since 3.0
 */
class MPEngNWSessionOperationOwner
    {
    public: //Owner notifying methods


        /**
         * NWSession operation destruction.
         *
         * Called by concrete NWSessionOperation from its destructor
         * to notify owner that the operation is dying.
         *
         * @since 3.0
         * @param aOperation The dying operation.
         */
        virtual void HandleNWSessionOperationDestruction( CPEngNWSessionOperation* aOperation ) = 0;


    protected:  //Destructor

        /**
         * Protected destructor.
         * Owner can't be destroyed via this interface.
         */
        virtual ~MPEngNWSessionOperationOwner() { }

    };



#endif //MPENGNWSESSIONOPERATIONOWNER_H


