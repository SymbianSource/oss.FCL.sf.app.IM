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
* handler for data accessor.
*
*/

// INCLUDE FILES
#include <e32std.h>

#include "ImpsAccessCli.h"
#include "ImpsAccessHandler.h"
#include "ImpsUtils.h"
#include "impserrors.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImpsAccessHandler2::CImpsAccessHandler2
// ----------------------------------------------------------------------------
CImpsAccessHandler2::CImpsAccessHandler2( 
    TInt aPriority, 
    RImpsAccessClient2& aClient  ) :
    // The base class adds this to the scheduler
    CImpsHandler2( aPriority, aClient)
    {
    }

// ----------------------------------------------------------------------------
// CImpsAccessHandler2::NewL
// ----------------------------------------------------------------------------
CImpsAccessHandler2* CImpsAccessHandler2::NewL( 
     RImpsAccessClient2& aClient,
     TInt aPriority )
     {

      // Perform the construction.
      CImpsAccessHandler2* self = new (ELeave)
            CImpsAccessHandler2( aPriority , aClient);

      CleanupStack::PushL( self );
      self->ConstructL();
      CleanupStack::Pop( self );

      return self;
     }

// ----------------------------------------------------------------------------
// CImpsAccessHandler2::~CImpsAccessHandler2()
// ----------------------------------------------------------------------------
CImpsAccessHandler2::~CImpsAccessHandler2()
     {

     }

// ----------------------------------------------------------------------------
// CImpsAccessHandler2::HandleEventL()
// ----------------------------------------------------------------------------

void CImpsAccessHandler2::HandleEventL( CImpsFields* aErrorFields )
    {
    // get the event data from server thread.
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsAccessHandler2::HandleEventL begins"));
#endif

    SImpsEventData* event = (SImpsEventData*)iEventData.Ptr();

    // SAP initiated Disconnect is another special case 
    // First is SAP initiated disconnect response
    if ( event->iReqMesType == EImpsMessageNone &&  
              event->iMessageType == EImpsDisconnect )
        {
        HandleAccessEventL( ); 
        }
    else
        {
        if ( event->iStatus != ( Imps_ERROR_BASE - 200 ) &&
                 event->iStatus != KErrNone )
            {
            HandleErrorEventL( event->iOpCode, event->iStatus, aErrorFields );
            }
            // Call regular observer
            // iStatus is error code or message size
         else if ( iStatus >= 0 )
            {
            HandleAccessEventL( ); 
            }
        }
    }

// ----------------------------------------------------------------------------
// CImpsAccessHandler2::HandleAccessEventL()
// ----------------------------------------------------------------------------
void CImpsAccessHandler2::HandleAccessEventL( )
    {
    SImpsEventData* event = (SImpsEventData*)iEventData.Ptr();
    // RImpsAccessClient2& client = (RImpsAccessClient2&)iClient;
    MImpsAccessHandler2* handler = ((RImpsAccessClient2&)iClient).Handler();

	TInt err( KErrNone );
	
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsAccessHandler2::HandleAccessEvent BEgins"));
#endif

	// special case, login is cancelled
	if ( ((RImpsAccessClient2&)iClient).LoginCancelled() )
	{
		// check that operation id is the same 
		// that was used when logged in
		if ( ((RImpsAccessClient2&)iClient).LoginOpId() == event->iOpCode )	
		{
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsAccessHandler2::HandleLoginCancelL call opid=%d sta=%d"), 
         event->iOpCode, event->iStatus);
#endif

            TRAP( err, handler->HandleLoginCancelL(
                event->iOpCode,
                *iClient.CspIdentifier() ));                 
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsAccessHandler2::HandleLoginCancelL call OK"));
#endif	
		// reset the variables
		((RImpsAccessClient2&)iClient).CancelHandled();
		return;
		}
	}
	
	// normal case,
    // call application's event handler
    TInt myType = event->iRequestType;
    switch ( myType )
        {

        case EImpsServWVLogin:
            {
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsAccessHandler2::HandleLoginL call opid=%d sta=%d"), 
         event->iOpCode, event->iStatus);
#endif

            TRAP( err, handler->HandleLoginL(
                event->iOpCode,
                *iClient.CspIdentifier() ));                 
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsAccessHandler2::HandleLoginL call OK"));
#endif
            break;
            }

        case EImpsServWVLogout:
            {
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsAccessHandler2::HandleLogoutL call opid=%d sta=%d"), 
         event->iOpCode, event->iStatus);
#endif
            TRAP( err, handler->HandleLogoutL(
                event->iOpCode,
                *iClient.CspIdentifier() ));   
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsAccessHandler2::HandleLogoutL call OK"));
#endif
            break;
            }

        default:
            break;
        }
    }
//  End of File  
