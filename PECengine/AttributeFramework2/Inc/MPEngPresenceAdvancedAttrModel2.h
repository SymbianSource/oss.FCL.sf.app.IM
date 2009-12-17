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
* Description:  Advanced Presence Attribute interface.
*
*/

#ifndef __MPENGPRESENCEADVANCEDATTRMODEL2_H
#define __MPENGPRESENCEADVANCEDATTRMODEL2_H

//  INCLUDES
#include "MPEngPresenceAttrModel2.h"
#include <E32Std.h>


//FORWARD DECLARATIONS
class MPEngXMLParser;
class MPEngXMLSerializer;
class RWriteStream;
class RReadStream;
class CPEngPresenceAttrModelStorageProxy;





// CLASS DECLARATION

/**
* Advanced attribute model interface for Presence Engine
* internal use.
*
* This class defines the internal advanced interface
* for concrete Presence Attribute Models. This interface is
* used inside of the Presence Engine and this allows variety
* of extended operations to be performed for attribute models.
*
* @since 3.0
*/
class MPEngPresenceAdvancedAttrModel2 : public MPEngPresenceAttrModel2
    {
    public: //XML handling

        /**
         * Encodes attribute value to XML.
         *
         * @since 3.0
         * @param aSerializer The serializer where to write the XML.
         */
        virtual void EncodeToXmlL( MPEngXMLSerializer& aSerializer ) const = 0;


        /**
         * Decodes attribute value from XML.
         *
         * @since 3.0
         * @param aParser The parser to use in parsing.
         * @param aDataBlock The XML block containing attribute data.
         */
        virtual void DecodeFromXmlL( MPEngXMLParser& aParser,
                                     const TDesC8& aDataBlock ) = 0;


    public: //Storage support

        /**
         * Access to attribute model storage proxy.
         * @since 3.0
         * @return Attribute model storage proxy.
         */
        virtual const CPEngPresenceAttrModelStorageProxy& Storage() const = 0;
        virtual CPEngPresenceAttrModelStorageProxy& Storage() = 0;




    public: //Serialization

        /**
         * Gets the byte size needed to to externalize attribute model.
         * @since 3.0
         * @return Byte size needed to externalize the attribute model.
         */
        virtual TInt ExternalizeSize() const  = 0;


        /**
         * Externalizes attribute model data.
         *
         * @since 3.0
         * @param aStream The stream to write.
         */
        virtual void ExternalizeL( RWriteStream& aStream ) const = 0;



        /**
         * Internalizes attribute model data.
         *
         * @since 3.0
         * @param aStream The stream read from.
         */
        virtual void InternalizeL( RReadStream& aStream ) = 0;




    public: //Network commits and session state handling


        /**
         * Sets attribute data source as network.
         * @since 3.0
         */
        virtual void HandleAttributeUpdatedToNetwork() = 0;


        /**
         * Handles network session status change.
         * @since 3.0
         * @param aSessionOpen If ETrue, network presence session
         * was opened. If EFalse, network presence session was closed.
         */
        virtual void HandleNetworkSessionStateChangeL( TBool aSessionOpen ) = 0;




    public: //destructor

        /**
         * Inline virtual destructor.
         * Concrete objects can be deleted through
         * this interface.
         */
        virtual ~MPEngPresenceAdvancedAttrModel2() {};

    };

#endif      //__MPENGPRESENCEADVANCEDATTRMODEL2_H


//  End of File


