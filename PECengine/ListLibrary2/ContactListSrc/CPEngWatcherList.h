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
* Description:  Watcher list
*
*/

#ifndef __CPENGWATCHERLIST_H__
#define __CPENGWATCHERLIST_H__

//  INCLUDES
#include   "CPEngContactListModBase.h"
#include <e32base.h>


/**
 * Watcher list,
 * Implements watcher list specific function of the const lists model
 * All editing function from the Contact list model are
 * implemented again and leaves with KErrNotSupported
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngWatcherList ) : public CPEngContactListModBase
    {

public: // Constructors, destructor

    /**
     * Two-phased constructor.
     */
    static CPEngWatcherList* NewLC(
        CPEngContactListSettings& aListSetting,
        MPEngStorageManager& aStorageManager,
        MPEngContactListSettingsManager& aCntLstSettingsManager );


    /**
     * Destructor
     */
    ~CPEngWatcherList();

public: // Functions from MPEngContactList2

    /**
     * Inserts a Contact into the Contact list.
     * @see <MPEngContactList2>
     */
    TInt AddContactL( const TDesC& aContact,
                      const TDesC& aNickName = KNullDesC );


    /**
     * Removes Contact from the Contact list
     * @see <MPEngContactList2>
     */
    void RemoveContactL( const TDesC& aContact );


    /**
     * Remove all contacts from the contact list
     * @see <MPEngContactList2>
     */
    void RemoveAllContactsL( );


    /**
     * Roll back all changes
     * @see <MPEngContactList2>
     */
    TInt RollBackAllChangesL();


public: // Functions from MPresenceStoreEntry

    /**
     * Storage Id
     * @see <MPEngStoreEntry.h>
     */
    virtual const TDesC& StorageId( ) const;


private: // Constructors

    /**
     * C++ constructor.
     * @param aStorageManager A reference to StorageManager instance to use.
     */
    CPEngWatcherList( CPEngContactListSettings& aListSetting,
                      MPEngContactListSettingsManager& aCntLstSettingsManager );

    /**
     * By default Symbian constructor is private.
     */
    void ConstructL( MPEngStorageManager& aStorageManager );
    };

#endif      //  __CPENGWATCHERLIST_H__


//  End of File






















