/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMPS Protocol implementation for Presence Framework
*
*/


#ifndef __CIMPSPLUGIN_H__
#define __CIMPSPLUGIN_H__

//  INCLUDES
#include <e32base.h>
#include <ximpprotocolpluginbase.h>

// FORWARD DECLARATIONS
class MXIMPServiceInfo;
class MXIMPContextClientInfo;
class CImpsConnection;



/**
 * IMPS Protocol Plugin
 *
 * IMPS protocol plugin is ECOM plugin and shall be loaded by PrFw.
 * IMPS protocol has one or many connections.
 * @lib ?library
 * @since S60 v4.0
 */
class CImpsPlugin : public CXIMPProtocolPluginBase
    {
    public:

        static CImpsPlugin* NewL();
        static CImpsPlugin* NewLC();

        ~CImpsPlugin();

    private:

        CImpsPlugin();
        void ConstructL();

    public: // From MXIMPProtocolPlugin

        void PrimeHost( MXIMPProtocolPluginHost& aHost );

        MXIMPProtocolConnection& AcquireConnectionL(
            const MXIMPServiceInfo& aServiceInfo,
            const MXIMPContextClientInfo& aClientCtxInfo );
        void ReleaseConnection(
            MXIMPProtocolConnection& aConnection );

    public: // from MXIMPBase

        TAny* GetInterface(
            TInt32 aInterfaceId,
            TIfGetOps  aOptions );

        const TAny* GetInterface(
            TInt32 aInterfaceId,
            TIfGetOps  aOptions ) const;

        TInt32 GetInterfaceId() const;

    private: // Data

        // Prime host
        MXIMPProtocolPluginHost* iHost;

        RPointerArray< CImpsConnection > iConnections;
    };

#endif // __CIMPSPLUGIN_H__
