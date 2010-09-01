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
* Description:  Property manager and value container
*
*/


#ifndef __CPENGCONTACTLISTPROPERTY_H__
#define __CPENGCONTACTLISTPROPERTY_H__

//  INCLUDES
#include    <e32base.h>

// FORWARD DECLARATION
class CPEngContactListProperty;
class RWriteStream;
class RReadStream;

// ENUMERATIONS

/**
 * The property types.
 */
enum TPEngPropertyType
    {
    ETInt    = 0,
    EDes8    = 1,
    EDes16   = 2
    };



//  CLASS DEFINITION
/**
 * Property manager.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( PEngPropertyManager )
    {
public: //New functions.

    /**
     * Gets a property value.
     *
     * @since 3.0
     * @param  aName Name of the property.
     * @param  aLevel Level of the property to look for.
     * @param  aValue Where value is be stored.
     * @return KErrNone if property was found. Else KErrNotFound.
     */
    static TInt GetProperty(
        const RPointerArray<CPEngContactListProperty>& aProperties,
        TUint aName,
        TUint aLevel,
        TInt& aValue );

    static TInt GetProperty(
        const RPointerArray<CPEngContactListProperty>& aProperties,
        TUint aName,
        TUint aLevel,
        TPtrC8& aValue );

    static TInt GetProperty(
        const RPointerArray<CPEngContactListProperty>& aProperties,
        TUint aName,
        TUint aLevel,
        TPtrC16& aValue );


    /**
     * Shortcut for getting integer type properties.
     *
     * @since 3.0
     * @param  aName Name of the property.
     * @param  aLevel Level of the property to look for.
     * @return if property is found, its value, if not found
     *          zero is returned as default value
     */
    static TInt PropertyOrZero(
        const RPointerArray<CPEngContactListProperty>& aProperties,
        TUint aName,
        TUint aLevel );

    /**
     * Sets a property.
     *
     * @since 3.0
     * @param  aName Name of the property.
     * @param  aLevel Level of the property to set.
     * @param  aValue Value to be set.
     * @return KErrNone if property was set successfully.
     *         System wide error case if something failed.
     */
    static void SetPropertyL(
        RPointerArray<CPEngContactListProperty>& aProperties,
        TUint aName,
        TUint aLevel,
        TInt aValue,
        TInt& aSize );

    static void SetPropertyL(
        RPointerArray<CPEngContactListProperty>& aProperties,
        TUint aName,
        TUint aLevel,
        const TDesC8& aValue,
        TInt& aSize );

    static void SetPropertyL(
        RPointerArray<CPEngContactListProperty>& aProperties,
        TUint aName,
        TUint aLevel,
        const TDesC16& aValue,
        TInt& aSize );


    /**
     * Delete a property
     *
     * @since 3.0
     * @param aName name of the property to delete
     * @param aLevel level of the property to delete
     * @return ETrue if property was found and deleted. Else EFalse.
     */
    static TBool DeletePropertyL(
        RPointerArray<CPEngContactListProperty>& aProperties,
        TUint aName,
        TUint aLevel,
        TInt& aSize );


    /**
     * Finds property in the array of the properties
     *
     * @since 3.0
     * @param aName name of the property
     * @param aLevel level of the property
     * @param aType type of the property if it should be validated
     * @return property index
     */
    static TInt FindProperty(
        const RPointerArray<CPEngContactListProperty>& aProperties,
        TUint aName,
        TUint aLevel );

    static TInt FindProperty(
        const RPointerArray<CPEngContactListProperty>& aProperties,
        TUint aName,
        TUint aLevel,
        TPEngPropertyType aType );


    /**
     * Externalize properties
     *
     * @since 3.0
     * @param aProperties array of properties
     * @param aStream Write stream
     * @prama aStoreType storage type
     */
    static void ExternalizePropertiesL(
        const RPointerArray<CPEngContactListProperty>& aProperties,
        RWriteStream& aStream,
        TInt aStoreType );

    /**
     * Internalize properties
     *
     * @since 3.0
     * @param aProperties array to store properties
     * @param aStream read stream to read properties from
     * @param aStoreType storage type
     * @prama aSize couter to update size
     */
    static void InternalizePropertiesL(
        RPointerArray<CPEngContactListProperty>& aProperties,
        RReadStream& aStream,
        TInt& aSize );
    };




//  CLASS DEFINITION
/**
 * Property value container.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListProperty ): public CBase
    {
public:  // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CPEngContactListProperty* NewLC( TUint aName,
    TUint aLevel );

    /**
     * Two-phased constructor.
     */
    static CPEngContactListProperty* NewLC( RReadStream& aStream );

    /**
     * Destructor.
     */
    virtual ~CPEngContactListProperty();


public: // New functions

    /**
     * Name
     *
     * @since 3.0
     * @return name of the property
     */
    TUint Name() const;


    /**
     * Level of the property
     *
     * @since 3.0
     * @return
     */
    TUint Level() const;


    /**
     * Value
     *
     * @since 3.0
     * @return value of chosen type of the property
     */
    TInt ValueInt() const;
    const TDesC16& ValueDes16() const;
    const TDesC8& ValueDes8() const;


    /**
     * Set given value
     *
     * @since 3.0
     * @param aValue new value to be set
     */
    void SetValue( TUint aValue );
    void SetValueL( const TDesC16& aValue );
    void SetValueL( const TDesC8& aValue );


    /**
     * Validate property type
     *
     * @since 3.0
     * @param aType type to validate
     * @return ETrue if type is valid for the property
     */
    TBool ValidateType( TPEngPropertyType aType ) const;


    /**
     * Externalizes property to the stream
     *
     * @since 3.0
     * @param aStream Stream to write to.
     */
    void ExternalizeL( RWriteStream& aStream, TInt aType ) const;


    /**
     * Size of the entry for storing.
     *
     * @since 3.0
     * @return Entry stream size.
     */
    TInt EntrySize();


    /**
     * Internalize contact list settings from the stream
     *
     * @since 3.0
     * @param aStream stream to read from.
     */
    void InternalizeL( RReadStream& aStream );


private:

    /**
     * C++ default constructor.
     */
    CPEngContactListProperty();

    /**
     * Maps given property level to storage type.
     * @param aLevel The level to map.
     * @param Mapped storage type.
     */
    static TInt StorageTypeForLevelL( TUint aLevel );


private:    // Data

    /// OWN: Property name
    TUint                   iName;

    /// OWN: Property level
    TUint                   iLevel;

    /// OWN: Property TInt value
    TInt                    iTIntValue;

    /// OWN: TInt value validity
    TBool					iTIntValueValid;

    /// OWN: Property TDes16 value
    HBufC16*                iTDes16Value;

    /// OWN: Property TDes8 value
    HBufC8*                 iTDes8Value;
    };

#endif      //  __CPENGCONTACTLISTPROPERTY_H__
