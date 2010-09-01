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
* Description:  Process Manager to get process interfaces e.g. login,settings
*
*/

#ifndef  CCARECORDEDCHATSPC_H
#define  CCARECORDEDCHATSPC_H

#include "MCARecordedChatsPC.h"
#include "MCALoggerHeadersInterface.h"
#include "MCAMessageRWInterfacePC.h"

//Forward Declaration
class CIdle;
class CSendUi;
class RWriteStream;
class MCARecordedChatsArrayPC;
class MCAMessagesWriteInterface;
class MCAMessagesReadInterface;
class CEikonEnv;
//Class declaration
class CCARecordedChatsPC: public CBase,
            public MCARecordedChatsPC,
            public MCAMessageRWInterfacePC
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CCARecordedChatsPC* NewL( MCALoggerHeadersInterface& iLoggerHeadersInterface,
                                         MCARecordedChatsArrayPC& aRecordedChatsArrayPC );

    public:

        /**
         *	Destructor
         */
        ~CCARecordedChatsPC();

    public: //From MCARecordedChatsPC

        /**
        * Delete one history file recognized by index.
        * @return true if the file has been deleted else return false
        */
        virtual TBool DeleteHistoryFileL( );


        /**
        * Deletes message container of current recorded chat
        * from engine. Used to free memory when exiting current
        * recorded chat view i.e. called from DoDeactivate method
        * of view class.
        */
        virtual void DeleteRecordedChatContainerL();

        /**
        * Initialize/unitialize selected items array pointer
        * @param aArray Pointer to selected items array.
        */
        virtual void SetSelectedItems( const CArrayFix< TInt >* aArray );

        /**
        * Return whether there are selected items
        * @param true if there is atleast one valid item selected else return false
        */
        virtual TBool ValidSelectedItems() const;

        /**
        * return the number of selected items
        * @Return an integer: count of selected items
        */
        virtual TInt SelectedItemsCount() const;

        /**
        * Prepares array for sending MTMs.
        */
        virtual void PrepareArrayForSendingMtmL();

        /**
        * Is index item marked or not
        * @param aIndex Item which marked status is returned
        * @return ETrue if item is marked, EFalse if not.
        */
        virtual TBool IsMarked( TInt aIndex );

        /**
        * Initialize array for deletion. Sort is from back to start.
        */
        virtual void InitDeleteArrayL();

        /**
        * Method is used to launch MTM editor with given targetUid
        * @param aTargetUid id for MTM editor to be launched.
        * @param aIndex Message index which to send.
        */
        // Code Scanner warning to be ignored
        virtual TInt SendMTML( TInt aIndex, TUid aTargetUid /*= KNullUid*/ );



        /**
        * Delete history files. If there is marked items those are deleted.
        * Otherwise item which index is given is deleted.
        * @param aIndex Item index
        * @return ETrue, if deletion is ok to start, EFalse if not.
        */
        virtual void DeleteHistoryFilesL( TInt aIndex, TBool aDeleteCurrentChat );

        /**
        * Check filename for MTM messages and correct
        * it to unique if it is not.
        * @param aFilename Proposed filename and corrected after checking.
        * @param aArray Array of already existing filenames.
        */
        virtual void CheckAndCorrectMtmFilename( TFileName& aFilename,
                                                 MDesCArray& aArray );

        /**
        * Get the number of stored contacts in all the lists
        * @return integer: number of stored contacts
        */
        virtual TInt StoredContactsCount() const;

        /**
        * To return the number of groups
        * @return integer: count of groups
        */
        virtual TInt GroupCountL( TBool aGroupsSupported ) const;

        /**
        * To populate the two arrays of group names and goup ids
        * @param aGroupNames: array of the current group names
        * @param aGroupIds: array of the current group IDs
        */
        virtual MCAServerContactsArrayPC* GetPairArrayForServerContactsArrayLC();

        /**
        * Get message for sending it via MTM
        * @param aIndex Index of message to send
        * @param aStream Stream where message is written.
        */
        virtual void GetMessageForSendingViaMTML(
            TInt aIndex, RWriteStream& aStream );


        //fix - refer Ui Spec Approved Version 1.0 (Instant Messaging NG 001 151006.pdf)
        //Section 10.2.10 Pg 131 -
        //"In case user has saved the image already or sent it by himself,
        // this option(save) is not available."
        /**
         * ReLoggingL
         * @since series 60  v3.2
         * @return void
         */
        virtual void ReLoggingL();

        /**
         * reset enginer loggers..for reading from different drive....
         * @since series 60  v3.2
         * @return void
         */
        void ResetEngineLoggerL();

    public://From MCAMessageRWInterfacePC
        /*
        * @see MCAMessageRWInterfacePC
        */
        virtual MCAMessagesReadInterface& ReadInterfaceL() const;

        /*
        * @see MCAMessageRWInterfacePC
        */
        virtual MCAMessagesWriteInterface& WriteInterfaceL() const;


    private: //Private Constructors
        /**
        * private default constructor
        * @param aLoggerHeadersInterface: Reference to the logger component
        * @param aRecordedChatsArrayPC: Reference to the Recorded chats
        *								array process component
        */
        CCARecordedChatsPC( MCALoggerHeadersInterface& aLoggerHeadersInterface,
                            MCARecordedChatsArrayPC& aRecordedChatsArrayPC );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private: //Data memebers


        /**
        * Reference to the MCALoggerHeadersInterface in the engine component
        */
        MCALoggerHeadersInterface& iLoggerHeadersInterface;

        /**
        * reference to MCARecordedChatsArrayPC
        */
        MCARecordedChatsArrayPC& iRecordedChatsArrayPC;

        // Owns. For sending messages via MTM
        CSendUi* iSendAppUi;


        // Owns. History temp directory;
        HBufC* iHistoryTempDir;

        // Does not own. Selected items array.
        const CArrayFix< TInt >* iSelectedItems;

        // Owns: Temp array for keeping track of deleted files
        RArray< TInt > iItemsForDeletion;

        CEikonEnv* iEikonEn;

    };

#endif //CCARECORDEDCHATSPC_H

