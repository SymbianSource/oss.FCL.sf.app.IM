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
* Description:  Factory class for creating logger manager and for getting
*				 interfaces for read and write access to history files.
*
*/


#ifndef __CALOGGERMANAGERFACTORY_H__
#define __CALOGGERMANAGERFACTORY_H__

//  INCLUDES

#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS

class MCALoggerMessageFactory;
class MCALoggerReadInterface;
class MCALoggerWriteInterface;

// CLASS DECLARATION

/**
 *  Logger manager with read access to history files.
 *
 *  @lib CALogger.dll
 *  @since 2.1
 */
class CALoggerManagerFactory // CSI: 51 # Not a C-class although begins with CA
    {

    private:

        /**
         *	Destructor
         */
        ~CALoggerManagerFactory();

        /**
         *	Constructor
         */
        CALoggerManagerFactory();

        /**
         *	Copy constructor
         *	@param aOther Copy source.
         */
        CALoggerManagerFactory( const CALoggerManagerFactory& aOther );

    public:

        /**
         *	Calls CCALoggerManagers ReadInstanceL.
         *	Caller cannot delete the returned pointer.
         *	@since 2.1
         *  @param aMessageFactory Factory which is used for message and header
         *							creation.NULL value is ignored and not set.
         *	@return Read interface to access to history data.
         */
        IMPORT_C static MCALoggerReadInterface* ReadInstanceL(
            MCALoggerMessageFactory* aMessageFactory = NULL, TBool aFirstTime = ETrue );

        /**
         *	Calls CCALoggermanagers WriteInstanceL
         *	Caller must delete the returned pointer,
         *	when it is not needed anymore. Ownership
         *	will be transfered to caller.
         *	@since 2.1
         *  @param aMessageFactory Factory which is used for message and header
         *							creation.NULL value is ignored and not set.
         *	@return Write interface for writing history data.
         */
        IMPORT_C static MCALoggerWriteInterface* WriteInstanceL(
            MCALoggerMessageFactory* aMessageFactory = NULL );

        /**
         *	Release Logger manager instance. This is for testing only.
         *	Calls CCALoggerManagers ReleaseInstanceL;
         *	@since 2.1
         */
        IMPORT_C static void ReleaseInstanceL();
    };

#endif      // __CCALOGGERMANAGERFACTORY_H__

// End of File
