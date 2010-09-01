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
* Description:  Shared basic implementation for client plug-ins.
*
*/

// INCLUDE FILES
#include <E32std.h>

#include <CPEngAttributeStore2.h>
#include <CPEngAttributeTransaction2.h>
#include <CPEngAttributeListStore2.h>
#include <CPEngAttributeListTransaction2.h>
#include <CPEngContactListStore2.h>
#include <CPEngContactListTransaction2.h>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlot2.h>
#include <PEngPresenceEngineConsts2.h>
#include <MPEngTransactionStatus2.h>
#include <PEngWVPresenceErrors2.h>
#include <PEngWVServices2.h>

#include "CCnUiClientPluginBase.h"
#include "IMPSPublishLevelPolicy.h"
#include "impscommonuibuilddefinitions.h"


// ================= MEMBER FUNCTIONS =======================

// Destructor
CCnUiClientPluginBase::~CCnUiClientPluginBase()
    {
    CancelAllProcessingAndCompleteIfNeeded();

    delete iPEngAttributeStore;
    delete iPEngAttributeListStore;

    delete iPEngAttributePublisher;
    delete iPEngAttributeListPublisher;

    delete iPEngCntListStore;
    delete iPEngCntListPublisher;

    delete iNWSessionSlotID;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiClientPluginBase::CCnUiClientPluginBase( TIMPSConnectionClient aClientID )
        : iClientID( aClientID ),
        iExplicitSyncNeeded( EFalse ),
        iStepProcessingActive( EFalse )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiClientPluginBase::ConstructL( CPEngNWSessionSlotID2& aNWSessionSlotID )
    {

    // Create contact-list store & publisher
    iPEngCntListStore = CPEngContactListStore2::NewL( aNWSessionSlotID );
    iPEngCntListPublisher = CPEngContactListTransaction2::NewL( aNWSessionSlotID );

    // Create attribute-list store & publisher
    iPEngAttributeListStore = CPEngAttributeListStore2::NewL( aNWSessionSlotID );
    iPEngAttributeListPublisher = CPEngAttributeListTransaction2::NewL( aNWSessionSlotID );

    // Create attribute store & publisher
    iPEngAttributeStore = CPEngAttributeStore2::NewL( aNWSessionSlotID );
    iPEngAttributePublisher = CPEngAttributeTransaction2::NewL( aNWSessionSlotID );

    iNWSessionSlotID = aNWSessionSlotID.CloneL();
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::ClientID()
// From MCnUiClientPlugin API
// -----------------------------------------------------------------------------
//
TIMPSConnectionClient CCnUiClientPluginBase::ClientID()
    {
    return iClientID;
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::ReleaseDynamicResources()
// From MCnUiClientPlugin API
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::ReleaseDynamicResources()
    {
    //Cancel first possibly running processing stuffs
    CancelAllPEngPublishers();
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::ConnectionOpenInitL()
// From MCnUiClientPlugin API
// Connection open
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::ConnectionOpenInitL( TBool /*aSapConnectionOpen*/,
                                                 const CIMPSSAPSettings& /*aSap*/ )
    {
    iExplicitSyncNeeded = ETrue;
    }

// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::VerifySapCapabilitiesL()
// From MCnUiClientPlugin API
// Connection open
// -----------------------------------------------------------------------------
//
TBool CCnUiClientPluginBase::VerifySapCapabilitiesL()
    {
    CPEngNWSessionSlot2* slot = CPEngNWSessionSlot2::NewLC( *iNWSessionSlotID );
    TPEngWVCspServicesTree2 cspTree;
    TPckg<TPEngWVCspServicesTree2> cspTreePtr( cspTree );
    User::LeaveIfError( slot->GetOption( KPEngWVCspServicesTree2,
                                         KPEngNWSessionSlotTransportQuery,
                                         cspTreePtr ) );

    CleanupStack::PopAndDestroy( slot );
    //let the real client to do the verification
    return DoVerifySapCapabilities( cspTree );
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::DoPostLoginProsessingL()
// From MCnUiClientPlugin API
// Connection open
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::DoPostLoginProsessingL( TRequestStatus& aStatus )
    {
    if ( iExplicitSyncNeeded )
        {
        //let the real client to start the processing if needed
        DoStartExplicitPostLoginSyncL();
        if ( iStepProcessingActive )
            {
            //processing started
            iRequest = &aStatus;
            *iRequest = KRequestPending;
            iStepProcessType = EExplicitPostLoginSync;
            }
        }
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::CancelPostLoginProsessing()
// From MCnUiClientPlugin API
// Connection open
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::CancelPostLoginProsessing()
    {
    CancelAllProcessingAndCompleteIfNeeded();
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::ConnectionCloseInitL()
// From MCnUiClientPlugin API
// Connection close
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::ConnectionCloseInitL( const CIMPSSAPSettings& /*aSap*/ )
    {
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::DoPreLogoutProsessingL()
// From MCnUiClientPlugin API
// Connection close
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::DoPreLogoutProsessingL( TRequestStatus& aStatus )
    {
    //let the real client to start the processing if needed
    DoStartPreLogoutPublishL();
    if ( iStepProcessingActive )
        {
        //processing started
        iRequest = &aStatus;
        *iRequest = KRequestPending;
        iStepProcessType = EPreLogoutPublish;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::CancelPreLogoutProsessing()
// From MCnUiClientPlugin API
// Connection close
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::CancelPreLogoutProsessing()
    {
    CancelAllProcessingAndCompleteIfNeeded();
    }




// -----------------------------------------------------------------------------
//
// Callback methods for PENG publishers
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::HandleContactListTransactionError()
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::HandleContactListTransactionError(
    TInt aError,
    CPEngContactListTransaction2& /*aAttributeTransaction*/,
    TInt aTransactionOperation )
    {
    if ( aTransactionOperation == EPEngTransOpCntListBaseSync )
        {
        HandleStepComplete( EBaseCntListSynch, aError );
        }
    else
        {
        HandleStepComplete( ECntListPublish, aError );
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::HandleContactListTransactionCompleteL()
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::HandleContactListTransactionCompleteL(
    MPEngTransactionStatus2& aStatus ,
    CPEngContactListTransaction2& /* aContactListTransaction */,
    TInt aTransactionOperation )
    {
    TInt error( aStatus.Status() );
    if ( error == KPEngNwErrPartiallySuccessful )
        {
        // ignore partial successfull
        error = KErrNone;
        }
    User::LeaveIfError( error );
    if ( aTransactionOperation == EPEngTransOpCntListBaseSync )
        {
        HandleStepComplete( EBaseCntListSynch, KErrNone );
        }
    else
        {
        HandleStepComplete( ECntListPublish, KErrNone );
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::HandleAttributeListTransactionError()
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::HandleAttributeListTransactionError(
    TInt aError,
    CPEngAttributeListTransaction2& /* aAttributeTransaction */,
    TInt /* aTransactionOperation */ )
    {
    HandleStepComplete( EAttribListPublish, aError );
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::HandleAttributeListTransactionCompleteL()
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::HandleAttributeListTransactionCompleteL(
    MPEngTransactionStatus2& aStatus,
    CPEngAttributeListTransaction2& /* aAttributeListTransaction */,
    TInt /* aTransactionOperation */ )
    {
    TInt error( aStatus.Status() );
    if ( error == KPEngNwErrPartiallySuccessful )
        {
        // ignore partial successfull
        error = KErrNone;
        }
    User::LeaveIfError( error );
    HandleStepComplete( EAttribListPublish, KErrNone );
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::HandleAttributeTransactionError()
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::HandleAttributeTransactionError(
    TInt aError,
    CPEngAttributeTransaction2& /* aAttributeTransaction */,
    TInt /* aTransactionOperation */ )
    {
    HandleStepComplete( EAttributePublish, aError );
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::HandleAttributeTransactionCompleteL()
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::HandleAttributeTransactionCompleteL(
    MPEngTransactionStatus2& aStatus ,
    CPEngAttributeTransaction2& /* aAttributeTransaction */,
    TInt /* aTransactionOperation */ )
    {
    TInt error( aStatus.Status() );
    if ( error == KPEngNwErrPartiallySuccessful )
        {
        // ignore partial successfull
        error = KErrNone;
        }
    User::LeaveIfError( error );
    HandleStepComplete( EAttributePublish, KErrNone );
    }



// -----------------------------------------------------------------------------
//
// Local helpers
//
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::SetActive()
// Signalling method for derived clients
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::SetStepProcessingActive()
    {
    iStepProcessingActive = ETrue;
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::HandleStepComplete()
// Private helper
// Handles the step completion and lets the client continue processing if needed
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::HandleStepComplete( TStepId aStepId, TInt aStepStatus )
    {
    if ( !iStepProcessingActive )
        {
        //Error coming from some publisher cancel, e.g.
        //explicit cancel round running
        //==> nothing to do (direct return protects agains cancel nesting.)
        return;
        }


    //not active anymore
    iStepProcessingActive = EFalse;

    //let the derived class to handle the step completion
    TInt err = KErrNone;
    if ( iStepProcessType == EExplicitPostLoginSync )
        {
        TRAP( err, DoHandleExplicitPostLoginSyncStepCompleteL( aStepId,  aStepStatus ) );
        }

    else
        {
        //EPreLogoutPublish
        TRAP( err, DoHandlePreLogoutPublishStepCompleteL( aStepId, aStepStatus ) );
        }

    if ( err != KErrNone )
        {
        //if starting failed, complete away with catched error..
        iStepProcessingActive = EFalse;
        aStepStatus = err;
        }


    if ( !iStepProcessingActive )
        {
        //No active processing anymore

        //cancel still possibly existing processing
        CancelAllPEngPublishers();

        //and complete the original request
        User::RequestComplete( iRequest, aStepStatus );
        }
    }




// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::CancelAllProcessingAndCompleteIfNeeded()
//
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::CancelAllProcessingAndCompleteIfNeeded()
    {
    //not definitely active anymore
    iStepProcessingActive = EFalse;

    //cancel all sub precessing parts
    CancelAllPEngPublishers();

    //and complete the original request if still having one
    if ( iRequest )
        {
        User::RequestComplete( iRequest, KErrCancel );
        }
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::CancelAllPEngPublishers()
// Private helper
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::CancelAllPEngPublishers()
    {
    // nothing done here at the moment
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginBase::LoginCancelledL()
// Private helper
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginBase::LoginCancelledL()
    {
    // empty implementation
    }


//  End of File

