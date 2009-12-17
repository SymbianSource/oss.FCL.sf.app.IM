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
* Description:  Offers information regarding a given header to the UI.
*
*/


#ifndef __MCARECORDEDCHATSPC_H__
#define __MCARECORDEDCHATSPC_H__

// INCLUDES
#include    "PublicEngineDefinitions.h"



#include    <e32std.h>
#include    <e32base.h>
#include    <bamdesca.h> // MDesCArray
#include    <badesca.h> // CDesCArray
// FORWARD DECLARATIONS
class RWriteStream;
class MCAMessagesReadInterface;
class MDesCArray;
class MCAServerContactsArrayPC;

// CLASS DECLARATION

/**
*  Interface class for UI->PC communication ( UI retrieves information ).
*  Offers information regarding a given header to the UI.
*
*  @lib wvuiprocessng.lib
*  @since 3.2
*/
class MCARecordedChatsPC
    {
    public: // New functions


        /**
        * Delete one history file recognized by index.
        * @return true if the file has been deleted else return false
        */
        virtual TBool DeleteHistoryFileL( ) = 0;


        /**
        * Deletes message container of current recorded chat
        * from engine. Used to free memory when exiting current
        * recorded chat view i.e. called from DoDeactivate method
        * of view class.
        */
        virtual void DeleteRecordedChatContainerL() = 0;

        /**
        * Initialize/unitialize selected items array pointer
        * @param aArray Pointer to selected items array.
        */
        virtual void SetSelectedItems( const CArrayFix< TInt >* aArray ) = 0;

        /**
        * Return whether there are selected items
        * @param true if there is atleast one valid item selected else return false
        */
        virtual TBool ValidSelectedItems() const = 0;

        /**
        * return the number of selected items
        * @Return an integer: count of selected items
        */
        virtual TInt SelectedItemsCount() const = 0;

        /**
        * Prepares array for sending MTMs.
        */
        virtual void PrepareArrayForSendingMtmL() = 0;

        /**
        * Is index item marked or not
        * @param aIndex Item which marked status is returned
        * @return ETrue if item is marked, EFalse if not.
        */
        virtual TBool IsMarked( TInt aIndex ) = 0;

        /**
        * Initialize array for deletion. Sort is from back to start.
        */
        virtual void InitDeleteArrayL() = 0;

        /**
        * Method is used to launch MTM editor with given targetUid
        * @param aTargetUid id for MTM editor to be launched.
        * @param aIndex Message index which to send.
        */
        // Code Scanner warning to be ignored
        virtual TInt SendMTML( TInt aIndex, TUid aTargetUid /*= KNullUid*/ ) = 0;

        /**
         * Delete history files. If there is marked items those are deleted.
         * Otherwise item which index is given is deleted.
         * @param aIndex Item index
         * @return ETrue, if deletion is ok to start, EFalse if not.
         */
        virtual void DeleteHistoryFilesL( TInt aIndex, TBool aDeleteCurrentChat ) = 0;

        /**
        * Check filename for MTM messages and correct
        * it to unique if it is not.
        * @param aFilename Proposed filename and corrected after checking.
        * @param aArray Array of already existing filenames.
        */
        virtual void CheckAndCorrectMtmFilename( TFileName& aFilename,
                                                 MDesCArray& aArray ) = 0;


        /**
        * Get the number of stored contacts in all the lists
        * @return integer: number of stored contacts
        */
        virtual TInt StoredContactsCount() const = 0;

        /**
        * To return the number of groups
        * @return integer: count of groups
        */
        virtual TInt GroupCountL( TBool aGroupsSupported ) const = 0;

        /**
        * To populate the two arrays of group names and goup ids
        * @return - Fills and returns groupname along with the ID
        * Ownership is transferred to the caller
        */
        virtual MCAServerContactsArrayPC* GetPairArrayForServerContactsArrayLC() = 0;

        //fix - refer Ui Spec Approved Version 1.0 (Instant Messaging NG 001 151006.pdf)
        //Section 10.2.10 Pg 131 -
        //"In case user has saved the image already or sent it by himself,
        // this option(save) is not available."
        /**
         * ReLoggingL - Relogs the messages to the file
         * @since series 60  v3.2
         * @return void
         */
        virtual void ReLoggingL() = 0;


        /**
         * Reset EngineLogger - Relogs the messages to the file
         * @since series 60  v3.2
         * @return void
         */
        virtual void ResetEngineLoggerL() = 0;

    public:

        /**
         *	Destructor
         */
        virtual ~MCARecordedChatsPC() {}
    };

#endif      // __MCARECORDEDCHATSPC_H__

// End of File
