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
* handler class for im client.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <imcvcodc.h>
#include "impsimcli.h"
#include "impsimhandler.h"
#include "impsfields.h"
#include "impsservercommon.h"
#include "impserrors.h"
#include "impsutils.h"
#include "impsdataaccessor.h"
#include "impscdatautils.h"
#include "impsliterals.h"  

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImpsImHandler2::CImpsImHandler2
// ----------------------------------------------------------------------------
CImpsImHandler2::CImpsImHandler2( 
    TInt aPriority, 
    RImpsImClient2& aClient ) :
    // The base class adds this to the scheduler
    CImpsHandler2( aPriority, aClient )
    {
    }

// ----------------------------------------------------------------------------
// CImpsImHandler2::NewL
// ----------------------------------------------------------------------------
CImpsImHandler2* CImpsImHandler2::NewL( 
     RImpsImClient2& aClient,
     TInt aPriority )
     {

      // Perform the construction.
      CImpsImHandler2* self = new (ELeave)
            CImpsImHandler2( aPriority, aClient);

      CleanupStack::PushL( self );
      self->ConstructL();
      CleanupStack::Pop( self );

      return self;
     }
       
// ----------------------------------------------------------------------------
// CImpsImHandler2::~CImpsImHandler2()
// ----------------------------------------------------------------------------
 CImpsImHandler2::~CImpsImHandler2()
     {

     }

// ----------------------------------------------------------------------------
// CImpsImHandler2::ConstructL
// ----------------------------------------------------------------------------
void CImpsImHandler2::ConstructL()
    {
    CImpsHandler2::ConstructL();
    }

// ----------------------------------------------------------------------------
// CImpsImHandler2::HandleEventL()
// ----------------------------------------------------------------------------

void CImpsImHandler2::HandleEventL( CImpsFields* aErrorFields )
    {
    // get the event data from server thread.
    SImpsEventData* event = (SImpsEventData*)iEventData.Ptr();

#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsImHandler2::HandleEventL opid=%d cli=%d"), 
         event->iOpCode, (TInt)&iClient );
#endif

    if ( event->iStatus != ( Imps_ERROR_BASE - 200 ) &&
             event->iStatus != KErrNone )
        {
        HandleErrorEventL( event->iOpCode, event->iStatus, aErrorFields );
        }
        // Call regular observer
        // iStatus is error code or message size
     else if ( iStatus >= 0 )
        {
        HandleImEventL( ); 
        }
    }

// ----------------------------------------------------------------------------
// CImpsImHandler2::HandleImEventL()
// ----------------------------------------------------------------------------
void CImpsImHandler2::HandleImEventL( )
    {
    SImpsEventData* event = (SImpsEventData*)iEventData.Ptr();
    RImpsImClient2& client = (RImpsImClient2&)iClient;
    MImpsImHandler2* imhandler = client.Handler();
    MImpsBlockingHandler2* blockhandler = client.BlockHandler();

    TInt err( KErrNone );
	
    // Check incoming message type
    TInt myType = event->iMessageType;

    CImpsDataAccessor* myAccess = CImpsDataAccessor::NewLC( iFields ); // << 
    CImpsKey* myKey = CImpsKey::NewLC();      // <<< myKey

    switch ( myType )
        {
        case EImpsNewMessage:
            {
            if ( !imhandler )
                {
                break;
                }
                
            // get messageID
            TPtrC msgId = TPtrC( KNullDesC );
            (void)TImpsCDataUtils::GetNewMessageIdL( 
                myKey, myAccess, msgId );

            myKey->Reset();

            // arrays
            // notice: optimize creation of arrays
            CPtrC16Array* users = new ( ELeave )CPtrC16Array( 4 );
            CleanupStack::PushL( users );   // <<< users
            CPtrC16Array* sns = new ( ELeave )CPtrC16Array( 4 );
            CleanupStack::PushL( sns );     //  <<< sns 
            CPtrC16Array* grs = new ( ELeave )CPtrC16Array( 4 );
            CleanupStack::PushL( grs );     // <<< grs 
            CPtrC16Array* sng = new (ELeave) CPtrC16Array(5);
            CleanupStack::PushL( sng );     // <<< grs 
            
            TImpsCDataUtils::AddValuesFromArrayL( 
                myKey, 
                KNewMessageElements, 
                sizeof( KNewMessageElements ) /
                sizeof( KNewMessageElements[0] ) );
            myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );
            TPtrC mySender;
            TPtrC myGroup;
            TImpsCDataUtils::GetSenderL( myKey, myAccess, 
                mySender, myGroup );
            TImpsCDataUtils::GetRecipientsL( myKey, myAccess, 
                grs, users, sng, sns );

            myKey->PopL();

            TPtrC myType;
            TImpsCDataUtils::GetContentTypeL( myAccess, myType );
            
            // !!!! Fix for JKIO-6PAGQ3 !!!!
            // Check whether the content type starts with "text/plain"
            if ( !myType.Left(KImpsContentType().Length()).CompareF( KImpsContentType ) || myType.Length()  == 0 )
                {
            TPtrC myContent;
            TImpsCDataUtils::GetContentDataL( myKey, myAccess, myContent );
            
            // Call API observer method
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsImHandler2::call HandleNewTextMessageL opid=%d"), event->iOpCode );
#endif
            TRAP( err, imhandler->HandleNewTextMessageL(
                event->iOpCode, 
                msgId,
                mySender,
                myGroup,  
                *users,
                *sns,
                myContent,
                *iClient.CspIdentifier() ));
                }
            else
                {
                TPtrC8 myContent;
                TInt inCleanup(0);
                TDesC8* content;
                myKey->Reset();
                TImpsCDataUtils::AddValuesFromArrayL( 
                    myKey, 
                    KNewMessageElements, 
                    sizeof( KNewMessageElements ) /
                    sizeof( KNewMessageElements[0] ) );

                myKey->AddL( CREATEKEY( EImpsKeyContentData, 0 ) );
                if ( myAccess->RestoreDesc8L( myKey, content ) )
                    {
                    TInt encoding (-1);
                    TImpsCDataUtils::GetContentEncodingL( myAccess, encoding);
                    if ( encoding == EImpsBASE64 )
                        {           
                        // allocate buffer for decoding
                        HBufC8* contentBuf = HBufC8::NewLC( content->Length() ); // << contentBuf
                        inCleanup++;
                        // BASE64 decoding
                        TImCodecB64 codec;
                        codec.Initialise();
                        TPtr8 ptr( contentBuf->Des() );
                        codec.Decode( *content , ptr );
                        myContent.Set(*contentBuf);
                        }
                    else
                        {
                        myContent.Set( *content );
                        }
                    }
                else
                    {
                    myContent.Set( KNullDesC8 );                                    
                    }
                    // Call API observer method
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsImHandler2::HandleNewContentMessageL opid=%d length=%d"), event->iOpCode, myContent.Length() );
                    
#endif
                    TRAP( err, imhandler->HandleNewContentMessageL(
                        event->iOpCode, 
                        msgId,
                        mySender,
                        myGroup,  
                        *users,
                        *sns,
                        myType,
                        myContent,
                        *iClient.CspIdentifier() ));
                    if ( inCleanup > 0 )
                        {
                        CleanupStack::PopAndDestroy( inCleanup ); //  >>> contentBuf
                        }
                    myKey->PopL();
                
                }              
            CleanupStack::PopAndDestroy(4); // >>> users, sns, grs, sng
            }
            break;

	case EImpsDeliveryReportReq:
	    {
            if ( !imhandler )
                {
                break;
                }
		TImpsCDataUtils::AddValuesFromArrayL( 
                myKey, 
                KTransContentElements, 
                sizeof( KTransContentElements ) /
                sizeof( KTransContentElements[0] ) );
            myKey->AddL( CREATEKEY( EImpsKeyDeliveryReport_Request, 0 ) );    
            myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );
            myKey->AddL( CREATEKEY( EImpsKeyMessageID, 0 ) );
            TDesC* msgId;
            myAccess->RestoreDescL( myKey, msgId );
            myKey->PopL( 2 );

            myKey->AddL( CREATEKEY( EImpsKeyResult, 0 ) );
            myKey->AddL( CREATEKEY( EImpsKeyCode, 0 ) );
            TInt result( 0 );
            myAccess->RestoreIntegerL( myKey, result );
            myKey->PopL();
            
            myKey->AddL( CREATEKEY( EImpsKeyDescription, 0 ) );
            TDesC* description;
            myAccess->RestoreDescL( myKey, description );
            
            TRAP( err, imhandler->HandleDeliveryReportL(
            	*msgId, result, description, *iClient.CspIdentifier() ) );
            
            // myKey->ResetL( );
            }
		break;		
		
        case EImpsGetBlockedListRes:
            if ( blockhandler )
                {

                TBool blockedInUse( EFalse );
                TBool grantedInUse( EFalse );

                CPtrC16Array* blockedList = new ( ELeave )CPtrC16Array( 4 );
                CleanupStack::PushL( blockedList  );   // <<< blockedList 

                CPtrC16Array* grantedList = new ( ELeave )CPtrC16Array( 4 );
                CleanupStack::PushL( grantedList  );   // <<< grantedList 

                // Get data from internal data format to API method
                TImpsCDataUtils::GetBlockedListResponseL(
                    myKey, myAccess,
                    blockedList,
                    blockedInUse,
                    grantedList,
                    grantedInUse );

#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsImHandler2::call GetBlockedListResponseL opid=%d"), event->iOpCode );
#endif
                // Call API observer method
                TRAP( err, blockhandler->GetBlockedListResponseL(
                    blockedList,
                    blockedInUse,
                    grantedList,
                    grantedInUse,
                    *client.CspIdentifier() ) );

                CleanupStack::PopAndDestroy(2); // >> blockedList, grantedlist

                }
            break;
                   

        default: // responses
            if ( event->iReqMesType == EImpsBlockEntityReq && blockhandler)
                {
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsImHandler2::call HandleCompleteL opid=%d"), event->iOpCode );
#endif
                TRAP( err, blockhandler->HandleCompleteL( event->iOpCode, /*event->iStatus,*/ *iClient.CspIdentifierL() ));
                }
            else if ( event->iReqMesType == EImpsSendMessageReq && imhandler)
                {
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsImHandler2::call HandleSendCompleteL opid=%d"), event->iOpCode );
#endif
                TRAP( err, imhandler->HandleSendCompleteL(
                    event->iOpCode, EFalse, *iClient.CspIdentifier())); 
                }
            else 
                {
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsImHandler2:: ERROR CORRUPTED ***** opid=%d"), event->iOpCode );
#endif
                __ASSERT_DEBUG( 0 == 1,
                                User::Panic( KImpsPanicCategory,
                                            EImpsCorrupted ) ); 
                User::Leave( KErrNotSupported );
                }
            break;
        }  // switch

        CleanupStack::PopAndDestroy(2); // myKey, myAccess

    }

//  End of File  
