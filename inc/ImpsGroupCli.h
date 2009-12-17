/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     WV engine group feature interface.
*
*/


#ifndef RImpsGroupCli_H
#define RImpsGroupCli_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <bamdesca.h>
#include "impsconst.h"
#include "impsgroupprops.h"
#include "impsclient.h"


// FORWARD DECLARATIONS
class CImpsGroupHandler2;
class MImpsGroupHandler2;
class CImpsGroupCommand2;

// CLASS DECLARATION

/**
*  RImpsGroupClient2 API
*
*  Applications use the subsystem via RImpsGroupClient2 class. First, they 
*  need to connect to the server by calling RegisterL() member function. 
*  
*  When everything is done, call Unregister() to end
*  the session.
*
*  A user of this API must be aware that raising/releasing
*  the PDP context may affect the response times of called functions.
*
*  Observer methods:
*  -----------------------
*  Client can be notified when a certain type of message is received 
*  from the remote server.
*  Client has to be connected in the server in order to get notify events.
*  If the client is disconnected by server by calling Unregister(), all notify
*  requests and possible incoming events are canceled. 
*
*  Notify events are session-specific, 
*  ie. if you have several RImpsGroupClients
*  connected to the server, each has its own notify requests. 
*
*  In order to use notify services, you need to implement MImpsGroupHandler2 
*  and give a pointer to your class in RegisterL.
*  Observer method doesn't need to do anything else than your 
*  application needs it to do.
*
*
*/



class RImpsGroupClient2 : public RImpsClient2
    {

    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */      
        IMPORT_C RImpsGroupClient2();

    public: // New functions        

        // INITILIZATION AND CONNECT

        /**
        * Registers the listener object for Group events and connects to
        * the Symbian Server.
        * Leaves with KImpsErrorAlreadyInUse if the client 
        * has already registered.
        * @param aEngine WV Engine server instance
        * @param aObserver The observer instance.
        * @param aClientId Client-Id for requests if non zero length.
        * @param aReceiveNew determines if new pushed messages are received.
        *        If EFalse then new messages are filtered out and 
        *        responses to own requests are deliverd only.
        * @param aPriority Observer priority. Refers to CActive priority.
        */
        IMPORT_C void RegisterL( RImpsEng& aEngine, 
                                 MImpsGroupHandler2* aObserver,
                                 const TDesC& aClientId,
                                 TBool aReceiveNew = ETrue,
                                 TInt aPriority = CActive::EPriorityStandard );


        /**
        * Unregisters the listener object and disconnects from 
        * the Symbian OS server.
        */
        IMPORT_C void Unregister();


        // WV PROTOCOL REQUESTS

        /**
        * Create a group.
        * MImpsGroupHandler2::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message 
        * to the Symbian OS server.
        * @param aGroupId group id
        * @param aProperties initial group properties
        * @param aScreenName, ScreenName of the user
        * @param aJoingGroup indicates that the newly created group is  
        *        joined (or not) at creation time.
        * WV CSP static conformance has the statement CREAG-5.
        * CREAG-5:
        *       If the JoinGroup transaction was not agreed during
        *       service negotiation from either server or client side,
        *       the Join-Group flag in the CreateGroupRequest
        *       primitive indicates ‘F’ (false).
        * @param aSubscribeNotification indicates if we subscribe notification.
        * @return operation-id (positive)
        */
        IMPORT_C TInt CreateGroupL( const TDesC& aGroupId,
                                    const CImpsCommonGroupProps* aProperties,
                                    const TDesC& aScreenName,
                                    TBool aJoinGroup,
                                    TBool aSubscribeNotification = EFalse );

        /**
        * Delete a group.
        * MImpsGroupHandler2::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a 
        * message to the Symbian OS server.
        * @param aGroupId group id
        * @return operation-id (positive)
        */
        IMPORT_C TInt DeleteGroupL( const TDesC& aGroupId );

        /**
        * Join a group.
        * MImpsGroupHandler2::HandleJoinL handles the server response.
        * Leaves if out of memory before sending a message 
        * to the Symbian OS server.
        * @param aGroupId group id
        * @param aScreenName screen name for the user, 
        *  optional (may be zero length)
        * @param aUsers ETrue if a user wants to get list of joined 
        *  users in a reponse,
        * @param aSubscribeNotification indicates if we subscribe notification.
        * @return operation-id (positive)
        */
        IMPORT_C TInt JoinGroupL( const TDesC& aGroupId,
                                  const TDesC& aScreenName,
                                  TBool aUsers,
                                  TBool aSubscribeNotification = EFalse );

        /**
        * Leave a group.
        * MImpsGroupHandler2::HandleLeaveL handles the server response.
        * Leaves if out of memory before sending a message 
        * to the Symbian OS server.
        * @param aGroupId group id
        * @return operation-id (positive)
        */
        IMPORT_C TInt LeaveGroupL( const TDesC& aGroupId );

        /**
        * Get group members.
        * MImpsGroupHandler2::HandleGroupMembersL handles the server response.
        * Leaves if out of memory before sending a message 
        * to the Symbian OS server.
        * @param aGroupId group id
        * @return operation-id (positive)
        */
        IMPORT_C TInt GroupMembersL( const TDesC& aGroupId );

        /**
        * Add group members.
        * The new users' type is ordinary initially. 
        * MImpsGroupHandler2::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a 
        * message to the Symbian OS server. Leaves with 
        * KErrArgument if both aUserList and aScreenNameList are empty
        * @param aGroupId target group id
        * @param aUserList list of users to be added (user-ids)
        * @param aScreenNameList Screennames of the users
        * @return operation-id (positive)
        */
        IMPORT_C TInt AddMembersL( const TDesC& aGroupId,
                                   const MDesCArray& aUserList,
                                   const MDesCArray& aScreenNameList );

        /**
        * Remove group members.
        * MImpsGroupHandler2::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a 
        * message to the Symbian OS server.
        * Leaves with KErrArgument if aUserList is empty
        * @param aGroupId target group id
        * @param aUserList list of users to be removed
        * @return operation-id (positive)
        */
        IMPORT_C TInt RemoveMembersL( const TDesC& aGroupId,
                                      const MDesCArray& aUserList );

        /**
        * Modify members' access rights.
        * MImpsGroupHandler2::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a 
        * message to the Symbian OS server.
        * @param aGroupId target group id
        * @param aAdminList list of new administrators user-ids
        * @param aModerList list of new moderators user-ids
        * @param aOrdinaryList list of new ordinary users user-ids
        * @return operation-id (positive)
        */
        IMPORT_C TInt ModifyMembersAccessL( const TDesC& aGroupId,
                                            const MDesCArray* aAdminList,
                                            const MDesCArray* aModerList,
                                            const MDesCArray* aOrdinaryList );

        /**
        * Get group properties.
        * MImpsGroupHandler2::HandleGroupPropertiesL 
        * handles the server response.
        * Leaves if out of memory before sending a 
        * message to the Symbian OS server.
        * @param aGroupId target group id
        * @return operation-id (positive)
        */
        IMPORT_C TInt GroupPropertiesL( const TDesC& aGroupId );

        /**
        * Set group properties.
        * MImpsGroupHandler2::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message 
        * to the Symbian OS server.
        * @param aGroupId target group id
        * @param aGroupProps common properties
        * @param aOwnProps user's own properties
        * @return operation-id (positive)
        */
        IMPORT_C TInt SetGroupPropertiesL( const TDesC& aGroupId,
                                           const CImpsCommonGroupProps* aGroupProps,
                                           const CImpsPrivateGroupProps* aOwnProps );

        /**
        * Update or get list of rejected users.
        * MImpsGroupHandler2::HandleRejectListL handles the server response.
        * If you want to get the current reject list only, then
        * give aGroupId parameter only.
        * Leaves if out of memory before sending a message 
        * to the Symbian OS server.
        * @param aGroupId target group id
        * @param aRejectedList users to be added to the reject list (user-ids)
        *        May be NULL.
        * @param aEnabledList users to be removed from 
        *        the reject list (user-ids). May be NULL.
        * @return operation-id (positive)
        */
        IMPORT_C TInt SetRejectListL( const TDesC& aGroupId,
                                      const MDesCArray* aRejectedList,
                                      const MDesCArray* aEnabledList );

        /**
        * Subscribe group change notice.
        * MImpsGroupHandler2::HandleCompleteL handles the server response.
        * After successfull subscription HandleGroupPropertiesL,
        * HandleNewUsersL and HandleLeftUsersL methods are called
        * whenever remote service sends notifications. 
        * Leaves if out of memory before sending a message 
        * to the Symbian OS server.
        * @param aGroupId group id
        * @return operation-id (positive)
        */
        IMPORT_C TInt SubscribeL( const TDesC& aGroupId );

        /**
        * Unsubscribe group change notice.
        * MImpsGroupHandler2::HandleCompleteL handles the server response.
        * Leaves if out of memory before sending a message 
        * to the Symbian OS server.
        * @param aGroupId group id
        * @return operation-id (positive)
        */
        IMPORT_C TInt UnsubscribeL( const TDesC& aGroupId );

        /**
        * Check the group change subscription status.
        * MImpsGroupHandler2::HandleSubscriptionL handles the server response.
        * Leaves if out of memory before sending a message 
        * to the Symbian OS server.
        * @param aGroupId group id
        * @return operation-id (positive)
        */
        IMPORT_C TInt CheckSubscriptionL( const TDesC& aGroupId );

        /**
        * Pointer to the observer handler
        * @return handler
        */
        MImpsGroupHandler2* Handler();

    private: // New functions

        /**
        * Registers the listener object for Fund events and connects to
        * the Symbian OS Server.
        * @param aEngine WV Engine server instance
        * @param aHandler the observer
        * @return general error code
        */
        TInt DoRegister( RImpsEng& aEngine,
                         CImpsHandler2* aHandler );

    private: // data

        MImpsGroupHandler2*      iHandlerCallBack;
        CImpsGroupCommand2*      iCommand;


    private: // friend classes

        friend class CImpsGroupCommand2;
    
    };

// CLASS DECLARATION

/**
* MImpsGroupHandler2
*
* Abstract interface for handling the notify events from the server.
* User derives his class from this and implements the observer methods below.
*/

class MImpsGroupHandler2
    {

    public:

    /**
    * Handles a response of CreateGroupL, DeleteGroupL, AddMembersL, 
    * RemoveMembersL, ModifyMembersAccessL, SubscribeL, UnsubscribeL.
    * @param aOpId  operation id to map responses to the requests.
    * @param aCspId CSP session identifier
    */
    virtual void HandleCompleteL( TInt aOpId,
                                  TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Handles JoinGroupL response.
    * Leaves with KErrEof if no more data.
    * @param aOpId  operation id to map responses to the requests.
    * @param aUserList list of joined users if requested.
    * @param aScreenNames list of screen names if requested
    * @param aWelcomeText welcome text from remote service. 
    * @param aCspId CSP session identifier
    */
    virtual void HandleJoinL( TInt aOpId,
                              const MDesCArray& aUserList,
                              const MDesCArray& aScreenNames,        
                              const TDesC& aWelcomeText,
                              TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Handles GroupMembersL response.
    * Leaves with KErrEof if no more data.
    * @param aOpId  operation id to map responses to the requests.
    * @param aUserList list of users of ordinary users
    * @param aScreenNames list of screen names of ordinary users
    * @param aModers list of moderators (user-ids)
    * @param aAdmins list of administrators
    * @param aCspId CSP session identifier
    */
    virtual void HandleGroupMembersL( TInt aOpId,
                                      const MDesCArray& aUserList,
                                      const MDesCArray& aScreenNames,
                                      const MDesCArray& aModers,
                                      const MDesCArray& aAdmins,
                                      TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Handles GroupPropertiesL response and subscribed notification.
    * @param aOpId  operation id to map responses to the requests.
    *        This is 0 if event is initiated by remote service based on the
    *        group change notification subscription.
    * @param aGroupId group id, presnet only if aOpId is 0.
    * @param aGroupProps common properties
    * @param aOwnProps user's own properties
    * @param aCspId CSP session identifier
    */
    virtual void HandleGroupPropertiesL( TInt aOpId,
                                         const TDesC& aGroupId,
                                         const CImpsCommonGroupProps& aGroupProps,
                                         const CImpsPrivateGroupProps& aOwnProps, 
                                         TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Handles SetRejectListL response.
    * Leaves with KErrEof if no more data.
    * @param aOpId  operation id to map responses to the requests.
    * @param aUserList list of rejected users in reject list
    * @param aCspId CSP session identifier
    */
    virtual void HandleRejectListL( TInt aOpId,
                                    const MDesCArray& aUserList,
                                    TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Handles CheckSubscriptionL response.
    * @param aOpId  operation id to map responses to the requests.
    * @param aIsSubscribed ETrue if group is subscribed.
    * @param aCspId CSP session identifier
    */
    virtual void HandleSubscriptionL( TInt aOpId,
                                      TBool aIsSubscribed,
                                      TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Handles group change notification about new users.
    *  Error indicates internal error.
    * @param aGroupId group id
    * @param aUserList list of joined users
    * @param aScreenNames list of screen names
    * @param aCspId CSP session identifier
    */
    virtual void HandleNewUsersL(
        const TDesC& aGroupId,
        const MDesCArray& aUserList,
        const MDesCArray& aScreenNames,
        TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Handles group change notification about users left.
    * Error indicates internal error.
    * @param aGroupId group id
    * @param aUserList list of users left
    * @param aScreenNames list of screen names
    * @param aCspId CSP session identifier
    */
    virtual void HandleLeftUsersL(
        const TDesC& aGroupId,
        const MDesCArray& aUserList,
        const MDesCArray& aScreenNames,
        TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Handles both LeaveGroupL response and a service 
    * initiated group leave situation.
    * Leaves with KErrEof if no more data.
    * @param aOpId  operation id to map responses to the requests. 
    *        This is 0 if operation is initiated by a remote service.
    * @param aGroupId group id
    * @param aDescription textual description of the leave
        * @param aCspId CSP session identifier
    */
    virtual void HandleLeaveL(
        TInt aOpId,
        const TDesC& aGroupId,
        const TDesC& aDescription,
        TImpsCspIdentifier& aCspId )  = 0;

    };
    
#endif 

// End of File


