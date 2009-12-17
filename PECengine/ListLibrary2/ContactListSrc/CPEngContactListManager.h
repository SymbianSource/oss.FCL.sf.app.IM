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
* Description:  Contact lists manager
*
*/

#ifndef __CPENGCONTACTLISTMANAGER_H__
#define __CPENGCONTACTLISTMANAGER_H__


//  INCLUDES
#include "CPEngContactListManagerBase.h"
#include "MPEngContactListManager.h"
#include <e32base.h>


// FORWARD DECLARATIONS
class MPEngContactList;
class CPEngContactListModBase;


// CLASS DECLARATION

/**
 * Contact List manager
 * Used to maintain contact lists.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListManager ) :
        public CPEngContactListManagerBase,
        public MPEngContactListManager
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngContactListManager* NewL(
        MPEngListLibFactory& aFactory,
        const CPEngSessionSlotId& aSessionSlot );

    /**
     * Two-phased constructor.
     */
    static CPEngContactListManager* NewLC(
        MPEngListLibFactory& aFactory,
        const CPEngSessionSlotId& aSessionSlot );

    /**
     * Destructor
     */
    virtual ~CPEngContactListManager();


public:  // Functions from MPEngContactListManager

    /**
     * Decrease reference count
     * @see <MPEngContactListManager.h>
     */
    void Close();


    /**
     * Retrieves contact list model
     * @see <MPEngContactListManager.h>
     */
    MPEngContactListAdvance& ContactListL( const TDesC& aContactList );


    /**
     * List of all contact lists
     * @see <MPEngContactListManager.h>
     */
    const MDesCArray& AllContactListsL() const;


    /**
     * Active contact list.
     * @see <MPEngContactListManager.h>
     */
    const MDesCArray& SynchronizedContactLists( ) const;


    /**
     * Contact list settings model
     * @see <MPEngContactListManager.h>
     */
    MPEngContactListProperties& ContactListSettingsL( const TDesC& aContacList );


    /**
     * Set Default contact list
     * @see <MPEngContactListManager.h>
     */
    void SetAsDefaultCntListL( const TDesC& aContactList );


    /**
     * Create new contact list.
     * @see <MPEngContactListManager.h>
     */
    MPEngContactListProperties& CreateContactListL(
        const TDesC& aContactList,
        TPEngContactListBaseSettings& aBaseSettings );

    /**
     * Delete contact list
     * @see <MPEngContactListManager.h>
     */
    void DeleteContactListL( const TDesC& aContactList );


    /**
     * Change contact list type
     * @see <MPEngContactListManager.h>
     */
    void ChangeContactListTypeL( const TDesC& aContactList,
                                 TPEngContactListType aContactListType );


    /**
     * Release contact list model from the memory
     * @see <MPEngContactListManager.h>
     */
    void ReleaseCntListModel( const TDesC& aContactList );



private: // Overwrittent form CPEngContactListManagerBase

    /**
     * Handle settings update
     */
    void HandleSettingsUpdateL();



private: // constructors

    /**
     * C++ constructor.
     */
    CPEngContactListManager( MPEngListLibFactory& aFactory );


    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const CPEngSessionSlotId& aSessionSlot );


private:    // Data

    /// OWN: All contact lists
    CPtrC16Array                            iAllLists;

    /// OWN: Contact lists which synhronized
    CPtrC16Array                            iSynchronizedLists;
    };

#endif      //  __CPENGCONTACTLISTMANAGER_H__

//  End of File




