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


#ifndef __CIMPSPROTOCOLPRESENCEPUBLISHING_H__
#define __CIMPSPROTOCOLPRESENCEPUBLISHING_H__

//  INCLUDES
#include <e32base.h>
#include <ximpcontext.h>
#include <protocolpresencepublishing.h>
#include <ximpprotocolconnection.h>
#include <ximpbase.h>

// FORWARD DECLARATIONS
class MXIMPServiceInfo;
class MImpsPrtPluginConnectionManager;
class CPresenceNotificationHandler;

class CImpsProtocolPresencePublishing:
            public CBase,
            public MProtocolPresencePublishing
    {

    public:

        static CImpsProtocolPresencePublishing* NewL( MImpsPrtPluginConnectionManager& aConnManager );


        ~CImpsProtocolPresencePublishing();


    private:

        CImpsProtocolPresencePublishing( MImpsPrtPluginConnectionManager& aConnManager );


        void ConstructL();

    public:
        // MXIMPBase Start

        TAny* GetInterface( TInt32 aInterfaceId,
                            TIfGetOps  aOptions );

        const TAny* GetInterface( TInt32 aInterfaceId,
                                  TIfGetOps  aOptions ) const;


        TInt32 GetInterfaceId() const;

        // From MProtocolPresencePublishing

        void DoPublishOwnPresenceL( const MPresenceInfo& aPresence,
                                    TXIMPRequestId aReqId );

        void DoSubscribeOwnPresenceL( const MPresenceInfoFilter& aPif,
                                      TXIMPRequestId aReqId );

        void DoUpdateOwnPresenceSubscriptionPifL( const MPresenceInfoFilter& aPif,
                                                  TXIMPRequestId aReqId );

        void DoUnsubscribeOwnPresenceL( TXIMPRequestId aReqId );


        void DoSubscribePresenceWatcherListL( TXIMPRequestId aReqId );


        void DoUnsubscribePresenceWatcherListL( TXIMPRequestId aReqId );

    public:

        void CreateDataHandlerL();

    private:

        /**
         * Request Id from PrFw
         */
        TXIMPRequestId iRequestId;

        /**
         * Imps Protocol Plugin Connection Manager
         * Not own.
         *** Write "Not own" if some other class owns this object.
         */
        MImpsPrtPluginConnectionManager& iConnMan;

        CPresenceNotificationHandler* iNotificationHandler;

    };

#endif // __CIMPSPROTOCOLPRESENCEPUBLISHING_H__
