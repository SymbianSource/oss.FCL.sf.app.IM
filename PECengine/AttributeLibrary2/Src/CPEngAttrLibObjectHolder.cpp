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
* Description:  Attribute library object holder.
*
*/

// INCLUDE FILES
#include    "CPEngAttrLibObjectHolder.h"
#include    "CPEngSessionSlotObject.h"
#include    "CPEngSessionSlotObjectCon.h"

#include    "CPEngPresenceAttrManager.h"
#include    "CPEngPresenceAttrTransactionEnv.h"
#include    "CPEngAttributeTransactionFactory.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::InstanceLC()
// Singleton retrieve method.
// -----------------------------------------------------------------------------
//
CPEngAttrLibObjectHolder* CPEngAttrLibObjectHolder::InstanceLC()
    {
    CPEngAttrLibObjectHolder* holder = NULL;

    holder = ( CPEngAttrLibObjectHolder* ) Dll::Tls();
    if ( !holder )
        {
        holder = CPEngAttrLibObjectHolder::NewLC();
        User::LeaveIfError( Dll::SetTls( holder ) );
        }
    else
        {
        TInt val = holder->Open();                 // CSI: 65 #
        CleanupClosePushL( *holder );
        }

    return holder;
    }


// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::HandleThreadDetach()
// -----------------------------------------------------------------------------
//
void CPEngAttrLibObjectHolder::HandleThreadDetach()
    {
    CPEngAttrLibObjectHolder* holder;
    holder = ( CPEngAttrLibObjectHolder* ) Dll::Tls();
    delete holder;
    }



// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttrLibObjectHolder* CPEngAttrLibObjectHolder::NewLC()
    {
    CPEngAttrLibObjectHolder* self = new( ELeave ) CPEngAttrLibObjectHolder;
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::CPEngAttrLibObjectHolder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttrLibObjectHolder::CPEngAttrLibObjectHolder()
    {
    }


// Destructor
CPEngAttrLibObjectHolder::~CPEngAttrLibObjectHolder()
    {
    delete iAttributeManagers;
    delete iAttributeTransEnvs;
    delete iAttributeTransFacts;
    Dll::SetTls( NULL );

#if _BullseyeCoverage
    cov_write();
#endif
    }



// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttrLibObjectHolder::ConstructL()
    {
    iAttributeManagers = CPEngSessionSlotObjectCon::NewL();
    iAttributeTransEnvs = CPEngSessionSlotObjectCon::NewL();
    iAttributeTransFacts = CPEngSessionSlotObjectCon::NewL();
    }



// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::AttributeManagerInstanceL()
//
// NOTE!! Method increases returned object reference count by one before
// returning it.
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrManager* CPEngAttrLibObjectHolder::AttributeManagerInstanceL(
    const CPEngSessionSlotId& aSlotId )
    {
    CPEngPresenceAttrManager* attrManager =
        ( CPEngPresenceAttrManager* ) iAttributeManagers->FindById( aSlotId );
    if ( !attrManager )
        {
        attrManager = CPEngPresenceAttrManager::NewLC( aSlotId );
        attrManager->SetOwner( *this );
        iAttributeManagers->AddL( attrManager );
        CleanupStack::Pop(); //Leave reference open to MPEngPresenceAttrManager
        }
    else {
        attrManager->Open();    // CSI: 65 #
        }

    return attrManager;
    }


// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::AttributeTransEnvInstanceL()
//
// NOTE!! Method increases returned object reference count by one before
// returning it.
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrTransactionEnv* CPEngAttrLibObjectHolder::AttributeTransEnvInstanceL(
    const CPEngSessionSlotId& aSlotId )
    {
    CPEngPresenceAttrTransactionEnv* transEnv =
        ( CPEngPresenceAttrTransactionEnv* ) iAttributeTransEnvs->FindById( aSlotId );
    if ( !transEnv )
        {
        MPEngPresenceAttrManager* attrMan = AttributeManagerInstanceL( aSlotId );
        CleanupClosePushL( *attrMan );
        transEnv = CPEngPresenceAttrTransactionEnv::NewL( aSlotId, *attrMan );
        CleanupStack::Pop(); //Leave reference open to MPEngPresenceAttrManager
        //transEnv took its ownership


        CleanupStack::PushL( transEnv );
        transEnv->SetOwner( *this );
        iAttributeTransEnvs->AddL( transEnv );
        CleanupStack::Pop(); //Leave initial reference open to CPEngPresenceAttrTransactionEnv
        }
    else {
        transEnv->Open();       // CSI: 65 #
        }

    return transEnv;
    }



// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::AttributeTransFactInstanceL()
//
// NOTE!! Method increases returned object reference count by one before
// returning it.
// -----------------------------------------------------------------------------
//
MPEngTransactionFactory* CPEngAttrLibObjectHolder::AttributeTransFactInstanceL(
    const CPEngSessionSlotId& aSlotId )
    {
    CPEngAttributeTransactionFactory* transFact =
        ( CPEngAttributeTransactionFactory* ) iAttributeTransFacts->FindById( aSlotId );
    if ( !transFact )
        {
        MPEngPresenceAttrManager* attrMan = AttributeManagerInstanceL( aSlotId );
        CleanupClosePushL( *attrMan );

        MPEngPresenceAttrTransactionEnv* attrEnv = AttributeTransEnvInstanceL( aSlotId );
        CleanupClosePushL( *attrEnv );

        transFact = CPEngAttributeTransactionFactory::NewL( aSlotId, *attrEnv, *attrMan );
        CleanupStack::Pop( 2 ); //CPEngAttributeTransactionFactory took ownership of
        //attrMan and attrEnv

        CleanupClosePushL( *transFact );
        transFact->SetOwner( *this );
        iAttributeTransFacts->AddL( transFact );
        CleanupStack::Pop(); //Leave initial CPEngAttributeTransactionFactory reference open
        }

    else {
        transFact->Open();      // CSI: 65 #
        }

    return transFact;
    }



// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::__DbgInstanceOrNull()
// Debug helper.
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
CPEngAttrLibObjectHolder* CPEngAttrLibObjectHolder::__DbgInstanceOrNull()
    {
    return ( CPEngAttrLibObjectHolder* ) Dll::Tls();
    }
#endif // _DEBUG


// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::__DbgObjectCounts()
// Debug helper.
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CPEngAttrLibObjectHolder::__DbgObjectCounts( TInt& aHolder,
                                                  TInt& aAttrMan,
                                                  TInt& aTransEnv,
                                                  TInt& aTransFact ) const
    {
    aHolder = 1;
    aAttrMan = iAttributeManagers->Count();
    aTransEnv = iAttributeTransEnvs->Count();
    aTransFact = iAttributeTransFacts->Count();
    }
#endif // _DEBUG



// -----------------------------------------------------------------------------
// __DoGetRefCount()
// Debug helper.
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void __DoGetRefCount( const CPEngSessionSlotObjectCon& aCon,
                      const CPEngSessionSlotId& aSlotId,
                      TInt& aRefCount )
    {

    const CPEngSessionSlotObject* obj = aCon.FindById( aSlotId );
    if ( obj )
        {
        aRefCount = obj->RefCount();
        }
    else
        {
        aRefCount = KErrNotFound;
        }
    }
#endif // _DEBUG


// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::__DbgAttrManRefCount()
// Debug helper.
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CPEngAttrLibObjectHolder::__DbgAttrManRefCount( const CPEngSessionSlotId& aSlotId,
                                                     TInt& aRefCount ) const
    {
    __DoGetRefCount( *iAttributeManagers, aSlotId, aRefCount );
    }
#endif // _DEBUG


// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::__DbgTransEnvRefCount()
// Debug helper.
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CPEngAttrLibObjectHolder::__DbgTransEnvRefCount( const CPEngSessionSlotId& aSlotId,
                                                      TInt& aRefCount ) const
    {
    __DoGetRefCount( *iAttributeTransEnvs, aSlotId, aRefCount );
    }
#endif // _DEBUG


// -----------------------------------------------------------------------------
// CPEngAttrLibObjectHolder::__DbgTransFactRefCount()
// Debug helper.
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CPEngAttrLibObjectHolder::__DbgTransFactRefCount( const CPEngSessionSlotId& aSlotId,
                                                       TInt& aRefCount ) const
    {
    __DoGetRefCount( *iAttributeTransFacts, aSlotId, aRefCount );
    }
#endif // _DEBUG


//  End of File
