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


#ifndef MCACONVERSATIONMESSAGE_H
#define MCACONVERSATIONMESSAGE_H

#include <e32base.h>
#include <badesca.h>

#include "TEnumsPC.h"

//	FORWARD CLASS DECLERATIONS
class CFbsBitmap;


// MDesCArray must not be fwd declared, or compiling test code will break
// since it's a MACRO. hence the include to badesca.h

// CLASS DECLARATION

/**
 *  Interface for handling one message
 *
 *  @lib wvuiprocessng.dll
 *  @since 3.2
 */
class MCAConversationMessage
    {
    public: // Definitions
        /**
         * Get timestamp when this message has been created.
         * For prioritization of message deletion and timestamping
         * chat messages
         */
        virtual const TTime& TimeStamp() const = 0;


    public: // Type of message
        /**
         * Type of message
         * @return Type of message. EMessageGroup, EMessagePersonal,
         *         EMessageSystem
         */
        virtual TEnumsPC::TMessageType MessageType() const = 0;

        /**
         * Type of messager
         * @return Type of message. EMessageFrom, EMessageTo, EMessageOther
         */
        virtual TEnumsPC::TMessagerType MessagerType() const = 0;

        /**
         * Type of content of message
         * @return Type of message. EMessageText, EMessagePicture
         */
        virtual TEnumsPC::TContentType ContentType() const = 0;


        /**
         * Returns the type of system message.
         * @return type of system message or
         *         TSystemMessageNone if not system message
         */
        virtual TEnumsPC::TSystemMessage SystemMessageType() = 0;

    public: // Content of message

        /**
         * Recipient of message.
         * @return Recipient of message. To whom the message is sent.
         */
        virtual const TDesC& Recipient() const = 0;

        /**
         * Sender of message
         * @return Sender of message. To who has sent the message.
         */
        virtual const TDesC& Sender() const = 0;

        /**
         * Textual content of message
         * @return Textual content.
         */
        virtual const TDesC& Text() const = 0;

        /**
         * Mime type of content.
         * @return Mime type of content.
         */
        virtual const TDesC8& MimeType() const = 0;

        /**
         * Content data in decriptor
         * @return Content data in descriptor
         */
        virtual const TDesC8& ContentData() const = 0;

        /**
         * Set content data for message
         * @param aContent. Ownership is transferred.
         */
        virtual void SetContentData( HBufC8* aContent ) = 0;

        /**
         * Current processing state. Might be different for different
         * content types
         * @return Processing state of message.
         */
        virtual TEnumsPC::TContentProcessState ContentProcessState() const = 0;

        /**
         * Set current processing state.
         * @aNewState New processing state
         */
        virtual void SetProcessState( TEnumsPC::TContentProcessState aNewState ) = 0;

        /**
         * Set the part of message to be forwarded,
         * used when forwarding a highlighted item from a message.
         * @param aFwdPart pointer to highlighted item descriptor,
         *                 use NULL to clear forward message part.
         */
        virtual void SetForwardPartL( const HBufC* aFwdPart ) = 0;

        /**
         * Forwarded part of message.
         * @return forwarded part of message or NULL if not set
         */
        virtual const HBufC* ForwardPart() const = 0;


    public: // Content processing.

        /**
         * Add content processor for message
         * @param aThumbSize
         * @ returns void
         */
        virtual void AddContentProcessorL( TSize aThumbSize ) = 0;


        /**
         * Release temporary content data
         */
        virtual void ReleaseTemporaryContentData() = 0;


        /**
         * Request processing remove. If processing is already started. Cancels processing.
         */
        virtual void RemoveProcessingL() = 0;


        /**
         * Get thumbnail bitmap
         * @return Thumbnail. Ownership is not tranferred
         */
        virtual CFbsBitmap* Thumbnail() const = 0;

    public:

        //fix - refer Ui Spec Approved Version 1.0
        //(Instant Messaging NG 001 151006.pdf)
        //Section 10.2.10 Pg 131 -
        //"In case user has saved the image already or sent it
        //by himself, this option(save) is not available."

        //The following API's are used, if message content is a image
        //This is part of new implementation required for wvuing
        //While merging with wvui20, please make sure you preserve
        //this change also.
        /**
         * API used to set the iSaved Flag to ETrue
         * if the image is saved by the user.
         * @param aSaved - ETrue if Image is Saved by the user
         * @return void
         */
        virtual void SetImageAsSaved( const TBool aSaved ) = 0;

        /**
         * API used to check whether the image was
         * SAVED or not.
         * @return TBool - ETrue if the user has already saved the image
         *   else EFalse
         */
        virtual TBool IsImageSaved() = 0;

        /**
         * Is message failed or not
         * @return ETrue if is, EFalse if not.
         */
        virtual TBool FailedMessage() const = 0;

    public: // For deletion of message

        /**
         * Virtual destructor
         */
        virtual ~MCAConversationMessage() {};
    };

#endif      // MCACONVERSATIONMESSAGE_H

// End of File

