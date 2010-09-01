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

#ifndef CPENGWVSTATUSTEXTCONSTRUCT_H
#define CPENGWVSTATUSTEXTCONSTRUCT_H

//  INCLUDES
#include    "CPEngWVAttributeConstructBase.h"
#include    "CPEngWVAttributeModelBase.h"
#include    <PEngWVPresenceAttributes2.h>
#include    <E32Base.h>



/**
 * WV Status Text attribute constructor.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngWVStatusTextConstruct ) : public CPEngWVAttributeConstructBase
    {
public:  // Constructors and destructor

    /**
     * C++ constructor.
     */
    CPEngWVStatusTextConstruct( TPEngWVCspVersion aCurrentCspVer );


    /**
     * Destructor.
     */
    virtual ~CPEngWVStatusTextConstruct();



public: // Functions from MPEngPresenceAttrConstructorTypeImp

    /**
     * From MPEngPresenceAttrConstructorTypeImp.
     *
     * @since 3.0
     */
    MPEngPresenceAttrModelTypeImp* NewAttributeInstanceLC( TBool aUserOwnAttribute ) const;




public: //Attribute model implementation

    /**
     * WV Status Text attribute implementation.
     *
     * @lib PEngAttrLib
     * @since 3.0
     */
    NONSHARABLE_CLASS( CPEngWVStatusTextModel ) : public CPEngWVAttributeModelBase
        {
public:  // Constructors and destructor

        /**
         * Destructor.
         */
        ~CPEngWVStatusTextModel();


        /**
         * C++ constructor.
         */
        CPEngWVStatusTextModel( TBool aUserOwnAttribute );


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

        //OWN: Status text
        HBufC* iStatusText;

        };
    };

#endif      //CPENGWVSTATUSTEXTCONSTRUCT_H


//  End of File


