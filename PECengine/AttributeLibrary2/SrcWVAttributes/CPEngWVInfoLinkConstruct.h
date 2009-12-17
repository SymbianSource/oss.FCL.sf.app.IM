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
* Description:  WV InfoLink attribute constructor.
*
*/

#ifndef CPENGWVINFOLINKCONSTRUCT_H
#define CPENGWVINFOLINKCONSTRUCT_H

//  INCLUDES
#include    "CPEngWVAttributeConstructBase.h"
#include    "CPEngWVAttributeModelBase.h"
#include    <PEngWVPresenceAttributes2.h>
#include    <E32Base.h>



/**
 * WV InfoLink attribute constructor.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngWVInfoLinkConstruct ) : public CPEngWVAttributeConstructBase
    {
public:  // Constructors and destructor

    /**
     * C++ constructor.
     */
    CPEngWVInfoLinkConstruct( TPEngWVCspVersion aCurrentCspVer );


    /**
     * Destructor.
     */
    virtual ~CPEngWVInfoLinkConstruct();



public: // Functions from MPEngPresenceAttrConstructorTypeImp

    /**
     * From MPEngPresenceAttrConstructorTypeImp.
     *
     * @since 3.0
     */
    MPEngPresenceAttrModelTypeImp* NewAttributeInstanceLC( TBool aUserOwnAttribute ) const;




public: //Attribute model implementation



    /**
     * InfoLink data holder.
     *
     * @lib PEngAttrLib
     * @since 3.0
     */
    NONSHARABLE_CLASS( CPEngWVInfoLink ) : public CBase
        {
public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CPEngWVInfoLink* NewLC();
        static CPEngWVInfoLink* NewLC( RReadStream& aStream );

        /**
         * Destructor.
         */
        virtual ~CPEngWVInfoLink();


private:

        /**
         * C++ default constructor.
         */
        CPEngWVInfoLink();


public:


        /**
         * Setters to set data fields.
         */
        void SetUrlL( const TDesC& aUrl );
        void SetDescriptionL( const TDesC& aDescr );
        void SetMimeTypeL( const TDesC& aMimeType );

        /**
         * Getters to access data fields.
         */
        void GetUrl( TPtrC16& aUrl ) const;
        void GetDescription( TPtrC16& aDescr ) const;
        void GetMimeType( TPtrC16& aMimeType ) const;

        /**
         * Serialization support.
         */
        TInt ExternalizeSize() const;
        void ExternalizeL( RWriteStream& aStream ) const;
        void InternalizeL( RReadStream& aStream );


private:    //Data

        //OWN: Info link
        HBufC* iInfoLink;

        //OWN: Info link description
        HBufC* iInfoLinkDescr;

        //OWN: Info link mime type
        HBufC* iInfoLinkMimeType;
        };




    /**
     * WV InfoLink attribute implementation.
     *
     * @lib PEngAttrLib
     * @since 3.0
     */
    NONSHARABLE_CLASS( CPEngWVInfoLinkModel ): public CPEngWVAttributeModelBase
        {

public:  // Constructors and destructor

        /**
         * Destructor.
         */
        ~CPEngWVInfoLinkModel();


        /**
         * C++ constructor.
         */
        CPEngWVInfoLinkModel( TBool aUserOwnAttribute );


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




private: // Helpers

        /**
         * Helper to set info link field value.
         *
         * @param aInfoLink The infolink struct to update.
         * @param aData The new data.
         * @param aInfoLinkFieldId The field.
         * @return ETrue if the aInfoLinkFieldId was a valid
         * infolink field. Else EFalse.
         */
        TBool SetInfoLinkDataL( CPEngWVInfoLink& aInfoLink,
                                const TDesC16& aData,
                                TInt aInfoLinkFieldId );

        /**
         * Helper to get info link field value.
         *
         * @param aInfoLink The infolink struct from to get.
         * @param aInfoLinkFieldId The field to get.
         * @param aData On the return points to the field data.
         * @return ETrue if the aInfoLinkFieldId was a valid
         * infolink field. Else EFalse.
         */
        TBool GetInfoLinkData( const CPEngWVInfoLink& aInfoLink,
                               TInt aInfoLinkFieldId,
                               TPtrC16& aData ) const;


private: //Data

        //OWN: Info link array
        RPointerArray< CPEngWVInfoLink > iInfoLinks;

        };
    };

#endif      //CPENGWVINFOLINKCONSTRUCT_H

//  End of File

