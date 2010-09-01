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
* Description:  Interface class for skin variant.
*
*/



#ifndef MCASKINVARIANT_H
#define MCASKINVARIANT_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CWindowGc;
class CCoeControl;
class MAknsControlContext;
class CGulIcon;
class TRgb;

// CLASS DECLARATION

/**
*	Interface class for skin variant.
*  @lib chat.app
*  @since 1.2s
*/
class MCASkinVariant
    {

    public: // New functions

        /**
        * Returns flag to enable/disable skin.
        * @since 1.2s
        * @return TInt, flag.
        */
        virtual TInt SkinFlag() const = 0;
        /**
        * Clears listbox background.
        * @since 1.2s
        * @param aGc, graphics context to draw to.
        * @param aUsedPortionOfViewRect, rect to draw to.
        * @param aContext, skin control context.
        */
        virtual void ClearChatListBoxBackGround(
            CWindowGc& aGc,
            const TRect& aUsedPortionOfViewRect,
            MAknsControlContext* aContext ) = 0;
        /**
        * Returns main pane background control context.
        * @since 1.2s
        * @return MAknsControlContext, skin control context.
        */
        virtual MAknsControlContext* MainPaneBackgroundContext() const = 0;

        /**
        * Update layout for skinvariant
        * @return None
        */
        virtual void UpdateLayout() = 0;

        /**
        * Draws skinned version of the message editor.
        * @since 1.2s
        * @param aGc, graphics context.
        * @param aParentRect, rects needed for drawing
        * @param aEditorRect
        * @param aOutlineRect
        * @return TBool, whether skins were drawn or not.
        */
        virtual TBool DrawSkinnedEditor( const CCoeControl* aControl,
                                         CWindowGc& aGc,
                                         TRect& aParentRect,
                                         TRect& aEditorRect,
                                         TRect& aOutlineRect ) = 0;

        /**
        * Loads skinned/normal IM application bitmaps.
        * @since 1.2s
        * @param aBitmapId, bitmap id
        * @param aMaskId, bitmap mask's id
        * @param aFullPath, bitmap path
        * @param aOverrideBrand, if true, bitmap is got from skins, not through
        *               brand bitmaps (if brand is active)
        * @param aCustomColor  If provided, will change the color of the bitmap
        *                      to this color.
        * @return CGulIcon, created icon, user of this method gets the ownership
        */
        virtual CGulIcon* LoadBitmapL( TInt aBitmapId, TInt aMaskId,
                                       const TDesC& aFullPath,
                                       TBool aOverrideBrand = EFalse,
                                       TRgb* aCustomColor = NULL ) = 0;

    };

#endif      // MCASKINVARIANT_H  

// End of File
