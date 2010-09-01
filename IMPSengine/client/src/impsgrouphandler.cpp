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
* handler class for imps group feature.
*
*/


// INCLUDE FILES
#include <e32std.h>

#include "impsgroupcli.h"
#include "impsgrouphandler.h"
#include "impsgrouppropsint.h"
#include "impsfields.h"
#include "impserrors.h"
#include "impsservercommon.h"
#include "impsdataaccessor.h"
#include "impscdatautils.h"
#include "impsutils.h"
#include "impserrors.h"
#include "WVEngineInternalCRKeys.h"

// CONSTANTS
const TInt KArraySize = 4;

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImpsGroupHandler2::CImpsGroupHandler2
// ----------------------------------------------------------------------------
CImpsGroupHandler2::CImpsGroupHandler2( 
    TInt aPriority, 
    RImpsGroupClient2& aClient  ) :
    // The base class adds this to the scheduler
    CImpsHandler2( aPriority, aClient )
    {
    }

// ----------------------------------------------------------------------------
// CImpsGroupHandler2::NewL
// ----------------------------------------------------------------------------
CImpsGroupHandler2* CImpsGroupHandler2::NewL( 
     RImpsGroupClient2& aClient,
     TInt aPriority )
     {

      // Perform the construction.
      CImpsGroupHandler2* self = new (ELeave)
            CImpsGroupHandler2( aPriority, aClient);

      CleanupStack::PushL( self );
      self->CImpsHandler2::ConstructL();
      CleanupStack::Pop( self );

      return self;
     }
       
// ----------------------------------------------------------------------------
// CImpsGroupHandler2::~CImpsGroupHandler2()
// ----------------------------------------------------------------------------
 CImpsGroupHandler2::~CImpsGroupHandler2()
     {
     }


// ----------------------------------------------------------------------------
// CImpsGroupHandler2::HandleEventL()
// ----------------------------------------------------------------------------
void CImpsGroupHandler2::HandleEventL( CImpsFields* aErrorFields )
    {
    SImpsEventData* event = (SImpsEventData*)iEventData.Ptr(); 

#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2::HandleEventL opid=%d cli=%d"), 
         event->iOpCode, (TInt)&iClient );
#endif

    // LeaveGroupResponse is a special case because of it can contain
    // reasonable data although having status code unequal 200.
    // First one is SAP initiated leavegroup response
    if ( event->iReqMesType == EImpsMessageNone &&  
         event->iMessageType == EImpsLeaveGroupRes )
        {
         TInt error = Imps_ERROR_BASE - event->iStatus ;
         if( error >= 800 && error < 900 
             || event->iStatus == KErrNone || error == 200 ) 
            {
            // NOTE
            // in case of sap initiated LeaveGroupResponse
            // the client needs the error code, so we need to
            // deliver it by calling also the error handler
            HandleErrorEventL( event->iOpCode, event->iStatus, aErrorFields );
            // call now the reqular handler
            HandleGroupEventL( ); 
            }
         else 
            {
            // If it is some other error code that 800-900 
            // then handle it as an error
            HandleErrorEventL( event->iOpCode, event->iStatus, aErrorFields );
            }
        }

    else 
        {
        if ( event->iStatus != ( Imps_ERROR_BASE - 200 ) &&
             event->iStatus != KErrNone )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log(
                _L("CImpsGroupHandler2: error event-iStatus=%d"), 
                event->iStatus);
#endif
            HandleErrorEventL( event->iOpCode, event->iStatus, aErrorFields );
            }
            // Call regular observer
            // iStatus is error code or message size
        else if ( iStatus >= 0 )
            {
            HandleGroupEventL( ); 
            }
        }
    }

// ----------------------------------------------------------------------------
// CImpsGroupHandler2::HandleGroupEventL()
// ----------------------------------------------------------------------------
void CImpsGroupHandler2::HandleGroupEventL( )
    {

#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2::HandleGroupEventL begins"));
#endif

    SImpsEventData* event = (SImpsEventData*)iEventData.Ptr();
    RImpsGroupClient2& client = (RImpsGroupClient2&)iClient;
    MImpsGroupHandler2* handler = client.Handler();

    TInt err( KErrNone );

#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2: msgType= %x opId= %d"),event->iMessageType, event->iOpCode );
#endif
    // We are not interested in if the message is response or push message
    // Just worry about message type from now on.

    CImpsDataAccessor* myAccessor = CImpsDataAccessor::NewLC( iFields ); // << 

    // call appropriate observer method
    switch ( event->iMessageType )
        {
        case EImpsStatus:
            {
            TRAP( err, handler->HandleCompleteL(
                    event->iOpCode,
                    *client.CspIdentifier() ));
            };
            break;
        case EImpsJoinGroupRes:
            {
            CDesCArrayFlat* users = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( users );
            CDesCArrayFlat* screenNames = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( screenNames );
            TPtrC welcomeText;
            welcomeText.Set( KNullDesC );

            CImpsKey* myKey = CImpsKey::NewLC();
            TImpsCDataUtils::AddValuesFromArrayL( 
                                        myKey, 
                            KJoinGroupResponseElements, 
                            sizeof( KJoinGroupResponseElements ) /
                            sizeof( KJoinGroupResponseElements[0] ) );

            // Get CSP version
            TReal version (0);
            TRAPD( err, version = TImpsDataUtils::GetCenRepRealValueL( 
       			KCRUIDWVEngineVariation, KWVEngineCspVersion )	);
       		// CSP 1.2 UserMapList
    	    if ( !err && version == 1.2 )
    	        {
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2::JoinGroupResponse version 1.2") );
#endif
                myKey->AddL( CREATEKEY( EImpsKeyUserMapList, 0 ) ); 
                TImpsCDataUtils::GetUserMapListL( myKey, 
                                               myAccessor, users, screenNames );
                myKey->PopL( ); // Pop out the UserMapList
                }
            // CSP 1.1 UserList
            else
                {
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2::JoinGroupResponse version 1.1") );
#endif 
                myKey->AddL( CREATEKEY( EImpsKeyUserList, 0 ) ); 
            TImpsCDataUtils::GetUserListL( myKey, 
                                           myAccessor, users, screenNames );
            myKey->PopL( ); // Pop out the UserList
                }
            myKey->AddL( CREATEKEY( EImpsKeyWelcomeNote, 0 ) );
            myKey->AddL( CREATEKEY( EImpsKeyContentData, 0 ) );
            TDesC* temp = NULL;
            if( myAccessor->RestoreDescL( myKey, temp ) )
                welcomeText.Set( *temp );
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2::calls HandleJoinL"));
#endif
            TRAP( err, handler->HandleJoinL(
                event->iOpCode,
                *users,
                *screenNames,
                welcomeText,
                *iClient.CspIdentifier() ));
            CleanupStack::PopAndDestroy( 3 ); // users, screennames, myKey
            };
            break;
        case EImpsLeaveGroupRes:
            {
            TPtrC groupId;
            groupId.Set( KNullDesC );
            TPtrC description;
            description.Set( KNullDesC );

            CImpsKey* myKey = CImpsKey::NewLC();
            TImpsCDataUtils::AddValuesFromArrayL( 
                                    myKey, 
                                    KLeaveGroupGroupIdElements, 
                                    sizeof( KLeaveGroupGroupIdElements ) /
                                    sizeof( KLeaveGroupGroupIdElements[0] ) );
            myKey->AddL( CREATEKEY( EImpsKeyGroupID, 0 ) );
            TDesC* temp = NULL;
            if( myAccessor->RestoreDescL( myKey, temp ) )
                groupId.Set( *temp );

            myKey->PopL( );
            myKey->AddL( CREATEKEY( EImpsKeyResult, 0 ) );
            myKey->AddL( CREATEKEY( EImpsKeyDescription, 0 ) );
            TDesC* temp2 = NULL;
            if( myAccessor->RestoreDescL( myKey, temp2 ) )
                description.Set( *temp2 );
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2::calls HandleLeaveL"));
#endif
            TRAP( err, handler->HandleLeaveL(
                event->iOpCode,
                groupId,
                description,
                *iClient.CspIdentifier() ));
            CleanupStack::PopAndDestroy( 1 ); // mykey
            }
            break;
        case EImpsGroupMembersRes:
            {
            CDesCArrayFlat* users = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( users );
            CDesCArrayFlat* screenNames = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( screenNames );
            CDesCArrayFlat* groupModerators = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( groupModerators );
            CDesCArrayFlat* groupAdmins = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( groupAdmins );

            CImpsKey* myKey = CImpsKey::NewLC();
            TImpsCDataUtils::AddValuesFromArrayL( 
                                myKey, 
                                KGroupMembersUserListElements, 
                                sizeof( KGroupMembersUserListElements ) /
                            sizeof( KGroupMembersUserListElements[0] ) );
            myKey->AddL( CREATEKEY( EImpsKeyUserList, 0 ) );
            TImpsCDataUtils::GetUserListL( myKey, myAccessor, users, screenNames );
            myKey->PopL( 2 ); // Pop out the Users and userList
            TImpsCDataUtils::GetModAndAdmL( myKey, myAccessor,
                                            groupModerators, groupAdmins );
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2:: calls HandleGroupMembersL"));
#endif
            TRAP( err, handler->HandleGroupMembersL(
                event->iOpCode,
                *users,
                *screenNames,
                *groupModerators,
                *groupAdmins,
                *iClient.CspIdentifier() ));
            // users, screennames, group moderators, group admins, mykey
            CleanupStack::PopAndDestroy( 5 ); 
            }
            break;
        case EImpsGroupPropertiesRes:
            {
            CImpsCommonGroupPropsInt* comP = CImpsCommonGroupPropsInt::NewL( );
            CleanupStack::PushL( comP );
            CImpsPrivateGroupPropsInt* priP = CImpsPrivateGroupPropsInt::NewL( );
            CleanupStack::PushL( priP );

            CImpsKey* myKey = CImpsKey::NewLC();
            TImpsCDataUtils::AddValuesFromArrayL( 
                                            myKey, 
                                            KGroupPropElements, 
                                            sizeof( KGroupPropElements ) /
                                            sizeof( KGroupPropElements[0] ) );
            TImpsCDataUtils::GetGroupPropertiesL( myKey, myAccessor, comP, priP );
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2:: calls HandleGroupPropertiesL(GroupProperties)"));
#endif
            TRAP( err, handler->HandleGroupPropertiesL(
                event->iOpCode,
                KNullDesC,
                *comP,
                *priP,
                *iClient.CspIdentifier() ));
            CleanupStack::PopAndDestroy( 3 ); // comP, pri, myKey
            }
            break;
        case EImpsGroupChangeNotice:
            {
            CImpsCommonGroupPropsInt* comP = CImpsCommonGroupPropsInt::NewL( );
            CleanupStack::PushL( comP );
            CImpsPrivateGroupPropsInt* priP = CImpsPrivateGroupPropsInt::NewL( );
            CleanupStack::PushL( priP );
            TPtrC groupId;
            groupId.Set( KNullDesC );

            CImpsKey* myKey = CImpsKey::NewLC();
            TImpsCDataUtils::AddValuesFromArrayL( 
                                            myKey, 
                                            KGroupChangeNoticeElements, 
                                            sizeof( KGroupChangeNoticeElements ) /
                                            sizeof( KGroupChangeNoticeElements[0] ) );
            myKey->AddL( CREATEKEY( EImpsKeyGroupID, 0 ) );
            TDesC* temp = NULL;
            if( myAccessor->RestoreDescL( myKey, temp ) )
                groupId.Set( *temp );

            myKey->PopL( );
            TImpsCDataUtils::GetGroupPropertiesL( myKey, myAccessor, comP, priP );
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2:: calls HandleGroupPropertiesL(GroupChange)"));
#endif
            TRAP( err, handler->HandleGroupPropertiesL(
                event->iOpCode,
                groupId,
                *comP,
                *priP,
                *iClient.CspIdentifier()));
            CleanupStack::PopAndDestroy( 3 ); // comP, pri, myKey
            }
            break;
        case EImpsGroupRejectListRes:
            {
            CDesCArrayFlat* users = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( users );

            CImpsKey* myKey = CImpsKey::NewLC();
            TImpsCDataUtils::AddValuesFromArrayL( 
                                    myKey, 
                                    KGroupRejectUserListElements, 
                                    sizeof( KGroupRejectUserListElements ) /
                                    sizeof( KGroupRejectUserListElements[0] ) );
            TImpsCDataUtils::GetUsersL( myKey, myAccessor, users );
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2:: calls HandleRejectListL"));
#endif
            TRAP( err, handler->HandleRejectListL(
                event->iOpCode,
                *users,
                *iClient.CspIdentifier()));
            CleanupStack::PopAndDestroy( 2 ); // users myKey
            }
            break;
        case EImpsSubsGroupNoticeRes:
            {
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2:: calls HandleSubscriptionL"));
#endif
            TRAP( err, handler->HandleSubscriptionL(
                event->iOpCode,
                event->iAux,
                *iClient.CspIdentifier() ));
            }
            break;

        case EImpsNewUsers:
            {
            CDesCArrayFlat* users = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( users );
            CDesCArrayFlat* screenNames = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( screenNames );
            TPtrC groupId;
            groupId.Set( KNullDesC );

            CImpsKey* myKey = CImpsKey::NewLC();
            TImpsCDataUtils::AddValuesFromArrayL( 
                                myKey, 
                                KGroupChangeNoticeJoinedElements, 
                                sizeof( KGroupChangeNoticeJoinedElements ) /
                                sizeof( KGroupChangeNoticeJoinedElements[0] ) );
            // Get CSP version
            TReal version (0);
            TRAPD( err, version = TImpsDataUtils::GetCenRepRealValueL( 
       			KCRUIDWVEngineVariation, KWVEngineCspVersion )	);
       		// CSP 1.2 UserMapList
    	    if ( !err && version == 1.2 )
    	        {
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2::Handle new users version 1.2") );
#endif
                myKey->AddL( CREATEKEY( EImpsKeyUserMapList, 0 ) ); 
                TImpsCDataUtils::GetUserMapListL( myKey, 
                                               myAccessor, users, screenNames );
                myKey->PopL( ); // Pop out the UserMapList
                }
            // CSP 1.1 UserList
            else
                { 
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2::Handle new users version 1.1") );
#endif
                myKey->AddL( CREATEKEY( EImpsKeyUserList, 0 ) ); 
                TImpsCDataUtils::GetUserListL( myKey, 
                                               myAccessor, users, screenNames );
                myKey->PopL( ); // Pop out the UserList
                }
                
            myKey->PopL( 1 ); // Pop out Joined 
            myKey->AddL( CREATEKEY( EImpsKeyGroupID, 0 ) );
            TDesC* temp = NULL;
            if( myAccessor->RestoreDescL( myKey, temp ) )
                groupId.Set( *temp );
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2:: calls HandleNewUsersL"));
#endif
            TRAP( err, handler->HandleNewUsersL(
                groupId,
                *users,
                *screenNames,
                *iClient.CspIdentifier() ));
            CleanupStack::PopAndDestroy( 3 ); // users, screenNames, myKey
            }
            break;

        case EImpsLeftUsers:
            {
            CDesCArrayFlat* users = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( users );
            CDesCArrayFlat* screenNames = new ( ELeave )CDesCArrayFlat( KArraySize );
            CleanupStack::PushL( screenNames );
            TPtrC groupId;
            groupId.Set( KNullDesC );

            CImpsKey* myKey = CImpsKey::NewLC();
            TImpsCDataUtils::AddValuesFromArrayL( 
                            myKey, 
                            KGroupChangeNoticeLeftElements, 
                            sizeof( KGroupChangeNoticeLeftElements ) /
                            sizeof( KGroupChangeNoticeLeftElements[0] ) );
            TImpsCDataUtils::GetUserListL( myKey, myAccessor, users, screenNames );
            myKey->PopL( 2 );  // Pop out Left and UserList
            myKey->AddL( CREATEKEY( EImpsKeyGroupID, 0 ) );
            TDesC* temp = NULL;
            if( myAccessor->RestoreDescL( myKey, temp ) )
                groupId.Set( *temp );
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2:: calls HandleLeftUsersL"));
#endif
            TRAP( err, handler->HandleLeftUsersL(
                groupId,
                *users,
                *screenNames,
                *iClient.CspIdentifier() ));
            CleanupStack::PopAndDestroy( 3 ); // myKey, users, screenNames
            }
            break;

        default:
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsGroupHandler2:: default branch"));
#endif
                __ASSERT_DEBUG( 0 == 1,
                                User::Panic( KImpsPanicCategory,
                                            EImpsCorrupted ) ); 
            User::Leave( KErrNotSupported );
            break;
        }
    CleanupStack::PopAndDestroy( 1 );  // >>> data accessor

    }


//  End of File  
