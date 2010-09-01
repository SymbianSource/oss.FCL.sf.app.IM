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
* Description:  WV Status Text attribute constructor.
*
*/

#ifndef CPENGWVTHUMBNAILCONSTRUCT_H
#define CPENGWVTHUMBNAILCONSTRUCT_H

//  INCLUDES
#include    "CPEngWVAttributeConstructBase.h"
#include    "CPEngWVAttributeModelBase.h"
#include    <PEngWVPresenceAttributes2.h>
#include    <E32Base.h>




/**
 * WV Thumbnail attribute constructor.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngWVThumbnailConstruct ) : public CPEngWVAttributeConstructBase
    {
public:  // Constructors and destructor

    /**
     * C++ constructor.
     */
    CPEngWVThumbnailConstruct( TPEngWVCspVersion aCurrentCspVer );


    /**
     * Destructor.
     */
    virtual ~CPEngWVThumbnailConstruct();



public: // Functions from MPEngPresenceAttrConstructorTypeImp

    /**
     * From MPEngPresenceAttrConstructorTypeImp.
     *
     * @since 3.0
     */
    MPEngPresenceAttrModelTypeImp* NewAttributeInstanceLC( TBool aUserOwnAttribute ) const;




public: //Attribute model implementation

    /**
     * WV Thumbnail attribute implementation.
     *
     * @lib PEngAttrLib
     * @since 3.0
     */
    NONSHARABLE_CLASS( CPEngWVThumbnailModel ) : public CPEngWVAttributeModelBase
        {
public:  // Constructors and destructor

        /**
         * Destructor.
         */
        ~CPEngWVThumbnailModel();


        /**
         * C++ constructor.
         */
        CPEngWVThumbnailModel( TBool aUserOwnAttribute );


public: // From MPEngPresenceAttrModelTypeImp


        /**
         * From MPEngPresenceAttrModelTypeImp.
         */
        TBool SetDataL( TInt* aIntData,
                        const TDesC8* a8Data,
                        const TDesC16* a16Data,
                        TInt aField,
                        TInt aGroup );

        TBool GetData( TInt* aIntData,
                       TPtrC8* a8Data,
                       TPtrC16* a16Data,
                       TInt aField,
                       TInt aGroup ) const;


        void EncodeDataToXmlL( MPEngXMLSerializer& aSerializer ) const;

        void DecodeDataFromXmlL( MPEngXMLParser& aParser,
                                 const TDesC8& aValueBlock );

        void ResetDataL();
        TInt DataExternalizeSize() const;
        void ExternalizeDataL( RWriteStream& aStream ) const;
        void InternalizeDataL( RReadStream& aStream  );



private: // Private helpers

        TBool SetData8L( const TDesC8& aData, TInt aField );
        TBool GetData8( TInt aField, TPtrC8& aData ) const;
        TBool SetData16L( const TDesC16& aData, TInt aField );
        TBool GetData16( TInt aField, TPtrC16& aData ) const;



private: //Data

        //OWN: Freeform thumbnail field
        HBufC8*     iDesC8Buf0;

        //OWN: Freeform thumbnail field
        HBufC8*     iDesC8Buf1;

        //OWN: Freeform thumbnail field
        HBufC8*     iDesC8Buf2;

        //OWN: Freeform thumbnail field
        HBufC16*     iDesC16Buf0;

        //OWN: Freeform thumbnail field
        HBufC16*     iDesC16Buf1;

        //OWN: Freeform thumbnail field
        HBufC16*     iDesC16Buf2;

        };
    };

#endif      //CPENGWVTHUMBNAILCONSTRUCT_H

//  End of File

