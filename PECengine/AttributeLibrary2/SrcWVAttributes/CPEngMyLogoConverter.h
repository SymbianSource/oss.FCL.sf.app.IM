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
* Description:  MyLogo converter.
*
*/

#ifndef CPENGMYLOGOCONVERTER_H_
#define CPENGMYLOGOCONVERTER_H_

//  INCLUDES
#include <E32Base.h>
#include <F32file.h>



//FORWARD DECLARATION
class CImageDecoder;
class CImageEncoder;
class CBitmapScaler;
class CFbsBitmap;
class MPEngMyLogoConversionObserver;


// CLASS DECLARATION

/**
 * MyLogo converter.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngMyLogoConverter ) : public CActive
    {
private:  // private enumerations

    /**
     * Logo converter states.
     */
    enum TLogoConverterState
        {
        EConverterIdle = 0,  //Idle state, no operation running
        EImageOpenToBitmap,
        EBitmapScaleDown,
        EBitmapStoreToImage,
        };


public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     * Constructs the converter for image file.
     */
    static CPEngMyLogoConverter* FileNewL( const TDesC& aFileName,
                                           MPEngMyLogoConversionObserver& aObserver );

    /**
     * Two-phased constructor.
     * Constructs the converter for image buffer.
     */
    static CPEngMyLogoConverter* DataNewL( const TDesC8& aImageData,
                                           MPEngMyLogoConversionObserver& aObserver );


    /**
     * Destructor.
     */
    virtual ~CPEngMyLogoConverter();


private:

    /**
     * C++ default constructor.
     */
    CPEngMyLogoConverter( MPEngMyLogoConversionObserver& aObserver );


    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructFromFileL( const TDesC& aImageFile );
    void ConstructFromDataL( const TDesC8& aImageData );



public: // New functions

    /**
     * Conversion service.
     *
     * @since 3.0
     *
     * @param
     */
    void IssueConvert( TRequestStatus& aRequestStatus );



protected:  // From CActive


    /**
     * RunL().
     *
     * Performs the conversion steps
     * according the states.
     *
     * @since 3.0
     */
    void RunL();


    /**
     * RunError().
     *
     * Completes the client request with reported error.
     *
     * @since 3.0
     * @param aError The leave code from RunL leave.
     * @return Always KErrNone.
     */
    TInt RunError( TInt aError );


    /**
     * DoCancel().
     *
     * cancels the currently active processing state.
     *
     * @since 3.0
     */
    void DoCancel();



private: // Private helper methods

    /**
     * Sets the active object active and advances the
     * state to given one.
     *
     * @since 3.0
     * @param aNewState The new state.
     */
    void SetStateActive( TLogoConverterState aNewState );


    /**
     * Checks does the processed image fulfil MyLogo
     * requirements.
     *
     * @since 3.0
     * @return ETrue if source image satisfies MyLogo
     * requirements. Else EFalse.
     */
    TBool VerifyMyLogoRequirementsL();


    /**
     * Issues the image open to bitmap.
     *
     * @since 3.0
     */
    void IssueImageOpenToBitmapL();


    /**
     * Issues the image scale down.
     *
     * @since 3.0
     */
    void IssueBitmapScaleDownL();


    /**
     * Issues the bitmap store to destination image.
     *
     * @since 3.0
     */
    void IssueBitmapStoreToImageL();


    /**
     * Notifies registered observer from
     * new logo data.
     *
     * @since 3.0
     */
    void NotifyNewLogo();


    /**
     * Completes the original client request with
     * given status and deletes the object itself.
     *
     * @since 3.0
     * @param aStatus The completion code.
     */
    void ConversionCompletedD( TInt aStatus );


    /**
     * Panics the converter with given reason.
     *
     * @since 3.0
     * @param aPanicReason The reason code.
     */
    void PanicConverter( TInt aPanicReason ) const;




private:    // Data

    //OWN: The file server session.
    RFs     iFs;

    //OWN: The converter state.
    TLogoConverterState     iState;

    //OWN: The convert observer.
    MPEngMyLogoConversionObserver&  iObserver;

    //REF: Request status from client
    TRequestStatus* iClientRequest;

    //OWN: The image data buffer
    HBufC8* iImageDataBuffer;

    //OWN: The image mime type buffer
    HBufC8* iImageMimeTypeBuffer;

    //OWN: The image filename buffer
    HBufC16*    iImageFileNameBuffer;

    //OWN: The image decoder. Lazy initialized.
    CImageDecoder*  iImageDecoder;

    //OWN: The image encoder. Lazy initialized.
    CImageEncoder*  iImageEncoder;

    //OWN: Image scaler. Lazy initialized.
    CBitmapScaler*  iBitmapScaler;

    //OWN: MyLogo fbs bitmap. Lazy initialized.
    CFbsBitmap* iMyLogoBitmap;
    };




// CLASS DECLARATION

/**
 * Logo converter observer interface.
 *
 * @since 3.0
 */
class MPEngMyLogoConversionObserver
    {
    public: //Observation methods


        /**
         * Reports the observer new MyLogo contents.
         *
         * NOTE: Observer must take the ownership
         * of parameters in all cases.
         *
         * @since 3.0
         * @param aImageData The MyLogo image data.
         * @param aImageMimeType The MyLogo mime type.
         * @param aImageSoureFileName The source file name.
         */
        virtual void HandleNewMyLogo( HBufC8* aImageData,
                                      HBufC8* aImageMimeType,
                                      HBufC* aImageSoureFileName ) = 0;


        /**
         * Reports the converter complete.
         *
         * @since 3.0
         */
        virtual void HandleConverterDestroy() = 0;




    protected: //Prohibited destructor

        /**
         * Inline virtual destructor. Concrete
         * observer can't be destroyed using
         * this interface.
         *
         * @since 3.0
         */
        virtual ~MPEngMyLogoConversionObserver() {};

    };






#endif      // CPENGMYLOGOCONVERTER_H_

// End of File
