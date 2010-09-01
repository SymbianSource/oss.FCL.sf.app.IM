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
* commander class for im.
*
*/


// INCLUDE FILES
#include <e32std.h>

#include "impsimcli.h"
#include "impsimcommand.h"
#include "impsfields.h"
#include "impspacked.h"
#include "impsutils.h"
#include "impsdataaccessor.h"
#include "impscdatautils.h"
#include "impsdetailed.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImpsImCommand2::CImpsImCommand2
// ----------------------------------------------------------------------------
CImpsImCommand2::CImpsImCommand2( 
    TInt aPriority, 
    RImpsImClient2& aClient  ) :
    CActive( aPriority ), iClient( aClient )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CImpsImCommand2::NewL
// ----------------------------------------------------------------------------
CImpsImCommand2* CImpsImCommand2::NewL( RImpsImClient2& aClient )
     {

      // Perform the construction.
      CImpsImCommand2* self = new (ELeave)
            CImpsImCommand2( EPriorityUserInput, aClient);
      CleanupStack::PushL( self );
      self->ConstructL();
      CleanupStack::Pop(1);     // self

      return self;
     }

// ----------------------------------------------------------------------------
// CImpsImCommand2::ConstructL
// ----------------------------------------------------------------------------
void CImpsImCommand2::ConstructL(  )
     {
     iDetailed = new (ELeave) CImpsDetailed(1);
     }

// ----------------------------------------------------------------------------
// CImpsImCommand2::~CImpsImCommand2
// ----------------------------------------------------------------------------
CImpsImCommand2::~CImpsImCommand2()
     {
     // Cancel any outstanding requests
     Cancel();
     delete iMessageStream;
     delete iDetailed;
     }

// ----------------------------------------------------------------------------
// CImpsImCommand2::StartRunL
// ----------------------------------------------------------------------------
void CImpsImCommand2::StartRunL( 
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
// CImpsImCommand2::SetParametersL
// version for newtextmessage
// ----------------------------------------------------------------------------
void CImpsImCommand2::SetParametersL( 
            const TDesC* aSenderSn,
            const MDesCArray* aUserIds,
            const TDesC* aGroupId,         
            const MDesCArray* aScreenNames, 
            const TDesC& aContent,
            TBool aDeliveryReportWanted )
    {

    // Generate one flat message presentation
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }

    if ( !iMessageStream )
        {
        iMessageStream = HBufC8::NewL( KImpsStreamSize );
        }
    *iMessageStream = KNullDesC8;

    // Initialize internal structure
    CImpsFields* fields = CImpsFields::NewL();
    CleanupStack::PushL( fields );          // <<< fields
    fields->SetMessageType( EImpsSendMessageReq );

    CImpsDataAccessor* myAccess = CImpsDataAccessor::NewL( fields );
    CleanupStack::PushL( myAccess );            // <<< myAccess
    CImpsKey* myKey = CImpsKey::NewLC();        // <<< myKey

    CPtrC16Array* grs = new (ELeave) CPtrC16Array(4);
    CleanupStack::PushL( grs );     // <<< grs

    CPtrC16Array* grns = new (ELeave) CPtrC16Array(4);
    CleanupStack::PushL( grns );     // <<< grns

    TImpsCDataUtils::AddValuesFromArrayL( 
        myKey, 
        KSendMessageElements, 
        sizeof( KSendMessageElements ) /
        sizeof( KSendMessageElements[0] ) );
    
    // add delivery report    
    myKey->AddL( CREATEKEY( EImpsKeyDeliveryReport, 0 ) );
    myAccess->StoreBooleanL( myKey, aDeliveryReportWanted );
    myKey->PopL();
    
    if ( aScreenNames )
        {
        TInt count = aScreenNames->MdcaCount();
        for ( TInt i = 0; i < count ; i++ )
            {
            grns->AppendL( *aGroupId );
            }
         // GroupID is used as Recipient( Group ( GroupID ) ) also
         // if aScreenNames is empty
        if ( count == 0 && aGroupId )
	        {
	        grs->AppendL( *aGroupId );	
	        }
        }
    // GroupID is used as Recipient( Group ( GroupID ) ) if
    // aScreenNames is omitted.
    else if ( aGroupId )
        {
        grs->AppendL( *aGroupId );
        }

    myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );

    TImpsCDataUtils::SetRecipientsL(
            myKey,
            myAccess,
            grs,
            aUserIds,  
            grns,         // ScreenName( GroupId )
            aScreenNames );    // ScreenName( SName ); 

    TImpsCDataUtils::SetSenderL(
            myKey,
            myAccess,
            aSenderSn,
            NULL );

    myKey->PopL();

    // notice: MessageInfo:ContentType is default now
    // Add actual ContentData
    TImpsCDataUtils::SetContentDataL(
        myKey, myAccess, aContent );
    // Set sender 
    // This is a "fuzzy" logic. 
    // aSenderSn + aGroupId -> Sender ( ScreenName( SName, GroupID ) )
    // !aSenderSn + !aGroupId -> Sender ( User ( UserID )) added by engine!
    // aSenderSn + !aGroupId -> Sender ( User ( UserID )) 
    // !aSenderSn + aGroupId -> Sender ( Group ( GroupID )) 

    CleanupStack::PopAndDestroy(4);     // >>> grns, grs, myKey, myAccess

    // pack, or actullay serialize the message data
    TImpsPackedEntity packedMessage( iMessageStream );
    TInt myError = packedMessage.PackEntity( *fields );

    // Allocate more memory if needed
    if ( ( myError != KErrNone ) && 
         ( iMessageStream->Des().Size() < fields->Size() ) )
        {
        // notice: consider how to shrink buffer later 
        // increase the size of the buffer and try again
        iMessageStream->Des().SetLength(0);    // to avoid copying contents
        iMessageStream = iMessageStream->ReAllocL( fields->Size() + 10 );
        myError = packedMessage.PackEntity( *fields );
        User::LeaveIfError( myError );
        }
    
    CleanupStack::PopAndDestroy( 1 );  // fields

    }

// ----------------------------------------------------------------------------
// CImpsImCommand2::SetParametersL
// version for newcontentmessage
// ----------------------------------------------------------------------------
void CImpsImCommand2::SetParametersL( 
            const TDesC* aSenderSn,
            const MDesCArray* aUserIds,
            const TDesC* aGroupId,         
            const MDesCArray* aScreenNames, 
            const TDesC& aContentType,
            const TDesC8& aContent,
            TBool aDeliveryReportWanted )
    {
    
    // Generate one flat message presentation
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }

    if ( !iMessageStream )
        {
        iMessageStream = HBufC8::NewL( KImpsStreamSize );
        }
    *iMessageStream = KNullDesC8;

    // Initialize internal structure
    CImpsFields* fields = CImpsFields::NewL();
    CleanupStack::PushL( fields );          // <<< fields
    fields->SetMessageType( EImpsSendMessageReq );

    CImpsDataAccessor* myAccess = CImpsDataAccessor::NewL( fields );
    CleanupStack::PushL( myAccess );            // <<< myAccess
    CImpsKey* myKey = CImpsKey::NewLC();        // <<< myKey

    CPtrC16Array* grs = new (ELeave) CPtrC16Array(4);
    CleanupStack::PushL( grs );     // <<< grs

    CPtrC16Array* grns = new (ELeave) CPtrC16Array(4);
    CleanupStack::PushL( grns );     // <<< grns

    TImpsCDataUtils::AddValuesFromArrayL( 
        myKey, 
        KSendMessageElements, 
        sizeof( KSendMessageElements ) /
        sizeof( KSendMessageElements[0] ) );
    
    // add delivery report    
    myKey->AddL( CREATEKEY( EImpsKeyDeliveryReport, 0 ) );
    myAccess->StoreBooleanL( myKey, aDeliveryReportWanted );
    myKey->PopL();
    
    if ( aScreenNames )
        {
        TInt count = aScreenNames->MdcaCount();
        for ( TInt i = 0; i < count ; i++ )
            {
            grns->AppendL( *aGroupId );
            }
        // GroupID is used as Recipient( Group ( GroupID ) ) also
        // if aScreenNames is empty
        if ( count == 0 && aGroupId )
	        {
	        grs->AppendL( *aGroupId );	
	        }
        }
    // GroupID is used as Recipient( Group ( GroupID ) ) only if
    // aScreenNames is omitted.
    else if ( aGroupId )
        {
        grs->AppendL( *aGroupId );
        }

    myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );

    TImpsCDataUtils::SetRecipientsL(
            myKey,
            myAccess,
            grs,
            aUserIds,  
            grns,         // ScreenName( GroupId )
            aScreenNames );    // ScreenName( SName ); 

    TImpsCDataUtils::SetSenderL(
            myKey,
            myAccess,
            aSenderSn,
            NULL );

    TPtrC myContentType( aContentType );
    TImpsCDataUtils::SetContentTypeL( myAccess, myContentType );

    TImpsCDataUtils::SetBinaryContentDataL(
        myKey, myAccess, aContent );  
    // Set sender 
    // This is a "fuzzy" logic. 
    // aSenderSn + aGroupId -> Sender ( ScreenName( SName, GroupID ) )
    // !aSenderSn + !aGroupId -> Sender ( User ( UserID )) added by engine!
    // aSenderSn + !aGroupId -> Sender ( User ( UserID )) 
    // !aSenderSn + aGroupId -> Sender ( Group ( GroupID )) 

    CleanupStack::PopAndDestroy(4);     // >>> grns, grs, myKey, myAccess

    // pack, or actullay serialize the message data
    TImpsPackedEntity packedMessage( iMessageStream );
    TInt myError = packedMessage.PackEntity( *fields );

    // Allocate more memory if needed
    if ( ( myError != KErrNone ) && 
         ( iMessageStream->Des().Size() < fields->Size() ) )
        {
        // notice: consider how to shrink buffer later 
        // increase the size of the buffer and try again
        iMessageStream->Des().SetLength(0);    // to avoid copying contents
        iMessageStream = iMessageStream->ReAllocL( fields->Size() + 10 );
        myError = packedMessage.PackEntity( *fields );
        User::LeaveIfError( myError );
        }
    
    CleanupStack::PopAndDestroy( 1 );  // fields

    }
    
// ----------------------------------------------------------------------------
// CImpsImCommand2::SetParametersL
// ----------------------------------------------------------------------------
void CImpsImCommand2::SetParametersL( 
            const MDesCArray* aBlockEntity,
            const MDesCArray* aUnBlockEntity,
            TBool aBlockedListInUse,
            const MDesCArray* aGrantedEntityList,
            const MDesCArray* aUnGrantEntity,
            TBool aGrantedListInUse )
    {

    // Generate one flat message presentation

    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }

    if ( !iMessageStream )
        {
        iMessageStream = HBufC8::NewL( KImpsStreamSize );
        }
    *iMessageStream = KNullDesC8;

    // Initialize internal structure
    CImpsFields* fields = CImpsFields::NewL();
    CleanupStack::PushL( fields );          // <<< fields
    fields->SetMessageType( EImpsBlockEntityReq );

    CImpsDataAccessor* myAccess = CImpsDataAccessor::NewL( fields );
    CleanupStack::PushL( myAccess );            // <<< myAccess
    CImpsKey* myKey = CImpsKey::NewLC();        // <<< myKey

    TImpsCDataUtils::SetBlockedListRequestL(
        myKey,
        myAccess,
        aBlockEntity,
        aUnBlockEntity,
        aBlockedListInUse,
        aGrantedEntityList,
        aUnGrantEntity,
        aGrantedListInUse );

    CleanupStack::PopAndDestroy(2);     // >>> myKey, myAccess

    // pack, or actullay serialize the message data
    TImpsPackedEntity packedMessage( iMessageStream );
    TInt myError = packedMessage.PackEntity( *fields );

    // Allocate more memory if needed
    if ( ( myError != KErrNone ) && 
         ( iMessageStream->Des().Size() < fields->Size() ) )
        {
        // notice: consider how to shrink buffer later 
        // increase the size of the buffer and try again
        iMessageStream->Des().SetLength(0);    // to avoid copying contents
        iMessageStream = iMessageStream->ReAllocL( fields->Size() + 10 );
        myError = packedMessage.PackEntity( *fields );
        User::LeaveIfError( myError );
        }

    CleanupStack::PopAndDestroy( 1 );  // fields

    }

// ----------------------------------------------------------------------------
// CImpsImCommand2::RunL
// ----------------------------------------------------------------------------

void CImpsImCommand2::RunL()
    {

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsImCommand2: RunL stat=%d h=%d cli=%d"), 
        iStatus.Int(), (TInt)this, (TInt)&iClient );
#endif

    MImpsErrorHandler2* ehandler = iClient.ErrorHandler();

    // Errors are handled here only. 
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
// CImpsImCommand2::DoCancel
// Before calling this the Cancel should have sent to the engine! 
// ----------------------------------------------------------------------------

void CImpsImCommand2::DoCancel()
    {
    }

//  End of File  

