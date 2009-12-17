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
* Description:  Data entry implementation.
*
*/

#ifndef __CPENGDATARESULTENTRY_H__
#define __CPENGDATARESULTENTRY_H__

//  INCLUDES
#include <E32Base.h>
#include "CPEngDataResultEntry.h"




//FORWARD DECLARATION
class RReadStream;
class RWriteStream;
class CPEngTransactionStatus;


// CLASS DECLARATION

/**
 * Data entry implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngDataResultEntry ) : public CBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * Leaves object to CleanupStack.
    *
    * @since 3.0
    * @param aOperationID Operation ID.
    * @param aData The data. Takes the ownership in full success.
    * @return New CPEngDataResultEntry object.
    */
    static CPEngDataResultEntry* NewLC( TInt aOperationID,
    HBufC* aData );

    /**
     * Two-phased constructor, initialises this object from
     * given stream and leaves object to CleanupStack.
     *
     * @since 3.0
     * @aStream The read stream where from to initialize object.
     */
    static CPEngDataResultEntry* NewLC( RReadStream& aStream );


    /**
     * Two-phased constructor, initialises this object from
     * given existing detailed result entry. Leaves object
     * to CleanupStack.
     *
     * @since 3.0
     * @aStream The detailed result where from to initialize object.
     */
    static CPEngDataResultEntry* NewLC( const CPEngDataResultEntry& aEntry );


    /**
     * Destructor.
     */
    virtual ~CPEngDataResultEntry();



private: //constructors

    /**
     * C++ default constructor.
     */
    CPEngDataResultEntry();


    /**
     * Symbian constructor.
     */
    void ConstructL( const CPEngDataResultEntry& aEntry );



public: // New functions

    /**
     * Gets the operation id.
     *
     * @return The operation id.
     */
    TInt Operation() const;


    /**
     * Gets the result data.
     *
     * @return The result data.
     */
    const TDesC16& Data() const;



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



private:    // Friend declarations

    /**
     * Friend class.
     * @since 3.0
     */
    friend class CPEngTransactionStatus;        // CSI: 36 #




private:    // Data

    //OWN: Link
    TSglQueLink iLink;

    //OWN: Operation id
    TInt    iOperationID;

    //OWN: Data result
    HBufC16*    iData;

    };


#endif      //  __CPENGDATARESULTENTRY_H__


// End of File


