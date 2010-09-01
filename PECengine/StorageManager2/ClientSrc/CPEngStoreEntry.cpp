/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Store entry abstract class
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngStoreEntry.h"
#include	"MPEngStorageManagerWatcher.h"
#include	"MPEngStorageManager.h"
#include	"PEngStorageGlobals.h"

//	Debug prints
#include	"PresenceDebugPrint.h"


// ============================= LOCAL CLASS DEFINITION ======================

// CLASS DEFINITION
NONSHARABLE_CLASS( TPEngSIdsDesWraper ):
        public MDesCArray
    {
public:
    inline TPEngSIdsDesWraper( const TDesC& aSid )
            : iSid( aSid )
        {
        }
public: // from MDesC16Array
    inline TInt MdcaCount() const
        {
        // we have only one Sid
        return 1;
        }

    inline TPtrC16 MdcaPoint( TInt aIndex ) const
        {
        // panic is index is out of 0
        __ASSERT_DEBUG( aIndex == 0, User::Panic(
                            KPEngSMPanic,
                            EPEngSM_StoreManNotDefined ) );
        ( void ) aIndex; // Prevent compilation warning about unused variable
        return iSid;
        }

private: // data
    const TDesC& 			iSid;
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngStoreEntry::CPEngStoreEntry
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngStoreEntry::CPEngStoreEntry(
    TPEngStorageType aStorageType )
        : iStorageType( aStorageType )
    {
    }

// -----------------------------------------------------------------------------
// CPEngStoreEntry::BaseConstructL
// Symbian 2nd phase Base constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngStoreEntry::BaseConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CPEngStoreEntry::BaseConstructL
// Symbian 2nd phase Base constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngStoreEntry::BaseConstructL(
    MPEngStorageManager& aStorageManager,
    TBool aRegister /* = ETrue */ )
    {
    iStorageManager = &aStorageManager;
    iStorageManager->Open();                        // CSI: 65 #

    if ( aRegister )
        {
        TPEngSIdsDesWraper sidWrap( this->StorageId() );
        iStorageManager->StorageManagerWatcher().RegisterListenSIDsObserverL(
            sidWrap,
            *this,
            MPEngStorageManagerWatcher::EPEngObserverInternal );
        }
    }


// Destructor
EXPORT_C CPEngStoreEntry::~CPEngStoreEntry()
    {
    if ( iStorageManager )
        {
        iStorageManager->StorageManagerWatcher(
        ).UnregisterListenSIDsObserver( *this );
        iStorageManager->Close();
        }
    }

// -----------------------------------------------------------------------------
// ==== Default implementation of functions from main class ====================
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngStoreEntry::Release
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngStoreEntry::Release()
    {
    __ASSERT_DEBUG( iStorageManager, User::Panic(
                        KPEngSMPanic,
                        EPEngSM_StoreManNotDefined ) );
    // do not check, let client crash,
    // since this should be always valid pointer
    iStorageManager->Unlock( *this );
    }

// -----------------------------------------------------------------------------
// CPEngStoreEntry::LockLC()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngStoreEntry::LockLC()
    {
    __ASSERT_DEBUG( iStorageManager, User::Panic(
                        KPEngSMPanic,
                        EPEngSM_StoreManNotDefined ) );
    TInt err( Lock() );
    CleanupReleasePushL( *this );
    return err;
    }

// -----------------------------------------------------------------------------
// CPEngStoreEntry::Lock
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngStoreEntry::Lock(
    TPengStorageLockPriority aPriority
    /* EStorageLockLevelBasic */,
    TBool aCreateEntry /* = ETrue */ ) const
    {
    __ASSERT_DEBUG( iStorageManager, User::Panic(
                        KPEngSMPanic,
                        EPEngSM_StoreManNotDefined ) );

    // do not check, let client crash,
    // since this should be always valid pointer
    return iStorageManager->Lock( *this, aPriority, aCreateEntry );
    }

// -----------------------------------------------------------------------------
// CPEngStoreEntry::Unlock
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngStoreEntry::Unlock() const
    {
    __ASSERT_DEBUG( iStorageManager, User::Panic(
                        KPEngSMPanic,
                        EPEngSM_StoreManNotDefined ) );
    // do not check, let client crash, since this should be always valid pointer
    return iStorageManager->Unlock( *this );
    }

// -----------------------------------------------------------------------------
// CPEngStoreEntry::BufferServerSideNotifications
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngStoreEntry::BufferServerSideNotifications()
    {
    __ASSERT_DEBUG( iStorageManager, User::Panic(
                        KPEngSMPanic,
                        EPEngSM_StoreManNotDefined ) );
    return iStorageManager->BufferServerSideNotifications();
    }

// -----------------------------------------------------------------------------
// CPEngStoreEntry::ReleaseServerSideBuffering
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngStoreEntry::ReleaseServerSideBuffering()
    {
    __ASSERT_DEBUG( iStorageManager, User::Panic(
                        KPEngSMPanic,
                        EPEngSM_StoreManNotDefined ) );

    return iStorageManager->ReleaseServerSideBuffering();
    }

// -----------------------------------------------------------------------------
// CPEngStoreEntry::StoreL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngStoreEntry::StoreL()
    {
    __ASSERT_DEBUG( iStorageManager, User::Panic(
                        KPEngSMPanic,
                        EPEngSM_StoreManNotDefined ) );
    iStorageManager->StoreL( *this );
    }

// -----------------------------------------------------------------------------
// ===================== Storage type support ==================================
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngStoreEntry::StorageType
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngStorageType CPEngStoreEntry::StorageType() const
    {
    return iStorageType;
    }

// -----------------------------------------------------------------------------
// CPEngStoreEntry::ChangeStorageTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngStoreEntry::ChangeStorageTypeL( TPEngStorageType aNewType )
    {
    __ASSERT_DEBUG( iStorageManager, User::Panic(
                        KPEngSMPanic,
                        EPEngSM_StoreManNotDefined ) );
    if ( iStorageType == aNewType )
        {
        return;
        }
    TInt err( iStorageManager->Delete( *this ) );
    err = err == KErrNotFound ? KErrNone : err;
    User::LeaveIfError( err );
    iStorageType = aNewType;
    // reset version counter to zero
    iEntryVersion = 0;
    iStorageManager->StoreL( *this );
    }

// -----------------------------------------------------------------------------
// ====  New functions of MPEngSIDChangeObserver with default implementation ===
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CPEngStoreEntry::HandleSIDsChangeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngStoreEntry::HandleSIDsChangeL(
    CPtrCArray& /* aChangedSIDs */ )
    {
    // does nothing
    }

// -----------------------------------------------------------------------------
// CPEngStoreEntry::HandleSIDNotifyError
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngStoreEntry::HandleSIDNotifyError(
    TInt /* aError */ )
    {
    // does nothing
    }

//  End of File




