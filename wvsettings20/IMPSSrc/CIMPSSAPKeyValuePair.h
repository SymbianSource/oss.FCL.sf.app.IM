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
* Description:  Key - value container.
*
*/

#ifndef __CIMPSSAPKEYVALUEPAIR_H
#define __CIMPSSAPKEYVALUEPAIR_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

// CLASS DECLARATION
/**
 * Key - value container.
 * Can be derived to expand services.
 *
 * @since 3.0
 **/
NONSHARABLE_CLASS( CIMPSSAPKeyValuePair ): public CBase
    {
public:  // Enumerations

    /**
     * Data type enumeration.
     * List supported value data types.
     * @since 3.0
     **/
    enum TValueType
        {
        EValueTypeNone = 0,
        EValueTypeInt,
        EValueTypeDesC8,
        EValueTypeDesC16
        };


public:  // Constructors and destructor

    /**
     * Non leaving two phased constructor.
     *
     * If object allocation fails, returns NULL.
     *
     * @since 3.0
     * @param aKey The key.
     */
    static CIMPSSAPKeyValuePair* New( const TDesC& aKey );

    /**
     * Two phased constructor initializing object state
     * from the stream.
     *
     * @since 3.0
     * @param aStream The stream to read object state.
     */
    static CIMPSSAPKeyValuePair* NewLC( RReadStream& aStream );

    /**
     * Two phased constructor initializing object state
     * from given flat descriptor.
     *
     * @since 3.0
     * @param aKeyValuePairFlat The flat representation of key-value pair
     */

    static CIMPSSAPKeyValuePair* NewLC( const TDesC& aKeyValuePairFlat );

    /**
     * Destructor.
     */
    virtual ~CIMPSSAPKeyValuePair();



protected:  // Constructor

    /**
     * C++ default constructor.
     */
    CIMPSSAPKeyValuePair();


    /**
     * Symbian OS constructor.
     * Initializes object state from given stream.
     *
     * @since 3.0
     * @param aStream The stream to read object state.
     */
    void ConstructL( RReadStream& aStream );


public:  //Data access

    /**
     * Key accessor.
     *
     * Gets the key associated with key-value object.
     * Virtual to allow derived classes to expand behaviour.
     *
     * @since 3.0
     * @return The key.
     */
    virtual const TDesC& Key() const;


    /**
     * Data setters.
     *
     * Overloaded to support integer type and
     * narrow and wide descriptors.
     *
     * @since 3.0
     * @param aValue The value to assign for key-value object.
     * @return Error code. One of following:
     * KErrNone: Value assigned/updated succesfully.
     * KErrGeneral: Different data type already assigned.
     * KErrNoMemory: No memory to allocate the new value.
     * Original value remains if assigning new value fails.
     */
    TInt SetValue( TInt aValue );
    TInt SetValue( const TDesC8& aValue );
    TInt SetValue( const TDesC16& aValue );


    /**
     * Data getters.
     *
     * Overloaded to support integer type and
     * narrow and wide descriptors.
     *
     * @since 3.0
     * @param aValue The value to get.
     * @return Error code. One of following:
     * KErrNone: Value read succesfully.
     * KErrNotFound: No value yet assigned.
     * KErrGeneral: Different data type assigned.
     * Given aValue isn't changed if reading fails.
     */
    TInt GetValue( TInt& aValue ) const;
    TInt GetValue( TPtrC8& aValue ) const;
    TInt GetValue( TPtrC16& aValue ) const;


    /**
     * Key-Value pair flat representation
     *
     * Creates flat representation of the key-value pair
     * and pushes it in cleanup stack.
     * Ownership is transferred to calling method.
     *
     * @since 3.0
     * @return The flat representation of key-value pair
     */

    HBufC* KeyValuePairFlatLC();


public:  // Streaming support


    /**
     * Externalises the value pair to given stream.
     *
     * @since 3.0
     * @param aStream stream to write to.
     */
    void ExternalizeL( RWriteStream& aStream ) const;


    /**
     * Approximates the value pair data size.
     *
     * @since 3.0
     * @return The value pair data size.
     */
    TInt DataSize() const;

    /**
     * Approximates the value pair data size
     * in decimal textual representation
     *
     * @since 3.0
     * @return The value pair data size.
     */
    TInt DataSizeDec() const;

    /**
    * Checks if the data fits in the flat
    * descriptor
    *
    * @since 3.0
    * @return ETrue if data is too big.
    */

    TBool IsDataTooBig() const;

private:    // Help functions

    /**
     * Resets the value pair data.
     * @since 3.0
     */
    void Reset();

    /**
     * Parses value pair data from flat descriptor.
     * @param aKeyValuePairFlat flat descriptor to parse
     * @since 3.0
     */
    void ParseFlatKeyValuePairL( const TDesC& aKeyValuePairFlat );

private:    // Data


    HBufC16*        iKey;           //OWN: The key.

    TValueType      iValueType;     //OWN: The value type.

    union                           //OWN: The value. Value type is determined from iValueType.
        {
        TInt        iInt;
        HBufC8*     iBuf8;
        HBufC16*    iBuf16;
        };

    };

#endif      // __CIMPSSAPKEYVALUEPAIR_H


//  End of File

