/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class implements IMPS factory interface.
 *
*/


#ifndef CCAIMPSFACTORY_H
#define CCAIMPSFACTORY_H

//  INCLUDES
#include "MCAImpsFactory.h"
#include <e32base.h>
#include <impsclient.h>

// FORWARD DECLARATIONS
class CCAImpsGroupClient;
class CCAImpsFundClient;
class CCAImpsImClient;
class CCAImpsAccessClient;

// CLASS DECLARATION

/**
 *  Concrete factory. Creates concrete interface implementations when requested.
 *
 *
 *  @lib CAAdapter.lib
 *  @since 1.2
 */
class CCAImpsFactory : public CBase, public MCAImpsFactory
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCAImpsFactory* NewL();

        /**
         * Destructor.
         */
        virtual ~CCAImpsFactory();

    public: // New functions

        /**
         * Creates access client when requested.
         * @return MCAImpsAccessClient abstract access client class. Ownership NOT transferred.
         */
        MCAImpsAccessClient* CreateAccessClientL();

        /**
         * Creates concrete instant messaging client.
         * @return Abstract base class for instant messaging client.
         */
        MCAImpsImClient* CreateImClientL();

        /**
         * Creates concrete group client.
         * @return Abstract base class for group client.
         */
        MCAImpsGroupClient* CreateGroupClientL();

        /**
         * Creates concrete fundamental client.
         * @return Abstract base class for fundamental client.
         */
        MCAImpsFundClient* CreateFundClientL();

        /**
         * Signals all interfaces to register themselves to WV Engine.
         */
        virtual void RegisterL();

        /**
         * Signals all interfaces to unregister themselves from WV Engine.
         */
        virtual void Unregister();

        /**
         * Connects to IMPS engine (server)
         * @since 2.1
         */
        virtual void ConnectL();

    private:

        /**
         * C++ default constructor.
         */
        CCAImpsFactory();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:    // Data

        //Concrete access client, owns
        CCAImpsAccessClient *iAccessClient;

        //Concrete group client, owns
        CCAImpsGroupClient *iGroupClient;

        //Concrete fundamental client, owns
        CCAImpsFundClient *iFundClient;

        //Concrete instant messaging client, owns
        CCAImpsImClient *iImClient;

        //Concrete presence client, owns
        //CCAImpsPresenceClient* iPresenceClient

        // Imps engine server session
        RImpsEng iImpsEng;

        // Indicates if RImpsEng is connected or not.
        TBool iConnected;
    };

#endif      // CCAIMPSFACTORY_H

// End of File
