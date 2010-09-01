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
* Description:  NWSessionSlotID implementation.
*
*/

#ifndef __CPENGSESSIONSLOTID_H__
#define __CPENGSESSIONSLOTID_H__

//  INCLUDES
#include <E32Base.h>


// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;



/**
 * NWSessionSlotID implementation.
 *
 * @since 3.0
 */
class CPEngSessionSlotId : public CBase
    {
    public: /* Construction */


        /**
         * Instantiates CPEngSessionSlotId object.
         *
         * @return New CPEngSessionSlotId instance.
         */
        IMPORT_C static CPEngSessionSlotId* NewL();
        IMPORT_C static CPEngSessionSlotId* NewLC();

        /**
         * Instantiates a new copy from
         * CPEngSessionSlotId object.
         *
         * @return New CPEngSessionSlotId copy.
         */
        IMPORT_C CPEngSessionSlotId* CloneL() const;
        IMPORT_C CPEngSessionSlotId* CloneLC() const;

        /**
         * Copies CPEngSessionSlotId contents from
         * source object.
         *
         * @param CPEngSessionSlotId source.
         */
        IMPORT_C void CopyL( const CPEngSessionSlotId& aSource );


        /**
         * Reset.
         */
        IMPORT_C void Reset();


        /**
         * Destructor.
         */
        ~CPEngSessionSlotId();

    protected:

        /**
         * C++ constructor.
         */
        CPEngSessionSlotId();



    public: // NWSessionSlotID implementation


        /**
         * Sets the NWSessionSlotID sub-part.
         *
         * @param aAddress Service address to use.
         * @param aUserId User id to use.
         * @param aAppId AppId to use.
         */
        IMPORT_C void SetServiceAddressL( const TDesC& aAddress );
        IMPORT_C void SetUserIdL( const TDesC& aUserId );

        /**
         * Fragment access.
         *
         * @return Requested sub-part.
         */
        IMPORT_C TPtrC ServiceAddress() const;
        IMPORT_C TPtrC UserId() const;




    public: // Match support

        /**
         * Pattern-matches NWSessionSlotIDs.
         *
         * @param aIdToMatch The NWSessionSlotID to match.
         * @return KErrNone if NWSessionSlotIDs match.
         * Else KErrNotFound.
         */
        IMPORT_C TInt Match( const CPEngSessionSlotId& aIdToMatch ) const;




    public: // Serialized presentation

        /**
         * Packs CPEngSessionSlotId state to buffer and
         * returns buffer to caller.
         * @return CPEngSessionSlotId state buffer.
         */
        IMPORT_C HBufC8* PackLC() const;


        /**
         * Initializes CPEngSessionSlotId state from a package
         * previously created with PackLC().
         *
         * @param aPack CPEngSessionSlotId state package.
         */
        IMPORT_C void UnPackL( const TDesC8& aPack );


        /**
         * Externalizes the data.
         *
         * @param aStream Stream where to write.
         */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;


        /**
         * Internalizes the data.
         *
         * @param aStream Stream where to read from.
         */
        IMPORT_C void InternalizeL( RReadStream& aStream );


        /**
         * Returns size of the buffer needed for the streaming
         *
         * @since 3.0
         * @return size of the buffer in bytes
         */
        IMPORT_C TInt Size() const;


    private: //Data

        //OWN: NWSessionSlotID service address sub-part
        HBufC* iServiceAddress;

        //OWN: NWSessionSlotID user id sub-part
        HBufC* iUserId;

    };

#endif  //__CPENGSESSIONSLOTID_H__

// End of File
