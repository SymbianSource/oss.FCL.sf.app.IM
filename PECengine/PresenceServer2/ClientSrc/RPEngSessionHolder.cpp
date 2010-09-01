/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sub session of the Presence server
*  				 It is used to maintain one session slot.
*
*/


//  INCLUDES
#include <f32file.h>

#include "RPEngSessionHolder.h"

#include "TPEngServerMessages.h"

#include "PEngWVServices2.h"


// PEC Engine internal constants
#include "PEngInternalGlobalConsts.h"

// debug prints
#include "PresenceDebugPrint.h"

// CONSTANTS
// estimation of the Log In data buffer size, used for optimization
const TInt KPEngLogInDataEstimation = 300;


// MACROS
/**
 * Return error code if client is not connected
 */
#define RETURN_ERR_IF_NOT_CONNECTED()\
    if(!iConnected)\
        {\
        return KErrDisconnected;\
        }
/**
 * Return from function if client is not connected
 */
#define RETURN_IF_NOT_CONNECTED()\
    if(!iConnected)\
        {\
        return;\
        }

/**
 * Return Error code if pointer is NULL
 */
#define RETURN_IF_NULL( aPointer, aErrorCode )\
    if ( !aPointer )\
        {\
        return aErrorCode;\
        }

// ============================ EXPORTED FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPEngSessionHolder::RPEngSessionHolder
// -----------------------------------------------------------------------------
//
EXPORT_C RPEngSessionHolder::RPEngSessionHolder()
        : iLogInData( KPEngLogInDataEstimation )
    {
    }

// Destructor
EXPORT_C RPEngSessionHolder::~RPEngSessionHolder()
    {
    }

// ==============  CONNECTION FUNCTIONS ========================================

// -----------------------------------------------------------------------------
// RPEngSessionHolder::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngSessionHolder::Connect(
    const TDesC8& aSessSlotId,
    const TDesC16& aAppId,
    RPEngManagerClient& aManagerClient )
    {
    PENG_DP_TXT( "RPEngSessionHolder::Connect()" );

    TIpcArgs messArguments;
    messArguments.Set( KMessageSlot0, &aSessSlotId );
    messArguments.Set( KMessageSlot1, &aAppId );

    TInt err( RSubSessionBase::CreateSubSession( aManagerClient,
                                                 EMainSessCreateHolderSubSession,
                                                 messArguments ) );

    iConnected = ( err == KErrNone );
    return err;
    }

// -----------------------------------------------------------------------------
// RPEngSessionHolder::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngSessionHolder::Close()
    {
    PENG_DP_TXT( "RPEngSessionHolder::Close()" );

    // all requests from this sub-session are canceled by framework
    CloseSubSession( EMainSessCloseSubSession );
    iConnected = EFalse;


#if _BullseyeCoverage
    cov_write();
#endif
    }


// ============== NEW CONNECTION HANDLING FUNCTIONS ============================

// -----------------------------------------------------------------------------
// RPEngSessionHolder::LogIn
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngSessionHolder::LogIn(
    const TDesC8& aLogInData,
    TRequestStatus& aStatus )
    {
    RETURN_ERR_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    iLogInData = aLogInData.Length();
    messArgs.Set( KMessageSlot0, &aLogInData );
    SendReceive( ESubSessLogIn , messArgs, aStatus );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// RPEngSessionHolder::CancelLogIn
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngSessionHolder::CancelLogIn()
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, ESubSessLogIn );
    SendReceive( ESubSessCancelRequest , messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngSessionHolder::Attach
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngSessionHolder::Attach()
    {
    RETURN_ERR_IF_NOT_CONNECTED();
    return SendReceive( ESubSessAttachHolder );
    }

// -----------------------------------------------------------------------------
// RPEngSessionHolder::Attach
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngSessionHolder::Attach(
    const TDesC& aProcessName )
    {
    RETURN_ERR_IF_NOT_CONNECTED();
    TIpcArgs messArg;
    messArg.Set( KMessageSlot0, &aProcessName );
    return SendReceive( ESubSessAttachWithProcessName, messArg );
    }


// -----------------------------------------------------------------------------
// RPEngSessionHolder::ForceLogOut
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngSessionHolder::ForceLogOut(
    TRequestStatus& aStatus )
    {
    RETURN_ERR_IF_NOT_CONNECTED();
    SendReceive( ESubSessForceLogOut, aStatus );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// RPEngSessionHolder::CancelLogOut
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngSessionHolder::CancelLogOut()
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, ESubSessForceLogOut );
    SendReceive( ESubSessCancelRequest , messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngSessionHolder::Detach
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngSessionHolder::Detach(
    TRequestStatus& aStatus )
    {
    RETURN_ERR_IF_NOT_CONNECTED();
    SendReceive( ESubSessDetachHolder, aStatus );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// RPEngSessionHolder::CancelDetach
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngSessionHolder::CancelDetach()
    {
    RETURN_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, ESubSessDetachHolder );
    SendReceive( ESubSessCancelRequest , messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngSessionHolder::LeaveAlive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngSessionHolder::LeaveAlive(
    const TDesC& aProcessName )
    {
    RETURN_ERR_IF_NOT_CONNECTED();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, &aProcessName );
    return SendReceive( ESubSessLeaveAlive, messArgs );
    }


// -----------------------------------------------------------------------------
// RPEngSessionHolder::GetServiceTree
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngSessionHolder::GetServiceTree(
    TPEngWVCspServicesTree2& aServiceTree )
    {
    RETURN_ERR_IF_NOT_CONNECTED();
    TPckgBuf<TPEngWVCspServicesTree2> serviceTree( aServiceTree );
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, &serviceTree );
    TInt err( SendReceive( ESubSessGetServiceTree, messArgs ) );
    aServiceTree = serviceTree();
    return err;
    }


// -----------------------------------------------------------------------------
// RPEngSessionHolder::LogInData
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngSessionHolder::LogInData(
    HBufC8*& aLogInData )
    {
    RETURN_ERR_IF_NOT_CONNECTED();
    delete aLogInData;
    aLogInData = HBufC8::New( iLogInData );
    RETURN_IF_NULL( aLogInData, KErrNoMemory );
    TPtr8 buffDes = aLogInData->Des();
    TIpcArgs messArgs;
    messArgs.Set( KMessageSlot0, & buffDes );
    TInt err( SendReceive( ESubSessGetLogInData, messArgs ) );
    if ( err > 0 )
        {
        delete aLogInData;
        aLogInData = NULL;
        aLogInData = HBufC8::New( err );
        RETURN_IF_NULL( aLogInData, KErrNoMemory );
        buffDes.Set( aLogInData->Des() );
        err = SendReceive( ESubSessGetLogInData, messArgs );
        }
    if ( err != KErrNone )
        {
        delete aLogInData;
        aLogInData = NULL;
        }
    return err;
    }

//  End of File
