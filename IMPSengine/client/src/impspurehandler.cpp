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
*       WV engine pure data API 
*
*
*/


// INCLUDE FILES
// INCLUDE FILES
#include <e32std.h>

#include "impspurecli.h"
#include "impspurehandler.h"
#include "impserrors.h"
#include "impsdataaccessor.h"
#include "impscdatautils.h"
#include "impsutils.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImpsPureHandler2::CImpsPureHandler2
// ----------------------------------------------------------------------------
CImpsPureHandler2::CImpsPureHandler2( 
    TInt aPriority, 
    RImpsPureClient2& aClient  ) :
    // The base class adds this to the scheduler
    CImpsHandler2( aPriority, aClient)
    {
    }

// ----------------------------------------------------------------------------
// CImpsPureHandler2::NewL
// ----------------------------------------------------------------------------
CImpsPureHandler2* CImpsPureHandler2::NewL( 
     RImpsPureClient2& aClient,
     TInt aPriority )
     {

      // Perform the construction.
      CImpsPureHandler2* self = new (ELeave)
            CImpsPureHandler2( aPriority, aClient);

      CleanupStack::PushL( self );
      self->CImpsHandler2::ConstructL();
      CleanupStack::Pop( self );
      return self;
     }

// ----------------------------------------------------------------------------
// CImpsPureHandler2::~CImpsPureHandler2()
// ----------------------------------------------------------------------------
CImpsPureHandler2::~CImpsPureHandler2()
     {
     }

// ----------------------------------------------------------------------------
// CImpsPureHandler2::HandleEventL()
// ----------------------------------------------------------------------------
void CImpsPureHandler2::HandleEventL( CImpsFields* aErrorFields )
    {
    // get the event data from server thread.
    SImpsEventData* event = (SImpsEventData*)iEventData.Ptr();
    TImpsMessageType msgType = (TImpsMessageType)event->iMessageType;

#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsPureHandler::HandleEventL opid=%d cli=%d"), 
         event->iOpCode, (TInt)&iClient );
#endif
    if ( msgType != EImpsPureData )
        {
        HandleErrorEventL( event->iOpCode, event->iStatus, aErrorFields );
        }
      // Call regular observer
      // iStatus is error code or message size
    else if ( iStatus >= 0 )
        {
        HandlePureEventL( ); 
        }
    }

// ----------------------------------------------------------------------------
// CImpsPureHandler2::HandlePureEventL()
// ----------------------------------------------------------------------------
void CImpsPureHandler2::HandlePureEventL( )
    {
    SImpsEventData* event = (SImpsEventData*)iEventData.Ptr();
    RImpsPureClient2& client = (RImpsPureClient2&)iClient;
    MImpsPureHandler2* handler = client.Handler();

    // Check request message type
    TInt myType = event->iMessageType;

    CImpsDataAccessor* myAccess = CImpsDataAccessor::NewLC( iFields ); // << 
    CImpsKey* myKey = CImpsKey::NewLC();      // <<< myKey

    switch ( myType )
        {
        case EImpsStatus:
        case EImpsPureData:
            {
            if ( !handler )
                {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsPureHandler2:HandlePureEventL: NO HANDLER opid=%d cli=%d h=%d"),
        event->iOpCode, (TInt)&iClient, (TInt)this );
#endif
                break;
                }
            TPtrC8 pureData;
            pureData.Set( KNullDesC8 );

            TImpsCDataUtils::AddValuesFromArrayL( 
                myKey, 
                KTransContentElements, 
                sizeof( KTransContentElements ) /
                sizeof( KTransContentElements[0] ) );
            myKey->AddL( CREATEKEY( EImpsKeyPureData, 0 ) );
            TDesC8* temp = NULL;
            if( myAccess->RestoreDesc8L( myKey, temp ) )
                pureData.Set( *temp );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("CImpsPureHandler2:HandlePureEventL: calls HandlePureDataL opid=%d cli=%d h=%d"),
        event->iOpCode, (TInt)&iClient, (TInt)this );
#endif

            // Call API observer method
            TInt err( KErrNone );
            TRAP( err, handler->HandlePureDataL( event->iOpCode, pureData, *iClient.CspIdentifier() ));
            }
            break;
        default: // responses
            __ASSERT_DEBUG( 0 == 1,
                            User::Panic( KImpsPanicCategory,
                                        EImpsCorrupted ) ); 
            User::Leave( KErrNotSupported );
            break;
        }  // switch
    CleanupStack::PopAndDestroy(2); // myKey, myAccess
    }

//  End of File  

