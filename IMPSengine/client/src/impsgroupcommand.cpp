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
* imps group commander.
*
*/


// INCLUDE FILES
#include <e32std.h>

#include "ImpsGroupCli.h"
#include "ImpsGroupCommand.h"
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
// CImpsGroupCommand2::CImpsGroupCommand2
// ----------------------------------------------------------------------------
CImpsGroupCommand2::CImpsGroupCommand2( 
    TInt aPriority, 
    RImpsGroupClient2& aClient  ) :
    CActive( aPriority ), 
    iClient( aClient )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::NewL
// ----------------------------------------------------------------------------
CImpsGroupCommand2* CImpsGroupCommand2::NewL( RImpsGroupClient2& aClient )
    {
    // Perform the construction.
    CImpsGroupCommand2* self = new (ELeave)
    CImpsGroupCommand2( EPriorityUserInput, aClient);
    return self;
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::~CImpsGroupCommand2
// ----------------------------------------------------------------------------
CImpsGroupCommand2::~CImpsGroupCommand2()
     {
     // Cancel any outstanding requests
     Cancel();
     ResetMembers();
     }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::StartRunL
// ----------------------------------------------------------------------------
void CImpsGroupCommand2::StartRunL( 
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
// CImpsGroupCommand2::RunL
// ----------------------------------------------------------------------------

void CImpsGroupCommand2::RunL()
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsGroupCommand2: RunL %d"), iStatus.Int() );
#endif

   __ASSERT_DEBUG( iType == EImpsServGroup, 
                   User::Panic( KImpsPanicCategory,
                                EImpsCorrupted ) );

   // Errors are handled here only. 
   // Successful cases are handled by CImpsImHandler.
   if ( iStatus == KErrNone )
       {
       return;
       } 

    MImpsErrorHandler2* ehandler = iClient.ErrorHandler();

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
// CImpsGroupCommand2::DoCancel
// Before calling this the unregister should have been called
// ----------------------------------------------------------------------------

void CImpsGroupCommand2::DoCancel()
    {
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::ResetMembers
// ----------------------------------------------------------------------------
void CImpsGroupCommand2::ResetMembers( )
    {
    delete iImpsFields;
    iImpsFields = NULL;
    if( iKey )
        iKey->Destroy( );
    iKey = NULL;
    delete iAccessor;
    iAccessor = NULL;
    delete iMessage;
    iMessage = NULL;
    iMessagePtr.Set( KNullDesC8 );
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::InitialiseL
// ----------------------------------------------------------------------------
void CImpsGroupCommand2::InitialiseL(const TImpsContent* aElementArray,
                                    const TUint aArraySize )
    {
    iImpsFields = CImpsFields::NewL();
    iAccessor = CImpsDataAccessor::NewL( iImpsFields );
    iKey = iAccessor->NewKeyL();
    for( TUint i = 0;i < aArraySize;i++ )
        iKey->AddL( CREATEKEY( aElementArray[i], 0 ) );
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::InsertElementL
// ----------------------------------------------------------------------------
void CImpsGroupCommand2::InsertElementL( const TDesC& aElementValue,
                     const TImpsContent aElementName )
    {
    iKey->AddL( CREATEKEY( aElementName, 0 ) );
    iAccessor->StoreDescL( iKey, aElementValue );
    iKey->PopL( );
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::InsertElementL
// ----------------------------------------------------------------------------
void CImpsGroupCommand2::InsertElementL( TBool aBool,
                     const TImpsContent aElementName )
    {
    iKey->AddL( CREATEKEY( aElementName, 0 ) );
    iAccessor->StoreBooleanL( iKey, aBool );
    iKey->PopL( );
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::InsertElementIntL
// ----------------------------------------------------------------------------
void CImpsGroupCommand2::InsertElementIntL( TInt aInt,
                     const TImpsContent aElementName )
    {
    iKey->AddL( CREATEKEY( aElementName, 0 ) );
    iAccessor->StoreIntegerL( iKey, aInt );
    iKey->PopL( );
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::InsertElementL
// ----------------------------------------------------------------------------
void CImpsGroupCommand2::InsertElementL( const TDesC& aElementValue,
                     const TImpsContent aElementName,
                     TInt aIndex )
    {
    iKey->AddL( CREATEKEY( aElementName, aIndex ) );
    iAccessor->StoreDescL( iKey, aElementValue );
    iKey->PopL( );
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::InsertEmptyL
// ----------------------------------------------------------------------------
void CImpsGroupCommand2::InsertEmptyL( const TImpsContent aElementName )
    {
    iKey->AddL( CREATEKEY( aElementName, 0 ) );
    iAccessor->StoreEmptyL( iKey );
    iKey->PopL( );
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::PackMessageL
// ----------------------------------------------------------------------------
void CImpsGroupCommand2::PackMessageL( )
    {
    iImpsFields->SetMessageType( iMessageType );
    delete iMessage;
    iMessage = NULL;
    iMessagePtr.Set( KNullDesC8 );
    iMessage = HBufC8::NewL( iImpsFields->Size() );
    TImpsPackedEntity packedMessage( iMessage );
    TInt error = packedMessage.PackEntity( *iImpsFields );
    User::LeaveIfError( error );
    }

// ----------------------------------------------------------------------------
// CImpsGroupCommand2::PackAndSendL
// ----------------------------------------------------------------------------
void CImpsGroupCommand2::PackAndSendL( TImpsMessageType aMsgType, TInt aOpId )
    {
    SetMessageType( aMsgType );
    PackMessageL( );
    iMessagePtr.Set( iMessage->Des() );
    TImpsServRequest req = EImpsServGroup;
    StartRunL( aOpId, EImpsServGroup );
    iClient.SendReceive( req, TIpcArgs( &iMessagePtr, aOpId ), iStatus );
    }

//  End of File  
