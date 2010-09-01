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
* Description:  AsyncOperationOwner interface.
*
*/

#ifndef MPENGASYNCOPERATIONOWNER_H
#define MPENGASYNCOPERATIONOWNER_H


// FORWARD DECLARATIONS
class CPEngAsyncOperation;


// CLASS DECLARATION

/**
 * Operation owner interface to receive operation
 * death etc. events.
 *
 * @since 3.0
 */
class MPEngAsyncOperationOwner
    {
    public: //Owner notifying methods


        /**
         * Async operation destruction.
         *
         * Called by concrete asynchronous operation from its destructor
         * to notify owner that the operation is dying.
         *
         * @since 3.0
         * @param aOperation The dying operation.
         */
        virtual void HandleAsyncOperationDestruction( CPEngAsyncOperation* aOperation ) = 0;



    protected:  //Destructor

        /**
         * Protected destructor.
         * Owner can't be destroyed via this interface.
         */
        virtual ~MPEngAsyncOperationOwner() { }

    };



#endif //MPENGASYNCOPERATIONOWNER_H


