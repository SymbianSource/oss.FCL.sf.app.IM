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
* Description:  Channel for writer to inform about state changes to observer
*
*/


#ifndef __MCALOGGERWRITEOBSERVER_H__
#define __MCALOGGERWRITEOBSERVER_H__

//INCLUDES

#include "PrivateDefinitions.h"

//	FORWARD CLASS DECLERATIONS

class MCALoggerMessageHeader;
class MCALoggerWriteInterface;

// CLASS DECLARATION

/**
 *  Channel for writer to inform about state changes to observer
 *
 *  @lib CALogger.dll
 *  @since 2.1
 */
class MCALoggerWriteObserver
    {
    public: //Type definitions


    public: // Interface

        /**
         *	Add file to write buffer. Ownership of iFile transfered to observer
         *	@since 2.1
         *	@param iFile File information
         */
        virtual void AddFileL( TFileRelation aFile ) = 0;

        /**
         *	Remove file from write buffer.
         *	@since 2.1
         *	@param aWriter File to be removed is recognized with file writer pointer.
         */
        virtual void RemoveFileL( MCALoggerWriteInterface& aWriter ) = 0;

        /**
         *	Creates new filename for writer. Ownership is transfered to caller.
         *	@since 2.1
         *	@return Pointer to new filename.
         */
        virtual HBufC* CreateNewFilenameL() = 0;

        /**
         *	Checks memory situation. Check if there is enough memory to write
         *	file.
         *	@since 2.1
         *	@param aMemoryRequired Memory required in bytes.
         *	@return ETrue if there is enough memory and EFalse if not.
         */
        virtual TBool CheckMemoryL( TInt aMemoryRequired ) = 0;

    protected:

        /**
         * Destructor.
         */
        virtual ~MCALoggerWriteObserver() {};
    };

#endif      // __MCALOGGERWRITEOBSERVER_H__

// End of File
