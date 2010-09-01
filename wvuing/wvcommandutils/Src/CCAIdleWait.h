/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Helper class for idle wait
 *
*/



#ifndef CCAIDLEWAIT_H
#define CCAIDLEWAIT_H

// INCLUDES
#include    <e32base.h>

// FORWARD DECLARATIONS
class CIdle;

// CLASS DECLARATION

/**
 *  This class gives some helper functions
 *
 *  @lib chat.app
 *  @since 2.1
 */
class CCAIdleWait: public CBase
    {
    public: // New functions

        /**
         * This method halts the execution until
         * processor has some idle time
         * @CallbackFlag. Flag for callback. If
         *                  ETrue -> continue idle.
         *                  EFalse -> Stop idle.
         */
        static void IdleWaitL( const TInt* aCallbackFlag = NULL );

    private: // New methods

        /**
         * destructor
         */
        ~CCAIdleWait();

        /**
         * Constructor
         * @see IdleWaitL method for parameter.
         */
        CCAIdleWait( const TInt* aCallbackFlag );

        /**
         * Does the actual waiting
         */
        void DoIdleWaitL();

        /**
         * Does the actual stopping
         */
        void DoIdleStop();

        /**
         * CIdle callback function
         * @param aInstance Instance of this class
         * @return Need for recall
         */
        static TBool Callback( TAny* aInstance );


    private: // Data

        // Owns
        CIdle* iIdle;

        // active object wait
        CActiveSchedulerWait iWait;

        /// Callback flag for knowing when to stop idle.
        const TInt* iCallbackFlag;

    };

#endif      // CCAIDLEWAIT_H

// End of File
