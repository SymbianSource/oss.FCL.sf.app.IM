/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header of concrete implementation class.
*
*/



#ifndef CALTERNATORIMP_H
#define CALTERNATORIMP_H

//  INCLUDES

#include "CAlternator.h"
#include "AlternatorGlobals.h"

// FORWARD DECLARATIONS
class CEikonEnv;
class RWriteStream;
class RReadStream;
class CCABrandInstallerImp;
// CLASS DECLARATION

/**
*  Concrete implementation of CAlternator.h See CAlternator.h.
*  @lib alternator.lib
*  @since 1.2s
*/
class CAlternatorImp : public CAlternator
    {
    public:  // Constructors and destructor


        /**
        * Constructor
        */
        static CAlternatorImp* NewL( const TDesC& aBrandRoot );

        /**
        * Destructor.
        */
        virtual ~CAlternatorImp();

    public: // Functions from base classes


        /**
        * From CAlternator
        */
        const TDesC& CurrentBitmapFileL( const TDesC& aModuleId,
                                         const TDesC& aAlternative,
                                         const TDesC& aBitmapFileName );

        TInt ChangeResourceL( const TDesC& aModuleId,
                              const TDesC& aAlternative,
                              const TDesC& aResFileName,
                              TInt& aResOffset );

        TInt ChangeBitmapFileL( const TDesC& aModuleId,
                                const TDesC& aAlternative,
                                const TDesC& aBitmapFileName );

        void GetCurrentResourceFileL( const TDesC& aModuleId,
                                      const TDesC& aAlternative,
                                      const TDesC& aResFileName,
                                      TDes& aFullPath );

    public: //New Functions

        /**
        * Changes root path of op branding files
        * @since 3.0
        * @param aBrandRoot, new root path
        */
        void SetBrandRoot( const TDesC& aBrandRoot );

    private:

        /**
        * Changes the resource file.
        * @since 1.2s
        * @param aModuleId, dll's (or app) id. For example "IM".
        * @param aAlternative, id for alternative
        *                      (for example operator "Oper1" ) files.
        * @param aResFileName,resource file's name.
        * @param aResOffSet,reference to resource offset
        * @return Returns error code if something went wrong.
        *         Leaves on OOM situations.
        */
        TInt LoadResourceFileL( const TDesC& aModuleId, const
                                TDesC& aAlternative,
                                const TDesC& aResFileName,
                                TInt& aResOffSet );


        /**
        * Checks if the path&file exists.
        * @param aCheckThis, path to be checked.
        * @since 1.2s
        */
        TBool IsPathOk( const TDesC& aCheckThis ) const;

        /**
        * Constructs complete path from parts and checks for correctness.
        * @param aResult, the complete path.
        * @param aModuleId, dll's (or app) id. For example "IM".
        * @param aAlternative, id for alternative
        *                     (for example operator "Oper1" ) files.
        * @param aFileName, filename of the complete path
        * @return KErrNone if path was found from C or MMC card,
        *         otherwise error is returned
        * @since 1.2s
        */
        TInt ConstructCompletePathAndCheck( TDes& aResult,
                                            const TDesC& aModuleId,
                                            const TDesC& aAlternative,
                                            const TDesC& aFileName );

    private:

        /**
        * C++ default constructor.
        */
        CAlternatorImp();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aBrandRoot );


    private:    // Data

        // Pointer to eikon environment
        CEikonEnv* iEikEnv;
        // Current bitmap path
        TFileName iCurrentBitmap;

        // Root for branding files like: c:\system\data\branding\
        // owned
        HBufC* iBrandRoot;

        CCABrandInstallerImp* iBrandInstaller;

    };

#endif      // CALTERNATORIMP_H

// End of File
