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
* Description:  See class description below.
*
*/


#ifndef __PENGSTORAGEGLOBALS_H__
#define __PENGSTORAGEGLOBALS_H__

//  INCLUDES
#include <e32base.h>


//  CONSTANTS
_LIT( KPEngStorageDrive, "C:" );
_LIT( KPEngStorageTempFolder, "temp\\" );
_LIT( KPEngStorageStateSuffix, ".st" );
const TInt KMinimalHashSize = 28; // Minimal size of the Hashed ID


// DATA TYPES
enum TPEngStorageType
    {
    //////////////////////////////////////////////////////////////////////////
    /**
     *	When CPEngStoreEntry::ExternalizeL() and CPEngStoreEntry::InternalizeL()
     *	and CPEngStoreEntry::GetEntrySize() is called for the mixed model
     *	types, type of the part of the model to be processed if passed.
     *	For single type Store models, this parameter should be
     *	ignored in the implementation of those functions in the
     *	derived classes.
     *
     *	When Mixed model type is used, for before each call of the
     *	CPEngStoreEntry::ExternalizeL() and CPEngStoreEntry::InternalizeL()
     *	CPEngStoreEntry::GetEntrySize() is called to get size of the current
     *	part of the model to be Externalizeddd / Internalized
     */
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /////////////	Basic Store Entry types //////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    /**
     * Signals First basic type, not to be used by clients as a Storage
     * type. How ever it can be used by the client as signalization
     * of the beginning of the Externalize/ Internalize operation
     */
    EPEngStorageBasicFirst =							0x00000001,

    /*
     * These models cannot be stored, StoreL and RetrieveL leaves with KErrNotSupported
     */
    EPEngStorageBasicTemporary =						0x00000002,

    /*
     * Permanent models, these remains there after boot
     * Storage Type in the ExternalizeL(), InternalizeL() GetEntrySize()
     * functions of the CPEngStoreEntry() can be ignored
     */
    EPEngStorageBasicPermanent =						0x00000004,

    /*
     * Cached models, these are deleted when cache is cleaned or when phone is booted
     * Storage Type in the ExternalizeL(), InternalizeL() GetEntrySize()
     * functions of the CPEngStoreEntry() can be ignored
      */
    EPEngStorageBasicCached =							0x00000008,

    /**
     * Signals last basic type, not to be used by clients!!
     */
    EPEngStorageBasicLast =								0x00000010,



    //////////////////////////////////////////////////////////////////////////
    ///////////// Additional Store entry types flag features /////////////////
    //////////////////////////////////////////////////////////////////////////
    /**
     * If this flag is switched one. Version of the stored Store entry model
     * is checked if it match the one which is stored in the store
     * If client modified different version, storing is denied
     */
    EPEngStorageFlagVersionChecked =			0x00000100,

    /**
     * This flag is used to notify if Lock of the store entry is obeyed or
     * not, while entry is stored in the Storage.
     * Setting up of this flag will disable check of the lock.
     */
    EPEngStorageLockIgnored =					0x00000200,

    //////////////////////////////////////////////////////////////////////////
    /**
     *	Mixed Store Entry types
     *	Those are predefined versions to be used as examples,
     *	it can be used by client
     */
    //////////////////////////////////////////////////////////////////////////

    /**
     *	Cached model with version control
     */
    EPEngMixedCachedVersionCon =
    EPEngStorageBasicCached | EPEngStorageFlagVersionChecked,

    /**
     *	Permanent with version control activated
     */
    EPEngMixedPermanentVersionCon =
    EPEngStorageBasicPermanent | EPEngStorageFlagVersionChecked,

    /*
     * Mixed model, this model has Permanent and Cached part.
     * CPEngStoreEntry::ExternalizeL() and CPEngStoreEntry::InternalizeL()
     * Is called twice. Current part to be processed is passed as parameter
     * to the called function.
     * When storing, either both or none of the parts is stored to the store.
     * Version check is not activated
     */
    EPEngMixedPermanentCached =
    EPEngStorageBasicPermanent | EPEngStorageBasicCached,

    /*
     * Mixed model, this model has Permanent and Cached part.
     * CPEngStoreEntry::ExternalizeL() and CPEngStoreEntry::InternalizeL()
     * Is called twice. Current part to be processed is passed as parameter
     * to the called function.
     * When storing, either both or none of the parts is stored to the store.
     * Version check is activated
     */
    // EPEngStoragePermanentNoVersionCheck | EPEngStorageCachedNoVersionCheck
    EPEngMixedPermanentCachedVersionCon =
    EPEngMixedPermanentCached | EPEngStorageFlagVersionChecked,


    //////////////////////////////////////////////////////////////////////////
    /////////////	Add new Store Entry types here ///////////////////////////
    //////////////////////////////////////////////////////////////////////////

    };

/**
 *	Levels of the storage Lock system. When requirement to the higher
 *	level of the lock is received by the storage, previous low level lock
 *	i released and replaced by the new lock. Owner of the previous lock
 *	is not allowed to manipulate with data anymore.
 */
enum TPengStorageLockPriority
    {
    EStorageLockLevelLow	= 0x0010,
    EStorageLockLevelBasic	= 0x0020,
    EStorageLockLevelHigh	= 0x0030
    };



//Panic definitions
_LIT( KPEngSMPanic, "PECSM" );
_LIT( KPEngSMEuserCBasePanic, "E32USER-CBase" );
const TInt KPEngSMEuserCBasePanicReason = 33 ;
enum TPEngStoragePanics
    {
    EPEngSM_StorageBaseNotSet = 1,
    EPEngSM_UnknownStorageType,
    EPEngSM_StoreManNotDefined,
    EPEngSM_SetNewEngagedSIdsHasBackup,
    EPEngSM_RemoveEngagedSIdsHasBackup,
    EPEngSM_SIdContainerHasStillObservers,
    EPEngSM_SIdNotifyUnderFlow,
    EPEngSM_SIdNotifyOverflow,
    EPEngSM_TypeOrderMismatch,
    EPEngSM_InternalizeLengthMismatch
    };

#endif      // __PENGSTORAGEGLOBALS_H__
