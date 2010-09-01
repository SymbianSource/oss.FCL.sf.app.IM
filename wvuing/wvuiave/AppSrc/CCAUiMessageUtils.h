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
* Description:  UI utils for message handling
*
*/



#ifndef CCAUIMESSAGEUTILS_H
#define CCAUIMESSAGEUTILS_H

//  INCLUDES
#include    <e32base.h>
#include    <MMGFetchVerifier.h>
#include    <apgcli.h>             // RApaLsSession
#include    <aknserverapp.h>       // MAknServerAppExitObserver
#include    <NewFileService.hrh>   // TNewServiceFileType


// FORWARD DECLARATIONS
class MCAConversationMessage;
class CDocumentHandler;
class CImageDecoder;
class MCAConversationPC;
class MCABlockChecker;
class MCAGroupPC;


// CLASS DECLARATION

/**
*  UI utils for message handling
*
*  @lib chat.exe
*  @since Series 60 3.2
*/
class CCAUiMessageUtils : public CActive,
            public MMGFetchVerifier,
            public MAknServerAppExitObserver
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCAUiMessageUtils* NewLC( MCABlockChecker* aBlockChecker = NULL );

        /**
         * Destructor.
         */
        virtual ~CCAUiMessageUtils();

    public: // New functions

        /**
         * Send new file
         */
        void SendNewFileL( TNewServiceFileType aFileType,
                           MCAConversationPC& aMessageRWInterfacePC,
                           const TDesC& aSender = KNullDesC,
                           const MDesCArray* aScreenNames = NULL,
                           TBool aIsWhisperAllowed = ETrue );

        /**
         * Send image
         */
        void SendImageL(
            MCAConversationPC& aMessageRWInterfacePC,
            const TDesC& aSender = KNullDesC,
            const MDesCArray* aScreenNames = NULL,
            TBool aIsWhisperAllowed = ETrue );

        /**
         * Send files
         */
        void SendFileL(  const MDesCArray& aFileNames,
                         MCAConversationPC& aMessageRWInterfacePC,
                         const TDesC& aSender = KNullDesC,
                         const MDesCArray* aScreenNames = NULL );

        /**
         * Send new file
         */
        void SendNewFileL( TNewServiceFileType aFileType,
                           MCAGroupPC& aMessageRWInterfacePC,
                           const TDesC& aSender = KNullDesC,
                           const MDesCArray* aScreenNames = NULL,
                           TBool aIsWhisperAllowed = ETrue );

        /**
         * Send image
         */
        void SendImageL(
            MCAGroupPC& aMessageRWInterfacePC,
            const TDesC& aSender = KNullDesC,
            const MDesCArray* aScreenNames = NULL,
            TBool aIsWhisperAllowed = ETrue );

        /**
         * Send files
         */
        void SendFileL(  const MDesCArray& aFileNames,
                         MCAGroupPC& aMessageRWInterfacePC,
                         const TDesC& aSender = KNullDesC,
                         const MDesCArray* aScreenNames = NULL );


        /**
         * Open object
         * @param aMessage Reference message with Image content
         */
        void OpenObjectL( MCAConversationMessage& aMsg );

        /**
         * Save object
         * @param aMessage Reference message with Image content
         * @return   KErrNone if success. KUserCancel if the user
         *           cancelled the operation.
         */
        TInt SaveObjectL( MCAConversationMessage& aMsg );

        /**
        * Forwards a content message to recipient(s)
        * @param aMessage         Reference message from which the content is
        *                         taken and forwarded
        * @param aRecipient       Recipient for the forwarded message
        * @param aScreenNames     Names of the recipients (if whispering is
        *                         allowed)
        * @param aMsgCreator      Message creator
        * @param aWriteInterface  Interface to which the forwarded message is
        *                         to be written.
        * @param aIsWhisperingAllowed ETrue if whispering is allowed.
        * @param aSender          Sender of the message
        * @return ETrue if message was handled, EFalse otherwise.
        */
        TBool ForwardContentMessageL(
            const MCAConversationMessage& aMessage,
            const TDesC& aRecipient,
            const MDesCArray* aScreenNames,
            MCAConversationPC& aMessageRWInterfacePC,
            TBool aIsWhisperingAllowed,
            const TDesC& aSender = KNullDesC );
        /**
         * Forwards a content message to recipient(s)
         * @param aMessage         Reference message from which the content is
         *                         taken and forwarded
         * @param aRecipient       Recipient for the forwarded message
         * @param aScreenNames     Names of the recipients (if whispering is
         *                         allowed)
         * @param aMsgCreator      Message creator
         * @param aWriteInterface  Interface to which the forwarded message is
         *                         to be written.
         * @param aIsWhisperingAllowed ETrue if whispering is allowed.
         * @param aSender          Sender of the message
         * @return ETrue if message was handled, EFalse otherwise.
         */
        TBool ForwardContentMessageL(
            const MCAConversationMessage& aMessage,
            const TDesC& aRecipient,
            const MDesCArray* aScreenNames,
            MCAGroupPC& aMessageRWInterfacePC,
            TBool aIsWhisperingAllowed,
            const TDesC& aSender = KNullDesC );

    protected:  // Functions from base classes

        /**
         * @see MAknServerAppExitObserver
         * @since 3.2
         */
        void HandleServerAppExit( TInt aReason );

        /**
         * @see MMGFetcVerifier
         */
        TBool VerifySelectionL( const MDesCArray* aSelectedFiles );

    protected: // From CActive

        /**
         * @see CActive
         */
        void RunL();

        /**
         * @see CActive
         */
        void DoCancel();

        /**
         * @see CActive.
         */
        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CCAUiMessageUtils( MCABlockChecker* aBlockChecker );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
         * Select recipients as image or file recipients.
         * @param aScreenNames The screen names within the group
         * @return ETrue if accepted, EFalse if cancelled
         * @since 3.2
         */
        TBool SelectRecipientsL( const MDesCArray* aScreenNames,
                                 TBool aIsWhisperAllowed = ETrue );



        /**
         * Does the actual file sending. The recipients,
         * if any, should already be in the iSelectedNames.
         * @see SendFileL
         * @since 3.2
         */
        void DoSendFileL(
            const MDesCArray& aFileNames,
            MCAConversationPC& aMsgRWInterface,
            const TDesC& aSender = KNullDesC );
        /**
         * Does the actual file sending. The recipients,
         * if any, should already be in the iSelectedNames.
         * @see SendFileL
         * @since 3.2
         */
        void DoSendFileL(
            const MDesCArray& aFileNames,
            MCAGroupPC& aMsgRWInterface,
            const TDesC& aSender = KNullDesC );

        /**
         * Retrieves the optimal decode size (as small as possible)
         * for validating the image
         */
        TSize DecodeSize( const TSize& aSize );

        /**
         * set Navigation Pane to be dimmed.
         */
        void SetNavigationPaneDimmed( TBool aDimmed );

        /**
         * check image file type before it will be sent.
         */
        TBool CheckFileL( const MDesCArray& aFileNames, TDataType& aDataType, TPtrC8& aMimeType, TBool& aSendOption );

    private:    // Data

        // Exit reason for server apps
        TInt iServerAppExitReason;

        // Activescheduler wait
        CActiveSchedulerWait iWait;

        // apa session
        RApaLsSession iApaSession;

        // owns, document handler
        CDocumentHandler* iDocHandler;

        // owns, selected screen names
        CDesCArray* iSelectedNames;

        // owns
        CImageDecoder* iDecoder;

        // Not owned - used to check if current recipient is blocked.
        MCABlockChecker* iBlockChecker;

        // the flat to indicate that VerifySelectionL() should be run or not.
        TBool iBeginVerifySelection;

    };

#endif      // CCAUIMESSAGEUTILS_H

// End of File
