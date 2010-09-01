/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
* commander for data accessor.
*
*/


// INCLUDE FILES
#include <e32std.h>

#include "ImpsAccessCli.h"
#include "ImpsAccessCommand.h"
#include "impsutils.h"
#include "impspacked.h"
#include "impsliterals.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImpsAccessCommand2::CImpsAccessCommand2
// ----------------------------------------------------------------------------
CImpsAccessCommand2::CImpsAccessCommand2( 
    TInt aPriority, 
    RImpsAccessClient2& aClient  ) :
    CActive( aPriority ), iClient( aClient )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CImpsAccessCommand2::NewL
// ----------------------------------------------------------------------------
CImpsAccessCommand2* CImpsAccessCommand2::NewL( RImpsAccessClient2& aClient )
     {

      // Perform the construction.
      CImpsAccessCommand2* self = new (ELeave)
            CImpsAccessCommand2( EPriorityUserInput, aClient);

      return self;
     }

// ----------------------------------------------------------------------------
// CImpsAccessCommand2::~CImpsAccessCommand2()
// ----------------------------------------------------------------------------
CImpsAccessCommand2::~CImpsAccessCommand2()
     {
     // Cancel any outstanding requests
     Cancel();

     delete iCookie;
     delete iMessageStream;
     }

// ----------------------------------------------------------------------------
// CImpsAccessCommand2::StartRunL()
// ----------------------------------------------------------------------------
void CImpsAccessCommand2::StartRunL( 
    TInt                aOpId,
    TImpsServRequest    aType )
    {
    iOpId = aOpId;
    iType = aType;
    iState = 0;

    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }


    // Set this active object active
    iStatus = KRequestPending;
    SetActive();
    }

// ----------------------------------------------------------------------------
// CImpsAccessCommand2::SetParametersL()
// ----------------------------------------------------------------------------
void CImpsAccessCommand2::SetParametersL( 
    const TDesC& aServer,
    const TDesC& aUser,
    const TDesC& aPsw,
    const TDesC& aClientId,
    TUint32 aAP,
    const TDesC* aKey1,
    const TDesC* aKey2 )
    {

    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }

    delete iCookie;
    iCookie = NULL;

    delete iMessageStream;
    iMessageStream = NULL;
    TInt myLen = aUser.Size() + aClientId.Size() + aPsw.Size() + 
                 aServer.Size();

    TPtrC key1 (KNullDesC);
    TPtrC key2 (KNullDesC);

    if ( aKey1 )
        {
        key1.Set( *aKey1 );
        }

    if ( aKey2 )
        {
        key2.Set( *aKey2 );
        }    
    
    myLen += key1.Size() + key2.Size();

    // some extra space for elements needed
    iMessageStream = HBufC8::NewL( myLen + 50 );
    *iMessageStream = KNullDesC8;

    CDesCArrayFlat* myTempArr = new ( ELeave )CDesCArrayFlat( 7 );
    CleanupStack::PushL( myTempArr );   // <<

    TImpsPackedEntity packedMessage( iMessageStream );

    myTempArr->AppendL( aUser );
    myTempArr->AppendL( aPsw );
    myTempArr->AppendL( aClientId );
    myTempArr->AppendL( aServer );
    myTempArr->AppendL( key1 );
    myTempArr->AppendL( key2 );
   
    // add aAp
    TBuf<4> temp;
    temp.Num( (TInt)aAP );
    myTempArr->AppendL( temp );

    const TUint8* pS = iMessageStream->Ptr();

    packedMessage.DoPackArray( pS, myTempArr );
    // update the length of the buffer
    iMessageStream->Des().SetLength( pS-iMessageStream->Ptr() );
    iMessagePtr.Set( iMessageStream->Des() );


    myTempArr->Reset();
   
    CleanupStack::PopAndDestroy( 1 );  // >> myTempArr
    }

// ----------------------------------------------------------------------------
// CImpsAccessCommand2::RunL()
// ----------------------------------------------------------------------------

void CImpsAccessCommand2::RunL()
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsAccessCommand2: RunL %d"), iStatus.Int() );
#endif

    MImpsAccessHandler2* handler = ( MImpsAccessHandler2* )iClient.iHandlerCallBack;
    MImpsErrorHandler2* ehandler = iClient.ErrorHandler();


    // Error handler should be registered
    if ( iStatus < KErrNone )
        {
        if ( ehandler )
            {
            TInt err( KErrNone );
            TRAP( err, ehandler->HandleErrorL( 
                iStatus.Int(),
                iOpId,
                NULL,
                NULL,
                *iClient.CspIdentifier() ));                 
            }
        return;
        }

    // Errors are handled here only except couple of nice cases
        if ( iType == EImpsServIsLogged )
        {
                return;
            }  
        if ( iType == EImpsServNbrSessions )
            {
            // Status includes the required data!
            // No "push" later
            TInt err( KErrNone );
            TRAP( err, handler->HandleNbrSessionsL(
                    iOpId,
                    iStatus.Int(),
                    *iClient.CspIdentifier() ));                 
                return;
        }
    }

// ----------------------------------------------------------------------------
// CImpsAccessCommand2::DoCancel()
// Before calling this the Cancel should have sent to the engine! 
// ----------------------------------------------------------------------------

void CImpsAccessCommand2::DoCancel()
    {
    }

//  End of File  

