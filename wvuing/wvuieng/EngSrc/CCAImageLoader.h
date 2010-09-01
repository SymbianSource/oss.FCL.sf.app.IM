/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Image loader and scaler
*
*/


#ifndef CCAIMAGELOADER_H
#define CCAIMAGELOADER_H

#include <f32file.h>
#include "MCAChatObserver.h"
#include "MCAContentProcessor.h"

//	FORWARD CLASS DECLERATIONS
class MCAChatInterface;
class CCAPictureMessage;
class CImageDecoder;
class CBitmapScaler;
class CFbsBitmap;
class CImageEncoder;
class MCAImpsFactory;

// CLASS DECLARATION

/**
 *  Image loader and scaler
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class CCAImageLoader : public CActive, public MCAContentProcessor
    {
    public: // Construction

        /**
         * Construction
         */
        static CCAImageLoader* NewL( MCAChatInterface& aChatInterface,
                                     MCAImpsFactory& aImpsFactory );

        /**
         * Destruction
         */
        ~CCAImageLoader();

    private: // Construction

        /**
         * Constructor
         */
        CCAImageLoader( MCAChatInterface& aChatInterface,
                        MCAImpsFactory& aImpsFactory );

        /**
         * Constructor
         */
        void ConstructL();

    private: // From CActive

        /**
         * @see CActive
         */
        void RunL();

        /**
         * @see CActive
         */
        void DoCancel();

        /**
         * @see CActive.
         */
        TInt RunError( TInt aError );

    private: // From MCAContentProcessor

        /**
         * @see MCAContentProcessor
         */
        void RequestProcessingL( MCAMessage& aMessage );

        /**
         * @see MCAContentProcessor
         */
        void StartProcessingL( MCAMessage& aMessage );

        /**
         * @see MCAContentProcessor
         */
        void CancelProcessing( MCAMessage& aMessage );

        /**
         * @see MCAContentProcessor
         */
        void RemoveProcessingL( MCAMessage& aMessage );

    private: // New helper methods

        /**
         * LaunchProcessing if needed.
         */
        void LaunchProcessingL();

        /**
         * Get new size after decoding.
         * Scale to nearest (larger or equal) size with ratio 1:1, 1:2, 1:4 or 1:8
         * directly when decoding.
         *
         * @param aSize Size of original image
         * @param aTargetSize Size of target image
         * @param aAnyRatio Set this to ETrue if the decoder is cabable of scaling to any ratio
         * @return New size of decoded image
         */
        TSize DecodeSize( const TSize& aSize, const TSize& aTargetSize,
                          TBool aAnyRatio );

        /**
         * Get new size after scaling. If scale not needed return EFalse
         * Scaled size will be 320x240 if width > height or 240x320 if
         * height > width.
         *
         * @param aSize Original size of image
         * @param aNewSize New size of scaled image
         * @return ETrue if scale needed.
         */
        TBool ScaleSize( const TSize& aSize, TSize& aNewSize );

        /**
         * Notify observer for completion and remove message from array
         */
        void CompleteRequestL();

        /**
         * Reset current operation and remove all pending messages from stack.
         * @param aReason reson for reseting
         */
        void ResetProcessingL( TInt aReason );

        /**
         * Deletes and cancels all processing units ( decoder, encoder etc ).
         */
        void ClearState();


    private: // Data

        /// File system access
        RFs iFs;

        // Decoder for image.
        CImageDecoder* iDecoder;
        CBitmapScaler* iScaler;
        CImageEncoder* iEncoder;

        /// Interface to access containers
        MCAChatInterface& iChatInterface;

        /// Array of images to operate.
        RPointerArray< MCAMessage > iMessages;

        /// Processing on/off.
        TBool iProcessing;

        /// Bitmap. Owns.
        CFbsBitmap* iBitmap;

        /// Current message in process
        MCAMessage* iCurrentMessage;

        /// Target size for bitmap
        TSize iTargetSize;

        /// Thumbnail size for bitmap
        TSize iThumbSize;

        /// Owns.
        HBufC8* iContent;

        // Interface to MCAImpsFactory. Not owned.
        MCAImpsFactory& iImpsFactory;

        // See IMVariant.hrh, flag depends on
        // feature EIMFeatSendNotScalable.
        TBool iSendNotScalable;
    };

#endif      // CCAIMAGELOADER_H

// End of File
