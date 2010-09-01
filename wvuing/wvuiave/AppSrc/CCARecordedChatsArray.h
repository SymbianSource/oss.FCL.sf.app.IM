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
* Description:  Array wrapper for recorded chats view
*
*/



#ifndef CCARECORDEDCHATSARRAY_H__
#define CCARECORDEDCHATSARRAY_H__

//  INCLUDES
#include <e32base.h>
#include "BAMDESCA.H"
#include "MCARecordedChatProvider.h"

// FORWARD DECLARATIONS
class MCALoggerMessageHeader;
class RWriteStream;
class MCARecordedChatsArrayPC;
class MCARecordedChatsPC;

// CLASS DECLARATION

/**
*  Recorded chats header array wrapper
*  Wraps real array handling to MDesCArray protocol
*
*  @chatng.exe
*  @since 3.2
*/
class CCARecordedChatsArray :   public CBase,
            public MDesCArray,
            public MCARecordedChatProvider
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aRecordedChatsPC Reference to Recorded Chats process component
        * @param aRecordedChatsArrayPC Reference to Recorded Chats Array process component
        */
        static CCARecordedChatsArray* NewL( MCARecordedChatsPC& aRecordedChatsPC,
                                            MCARecordedChatsArrayPC& aRecordedChatsArrayPC );

        /**
        * Destructor.
        */
        virtual ~CCARecordedChatsArray();

    public: // New Methods

        /**
        * Recorded chat index by name.
        * @since S60 v3.2
        * @param aName Name of recorded chat.
        * @return Index of chat or KErrNotFound if
        *         it was not found.
        */
        TInt Index( const TDesC& aName ) const;



        /**
         * Returns the logger message header from given index
         * @param aIndex Item index
         * @return Logger message header
         */
        MCALoggerMessageHeader& LoggerMessageHeader( TInt aIndex ) const;

        /**
         *	Return name for history file found from index
         *  @param aIndex index to search
         */
        const TDesC& Name( TInt aIndex ) const;

        /**
         *	Delete history files. If there is marked items those are deleted.
         *	Otherwise item which index is given is deleted.
         *  @param aIndex Item index
         *  @return ETrue, if deletion is ok to start, EFalse if not.
         */
        TBool DeleteHistoryFilesL( TInt aIndex );

        /**
         *	Delete history file from local delete index
         *  @return ETrue, there is items to delete,
         *          EFalse if deletion is ready.
         */
        TBool DeleteHistoryFileL();

        /**
         *	Is current chat ordered to be deleted
         *  @return ETrue, if is, EFalse if not.
         */
        TBool CurrentChatDeleteStatus();

        /**
         *	Set curren item index. This must be set before messages are opened
         *  to Recorded Chats view.
         *  @param aIndex Item index
         */
        void SetCurrentItemIndex( TInt aIndex );

        /**
         *	Method is used to launch MTM editor with given targetUid
         *  @param aTargetUid id for MTM editor to be launched.
         *  @param aIndex Message index which to send.
         */
        //Code Scanner warning to be ignored
        TInt SendMTML( TInt aIndex, TUid aTargetUid = KNullUid );

        /**
         *	Progress count for progress of deletion.
         *  @return const reference to marked count for progress of deletion.
         */
        TInt ProgressCount() const;

        /**
         * Prepares array for sending MTMs.
         */
        void PrepareArrayForSendingMtmL();


        /**
         *	Return current item index
         *  @return current item index
         */
        TInt CurrentItemIndex() const;

        /**
         *  Initialize/unitialize selected items array pointer
         *  @param aArray Pointer to selected items array.
         */
        void SetSelectedItems( const CArrayFix< TInt >* aArray );

    public: // Functions from base classes (MDesCArray)

        /**
         *  Count of items in array.
         *  @return the number of descriptor elements in
         *  a descriptor array.
         */
        TInt MdcaCount() const;

        /**
         * Indexes into a descriptor array.
         * @param aIndex The position of the descriptor element within a
         * descriptor array
         * @return A 16 bit non-modifiable pointer descriptor representing
         * the descriptor element located at position aIndex within a
         * descriptor array.
         */
        TPtrC16 MdcaPoint( TInt aIndex ) const;

    public: //Methods from MCARecordedChatProvider

        /**
         *  Recorded chat name.
         *  @return name of recorded chat.
         */
        const TDesC& RecordedChatName() const;

        /**
         *  End time of recorded chat.
         *	@return end time of recorded chat.
         */
        TTime RecordedEndDate() const;

        /**
         *  Set current chat to be deleted.
         *  @return ETrue if deletion is ok to continue, EFalse if not.
         */
        TBool DeleteChatL();

        /**
         *	Send current chat via MTM
         *  @param aTargetUid id of MTM to use for sending.
         */
        void SendChatViaMTML( TUid aTargetUid = KNullUid );

        /**
         * @see MCARecordedChatProvider
         */
        void DynInitSendMenuL( CEikMenuPane& aMenuPane );

        /**
         * @see MCARecordedChatProvider
         */
        void DeleteRecordedChatContainerL();

    private:

        /**
         * Show confirmation note for deletion
         * @param aIndex Index of current chat to be deleted
         * @return ETrue if deletion is ok, EFalse if not.
         */
        TBool ShowConfirmationNoteL( TInt aIndex );

        /**
         * Check filename for MTM messages and correct
         * it to unique if it is not.
         * @param aFilename Proposed filename and corrected after checking.
         * @param aArray Array of already existing filenames.
         */
        void CheckAndCorrectMtmFilename( TFileName& aFilename,
                                         MDesCArray& aArray );

        /**
         *  Initialize array for deletion. Sort is from back to start.
         */
        void InitDeleteArrayL();

    private:

        /**
         * C++ default constructor.
         * @param aRecordedChatsPC Reference to Recorded Chats process component
         * @param aRecordedChatsArrayPC Reference to Recorded Chats Array process component
         */

        CCARecordedChatsArray( MCARecordedChatsPC& aRecordedChatsPC,
                               MCARecordedChatsArrayPC& aRecordedChatsArrayPC );

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

    private:    // Data

        // Current chat deletion or not
        TBool iDeleteCurrentChat;

        // Read access to recorded chats PC.
        MCARecordedChatsPC& iRecordedChatsPC;

        // Read access to recorded chats array PC.
        MCARecordedChatsArrayPC& iRecordedChatsArrayPC;
    };

#endif      // CCARECORDEDCHATSARRAY_H__

// End of File
