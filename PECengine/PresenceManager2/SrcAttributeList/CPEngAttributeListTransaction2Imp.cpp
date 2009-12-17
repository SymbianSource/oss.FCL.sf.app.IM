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
* Description:  Presence contact list transactions.
*
*/

// INCLUDE FILES
#include "CPEngAttributeListTransaction2Imp.h"
#include "CPEngAsyncOperation.h"
#include "CPEngNWSessionSlotID2.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "CPEngAttrLstPublishSyncOp.h"


//LOCAL constants
#ifdef _DEBUG
namespace
    {
    //Panic
    _LIT( KPEngAttrListTransPanic, "PEngAtLstTrans" );

    //Panic reasons
    enum TPEngAttrListTransPanicReasons
        {
        EAttrListTransUnkownTransOp
        };

    void PEngAttrListTransPanic( TPEngAttrListTransPanicReasons aPanicReason )
        {
        User::Panic( KPEngAttrListTransPanic, aPanicReason );
        }
    }
#endif


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransaction2Imp* CPEngAttributeListTransaction2Imp::NewL(
    CPEngAttributeListTransaction2& aInterface,
    TInt aPriority,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngAttributeListTransaction2Imp* self =
        new ( ELeave ) CPEngAttributeListTransaction2Imp( aInterface, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngAttributeListTransaction2Imp::~CPEngAttributeListTransaction2Imp()
    {
    //Cancel also deletes the operation objects
    CancelPublishAttributeLists();
    iTransactionServer.Close();
    delete iUsedSlot;
    }



// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2Imp::CPEngAttributeListTransaction2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransaction2Imp::CPEngAttributeListTransaction2Imp(
    CPEngAttributeListTransaction2& aInterface,
    TInt aPriority )
        : iInterface( aInterface ),
        iCActivePriority( aPriority )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransaction2Imp::ConstructL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    User::LeaveIfError( iTransactionServer.Connect() );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2Imp::IsPublishAttributeListsActive()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeListTransaction2Imp::IsPublishAttributeListsActive() const
    {
    return ( iAttributeListPublishOp != NULL );     // CSI: 64 #
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2Imp::PublishAttributeLists()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListTransaction2Imp::PublishAttributeLists(
    MPEngAttributeListTransactionObserver2& aObserver )
    {
    if ( iAttributeListPublishOp )
        {
        return KErrInUse;
        }

    CPEngAttrLstPublishSyncOp* op = NULL;
    TRAPD( err,
           op = CPEngAttrLstPublishSyncOp::NewL( iCActivePriority,
                                                 iInterface,
                                                 *iUsedSlot,
                                                 aObserver,
                                                 iTransactionServer ); );

    if ( err == KErrNone )
        {
        //Success
        op->Publish();
        op->SetOwner( *this );
        iAttributeListPublishOp = op;

        }

    return err;
    }



// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2Imp::CancelPublishAttributeLists()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransaction2Imp::CancelPublishAttributeLists()
    {
    if ( iAttributeListPublishOp )
        {
        iAttributeListPublishOp->CancelOpD();
        iAttributeListPublishOp = NULL;
        }
    }



// -----------------------------------------------------------------------------
// CPEngAttributeListTransaction2Imp::HandleAsyncOperationDestruction()
// from MPEngAsyncOperationOwner
// -----------------------------------------------------------------------------
//
void CPEngAttributeListTransaction2Imp::HandleAsyncOperationDestruction(
    CPEngAsyncOperation* aOperation )
    {
    if ( aOperation == iAttributeListPublishOp )
        {
        iAttributeListPublishOp = NULL;
        return;
        }

    __ASSERT_DEBUG( EFalse, PEngAttrListTransPanic( EAttrListTransUnkownTransOp ) );
    }


// End of File


