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
* Description:  Container for individual key-value pairs.
*
*/

#ifndef __CIMPSSAPKEYVALUEPAIRS_H
#define __CIMPSSAPKEYVALUEPAIRS_H

//  INCLUDES
#include "MIMPSSapObject.h"
#include "CIMPSSAPKeyValuePair.h"

#include <e32base.h>
#include <s32strm.h>


// FORWARD DECLARATIONS
class CIMPSSAPLookupKeyValuePair;


// CLASS DECLARATION
/**
 * Container for individual key-value pairs.
 *
 * @since 3.0
 **/
NONSHARABLE_CLASS( CIMPSSAPKeyValuePairs ): public CBase, public MIMPSSapObject
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     */
    static CIMPSSAPKeyValuePairs* NewL();


    /**
     * Destructor.
     */
    virtual ~CIMPSSAPKeyValuePairs();


private:  // Constructors


    /**
     * C++ default constructor.
     */
    CIMPSSAPKeyValuePairs();

    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: //Generic object interface from MIMPSSapObject


    TIMPSSapObjectID ObjectID() const;
    TInt ObjectVersion() const;
    void ExternalizeVersionDataL( TInt aVersion, RWriteStream& aStream ) const;
    void InternalizeVersionDataL( TInt aVersion, RReadStream& aStream );
    TInt DataSize() const;
    void Reset();


public: // Key value access


    /**
     * Key-value setters.
     *
     * Different methods to support integer type and
     * narrow and wide descriptors.
     *
     * @since 3.0
     * @param aKey The key to use identify the value.
     * @param aValue The value to assign for keyword.
     * @return Error code. One of following:
     * KErrNone: Value assigned/updated succesfully.
     * KErrGeneral: Different data type already assigned for given key.
     * KErrNoMemory: No memory to allocate the new value.
     *               Original value for key remains if assigning
     *               value fails,
     * KErrTooBig: Key and value are too big for storing. Original value
     *			   remains.
     *
     */
    TInt SetValueInt( const TDesC& aKey, TInt aValue );
    TInt SetValueDesC8( const TDesC& aKey, const TDesC8& aValue );
    TInt SetValueDesC16( const TDesC& aKey, const TDesC16& aValue );


    /**
     * Key-value getters.
     *
     * Different methods to support integer type and
     * narrow and wide descriptors.
     *
     * @since 3.0
     * @param aKey The key to use identify the value.
     * @param aValue The value to get. Given aValue isn't
     * changed if reading fails,
     * @return Error code. One of following:
     * KErrNone: Value read succesfully.
     * KErrNotFound: No value yet assigned.
     * KErrGeneral: Different data type assigned for given key.
     */
    TInt GetValueInt( const TDesC& aKey, TInt& aValue );
    TInt GetValueDesC8( const TDesC& aKey, TPtrC8& aValue );
    TInt GetValueDesC16( const TDesC& aKey, TPtrC16& aValue );


    /**
     * Key value remove.
     *
     * @since 3.0
     * @param aKey The key to remove.
     * @return Error code. One of following:
     * KErrNone: Key value removed succesfully.
     * KErrNotFound: Key value not found.
     */
    TInt DeletePair( const TDesC& aKey );

    /**
     * Return Value-pair array.
     *
     * @since 3.0
     * @return Array of value-pairs:
     */
    const RPointerArray< CIMPSSAPKeyValuePair >& Pairs() const;
    RPointerArray< CIMPSSAPKeyValuePair >& Pairs();


private:    // Data

    mutable CIMPSSAPLookupKeyValuePair*   iLookupKey;  //OWN: Lookup pair

    RPointerArray< CIMPSSAPKeyValuePair > iPairs;      //OWN: Pairs
    };

#endif      // __CIMPSSAPKEYVALUEPAIRS_H


//  End of File



