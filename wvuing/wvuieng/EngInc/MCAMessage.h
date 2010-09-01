/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef MCAMESSAGE_H
#define MCAMESSAGE_H

#include <e32base.h>
#include <badesca.h>

//	FORWARD CLASS DECLERATIONS
class MCAMessageContainerInfo;
class MCALoggerMessage;
class MCAContentProcessor;
class MCAContentProcessObserver;

// MDesCArray must not be fwd declared, or compiling test code will break
// since it's a MACRO. hence the include to badesca.h

// CLASS DECLARATION

/**
 *  Interface for handling one message
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAMessage
    {
    public: // Definitions

        /**
         * Different message types
         */
        enum TMessageType
            {
            EMessagePTOP,
            EMessageWhisper,
            EMessageGroup,
            EMessageSystem,
            EMessageInvalid
            };

        /**
         * System message enumerations.
         */
        enum TSystemMessage
            {
            ESystemMessageNone,
            ESystemMessageWelcome,
            ESystemMessageNewUsers,
            ESystemMessageTopicChanged,
            ESystemMessagePrivatChanged,
            ESystemMessageLeftUsers,
            ESystemMessageDateChange
            };

        /**
         * Direction of message. Coming or going or other.
         */
        enum TMessagerType
            {
            EMessageSent,
            EMessageReceived,
            EMessageOther
            };

        /**
         * Different message content types
         */
        enum TContentType
            {
            EContentText,
            EContentPicture,
            EContentOther,
            EContentInvalid
            };

        /**
         * Content processing states
         */
        enum TContentProcessState
            {
            EContentNotProcessed = 0,
            EContentNotSupported,
            EContentNotSupportedDrm,
            EContentCorrupted,
            EBitmapDecoded,
            EBitmapScaled,
            EContentReady,
            EContentDecoded,
            EThumbnailReady
            };

    public: // General information

        /**
         * Get timestamp when this message has been created.
         * For prioritization of message deletion and timestamping
         * chat messages
         */
        virtual const TTime& TimeStamp() const = 0;

        /**
         * Message size in bytes.
         * @return Size of message in bytes.
         */
        virtual TInt SizeInBytes() const = 0;

    public: // Type of message
        /**
         * Type of message
         * @return Type of message. EMessageGroup, EMessagePersonal,
         *         EMessageSystem
         */
        virtual TMessageType MessageType() const = 0;

        /**
         * Type of messager
         * @return Type of message. EMessageFrom, EMessageTo, EMessageOther
         */
        virtual TMessagerType MessagerType() const = 0;

        /**
         * Type of content of message
         * @return Type of message. EMessageText, EMessagePicture
         */
        virtual TContentType ContentType() const = 0;

        /**
         * Is message failed or not
         * @return ETrue if is, EFalse if not.
         */
        virtual TBool FailedMessage() const = 0;

        /**
         * Returns the type of system message.
         * @return type of system message or
         *         TSystemMessageNone if not system message
         */
        virtual TSystemMessage SystemMessageType() = 0;

    public: // Identification of message
        /**
         * Operation code of message. One way to identify message.
         * @return Operation code of message.
         */
        virtual TInt OperationCode() const = 0;

        /**
         * Set operation code of message.
         * @param aOpId, new operation code for message
         */
        virtual void SetOperationCode( TInt aOpId ) = 0;

        /**
         * Part of container identifier which contains this message
         * @return Server address to identify container which contains
         *         this message
         */
        virtual const TDesC& ServerAddress() const = 0;

        /**
         * Part of container identifier which contains this message
         * @return User id to identify container which contains this message
         */
        virtual const TDesC& UserId() const = 0;

        /**
         * Part of container identifier which contains this message
         * @return Identifier to identify container which contains this message
         */
        virtual const TDesC& Target() const = 0;

        /**
         * Set containerinfo of message. Needed for identifying of message.
         * @param aInfo. ContainerInfo.
         */
        virtual void SetContainerInfo( MCAMessageContainerInfo* aInfo ) = 0;

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
         * Recipients array.
         * @return Array of recipients
         */
        virtual const MDesCArray* Recipients() const = 0;

        /**
         * Screennames array
         */
        virtual const MDesCArray* ScreenNames() const = 0;

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
        virtual TContentProcessState ContentProcessState() const = 0;

        /**
         * Set current processing state.
         * @aNewState New processing state
         */
        virtual void SetProcessState( TContentProcessState aNewState ) = 0;

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

    public: // Message owner managing.

        /**
         * Increase owner count. There can be multiply owners for one message.
         * This method is for increasing the count. Every owner class
         * must use also DecreaseOwnerCount.
         */
        virtual void IncreaseOwnerCount() = 0;

        /**
         * Decrease owner count. There can be multiply owners for one message.
         * This method is for decreasing the count. Every owner class
         * must have called IncreaseOwnerCount before calling this method.
         * @return ETrue if owner can delete message. EFalse if not.
         */
        virtual TBool DecreaseOwnerCount() = 0;

    public: // Log interface of message.

        /**
         * Interface for logger.
         * @return Interface for logger.
         */
        virtual MCALoggerMessage& LoggerMessage() = 0;

    public: // Content processing.

        /**
         * Add content processor for message
         * @param aProcessor Processor for content.
         */
        virtual TInt AddContentProcessor(
            MCAContentProcessor& aProcessor,
            MCAContentProcessObserver* aObserver ) = 0;

        /**
         * Handle processing completion. Called by content processor when
         * processing is ready.
         * @param aProcessor Processor which made processing.
         * @param aStatus, Status of processing. KErrNone -> OK.
         */
        virtual void HandleProcessingComplete( MCAContentProcessor& aProcessor,
                                               TInt aStatus ) = 0;

        /**
         * Release temporary content data
         */
        virtual void ReleaseTemporaryContentData() = 0;


    public:
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

    public: // For deletion of message

        /**
         * Virtual destructor
         */
        virtual ~MCAMessage() {}
    };

#endif      // MCAMESSAGE_H

// End of File

