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
* Description:  Abstract interface of IMPS factory.
 *
*/



#ifndef MCAIMPSFACTORY_H
#define MCAIMPSFACTORY_H

// FORWARD DECLARATIONS
class MCAImpsImClient;
class MCAImpsGroupClient;
class MCAImpsFundClient;
class MCAImpsAccessClient;
class TImpsCspIdentifier;

// CLASS DECLARATION

/**
 *  Abstract factory interface.
 *  Use this to create client interfaces.
 *  Factory implementation owns interface classes. Factory is deleted when DLL
 *  is unloaded. Factory implementation deletes interface classes in destructor.
 *
 *  @lib CAAdapter.lib
 *  @since 1.2
 */
class MCAImpsFactory
    {
    public: // New functions

        /**
         * Creates access client when requested.
         * @return MCAImpsAccessClient abstract access client class. Ownership NOT transferred.
         */
        virtual MCAImpsAccessClient* CreateAccessClientL() = 0;

        /**
         * Creates IM client when requested.
         * @return MCAImpsImClient abstract IM client class. Ownership NOT transferred.
         */
        virtual MCAImpsImClient* CreateImClientL() = 0;

        /**
         * Creates group client when requested.
         * @return MCAImpsGroupClient abstract group client class. Ownership NOT transferred.
         */
        virtual MCAImpsGroupClient* CreateGroupClientL() = 0;

        /**
         * Creates fundamental client when requested.
         * @return MCAImpsFundClient abstract fundamental client class. Ownership NOT transferred.
         */
        virtual MCAImpsFundClient* CreateFundClientL() = 0;

        /**
         * Registers all interfaces from WV Engine when requested.
         */
        virtual void RegisterL() = 0;

        /**
         * Unregisters all interfaces from WV Engine when requested.
         */
        virtual void Unregister() = 0;

        /**
         * Connects to IMPS engine (server)
         * @since 2.1
         */
        virtual void ConnectL() = 0;

    protected:  // New functions

        /**
         * Destructor.
         */
        virtual ~MCAImpsFactory() { };

    };

#endif      // MCAIMPSFACTORY_H

// End of File
