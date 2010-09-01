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
* Description:  Attribute model constuctor.
*
*/

#ifndef CPENGPRESENCEATTRCONSTRUCTOR_H
#define CPENGPRESENCEATTRCONSTRUCTOR_H


//  INCLUDES
#include "MPEngPresenceAdvancedAttrModel2.h"
#include "MPEngPresenceAttrManager.h"
#include <MPEngPresenceAttrTypeProperties2.h>
#include <E32Base.h>


// FORWARD DECLARATIONS
class MPEngPresenceAttrConstructorTypeImp;
class CPEngAttrConstructorCon;


//PANIC CATEGORY & REASONS

/**
 * Presence Attribute Constructor panic category.
 * @since 3.0
 */
_LIT( KPEngAttributeConstructorPanicCategory, "PEngAttrConstruct" );



/**
 * Presence Attribute Constructor panic reasons.
 * @since 3.0
 */
enum TPEngAttributeConstructorPanics
    {
    EPEngAttributeTypeIDNotUnique = 1,
    EPEngLocalAttributeTypeProperties = 2,
    EPEngNetworAttributeTypeProperties = 3
    };




/**
 * Attribute model constuctor.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngPresenceAttrConstructor ) : public CBase,
        public MPEngPresenceAttrTypeProperties2
    {
public: // Construction

    /**
     * Instantiates CPEngPresenceAttrConstructor object.
     *
     * @param aTypeConstruct The attribute type specific construct.
     * @return New CPEngPresenceAttrConstructor instance.
     */
    static CPEngPresenceAttrConstructor* NewL(
        CPEngAttrConstructorCon& aContainer,
        MPEngPresenceAttrConstructorTypeImp& aTypeConstructor,
        TUint32 aType,
        TPEngAttributeOriginator aOrigin );

    /**
     * Destructor.
     * Virtual by CBase.
     */
    ~CPEngPresenceAttrConstructor();



private:

    /**
     * C++ default constructor.
     */
    CPEngPresenceAttrConstructor( CPEngAttrConstructorCon& aContainer,
                                  MPEngPresenceAttrConstructorTypeImp& aTypeConstructor,
                                  TUint32 aType,
                                  TPEngAttributeOriginator aOrigin );



public: // New methods


    /**
     * Creates a new attribute model instance.
     *
     * @since 3.0
     * @param aUserModel Is this a user model on not.
     * @param aPresenceID The attribute owners presence ID.
     * @return New attribute model instance.
     */
    MPEngPresenceAdvancedAttrModel2* NewAttributeInstanceLC(
        TBool aUserModel,
        const TDesC& aPresenceID );


    /**
     * Opens the reference to attribute model constructor.
     *
     * @since 3.0
     */
    void OpenRef();


    /**
     * Closes reference to attribute model constructor.
     *
     * @since 3.0
     */
    void CloseRef();


    /**
     * Gets attribute model XML names for currently used
     * WV CSP version.
     *
     * @since 3.0
     * @param aAttributeName On the return contains attribute
     * current XML name.
     * @param aAttributeNameSpace On the return contains attribute
     * current XML namespace.
     * @return KErrNone if names succesfully found for current
     * CSP version. Else KErrNotFound.
     */
    TInt GetCurrentXmlNames( TPtrC8& aAttributeName,
                             TPtrC8& aAttributeNameSpace ) const;




public: // Functions from MPEngPresenceAttrTypeProperties2


    /**
     * Implementation from MPEngPresenceAttrTypeProperties2.
     *
     * @since 3.0
     */
    TUint32 Type() const ;
    TPEngAttributeOriginator Originator() const;

    TInt GetProperty( TUint aPropertyName,
                      TUint aPropertyLevel,
                      TInt& aProperty ) const;

    TInt GetProperty( TUint aPropertyName,
                      TUint aPropertyLevel,
                      TDes8& aProperty ) const;

    TInt GetPropertyPtr( TUint aPropertyName,
                         TUint aPropertyLevel,
                         TPtrC16& aProperty ) const;


    /**
     * Extensions. Currently NULL.
     *
     * @since 3.0
     */
    MPEngPresenceAttrTypePropertiesExt2* Extension();
    TAny* TypeExtension( TUint32 aType );



public:   //Assertion handlers

    /**
     * Panic assertion handler.
     *
     * Panics on debug builds with given reason and leaves on
     * release builds with given leave code.
     *
     * @since 3.0
     * @param aReason Panic reason.
     * @param aLeaveCode Leave code.
     */
    void __DbgPanicL( TPEngAttributeConstructorPanics aReason,
                      TInt aLeaveCode ) const;



public:   //Data

    //OWN: Attribute type, change not allowed
    //Public to allow access from TArray and attribute model
    const TUint32   iType;


private:    // Data

    //REF: Container owning constructor
    CPEngAttrConstructorCon&    iContainer;

    //OWN: Attribute type specific constructor
    MPEngPresenceAttrConstructorTypeImp*    iTypeConstructor;

    //OWN: Attribute type data originator, change not allowed
    const TPEngAttributeOriginator      iOrigin;


    };

#endif      // CPENGPRESENCEATTRCONSTRUCTOR_H

//  End of File


