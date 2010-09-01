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
* Description: this class handles the search session
*
*/

// INCLUDE FILES
#include "CPEngSearchSessionHandler.h"
#include "CPEngNWSessionSlot2.h"
#include "PEngWVSearchPanics.h"
#include <CIMPSSAPSettings.h>
#include <impserrors.h>

//MACROS

#define RETURN_IF_NOT_PENDING( aId ) \
        {                                \
        if( !IsRequestPending( aId ) )   \
            {                            \
            return;                      \
            }                            \
        }                                \
     

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::NewL()
// -----------------------------------------------------------------------------
CPEngSearchSessionHandler* CPEngSearchSessionHandler::NewL(
    TInt aPriority,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngSearchSessionHandler* self = new ( ELeave ) CPEngSearchSessionHandler();
    CleanupStack::PushL( self );
    self->ConstructL( aPriority, aNWSessionSlotID );
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::CPEngSearchSessionHandler()
// -----------------------------------------------------------------------------
CPEngSearchSessionHandler::CPEngSearchSessionHandler()
        : iOperation( EPEngAccOp_Idle )
    {
    }


// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::~CPEngSearchSessionHandler()
// -----------------------------------------------------------------------------
CPEngSearchSessionHandler::~CPEngSearchSessionHandler()
    {
    iImpsAccClient.Unregister();
    iImpsEng.Close();

    delete iImpsSap;

    if ( iRequest )
        {
        User::RequestComplete( iRequest, KErrCancel );
        }
    }



// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::ConstructL()
// -----------------------------------------------------------------------------
void CPEngSearchSessionHandler::ConstructL( TInt aPriority,
                                            const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iImpsSap = CIMPSSAPSettings::NewL();


    //Load SAP data
    CPEngNWSessionSlot2* slot = CPEngNWSessionSlot2::NewLC( aNWSessionSlotID, aPriority );
    User::LeaveIfError( slot->OpenNWPresenceSessionOwnership() );
    User::LeaveIfError( slot->GetNWPresenceSessionSap( *iImpsSap ) );
    CleanupStack::PopAndDestroy( slot );


    User::LeaveIfError( iImpsEng.Connect() );
    iImpsAccClient.RegisterL( iImpsEng, this, aPriority );
    iImpsAccClient.RegisterErrorObserverL( *this );
    }


// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::OpenCspSessionL()
// -----------------------------------------------------------------------------
void CPEngSearchSessionHandler::OpenCspSessionL( TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iRequest, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( iImpsSap, SearchPanicOrLeaveL( EPEngNoSapContainerLoaded, KErrNotReady ) );


    TImpsCspIdentifier sessIndentifier;
    sessIndentifier.SetUserIdL( iImpsSap->SAPUserId() );
    sessIndentifier.SetSapL( iImpsSap->SAPAddress() );

    iOpId = iImpsAccClient.LoginL( sessIndentifier,
                                   iImpsSap->SAPUserPassword(),
                                   iImpsSap->ClientId(),
                                   iImpsSap->AccessPoint(),
                                   &( iImpsSap->ServerAuthenticationName() ),
                                   &( iImpsSap->ServerAuthenticationPassword() ) );





    iRequest = &aStatus;
    *iRequest = KRequestPending;
    iOperation = EPEngAccOp_LogIn;
    }



// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::CloseCspSessionL()
// -----------------------------------------------------------------------------
void CPEngSearchSessionHandler::CloseCspSessionL( TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iRequest, User::Leave( KErrAlreadyExists ) );

    iOpId = iImpsAccClient.LogoutL();
    iRequest = &aStatus;
    *iRequest = KRequestPending;
    iOperation = EPEngAccOp_LogOut;
    }



// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::ImpsEngine()
// -----------------------------------------------------------------------------
RImpsEng& CPEngSearchSessionHandler::ImpsEngine()
    {
    return iImpsEng;
    }


// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::IsRequestPending()
// -----------------------------------------------------------------------------
TBool CPEngSearchSessionHandler::IsRequestPending( TInt aId )
    {
    if ( ( aId == iOpId ) && iRequest )
        {
        return ETrue;
        }

    return EFalse;
    }



// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::HandleLoginL()
// From MImpsAccessHandler2
// -----------------------------------------------------------------------------
void CPEngSearchSessionHandler::HandleLoginL( TInt aId,
                                              TImpsCspIdentifier& /*aCspId*/ )
    {
    RETURN_IF_NOT_PENDING( aId );
    User::RequestComplete( iRequest, KErrNone );
    }


// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::HandleLoginL()
// From MImpsAccessHandler2
// -----------------------------------------------------------------------------
void CPEngSearchSessionHandler::HandleLogoutL( TInt aId,
                                               TImpsCspIdentifier& /*aCspId*/ )
    {
    RETURN_IF_NOT_PENDING( aId );
    User::RequestComplete( iRequest, KErrNone );
    }


// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::HandleLoginL()
// From MImpsAccessHandler2
// -----------------------------------------------------------------------------
void CPEngSearchSessionHandler::HandleLoginCancelL( TInt aCancelledOpId,
                                                    TImpsCspIdentifier& /*aCspId*/ )
    {
    RETURN_IF_NOT_PENDING( aCancelledOpId );
    User::RequestComplete( iRequest, KErrCancel );
    }


// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::HandleLoginL()
// From MImpsAccessHandler2
// -----------------------------------------------------------------------------
void CPEngSearchSessionHandler::HandleNbrSessionsL( TInt /*aId*/,
                                                    TInt /*aNbr*/,
                                                    TImpsCspIdentifier& /*aCspId*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngSearchSessionHandler::HandleErrorL()
// From MImpsErrorHandler2
// -----------------------------------------------------------------------------
void CPEngSearchSessionHandler::HandleErrorL( TInt aStatus,
                                              TInt aOpId,
                                              const TDesC* /*aDescription*/,
                                              const CImpsDetailed* /*aDetailedRes*/,
                                              TImpsCspIdentifier& /*aCspId*/ )
    {
    RETURN_IF_NOT_PENDING( aOpId );

    //Map ignorable errors to KErrNone
    switch ( iOperation )
        {
        case EPEngAccOp_LogOut:
            {
            if ( ( aStatus == KImpsErrorNotLogged ) ||
                 ( aStatus == KImpsErrorOthersLogged ) ||
                 ( aStatus == KImpsErrorShuttingDown ) ||
                 ( aStatus == KImpsErrorBearerSuspended ) )
                {
                aStatus = KErrNone;
                }
            break;
            }

        default:
            {
            //Nothing to do
            break;
            }
        }

    User::RequestComplete( iRequest, aStatus );
    }


// End of file
