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
* Description:  WV Status content attribute implementation.
*
*/

#ifndef CPENGWVSTATUSCONTENTMODEL_H
#define CPENGWVSTATUSCONTENTMODEL_H

//  INCLUDES
#include    "CPEngWVAttributeConstructBase.h"
#include    "CPEngWVAttributeModelBase.h"
#include    "CPEngMyLogoConverter.h"

#include    <PEngWVPresenceAttributes2.h>
#include    <E32Base.h>



//FORWARD DECLARATION
class T_CWVStatusContentModel;


/**
 * WV Status content attribute implementation.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngWVStatusContentModel ): public CPEngWVAttributeModelBase,
        public MPEngMyLogoConversionObserver
    {
public:  // Constructors and destructor

    /**
     * Destructor.
     */
    ~CPEngWVStatusContentModel();


    /**
     * C++ constructor.
     */
    CPEngWVStatusContentModel( TBool aUserOwnAttribute );


public: // From MPEngPresenceAttrModelTypeImp


    /**
     * From MPEngPresenceAttrModelTypeImp.
     */
    TBool GetData( TInt* aIntData,
                   TPtrC8* a8Data,
                   TPtrC16* a16Data,
                   TInt aField,
                   TInt aGroup ) const;

    TBool SetDataL( TInt* aIntData,
                    const TDesC8* a8Data,
                    const TDesC16* a16Data,
                    TInt aField,
                    TInt aGroup );

    TBool SetDataAsyncL( const TDesC8* a8Data,
                         const TDesC16* a16Data,
                         TRequestStatus& aRequestStatus,
                         TInt aField,
                         TInt aGroup );

    void CancelSetDataAsync();

    void EncodeDataToXmlL( MPEngXMLSerializer& aSerializer ) const;

    void DecodeDataFromXmlL( MPEngXMLParser& aParser,
                             const TDesC8& aValueBlock );

    void ResetDataL();
    TInt DataExternalizeSize() const;
    void ExternalizeDataL( RWriteStream& aStream ) const;
    void InternalizeDataL( RReadStream& aStream  );




private: // MPEngMyLogoConversionObserver

    void HandleNewMyLogo( HBufC8* aImageData,
                          HBufC8* aImageMimeType,
                          HBufC* aImageSoureFileName );
    void HandleConverterDestroy();

private: // Helper

    void CheckReceivedStatusContentL();



private: //Friend declaration


    /**
     * Unit test class is friend so it can access members.
     */
    friend class T_CWVStatusContentModel;       // CSI: 36 #


private: //Data


    //OWN: MyLogo status content data.
    HBufC8*     iMyLogoData;

    //OWN: MyLogo status content mime type.
    HBufC8*     iMyLogoMimeType;

    //OWN: MyLogo status source file name.
    HBufC*      iMyLogoSoureFileName;


    //OWN: Raw status content data from network.
    HBufC8*     iStatusContentNW;

    //OWN: Asynchronous MyLogo converter, lazy initialized
    CPEngMyLogoConverter*   iMyLogoConverter;

    };


#endif      //CPENGWVSTATUSCONTENTMODEL_H

//  End of File

