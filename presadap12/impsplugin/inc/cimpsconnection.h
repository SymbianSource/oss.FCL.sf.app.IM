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


#ifndef __CIMPSCONNECTION_H__
#define __CIMPSCONNECTION_H__

//  INCLUDES
#include <e32base.h>
#include <ximpprotocolconnectionhost.h>
#include <ximpprotocolconnection.h>
#include "PImpsAdapWVCspVersion.h"


// FORWARD DECLARATIONS
class MXIMPServiceInfo;
class MXIMPContextClientInfo;
class MXIMPProtocolConnection;
class CImpsPluginConnectionManager;
class CImpsPluginLogoutRequest;
class CImpsPluginLoginRequest;
class CImpsProtocolPresenceFeatures;



/**
 * CImpsConnection
 *
 * Imps Protocol Connection Object. Protocol Connection Object has connection Manager.
 *
 * @lib ?library
 * @since S60 v4.0
 */
class CImpsConnection :
            public CBase,
            public MXIMPProtocolConnection
    {

    public:

        static CImpsConnection* NewL( const MXIMPServiceInfo& aServiceInfo,
                                      const MXIMPContextClientInfo& aClientCtxInfo );


        ~CImpsConnection();


    private:

        CImpsConnection();


        void ConstructL( const MXIMPServiceInfo& aServiceInfo,
                         const MXIMPContextClientInfo& aClientCtxInfo );


    public: // from MXIMPBase

        TAny* GetInterface( TInt32 aInterfaceId,
                            TIfGetOps  aOptions );

        const TAny* GetInterface( TInt32 aInterfaceId,
                                  TIfGetOps  aOptions ) const;


        TInt32 GetInterfaceId() const;


    public: // from CPrFwProtocolConnection

        // Common
        void PrimeHost( MXIMPProtocolConnectionHost& aHost );


        // session management
        void OpenSessionL( const MXIMPContextClientInfo& aClientCtxInfo,
                           TXIMPRequestId aOpId );

        void OpenSessionL( const TInt& aSettingsId,
                           TXIMPRequestId aReqId );

        void CloseSession( const MXIMPContextClientInfo& aClientCtxInfo,
                           TXIMPRequestId aOpId );

        void ReOpenSessionsL( TXIMPRequestId aReqId );

        void GetSupportedFeaturesL( CDesC8Array& aFeatures ) const;

        MProtocolPresenceFeatures& ProtocolPresenceFeatures();

        /**
         * @see MXIMPProtocolConnection
         */
        TAny* GetProtocolInterface( TInt aInterfaceId );

    private:    // data members

        CImpsPluginConnectionManager*	iConnMan;

        CImpsProtocolPresenceFeatures* iPresFeatures;

    };


#endif // CIMPSCONNECTION_H__
