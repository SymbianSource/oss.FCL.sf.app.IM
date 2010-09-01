/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message containing picture
*
*/


#ifndef CCACONTENTMESSAGE_H
#define CCACONTENTMESSAGE_H

#include <e32base.h>

#include "CCAMessageBase.h"

//	FORWARD CLASS DECLERATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
 *  Message containing picture
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class CCAContentMessage : public CCAMessageBase
    {
    public: // Construction

        /**
         * Construction
         * @return Pointer to new created instance of CCAContentMessage
         */
        static CCAContentMessage* NewL( TInt aOpCode,
                                        const TDesC& aSender,
                                        const TDesC& aRecipient,
                                        const MDesCArray* aRecipients,
                                        const MDesCArray* aScreenNames,
                                        const TDesC8& aContentType,
                                        const TDesC8& aContentData,
                                        const TDesC& aContentAddress );

        /**
         * Construction. This is only skeleton creation. Used with internalize
         * @return Pointer to new created instance of CCAContentMessage
         */
        static CCAContentMessage* NewL();

        /**
         * Construction
         * @return Pointer to new created instance of CCAContentMessage
         */
        static CCAContentMessage* NewLC( TInt aOpCode,
                                         const TDesC& aSender,
                                         const TDesC& aRecipient,
                                         const MDesCArray* aRecipients,
                                         const MDesCArray* aScreenNames,
                                         const TDesC8& aContentType,
                                         const TDesC8& aContentData,
                                         const TDesC& aContentAddress );

        /**
         * Destruction
         */
        ~CCAContentMessage();

    private: // Construction

        /**
         * Constructor
         */
        CCAContentMessage();

        /**
         * Constructor
         */
        void ConstructL( const TDesC8& aContentType,
                         const TDesC8& aContentData,
                         const TDesC& aContentAddress );

    private: // From MCAMessage

        /**
         * @see MCAMessage
         */
        TInt SizeInBytes() const;

        /**
         * @see MCAMessage
         */
        TContentType ContentType() const;

        /**
         * @see MCAMessage
         */
        void ReleaseTemporaryContentData();

        /**
         * @see MCALoggerMessage
         */
        void InternalizeL( RReadStream& aStream );

    public: // New methods

        /**
         * Set thumbnail size. Picture is scaled to this one,
         * when thumbnail created.
         * @param aSize, Size of thumbnail.
         */
        IMPORT_C void SetThumbnailSize( TSize aSize );

        /**
         * Get thumbnail size.
         * @return Thumbnail size
         */
        TSize ThumbnailSize() const;

        /**
         * Set thumbnail picture for message. Ownership is transferred.
         * @param aBitmap. New thumbnail bitmap
         */
        void SetThumbnail( CFbsBitmap* aBitmap );

        /**
         * Get thumbnail bitmap
         * @return Thumbnail. Ownership is not tranferred
         */
        IMPORT_C CFbsBitmap* Thumbnail() const;

    private: //Member data

        /// Size of thumbnails
        TSize iThumbnailSize;

        /// Thumbnail
        CFbsBitmap* iThumbnail;

        /// Type of content
        TContentType iContentType;

    };

#endif      // CCACONTENTMESSAGE_H

// End of File
