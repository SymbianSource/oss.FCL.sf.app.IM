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
* Description: this class is an operation handler for publishing user own attributes.
*
*/


// INCLUDE FILES
#include "CPEngAttributePublishOp.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngPresenceAttrManager.h"
#include "PEngAttrLibFactory.h"
#include "MPEngAdvTransactionStatus2.h"


#include <CPEngAttributeTransaction2.h>
#include <MPEngAttributeTransactionObserver2.h>
#include <MPEngPresenceAttrModel2.h>


// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CPEngAttributePublishOp* CPEngAttributePublishOp::NewL( TInt aPriority,
                                                        CPEngAttributeTransaction2& aInterface,
                                                        CPEngNWSessionSlotStorageProxy& aUsedSlot,
                                                        RPointerArray<MPEngPresenceAttrModel2>& aModels,
                                                        MPEngAttributeTransactionObserver2& aObserver,
                                                        RPEngManagerClient& aServer )
    {
    CPEngAttributePublishOp* self = new ( ELeave ) CPEngAttributePublishOp( aPriority,
                                                                            aInterface,
                                                                            aObserver,
                                                                            aServer );

    CleanupStack::PushL( self );
    self->ConstructL( aUsedSlot, aModels );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CPEngAttributePublishOp::~CPEngAttributePublishOp()
    {
    CActive::Cancel();

    if ( iModelsOwned )
        {
        iModels.ResetAndDestroy();
        }
    else
        {
        iModels.Reset();
        }

    if ( iAttrManager )
        {
        iAttrManager->Close();
        }
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEngAttributePublishOp::CPEngAttributePublishOp( TInt aPriority,
                                                  CPEngAttributeTransaction2& aInterface,
                                                  MPEngAttributeTransactionObserver2& aObserver,
                                                  RPEngManagerClient& aServer )
        : CPEngAsyncOperation( aPriority, aServer ),
        iInterface( aInterface ),
        iObserver( aObserver )
    {
    }



// EPOC default constructor can leave.
void CPEngAttributePublishOp::ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot,
                                          RPointerArray<MPEngPresenceAttrModel2>& aModels )
    {
    BaseConstructL( aUsedSlot );
    iAttrManager = PEngAttrLibFactory::AttributeManagerInstanceL( aUsedSlot.BaseId() );



    //Verify parameter models
    const TInt modelsCount( aModels.Count() );
    if ( modelsCount == 0 )
        {
        User::Leave( KErrArgument );
        }


    //Copy models pointers to internal array
    //Ownership of models remain still on the parameter array
    //Check at same that each pointer is valid
    TInt ii;
    for ( ii = 0; ii < modelsCount; ii++ )
        {
        MPEngPresenceAttrModel2* m = aModels[ii];
        if ( !m )
            {
            User::Leave( KErrArgument );
            }

        User::LeaveIfError( iModels.Append( aModels[ii] ) );
        }


    //Check against duplicates
    for ( ii = 0; ii < modelsCount - 1; ii++ )
        {
        for ( TInt jj( ii + 1 ); jj < modelsCount; jj++ )
            {
            if ( aModels[ii]->Type() == aModels[jj]->Type() )
                {
                User::Leave( KErrAlreadyExists );
                }
            }
        }


    //Check every model is properly locked for used NWSessionSlot &
    //can be used in NW operation
    for ( ii = 0; ii < modelsCount; ii++ )
        {
        MPEngPresenceAttrModel2& model = *aModels[ii];
        iAttrManager->ModelEditLockedFromThisSlotL( model );
        User::LeaveIfError( iAttrManager->ValidNetworkAttribute( model.Type() ) );
        }


    //Pack the publish request
    HBufC16* data = iAttrManager->PackModelArrayL( iModels );
    InitTransaction( data, EPEngTransOpOwnAttributePublish );
    }




// -----------------------------------------------------------------------------
// CPEngAttributePublishOp::Publish()
// -----------------------------------------------------------------------------
//
void CPEngAttributePublishOp::Publish()
    {
    iModelsOwned = ETrue;
    IssueTransaction();
    }




// -----------------------------------------------------------------------------
// CPEngAttributePublishOp::DoHandleOpSuccessL()
// -----------------------------------------------------------------------------
void CPEngAttributePublishOp::DoHandleOpSuccessL(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )

    {
    //Release the holded attributes models
    //Model delete frees the edit lock
    iModels.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CPEngAttributePublishOp::DoHandleOpFailure()
// -----------------------------------------------------------------------------
void CPEngAttributePublishOp::DoHandleOpFailure(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )
    {
    //Release the holded attributes models
    //Model delete frees the edit lock
    iModels.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CPEngAttributePublishOp::DoNotifyObserver()
// -----------------------------------------------------------------------------
TPEngAsyncOpResult CPEngAttributePublishOp::DoNotifyObserver(
    MPEngAdvTransactionStatus2& aStatus,
    TInt aTransactionOperation )
    {
    TRAPD( err, iObserver.HandleAttributeTransactionCompleteL(
               aStatus,
               iInterface,
               aTransactionOperation ) );
    if ( err != KErrNone )
        {
        iObserver.HandleAttributeTransactionError( err,
                                                   iInterface,
                                                   aTransactionOperation );
        }

    return EPEngAsyncOpCompleted;
    }


//End of file


