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
*       WV engine pure data command
*
*
*/


// INCLUDE FILES
#include <e32std.h>

#include "impspurecli.h"
#include "impspurecommand.h"
#include "ImpsFields.h"
#include "ImpsPacked.h"
#include "impsutils.h"
#include "ImpsDataAccessor.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImpsPureCommand2::CImpsPureCommand2
// ----------------------------------------------------------------------------
CImpsPureCommand2::CImpsPureCommand2( 
    TInt aPriority, 
    RImpsPureClient2& aClient  ) :
    CActive( aPriority ), iClient( aClient ),
        iMessagePtr()
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CImpsPureCommand2::NewL
// ----------------------------------------------------------------------------
CImpsPureCommand2* CImpsPureCommand2::NewL( RImpsPureClient2& aClient )
     {

      // Perform the construction.
      CImpsPureCommand2* self = new (ELeave) CImpsPureCommand2( 
                                                   EPriorityUserInput, aClient);

      return self;
     }

// ----------------------------------------------------------------------------
// CImpsPureCommand2::~CImpsPureCommand2
// ----------------------------------------------------------------------------
CImpsPureCommand2::~CImpsPureCommand2()
     {
     // Cancel any outstanding requests
     Cancel();

     ResetMembers();
     }

// ----------------------------------------------------------------------------
// CImpsPureCommand2::StartRunL
// ----------------------------------------------------------------------------
void CImpsPureCommand2::StartRunL( 
    TInt                aOpId,
    TImpsServRequest    aType )
    {
    iOpId = aOpId;
    iType = aType;

    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }

    // Set this active object active
    iStatus = KRequestPending;
    SetActive();
    }

// ----------------------------------------------------------------------------
// CImpsPureCommand2::RunL
// ----------------------------------------------------------------------------

void CImpsPureCommand2::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsPureCommand2: RunL stat=%d h=%d cli=%d"), 
        iStatus.Int(), (TInt)this, (TInt)&iClient );
#endif

    MImpsErrorHandler2* ehandler = iClient.ErrorHandler();
    // Successful cases are handled by CImpsImHandler.
    if ( iStatus == KErrNone )
        {
        return;
        }

    // Handler should be registered
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
    }

// ----------------------------------------------------------------------------
// CImpsPureCommand2::DoCancel
// Before calling this the Cancel should have sent to the engine! 
// ----------------------------------------------------------------------------
void CImpsPureCommand2::DoCancel()
    {
    }

// ----------------------------------------------------------------------------
// CImpsPureCommand2::ResetMembers
// ----------------------------------------------------------------------------
void CImpsPureCommand2::ResetMembers( )
    {
    delete iImpsFields;
    iImpsFields = NULL;
    if ( iKey )
        iKey->Destroy( );
    iKey = NULL;
    delete iAccessor;
    iAccessor = NULL;
    delete iMessage;
    iMessage = NULL;
    iMessagePtr.Set ( KNullDesC8 );
    }

// ----------------------------------------------------------------------------
// CImpsPureCommand2::InitialiseL
// ----------------------------------------------------------------------------
void CImpsPureCommand2::InitialiseL( const TImpsContent* aElementArray,
                                    const TUint aArraySize )
    {
    iImpsFields = CImpsFields::NewL();
    iAccessor = CImpsDataAccessor::NewL( iImpsFields );
    iKey = iAccessor->NewKeyL();
    for( TUint i = 0;i < aArraySize;i++ )
        iKey->AddL( CREATEKEY( aElementArray[i], 0 ) );
    }

// ----------------------------------------------------------------------------
// CImpsPureCommand2::InsertElementL
// ----------------------------------------------------------------------------
void CImpsPureCommand2::InsertElementL( const TDesC8& aElementValue,
                     const TImpsContent aElementName )
    {
    iKey->AddL( CREATEKEY( aElementName, 0 ) );
    iAccessor->StoreDesc8L( iKey, aElementValue );
    iKey->PopL( );
    }


// ----------------------------------------------------------------------------
// CImpsPureCommand2::SetMessageType
// ----------------------------------------------------------------------------
void CImpsPureCommand2::SetMessageType( TImpsMessageType aMsgType )
    {
    iMessageType = aMsgType;
    }

// ----------------------------------------------------------------------------
// CImpsPureCommand2::PackMessageL
// ----------------------------------------------------------------------------
TInt CImpsPureCommand2::PackMessageL( )
    {
    iImpsFields->SetMessageType( iMessageType );
    iMessage = HBufC8::NewL( iImpsFields->Size() );
    TImpsPackedEntity packedMessage( iMessage );
    TInt error = packedMessage.PackEntity( *iImpsFields );
    return error;
    }

// ----------------------------------------------------------------------------
// CImpsPureCommand2::PackAndSendL
// ----------------------------------------------------------------------------
TInt CImpsPureCommand2::PackAndSendL( TImpsMessageType aMsgType, TInt aOpId )
    {
    SetMessageType( aMsgType );
    TInt error = PackMessageL( );
    if( error == KErrNone )
        {
        // p[ 0 ] = ( TAny* )( iMessage );
        // p[ 1 ] = ( TAny* )( aOpId );
        iMessagePtr.Set( iMessage->Des() );
        TImpsServRequest req = EImpsServPres;
        error = iClient.SendReceive( req, TIpcArgs( &iMessagePtr, aOpId ) );
        }
   	return error;
    }
//  End of File  

