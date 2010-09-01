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
* Description:  Resolves the network registration state and listens to the
*				 registration state changes
*
*/



#ifndef __CPENGAONWSTATUSOBSERVER_H
#define __CPENGAONWSTATUSOBSERVER_H

//  INCLUDES
#include "MIMPSSystemNotifierObserver.h"

#include <e32base.h>

// FORWARD DECLARATIONS
class MPEngAONwStatusObserver;
class MIMPSSystemNotifier;

// CLASS DECLARATION

/**
*  Network observer class for getting the current network status and getting
*  the events of network registration change.
*  @lib PEngAOPlugin.lib
*  @since 2.6
*/
class CPEngAONwStatusObserver : public CBase,
            public MIMPSSystemNotifierObserver
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPEngAONwStatusObserver* NewL( MPEngAONwStatusObserver& aObserver );

        /**
        * Destructor.
        */
        virtual ~CPEngAONwStatusObserver();

    public:		// new methods

        /**
         * Checks if network is available. If not then starts observe when
         * network status returns.
         * @return TBool ETrue if network is available
         */
        TBool IsNetworkAvailable();

        /**
         * Stops observering Network availability changes
         */
        void StopObserving();

    protected:  // From MIMPSSystemNotifierObserver

        /**
        * @see MIMPSSystemNotifierObserver
        */
        void HandleSystemNotifierEventL( TUid aKey, TInt aValue );

    private:

        /**
        * C++ default constructor.
        */
        CPEngAONwStatusObserver( MPEngAONwStatusObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /// system events observer
        MIMPSSystemNotifier* iSystemNotifier;

        /// A reference to the observing object
        MPEngAONwStatusObserver& iObserver;

        /// Reserved pointer for future extension
        TAny* iReserved;

    };

#endif      // __CPENGAONWSTATUSOBSERVER_H

// End of File
