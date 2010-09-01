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
* Description:  Presence attribute transactions.
*
*/

// INCLUDE FILES
#include "CPEngAttributeTransaction2Imp.h"
#include "CPEngAsyncOperation.h"
#include "CPEngNWSessionSlotID2.h"
#include "CPEngNWSessionSlotStorageProxy.h"


#include "CPEngAttributePublishOp.h"
#include "CPEngAttributeFetchOp.h"


//LOCAL constants
#ifdef _DEBUG
namespace
    {
    //Panic
    _LIT( KPEngAttrTransPanic, "PEngAttrTrans" );

    //Panic reasons
    enum TPEngAttrTransPanicReasons
        {
        EAttrTransUnkownTransOp
        };

    void AttrTransPanic( TPEngAttrTransPanicReasons aPanicReason )
        {
        User::Panic( KPEngAttrTransPanic, aPanicReason );
        }
    }
#endif


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributeTransaction2Imp* CPEngAttributeTransaction2Imp::NewL(
    CPEngAttributeTransaction2& aInterface,
    TInt aPriority,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngAttributeTransaction2Imp* self =
        new ( ELeave ) CPEngAttributeTransaction2Imp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngAttributeTransaction2Imp::~CPEngAttributeTransaction2Imp()
    {
    //Cancel also deletes the operation objects
    CancelPublishOwnAttributes();
    CancelFetchAttributes();
    iTransactionServer.Close();
    delete iUsedSlot;
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::CPEngAttributeTransaction2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeTransaction2Imp::CPEngAttributeTransaction2Imp(
    CPEngAttributeTransaction2& aInterface,
    TInt aPriority )
        : iInterface( aInterface ),
        iCActivePriority( aPriority )
    {
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeTransaction2Imp::ConstructL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    User::LeaveIfError( iTransactionServer.Connect() );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::IsPublishOwnAttributesActive()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeTransaction2Imp::IsPublishOwnAttributesActive() const
    {
    return ( iAttributePublishOp != NULL );     // CSI: 64 #
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::PublishAndUnLockOwnAttribute()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeTransaction2Imp::PublishAndUnLockOwnAttribute(
    MPEngPresenceAttrModel2*& aModel,
    MPEngAttributeTransactionObserver2& aObserver )
    {
    RPointerArray< MPEngPresenceAttrModel2 > models;
    TInt err = models.Append( aModel );
    if ( err == KErrNone )
        {
        err = PublishAndUnLockOwnAttributes( models, aObserver );
        }

    if ( err == KErrNone )
        {
        aModel = NULL;
        }

    models.Reset();
    return err;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::PublishAndUnLockOwnAttributes()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeTransaction2Imp::PublishAndUnLockOwnAttributes(
    RPointerArray<MPEngPresenceAttrModel2>& aModels,
    MPEngAttributeTransactionObserver2& aObserver )
    {
    if ( iAttributePublishOp )
        {
        return KErrInUse;
        }

    CPEngAttributePublishOp* op = NULL;
    TRAPD( err,
           op = CPEngAttributePublishOp::NewL( iCActivePriority,
                                               iInterface,
                                               *iUsedSlot,
                                               aModels,
                                               aObserver,
                                               iTransactionServer ); );

    if ( err == KErrNone )
        {
        //Success
        //==> Ownership of passed attribute objects is now  on the publish operation
        op->Publish();
        op->SetOwner( *this );
        iAttributePublishOp = op;

        aModels.Reset();
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::CancelPublishOwnAttributes()
// -----------------------------------------------------------------------------
//
void CPEngAttributeTransaction2Imp::CancelPublishOwnAttributes()
    {
    if ( iAttributePublishOp )
        {
        iAttributePublishOp->CancelOpD();
        iAttributePublishOp = NULL;
        }
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::IsFetchAttributesActive()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeTransaction2Imp::IsFetchAttributesActive() const
    {
    return( iAttributeFetchOp != NULL );        // CSI: 64 #
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::FetchAttributes()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeTransaction2Imp::FetchAttributes(
    const MDesCArray& aPresenceIDs,
    const TArray<TUint32>& aTypes,
    MPEngAttributeTransactionObserver2& aObserver,
    TInt aOptions,
    TBool aResultAsLoclObjects )
    {
    if ( iAttributeFetchOp )
        {
        return KErrInUse;
        }

    if ( aOptions != 0 )
        {
        return KErrArgument;
        }


    CPEngAttributeFetchOp* op = NULL;
    TRAPD( err,
           op = CPEngAttributeFetchOp::NewL( iCActivePriority,
                                             iInterface,
                                             *iUsedSlot,
                                             aResultAsLoclObjects,
                                             aPresenceIDs,
                                             aTypes,
                                             aObserver,
                                             iTransactionServer ); );

    if ( err == KErrNone )
        {
        op->Fetch();
        op->SetOwner( *this );
        iAttributeFetchOp = op;
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::CancelFetchAttributes()
// -----------------------------------------------------------------------------
//
void CPEngAttributeTransaction2Imp::CancelFetchAttributes()
    {
    if ( iAttributeFetchOp )
        {
        iAttributeFetchOp->CancelOpD();
        iAttributeFetchOp = NULL;
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::GetFetchedAttributes()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeTransaction2Imp::GetFetchedAttributes(
    RPointerArray< MPEngPresenceAttrModel2>& aModels )
    {
    if ( iAttributeFetchOp )
        {
        return iAttributeFetchOp->GetFetchedAttributes( aModels );
        }

    return KErrNotReady;
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTransaction2Imp::HandleAsyncOperationDestruction()
// from MPEngAsyncOperationOwner
// -----------------------------------------------------------------------------
//
void CPEngAttributeTransaction2Imp::HandleAsyncOperationDestruction(
    CPEngAsyncOperation* aOperation )
    {
    if ( aOperation == iAttributeFetchOp )
        {
        iAttributeFetchOp = NULL;
        return;
        }

    if ( aOperation == iAttributePublishOp )
        {
        iAttributePublishOp = NULL;
        return;
        }

    __ASSERT_DEBUG( EFalse, AttrTransPanic( EAttrTransUnkownTransOp ) );
    }


// End of File


