/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for loading images.
*
*/


#ifndef __MCAIMAGELOADER_H__
#define __MCAIMAGELOADER_H__

const TSize KNullSize( 0, 0 );

//	FORWARD CLASS DECLERATIONS

// CLASS DECLARATION

/**
 *  Interface for loading images.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAImageLoader
    {
    public: // Interface

        /**
         *
         */
        void LoadImageL( const TDesC& aContentType,
                         const TDesC8& aContent,
                         MCAImageLoaderObserver*  aObserver,
                         const TSize& aSize = KNullSize ) = 0;

        /**
         *
         */
        CFbsBitmap* LoadImageL( const TDesC& aContentType,
                                const TDesC8& aContent,
                                const TSize& aSize = KNullSize ) = 0;


    private: // Protection

        /**
         * Destructor for protection.
         */
        ~MCAImageLoader();

    };

#endif      // __MCAIMAGELOADER_H__

// End of File
