/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Inactive implementation for res.file& bitmap changing feature.
 *
*/



#ifndef CCAOPBRANDVARIANT12_H
#define CCAOPBRANDVARIANT12_H

//  INCLUDES
#include "MCAOpBrandVariant.h"

// CLASS DECLARATION

/**
 *  Inactive implementation for res.file& bitmap changing feature.
 *  See method comments from base class MCAOpBrandVariant.h
 *
 *  @lib chat.app
 *  @since 1.2s
 */
class CCAOpBrandVariant12 : public CBase, public MCAOpBrandVariant
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCAOpBrandVariant12* NewL();

        /**
         * Destructor.
         */
        virtual ~CCAOpBrandVariant12();


    public: // Functions from base classes

        /**
         * From MCAOpBrandVariant, see MCAOpBrandVariant.h for
         * method comments.
         * @since 1.2s
         */
        void RemoveAppResourceL( CCoeEnv* aCoeEnv, TInt& aAppResFileOffset );
        void RetrieveSavedSettingsL( CCoeEnv* aCoeEnv, const TDesC& aOrigResFilePath );
        void ActivateBrandL( const TDesC& aServer, const TDesC& aAppResFileName,
                             const TDesC& aEngResFileName,
                             TDes& aChatAppMbmFileName );
        void DeleteResource( CCoeEnv* aCoeEnv );
        void DeleteEngResource( CCoeEnv* aCoeEnv );
        void RetrieveSavedEngSettingsL( CCoeEnv* aCoeEnv, const TDesC& aOrigResFilePath );

    private:

        /**
         * C++ default constructor.
         */
        CCAOpBrandVariant12();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
    };

#endif      // CCAOPBRANDVARIANT12_H

// End of File
