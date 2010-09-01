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
* Description:  Interface for getting write access to history files
*
*/


#ifndef __MCALOGGERWRITEINTERFACE_H__
#define __MCALOGGERWRITEINTERFACE_H__

//INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS

class MCALoggerMessageHeader;
class MCALoggerMessage;

// CLASS DECLARATION

/**
 *  Interface for getting write access to history files
 *
 *  @lib CALogger.dll
 *  @since 2.1
 */
class MCALoggerWriteInterface
    {
    public:

        /**
         *	Destructor
         */
        virtual ~MCALoggerWriteInterface() {}

    public: // Interface

        /**
         * Starts logging. Sets logging on. Informs observer to addfile.
         *	Leaves with KErrArgument if header is NULL.
         *	Leaves with KErrInUse if loggins is already on.
         *	Leaves if file creation fails.
         * @since 2.1
         * @param aFile Header information.
         */
        virtual void StartLoggingL( MCALoggerMessageHeader* aFile ) = 0;

        /**
         *	Ends logging. Close the file. Informs observer to remove file.
         *	Sets logging off.
         *	@since 2.1
         */
        virtual void EndLoggingL() = 0;

        /**
         *	Checks if logging is on.
         *	@since 2.1
         *	@return ETrue if logging is on. EFalse if logging is off.
         */
        virtual TBool IsLoggingOn() = 0;

        /**
         *	Logs message. Logs message if there is enough memory.
         *	Leaves if logging is not on with KErrNotReady
         *	Leaves if logging fails.
         *	@since 2.1
         *	@param aMessage Message information.
         */
        virtual void MakeLogL( MCALoggerMessage& aMessage ) = 0;


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
         * same image

        /**
         * Starts relogging. Sets logging on.
         *	Leaves with KErrArgument if header is NULL.
         *	Leaves with KErrInUse if loggins is already on.
         *	Leaves if file replacing of file fails.
         * @since 3.2
         * @param aFile Header information.
         */
        virtual void ReLoggingL( MCALoggerMessageHeader& aFile ) = 0;

        /**
         *	Ends Relogging. Close the file.
         *	Sets logging off.
         *	@since 3.2
         */
        virtual void EndReLogging() = 0;


    };

#endif      // __MCALOGGERWRITEINTERFACE_H__

// End of File