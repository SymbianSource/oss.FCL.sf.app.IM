/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles incoming invitations
*
*/


#ifndef CCAINVITEMANAGER_H
#define CCAINVITEMANAGER_H

// INCLUDES
#include "MCAInvite.h"
#include "MCAImpsFundClient.h"
#include "MCAInviteTrackerCallback.h"

// FORWARD DECLARATIONS
class MCAInviteObserver;
class CCAInvitationRequest;
class MCASettings;
class CCARequestMapper;
class CCAInviteTracker;
class MCAStoredGroup;
class MCAGroupManagerInterface;
class MCAImpsFactory;

// CLASS DECLARATION

/**
 *  For invite-handling.
 *
 *
 *  @lib CAEngine.lib
 *  @since 1.2
 */
class CCAInviteManager : public CBase,
            public MImpsInviteHandler2,
            public MCAInvite,
            public MCAInviteTrackerCallback,
            public MCAInviteReadObserver
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aImpsFundAPI is the API to the invite services.
         * @param aSettingsAPI is the AI to the settings side.
         * @param aRequestMapper is the API to the request mapper.
         *        Used to provide synchronous waiting.
         * @param aGroupManager is the API to the group manager.
         */
        static CCAInviteManager* NewL(
            MCAImpsFactory* aIMPSFactory,
            MCASettings&      aSettingsAPI,
            CCARequestMapper& aRequestMapper,
            MCAGroupManagerInterface& aGroupManager );

        /**
         * Destructor.
         */
        virtual ~CCAInviteManager();

    public: // From MCAInviteTrackerCallback

        /**
         * From MCAInviteTrackerCallBack
         * @see MCAInviteTrackerCallBack::InviteExpired
         */
        void InviteExpiredL( const MCAInvitation* aInvitation );

    public: // From MCAInvite

        /**
         * From MCAInvite.
         * @see MCAInvite::AddInviteObserver.
         */
        TInt AddInviteObserver( MCAInviteObserver* aObserver );

        /**
         * From MCAInvite.
         * @see MCAInvite::RemoveInviteObserver.
         */
        void RemoveInviteObserver( MCAInviteObserver* aObserver );

        /**
         * From MCAInvite.
         * @see MCAInvite::SendInvitationL.
         */
        void SendInvitationL( const CCAInvitationRequest& aInvitation );

        /**
         * From MCAInvite
         * @see MCAInvite::PopulateInviteList.
         */
        void PopulateInviteList( RPointerArray<MCAInvitation>& aList );

        /**
         * From MCAInvite
         * @see MCAInvite::ActiveInvitations.
         */
        TInt ActiveInvitations();

        /**
         * From MCAInvite
         * @see MCAInvite::RemoveInvitation
         */
        void RemoveInvitationL( MCAInvitation* aInvitation );

        /**
         * From MCAInvite
         * @see MCAInvite::RejectReasonPtr
         */
        HBufC* RejectReasonPtr();

        /**
         * @see MCAInvite
         */
        TInt UnreadInvitesCount() const;

        /**
         * @see MCAInvite
         */
        void ReplyInvitationL( const MCAInvitation* aInvitation,
                               const TBool aInviteAccepted,
                               const TDesC& aResponse,
                               const TDesC& aScreenName );


    private: // From MImpsInviteHandler

        /**
         * @see MImpsInviteHandler::GroupInviteUserRequest.
         */
        void HandleGroupInviteL( const TDesC& aInviteID,
                                 const TDesC& aUserID,
                                 const TDesC& aScreenName,
                                 const TDesC& aGroupName,
                                 const TDesC& aInviteReason,
                                 const TInt aValidityPeriod,
                                 TImpsCspIdentifier& aCspId );

        /**
         * @see MImpsInviteHandler::GroupInviteUserRequest.
         */
        void HandleImInviteL( const TDesC& aInviteID,
                              const TDesC& aUserID,
                              const TDesC& aInviteReason,
                              const TInt aValidityPeriod,
                              TImpsCspIdentifier& aCspId );

        /**
         * @see MImpsInviteHandler::GroupInviteUserRequest.
         */
        void HandleContentInviteL( const TDesC& aInviteID,
                                   const TDesC& aUserID,
                                   MDesCArray* aUrlList,
                                   const TDesC& aInviteReason,
                                   const TInt aValidityPeriod,
                                   TImpsCspIdentifier& aCspId );

        /**
         * @see MImpsInviteHandler::GroupInviteUserRequest.
         */
        void HandleInviteResponseL( const TDesC& aInviteID,
                                    const TBool aAcceptance,
                                    const TDesC& aUserID,
                                    const TDesC& aScreenName,
                                    const TDesC& aGroupName,
                                    const TDesC& aResponse,
                                    TImpsCspIdentifier& aCspId );

        /**
         * @see MImpsInviteHandler::GroupInviteUserRequest.
         */
        void HandleInviteCancelL( const TDesC& aInviteID,
                                  const TDesC& aUserID,
                                  const TDesC& aScreenName,
                                  const TDesC& aGroupName,
                                  const TDesC& aResponse,
                                  TImpsCspIdentifier& aCspId );

        /**
         * @see MImpsInviteHandler::GroupInviteUserRequest.
         */
        void  HandleCompleteL( TInt aOperationId,
                               TImpsCspIdentifier& aCspId );

    private:

        /**
         * @see MCAInviteReadObserver
         */
        void HandleInviteReadL() const;

    private:

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

        /**
         * C++ default constructor.
         * @see CCAInviteManager::NewL.
         */
        CCAInviteManager(
            MCAImpsFactory* aIMPSFactory,
            MCASettings&      aSettingsAPI,
            CCARequestMapper& aRequestMapper,
            MCAGroupManagerInterface& aGroupManager );

        /**
         * Prepares target group for wanted invitee.
         * @since 1.2
         * @param aInvitation Invitation
         */
        void PrepareGroupForInviteeL( const CCAInvitationRequest& aInvitation );

        /**
         * Removes invite from invite queue.
         * @param aInviteId Invite to be removed
         * @return ETrue if invite was found from queue
         */
        TBool RemoveInviteL( const MCAInvitation* aInvitation );

        /**
         * Finds the invite from invite queue
         * @param aInviteId Invite to be searched
         * @return CCAInvitation if found, NULL otherwise
         */
        MCAInvitation* FindInvite( const TDesC& aInviteId );

        /**
         * Creates a CCAInvitation.
         * @param aInviteID invitation id
         * @param aUserId WV ID
         * @param aScreenName screen name
         * @param aGroupId group id
         * @param aInviteReason invite reason
         * @param aValidityPeriod validity period
         * @return created CCAInvitation.
         *         The ownership is transferred to caller.
         */
        MCAInvitation* CreateInviteRequestL( const TDesC& aInviteID,
                                             const TDesC& aUserID,
                                             const TDesC& aScreenName,
                                             const TDesC& aGroupID,
                                             const TDesC& aInviteReason,
                                             const TInt aValidityPeriod );

    private: // Data

        // Number that is used to generate unique ids
        TInt iInviteIDOrdinal;

        // Doesn't own. Pointers to observers
        RPointerArray<MCAInviteObserver> iInviteObservers;

        // Doesn't own. Pointer to engine's request mapper
        CCARequestMapper* iRequestMapper;

        // Reference to settings API
        MCASettings& iSettingsAPI;

        // not owned
        MCAImpsFactory* iImpsFactory;

        // Doesn't own. Pointer to wvengine's fundamental API
        MCAImpsFundClient* iImpsFundAPI;

        // The operation id of current operation
        TInt iCurrentOpId;

        // Doesn't own.
        const CCAInvitationRequest* iCurrentInviteRequest;

        // Owns. Last generated invite ID
        HBufC* iLastInviteId;

        // Owns.
        RPointerArray<CCAInviteTracker>	iInviteTrackerQueue;

        // Owns
        HBufC* iRejectReason;

        // group manager inerface, not owned
        MCAGroupManagerInterface* iGroupManager;
    };

#endif      // CCAINVITEMANAGER_H

// End of File
