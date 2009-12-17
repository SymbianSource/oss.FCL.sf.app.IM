/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements group services interface.
*
*/



#ifndef CCAIMPSGROUPCLIENT_H
#define CCAIMPSGROUPCLIENT_H

//  INCLUDES
#include "MCAImpsGroupClient.h"

// FORWARD DECLARATIONS
class RImpsEng;

// CLASS DECLARATION

/**
*  Implements MCAImpsGroupClient.
*  Forwards requests to RImpsGroupClient
*
*  @lib CAAdapter.dll
*  @since 1.2
*/
class CCAImpsGroupClient : public CBase, public MCAImpsGroupClient
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aImpsEng Reference to RImpsEng
         */
        static CCAImpsGroupClient* NewL( RImpsEng& aImpsEng );

        /**
        * Destructor.
        */
        virtual ~CCAImpsGroupClient();

    private:

        /**
        * C++ default constructor.
        * @param Reference to RImpsEng.
        */
        CCAImpsGroupClient( RImpsEng& aImpsEng );

    public: // Functions from base classes

        // INITILIZATION AND CONNECT

        /**
        * From MCAImpsGroupClient Registers the listener object for
        * Group events.
        * @since 1.2
        * @param aObserver The observer instance.
        * @param aApplicationId application UID or
        *                another unique id for your application.
        * @param aPriority observer priority. Refer to CActive priority.
        */
        virtual void RegisterL(
            MImpsGroupHandler2* aObserver,
            TUid aApplicationId,
            TInt aPriority );

        /**
        * Unregisters the listener object and disconnects from the server.
        * @since 1.2
        */
        virtual void Unregister();


        // WV PROTOCOL REQUESTS

        /**
        * From MCAImpsGroupClient Creates a group.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @param aProperties initial group properties
        * @param aScreenName ScreenName of the user
        * @param aJoinGroup indicates that the newly created group is joined
        * (or not) at creation time.
        * @param aSubscribeNotification indicates if we sucscribe notification.
        * @return oparation-id (positive)
        */
        virtual TInt CreateGroupL( const TDesC& aGroupId,
                                   const CImpsCommonGroupProps* aProperties,
                                   const TDesC& aScreenName, TBool aJoinGroup,
                                   TBool aSubscribeNotification = EFalse );

        /**
        * From MCAImpsGroupClient Deletes a group.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt DeleteGroupL( const TDesC& aGroupId );

        /**
        * From MCAImpsGroupClient Join a group.
        * MImpsGroupHandler::HandleJoinL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @param aScreenName screen name for the user, optional (may be zero
        *        length)
        * @param aUsers ETrue if a user wants to get list of joined users in a
        *        reponse,
        * @param aSubscribeNotification indicates if we sucscribe notification.
        * @return oparation-id (positive)
        */
        virtual TInt JoinGroupL( const TDesC& aGroupId,
                                 const TDesC& aScreenName, TBool aUsers,
                                 TBool aSubscribeNotification = EFalse );

        /**
        * From MCAImpsGroupClient Leave a group.
        * MImpsGroupHandler::HandleLeaveL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt LeaveGroupL( const TDesC& aGroupId );

        /**
        * From MCAImpsGroupClient Get group members.
        * MImpsGroupHandler::HandleGroupMembersL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt GroupMembersL( const TDesC& aGroupId );

        /**
        * From MCAImpsGroupClient Add group members.
        * The new users' type is ordinary initially.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @param aUserList list of users to be added (user-ids)
        * @param aScreenNameList Screennames of the users
        * @return oparation-id (positive)
        */
        virtual TInt AddMembersL( const TDesC& aGroupId,
                                  const MDesCArray& aUserList, const MDesCArray& aScreenNameList );

        /**
        * From MCAImpsGroupClient Remove group members.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @param aUserList list of users to be removed
        * @return oparation-id (positive)
        */
        virtual TInt RemoveMembersL( const TDesC& aGroupId,
                                     const MDesCArray& aUserList );

        /**
        * From MCAImpsGroupClient Modify members' access rights.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @param aAdminList list of new administrators user-ids
        * @param aModerList list of new moderators user-ids
        * @param aOrdinaryList list of new ordinary users user-ids
        * @return oparation-id (positive)
        */
        virtual TInt ModifyMembersAccessL( const TDesC& aGroupId,
                                           const MDesCArray* aAdminList, const MDesCArray* aModerList,
                                           const MDesCArray* aOrdinaryList );

        /**
        * From MCAImpsGroupClient Get group properties.
        * MImpsGroupHandler::HandleGroupPropertiesL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @return oparation-id (positive)
        */
        virtual TInt GroupPropertiesL( const TDesC& aGroupId );

        /**
        * From MCAImpsGroupClient Set group properties.
        * MImpsGroupHandler::HandlecompleteL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @param aGroupProps common properties
        * @param aOwnProps user's own properties
        * @return oparation-id (positive)
        */
        virtual TInt SetGroupPropertiesL( const TDesC& aGroupId,
                                          const CImpsCommonGroupProps* aGroupProps,
                                          const CImpsPrivateGroupProps* aOwnProps );

        /**
        * From MCAImpsGroupClient Update or get list of rejected users.
        * MImpsGroupHandler::HandleRejectListL handles the server response.
        * If you want to get the current reject list only, then
        * give aGroupId parameter only.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId target group id
        * @param aRejectedList users to be added to the reject list
        *        (user-ids) May be NULL.
        * @param aEnabledList users to be removed from the reject list
        *        (user-ids) May be NULL.
        * @return oparation-id (positive)
        */
        virtual TInt SetRejectListL( const TDesC& aGroupId,
                                     const MDesCArray* aRejectedList, const MDesCArray* aEnabledList );

        /**
        * From MCAImpsGroupClient Subscribe group change notice.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * After successfull subscription HandleGroupPropertiesL,
        * HandleNewUsersL and HandleLeftUsersL methods are called
        * whenever remote service sends notifications.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt SubscribeL( const TDesC& aGroupId );

        /**
        * From MCAImpsGroupClient Unsubscribe group change notice.
        * MImpsGroupHandler::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt UnsubscribeL( const TDesC& aGroupId );

        /**
        * From MCAImpsGroupClient Check the group change subscription status.
        * MImpsGroupHandler::HandleSubscriptionL handles the server response.
        * Leaves if out of memory before sending a message to the
        * Symbian OS server.
        * @since 1.2
        * @param aGroupId group id
        * @return oparation-id (positive)
        */
        virtual TInt CheckSubscriptionL( const TDesC& aGroupId );

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
        * Status observer accessor. This is missing from WV Engine.
        * Introduced for consistency.
        * @since 1.2
        * @return Status observer pointer. Ownership NOT transferred.
        */
        virtual MImpsStatusHandler2* StatusHandler() const;

        /**
        * Returns actual WV Engine interface class pointer which is registered.
        * @since 1.2
        * @return RImpsGroupClient pointer which is registered and can be used.
        *  Ownership NOT transferred.
        */
        RImpsGroupClient2* InterfaceL();

        /**
        * Available services accessor
        * Notice that there are no services in NOT_LOGGED state, i.e.
        * before login has been executed successfully from any client.
        * @param aServices Service tree having all supported features
        *        and functions.
        * @since 1.2
        */
        virtual void GetServicesL( TImpsServices& aServices );

    private:    // Data

        // WV Engine group interface.
        RImpsGroupClient2 iClient;

        // Indicator if we are doing lazy initialization with WV Engine.
        TBool	iLazyInitialization;

        // Indicator if we have registered WV Engine interface.
        TBool   iRegistered;

        // Store priority if we are doing lazy initialization.
        TInt	iPriority;

        // Store application id if we are doing lazy initialization.
        TUid	iApplicationId;

        // Group observer, stored in lazy initialization. Doens't own.
        MImpsGroupHandler2*	iGroupHandler;

        // Status observer, stored in lazy initialization. Doens't own.
        MImpsStatusHandler2* iStatusHandler;

        // Error observer, stored in lazy initialization. Doens't own.
        MImpsErrorHandler2* iErrorHandler;

        // Reference to IMPS engine server session instance.
        RImpsEng& iImpsEng;
    };

#endif      // MCAIMPSGROUPCLIENT_H

// End of File
