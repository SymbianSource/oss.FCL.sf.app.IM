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
*       Interface for WV fundamental services.
*       The supported features are search and invite.
*
*/


#ifndef CImpsFundCli_H
#define CImpsFundCli_H

// INCLUDES
#include "impsclient.h"
#include "impsfundamental.h"

// FORWARD DECLARATIONS
class CImpsFundCommand2;
class MImpsSearchHandler2;
class MImpsInviteHandler2;

// CLASS DECLARATION

/**
*  RImpsFundClient2 API
*
*  Applications use the subsystem via RImpsFundClient2 class. First, they need
*  to connect to the server by calling RegisterL() member function. 
*  
*  When everything is done, call Unregister() to end
*  the session.
*
*  A user of this API must be aware that raising/releasing
*  the PDP context may affect the response times of called functions.
*
*
*  Observer methods:
*  -----------------------
*  Client can be notified when a certain type of message is received 
*  from the remote server.
*  Client has to be connected in the server in order to get notify events.
*  If the client is disconnected by server by calling Unregister(), all notify
*  requests and possible incoming events are canceled. 
*
*  Notify events are session-specific, ie. if you have several RImpsFundClients
*  connected to the server, each has its own notify requests. 
*
*  In order to use search feature, you need to implement MImpsSearchHandler2 
*  and give a pointer to your class in RegisterL.
*  Observer method doesn't need to do anything else than your application 
*  needs it to do.
*
*/ 


class RImpsFundClient2 : public RImpsClient2
    {

    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */      
        IMPORT_C RImpsFundClient2();

    public: // New functions        

        // INITILIZATION AND CONNECT

        /**
        * Registers the listener objects for Fundamental events and connects
        * to the Symbian OS Server.
        * Leaves with KImpsErrorAlreadyInUse if the client 
        * has already registered.
        * @param aEngine WV Engine server instance
        * @param aSearchObs search result observer. May be NULL.
        * @param aInviteObs invite observer. May be NULL.
        * @param aClientId Client-Id for requests if non zero length.
        * @param aReceiveNew determines if new pushed messages are received.
        *        If EFalse then new messages are filtered out and 
        *        responses to own requests are deliverd only.
        * @param aPriority Observer priority. Refers to CActive priority.
        */
        IMPORT_C void RegisterL( RImpsEng& aEngine, 
                                 MImpsSearchHandler2* aSearchObs,
                                 MImpsInviteHandler2* aInviteObs,
                                 const TDesC& aClientId,
                                 TBool aReceiveNew = ETrue,
                                 TInt aPriority = CActive::EPriorityStandard );

        /**
        * Unregisters the listener objects and disconnects from the server.
        */
        IMPORT_C void Unregister();


        // Fundamental SERVICES

        /**
        * Start search
        * @param aPairs defines what is to be searched
        * @param aSearchLimit how many results you want
        * @return operation-id (positive)
        */
        IMPORT_C TInt SearchFirstL( const CSearchPairs& aPairs,
                                    TInt aSearchLimit );
        
        /**
        * Continue search
        * @param aSearchID which search is to be continued
        * @param aIndex from which index the search is continued
        * @return operation-id (positive)
        */
        IMPORT_C TInt SearchNextL( TInt aSearchID, 
                                   TInt aIndex );


        /**
        * Stop search
        * @param aSearchID which search is to be stopped
        * @return operation-id (positive)
        */
        IMPORT_C TInt StopSearchL( TInt aSearchID );

              
        /* --- Comments on the parameters ---
        *
        *  - aInviteID
        *    Spec says this must be unique in the context of 
        *    the "server domain".
        *    Thus it is possible, though not very likely, that some other
        *    client may send the exact same ID that we did.
        *  
        *  - aUserID, aScreenName & aGroupName
        *    There may be more than one user in more than one group invited
        *    with a single request. The list of the invitees, according to 
        *    the spec, can be "any combination of" user IDs and screen names.
        *    What this means in practice is that the users can be identified 
        *    either by their user ID or screen name. For this reason, 
        *    the indexes of aScreenName and aGroupName
        *    MUST correspond to each other. 
        *    For example, if we want to invite the user <SName="Opa"> in 
        *    <GroupID="Chess">, <SName="Opa"> in <GroupID="Chess">, 
        *    aScreenName[0] must be "Opa" and aUserGroupID[0] "Chess". 
        *    Thus, if user ID is specified, the client does not check the array
        *    for screen names on top of that. If, on the other hand, there 
        *    is no user ID, BOTH screen name AND group ID MUST be present.
        *
        *  --- Comments end --- 
        */

        /** 
        *  Invite users to a group
        *  Primitive: InviteRequest
        *  Note that the aScreenName & aGroupName parameters together form the
        *  actual screen name primitives
        *  @param aInviteID         = Invite ID; unique in the context 
        *                             of the session
        *  @param aUserID           = IDs of the users to invite
        *  @param aScreenName       = Screen names (<SName>) of the users 
        *                             to be invited
        *  @param aGroupName        = The group (<GroupID>) in which the screen 
        *                             name is unique
        *  @param aInviteGroup      = ID of the group to which the users
        *                             are invited to join
        *  @param aOwnScreenName    = SName of the inviter
        *  @param aOwnGroupName     = The group, in which the inviters 
        *                             SName is unique
        *  @param aInviteReason     = Free text
        *  @param aValidityPeriod   = How long is this invitation valid
        *  @return operation-id (positive)
        */
        IMPORT_C TInt GroupInviteL( const TDesC& aInviteID,
                                    const MDesCArray* aUserID,
                                    const MDesCArray* aScreenName,
                                    const MDesCArray* aGroupName,
                                    const TDesC& aInviteGroup,
                                    const TDesC& aOwnScreenName,
                                    const TDesC& aOwnGroupName,
                                    const TDesC& aInviteReason,
                                    const TInt aValidityPeriod );

        /** 
        *  Invite user to IM conversation
        *  Primitive: InviteRequest
        *  @param aInviteID         = Invite ID; unique in the context 
        *                             of the session
        *  @param aInviteReason     = Free text
        *  @param aValidityPeriod   = How long is this invitation valid?
        *  @return operation-id (positive)
        */
        IMPORT_C TInt ImInviteL( const TDesC& aInviteID,
                                 const TDesC& aUserID,                              
                                 const TDesC& aInviteReason,
                                 const TInt aValidityPeriod );
        
        /**
        *  Cancel an invite 
        *  Primitive: CancelInviteRequest
        *  @param aInviteID         = Invite to be cancelled
        *  @param aUserID           = ID(s) of the user(s)
        *  @param aScreenName       = Screen names (<SName>) of the users
        *  @param aGroupName        = Name of the group 
        *  @param aCancelReason     = A short text describing the 
        *                             reason for the cancelling.
        *  @param aOwnScreenName    = Screen name of the canceller
        *  @param aOwnGroupName     = Group name of the canceller.
        *  @return operation-id (positive)
        */
        IMPORT_C TInt CancelInviteL( const TDesC& aInviteId,
                                     const MDesCArray* aUserID,
                                     const MDesCArray* aScreenName,
                                     const MDesCArray* aGroupName,
                                     const TDesC& aCancelReason, 
                                     const TDesC& aOwnScreenName,
                                     const TDesC& aOwnGroupName );

        /**
        *  Send response to invite
        *  Primitive: InviteUserResponse
        *  @param aInviteId         = ID of the invite response
        *  @param aAcceptance       = Do we meet the claim
        *  @param aInviteResponse   = Textual description of the response
        *  @param aOwnScreenName    = Screen name of the responding user
        *  @param aOwnGroupName     = Group name of the responding user
        *  @return operation-id (positive)
        */
        IMPORT_C TInt InviteResponseL( const TDesC& aInviteId,
                                       TBool aAcceptance,
                                       const TDesC& aInviteResponse,
                                       const TDesC& aOwnScreenName,
                                       const TDesC& aOwnGroupName );


        /**
        * Pointer to the search observer handler
        * @return handler
        */
        inline MImpsSearchHandler2* SearchHandler() const;

        /**
        * Pointer to the invite observer handler
        * @return handler
        */
        inline MImpsInviteHandler2* InviteHandler() const;


    private:

        /**
        * Registers the listener object for Fund events and connects to
        * the Symbian OS Server.
        * @param aEngine WV Engine server instance
        * @param aHandler the observer
        * @return general error code
        */
        TInt DoRegister( 
           RImpsEng& aEngine,
           CImpsHandler2* aHandler );

    private: // data

        CImpsFundCommand2*             iCommand;
        MImpsSearchHandler2*           iSearchCallBack;
        MImpsInviteHandler2*           iInviteCallBack;
       

    private: // friend classes

        friend class CImpsFundCommand2;
    
    };

// CLASS DECLARATION

/**
* MImpsSearchHandler2
*
* Abstract interface for handling the search result events from the server.
* User derives his class from this and implements the methods below.
*/

class MImpsSearchHandler2
    {

    public: // New functions

    /**
    * Observer method for search response
    * Primitive: SearchResponse
    * @param aId operation id returned by SearchFirstL or SearchNextL.
    * @param aSearchID search operation id, 
    *        used when search is continued or stopped.
    * @param aIndex indicates the particular index from which the 
    *        next search can start. It is 0 in error case.
    * @param aResultsFound indicates the number of the current findings.
    * @param aCompleted TRUE if server has completed the search, 
    *        FALSE if there are more results available.
    * @param aType which kind of items there are in aResults parameter
    * @param aResults list of results, 
    *        NULL if none.
    * @param aCspId CSP session identifier
    */
    virtual void HandleSearchL( TInt aId, 
                                TInt aSearchId, 
                                TInt aIndex, 
                                TInt aResultsFound, 
                                TBool aCompleted, 
                                TImpsSearchResultType aType,
                                MDesCArray* aResults ,
                                TImpsCspIdentifier& aCspId )  = 0;


    /**
    * Observer for stopped search
    * This is called when WV server has stopped the search after StopSearchL.
    * Notice that a previous search is invalidated 
    * when a new search is started,
    * but this method is not called in that situation.
    * @param aId operation id returned by StopSearchL.
    * @param aSearchID search operation id, 
    *        used when search was stopped.
    * @param aCspId CSP session identifier
    */
    virtual void HandleSearchStoppedL( TInt aId, 
                                       TInt aSearchId,
                                       TImpsCspIdentifier& aCspId )  = 0;

    };

/**
* MImpsInviteHandler2
*
* Abstract interface for handling invite events from the server.
* User derives from this and implements the pure virtual methods below.
*/
class MImpsInviteHandler2
    {
    public:

        /**
        *  Handle invite to a group
        *  Primitive: InviteUserRequest
        *  @param aInviteID         = ID of the invite request
        *  @param aUserID           = User ID of the inviter
        *  @param aScreenName       = SName of the inviter
        *  @param aGroupName        = GroupID of the inviter
        *  @param aInviteReason     = Textual description of the invitation
        *  @param aValidityPeriod   = Time that the invitation is valid
        *  @param aCspId CSP session identifier
        */
        virtual void HandleGroupInviteL( const TDesC& aInviteID,
                                         const TDesC& aUserID,
                                         const TDesC& aScreenName,
                                         const TDesC& aGroupName,
                                         const TDesC& aInviteReason,
                                         const TInt aValidityPeriod,
                                         TImpsCspIdentifier& aCspId )  = 0;

        /**
        *  Handle invite to a conversation
        *  Primitive: InviteUserRequest
        *  @param aInviteID         = ID of the invite request
        *  @param aUserID           = User ID of the inviter
        *  @param aInviteReason     = Textual description of the invitation
        *  @param aValidityPeriod   = Time that the invitation is valid
        *  @param aCspId CSP session identifier
        */
        virtual void HandleImInviteL( const TDesC& aInviteID,
                                      const TDesC& aUserID,
                                      const TDesC& aInviteReason,
                                      const TInt aValidityPeriod,
                                      TImpsCspIdentifier& aCspId )  = 0;

        /**
        *  Meaning of this method is to provide a way that SAP can use to ask 
        *  if the user accepts multimedia content in incoming messages. If the
        *  url is something else than a fixed one ( stored in CenRep ) the
        *  client should use browser to message retrieving. If the url is 
        *  the fixed one the client should either accept or deny the multimedia
        *  content receiving by sending InviteResponse.
        *  Primitive: InviteUserRequest
        *  @param aInviteID         = ID of the invite request
        *  @param aUserID           = User ID of the inviter
        *  @param aUrlList          = List of url's
        *  @param aInviteReason     = Textual description of the invitation
        *  @param aValidityPeriod   = Time that the invitation is valid
        *  @param aCspId CSP session identifier
        */
        virtual void HandleContentInviteL( const TDesC& aInviteID,
                                           const TDesC& aUserID,
                                           MDesCArray* aUrlList,
                                           const TDesC& aInviteReason,
                                           const TInt aValidityPeriod,
                                           TImpsCspIdentifier& aCspId )  = 0;

        /**
        *  Handle invite response  
        *  Primitive: HandleInviteResponse
        *  @param aInviteId             = ID of the invite request
        *  @param aAcceptance           = Was the invitation accepted or not
        *  @param aUserID               = UserID of the responding user
        *  @param aScreenName           = SName of the responding user
        *  @param aGroupName            = Group ID of the responding user
        *  @param aInviteResponse       = Textual description of the response
        * @param aCspId CSP session identifier
        */
        virtual void HandleInviteResponseL( const TDesC& aInviteID,
                                            const TBool aAcceptance,
                                            const TDesC& aUserID,
                                            const TDesC& aScreenName,
                                            const TDesC& aGroupName,
                                            const TDesC& aResponse,
                                            TImpsCspIdentifier& aCspId )  = 0;

        /**
        * Handle cancelled invite
        * Primitive: CancelInviteUserRequest
        * @param aInviteID              = ID of the invitation request
        * @param aUserID                = ID of the cancelling user
        * @param aScreenName            = SName of the canceller
        * @param aGroupName             = GroupID of the canceller
        * @param aCancelReason          = Textual description of 
        *                                 the cancellation
        * @param aCspId CSP session identifier
        */
        virtual void HandleInviteCancelL( const TDesC& aInviteID,
                                          const TDesC& aUserID,
                                          const TDesC& aScreenName,
                                          const TDesC& aGroupName,
                                          const TDesC& aResponse,
                                          TImpsCspIdentifier& aCspId )  = 0;

        /**
        * Handle server response after request
        * @param aOpId  operation id to map responses to the requests.
        * @param aCspId CSP session identifier
        */
        virtual void  HandleCompleteL( TInt aOperationId, 
                                       TImpsCspIdentifier& aCspId )  = 0;
    };

#include "impsfundcli.inl"

#endif
