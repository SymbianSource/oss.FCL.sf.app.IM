/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Abstract interface of the send data handler
*
*/



#ifndef __MPENGHANDLERSENDDATA_H__
#define __MPENGHANDLERSENDDATA_H__

//  INCLUDES

#include <e32def.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class CPEngOutGoingTransContainer;
class CPEngTransactionStatus;
// CLASS DECLARATION

/**
 *  Abstract interface of the send data handler
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
class MPEngHandlerSendData
    {
    public: // New functions

        /**
         *  Create new transaction containers and start their processing
         *
         *	Function goes over array of outgoing transaction handlers
         *  and if there is some handlers, their containers are created
         *	and started to process transactions
         *
         *  @since 3.0
         */
        virtual void CreateNewContainersRunThemL() = 0;

        /**
         *  Complete passed transaction container
         *
         *  Transaction container is deleted from the array of containers
         *	Its transaction result is tored(owner ship is transfered)
         *	If completion code of the container is different than KErrNone
         *	whole request processing is stoped and completed with such
         *	a error.
         *
         *  @since 3.0
         *  @param aContainer transaction container to be completed
         *  @param aStatus result of the transaction status, ownership
         *						is taken by this function
         *  @param aErrCode complete code of the container transaction
         */
        virtual void CompleteTransContainer(
            CPEngOutGoingTransContainer* aContainer,
            CPEngTransactionStatus* aStatus,
            TInt aErrCode ) = 0;

        /**
         *  Checks if this is last running transaction container
         *
         *  @since 3.0
         *  @return ETrue if calling transaction container is last one
         */
        virtual TBool LastRunningTransaction() = 0;


    public:  //Destructor

        /**
         * Virtual inline destructor.
         *
         * Concrete Request Handler can be
         * destroyed using this inteface.
         */
        virtual ~MPEngHandlerSendData() {};



    };

#endif      // __MPENGHANDLERSENDDATA_H__

// End of File
