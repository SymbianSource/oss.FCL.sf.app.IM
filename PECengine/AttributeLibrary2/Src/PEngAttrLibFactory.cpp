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
* Description:  PEngAttrLibFactory - object factory
*
*/

// INCLUDE FILES
#include    "PEngAttrLibFactory.h"
#include    "CPEngAttrLibObjectHolder.h"
#include    "CPEngSessionSlotId.h"

#include    "MPEngPresenceAttrManager.h"
#include    "MPEngPresenceAttrTransactionEnv.h"
#include    "MPEngTransactionFactory.h"

#include    <E32Std.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngAttrLibFactory::AttributeManagerInstanceL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngPresenceAttrManager* PEngAttrLibFactory::AttributeManagerInstanceL(
    const CPEngSessionSlotId& aSlotId )
    {
    CPEngAttrLibObjectHolder* holder = CPEngAttrLibObjectHolder::InstanceLC();
    MPEngPresenceAttrManager* instance = holder->AttributeManagerInstanceL( aSlotId );
    CleanupStack::PopAndDestroy(); // holder

    return instance;
    }


// -----------------------------------------------------------------------------
// PEngAttrLibFactory::AttributeManagerInstanceLC()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngPresenceAttrManager* PEngAttrLibFactory::AttributeManagerInstanceLC(
    const CPEngSessionSlotId& aSlotId )
    {
    MPEngPresenceAttrManager* instance = AttributeManagerInstanceL( aSlotId );
    CleanupClosePushL( *instance );

    return instance;
    }


// -----------------------------------------------------------------------------
// PEngAttrLibFactory::AttributeTransEnvInstanceL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngPresenceAttrTransactionEnv* PEngAttrLibFactory::AttributeTransEnvInstanceL(
    const CPEngSessionSlotId& aSlotId )
    {
    CPEngAttrLibObjectHolder* holder = CPEngAttrLibObjectHolder::InstanceLC();
    MPEngPresenceAttrTransactionEnv* instance = holder->AttributeTransEnvInstanceL( aSlotId );
    CleanupStack::PopAndDestroy(); // holder

    return instance;
    }


// -----------------------------------------------------------------------------
// PEngAttrLibFactory::AttributeTransEnvInstanceLC()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngPresenceAttrTransactionEnv* PEngAttrLibFactory::AttributeTransEnvInstanceLC(
    const CPEngSessionSlotId& aSlotId )
    {
    MPEngPresenceAttrTransactionEnv* instance = AttributeTransEnvInstanceL( aSlotId );
    CleanupClosePushL( *instance );

    return instance;
    }


// -----------------------------------------------------------------------------
// PEngAttrLibFactory::AttributeTransFactoryInstanceL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngTransactionFactory* PEngAttrLibFactory::AttributeTransFactoryInstanceL(
    const CPEngSessionSlotId& aSlotId )

    {
    CPEngAttrLibObjectHolder* holder = CPEngAttrLibObjectHolder::InstanceLC();
    MPEngTransactionFactory* instance = holder->AttributeTransFactInstanceL( aSlotId );
    CleanupStack::PopAndDestroy(); // holder

    return instance;
    }


// -----------------------------------------------------------------------------
// PEngAttrLibFactory::AttributeTransFactoryInstanceLC()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngTransactionFactory* PEngAttrLibFactory::AttributeTransFactoryInstanceLC(
    const CPEngSessionSlotId& aSlotId )
    {
    MPEngTransactionFactory* instance = AttributeTransFactoryInstanceL( aSlotId );
    CleanupClosePushL( *instance );

    return instance;

    }


// -----------------------------------------------------------------------------
// PEngAttrLibFactory::__DbgObjectCounts()
// Debug helper
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngAttrLibFactory::__DbgObjectCounts( TInt& aHolder,
                                                     TInt& aAttrMan,
                                                     TInt& aTransEnv,
                                                     TInt& aTransFact )
    {
#ifdef _DEBUG

    aHolder = KErrNotFound;
    aAttrMan = KErrNotFound;
    aTransEnv = KErrNotFound;
    aTransFact = KErrNotFound;

    CPEngAttrLibObjectHolder* holder = CPEngAttrLibObjectHolder::__DbgInstanceOrNull();
    if ( holder )
        {
        holder->__DbgObjectCounts( aHolder, aAttrMan, aTransEnv, aTransFact );
        }

#else

    // suppress the unreferenced variable warnings
    ( void ) aHolder;
    ( void ) aAttrMan;
    ( void ) aTransEnv;
    ( void ) aTransFact;

#endif //_DEBUG
    }


// -----------------------------------------------------------------------------
// PEngAttrLibFactory::__DbgAttrManRefCount()
// Debug helper
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngAttrLibFactory::__DbgAttrManRefCount( const CPEngSessionSlotId& aSlotId,
                                                        TInt& aRefCount )
    {
#ifdef _DEBUG

    aRefCount = KErrNotFound;
    CPEngAttrLibObjectHolder* holder = CPEngAttrLibObjectHolder::__DbgInstanceOrNull();
    if ( holder )
        {
        holder->__DbgAttrManRefCount( aSlotId, aRefCount );
        }

#else

    // suppress the unreferenced variable warnings
    ( void ) aSlotId;
    ( void ) aRefCount;

#endif //_DEBUG
    }



// -----------------------------------------------------------------------------
// PEngAttrLibFactory::__DbgTransEnvRefCount()
// Debug helper
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngAttrLibFactory::__DbgTransEnvRefCount( const CPEngSessionSlotId& aSlotId,
                                                         TInt& aRefCount )
    {
#ifdef _DEBUG

    aRefCount = KErrNotFound;

    CPEngAttrLibObjectHolder* holder = CPEngAttrLibObjectHolder::__DbgInstanceOrNull();
    if ( holder )
        {
        holder->__DbgTransEnvRefCount( aSlotId, aRefCount );
        }

#else

    // suppress the unreferenced variable warnings
    ( void ) aSlotId;
    ( void ) aRefCount;

#endif //_DEBUG
    }

// -----------------------------------------------------------------------------
// PEngAttrLibFactory::__DbgTransFactRefCount()
// Debug helper
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngAttrLibFactory::__DbgTransFactRefCount( const CPEngSessionSlotId& aSlotId,
                                                          TInt& aRefCount )
    {
#ifdef _DEBUG
    aRefCount = KErrNotFound;

    CPEngAttrLibObjectHolder* holder = CPEngAttrLibObjectHolder::__DbgInstanceOrNull();
    if ( holder )
        {
        holder->__DbgTransFactRefCount( aSlotId, aRefCount );
        }

#else

    // suppress the unreferenced variable warnings
    ( void ) aSlotId;
    ( void ) aRefCount;

#endif //_DEBUG
    }



//  End of File

