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
* Description: this class is an operation handler for fetching attributes from network.
*
*/

// INCLUDE FILES
#include "CPEngAttributeFetchOp.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngPresenceAttrManager.h"
#include "MPEngAdvTransactionStatus2.h"
#include "PEngAttrLibFactory.h"



#include <CPEngAttributeTransaction2.h>
#include <MPEngAttributeTransactionObserver2.h>
#include <MPEngPresenceAttrModel2.h>



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEngAttributeFetchOp::NewL()
// -----------------------------------------------------------------------------
CPEngAttributeFetchOp* CPEngAttributeFetchOp::NewL( TInt aPriority,
                                                    CPEngAttributeTransaction2& aInterface,
                                                    CPEngNWSessionSlotStorageProxy& aUsedSlot,
                                                    TBool aResultAsLocalObjects,
                                                    const MDesCArray& aPresenceIDs,
                                                    const TArray<TUint32>& aTypes,
                                                    MPEngAttributeTransactionObserver2& aObserver,
                                                    RPEngManagerClient& aServer )
    {
    CPEngAttributeFetchOp* self = new ( ELeave ) CPEngAttributeFetchOp( aPriority,
                                                                        aInterface,
                                                                        aObserver,
                                                                        aServer );
    CleanupStack::PushL( self );
    self->ConstructL( aUsedSlot,
                      aPresenceIDs,
                      aTypes,
                      aResultAsLocalObjects );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPEngAttributeFetchOp::~CPEngAttributeFetchOp()
// -----------------------------------------------------------------------------
CPEngAttributeFetchOp::~CPEngAttributeFetchOp()
    {
    CActive::Cancel();

    iResultModels.ResetAndDestroy();

    if ( iAttrManager )
        {
        iAttrManager->Close();
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchOp::CPEngAttributeFetchOp()
// -----------------------------------------------------------------------------
CPEngAttributeFetchOp::CPEngAttributeFetchOp( TInt aPriority,
                                              CPEngAttributeTransaction2& aInterface,
                                              MPEngAttributeTransactionObserver2& aObserver,
                                              RPEngManagerClient& aServer )
        : CPEngAsyncOperation( aPriority, aServer ),
        iInterface( aInterface ),
        iObserver( aObserver )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchOp::ConstructL()
// -----------------------------------------------------------------------------
void CPEngAttributeFetchOp::ConstructL( CPEngNWSessionSlotStorageProxy& aUsedSlot,
                                        const MDesCArray& aPresenceIDs,
                                        const TArray<TUint32>& aTypes,
                                        TBool aResultAsLocalObjects )
    {
    CPEngAsyncOperation::BaseConstructL( aUsedSlot );
    iAttrManager = PEngAttrLibFactory::AttributeManagerInstanceL( aUsedSlot.BaseId() );


    TInt ii;
    TInt jj;


    //Verify attribute types
    const TInt typesCount( aTypes.Count() );
    if ( typesCount == 0 )
        {
        User::Leave( KErrArgument );
        }


    for ( ii = 0; ii < typesCount - 1; ii++ )
        {
        for ( jj = ii + 1 ; jj < typesCount; jj++ )
            {
            if ( aTypes[ii] == aTypes[jj] )
                {
                User::Leave( KErrAlreadyExists );
                }
            }
        }

    //Check every type can be used in NW operation
    for ( ii = 0; ii < typesCount; ii++ )
        {
        User::LeaveIfError( iAttrManager->ValidNetworkAttribute( aTypes[ii] ) );
        }


    //Verify presence ID's
    const TInt presenceIDCount( aPresenceIDs.MdcaCount() );
    if ( presenceIDCount == 0 )
        {
        User::Leave( KErrArgument );
        }

    for ( ii =  0; ii < presenceIDCount - 1; ii++ )
        {
        for ( jj = ii + 1; jj < presenceIDCount; jj++ )
            {
            if ( aPresenceIDs.MdcaPoint( ii ) ==
                 aPresenceIDs.MdcaPoint( jj ) )
                {
                User::Leave( KErrAlreadyExists );
                }
            }
        }

    //Generate the request
    HBufC16* data = iAttrManager->PackFetchRequestL( aPresenceIDs,
                                                     aTypes );

    if ( aResultAsLocalObjects )
        {
        InitTransaction( data, EPEngTransOpAttributeFetchToObjects );
        }
    else
        {
        InitTransaction( data, EPEngTransOpAttributeFetchToCache );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchOp::Fetch()
// -----------------------------------------------------------------------------
void CPEngAttributeFetchOp::Fetch()
    {
    IssueTransaction();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchOp::GetFetchedAttributes()
// -----------------------------------------------------------------------------
TInt CPEngAttributeFetchOp::GetFetchedAttributes(
    RPointerArray< MPEngPresenceAttrModel2>& aModels )
    {
    const TInt count = iResultModels.Count();
    for ( TInt ii = 0; ii < count; ii++ )
        {
        MPEngPresenceAttrModel2* m = iResultModels[ 0 ];
        TInt err = aModels.Append( m );
        if ( err != KErrNone )
            {
            return err;
            }

        iResultModels.Remove( 0 );
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchOp::DoHandleOpSuccessL()
// -----------------------------------------------------------------------------
void CPEngAttributeFetchOp::DoHandleOpSuccessL( MPEngAdvTransactionStatus2& aStatus,
                                                TInt aTransactionOperation )

    {
    if ( aTransactionOperation == EPEngTransOpAttributeFetchToObjects )
        {
        TPtrC16 dataResult( NULL, 0 );
        aStatus.GetFirstDataResultByOperation( EPEngTransOpAttributeFetchToObjects,
                                               dataResult );
        if ( dataResult.Length() > 0 )
            {
            iAttrManager->UnpackModelArrayL( dataResult,
                                             EPEngInstanceOptionsNone,
                                             iResultModels );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchOp::DoHandleOpFailure()
// -----------------------------------------------------------------------------
void CPEngAttributeFetchOp::DoHandleOpFailure(
    MPEngAdvTransactionStatus2& /*aStatus*/,
    TInt /*aTransactionOperation*/ )
    {
    iResultModels.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeFetchOp::DoNotifyObserver()
// -----------------------------------------------------------------------------
TPEngAsyncOpResult CPEngAttributeFetchOp::DoNotifyObserver(
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



// End of file
