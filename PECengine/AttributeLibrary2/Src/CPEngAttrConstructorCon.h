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
* Description:  Presence attribute constructor container.
*
*/

#ifndef CPENGATTRCONSTRUCTORCON_H
#define CPENGATTRCONSTRUCTORCON_H

//  INCLUDES
#include <E32Base.h>
#include <MPEngPresenceAttrTypeProperties2.h>



//FORWARD DECLARATION
class CPEngPresenceAttrConstructor;
class MPEngPresenceAttrConstructorTypeImp;
class CPEngSessionSlotObject;



// CLASS DECLARATION
/**
 * Presence attribute model constructor container.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttrConstructorCon ) : public CBase
    {
public: /* Construction */


    /**
     * Instantiates CPEngAttrConstructorCon object.
     *
     * @return New CPEngAttrConstructorCon instance.
     */
    static CPEngAttrConstructorCon* NewL( CPEngSessionSlotObject& aOwner );


    /**
     * Destructor.
     */
    ~CPEngAttrConstructorCon();


private:

    /**
     * C++ constructor.
     */
    CPEngAttrConstructorCon( CPEngSessionSlotObject& aOwner );



public: // Constructor management


    /**
     * Adds new attribute type constructor.
     * Takes always the ownership of given aTypeConstructor object.
     *
     * @param aTypeConstructor The type constructor.
     * @param aType The attribute type.
     * @param aOrigin The attribute origin setting.
     */
    void AddConstructorL( MPEngPresenceAttrConstructorTypeImp* aTypeConstructor,
                          TUint32 aType,
                          TPEngAttributeOriginator aOrigin );

    /**
     * Finds a constructor by attribute type.
     *
     * @param aType The attribute type.
     * @return The attribute constructor. NULL if no constructor
     * found for requested type. Constructor ownership stays
     * on the container.
     */
    CPEngPresenceAttrConstructor* FindByType( TUint32 aType );


    /**
     * Finds a constructor by attribute type.
     * Leaves if the attribute type is unknown.
     *
     * @param aType The attribute type.
     * @return The attribute constructor.
     * Constructor ownership stays on the container.
     */
    CPEngPresenceAttrConstructor& FindByTypeL( TUint32 aType );


    /**
     * Gets a TArray presenting known attribute types.
     *
     * @return The TArray of known attribute types.
     */
    TArray<TUint32> TypeArray() const;




private:    // Reference access for CPEngPresenceAttrConstructor

    /**
     * CPEngPresenceAttrConstructor is a friend
     * so it can increase / decrease reference count when needed.
     *
     * @since 3.0
     */
    friend class CPEngPresenceAttrConstructor;      // CSI: 36 #


    /**
     * Opens a reference to container.
     *
     * @since 3.0
     */
    void OpenRef();


    /**
     * Closes a reference to container.
     *
     * @since 3.0
     */
    void CloseRef();




private:    // Helpers


    /**
     * Assertion function to verify attribute
     * constructor contents.
     * @param aConstructor The constructor instance to verify.
     */
    void __AssertConstructorValidL( const CPEngPresenceAttrConstructor& aConstructor );


    /**
     * TArray accessor functions for attribute type list.
     */
    static TInt GetTypeCount( const CBase* aPtr );
    static const TAny* GetTypePtr( const CBase* aPtr, TInt aIndex );



private:    // Data

    //REF: The Presence Attribute Manger owning this container
    CPEngSessionSlotObject&  iOwner;


    //OWN: The attribute constructors.
    RPointerArray< CPEngPresenceAttrConstructor > iConstructors;

    };



#endif      // CPENGATTRCONSTRUCTORCON_H

// End of File
