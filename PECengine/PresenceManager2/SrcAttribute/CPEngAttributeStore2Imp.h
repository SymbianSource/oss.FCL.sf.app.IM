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
* Description:  Store API to retrieve presence attribute models.
*
*/

#ifndef CPENGATTRIBUTESTORE2IMP_H
#define CPENGATTRIBUTESTORE2IMP_H

//  INCLUDES
#include <E32Base.h>



//FORWARD DECLARATIONS
class MPEngPresenceAttrModel2;
class MPEngPresenceAttrManager;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotStorageProxy;


/**
 *
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeStore2Imp ): public CBase

    {
public: /* Construction */


    /**
     * Instantiates CPEngAttributeStore2Imp object.
     *
     * @return New CPEngAttributeStore2Imp instance.
     */
    static CPEngAttributeStore2Imp* NewL(
        const CPEngNWSessionSlotID2& aNWSessionSlotID,
        TInt aPriority );


    /**
     * Destructor.
     */
    ~CPEngAttributeStore2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngAttributeStore2Imp( TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Attribute store implementation */

    /**
     * Implementation.
     * @since 3.0
     */
    TInt GetAttribute( TUint32 aType,
                       const TDesC& aPresenceID,
                       MPEngPresenceAttrModel2*& aModel ) const;


    TInt GetAndLockAttribute( TUint32 aType,
                              const TDesC& aPresenceID,
                              MPEngPresenceAttrModel2*& aModel );

    TInt StoreAttribute( MPEngPresenceAttrModel2& aModel,
                         TBool aReleaseEditLock );

    TInt UnLockAttribute( MPEngPresenceAttrModel2& aModel );

    TInt AttributeAvailable( TUint32 aType, const TDesC& aPresenceID ) const;

    TInt AttributeTypeSupported( TUint32 aType ) const;


private: //Data

    //OWN: The priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy* iUsedSlot;

    //OWN: Attribute manager
    MPEngPresenceAttrManager*   iAttrManager;


    };

#endif  // CPENGATTRIBUTESTORE2IMP_H

// End of File
