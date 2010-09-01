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


#ifndef __CIMPSPROTOCOLPRESENCEWATCHING_H__
#define __CIMPSPROTOCOLPRESENCEWATCHING_H__


#include <e32base.h>
#include <protocolpresencewatching.h>



class MXIMPServiceInfo;
class MImpsPrtPluginConnectionManager;


/**
 * CImpsProtocolPresenceWatching
 *
 * Issue Imps Protocol Presence Watching Requests to Network Server
 *
 * @lib ?library
 * @since S60 v4.0
 */
class CImpsProtocolPresenceWatching :   public CBase,
            public MProtocolPresenceWatching
    {
    public:

        static CImpsProtocolPresenceWatching* NewL( MImpsPrtPluginConnectionManager& aConnManager );
        ~CImpsProtocolPresenceWatching();


    private:

        CImpsProtocolPresenceWatching( MImpsPrtPluginConnectionManager& aConnManager );
        void ConstructL();


    public:

        TAny* GetInterface( TInt32 aInterfaceId,
                            TIfGetOps  aOptions );

        const TAny* GetInterface( TInt32 aInterfaceId,
                                  TIfGetOps  aOptions ) const;
        TInt32 GetInterfaceId() const;


    public:

        void DoSubscribePresentityPresenceL(
            const MXIMPIdentity& aPresentityId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        void DoUpdatePresentityPresenceSubscriptionPifL(
            const MXIMPIdentity& aPresentityId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );


        void DoUnsubscribePresentityPresenceL(
            const MXIMPIdentity& aPresentityId,
            TXIMPRequestId aReqId );


    public: // Subscribe presence information from presentity group members



        void DoSubscribePresentityGroupMembersPresenceL(
            const MXIMPIdentity& aGroupId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        void DoUpdatePresentityGroupMembersPresenceSubscriptionPifL(
            const MXIMPIdentity& aGroupId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        void DoUnsubscribePresentityGroupMembersPresenceL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );


    private: // data members

        /**
         * Request Id from PrFw
         */

        /**
         * Imps Protocol Plugin Connection Manager
         * Not own.  *** Write "Not own" if some other class owns this object.
         */
        MImpsPrtPluginConnectionManager& iConnMan;

    };

#endif // CIMPSPROTOCOLPRESENCEWATCHING_H__
