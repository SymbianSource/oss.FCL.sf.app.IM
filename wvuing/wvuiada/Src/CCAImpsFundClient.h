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
* Description:  Implements fundamental services interface.
*
*/



#ifndef CCAIMPSFUNDCLIENT_H
#define CCAIMPSFUNDCLIENT_H

//  INCLUDES
#include "MCAImpsFundClient.h"

// FORWARD DECLARATIONS
class RImpsEng;

// CLASS DECLARATION

/**
*  Implements MCAImpsFundClient.
*  Forwards requests to RImpsFundClient
*
*  @lib CAAdapter.dll
*  @since 1.2
*/
class CCAImpsFundClient : public CBase, public MCAImpsFundClient
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param Reference to RImpsEng
        */
        static CCAImpsFundClient* NewL( RImpsEng& aImpsEng );

        /**
        * Destructor.
        */
        virtual ~CCAImpsFundClient();

    public: // Functions from base classes

        // INITILIZATION AND CONNECT

        /**
        * Registers the listener objects for Fundamental events and connects to
        * the Symbian OS Server.
        * @since 1.2
        * @param aSearchObs search result observer. May be NULL. Ownership NOT transferred.
        * @param aInviteObs invite observer. May be NULL. Ownership NOT transferred.
        * @param aPriority Observer priority. Refer to CActive priority.
        */
        virtual void RegisterL( MImpsSearchHandler2* aSearchObs,
                                MImpsInviteHandler2* aInviteObs,
                                TInt aPriority );

        /**
        * Unregisters the listener object and disconnects from the server.
        * @since 1.2
        */
        virtual void Unregister();

        // Fundamental SERVICES

        /**
        * Start search
        * @since 1.2
        * @param aPairs defines what is to be searched
        * @param aSearchLimit how many results you want
        * @return oparation-id (positive)
        */
        TInt SearchFirstL( const CSearchPairs& aPairs,
                           TInt aSearchLimit );

        /**
        * Continue search
        * @since 1.2
        * @param aSearchID which search is to be continued
        * @param aIndex from which index the search is continued
        * @return oparation-id (positive)
        */
        TInt SearchNextL( TInt aSearchID, TInt aIndex );


        /**
        * Stop search
        * @since 1.2
        * @param aSearchID which search is to be continued
        * @return oparation-id (positive)
        */
        TInt StopSearchL( TInt aSearchID );

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
        TInt InviteGroupRequestL( const TDesC& aInviteId,
                                  const MDesCArray* aUserId,
                                  const MDesCArray* aScreenName,
                                  const MDesCArray* aGroupName,
                                  const TDesC& aInviteGroup,
                                  const TDesC& aOwnScreenName,
                                  const TDesC& aOwnGroupName,
                                  const TDesC& aInviteReason,
                                  const TInt aValidityPeriod );

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
        TInt CancelGroupInviteRequestL( const TDesC& aInviteId,
                                        const MDesCArray* aUserId,
                                        const MDesCArray* aScreenName,
                                        const MDesCArray* aGroupName,
                                        const TDesC& aCancelReason,
                                        const TDesC& aOwnScreenName,
                                        const TDesC& aOwnGroupName );

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
        TInt GroupInviteUserResponseL( const TDesC& aInviteId,
                                       TBool aAcceptance,
                                       const TDesC& aInviteResponse,
                                       const TDesC& aOwnScreenName,
                                       const TDesC& aOwnGroupName );

        /**
        * Available services accessor
        * Notice that there are no services in NOT_LOGGED state, i.e.
        * before login has been executed successfully from any client.
        * @param aServices Service tree having all supported features and functions.
        * @since 1.2
        */
        virtual void GetServicesL( TImpsServices& aServices );

        /**
        * Register an error event observer.
        * If this is not registered then a client does not
        * receive error events from WV engine.
        * An application may implement only one HandleErrorL method
        * and give pointer to it in each WV client instance.
        * @since 1.2
        * @param aObs error event observer
        */
        virtual void RegisterErrorObserverL( MImpsErrorHandler2 &aObs );

        /**
        * Registers the service status change observer.
        * This can be called any time
        * @since 1.2
        * @param aObs status observer implementation. Ownership NOT transferred.
        */
        virtual void RegisterStatusObserverL( MImpsStatusHandler2 *aObs );

        /**
        * Unregisters an error observer.
        * Forwards the call to WV Engine always
        * This method is not necessary if unregister of the particular
        * client has been executed (Unregister).
        * @since 1.2
        */
        virtual void UnregisterErrorObserverL();

        /**
        * Unregisters the service status change observer.
        * Forwards the call to WV Engine always
        * This method is not necessary if unregister of the particular
        * client type has been executed (Unregister).
        * @since 1.2
        */
        virtual void UnregisterStatusObserverL();

        /**
        * Internal use only.
        * Error observer accessor
        * @since 1.2
        * @return error observer pointer. Ownership NOT transferred.
        */
        virtual MImpsErrorHandler2* ErrorHandler() const;

        /**
        * Internal use only.
        * Status observer accessor. This is missing from WV Engine. Introduced for consistency.
        * @since 1.2
        * @return Status observer pointer. Ownership NOT transferred.
        */
        virtual MImpsStatusHandler2* StatusHandler() const;

        /**
        * Returns actual WV Engine interface class pointer which is registered.
        * @since 1.2
        * @return RImpsFundClient pointer which is registered and can be used.
        *  Ownership NOT transferred.
        */
        RImpsFundClient2* InterfaceL();

    private:  // New functions

        /**
        * C++ default constructor.
        * @param Reference to RImpsEng
        */
        CCAImpsFundClient( RImpsEng& aImpsEng );

    private:    // Data

        // WV Engine fundamental interface.
        RImpsFundClient2 iClient;

        // Indicator if we are doing lazy initialization with WV Engine.
        TBool	iLazyInitialization;

        // Indicator if we have registered WV Engine interface.
        TBool   iRegistered;

        // Store priority if we are doing lazy initialization.
        TInt	iPriority;

        // Search observer, stored in lazy initialization. Doens't own.
        MImpsSearchHandler2* iSearchHandler;

        // Invite observer, stored in lazy initialization. Doens't own.
        MImpsInviteHandler2* iInviteHandler;

        // Status observer, stored in lazy initialization. Doens't own.
        MImpsStatusHandler2* iStatusHandler;

        // Error observer, stored in lazy initialization. Doens't own.
        MImpsErrorHandler2* iErrorHandler;

        // Refernce to IMPS engine server session instance.
        RImpsEng& iImpsEng;
    };

#endif      // MCAIMPSFUNDCLIENT_H

// End of File
