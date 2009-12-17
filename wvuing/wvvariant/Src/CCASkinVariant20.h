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
* Description:  Active implementation of skin feature.
*
*/



#ifndef CCASKINVARIANT20_H
#define CCASKINVARIANT20_H

//  INCLUDES
#include "MCASkinVariant.h"
#include <e32base.h>
#include <gdi.h>
#include <AknsItemID.h>
#include <AknIconUtils.h>


// FORWARD DECLARATIONS
class CCoeControl;
class CAknsBasicBackgroundControlContext;
class MCAOpBrandVariant;

// CLASS DECLARATION

/**
*  Active implementation of skin feature.
*  See base class MCASkinVariant.h for comments.
*
*  @lib chat.app
*  @since 2.0
*/
class CCASkinVariant20 : public CBase, public MCASkinVariant
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCASkinVariant20* NewL(
            MCAOpBrandVariant* aOpBrandVariant = NULL );

        /**
        * Destructor.
        */
        virtual ~CCASkinVariant20();

    public: // Functions from base classes

        /**
        * From MCASkinVariant
        * See method comments from MCASkinVariant.h.
        * @since 2.0
        */
        TInt SkinFlag() const;
        void ClearChatListBoxBackGround( CWindowGc& aGc,
                                         const TRect& aUsedPortionOfViewRect,
                                         MAknsControlContext* aContext );

        MAknsControlContext* MainPaneBackgroundContext() const;

        /**
        * From MCASkinVariant
        * @see MCASkinVariant
        */
        void UpdateLayout();

        TBool DrawSkinnedEditor( const CCoeControl* aControl,
                                 CWindowGc& aGc,
                                 TRect& aParentRect,
                                 TRect& aEditorRect,
                                 TRect& aOutlineRect );


        CGulIcon* LoadBitmapL( TInt aBitmapId, TInt aMaskId, const TDesC& aFullPath,
                               TBool aOverrideBrand = EFalse,
                               TRgb* aCustomColor = NULL );

    private:

        void MapFromBitmapIdToAknsItemId( TInt aBitmapId,
                                          TAknsItemID& aItemId,
                                          TInt& aColorGroupId );

    private:

        /**
        * C++ default constructor.
        */
        CCASkinVariant20();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MCAOpBrandVariant* aOpBrandVariant );

    private:    // Data

        // skin background control context
        CAknsBasicBackgroundControlContext* iBgContext;
        // doesn't own
        MCAOpBrandVariant* iOpBrandVariant;

        // owns, last used bitmap file
        HBufC* iBitmapFile;

        // doesn't own
        RFs& iFs;

    };

#endif      // CCASKINVARIANT20_H

// End of File
