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

#ifndef CIMPSPROTOCOLPRESENTITYGROUPS_H__
#define CIMPSPROTOCOLPRESENTITYGROUPS_H__


#include <e32base.h>
#include <protocolpresentitygroups.h>


class MImpsPrtPluginConnectionManager;


/**
 * ?one_line_short_description
 *
 * ?more_complete_description
 *
 * @lib ?library
 * @since S60 v4.0
 */
class CImpsProtocolPresentityGroups :   public CBase,
            public MProtocolPresentityGroups
    {

    public:

        static CImpsProtocolPresentityGroups* NewL( MImpsPrtPluginConnectionManager& aConnManager );
        ~CImpsProtocolPresentityGroups();


    private:

        CImpsProtocolPresentityGroups( MImpsPrtPluginConnectionManager& aConnManager );
        void ConstructL();

    public:

        // MXIMPBase Start

        TAny* GetInterface( TInt32 aInterfaceId,
                            TIfGetOps  aOptions );

        const TAny* GetInterface( TInt32 aInterfaceId,
                                  TIfGetOps aOptions ) const;


        TInt32 GetInterfaceId() const;

        // MXIMPBase End
    public:

        void DoSubscribePresentityGroupListL(
            TXIMPRequestId aReqId );

        void DoUnsubscribePresentityGroupListL(
            TXIMPRequestId aReqId );

    public: // Manage presentity groups

        void DoCreatePresentityGroupL(
            const MXIMPIdentity& aGroupId,
            const TDesC16& aDisplayName,
            TXIMPRequestId aReqId );

        void DoDeletePresentityGroupL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );

        void DoUpdatePresentityGroupDisplayNameL(
            const MXIMPIdentity& aGroupId,
            const TDesC16& aDisplayName,
            TXIMPRequestId aReqId );

    public: // Subscribe presentity group content

        void DoSubscribePresentityGroupContentL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );
        void DoUnsubscribePresentityGroupContentL(
            const MXIMPIdentity& aGroupId,
            TXIMPRequestId aReqId );

    public: // Manage presentity group members

        void DoAddPresentityGroupMemberL(
            const MXIMPIdentity& aGroupId,
            const MXIMPIdentity& aMemberId,
            const TDesC16& aMemberDisplayName,
            TXIMPRequestId aReqId );

        void DoRemovePresentityGroupMemberL(
            const MXIMPIdentity& aGroupId,
            const MXIMPIdentity& aMemberId,
            TXIMPRequestId aReqId );

        void DoUpdatePresentityGroupMemberDisplayNameL(
            const MXIMPIdentity& aGroupId,
            const MXIMPIdentity& aMemberId,
            const TDesC16& aMemberDisplayName,
            TXIMPRequestId aReqId );

    public:

        void CreateDataHandlerL();

    private:    // data members

        MImpsPrtPluginConnectionManager& iConnMan;

    };

#endif // CIMPSPROTOCOLPRESENTITYGROUPS_H__
