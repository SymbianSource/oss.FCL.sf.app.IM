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

// INCLUDE FILES
#include "CPEngWatcherList.h"
#include "PresenceDebugPrint.h"
#include <e32std.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngWatcherList::CPEngWatcherList()
// -----------------------------------------------------------------------------
//
CPEngWatcherList::CPEngWatcherList(
    CPEngContactListSettings& aListSetting,
    MPEngContactListSettingsManager& aCntLstSettingsManager )
        : CPEngContactListModBase( aListSetting,
                                   aCntLstSettingsManager )
    {
    PENG_DP( D_PENG_LIT( "CPEngWatcherList::CPEngWatcherList" ) );
    }


// -----------------------------------------------------------------------------
// CPEngWatcherList::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngWatcherList::ConstructL( MPEngStorageManager& aStorageManager )
    {
    CPEngContactListModBase::ConstructL( aStorageManager );
    }


// -----------------------------------------------------------------------------
// CPEngWatcherList::NewLC()
// -----------------------------------------------------------------------------
//
CPEngWatcherList* CPEngWatcherList::NewLC(
    CPEngContactListSettings& aListSetting,
    MPEngStorageManager& aStorageManager,
    MPEngContactListSettingsManager& aCntLstSettingsManager )
    {
    CPEngWatcherList* self = new ( ELeave ) CPEngWatcherList(
        aListSetting,
        aCntLstSettingsManager );

    CleanupClosePushL( *self );
    self->ConstructL( aStorageManager );

    return self;
    }

// Destructor
CPEngWatcherList::~CPEngWatcherList()
    {
    PENG_DP( D_PENG_LIT( "CPEngWatcherList::~CPEngWatcherList" ) );
    }


// -----------------------------------------------------------------------------
// CPEngWatcherList::AddContactL()
// -----------------------------------------------------------------------------
//
TInt CPEngWatcherList::AddContactL( const TDesC& /* aContact */,
                                    const TDesC& /* aNickName = KNullDesC */ )
    {
    PENG_DP( D_PENG_LIT( "CPEngWatcherList::AddIdL()" ) );
    User::Leave( KErrNotSupported );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngWatcherList::RemoveContactL()
// -----------------------------------------------------------------------------
//
void CPEngWatcherList::RemoveContactL( const TDesC& /* aContact */ )
    {
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CPEngWatcherList::RemoveAllContactsL()
// -----------------------------------------------------------------------------
//
void CPEngWatcherList::RemoveAllContactsL()
    {
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CPEngWatcherList::RollBackAllChangesL()
// -----------------------------------------------------------------------------
//
TInt CPEngWatcherList::RollBackAllChangesL( void )
    {
    User::Leave( KErrNotSupported );
    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// CPEngWatcherList::StorageId()
// -----------------------------------------------------------------------------
//
const TDesC& CPEngWatcherList::StorageId() const
    {
    return KPEngWatcherList;
    }

//  End of File

