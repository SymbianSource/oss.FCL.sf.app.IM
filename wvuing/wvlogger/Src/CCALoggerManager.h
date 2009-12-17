/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Logger manager with read access to history files.
*
*/


#ifndef __CCALOGGERMANAGER_H__
#define __CCALOGGERMANAGER_H__

//  INCLUDES

#include <f32file.h>
#include <s32file.h>

#include "MCALoggerReadInterface.h"
#include "MCALoggerWriteObserver.h"

// FORWARD DECLARATIONS

class MCALoggerMessageFactory;

// CLASS DECLARATION

/**
 *  Logger manager with read access to history files.
 *
 *  @lib CALogger.dll
 *  @since 2.1
 */
class CCALoggerManager : 	public CBase,
            public MCALoggerReadInterface,
            public MCALoggerWriteObserver
    {
    public:

        /**
         *	Destructor.
         *	Closes filesession. Resets toc and write arrays.
         */
        virtual ~CCALoggerManager();

    protected:

        /**
         * Two-phased constructor. Creates instance to CCALoggerManager
         *	Can leave if there is no enough memory.
         *	Can leave if Dll::SetTls call fails.
         *	@return Instance to CCALoggerManager
         */
        static CCALoggerManager* NewL();

        /**
         * C++ default constructor.
         */
        CCALoggerManager();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:	//new methods

        /**
         *	Get instance of CCALoggerManager
         *	If instance is not yet created, creates instance.
         *	Sets message factor if passed.
         *	Reads toc file if not yet read and if messagefactory is set.
         *	@since 2.1
         *  @param aMessageFactory Factory which is used for message and header
         *							creation.
         *  @param aCreate tells if instance should be created or not if not exists.
         *	@return Singleton instance of CCALoggerManager
         */
        static CCALoggerManager* InstanceL(
            MCALoggerMessageFactory* aMessageFactory,
            TBool aCreate = ETrue , TBool aNotFirstTime = ETrue );

        /**
         *	Get suitable filename. Check situation and updates suitable
         *  filename.
         *	If latest filename is checked before, just add one to current
         *	number. Otherwise check out directory contents and see which
         *	is last file number used.
         *	@since 2.1
         */
        void SuitableLogFileNameL();

        /**
         *	Read header information from toc file and updates toc file
         *	to contain only those headers that points to files that exists.
         *	If message factory does not exists. This call does nothing.
         *	Leaves if some other error than KErrNotFound in file open rutine.
         *	Leaves if error happens when header is tried to append to array.
         *	Leaves if toc filename rename fails
         *	Leaves if leaves if new toc file creation fails.
         *	Leaves if memory check fails with KErrDiskFull
         *	@since 2.1
         */
        void ReadTocFileL( TBool aFirstTime );

        /**
         *	Append new header to toc file. Creates file if not exists.
         *	Leaves if file open and creation fails.
         *	Leaves if memory check fails with KErrDiskFull
         *	@since 2.1
         *	@param aHeader New header information which will be written
         *  @param aTemporary ETrue, and we are saving temporary toc file.
         *                    EFalse and we are saving real toc file.
         */
        void WriteTocFileL( MCALoggerMessageHeader& aHeader,
                            TBool aTemporary = EFalse );

        /**
         *	Read information from message file.
         *	Leaves if message factory is not set with KErrNotReady
         *	Leaves if history file cannot be opened.
         *	Leaves if message append to array fails.
         *	@since 2.1
         *	@param aMessageArray Client provides array for messages read from
         *			file. Messages are appended to array.
         *	@param aKey File to be read is recognized by header. Pointer to
         *				aKey reference is used for task.
         */
        void ReadMessageFileL(
            RPointerArray< MCALoggerMessage >& aMessageArray,
            MCALoggerMessageHeader& aKey );

        /**
         *	Checks if header already exists
         *	Users aHeaders FilenameL method for key to check
         *	@since 2.5
         *	@param aHeader Checked header
         *	@return ETrue if exists, EFalse if not.
         */
        TBool CheckIfHeaderExistsL( MCALoggerMessageHeader& aHeader );

        /**
         *
         */
        void PopulateTocArrayL( RPointerArray< MCALoggerMessageHeader >& aArray,
                                const TDesC& aTocFilename

#ifdef RD_MULTIPLE_DRIVE
                                , TBool aFirstTime
#endif
                              );
        /**
        *    Leave if file operation return code is KErrDiskFull.
        *    @param aErrorCode is file operation return code.
        */
        void LeaveIfDiskFull( TInt aErrorCode );



    public:	//new methods

        /**
         *	Get read interface to history data. Client cannot delete instance.
         *	Leaves if instance call fails.
         *	@since 2.1
         *  @param aMessageFactory Factory which is used for message and header
         *							creation.
         *	@return Pointer to instance of CCALoggerManager
         */
        static MCALoggerReadInterface* ReadInstanceL(
            MCALoggerMessageFactory* aMessageFactory, TBool aFirstTime = EFalse );

        /**
         *	Creation of write instance to history data
         *	Leaves if instance call fails.
         *	Leaves if creation of CCALoggerWriter fails.
         *	@since 2.1
         *  @param aMessageFactory Factory which is used for message and header
         *							creation.
         *	@return Pointer to new created CCALoggerWriter. Ownership is
         *					transferred to the caller.
         */
        static MCALoggerWriteInterface* WriteInstanceL(
            MCALoggerMessageFactory* aMessageFactory );

        /**
         *	Releases instance. This is only for testing purposes.
         *	Sets Dll::Tls to NULL.
         *	Leaves if instance call fails.
         */
        static void ReleaseInstanceL();

    public:	//	Inherited from MCALoggerWriteObserver

        /**
         *	Add file to write buffer. TFileRelation contains two pointer.
         *	MCALoggerMessageHeader and MCALoggerWriteInterface
         *	Ownership of MCALoggerMessageHeader is transfered to this class
         *	Ownership of MCALoggerWriteInterface will stay in caller.
         *	Calls first RemoveFileL with MCALoggerWriteInterface parameter
         *	to be sure that there is no other file in write array for
         *	MCALoggerWriteInterface. Then appends TFileRelation parameter to
         *	array.
         *	Leaves if TPointerArray::Append fails.
         *	@since 2.1
         *	@param iFile File information
         */
        void AddFileL( TFileRelation aFile );

        /**
         *	Remove file from write array and append it to toc array.
         *	After that header is written to toc file.
         *	Leaves if TPointerArray::Append fails.
         *	Leaves if header file write fails.
         *	@since 2.1
         *	@param aWriter File to be removed
         *		   is recognized with file writer pointer.
         */
        void RemoveFileL( MCALoggerWriteInterface& aWriter );

        /**
         *	Creates new filename for writer. Ownership is transfered to caller.
         *	Leaves if HBufC::NewL fails.
         *	Leaves if SuitableLogFileNameL fails.
         *	@since 2.1
         *	@return Pointer to new filename.
         */
        HBufC* CreateNewFilenameL();

#ifdef RD_MULTIPLE_DRIVE

        /**
         *	Creates new filename for writer. Ownership is transfered to caller.
         *	Leaves if HBufC::NewL fails.
         *	Leaves if SuitableLogFileNameL fails.
         *	@since 2.1
         *	@return Pointer to new filename.
         */
        HBufC* CreateNewFilenameMDL();

        /**
         *	Creates new filename for writer. Ownership is transfered to caller.
         *	Leaves if HBufC::NewL fails.
         *	Leaves if SuitableLogFileNameL fails.
         *	@since 2.1
         *	@return Pointer to new filename.
         */
        TBool GetDriveStatusL( const TDriveNumber aDriveNumber );

#endif

        /**
         *	Checks memory situation. Check if there is enough memory to write
         *	file.
         *	Leaves if SysUtil::FFSSPaceGBelowCriticalLevelL fails.
         *	@since 2.1
         *	@param aMemoryRequired Memory required in bytes.
         *	@return ETrue if there is enough memory and EFalse if not.
         */
        TBool CheckMemoryL( TInt aMemoryRequired );

    public:	//Inherited from MCALoggerReadInterface

        /**
         *	Client gets pointer to message header array trough parameter.
         *	@since 2.1
         *	@param aHeaderArray Pointer to message header array is set to this
         *						parameter. Ownership of array does not change.
         */
        void GetLoggerMessageHeaderArray(
            RPointerArray< MCALoggerMessageHeader >*& aHeaderArray );

        /**
         *	Client asks logger to create messages to array concerning one header
         *	Leaves with KErrNotFound if history file does not exists.
         *	Leaves if history file read fails.
         *	@since 2.1
         *	@param aMessageArray Client provides array for messages read from
         *			file. Messages are appended to array.
         *	@param aKey File to be read is recognized by header. Pointer to
         *				aKey reference is used for task.
         */
        void CreateLoggerMessagesL(
            RPointerArray< MCALoggerMessage >& aMessageArray,
            MCALoggerMessageHeader& aKey  );

        /**
         *	Delete one history file recognized by header.
         *	Leaves if MCALoggerMessageHeader::FileNameL fails.
         *	WARNING: header passed will be deleted and removed from header
         *	array. Array is same that client has been given to with
         *	GetLoggerMessageHeaderArray so removal will be seen directly in
         *	client side too.
         *	@since 2.1
         *	@param aKey File to be read is recognized by header. Pointer to
         *				aKey reference is used for task.
         */
        void DeleteHistoryFileL( MCALoggerMessageHeader& aKey );

        /**
         *	Set message factory. Factory is used to create messages and message
         *	headers for logger and client use.
         *	@since 2.1
         *	@param aMessageFactory Pointer to message factory.
         */
        void SetLoggerMessageFactory(
            MCALoggerMessageFactory* aMessageFactory );

    private:

        /**
         *	Reference to message factory for creation messages and files
         *  when needed. Does not own.
         */
        MCALoggerMessageFactory* iMessageFactory;

        /**
         *	List of those files which have been created for writing, but not
         *	yet ready ones.
         */
        RArray< TFileRelation > iWriteFileArray;

        /**
         *	List of those files which have been created for writing, but not
         *	yet ready ones.
         */
        RPointerArray< MCALoggerMessageHeader > iFileTocArray;

        /**
         *	Temp list for toc file. Member variable for leave simpler error management.
         *  eg. leaves.
         */
        RPointerArray< MCALoggerMessageHeader > iFileTempTocArray;

        /**
         *	We have checked latest possible file name number to give FileWriter.
         */
        TBool iWeHaveLatestFileNameNumber;

        /**
         *	Latest file name number to give FileWriter when asked.
         */
        TInt iLatestFileNameNumber;

        /**
         *	File Session for handling files.
         */
        RFs iFileSession;



        /**
         *	Table of contents. Header information of history files.
         */
        TBool iTocFileRead;
    };

#endif      // __CCALOGGERMANAGER_H__

// End of File
