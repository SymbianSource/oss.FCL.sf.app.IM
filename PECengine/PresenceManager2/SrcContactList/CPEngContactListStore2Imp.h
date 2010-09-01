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
* Description:  Store API to retrieve contact list models.
*
*/

#ifndef CPENGCONTACTLISTSTORE2IMP_H
#define CPENGCONTACTLISTSTORE2IMP_H

//  INCLUDES
#include <E32Base.h>
#include "TPEngMDesCArrayAdapter.h"
#include "TPEngContactListBaseSettings.h"


//FORWARD DECLARATIONS
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotStorageProxy;
class MPEngContactListManager;
class MPEngContactList2;
class MPEngContactListProperties;
class TPEngContactListBaseSettings;



/**
 *
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListStore2Imp ): public CBase

    {
public: /* Construction */


    /**
     * Instantiates CPEngContactListStore2Imp object.
     *
     * @return New CPEngContactListStore2Imp instance.
     */
    static CPEngContactListStore2Imp* NewL(
        const CPEngNWSessionSlotID2& aNWSessionSlotID,
        TInt aPriority );


    /**
     * Destructor.
     */
    ~CPEngContactListStore2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngContactListStore2Imp( TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Contact list store implementation */

    /**
     * Implementation.
     * @since 3.0
     */
    const MDesCArray& AllContactListsL() const;

    const MDesCArray& SynchronizedContactListsL() const;

    MPEngContactList2& ContactListL( const TDesC& aContactList );

    MPEngContactListProperties& CreateContactListL(
        const TDesC& aContactList,
        TPEngContactListBaseSettings& aBaseSettings );

    void DeleteContactListL( const TDesC& aContactList );

    void ChangeContactListTypeL( const TDesC& aContactList,
                                 TPEngContactListType aContactListType );

    MPEngContactListProperties& ContactListPropertiesL(
        const TDesC& aContactList );

    void SetAsDefaultContactListL( const TDesC& aContactList );

    TInt ReleaseContactList( const TDesC& aContactList );


    //Helper for handling deprecated functions
    const MDesCArray& DummyList() const;




private: //Data

    //OWN: The priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy*     iUsedSlot;

    //OWN:
    MPEngContactListManager*    iCntLstManager;

    //OWN:
    TPEngMDesCArrayAdapter  iDummy;



    };

#endif  //CPENGCONTACTLISTSTORE2IMP_H

// End of File
