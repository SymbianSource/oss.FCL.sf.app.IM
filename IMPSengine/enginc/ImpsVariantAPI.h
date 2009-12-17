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
* Description:  Handles variants
*
*/



#ifndef ImpsVariantAPI_H
#define ImpsVariantAPI_H

//  INCLUDES
#include "impsservices.h"

// ENUMERATIONS

enum TImpsVariants
    {
    //Variants
    EDpb = 1,
    EIpCirStandalone,
    EWAPUDP,
    EOpenAPI,
    EImLauncher
    };

// CLASS DECLARATION
class TThreadLocalStoragePackage
    {
    public:
        TInt iUserCount;
        TInt iFeatureBitMask;
    };

// CLASS DECLARATION

/**
*  Handler for bldvariants.
*/
class CImpsVariant : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CImpsVariant* NewLC();

        /**
        * Destructor.
        */
        virtual ~CImpsVariant();

    public: // New functions

        /**
        * Feature accessor
        * Leave conditions: out of memory.
        */
        IMPORT_C TBool IsFeatureSupportedL( TImpsVariants aVariant ) const;

    private:

        /**
        * C++ default constructor.
        */
        CImpsVariant();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:    // Data

        TThreadLocalStoragePackage*     iPointerPackage;
    };

#endif      // ImpsVariantAPI_H

// End of File
