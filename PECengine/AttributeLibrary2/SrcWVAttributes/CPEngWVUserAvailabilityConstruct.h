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
* Description:  WV User Availability attribute constructor.
*
*/

#ifndef CPENGWVUSERAVAILABILITYCONSTRUCT_H
#define CPENGWVUSERAVAILABILITYCONSTRUCT_H

//  INCLUDES
#include    "CPEngWVAttributeConstructBase.h"
#include    "CPEngWVAttributeModelBase.h"
#include    <PEngWVPresenceAttributes2.h>
#include    <E32Base.h>



/**
 * WV User Availability attribute constructor.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngWVUserAvailabilityConstruct ) : public CPEngWVAttributeConstructBase
    {
public:  // Constructors and destructor

    /**
     * C++ constructor.
     */
    CPEngWVUserAvailabilityConstruct( TPEngWVCspVersion aCurrentCspVer );


    /**
     * Destructor.
     */
    virtual ~CPEngWVUserAvailabilityConstruct();



public: // Functions from MPEngPresenceAttrConstructorTypeImp

    /**
     * From MPEngPresenceAttrConstructorTypeImp.
     *
     * @since 3.0
     */
    MPEngPresenceAttrModelTypeImp* NewAttributeInstanceLC( TBool aUserOwnAttribute ) const;




private: //Attribute model implementation

    /**
     * WV User Availability attribute implementation.
     *
     * @lib PEngAttrLib
     * @since 3.0
     */
    NONSHARABLE_CLASS( CPEngWVUserAvailabilityModel ) : public CPEngWVAttributeModelBase
        {
public:  // Constructors and destructor

        /**
         * Destructor.
         */
        ~CPEngWVUserAvailabilityModel();


        /**
         * C++ constructor.
         */
        CPEngWVUserAvailabilityModel( TBool aUserOwnAttribute );


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

private: //Data

        //OWN: Availability
        TPEngWVUserAvailabilityValues iAvailability;
        };

    };

#endif      //CPENGWVUSERAVAILABILITYCONSTRUCT_H

//  End of File

