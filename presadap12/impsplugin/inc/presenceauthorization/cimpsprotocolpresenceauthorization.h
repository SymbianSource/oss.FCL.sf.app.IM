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


#ifndef __CIMPSPROTOCOLPRESENCEAUTHORIZATION_H__
#define __CIMPSPROTOCOLPRESENCEAUTHORIZATION_H__


//  INCLUDES
#include <e32base.h>
#include <ximpcontext.h>
#include <protocolpresenceauthorization.h>
#include <ximpprotocolconnection.h>


// for WV Engine
#include "ImpsAccesscli.h"
#include "ImpsImcli.h"
#include "impserrors.h"
#include "impsservices.h"


// FORWARD DECLARATIONS

class MXIMPServiceInfo;
class MImpsPrtPluginConnectionManager;


/**
 * CImpsProtocolPresenceAuthorization
 *
 * IMPS Protocol Request Authorization Request Handling
 *
 * @lib ?library
 * @since S60 v4.0
 */

class CImpsProtocolPresenceAuthorization:
            public CBase,
            public MProtocolPresenceAuthorization
    {
    public:

        static CImpsProtocolPresenceAuthorization* NewL( MImpsPrtPluginConnectionManager& aConnManager );


        ~CImpsProtocolPresenceAuthorization();


    private:

        CImpsProtocolPresenceAuthorization( MImpsPrtPluginConnectionManager& aConnManager );


        void ConstructL();

    public: // MXIMPBase Start

        TAny* GetInterface( TInt32 aInterfaceId,
                            TIfGetOps  aOptions );

        const TAny* GetInterface( TInt32 aInterfaceId,
                                  TIfGetOps  aOptions ) const;


        TInt32 GetInterfaceId() const;

        // MXIMPBase End

        void DoSubscribePresenceGrantRequestListL(
            TXIMPRequestId aReqId );

        void DoUnsubscribePresenceGrantRequestListL(
            TXIMPRequestId aReqId );

    public: // Granting presence information access to single presentities

        void DoGrantPresenceForPresentityL(
            const MXIMPIdentity& aPresentityId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        void DoUpdatePresenceGrantPifForPresentityL(
            const MXIMPIdentity& aPresentityId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        void DoWithdrawPresenceGrantFromPresentityL(
            const MXIMPIdentity& aPresentityId,
            TXIMPRequestId aReqId );

    public: // Granting presence information access to presentity group members

        void DoGrantPresenceForPresentityGroupMembersL(
            const MXIMPIdentity& aGroupId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        void DoUpdatePresenceGrantPifForPresentityGroupMembersL(
            const MXIMPIdentity& aGroupId,
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        void DoWithdrawPresenceGrantFromPresentityGroupMembersL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );

    public: // Granting presence information access to everyone

        void DoGrantPresenceForEveryoneL(
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        void DoUpdatePresenceGrantPifForEveryoneL(
            const MPresenceInfoFilter& aPif,
            TXIMPRequestId aReqId );

        void DoWithdrawPresenceGrantFromEveryoneL(
            TXIMPRequestId aReqId );
    public: // Subscribe presence block list

        void DoSubscribePresenceBlockListL(
            TXIMPRequestId aReqId );


        void DoUnsubscribePresenceBlockListL(
            TXIMPRequestId aReqId );

    public: // Blocking presence information access from single presentities

        void DoBlockPresenceForPresentityL(
            const MXIMPIdentity& aPresentityId,
            TXIMPRequestId aReqId );

        void DoCancelPresenceBlockFromPresentityL(
            const MXIMPIdentity& aPresentityId,
            TXIMPRequestId aReqId );


    public:

        void CreateDataHandlerL();

        void SendResponseForReactiveAuthRequestL( const MXIMPIdentity & aIdentity,
                                                  const MPresenceInfoFilter & aPif,
                                                  TXIMPRequestId aReqId );

    private:

        /**
         * Request Id from PrFw
         */
        TXIMPRequestId iRequestId;


        /**
         * Imps Protocol Plugin Connection Manager
         * Not own.  *** Write "Not own" if some other class owns this object.
         */
        MImpsPrtPluginConnectionManager& iConnMan;



    };

#endif // __CIMPSPROTOCOLPRESENCEAUTHORIZATION_H__












