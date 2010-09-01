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
*     Interface for WV IM services.
*
*/


#ifndef IMPS_IM_CLIENT_H
#define IMPS_IM_CLIENT_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <bamdesca.h>
#include "impsconst.h"
#include "impsclient.h"

// CONSTANTS

// FORWARD DECLARATIONS
class MImpsImHandler2;
class MImpsBlockingHandler2;
class CImpsImCommand2;

// CLASS DECLARATION

/**
*  RImpsImClient2 API
*
*  Applications use IM service via RImpsImClient2 class. First, they need to
*  connect to the server by calling RegisterL() member function.
*  Immediately after that an error handler must be registered by 
*  RegisterErrorObserverL. 
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
*  Notify events are session-specific, ie. if you have several RImpsImClients
*  connected to the server, each has its own notify requests. 
*
*  In order to use notify services, you need to implement MImpsImHandler2 
*  and give a pointer to your class in RegisterL.
*  Observer method doesn't need to do anything else than your application 
*  needs it to do.
*
*
*  Addressing:
*  -----------
*  WV CSP specification defines characters that must be escaped if they
*  occur within the User-ID, Resource or Domain portions of a Wireless Village
*  address. WV engine does NOT escape those characters on behalf of user 
*  application. WV CSP also denies usage of certain characters in UserID
*  totally.
*
*/


// CLASS DECLARATION


class RImpsImClient2 : public RImpsClient2
    {

    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */      
        IMPORT_C RImpsImClient2();

    public: // New functions        

        // INITILIZATION AND CONNECT

        /**
        * Registers the listener object for IM events and connects to
        * the Symbian OS Server.
        * Leaves with KImpsErrorAlreadyInUse if the client 
        * has already registered.
        * @param aEngine WV Engine server instance
        * @param aObserver new message observer. May be NULL.
        * @param aBlockingHandler block feature observer. May be NULL.
        * @param aClientId Client-Id. If non zero length then new messages
        *        having this Client-Id only are delivered 
        *        when aReceiveNew is ETrue. 
        * @param aReceiveNew determines if new pushed messages are received.
        *        If EFalse then new messages are filtered out and 
        *        responses to own requests are deliverd only.
        * @param aPriority observer priority. Refer to CActive priority.
        */
        IMPORT_C void RegisterL( RImpsEng& aEngine,
                                 MImpsImHandler2* aImpsObserver,
                                 MImpsBlockingHandler2* aBlockingHandler,
                                 const TDesC& aClientId,
                                 TBool aReceiveNew = ETrue,
                                 TInt aPriority = CActive::EPriorityStandard );

        /**
        * Unregisters listener objects and disconnects from the Symbian server.
        */
        IMPORT_C void Unregister();


        // MESSAGING


        /**
        * Send a text message. 
        * MImpsImHandler2 handles the server response.
        * Leaves if out of memory before sending a message to the SOS server.
        * If sender information is not given then WV engine inserts user's
        * UserId as a sender.
        * Recipient must be specified, i.e. at least aUserIds or aGroupId
        * must be specified.
        * Errors are handled by MImpsErrorHandler2.
        * @param aSenderSn, sender's screen name. Optional.      
        * @param aUserIds recipients UserID, optional
        * @param aGroupId, optional. 
        *        1) Group recipient as such or 
        *        2) GroupID for aScreenNames. 
        *        GroupID is used as Recipient( Group ( GroupID ) ) only if
        *        aScreenNames is omitted.
        * @param aScreenNames recipients' screen names in the group, optional
        *        Recipient(Group*(ScreenName( SName)))
        * @param aContent UNICODE text/plain content
        * @param aDeliveryReportWanted True, if the user wats to 
        *        order delivery report. HandleDeliveryReportL from
        *        MImpsImHandler handles the arriving report.
        * @return operation-id 
        */
        IMPORT_C TInt SendTextMessageL( const TDesC* aSenderSn,   
                                        const MDesCArray* aUserIds,
                                        const TDesC* aGroupId,         
                                        const MDesCArray* aScreenNames, 
                                        const TDesC& aContent, 
                                        TBool aDeliveryReportWanted );

        /**
        * Send a message with binary content. 
        * MImpsImHandler2 handles the server response.
        * Leaves if out of memory before sending a message to the SOS server.
        * Leaves with KErrNotSupported if the content type is not supported.
        * If sender information is not given then WV engine inserts user's
        * UserId as a sender.
        * Recipient must be specified, i.e. at least aUserIds or aGroupId
        * must be specified.
        * Errors are handled by MImpsErrorHandler2.
        * @param aSenderSn, sender's screen name. Optional. 
        * @param aUserIds recipients UserID, optional
        * @param aGroupId, optional. 
        *        1) Group recipient as such or 
        *        2) GroupID for aScreenNames. 
        *        GroupID is used as Recipient( Group ( GroupID ) ) only if
        *        aScreenNames is omitted.
        * @param aScreenNames recipients' screen names in the group, optional
        *        Recipient(Group*(ScreenName( SName)))
        * @param aContentType MIME type of the content
        * @param aContent The content of the message
        * @param aDeliveryReportWanted True, if the user wats to 
        *        order delivery report. HandleDeliveryReportL from
        *        MImpsImHandler2 handles the arriving report.
        * @return operation-id 
        */
        IMPORT_C TInt SendContentMessageL( const TDesC* aSenderSn,     
                                           const MDesCArray* aUserIds,
                                           const TDesC* aGroupId,         
                                           const MDesCArray* aScreenNames, 
                                           const TDesC& aContentType, 
                                           const TDesC8& aContent,
                                           TBool aDeliveryReportWanted );

        /**
        * Block, UnBlock, Grant, UnGrant users based on UserID
        * MImpsBlockHandler2 handles successful server response.
        * Errors are handled by MImpsErrorHandler2
        * @param aBlockEntity entities to be blocked
        * @param aUnBlockEntity entities to be unblocked
        * @param aBlockedListInUse is blocked list in use or not
        * @param aGrantedEntityList entities to be granted
        * @param aUnGrantEntity entities to be ungranted
        * @param aGrantedListInUse is granted list in use or not
        * @return operation-id 
        */
        IMPORT_C TInt BlockEntityRequestL( const MDesCArray* aBlockEntity,
                                           const MDesCArray* aUnBlockEntity,
                                           TBool aBlockedListInUse,
                                           const MDesCArray* aGrantedEntityList,
                                           const MDesCArray* aUnGrantEntity,
                                           TBool aGrantedListInUse );

        /**
        * Get Blocked and granted users list from server
        * Supports only UserIDs in response.
        * MImpsBlockHandler2 handles successful server response.
        * Errors are handled by MImpsErrorHandler2
        * @return operation-id 
        */
        IMPORT_C TInt GetBlockedListRequestL();

        /**
        * Pointer to the observer handler
        * @return handler
        */
        MImpsImHandler2* Handler();

        /**
        * Pointer to the block observer handler
        * @return handler
        */
        MImpsBlockingHandler2* BlockHandler();

    private: // New functions

       /**
        * Registers the listener object for Access events and connects to
        * the Symbian OS Server.
        * @param aEngine WV Engine server instance
        * @param aHandler the observer
        * @return general error code
        */
        TInt DoRegister( 
            RImpsEng& aEngine,
            CImpsHandler2* aHandler );


    private: // data

        MImpsImHandler2*             iImHandlerCallback;
        MImpsBlockingHandler2*       iBlockingHandlerCallback;
        CImpsImCommand2*             iCommand;
        TPtrC8                       iMessagePtr;

    private: // friend classes

         friend class CImpsImCommand;
    };

// CLASS DECLARATION

/**
* MImpsImHandler2
*
* Abstract interface for handling the notify events from the server.
* User derives his class from this and implements the methods below.
*/

class MImpsImHandler2
    {

    public: // New functions

    /**
    * Observer method for incoming instant message
    * @param aOpId operation id map response to the 
    *        GetMessageFromServerL request.
    *        For new "pushed" messages this is 0. 
    * Operation code may be 0 if the event is no consequence of
    * the request from this session. (e.g. GetMessage)
    * @param aMessageId message-id
    * @param aSender message sender, UserID or ScreenName if aGroupId
    * @param aGroupId GroupID for ScreenName in aSender, may be zero length.
    *        If aRecipients and aScreenNames are empty then this specifies
    *        message recipient too.
    * @param aRecipients recipients, UserIDs
    * @param aScreenNames screen name recipients, 
    *        Recipient(Group(ScreenName, SName ))
    *        The groups for screen names are in aSnGroups.
    * @param aText message text in UNICODE format.
    * @param aCspId CSP session identifier
    */
    virtual void HandleNewTextMessageL( TInt aOpId,
                                        const TDesC& aMessageId,
                                        const TDesC& aSender,   
                                        const TDesC& aGroupId, 
                                        const MDesCArray& aRecipients,
                                        const MDesCArray& aScreenNames,                                   
                                        const TDesC& aText,
                                        TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Observer method for incoming content message
    * @param aOpId operation id map response to the 
    *        GetMessageFromServerL request.
    *        For new "pushed" messages this is 0. 
    * Operation code may be 0 if the event is no consequence of
    * the request from this session. (e.g. GetMessage)
    * @param aMessageId message-id
    * @param aSender message sender, UserID or ScreenName if aGroupId
    * @param aGroupId GroupID for ScreenName in aSender, may be zero length.
    *        If aRecipients and aScreenNames are empty then this specifies
    *        message recipient too.
    * @param aRecipients recipients, UserIDs
    * @param aScreenNames screen name recipients, 
    *        Recipient(Group(ScreenName, SName ))
    *        The groups for screen names are in aSnGroups.    
    * @param aContentType MIME type of the content
    * @param aContent The content of the message
    * @param aCspId CSP session identifier
    */
    virtual void HandleNewContentMessageL( TInt aOpId,
                                           const TDesC& aMessageId,
                                           const TDesC& aSender,   
                                           const TDesC& aGroupId, 
                                           const MDesCArray& aRecipients,
                                           const MDesCArray& aScreenNames,                  
                                           const TDesC& aContentType, 
                                           const TDesC8& aContent,
                                           TImpsCspIdentifier& aCspId )  = 0;

    /**
    * Observer for message send completion.
    * This means that SAP has confirmed the send transaction.
    * Common for all kind of messages.
    * @param aOpId  operation id to map responses to the requests.
    * @param aDeliveryReportOrdered ETrue if delivery report wanted.
    * @param aCspId CSP session identifier
    */
    virtual void HandleSendCompleteL( TInt aOpId,
                                      TBool aDeliveryReportOrdered,
                                      TImpsCspIdentifier& aCspId )  = 0;

   
    /**
    * Observer for delivery report request.
    * This means that SAP has delivered the message to all recipients.
    * @param aMessageId MessageID from message that has been delivered.
    * @param aResult result code
    * @param aDescripton result description or NULL
    * @param aCspId CSP session identifier
    */
    virtual void HandleDeliveryReportL( const TDesC& aMessageId,
                                        TInt aResult,
                                        const TDesC* aDescription,
                                        TImpsCspIdentifier& aCspId )  = 0;

    };

//Handler
class MImpsBlockingHandler2
    {
    public:

        /**
        * Handler for GetBlockedListRequestL reponse
        * @param aBlockedList list of blocked users identified by UserID
        * @param aBlockedListInUse is blocked list use or not
        * @param aGrantedList list of granted users identified by UserID
        * @param aGranedListInUse is granted list use or not
        * @param aCspId CSP session identifier
        */
        virtual void GetBlockedListResponseL( const MDesCArray* aBlockedList,
                                              TBool aBlockedListInUse,
                                              const MDesCArray* aGrantedList,
                                              TBool aGrantedListInUse,
                                              TImpsCspIdentifier& aCspId )  = 0;

               
        /**
        * Handler for BlockEntityRequestL response
        * @param aOpId  operation id to map responses to the requests.
        * @param aCspId CSP session identifier
        */
        virtual void HandleCompleteL( TInt aOpId, 
                                      TImpsCspIdentifier& aCspId )  = 0;
    };


#endif 
