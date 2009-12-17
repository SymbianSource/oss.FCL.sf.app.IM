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
* Description:  Attribute list model implementation
*
*/

#ifndef __CPENGATTRIBUTELISTMODEL_H__
#define __CPENGATTRIBUTELISTMODEL_H__

//  INCLUDES
#include <e32base.h>
#include "MPEngAttributeList2.h"

// FORWARD DECLARATIONS
class   MPEngPresenceAttrManager;


// CLASS DECLARATION

/**
 * Attribute list model
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeListModel ): public CBase,
        public MPEngAttributeList2
    {
public:
    /**
     * Two-phased constructor.
     */
    static CPEngAttributeListModel* NewL(
        MPEngPresenceAttrManager*   aAttributeManager );

    static CPEngAttributeListModel* NewLC(
        MPEngPresenceAttrManager* aAttributeManager );

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeListModel* NewL(
        MPEngPresenceAttrManager*   aAttributeManager,
        const RArray<TUint32>& aPresenceAttributes );

    static CPEngAttributeListModel* NewLC(
        MPEngPresenceAttrManager* aAttributeManager,
        const RArray<TUint32>& aPresenceAttributes );

    /**
     * Destructor.
     */
    ~CPEngAttributeListModel();


public: // From MPEngAttributeList2

    /**
     * Adds a presence attribute into the attribute list.
     * @see <MPEngAttributeList2.h>
     */
    void AddPresenceAttributeL( TUint32 aPresenceAttribute );


    /**
     * Removes the presence attribute from the attribute list.
     * @see <MPEngAttributeList2.h>
     */
    TInt RemovePresenceAttribute( TUint32 aPresenceAttribute );


    /**
     * Removes all presence attributes from the attribute list
     * @see <MPEngAttributeList2.h>
     */
    void RemoveAllAttributes();


    /**
     * Checks is the presence attribute valid and can it be added
     * @see <MPEngAttributeList2.h>
     */
    TInt ConfirmPresenceAttribute( TUint32 aPresenceAttribute ) const;


    /**
     * Current set of presence attributes in the attribute list.
     * @see <MPEngAttributeList2.h>
     */
    TArray<TUint32> ListOfAttributes() const;



    /**
     *  Generic attribute list extension interface.
     *  @see <MPEngAttributeList2.h>
     *  @since 3.0
     */
    MPEngAttributeListExt2* Extension()
        {
        return NULL;
        }

    /**
     * Presence Engine internal attribute list interface.
     * @see <MPEngAttributeList2.h>
     * @since 3.0
     */
    const MPEngAdvancedAttributeList2* Advanced() const
        {
        return NULL;
        }

    MPEngAdvancedAttributeList2* Advanced()
        {
        return NULL;
        }


    /**
     * Close reference to the object
     *
     * @since 3.0
     */
    void Close();


public: // New functions

    /**
     * Reference to the array of the attributes
     *
     * @since 3.0
     * @return array of presence attributes
     */
    const RArray<TUint32>& PresenceAttributes() const;


private:    // constructors

    /**
     * Symbian constructor.
     */
    void ConstructL( const RArray<TUint32>& aPresenceAttributes );

    /**
     * C++ constructor.
     */
    CPEngAttributeListModel( MPEngPresenceAttrManager* aAttributeManager );


private:    // Data

    /// OWN: List of presence attributes
    RArray<TUint32>             iListOfAttributes;

    /// REF: Attribute manager
    MPEngPresenceAttrManager*   iAttributeManager;

    };

#endif //   __CPENGATTRIBUTELISTMODEL_H__

// End of File

