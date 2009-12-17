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
* Description:  Tracked Presence ID entry.
*
*/

#ifndef CPENGTRACKEDPRESENCEIDENTRY_H
#define CPENGTRACKEDPRESENCEIDENTRY_H

//  INCLUDES
#include <E32Base.h>


//FORWARD DECLARATIONS
class CPEngTrackedPresenceID2;


/**
 * Tracked Presence ID entry.
 *
 * This class can be derived to provide additional
 * functionality for Presence ID handling.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngTrackedPresenceIDEntry ): public CBase
    {
    //-----------------------------------------------------------------------
public: /* Construction */

    /**
     * Instantiates CPEngTrackedPresenceIDEntry object.
     *
     * @param aPresenceID Tracked Presence ID.
     * @param aTypes Initial tracked attributes.
     * @param aFilterFlags Initial filter flags.
     * @return New CPEngTrackedPresenceIDEntry instance.
     */
    static CPEngTrackedPresenceIDEntry* NewLC(
        const TDesC& aPresenceID,
        const TArray<TUint32>& aTypes,
        TInt aFilterFlag );


    /**
     * Destructor.
     * Virtual by CBase.
     */
    ~CPEngTrackedPresenceIDEntry();



protected:  // Constructor

    /**
     * C++ default constructor.
     */
    CPEngTrackedPresenceIDEntry();


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const TDesC& aPresenceID,
                     const TArray<TUint32>& aTypes,
                     TInt aFilterFlag );


    //-----------------------------------------------------------------------
public: /* Interface */


    /**
     * Interface getter.
     *
     * @since 3.0
     * @return Reference to interface.
     */
    CPEngTrackedPresenceID2& Interface();



    //-----------------------------------------------------------------------
public: /* Tracked presence ID */

    /**
     * Gets the presence id.
     *
     * Virtual so derived class can override
     * behaviour.
     *
     * @since 3.0
     * @return The Presence ID.
     */
    virtual TPtrC PresenceID() const;


    /**
     * Compares two tracked PresenceID entry
     * according to Presence IDs.
     *
     * @since 3.0
     * @return Return values according to common
     * compare functionality.
     */
    static TInt Compare( const CPEngTrackedPresenceIDEntry& aLhs,
                         const CPEngTrackedPresenceIDEntry& aRhs );




    //-----------------------------------------------------------------------
public: /* Tracked attributes */

    /**
     * Checks is given attribute in the list of
     * tracked attributes.
     *
     * @since 3.0
     * @param Attribute to check.
     * @return ETrue if given attribute is in the
     * list of tracked attributes. Else EFalse.
     */
    TBool IsOnlyTrackedAttribute( TUint32 aType ) const;


    /**
     * Checks is there any attributes in the list of
     * tracked attributes.
     *
     * @since 3.0
     * @return ETrue if there is one or more tracked attributes.
     * Else EFalse.
     */
    TBool AttributesTracked() const;


    /**
     * Adds new attributes to list of tracked attributes.
     *
     * "Already exist" errors are ignored.
     *
     * @since 3.0
     * @param Tracked presence attributes.
     */
    void AddTrackedAttributesL( const TArray<TUint32>& aTypes );


    /**
     * Removes the given attributes from the list of
     * tracked attributes.
     *
     * "Not found" errors are ignored.
     *
     * @since 3.0
     * @param aTypes Attributes to remove.
     */
    void RemoveTrackedAttributes( const TArray<TUint32>& aTypes );


    /**
     * Generic array presenting tracked attributes.
     *
     * @since 3.0
     * @return Generic array presenting tracked attributes.
     */
    TArray<TUint32> TrackedAttributes() const;


    /**
     * Iterates to next tracked attribute and returns
     * its value.
     *
     * If no more tracked attributes, returns
     * KPEngNullAttributeType.
     *
     * @since 3.0
     * @Param Iteration start index.
     * Updates the index during iteration.
     * @return Iterated attribute value.
     */
    TUint32 NextTrackedAttribute( TInt& aIterIx ) const;



    //-----------------------------------------------------------------------
public: /* Filter flags */

    /**
     * Sets filter flag.
     *
     * @since 3.0
     * @param aFlag New flag value.
     */
    void SetFilterFlag( TInt aFlag );


    /**
     * Gets filter flag.
     *
     * @since 3.0
     * @return Filter flag
     */
    TInt FilterFlag() const;





    //-----------------------------------------------------------------------
public: /* Attribute change marks */


    /**
     * Marks all attributes as changed.
     *
     *
     * @since 3.0
     */
    void MarkAllAttributesChanged();

    /**
     * Resets change mark from all tracked attributes.
     *
     * @since 3.0
     */
    void ResetChangeMarks();


    /**
     * Gets change status.
     *
     * @since 3.0
     * @return ETrue if one or more attributes is marked as changed.
     * Else EFalse.
     */
    TBool AttributesChanged() const;


    /**
     * Iterates to next changed attribute and
     * returns its value.
     *
     * If no more changed attributes, returns
     * KPEngNullAttributeType.
     *
     * @since 3.0
     * @Param Iteration start index.
     * Updates the index during iteration.
     * @return Iterated attribute value.
     */
    TUint32 NextChangedAttribute( TInt& aIterIx ) const;



private: //Helpers

    /**
     * Generic array accessors.
     * @since 3.0
     */
    static TInt TrackedAttributesCount( const CBase* aThis );
    static const TAny* TrackedAttributeAt( const CBase* aThis, TInt aIndex );



private: //Data

    //OWN: Exported class interface
    CPEngTrackedPresenceID2*    iInterface;

    //OWN: Tracked Presence ID
    HBufC*      iPresenceID;

    //OWN: Attributes tracked for Presence ID
    RArray< TUint32 >     iTrackedAttributes;

    //OWN: Is any of the tracked attributes changed?
    TBool iAttributesChanged;

    //OWN: Filter flags
    TInt    iFilterFlag;


    };

#endif      //CPENGTRACKEDPRESENCEIDENTRY_H



