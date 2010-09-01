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
* Description:  WV Client Info attribute constructor.
*
*/

#ifndef CPENGWVCLIENTINFOCONSTRUCT_H
#define CPENGWVCLIENTINFOCONSTRUCT_H

//  INCLUDES
#include    "CPEngWVAttributeConstructBase.h"
#include    "CPEngWVAttributeModelBase.h"
#include    <PEngWVPresenceAttributes2.h>
#include    <E32Base.h>



//FORWARD DECLARATION
class T_CWVClientInfoModel;


/**
 * WV Client Info attribute constructor.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngWVClientInfoConstruct ) : public CPEngWVAttributeConstructBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngWVClientInfoConstruct* NewL( TPEngWVCspVersion aCurrentCspVer );


    /**
     * Destructor.
     */
    virtual ~CPEngWVClientInfoConstruct();


private:  // Constructors

    /**
     * C++ constructor.
     */
    CPEngWVClientInfoConstruct( TPEngWVCspVersion aCurrentCspVer );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: // Functions from MPEngPresenceAttrConstructorTypeImp

    /**
     * From MPEngPresenceAttrConstructorTypeImp.
     *
     * @since 3.0
     */
    MPEngPresenceAttrModelTypeImp* NewAttributeInstanceLC( TBool aUserOwnAttribute ) const;




public: //Attribute model implementation

    /**
     * WV Client Info attribute implementation.
     *
     * @lib PEngAttrLib
     * @since 3.0
     */
    NONSHARABLE_CLASS( CPEngWVClientInfoModel ) : public CPEngWVAttributeModelBase
        {
public:  // Constructors and destructor

        /**
         * Destructor.
         */
        ~CPEngWVClientInfoModel();


        /**
         * C++ constructor.
         */
        CPEngWVClientInfoModel( const CPEngWVClientInfoConstruct& aConstructor,
                                TBool aUserOwnAttribute );


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

        void EncodeDataToXmlL( MPEngXMLSerializer& aSerializer ) const;

        void DecodeDataFromXmlL( MPEngXMLParser& aParser,
                                 const TDesC8& aValueBlock );

        void ResetDataL();
        TInt DataExternalizeSize() const;
        void ExternalizeDataL( RWriteStream& aStream ) const;
        void InternalizeDataL( RReadStream& aStream  );


private: //Friend declaration


        /**
         * Unit test class is friend so it can access members.
         */
        friend class T_CWVClientInfoModel;      // CSI: 36 #


private: //Data

        //REF: The constructor
        const CPEngWVClientInfoConstruct&   iConstructor;

        //OWN: Device type
        TPEngWVCliInfDeviceTypeValues   iDeviceType;

        //OWN: Device detail
        HBufC*  iDeviceManufacturer;

        //OWN: Device detail
        HBufC*  iClientProducer;

        //OWN: Device detail
        HBufC*  iDeviceModel;

        //OWN: Device detail
        HBufC*  iClientVersion;

        //OWN: Device detail
        HBufC*  iClientLanguage;
        };


private: //Friend declaration

    /**
     * Model is friend so it can access directly
     * cached device details.
     */
    friend class CPEngWVClientInfoModel;        // CSI: 36 #

    /**
     * Unit test class is friend so it can access members.
     */
    friend class T_CWVClientInfoModel;          // CSI: 36 #



private: //Data


    //OWN: User device detail
    HBufC*  iUserDeviceManufacturer;

    //OWN: User device detail
    HBufC*  iUserClientProducer;

    //OWN: User device detail
    HBufC*  iUserDeviceModel;

    //OWN: User device detail
    HBufC*  iUserClientVersion;

    //OWN: User device detail
    HBufC*  iUserClientLanguage;

    };

#endif      //CPENGWVCLIENTINFOCONSTRUCT_H

//  End of File

