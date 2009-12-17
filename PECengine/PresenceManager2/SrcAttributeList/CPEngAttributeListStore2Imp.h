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
* Description:  Store API to retrieve attribute lists.
*
*/

#ifndef CPENGATTRIBUTELISTSTORE2IMP_H
#define CPENGATTRIBUTELISTSTORE2IMP_H

//  INCLUDES
#include <E32Base.h>
#include <MPEngAttributeList2.h>



//FORWARD DECLARATIONS
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotStorageProxy;
class MPEngAttributeListManager;


/**
 *
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeListStore2Imp ): public CBase

    {
public: /* Construction */


    /**
     * Instantiates CPEngAttributeListStore2Imp object.
     *
     * @return New CPEngAttributeListStore2Imp instance.
     */
    static CPEngAttributeListStore2Imp* NewL(
        const CPEngNWSessionSlotID2& aNWSessionSlotID,
        TInt aPriority );


    /**
     * Destructor.
     */
    ~CPEngAttributeListStore2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngAttributeListStore2Imp( TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Attribute list store implementation */

    /**
     * Implementation.
     * @since 3.0
     */
    MPEngAttributeList2* CreateEmptyAttributeListL();

    MPEngAttributeList2* GetDefaultAttributeListL(
        TPEngAttributeListsSet2 aAttrListsSet );

    void SetAsDefaultAttributeListL(
        MPEngAttributeList2& aAttributeList );

    void DeleteDefaultAttributeListL();

    MPEngAttributeList2* GetAttributeListForContactListL(
        const TDesC& aContactListName,
        TPEngAttributeListsSet2 aAttrListsSet );

    void AttachAttributeListToContactListL(
        const TDesC& aContactListName,
        MPEngAttributeList2& aAttributeList );

    void DeleteAttributeListFromContactListL(
        const TDesC& aContactListName );

    MPEngAttributeList2* GetAttributeListForUserL(
        const TDesC& aPresenceID,
        TPEngAttributeListsSet2 aAttrListsSet );

    void AttachAttributeListToUserL( const TDesC& aPresenceID,
                                     MPEngAttributeList2& aList );

    void DeleteAttributeListFromUserL( const TDesC& aPresenceID );



private: //Data

    //OWN: The priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy*     iUsedSlot;


    //OWN:
    MPEngAttributeListManager*  iAttrListManager;


    };

#endif  // CPENGATTRIBUTELISTSTORE2IMP_H

// End of File
