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
* Description:  Attribute type array.
*
*/

#ifndef CPENGATTRIBUTETYPESTACK_H
#define CPENGATTRIBUTETYPESTACK_H

//  INCLUDES
#include <E32Base.h>


// FORWARD DECLARATIONS
class MPEngPresenceAttrManager;
class TPEngAttributeEntry;



// CLASS DECLARATION
/**
 *  Attribute type stack.
 *
 *  Encapsulates stack of holding presence attribute details.
 *  Attribute details consists from attribute type id
 *  (id number), attribute XML name string and attribute
 *  XML namespace string. Attribute type stack maintains the
 *  attributes in stack like structure. Attributes can be
 *  added to stack or the stack can be completely reseted.
 *
 *  NOTE: Attribute details stay valid only as long as
 *  the MPEngPresenceAttrManager singleton is valid for
 *  current thread. When MPEngPresenceAttrManager singleton
 *  is released, holded attribute details data is invalidated.
 *
 *  @lib PEngAttrLib
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeTypeStack ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeTypeStack* NewL();


    /**
     * Two-phased constructor.
     */
    static CPEngAttributeTypeStack* NewLC();


    /**
     * Destructor.
     */
    ~CPEngAttributeTypeStack();



private:    //Constructors

    /**
     * C++ default constructor.
     */
    CPEngAttributeTypeStack();

    /**
     * Symbian OS constructor.
     */
    void ConstructL();




public: // New functions


    /**
     * Resets the stack state.
     *
     * After calling this, attribute and namespace
     * counts are zero.
     *
     * @since 3.0
     */
    void Reset();


    /**
     * Pushes new attributes to attribute stack.
     *
     * Pushs new attributes to attribute stack.
     * Attributes are appended after existing attributes,
     * in the order of passed passed aAttributeIds array.
     * Uses given aAttributeManager to retrive attribute
     * details. (See MPEngPresenceAttrManager::GetAttributeXmlNameAndNameSpace())
     *
     * If one of the passed attributes exist allready
     * in the stack, leaves with KErrAlreadyExists.
     * If retrieving the attibute details fails, leaves with
     * errors coming from
     * MPEngPresenceAttrManager::GetAttributeXmlNameAndNameSpace()
     * If appending the attributes fail due insufficient memory,
     * leaves with KErrNoMemory. If there happens a leave,
     * attribute stack state stays untouched.
     *
     * @since 3.0
     * @param aAttributeIds ID's of the attributes to add.
     * @param aAttributeManager The presence manager to use
     * in attribute details retrieving.
     */
    void PushAttributesL( const TArray<TUint32> aAttributeIds,
                          MPEngPresenceAttrManager& aAttributeManager );


    /**
     * Pushess attribute to attribute stack.
     *
     * Pushs attribute to attribute stack.
     * Attribute is appended after existing attributes.
     * Uses given aAttributeManager to retrive attribute
     * details. (See MPEngPresenceAttrManager::GetAttributeXmlNameAndNameSpace())
     *
     * If the passed attribute allready  exists in the stack,
     * leaves with KErrAlreadyExists. If retrieving the attibute
     * details fails, leaves with errors coming from
     * MPEngPresenceAttrManager::GetAttributeXmlNameAndNameSpace()
     * If appending the attribute fail due insufficient memory,
     * leaves with KErrNoMemory. If there happens a leave,
     * attribute stack state stays untouched.
     *
     * @since 3.0
     * @param aAttributeId ID of the attribute to add.
     * @param aAttributeManager The presence manager to use
     * to retrieve attribute details.
     */
    void PushAttributeL( TUint32 aAttributeId,
                         MPEngPresenceAttrManager& aAttributeManager );

    /**
     * Count of unique namespaces.
     *
     * Gets the count of unique namespaces declared
     * by the contained attributes.
     *
     * @since 3.0
     * @return Count of unique attributes.
     */
    TInt UniqueNameSpaceCount() const;


    /**
     * Gets unique namespace.
     *
     * Gets unique name space by index.
     *
     * @since 3.0
     * @param aNameSpacesIndex Index of the namespace to get.
     * @return Reference to namespace string.
     */
    const TDesC8& UniqueNameSpace( TInt aNameSpacesIndex ) const;


    /**
     * Count of holded attribute types.
     *
     * Gets the count of contained attributes.
     *
     * @since 3.0
     * @return Count of contained attributes.
     */
    TInt TypeCount() const;


    /**
     * Gets attribute name.
     *
     * Gets attribute name by index.
     *
     * @since 3.0
     * @param aTypeCountIndex Index of the attribute which name to get.
     * @return Reference to attribute name string.
     */
    const TDesC8& Name( TInt aTypeCountIndex ) const;


    /**
     * Gets attribute namespace.
     *
     * Gets attribute namespace by index.
     *
     * @since 3.0
     * @param aTypeCountIndex Index of the attribute which namespace to get.
     * @return Reference to attribute namespace string.
     */
    const TDesC8& NameSpace( TInt aTypeCountIndex ) const;


    /**
     * Gets attribute type id.
     *
     * Gets attribute type id by index.
     *
     * @since 3.0
     * @param aTypeCountIndex Index of the attribute which type id to get.
     * @return Attribute type id.
     */
    TUint32 TypeId( TInt aTypeCountIndex ) const;


    /**
     * Generic array construction.
     *
     * Constructs the generic array presenting
     * the attribute type id's holded by the
     * CPEngAttributeTypeStack.
     * Constructed generic array is valid only
     * as long as the originating
     * CPEngAttributeTypeStack object.
     *
     * @since 3.0
     * @return Generic array presenting holded attribute
     * type ID's.
     */
    TArray<TUint32> TypeIdArray() const;



private: //Helpers


    /**
     * Pushes attribute to internal stack.
     */
    TInt DoPushAttribute( TUint32 aAttributeId,
                          MPEngPresenceAttrManager& aAttributeManager );


    /**
     * Checks is the given name space new.
     * Returns ETrue if the namespace is new,
     * else EFalse.
     */
    TBool IsNewNameSpace( const TDesC8& aNameSpace ) const;


    /**
     * Checks is the given attribute new.
     * Returns ETrue if the attribute is new,
     * else EFalse.
     */
    TBool IsNewAttribute( TUint32 aAttributeId );


    /**
     * TArray support
     */
    static TInt GetTypeIdCount( const CBase* aPtr );
    static const TAny* GetTypeIdPtr( const CBase* aPtr, TInt aIndex );



private: //helper class

    /**
     *  Simple container for presence attribute
     *  details. For CPEngAttributeTypeStack
     *  internal use only.
     *
     *  @since 3.0
     */
    class TPEngAttributeEntry
        {
        public:
            /**
             * C++ constructor.
             * Initializes the data members.
             */
            TPEngAttributeEntry();

        public:     //public data
            TUint32     iTypeId;
            TPtrC8      iName;
            TPtrC8      iNS;
        };



private:    // Data

    //OWN: Attribute stack
    RArray< TPEngAttributeEntry >*  iAttributes;

    //OWN: Unique name spaces declared by the holded attributes
    RArray< TPtrC8 >*  iUniqueNs;

    };

#endif      // CPENGATTRIBUTETYPESTACK_H

// End of File
