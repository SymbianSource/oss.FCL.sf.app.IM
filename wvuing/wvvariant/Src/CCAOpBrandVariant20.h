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
* Description:  Active implementation of res.file& bitmaps changing feature.
 *
*/



#ifndef CCAOPBRANDVARIANT20_H
#define CCAOPBRANDVARIANT20_H

//  INCLUDES
#include "MCAOpBrandVariant.h"


// FORWARD DECLARATIONS
class CAlternator;

// CLASS DECLARATION

/**
 * Active implementation of res.file& bitmaps changing feature.
 * See base class MCAOpBrandVariant.h for method comments.
 *
 *  @lib chat.app
 *  @since 1.2s
 */
class CCAOpBrandVariant20 : public CBase, public MCAOpBrandVariant
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCAOpBrandVariant20* NewL();

        /**
         * Destructor.
         */
        virtual ~CCAOpBrandVariant20();


    public: // Functions from base classes

        /**
         * Methods implemented from MCAOpBrandVariant.
         * See MCAOpBrandVariant.h for method comments.
         * @since 1.2s
         */
        void RemoveAppResourceL( CCoeEnv* aCoeEnv, TInt& aAppResFileOffset );
        void RetrieveSavedSettingsL( CCoeEnv* aCoeEnv, const TDesC& aOrigAppResPath,
                                     const TDesC& aOrigEngResPath,
                                     const TDesC& aOrigCommUiResPath,
                                     TDes& aBitmapPath );
        void ActivateBrandL( const TDesC& aServer, TDes& aBitmapPath );
        void DeleteResource( CCoeEnv* aCoeEnv );
        TBool BrandBitmapsActivated() const;
        // not implemented in 2.0
        void CurrentResourceFileL( const TDesC& aServer, const TDesC& aResourceFile, TDes& aFullPath );

    private:

        /**
         * C++ default constructor.
         */
        CCAOpBrandVariant20();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();


    private:    // Data

        // application resource file offset
        TInt iAppResFileOffset;
        // app eng's resource file offset
        TInt iEngResFileOffset;
        // common ui's resource file offset
        TInt iCommUiResFileOffset;
        // original application resource file path
        TFileName iOrigAppResource;
        // original engine resource file path
        TFileName iOrigEngResource;
        // original common ui resource file path
        TFileName iOrigCommUiResource;
        // original application bitmap path
        TFileName iOrigBitmapPath;
        // interface to alternator.dll
        CAlternator* iAlternator;
        // control environment
        CCoeEnv* iCoeEnv;
        HBufC* iServerName;

    };

#endif      // CCAOPBRANDVARIANT20_H

// End of File
