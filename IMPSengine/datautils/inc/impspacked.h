/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Packing of data for IPC between client and engine server.
*     Data Format:
*          integers: memory copy all in once
*          descriptor: lenght + data bytes
*          array: nbr-elements (n) + n * length + data bytes
*
*
*/


#ifndef TImpsPackedEntity_H
#define TImpsPackedEntity_H

//  INCLUDES
#include "impsfields.h"
#include "impsdata.h"



// FORWARD DECLARATIONS
class CImpsCommonGroupPropsInt;
class CImpsPrivateGroupPropsInt;
class CImpsGroupProps;


// CLASS DECLARATION

class TImpsPackedEntity
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aBuffer buffered entity
        */
        IMPORT_C TImpsPackedEntity( HBufC8*& aBuffer );

    public:

        /**
        * Make a packed entity
        * @param aEntity entity to be packed
        * @return error code
        */
        IMPORT_C TInt PackEntity( const CImpsFields& aEntity );

        /**
        * Open a packed entity
        * @param aEntity entity where to place the data
        * @return error code
        */
        IMPORT_C void UnpackEntityL( CImpsFields& aEntity );

        /**
        * Pack array of descriptors
        */
        IMPORT_C static void DoPackArray(
            const TUint8*& aPtrStart,
            const MDesCArray* aArray );

        /**
        * Pack a single descriptor.
        * This is used in access client in this light way
        */
        static void DoPackDescriptor(
            const TUint8*& aPtrStart,
            const TDesC& aDescriptor );

        /**
        * Unpack a single descriptor
        * This is used in access client in this light way
        */
        static void DoUnpackDescriptorLC(
            const TUint8*& aPtrStart,
            HBufC*& aDescriptor );

        /**
        * Unpack array of descriptors
        */
        IMPORT_C static void DoUnpackArrayL(
            const TUint8*& aPtrStart,
            CDesCArray* aArray );

    private:

        /**
        * Unpacks general headers (and IM at the very moment)
        * @param aPtr read data pointer
        * @param aEntity internal data structure to be updated
        */
        void DoUnpackEntityL(
            TUint8*& aPtr,
            CImpsFields& aEntity );

        /**
        * Unpacks CImpsData specific data
        * @param aPtr read data pointer
        * @param aEntity internal data structure to be updated
        */
        void DoUnpackDataL(
            TUint8*& aPtr,
            CImpsData& aEntity );

        /**
        * Pack CImpsFields into flat memory
        * @param aPtrStart where to start write. Increased and returned.
        * @param aPtrEnd end of buffer
        * @return error code
        */
        TInt DoPackEntity(
            const TUint8*& aPtrStart,
            const TUint8* aPtrEnd,
            const CImpsFields& aEntity );

        /**
        * Pack CImpsData into flat memory
        * @param aPtrStart where to start write. Increased and returned.
        * @param aPtrEnd end of buffer
        * @return error code
        */
        TInt DoPackData(
            const TUint8*& aPtrStart,
            const TUint8* aPtrEnd,
            const CImpsData& aEntity );

    private:    // Data
        HBufC8*&    iBuffer;

    };

#endif      // ?INCLUDE_H   

// End of File
