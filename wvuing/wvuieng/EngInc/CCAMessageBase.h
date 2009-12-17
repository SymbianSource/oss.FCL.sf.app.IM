/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message Base class
*
*/


#ifndef CCAMESSAGEBASE_H
#define CCAMESSAGEBASE_H

#include <e32base.h>
#include <badesca.h>

#include "MCAMessage.h"
#include "MCALoggerMessage.h"
#include "MCAMessageCreator.h"

//	FORWARD CLASS DECLERATIONS
class MCAMessageContainerInfo;

// Definitions
// Version 0: Initial version
// Version 1: Added TSystemMessage type. See InternalizeL.
const TUint16 KVersionNum( 1 );

// CLASS DECLARATION

/**
 *  Message Base class
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class CCAMessageBase : public CBase,
            public MCAMessage,
            public MCALoggerMessage
    {
    public: // Helper definitions

        struct SProcessorObserverPair
            {
            MCAContentProcessor* iProcessor;
            MCAContentProcessObserver* iObserver;
            };

    public: // Construction

        /**
         * Destruction
         */
        ~CCAMessageBase();

    protected: // Construction

        /**
         * Constructor
         */
        CCAMessageBase();

        /**
         * Remember to call this one as last line of child ConstructL
         */
        void ConstructL(    TInt aOpCode,
                            const TDesC& aSender,
                            const TDesC& aRecipient,
                            const MDesCArray* aRecipients,
                            const MDesCArray* aScreenNames );

    protected: // From MCAMessage

        /**
         * @see MCAMessage
         */
        const TTime& TimeStamp() const;

        /**
         * @see MCAMessage
         */
        TMessageType MessageType() const;

        /**
         * @see MCAMessage
         */
        TMessagerType MessagerType() const;

        /**
         * @see MCAMessage
         */
        TInt OperationCode() const;

        /**
         * @see MCAMessage
         */
        void SetOperationCode( TInt aOpId );

        /**
         * @see MCAMessage
         */
        const TDesC& Recipient() const;

        /**
         * @see MCAMessage
         */
        const TDesC& Sender() const;

        /**
         * @see MCAMessage
         */
        const MDesCArray* Recipients() const;

        /**
         * @see MCAMessage
         */
        const MDesCArray* ScreenNames() const;

        /**
         * @see MCAMessage
         */
        void IncreaseOwnerCount();

        /**
         * @see MCAMessage
         */
        TBool DecreaseOwnerCount();

        /**
         * @see MCAMessage
         */
        const TDesC& Text() const;

        /**
         * @see MCAMessage
         */
        const TDesC& ServerAddress() const;

        /**
         * @see MCAMessage
         */
        const TDesC& UserId() const;

        /**
         * @see MCAMessage
         */
        const TDesC& Target() const;

        /**
         * @see MCAMessage
         */
        void SetContainerInfo( MCAMessageContainerInfo* aInfo );

        /**
         * @see MCAMessage
         */
        MCALoggerMessage& LoggerMessage();

        /**
         * @see MCAMessage
         */
        TInt AddContentProcessor( MCAContentProcessor& aProcessor,
                                  MCAContentProcessObserver* aObserver );

        /**
         * @see MCAMessage
         */
        void HandleProcessingComplete( MCAContentProcessor& aProcessor,
                                       TInt aStatus );

        /**
         * @see MCAMessage
         */
        const TDesC8& MimeType() const;

        /**
         * @see MCAMessage
         */
        const TDesC8& ContentData() const;

        /**
         * @see MCAMessage
         */
        void SetContentData( HBufC8* aNewContent );

        /**
         * @see MCAMessage
         */
        TContentProcessState ContentProcessState() const;

        /**
         * @see MCAMessage
         */
        void SetProcessState( TContentProcessState aNewState );

        /**
         * @see MCAMessage
         */
        void ReleaseTemporaryContentData();

        /**
         * @see MCAMessage
         */
        TInt SizeInBytes() const;

        /**
         * @see MCAMessage
         */
        TBool FailedMessage() const;

        /**
         * @see MCAMessage
         */
        void SetForwardPartL( const HBufC* aFwdPart );

        /**
         * @see MCAMessage
         */
        const HBufC* ForwardPart() const;

        /**
         * @see MCAMessage
         */
        TSystemMessage SystemMessageType();


        /**
         * @see MCAMessage
         */
        void SetImageAsSaved( const TBool aSaved );

        /**
         * @see MCAMessage
         */
        TBool IsImageSaved();

    public:

        /**
         * Set default content process observer which is informed always
         * when messages has been changed
         */
        void SetContentProcessObserver( MCAContentProcessObserver* aObserver );

    protected: // From MCALoggerMessage

        /**
         * @see MCALoggerMessage
         */
        void InternalizeL( RReadStream& aStream );

        /**
         * @see MCALoggerMessage
         */
        void ExternalizeL( RWriteStream& aStream ) const;

        /**
         * @see MCALoggerMessage
         */
        TInt MessageSizeInBytesL() const;

    public: // New methods

        /**
         *	Externalize message to MTM send format
         *  @param aStream Stream where message is externalized to.
         *  @param aShowTimeStamps show or hide the timestamps
         *  @since 2.5
         */
        virtual void ExternalizeForMTML( RWriteStream& aStream,
                                         TBool aShowTimeStamps ) const;

        /**
         * Set message type
         * @param aNewType New message type.
         */
        virtual void SetMessageType( TMessageType aNewType );

        /**
         * Set messager type
         * @param aNewType New messager type.
         */
        virtual void SetMessagerType( TMessagerType aNewType );

        /**
         * Set message as system message
         * @param aType Type of system message
         */
        void SetSystem( TSystemMessage aType );

        /**
         * Set version number of message. Version information is used
         * when reading message data from data streams.
         * @param aVersion new version number of message
         */
        void SetVersionNumber( TUint16 aVersion );

    protected:

        /**
         * Set failed message
         */
        virtual void SetFailed( TBool aFailed = ETrue );

        /**
         * Launches waiting processing.
         */
        void LaunchProcessingL();

    private: // New helper methods

        /**
         * Resolves type for message
         */
        void ResolveMessageType( const MDesCArray* aRecipients,
                                 const MDesCArray* iScreenNames );

    protected: // Member variables

        /// Type of message
        TMessageType iMessageType;

        /// Type of messager
        TMessagerType iMessagerType;

        /// Owns Message content type
        HBufC8* iMimeType;

        /// Owns Message sender
        HBufC* iSender;

        /// Owns Recipient of message
        HBufC* iRecipient;

        /// Owns Message content
        HBufC8* iContentData;

        /// Timestamp.
        TTime iTime;

        /// Operation code
        TInt iOperationCode;

        /// Ownercount to count owners.
        TInt iOwnerCount;

        /// Owns. Textual content.
        HBufC* iText;

        /// Does not own. Container information
        MCAMessageContainerInfo* iInfo;

        /// Processors for content
        RArray< SProcessorObserverPair > iProcessors;

        /// Processing state
        TContentProcessState iContentProcessingState;

        /// Default content process observer
        MCAContentProcessObserver* iDefaultContentObserver;

        /// Owns. Recipient list.
        CDesCArrayFlat* iRecipients;

        /// Owns. ScreenNames list.
        CDesCArrayFlat* iScreenNames;

        /// Is message failed message or not.
        TBool iFailed;

        /// Owns. Part of message to be forwarded.
        const HBufC* iForwardPart;

        /// If this is a system message, this defines the type
        TSystemMessage iSystemMessageType;

        // Message's version used in internalizing/externalizing
        TUint16 iMessageVersion;

        //this flag is used if message content is image
        //if the user has chosen to save the image then this value will
        //be ETrue else EFalse
        TBool iSaved;
    };

#endif      // CCAMESSAGEBASE_H

// End of File
