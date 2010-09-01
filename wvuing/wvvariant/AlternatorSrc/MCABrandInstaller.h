/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  An abstract interface for installing branding data
*
*/



#ifndef MCABRANDINSTALLER_H
#define MCABRANDINSTALLER_H

//  INCLUDES

#include <e32base.h>

// CLASS DECLARATION

/**
 *  An abstract interface for installing branding data.
 *
 *  @lib alternator.lib
 *  @since 3.0
 */
class MCABrandInstaller
    {
    public: // New functions

        /**
         * Checks if given folder exists
         * @since 3.0
         * @param aFolderPath Path of folder
         * @return KErrNone if folder exists, KErrNotFound if not found or other system wide error code
         */
        virtual TInt FolderExists( const TDesC& aFolderPath ) = 0;

        /**
         * Creates given folder
         * @since 3.0
         * @param aFolder Name of the created folder
         * @return KErrNone if folder creation was succesfull, else system wide error codes
         */
        virtual TInt CreateFolder( const TDesC& aFolder ) = 0;

        /**
         * Moves source folder to destination folder
         * @since 3.0
         * @param aSourcePath Source folder path
         * @param aDestFolder Destination folder path
         * @return KErrNone if successfull else system wide error codes
         */
        virtual TInt MoveFolder( const TDesC& aSourcePath, const TDesC& aDestFolder ) = 0;

        /**
         * Removes given folder
         * @since 3.0
         * @param aFolderPath Path of the removed folder
         * @return KErrNone if successfull else system wide error codes
         */
        virtual TInt RemoveFolder( const TDesC& aFolderPath ) = 0;

    protected:  // New functions

        virtual ~MCABrandInstaller() {};

    };

#endif      // MCABRANDINSTALLER_H

// End of File
