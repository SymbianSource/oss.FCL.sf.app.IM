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
* Description:  Process Component for Conversations/Groups
*
*/



#ifndef CCACONVERSATIONPC_H
#define CCACONVERSATIONPC_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>

#include "MCAConversationPC.h"
#include "chatdefinitions.h"
#include "MCAChatObserver.h"
#include "MCAStoredContactsObserver.h"
#include "MCAMessageErrorObserver.h"
#include "MCAMessageRWInterfacePC.h"


// FORWARD DECLARATIONS
class MCAChatInterface;
class MCAStoredContacts;
class MCAConversationObserverPC;
class MCAMessagesReadInterface;
class MCAMessagesWriteInterface;
class CCAEngine;
class MCAStoredGroups;
class MCAMessageContainerInfo;



// CLASS DECLARATION
/**
*  CCAConversationPC
*  Process Component class for Conversations
*
*/
class CCAConversationPC : public CBase,
            public MCAConversationPC,
            public MCAChatObserver,
            public MCAStoredContactsObserver,
            public MCAMessageErrorObserver,
            public MCAMessageRWInterfacePC
    {



    public: // From MCAConversationPC


        /**
         * @see MCAConversationPC
         */
        void AddObserver( MCAConversationObserverPC* aObserver ) ;

        /**
         * @see MCAConversationPC
         */
        void RemoveObserver() ;


        /**
         * @see MCAConversationPC
         */
        void DeleteChatL( const TDesC& aServerAddress,
                          const TDesC& aUserId,
                          const TDesC& aTargetId ) ;


        /**
         * @see MCAConversationPC
         */
        void InitialiseResourcesL( const TDesC& aContactId ) ;

        /**
         * @see MCAConversationPC
         */
        void ReleaseResources() ;


        /**
         * @see MCAConversationPC
         */
        const TPtrC GetIdentification( const TDesC& aContactId ) ;


        /**
         * @see MCAConversationPC
         */
        const TDesC& GetReadInterfaceId()	;


        /**
         * @see MCAConversationPC
         */
        void GetLogFileName( TPtr& aLogFileNamePtr ) ;

        /**
         * @see MCAConversationPC
         */
        void RecordChatL( const TDesC& aLogFilename ) ;


        /**
         * @see MCAConversationPC
         */
        void SendMessageL( const TDesC& aMessageBuffer ) ;

        /**
         * @see MCAConversationPC
         */
        MCAServerContactsArrayPC* PopulateGroupDetailsLC() ;


        /**
         * @see MCAConversationPC
         */
        TPtrC GetReadInterfaceName() ;

        /**
         * @see MCAConversationPC
         */
        void SetActiveConversationL( const TDesC& aWvId ) ;

        /**
         * @see MCAConversationPC
         */
        TBool FindAnyContact( const TDesC& aContactId ) ;

        /**
         * @see MCAConversationPC
         */
        TInt GroupCount( TBool aOnlyVisible );

        /**
         * @see MCAConversationPC
         */
        TInt ContactCount();

        /**
         * @see MCAConversationPC
         */
        void CheckReply( TPtrC& aWvid, TBool& aContactInlist );


        /**
         * @see MCAConversationPC
         */
        TBool IsActiveConversation( const TDesC& aContactId );

        /**
         * @see MCAConversationPC
         */
        void SetMessageReadInterfaceL( const TDesC& aWvId );

        /**
         * @see MCAConversationPC
         */
        void SetMessageWriteInterfaceL();

        /**
         * @see MCAConversationPC
         */
        void CloseConversations();

        /**
         * @see MCAConversationPC
         */
        void LeaveJoinedGroupsL();

        /**
         * @see MCAConversationPC
         */
        TInt MessageCount();


    public: // From MCAStoredContactsObserver

        /**
         * @see MCAStoredContactsObserver
         */

        void HandleContactDelete( const TDesC& aContactId ) ;

        /**
         * @see MCAStoredContactsObserver
         */
        void HandleAddition( MCAContactList& aList, MCAStoredContact& aContact ) ;

        /**
         * @see MCAStoredContactsObserver
         */
        void HandleChange( MCAContactList* aList, MCAStoredContact* aContact,
                           TStorageManagerGlobals::TCAObserverEventType aEventType,
                           TBool aUserIdChanged ) ;


    private: // From MCAChatObserver

        /**
         * @see MCAChatObserver
         */

        void HandleChatEvent( TChatEventType aEvent, MCAMessage* aMessage = NULL ) ;


    private: // From MCAMessageErrorObserver
        /**
         * @see MCAMessageErrorObserver
         */
        void HandleMessageError( TInt aError, MCAMessage* aMessage ) ;


    public: //From MCAMessageRWInterfacePC
        /**
         * @see MCAMessageRWInterfacePC
         */
        MCAMessagesReadInterface& ReadInterfaceL() const;

        /**
         * @see MCAMessageRWInterfacePC
         */
        MCAMessagesWriteInterface& WriteInterfaceL() const;


    private:

        void ConstructL();


    public:
        /**
        * Two-phased constructor.
        * @param aEngine: Reference to chat engine component
        */
        static CCAConversationPC* NewL( CCAEngine& aEngine );
        /**
         * Destructor
         */
        ~CCAConversationPC();

    private:
        /**
        * Default constructor.
        * @param aEngine: Reference to chat engine component
        */
        CCAConversationPC( CCAEngine& aEngine );

        struct TArrayPairs // Struct to hold identification and buffer
            {
            TBuf<KMaxWVIDLength> iIdentification;
            MCAMessagesReadInterface* iReadInterface;
            };


    private: //Data

        //Chat interface
        MCAChatInterface* iChatInterface;

        //Doesn't own. Handle to contacts interface
        MCAStoredContacts* iContacts;

        //Doesn't own
        MCAStoredGroups* iGroups;

        //Doesn't own
        MCAConversationObserverPC* iObserver;

        //Doesnt Own,Ref to chat engine
        CCAEngine& iEngine;

        //Owns. Contact id
        HBufC* iContactId;

        // Interface for reading messages
        MCAMessagesReadInterface* iReadInterface;

        // Interface for writing messages
        MCAMessagesWriteInterface* iWriteInterface;

        MCAMessageContainerInfo* iMsgContainerInfo;

    };

#endif      // CCACONVERSATIONPC_H

// End of File
