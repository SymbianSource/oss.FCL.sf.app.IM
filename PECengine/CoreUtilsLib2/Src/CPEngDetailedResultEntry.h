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
* Description:  Detailed entry implementation.
*
*/

#ifndef __CPENGDETAILEDRESULTENTRY_H__
#define __CPENGDETAILEDRESULTENTRY_H__

//  INCLUDES
#include <E32Base.h>
#include "MPEngTransactionStatus2.h"
#include "CPEngDetailedResultEntry.h"


//FORWARD DECLARATION
class RReadStream;
class RWriteStream;
class CPEngTransactionStatus;


// CLASS DECLARATION

/**
 * Detailed entry implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngDetailedResultEntry ) : public CBase,
        public MPEngDetailedResultEntry2
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * Leaves object to CleanupStack.
    *
    * @since 3.0
    * @param aOperationID Operation ID.
    * @param aDetErr    Error code.
    * @param aAttribID  Attribute ID. Can be also NULL if not defined.
    * @param aPresID    Presence ID. Can be also NULL if not defined.
    * @param aContactListID Contact list ID. Can be also NULL if not defined.
    * @param aDescription Error description. Can be also NULL if not defined.
    * @return New CPEngDetailedResultEntry object.
    */
    static CPEngDetailedResultEntry* NewLC( TInt aOperationID,
    TInt aDetErr,
    const TUint32* aAttribID,
    const TDesC* aPresID,
    const TDesC* aContactListID,
    const TDesC* aDescription );

    /**
     * Two-phased constructor, initialises this object from
     * given stream and leaves object to CleanupStack.
     *
     * @since 3.0
     * @param aStream The read stream where from to initialize object.
     */
    static CPEngDetailedResultEntry* NewLC( RReadStream& aStream );


    /**
     * Two-phased constructor, initialises this object from
     * given existing detailed result entry. Leaves object
     * to CleanupStack.
     *
     * @since 3.0
     * @param aStream The detailed result where from to initialize object.
     */
    static CPEngDetailedResultEntry* NewLC( const CPEngDetailedResultEntry& aEntry );


    /**
     * Destructor.
     */
    virtual ~CPEngDetailedResultEntry();



private: //constructors

    /**
     * C++ default constructor.
     */
    CPEngDetailedResultEntry();


    /**
     * Symbian constructor.
     */
    void ConstructL( TInt aOperationID,
                     TInt aDetErr,
                     const TUint32* aAttribID,
                     const TDesC* aPresID,
                     const TDesC* aContactListID,
                     const TDesC* aDescription );

    /**
     * Symbian constructor.
     */

    void ConstructL( const CPEngDetailedResultEntry& aEntry );



public: // Functions from MPEngDetailedResultEntry

    /**
     * From MPEngDetailedResultEntry
     */
    TInt Operation() const;
    TInt Error() const;

    TInt GetDetailedDesc( TPtrC& aDescriptionText,
                          TPEngDetailedResultDescriptionText2 aDescriptionID ) const;
    TInt GetDetailedInt( TUint32& aDescriptionInt,
                         TPEngDetailedResultDescriptionInteger2 aDescriptionID ) const;

    MPEngDetailedResultEntryExtension2* EntryExtension();


public: //Streaming support

    /**
     * Internalizes this objects state from given
     * read stream.
     *
     * @since 3.0
     * @param aStream The stream to read from.
     */
    void InternalizeL( RReadStream& aStream );


    /**
     * Externalizes this objects state to given write stream.
     *
     * @since 3.0
     * @param aStream The stream to write to.
     */
    void ExternalizeL( RWriteStream& aStream ) const;


    /**
     * Approximates the needed stream length needed to
     * externalize the container data.
     *
     * @since 3.0
     * @return Returns approximation of needed space in bytes.
     */
    TInt ExternalizeSize() const;


public: //Streaming helpers

    /**
     * Gets HBufC. Returns either buffer/integer content
     * on passed aBuffer or empty descriptor if the passed buffer pointer is NULL.
     * If the passed buffer pointer is NULL, returns KErrNotFound. Else KErrNone.
     */
    static TInt DoReturnBuffer( const HBufC* const & aPtr, TPtrC16& aBuffer );
    static TInt DoReturnInteger( const TUint32* aPtr, TUint32& aInteger );


    /**
     * Externalizes passed buffer/ integer to given write stream.
     * If buffer is NULL, externalizes just EPEngNullField information.
     */
    static void DoExternalizeBufferOrNullL( const HBufC* aBuffer, RWriteStream& aStream );
    static void DoExternalizeIntegerOrNullL( TUint32* aInteger, RWriteStream& aStream );


    /**
     * Internalizes buffer / integer from given read stream.
     * If no buffer isn't contained next on the stream, creates NULL buffer.
     * If internalization fails, leaves the destination buffer untouched.
     */
    static void DoInternalizeBufferOrNullL( HBufC*& aBuffer, RReadStream& aStream );
    static void DoInternalizeIntegerOrNullL( TUint32*& aInteger, RReadStream& aStream );


    /**
     * Approximates passed buffer/ integer stream write size.
     */
    static TInt DoBufferExternalizeSizeInBytes( const HBufC* aBuffer );
    static TInt DoIntegerExternalizeSizeInBytes( TUint32* aInteger );


private:    // Friend declarations

    /**
     * Friend class.
     * @since 3.0
     */
    friend class CPEngTransactionStatus;        // CSI: 36 #


private:    // Data

    //OWN: Link
    TSglQueLink     iLink;

    //OWN: operation
    TInt                iOperationID;

    //OWN: Detailed error
    TInt                iDetErr;

    //OWN: Attribute ID
    TUint32*            iAttribID;

    //OWN: Presence ID
    HBufC*              iPresID;

    //OWN: Contact list ID
    HBufC*              iContactListID;

    //OWN: Description
    HBufC*              iDescription;

    };


#endif      //  __CPENGDETAILEDRESULTENTRY_H__


// End of File


