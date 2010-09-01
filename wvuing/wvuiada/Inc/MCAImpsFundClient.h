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
* Description:  Abstract interface for fundamental services (RImpsFundClient)
*
*/



#ifndef MCAIMPSFUNDCLIENT_H
#define MCAIMPSFUNDCLIENT_H

//  INCLUDES
#include "MCAImpsClient.h"
#include <ImpsFundCli.h>

// CLASS DECLARATION

/**
*  Interface class for Wireless Village fundamental services.
*  Same interface as RImpsFundClient.
*  Comments are copied from RImpsFundClient.
*
*  @lib CAAdapter.dll
*  @since 1.2
*/
class MCAImpsFundClient : public MCAImpsClient
    {
    public: // New functions

        // INITILIZATION AND CONNECT

        /**
        * Registers the listener objects for Fundamental events.
        * @since 1.2
        * @param aSearchObs search result observer. May be NULL.
        * @param aInviteObs invite observer. May be NULL. Future option.
        * @param aPriority Observer priority. Refer to CActive priority.
        */
        virtual void RegisterL( MImpsSearchHandler2* aSearchObs,
                                MImpsInviteHandler2* aInviteObs,
                                TInt aPriority = 0 ) = 0;

        /**
        * Unregisters the listener objects and disconnects from the server.
        * @since 1.2
        */
        virtual void Unregister() = 0;


        // Fundamental SERVICES

        /**
        * Start search
        * @since 1.2
        * @param aPairs defines what is to be searched
        * @param aSearchLimit how many results you want
        * @return oparation-id (positive)
        */
        virtual TInt SearchFirstL( const CSearchPairs& aPairs,
                                   TInt aSearchLimit ) = 0;

        /**
        * Continue search
        * @since 1.2
        * @param aSearchID which search is to be continued
        * @param aIndex from which index the search is continued
        * @return oparation-id (positive)
        */
        virtual TInt SearchNextL( TInt aSearchID, TInt aIndex ) = 0;


        /**
        * Stop search
        * @param aSearchID which search is to be continued
        * @since 1.2
        * @return oparation-id (positive)
        */
        virtual TInt StopSearchL( TInt aSearchID ) = 0;

        /**
        * Send group invite request
        * InviteGroupRequest
        * @since 1.2
        * @param aInviteId Generated invite id
        * @param aUserId IDs of the users to be invited
        * @param aScreenName Screen names (<SName>) of the users to be invited
        * @param aGroupName The group (<GroupID>) in which the screen name is unique
        * @param aInviteGroup ID of the group to which the users are invited to join
        * @param aOwnScreenName Screen name of the inviter
        * @param aOwnGroupName Group name of the inviter
        * @param aInviteReason A short text describing the reason of the invite
        * @param aValidityPeriod How long the invitation is valid
        * @return oparation-id (positive)
        */
        virtual TInt InviteGroupRequestL( const TDesC& aInviteId,
                                          const MDesCArray* aUserId,
                                          const MDesCArray* aScreenName,
                                          const MDesCArray* aGroupName,
                                          const TDesC& aInviteGroup,
                                          const TDesC& aOwnScreenName,
                                          const TDesC& aOwnGroupName,
                                          const TDesC& aInviteReason,
                                          const TInt aValidityPeriod ) = 0;

        /**
        * Cancel a group invite request
        * CancelGroupInviteRequest
        * @since 1.2
        * @param aInviteId Invite to be cancelled
        * @param aUserId IDs of the users
        * @param aScreenName ID of the group (<GroupID>) in which the screen name is unique
        * @param aGroupName Name of the group
        * @param aCancelReason A short text describing the reason for the cancelling.
        * @param aOwnScreenName Screen name of the canceller
        * @param aOwnGroupName Group name of the canceller.
        * @return oparation-id (positive)
        */
        virtual TInt CancelGroupInviteRequestL( const TDesC& aInviteId,
                                                const MDesCArray* aUserId,
                                                const MDesCArray* aScreenName,
                                                const MDesCArray* aGroupName,
                                                const TDesC& aCancelReason,
                                                const TDesC& aOwnScreenName,
                                                const TDesC& aOwnGroupName ) = 0;

        /**
        * Send response to invite
        * InviteUserResponse
        * @since 1.2
        * @param aInviteId         = ID of the invite response
        * @param aAcceptance       = Do we meet the claim?
        * @param aInviteResponse   = Textual description of the response
        * @param aOwnScreenName    = Screen name of the responding user
        * @param aOwnGroupName     = Group name of the responding user
        * @return oparation-id (positive)
        */
        virtual TInt GroupInviteUserResponseL( const TDesC& aInviteId,
                                               TBool aAcceptance,
                                               const TDesC& aInviteResponse,
                                               const TDesC& aOwnScreenName,
                                               const TDesC& aOwnGroupName ) = 0;

    protected:  // New functions

        /**
        * Destructor.
        */
        virtual ~MCAImpsFundClient() { };

    };

#endif      // MCAIMPSFUNDCLIENT_H

// End of File
