/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: this class handles the network session operation
*
*/

// INCLUDE FILES
#include "CPEngNWSessionProxy.h"
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngSessionSlotId.h"

#include <CPEngNWSessionSlotID2.h>
#include <E32Std.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngNWSessionProxy* CPEngNWSessionProxy::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CPEngNWSessionProxy* self = new ( ELeave ) CPEngNWSessionProxy;
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }




// Destructor
CPEngNWSessionProxy::~CPEngNWSessionProxy()
    {
    Close();
    delete iNWSessionSlotID;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::CPEngNWSessionProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionProxy::CPEngNWSessionProxy()
        : iInitialized( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionProxy::ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iNWSessionSlotID = aNWSessionSlotID.Implementation()->CloneL();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::InitializeConnection()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionProxy::InitializeConnection()
    {
    if ( iInitialized )
        {
        return KErrNone;
        }

    TInt err = iMainClient.Connect();
    if ( err == KErrNone )
        {
        TRAP( err,
            {
            HBufC8* slotIdPkg = iNWSessionSlotID->BasePart().PackLC();
            User::LeaveIfError( iSessionClient.Connect( *slotIdPkg,
                                                        iNWSessionSlotID->AppId(),
                                                        iMainClient ) );
            CleanupStack::PopAndDestroy( slotIdPkg );
            } );
        }

    if ( err == KErrNone )
        {
        iInitialized = ETrue;
        }
    else
        {
        Close();
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::Close()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionProxy::Close()
    {
    iSessionClient.Close();
    iMainClient.Close();
    iInitialized = EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::OpenSession()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionProxy::OpenSession( const TDesC8& aLogInData,
                                       TRequestStatus& aStatus )
    {
    TInt err = InitializeConnection();
    if ( err != KErrNone )
        {
        return err;
        }

    return iSessionClient.LogIn( aLogInData, aStatus );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::CancelOpenSession()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionProxy::CancelOpenSession()
    {
    if ( iInitialized )
        {
        iSessionClient.CancelLogIn();
        }
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::CloseSession()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionProxy::CloseSession( TRequestStatus& aStatus )
    {
    if ( !iInitialized )
        {
        return KErrNotReady;
        }

    return iSessionClient.Detach( aStatus );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::CancelCloseSession()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionProxy::CancelCloseSession()
    {
    if ( iInitialized )
        {
        iSessionClient.CancelDetach();
        }
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::ForceCloseSession()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionProxy::ForceCloseSession( TRequestStatus& aStatus )
    {
    TInt err = InitializeConnection();
    if ( err != KErrNone )
        {
        return err;
        }

    return iSessionClient.ForceLogOut( aStatus );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::CancelForceCloseSession()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionProxy::CancelForceCloseSession()
    {
    if ( iInitialized )
        {
        iSessionClient.CancelLogOut();
        }
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::OpenOwnership()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionProxy::OpenOwnership()
    {
    TInt err = InitializeConnection();
    if ( err != KErrNone )
        {
        return err;
        }

    return iSessionClient.Attach();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::StoreOwnership()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionProxy::StoreOwnership( const TDesC16& aOwnerID )
    {
    TInt err = KErrNotReady;

    if ( iInitialized )
        {
        err = iSessionClient.LeaveAlive( aOwnerID );
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::RestoreOwnership()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionProxy::RestoreOwnership( const TDesC16& aOwnerID )
    {
    TInt err = InitializeConnection();
    if ( err != KErrNone )
        {
        return err;
        }

    return iSessionClient.Attach( aOwnerID );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::GetServiceTree()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionProxy::GetServiceTree( TPEngWVCspServicesTree2& aCspTree )
    {
    TInt err = InitializeConnection();
    if ( err != KErrNone )
        {
        return err;
        }

    return iSessionClient.GetServiceTree( aCspTree );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionProxy::GetLoginData()
// -----------------------------------------------------------------------------
//
TInt CPEngNWSessionProxy::GetLoginData( HBufC8*& aLogInData )
    {
    TInt err = InitializeConnection();
    if ( err != KErrNone )
        {
        return err;
        }
    aLogInData = NULL;
    return iSessionClient.LogInData( aLogInData );
    }


//  End of File

