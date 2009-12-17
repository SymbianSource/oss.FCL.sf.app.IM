/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef CCAOPBRANDVARIANT21_H
#define CCAOPBRANDVARIANT21_H

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
class CCAOpBrandVariant21 : public CBase, public MCAOpBrandVariant
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAOpBrandVariant21* NewL();

        /**
        * Destructor.
        */
        virtual ~CCAOpBrandVariant21();


    public: // Functions from base classes

        /**
        * Methods implemented from MCAOpBrandVariant.
        * See MCAOpBrandVariant.h for method comments.
        * @since 1.2s
        */
        void RemoveAppResourceL( CCoeEnv* aCoeEnv, TInt& aAppResFileOffset );

        /**
        * Retrieves branded application resource file if there is one.
        * If not, the default
        * application resource file is loaded.
        * @since 1.2s
        * @param aCoeEnv, control environment.
        * @param aOrigAppResPath, original application resource path
        * @param aOrigEngResPath, original engine resource path
        * @param aOrigPresenceResPath, original presence resource path
        * @param aOrigCommUiResPath, original common ui resource path
        * @param aBitmapPath, bitmap path
        */
        void RetrieveSavedSettingsL( CCoeEnv* aCoeEnv,
                                     const TDesC& aOrigAppResPath,
                                     const TDesC& aOrigEngResPath,
                                     const TDesC& aOrigPresenceResPath,
                                     const TDesC& aOrigVariationResPath,
                                     const TDesC& aOrigCommUiResPath,
                                     TDes& aBitmapPath );

        void ActivateBrandL( const TDesC& aServer, TDes& aBitmapPath );

        void DeleteResource( CCoeEnv* aCoeEnv );

        TBool BrandBitmapsActivatedL() const;

        void CurrentResourceFileL( const TDesC& aServer,
                                   const TDesC& aResourceFile,
                                   TDes& aFullPath  );

    private:

        /**
        * C++ default constructor.
        */
        CCAOpBrandVariant21();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:    // Data

        // application resource file offset
        TInt iAppResFileOffset;
        // app eng's resource file offset
        TInt iEngResFileOffset;
        // app presence's resource file offset
        TInt iPresenceResFileOffset;
        // app presence's resource file offset
        TInt iVariationResFileOffset;
        // common ui's resource file offset
        TInt iCommUiResFileOffset;
        // original application resource file path
        TFileName iOrigAppResource;
        // original engine resource file path
        TFileName iOrigEngResource;
        // original presence resource file path
        TFileName iOrigPresenceResource;
        // original presence resource file path
        TFileName iOrigVariationResource;
        // original common ui resource file path
        TFileName iOrigCommUiResource;
        // original application bitmap path
        TFileName iOrigBitmapPath;
        // interface to alternator.dll
        CAlternator* iAlternator;
        // control environment
        CCoeEnv* iCoeEnv;
        TBuf<64> iServerName;

        // Are branded bitmaps in use
        TBool iBrandedBitmaps;

    };

#endif      // CCAOPBRANDVARIANT21_H

// End of File
