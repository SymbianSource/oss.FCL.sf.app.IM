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
* Description:  WV Comm Cap attribute constructor.
*
*/

#ifndef CPENGWVCOMMCAPCONSTRUCT_H
#define CPENGWVCOMMCAPCONSTRUCT_H

//  INCLUDES
#include    "CPEngWVAttributeConstructBase.h"
#include    "CPEngWVAttributeModelBase.h"
#include    <PEngWVPresenceAttributes2.h>
#include    <E32Base.h>



/**
 * WV Comm Cap attribute constructor.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngWVCommCapConstruct ) : public CPEngWVAttributeConstructBase
    {
public:  // Constructors and destructor

    /**
     * C++ constructor.
     */
    CPEngWVCommCapConstruct( TPEngWVCspVersion aCurrentCspVer );


    /**
     * Destructor.
     */
    virtual ~CPEngWVCommCapConstruct();



public: // Functions from MPEngPresenceAttrConstructorTypeImp

    /**
     * From MPEngPresenceAttrConstructorTypeImp.
     *
     * @since 3.0
     */
    MPEngPresenceAttrModelTypeImp* NewAttributeInstanceLC( TBool aUserOwnAttribute ) const;




private: //Attribute model implementation

    /**
     * WV Comm Cap attribute implementation.
     *
     * @lib PEngAttrLib
     * @since 3.0
     */
    NONSHARABLE_CLASS( CPEngWVCommCapModel ) : public CPEngWVAttributeModelBase
        {
public:  // Constructors and destructor

        /**
         * Destructor.
         */
        ~CPEngWVCommCapModel();


        /**
         * C++ constructor.
         */
        CPEngWVCommCapModel( TBool aUserOwnAttribute );


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
         * Helper method to set certain
         * communication capability status value.
         *
         * @param aCapabilityHolder The holder for capability.
         * @param aNewStatus The new capability status.
         */
        void DoSetCapabilityStatusL(
            TPEngWVCommunicationCapabilityValues& aCapabilityHolder,
            TInt aNewStatus );


private: //Data

        //OWN: IM communication capability
        TPEngWVCommunicationCapabilityValues    iIMCapability;

        };

    };

#endif      //CPENGWVCOMMCAPCONSTRUCT_H

//  End of File

