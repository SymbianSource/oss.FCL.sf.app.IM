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
* Description:  Attribute type specific data interface.
*
*/

#ifndef __MPENGPRESENCEATTRMODELTYPEIMP_H
#define __MPENGPRESENCEATTRMODELTYPEIMP_H

//INCLUDES
#include <E32Base.h>


// FORWARD DECLARATIONS
class MPEngXMLSerializer;
class MPEngXMLParser;
class RWriteStream;
class RReadStream;


/**
 * Attribute type specific data interface.
 *
 * @since 3.0
 */
class MPEngPresenceAttrModelTypeImp
    {
    public:  //Destructor

        /**
         * Destructor.
         *
         * @since 3.0
         */
        virtual ~MPEngPresenceAttrModelTypeImp() {};


    public: // Pure virtual functions



        /**
         * Gets attribute data.
         *
         * @since 3.0
         */
        virtual TBool GetData( TInt* aIntData,
                               TPtrC8* a8Data,
                               TPtrC16* a16Data,
                               TInt aField,
                               TInt aGroup ) const = 0;


        /**
         * Sets attribute data.
         *
         * @since 3.0
         */
        virtual TBool SetDataL( TInt* aIntData,
                                const TDesC8* a8Data,
                                const TDesC16* a16Data,
                                TInt aField,
                                TInt aGroup ) = 0;


        /**
         * Sets attribute data.
         * Asynchronous version.
         *
         * @since 3.0
         */
        virtual TBool SetDataAsyncL( const TDesC8* a8Data,
                                     const TDesC16* a16Data,
                                     TRequestStatus& aRequestStatus,
                                     TInt aField,
                                     TInt aGroup ) = 0;

        /**
         * Cancels asynchronous attribute data set.
         *
         * @since 3.0
         */
        virtual void CancelSetDataAsync() = 0;


        /**
         * Gets attribute type specific extension.
         *
         * @since 3.0
         */
        virtual TAny* TypeExtension( TUint32 aType ) = 0;


        /**
         * Encodes attribute data to XML.
         *
         * @since 3.0
         */
        virtual void EncodeDataToXmlL( MPEngXMLSerializer& aSerializer ) const = 0;


        /**
         * Decodes attribute data from XML.
         *
         * @since 3.0
         */
        virtual void DecodeDataFromXmlL( MPEngXMLParser& aParser,
                                         const TDesC8& aValueBlock ) = 0;


        /**
         * Resets the attribute model data to its default values.
         *
         * @since 3.0
         */
        virtual void ResetDataL() = 0;



        /**
         * Gets the size needed to externalizes attribute
         * data to stream.
         *
         * @since 3.0
         */
        virtual TInt DataExternalizeSize() const = 0;


        /**
         * Externalizes attribute data to stream.
         *
         * @since 3.0
         */
        virtual void ExternalizeDataL( RWriteStream& aStream ) const = 0;


        /**
         * Internalizes attribute data from stream.
         *
         * @since 3.0
         */
        virtual void InternalizeDataL( RReadStream& aStream  ) = 0;


        /**
         * Handles network session status change.
         * @since 3.0
         * @param aSessionOpen If ETrue, network presence session
         * was opened. If EFalse, network presence session was closed.
         */
        virtual void HandleNetworkSessionStateChangeL( TBool aSessionOpen ) = 0;


    };

#endif      // __MPENGPRESENCEATTRMODELTYPEIMP_H

//  End of File

