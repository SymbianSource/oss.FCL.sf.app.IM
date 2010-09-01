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



#ifndef __CPENGAOCALLOBSERVER_H
#define __CPENGAOCALLOBSERVER_H

//  INCLUDES
#include "MIMPSSystemNotifierObserver.h"

#include <e32base.h>
#include <e32std.h>

// FORWARD DECLARATIONS
class MPEngAOCallObserver;
class MIMPSSystemNotifier;

// CLASS DECLARATION

/**
*  Network observer class for getting the current network status and getting
*  the events of network registration change.
*  @lib PEngAOPlugin.lib
*  @since 2.1
*/
class CPEngAOCallObserver : public CBase,
            public MIMPSSystemNotifierObserver
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPEngAOCallObserver* NewL();

        /**
        * Destructor.
        */
        virtual ~CPEngAOCallObserver();

    public: // New functions

        /**
        * Checks if there's an active call currently. And if there is, calls
        * StartObserving to start the observing of the call and
        * sets the observer to which the notify about the call events is
        * given.
        * @param aObserver Call observer
        * @return TBool ETrue if there's an active call, else EFalse
        */
        TBool CallActive( MPEngAOCallObserver* aObserver );

        /**
         * Stops observing call state
         */
        void StopObserving();

        /**
        * Mark the testing class a friend, because this class is not "public" class
        */
        friend class T_CPEngAOCallObserver; // CSI: 36 # Only used for testing

    protected:  // From MIMPSSystemNotifierObserver

        /**
        * @see MIMPSSystemNotifierObserver
        */
        void HandleSystemNotifierEventL( TUid aKey, TInt aValue );

    private:

        /**
        * C++ default constructor.
        */
        CPEngAOCallObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // helpers

        /**
        * Calls HandleCallEndedL for all observers
        * @param TBool Whether the call ended already or not
        */
        void HandleCallEndedL( TBool aEnded );

    private:    // Data
        /// system events observer
        MIMPSSystemNotifier* iSystemNotifier;

        /// call observing object.
        MPEngAOCallObserver* iObserver;

        /// Reserved pointer for future extension
        TAny* iReserved;


    };

#endif      // __CPENGAOCALLOBSERVER_H

// End of File
