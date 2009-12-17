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
* Description:  ImLauncher plugin implementation
*
*/



#ifndef CIMLAUNCHERIMPLEMENTATION_H
#define CIMLAUNCHERIMPLEMENTATION_H

//  INCLUDES
#include "MCAAudioManagerObserver.h"

#include <cimpspresenceconnectionuing.h>

#include <IMLauncherPlugin.h>
#include <bldvariant.hrh>

// CLASS DECLARATION
class CCAAudioManager;

/**
*  IM Application launcher implementation
*
*  @lib ImLauncher
*  @since 2.6
*/
class CImLauncherImplementation
            : public CImLauncherPlugin,
            public MCAAudioManagerObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CImLauncherImplementation* NewL();

        /**
        * Destructor.
        */
        ~CImLauncherImplementation();

    public: // From CImLauncherInterface

        /**
        * From CImLauncherPlugin
        * Method for starting an application which can receive instant messages.
        * Asynchronous function.
        * @since 2.6
        * @param aStatus Request status
        */
        void StartApplicationL( TRequestStatus& aStatus, const TDesC& aSap, const TDesC& aUserId );

        /**
        * From CImLauncherPlugin
        * Method for canceling the application start.
        * @since 2.6
        */
        void CancelStartApplication();

    private: // from class MCAAudioManagerObserver

        /**
         * @see MCAAudioManagerObserver
         */
        void PlayCompleted();

    private:

        /**
        * C++ default constructor.
        */
        CImLauncherImplementation();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // new functions

        /**
         * Sets Instante Message indicator visible.
         */
        void SetIMUIPIconL();

        /**
         * Shows IM message soft notification
         */
        void ShowSoftNotificationL();


        /**
         * Add one to unread message count (PS key)
         */
        void AddMessageCounterL();

    private: // data

        // Audio player for playing incoming messages, owned
        CCAAudioManager* iAudioManager;

        // Status variable to be return
        TRequestStatus* iStatus;

        // Pending requst err status
        TInt iRequestErr;

        // Owns. Connection UI
        CIMPSPresenceConnectionUi* iConnUI;
    };

#endif      // CIMLAUNCHERIMPLEMENTATION_H
// End of File
