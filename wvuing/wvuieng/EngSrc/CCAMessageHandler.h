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
* Description:  Handler class for incoming and outgoing messages
*
*/


#ifndef __CCAMESSAGEHANDLER_H__
#define __CCAMESSAGEHANDLER_H__


#include    "ImpsImCli.h"                   // MImpsImHandler2
#include    "MCAMessageObserver.h"
#include    "MCAMessageErrorInformer.h"
#include    "MCAContentProcessObserver.h"
#include    "MCAMessageCreator.h"           // For SMessageData

//	FORWARD CLASS DECLERATIONS
class MCAChatInterface;
class MCAMessageUtils;
class MCAMessagesReadInterface;
class MCAImpsFactory;
class MCAImpsImClient;

// CLASS DECLARATION

/**
 *  Handler class for incoming and outgoing messages
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */

class CCAMessageHandler : public CBase,
            public MImpsImHandler2,
            public MCAMessageObserver,
            public MCAMessageErrorInformer,
            public MCAContentProcessObserver
    {
    public: // Construction

        /**
         * Construction
         * @param aChatInterface Access to message read and write interfaces
         * @param aMessageUtils Access to message utils ( Message creator,
         *                                                processors )
         * @param aImpsFactory Factory for IMPS interface wrappers
         * @return Pointer to new created instance of CCAMessageCreator
         */
        static CCAMessageHandler* NewL( MCAChatInterface& aChatInterface,
                                        MCAMessageUtils& aMessageUtils,
                                        MCAImpsFactory* aIMPSFactory );

        /**
         * Construction
         * @see CCAMessageHandler::NewL
         */
        static CCAMessageHandler* NewLC( MCAChatInterface& aChatInterface,
                                         MCAMessageUtils& aMessageUtils,
                                         MCAImpsFactory* aIMPSFactory );

        /**
         * Destruction
         */
        ~CCAMessageHandler();

    private: // Construction

        /**
         * Constructor
         * @see CCAMessageHandler::NewL
         */
        CCAMessageHandler( MCAChatInterface& aChatInterface,
                           MCAMessageUtils& aMessageUtils,
                           MCAImpsFactory* aIMPSFactory );

        /**
         * Constructor
         */
        void ConstructL();

    private: // From MImpsImHandler

        /**
         * @see MImpsImHandler2
         */
        void HandleNewTextMessageL( TInt aOpId,
                                    const TDesC& aMessageId,
                                    const TDesC& aSender,
                                    const TDesC& aGroupId,
                                    const MDesCArray& aRecipients,
                                    const MDesCArray& aScreenNames,
                                    const TDesC& aText,
                                    TImpsCspIdentifier& aCspId );
        /**
         * @see MImpsImHandler2
         */
        void HandleNewContentMessageL( TInt aOpId,
                                       const TDesC& aMessageId,
                                       const TDesC& aSender,
                                       const TDesC& aGroupId,
                                       const MDesCArray& aRecipients,
                                       const MDesCArray& aScreenNames,
                                       const TDesC& aContentType,
                                       const TDesC8& aContent,
                                       TImpsCspIdentifier& aCspId );
        /**
         * @see MImpsImHandler2
         */
        void HandleSendCompleteL( TInt aOpId,
                                  TBool aDeliveryReportOrdered,
                                  TImpsCspIdentifier& aCspId );
        /**
         * @see MImpsImHandler2
         */
        void HandleDeliveryReportL( const TDesC& aMessageId,
                                    TInt aResult,
                                    const TDesC* aDescription,
                                    TImpsCspIdentifier& aCspId );

    private: // From MCAMessageObserver

        /**
         * @see MCAMessageObserver
         */
        void HandleMessageEvent( TMessageEventType aEvent, TInt aIndex );

    private: // From MCAMessageErrorInformer

        /**
         * @see MCAMessageErrorInformer
         */
        void RegisterObserver( MCAMessageErrorObserver* aObserver );

        /**
         * @see MCAMessageErrorInformer
         */
        void UnregisterObserver( MCAMessageErrorObserver* aObserver );

    private: // From MCAContentProcessObserver

        /**
         * @see MCAContentProcessObserver
         */
        void HandleProcessingComplete(  MCAContentProcessor& aProcessor,
                                        MCAMessage& aMessage,
                                        TInt aStatus );

    public: // New methods

        /**
         * Handle sent message.
         * @param aStatus, Status of operation
         * @param aOperationCode. Operation code to identify message.
         * @param aSuccess ETrue, if message sent ok. EFalse if not.
         */
        void HandleMessageSentL( TInt aStatus, TInt aOperationCode,
                                 TBool aSuccess );

        /**
         * Set state of local echo in group.
         */
        void SetLocalEchoInGroup( TBool aLocalEchoInGroup );

    private: // New helper methods

        /**
         * Callback method for CIdle
         * @param aInstance Instance of CCAMessageHandler
         * @return ETrue, if more messages, EFalse if last one.
         */
        static TInt SendMessage( TAny *aInstance );

        /**
         * Send one message
         * @return ETrue, if more messages, EFalse if last one.
         */
        TInt DoSendMessage();

        /**
         * Handle new message.
         * @param aData. Data of message.
         */
        void HandleNewMessageL( MCAMessageCreator::SMessageData& aData );

        /**
         * Notify observers about
         * @param aStatus, Error code
         * @param aMessage, Message that failed.
         */
        void NotifyMessageError( TInt aStatus, MCAMessage* aMessage );

        /**
         * Get next holding message if exists.
         * @return Next holding message if exists. NULL if not.
         */
        MCAMessage* NextHoldingMessage();

        /**
         * Is holding needed for message. If it is, appended to holding.
         * @param aMessage. Checked message
         * @return ETrue, if appended to holding EFalse if not.
         */
        TBool AppendedToHoldingL( MCAMessage& aMessage );

        /**
         * Send message to server
         * @param aMessage. Message to sent.
         * @return Operation code.
         */
        TInt SendMessageToServerL( MCAMessage& aMessage );

        /**
         * Handle message sent failed. Create failed message and notify user.
         * @param aMessage. Failed message
         * @param aError, Error code for fail.
         */
        void HandleMessageSentFailedL( MCAMessage& aMessage, TInt aError );

        /**
         * Show message to user who sent it.
         * @param aMessage. Sent message to echo.
         */
        void LocalEchoMessageL( MCAMessage& aMessage );

    private: // Member data

        /// does not own. This is used for example to get message creator
        MCAMessageUtils& iMessageUtils;

        /// Does not own. Access to message containers
        MCAChatInterface& iChatInterface;

        /// Does not own. Access to messages in send buffer
        MCAMessagesReadInterface* iSendBuffer;

        // not owned
        MCAImpsFactory* iImpsFactory;

        // not owned
        MCAImpsImClient* iImpsImClient;

        /// Idle for message sending purposes. Owns.
        CIdle* iIdle;

        /// Message error observers
        RPointerArray< MCAMessageErrorObserver > iErrorObservers;

        /// Messages which are processed before sending.
        RPointerArray< MCAMessage > iHoldingMessages;

        /// Is local echo in groups used.
        TBool iLocalEchoInGroup;

        /// If send is active, do not start it again.
        TBool iSendLaunchLock;
    };

#endif      // CCAMESSAGEHANDLER_H

// End of File
