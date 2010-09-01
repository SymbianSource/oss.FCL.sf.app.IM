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
* Description:  Attribute asynhronous data set hook.
*
*/


// INCLUDE FILES
#include    "CPEngAttrModelAsyncDataSetHook.h"
#include    "CPEngPresenceAttrModel.h"
#include    "MPEngPresenceAttrModelTypeImp.h"
#include    "PresenceDebugPrint.h"
#include    <E32Base.h>


//LOCAL constants
namespace
    {
    _LIT( KPEngAsyncDataSetHookPanic, "AsyncDataSetHook" );
    enum TPEngAsyncDataSetHookPanicReasons
        {
        EHookNotReady
        };

    void PanicAsyncDataSetHook( TPEngAsyncDataSetHookPanicReasons aPanicReason )
        {
        User::Panic( KPEngAsyncDataSetHookPanic, aPanicReason );
        }

    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttrModelAsyncDataSetHook::CPEngAttrModelAsyncDataSetHook
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttrModelAsyncDataSetHook::CPEngAttrModelAsyncDataSetHook()
        : CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPEngAttrModelAsyncDataSetHook::~CPEngAttrModelAsyncDataSetHook()
    {
    CActive::Cancel();
    }


// -----------------------------------------------------------------------------
// CPEngAttrModelAsyncDataSetHook::InitializeLC()
// -----------------------------------------------------------------------------
//
void CPEngAttrModelAsyncDataSetHook::InitializeLC( TRequestStatus& aRequestStatus,
                                                   CPEngPresenceAttrModel& aAttributeModel,
                                                   MPEngPresenceAttrModelTypeImp& aTypeImp )
    {
    __ASSERT_ALWAYS( !IsActive(), User::Leave( KErrInUse ) );
    CleanupStack::PushL( TCleanupItem( ResetTheHook, this ) );

    iStatusToComplete = &aRequestStatus;
    iHookOwner = &aAttributeModel;
    iHookedImp = &aTypeImp;
    }


// -----------------------------------------------------------------------------
// CPEngAttrModelAsyncDataSetHook::Status()
// -----------------------------------------------------------------------------
//
TRequestStatus& CPEngAttrModelAsyncDataSetHook::Status()
    {
    return iStatus;
    }


// -----------------------------------------------------------------------------
// CPEngAttrModelAsyncDataSetHook::Activate()
// -----------------------------------------------------------------------------
//
void CPEngAttrModelAsyncDataSetHook::Activate()
    {
    __ASSERT_ALWAYS( ( iStatusToComplete && iHookOwner && iHookedImp ),
                     PanicAsyncDataSetHook( EHookNotReady ) );

    if ( iStatus == KRequestPending )
        {
        PENG_DP_TXT( "CPEngAttrModelAsyncDataSetHook: Puting hook pending." );
        *iStatusToComplete = KRequestPending;
        SetActive();
        }

    else
        {
        PENG_DP_TXT( "CPEngAttrModelAsyncDataSetHook: Hook ended directly." );
        *iStatusToComplete = iStatus;
        Reset();
        }
    }



// -----------------------------------------------------------------------------
// CPEngAttrModelAsyncDataSetHook::RunL()
// -----------------------------------------------------------------------------
//
void CPEngAttrModelAsyncDataSetHook::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        //Data set done. Update the attribute state.
        iHookOwner->SetQualifier( ETrue );
        }

    //complete the hooked request with return status from real operation
    User::RequestComplete( iStatusToComplete, iStatus.Int() );

    Reset();
    }


// -----------------------------------------------------------------------------
// CPEngAttrModelAsyncDataSetHook::DoCancel()
// -----------------------------------------------------------------------------
//
void CPEngAttrModelAsyncDataSetHook::DoCancel()
    {
    iHookedImp->CancelSetDataAsync();

    //then complete the client request
    User::RequestComplete( iStatusToComplete, KErrCancel );

    Reset();
    }


// -----------------------------------------------------------------------------
// CPEngAttrModelAsyncDataSetHook::Reset()
// -----------------------------------------------------------------------------
//
void CPEngAttrModelAsyncDataSetHook::Reset()
    {
    //reset the state
    iHookOwner = NULL;
    iHookedImp = NULL;

    iStatusToComplete = NULL;
    }

// -----------------------------------------------------------------------------
// CPEngAttrModelAsyncDataSetHook::ResetTheHook()
// -----------------------------------------------------------------------------
//
void CPEngAttrModelAsyncDataSetHook::ResetTheHook( TAny* aHook )
    {
    CPEngAttrModelAsyncDataSetHook* self = static_cast< CPEngAttrModelAsyncDataSetHook* >( aHook );
    self->Reset();
    }


//  End of File




















