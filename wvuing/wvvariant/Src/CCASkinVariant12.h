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
* Description: Skin Variant
*
*/


#ifndef CCASKINVARIANT12_H
#define CCASKINVARIANT12_H

//  INCLUDES
#include "MCASkinVariant.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CCoeControl;
class MAknsControlContext;

// CLASS DECLARATION

/**
* Skin variant
*
* @lib CAVariant
*/
class CCASkinVariant12 : public CBase, public MCASkinVariant
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCASkinVariant12* NewL();

        /**
        * Destructor.
        */
        virtual ~CCASkinVariant12();


    public: // Functions from base classes

        /**
        * Flag for skin
        * @return Skin flag
        */
        TInt SkinFlag() const;
        void ClearChatListBoxBackGround( CWindowGc& aGc,
                                         const TRect& aUsedPortionOfViewRect, MAknsControlContext* aContext );




        MAknsControlContext* MainPaneBackgroundContext() const;

        TBool DrawSkinnedEditor( const CCoeControl* aControl,
                                 CWindowGc& aGc,
                                 TRect& aParentRect,
                                 TRect& aEditorRect,
                                 TRect& aOutlineRect );

        CGulIcon* LoadBitmapL( TInt aBitmapId, TInt aMaskId, const TDesC& aFullPath, TBool aOverrideBrand = EFalse );


    private:

        /**
        * C++ default constructor.
        */
        CCASkinVariant12();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    };

#endif      // CCASKINVARIANT12_H

// End of File
