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
* Description: this class is an operation handler for performing attribute
* list publish operation.
*
*/


// INCLUDE FILES
#include "CPEngAttrLstPublishSyncOp.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngAdvTransactionStatus2.h"

#include <CPEngAttributeListTransaction2.h>
#include <MPEngAttributeListTransactionObserver2.h>



// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CPEngAttrLstPublishSyncOp* CPEngAttrLstPublishSyncOp::NewL(
    TInt aPriority,
    CPEngAttributeListTransaction2& aInterface,
    CPEngNWSessionSlotStorageProxy& aUsedSlot,
    MPEngAttributeListTransactionObserver2& aObserver,
    RPEngManagerClient& aServer )
    {
    CPEngAttrLstPublishSyncOp* self = new ( ELeave ) CPEngAttrLstPublishSyncOp( aPriority,
                                                                                aInterface,
                                                                                aObserver,
                                                                                aServer );

    CleanupStack::PushL( self );
    self->ConstructL( aUsedSlot );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CPEngAttrLstPublishSyncOp::~CPEngAttrLstPublishSyncOp()
    {
    CActive::Cancel();
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEngAttrLstPublishSyncOp::CPEngAttrLstPublishSyncOp( TInt aPriority,
                                                      CPEngAttributeListTransaction2& aInterface,
                                                      MPEngAttributeListTransactionObserver2& aObserver,
                                                      RPEngManagerClient& aServer )
        : CPEngAsyncOperation( aPriority, aServer ),
        iInterface( aInterface ),
        iObserver( aObserver )
    {
    }



// EPOC default constructor can leave.
void CPEngAttrLstPublishSyncOp::ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot )
    {
    BaseConstructL( aUsedSlot );

    //Attribute list publish doesn't take any data
    InitTransaction( NULL, EPEngTransOpAttrListPublish );
    }


// -----------------------------------------------------------------------------
// CPEngAttrLstPublishSyncOp::Publish()
// -----------------------------------------------------------------------------
//
void CPEngAttrLstPublishSyncOp::Publish()
    {
    IssueTransaction();
    }


// -----------------------------------------------------------------------------
// CPEngAttrLstPublishSyncOp::DoHandleOpSuccessL()
// -----------------------------------------------------------------------------
void CPEngAttrLstPublishSyncOp::DoHandleOpSuccessL(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )

    {
    }


// -----------------------------------------------------------------------------
// CPEngAttrLstPublishSyncOp::DoHandleOpFailure()
// -----------------------------------------------------------------------------
void CPEngAttrLstPublishSyncOp::DoHandleOpFailure(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttrLstPublishSyncOp::DoNotifyObserver()
// -----------------------------------------------------------------------------
TPEngAsyncOpResult CPEngAttrLstPublishSyncOp::DoNotifyObserver(
    MPEngAdvTransactionStatus2& aStatus,
    TInt aTransactionOperation )
    {
    TRAPD( err, iObserver.HandleAttributeListTransactionCompleteL(
               aStatus,
               iInterface,
               aTransactionOperation ) );
    if ( err != KErrNone )
        {
        iObserver.HandleAttributeListTransactionError( err,
                                                       iInterface,
                                                       aTransactionOperation );
        }

    return EPEngAsyncOpCompleted;
    }


//End of file


