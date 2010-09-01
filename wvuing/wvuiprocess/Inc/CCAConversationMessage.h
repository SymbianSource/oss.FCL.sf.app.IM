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
* Description:  Interface for handling one message
*
*/


#ifndef CCACONVERSATIONMESSAGE_H
#define CCACONVERSATIONMESSAGE_H

#include <e32base.h>
#include <badesca.h>
#include <e32std.h>


#include "MCAConversationMessage.h"

//	FORWARD CLASS DECLERATIONS
class MCAMessage;


class CCAConversationMessage : public CBase,
            public MCAConversationMessage
    {

    public: //from MCAConversationMessage
        /**
         * @see MCAConversationMessage
         */
        const TTime& TimeStamp() const ;

        /**
         * @see MCAConversationMessage
         */
        TEnumsPC::TMessageType MessageType() const ;

        /**
         * @see MCAConversationMessage
         */
        TEnumsPC::TMessagerType MessagerType() const ;

        /**
         * @see MCAConversationMessage
         */
        TEnumsPC::TContentType ContentType() const ;


        /**
         * @see MCAConversationMessage
         */
        TEnumsPC::TSystemMessage SystemMessageType() ;
        /**
         * @see MCAConversationMessage
         */
        const TDesC& Recipient() const ;

        /**
         * @see MCAConversationMessage
         */
        const TDesC& Sender() const ;

        /**
         * @see MCAConversationMessage
         */
        const TDesC& Text() const ;

        /**
         * @see MCAConversationMessage
         */
        const TDesC8& MimeType() const ;

        /**
         * @see MCAConversationMessage
         */
        const TDesC8& ContentData() const ;

        /**
         * @see MCAConversationMessage
         */
        void SetContentData( HBufC8* aContent ) ;

        /**
         * @see MCAConversationMessage
         */
        TEnumsPC::TContentProcessState ContentProcessState() const ;

        /**
         * @see MCAConversationMessage
         */
        void SetProcessState( TEnumsPC::TContentProcessState aNewState ) ;

        /**
         * @see MCAConversationMessage
         */
        void SetForwardPartL( const HBufC* aFwdPart ) ;

        /**
         * @see MCAConversationMessage
         */
        const HBufC* ForwardPart() const ;


        /**
         * @see MCAConversationMessage
         */
        void AddContentProcessorL( TSize aThumbSize ) ;


        /**
         * @see MCAConversationMessage
         */
        void ReleaseTemporaryContentData() ;


        /**
         * @see MCAConversationMessage
         */
        void RemoveProcessingL();


        /**
         * @see MCAConversationMessage
         */
        CFbsBitmap* Thumbnail() const;


        /**
         * @see MCAConversationMessage
         */
        void SetImageAsSaved( const TBool aSaved );


        /**
         * @see MCAConversationMessage
         */
        TBool IsImageSaved();

        /**
         * @see MCAConversationMessage
         */
        TBool FailedMessage() const;


    public:
        /**
         * @see MCAMessage
         * returns the refernce to the message
         */
        MCAMessage& CCAConversationMessage::GetMessage() const;

    public:

        /**
        * NewL Functions.
        * @param aMessage- reference to the message holded by the engine.
        * @param aFwdMessage - ptr to the message. The ownership is transferred to
        *						this object. Pass NULL if u have not created the message.
        *						Pass Valid value if caller created the MCAMessage object
        */
        static CCAConversationMessage* NewL( MCAMessage& aMessage,
                                             MCAMessage* aFwdMessage = NULL );



    private: // Construction

        /**
         * Constructor
         */
        CCAConversationMessage( MCAMessage& aMessage,
                                MCAMessage* aFwdMessage );


    public: // Destructor

        ~CCAConversationMessage();

    private:

        //doesnt own - this is reference to message ownded by engine or
        // iFwdMessage(below) owned by this class.
        MCAMessage& iMessage;

        //owns - forwarded message
        MCAMessage* iFwdMessage;
    };

#endif      // CCACONVERSATIONMESSAGE_H

// End of File

