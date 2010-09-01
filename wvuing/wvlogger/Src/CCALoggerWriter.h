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
* Description:  History file writer class
*
*/


#ifndef __CCALOGGERWRITER_H__
#define __CCALOGGERWRITER_H__

//  INCLUDES

#include <f32file.h>
#include <s32file.h>

#include "MCALoggerWriteInterface.h"

// FORWARD DECLARATIONS

class MCALoggerWriteObserver;

// CLASS DECLARATION

/**
 *  History file writer class
 *
 *  @lib CALogger.dll
 *  @since 2.1
 */
class CCALoggerWriter : public CBase, public MCALoggerWriteInterface
    {
    public:

        /**
         * Destructor.
         * If logging is still on. Ends it.
         */
        virtual ~CCALoggerWriter();

        /**
         *	Two-phased constructor.
         *	Leaves if creation of CCALoggerWriter fails.
         *	Leaves if constructL fails.
         *	@param aObserver Write observer that will be informed about write
         *					situations like file add and remove.
         *	@param aFileSession Open file session for file server utilities.
         *	@return Instance to CCALoggerWriter
         */
        static CCALoggerWriter* NewL(	MCALoggerWriteObserver& aObserver,
                                      RFs& aFileSession );

    protected:

        /**
         * C++ default constructor.
         */
        CCALoggerWriter( MCALoggerWriteObserver& aObserver, RFs& aFileSession );

    public: // Inherited from MCALoggerWriteInterface.
        /**
         * Starts logging. Sets logging on. Informs observer to addfile.
         *	Leaves with KErrArgument if header is NULL.
         *	Leaves with KErrInUse if loggins is already on.
         *	Leaves if MCALoggerWriteObserver::CreateNewFilenameL fails.
         *	Leaves if MCALoggerMessageHeader::SetFilenameL fails.
         *	Leaves if MCALoggerWriteObserver::AddFileL fails.
         *	Leaves if file creation fails.
         * @since 2.1
         * @param aFile Header information.
         */
        void StartLoggingL( MCALoggerMessageHeader* aFile );

        /**
         *	Ends logging. Close the file. Informs observer to remove file.
         *	Sets logging off.
         *	Leaves if MCALoggerWriteObserver::RemoveFileL fails.
         *	@since 2.1
         */
        void EndLoggingL();

        /**
         *	Checks if logging is on.
         *	@since 2.1
         *	@return ETrue if logging is on. EFalse if logging is off.
         */
        TBool IsLoggingOn();

        /**
         *	Logs message. Logs message if there is enough memory.
         *	Leaves if memory check fails with KErrDiskFull
         *	Leaves if logging is not on with KErrNotReady
         *	Leaves if logging fails.
         *	@since 2.1
         *	@param aMessage Message information.
         */
        void MakeLogL( MCALoggerMessage& aMessage );


    public:

        //fix - refer Ui Spec Approved Version 1.0
        //(Instant Messaging NG 001 151006.pdf)
        //Section 10.2.10 Pg 131 -
        //"In case user has saved the image already or sent it
        //by himself, this option(save) is not available."

        /**
         * Note : Please dont delete while merging the wvui20 changes
         * Relogs the file and saves all the messages
         * only if the file has become dirty.
         * New implementation - Used if user saves an image
         * in saved conversations that info is stored back on
         * to the file and subsequently we dont allow resaving of the
         * same image  */

        /**
         * Starts relogging. Sets logging on.
         *	Leaves with KErrArgument if header is NULL.
         *	Leaves with KErrInUse if loggins is already on.
         *	Leaves if file replacing of file fails.
         * @since 3.2
         * @param aFile Header information.
         */
        void ReLoggingL( MCALoggerMessageHeader& aFile );

        /**
         *	Ends Relogging. Close the file.
         *	Sets logging off.
         *	@since 3.2
         */
        void EndReLogging();

    private: //Data

        /**
         * Observer for writer.
         */
        MCALoggerWriteObserver& iManager;

        /**
         *	Flag which tells status of log. ETrue, logging is on. EFalse off.
         */
        TBool iLoggingOn;

        // handle to the file server
        RFs& iFileSession;

        // File stream handle
        RFileWriteStream iFileStream;
    };

#endif      // __CCALOGGERWRITER_H__

// End of File
