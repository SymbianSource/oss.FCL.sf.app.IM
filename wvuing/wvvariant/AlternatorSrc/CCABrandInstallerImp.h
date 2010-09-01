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
* Description:  Implementation for installing branding data
 *
*/



#ifndef CCABRANDINSTALLER_H
#define CCABRANDINSTALLER_H

//  INCLUDES
#include "MCABrandInstaller.h"
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CEikonEnv;
class CFileMan;

// CLASS DECLARATION

/**
 * Implementation for installing branding data.
 *
 * @lib alternator.lib
 * @since S60 3.0
 */
class CCABrandInstallerImp : public MCABrandInstaller, public CBase
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCABrandInstallerImp* NewL();

        static CCABrandInstallerImp* NewLC();

        /**
         * Destructor.
         */
        virtual ~CCABrandInstallerImp();

    public: // New functions

        /**
         * Checks if brand already exists
         * @since Series 60 3.0
         * @param aBrandName Name of brand "server"
         * @return EFalse if not exists else ETrue
         */
        TBool BrandAlreadyExistsL( const TDesC& aBrandName );

        /**
         * Creates import folder if not existing
         */
        void CreateImportFolderL();

        /**
         * Checks if import folder exists
         * @retun EFalse if not existing
         */
        TBool ImportFolderExistsL();

        /**
         * Lists installed brands
         * @param aListOfBrands Array that is filled of list of installed brands
         */
        void ListOfInstalledBrandsL( CDesCArray& aListOfBrands );

        /**
         * Lists uninstalled brands from import folder
         * @param aListOfBrands Array that is filled of list of uninstalled brands
         */
        void ListOfUninstalledBrandsL( CDesCArray& aListOfBrands );

        /**
         * Checks if new brand is available
         * @return ETrue if available
         */
        TBool IsNewBrandAvailableL();

        void InstallNewBrandL();

    public: // Functions from base classes

        /**
         * From MCABrandInstaller
         */
        TBool FolderExists( const TDesC& aFolderPath );

        /**
         * From MCABrandInstaller
         */
        TInt CreateFolder( const TDesC& aFolderPath );

        /**
         * From MCABrandInstaller
         */
        TInt MoveFolder( const TDesC& aSourcePath, const TDesC& aDestFolder );

        /**
         * From MCABrandInstaller
         */
        TInt RemoveFolder( const TDesC& aFolderPath );

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
         * C++ default constructor.
         */
        CCABrandInstallerImp();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:    // Data

        //Handle to eikon environment
        CEikonEnv* iEikEnv;
        //
        CFileMan* iFileMan;

        HBufC* iPrivatePath;
    };

#endif      // CCABRANDINSTALLER_H

// End of File
