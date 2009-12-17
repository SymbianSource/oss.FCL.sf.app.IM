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
* Description:  Private
*
*/


#ifndef CCAPICTURE_H
#define CCAPICTURE_H

//  INCLUDES
#include    <gdi.h>

// FORWARD DECLARATIONS
class TSize;
class CGulIcon;


// CLASS DECLARATION

/**
*  Picture wrapper for icons and smileys
*
*  @since Series 60 Platform 3.0
*/
class CCAPicture : public CPicture
    {

    public:  // Constructors and destructor

        /**
         * Constructor.
         * @param aMap Graphics device map for converting pixels to twips
         * @param aBitmap Bitmap
         * @param aBitmapMask Mask. Can be NULL
         * @param aIndex Index of the picture, used for the rich text icons.
         */
        CCAPicture( MGraphicsDeviceMap& aMap, CGulIcon* aIcon, TInt aIndex = -1,
                    TBool aExternalIcon = ETrue );
        /**
         * Destructor.
         */
        ~CCAPicture();

    public: // From CPicture

        /**
         * Prohibit linebreaks.
         */
        TBool LineBreakPossible( TUint aClass,
                                 TBool aBeforePicture,
                                 TBool aHaveSpaces ) const;
        /**
         * Draw the picture
         */
        void Draw( CGraphicsContext& aGc,
                   const TPoint& aTopLeft,
                   const TRect& aClipRect,
                   MGraphicsDeviceMap* aMap ) const;
        /**
         * Does nothing. There's no need for it, but must be implemented.
         */
        void ExternalizeL( RWriteStream& aStream ) const;

        /**
         * Returns the picture's size in twips.
         * @param aSize Size.
         */
        void GetOriginalSizeInTwips( TSize& aSize ) const;

        /**
        * Gets the index of the picture, if it is set in the constructor
        * @return TInt The index of the picture, -1 if not set in the
        * 	           constructor.
        */

        TInt Index() const;

    private:

        /**
         * Sets the size of the picture in twips.
         * according to the size in pixels.
         */
        void SetTwips() const;

    protected:	// Data

        // Index of a smiley icon
        TInt iIndex;

        // we might own
        CGulIcon* iIcon;

        TBool iExternalIcon;

        MGraphicsDeviceMap& iGfxMap;
    };

#endif 		// CCAPICTURE_H

// End of File
