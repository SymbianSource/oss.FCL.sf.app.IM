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
* const definition class.
*
*/

#include    "impsGroupHandler.h"
#include    "ImpsGroupCli.h"
#include    "ImpsGroupCommand.h"
#include    "impsServerCommon.h"
#include    "impsliterals.h"
#include    "WVEngineInternalCRKeys.h"
#include    "impsdatautils.h"

// CONSTANTS

const TImpsContent KCreateGroupElements[]               = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeyCreateGroup_Request };
const TImpsContent KDeleteGroupElements[]               = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeyDeleteGroup_Request };
const TImpsContent KJoinGroupElements[]                 = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeyJoinGroup_Request };
const TImpsContent KLeaveGroupElements[]                = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeyLeaveGroup_Request };
const TImpsContent KGetGroupGroupElements[]             = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeyGetGroupMembers_Request };
const TImpsContent KAddGroupMembersElements[]           = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeyAddGroupMembers_Request };
const TImpsContent KRemoveGroupMembersElements[]        = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeyRemoveGroupMembers_Request };
const TImpsContent KModifyGroupMembersElements[]        = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeyMemberAccess_Request };
const TImpsContent KGetGroupGroupPropertiesElements[]   = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeyGetGroupProps_Request };
const TImpsContent KSetGroupGroupPropertiesElements[]   = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeySetGroupProps_Request };
const TImpsContent KRejectListElements[]                = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeyRejectList_Request };
const TImpsContent KImpsSubscribeGroupNoticeRequest[]   = { EImpsKeySession,
                                                            EImpsKeyTransaction,
                                                            EImpsKeyTransactionContent,
                                                            EImpsKeySubscribeGroupNotice_Request };
// LOCAL CONSTANTS AND MACROS
_LIT( KMaxNro,               "%d" );


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RImpsGroupClient2::RImpsGroupClient2
// ---------------------------------------------------------
EXPORT_C RImpsGroupClient2::RImpsGroupClient2() :
        RImpsClient2(), 
        iHandlerCallBack( NULL ),
        iCommand( NULL )
    {
    SetOpIdRange( ); 
    }

// ---------------------------------------------------------
// RImpsGroupClient2::RegisterL
// ---------------------------------------------------------
EXPORT_C void RImpsGroupClient2::RegisterL(
    RImpsEng& aEngine, 
    MImpsGroupHandler2* aObserver,
    const TDesC& aClientId,
    TBool aReceiveNew,
    TInt aPriority )
    {

    iHandlerCallBack = aObserver;
    iHandler = CImpsGroupHandler2::NewL( *this, aPriority );
    iCommand = CImpsGroupCommand2::NewL( *this );
    iActiveCommand = iCommand;

    // Start the handler
    iHandler->StartRun();
    iHandleNew = aReceiveNew;
    iAnyContent = EFalse;

    // Send registeration to the server thread
    TInt err = DoRegister( aEngine, iHandler );
    User::LeaveIfError( err ); 

    DoAssign( aClientId );
    }

// ---------------------------------------------------------
// RImpsGroupClient2::UnRegister
// ---------------------------------------------------------
//
EXPORT_C void RImpsGroupClient2::Unregister()
   {
   DoUnregister();
   }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::DoRegister()
// ----------------------------------------------------------------------------
TInt RImpsGroupClient2::DoRegister(
   RImpsEng& aEngine,
   CImpsHandler2* aHandler )
   {
   return DoRegister2( aEngine, aHandler, EImpsGroupRegister );
   }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::CreateGroupL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::CreateGroupL(
    const TDesC& aGroupId,
    const CImpsCommonGroupProps* aProperties,
    const TDesC& aScreenName,
    TBool aJoinGroup,
    TBool aSubscribeNotification )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KCreateGroupElements,
                           sizeof( KCreateGroupElements ) /
                           sizeof( KCreateGroupElements[0] ) );

    TInt index(0);
    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->AddKeyL( CREATEKEY( EImpsKeyGroupProperties, 0 ) );

    iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
    iCommand->InsertElementL( KImpsName, EImpsKeyName );
    iCommand->InsertElementL( aProperties->GroupName( ), EImpsKeyValue );
    iCommand->PopKeyL( );
    index++;

    if( aProperties->IsOpen( ) != EImpsPropUndef )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
        iCommand->InsertElementL( KImpsAccessType, EImpsKeyName );
        if( aProperties->IsOpen( ) == EImpsPropYes )
            iCommand->InsertElementL( KImpsOpen, EImpsKeyValue );
        else
            iCommand->InsertElementL( KImpsRestricted, EImpsKeyValue );
        iCommand->PopKeyL( );
        index++;
        }

    if( aProperties->IsPrivateAllowed( ) != EImpsPropUndef )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
        iCommand->InsertElementL( KImpsPrivateMessaging, EImpsKeyName );
        if( aProperties->IsPrivateAllowed( ) == EImpsPropYes )
            iCommand->InsertElementL( KImpsTrue, EImpsKeyValue );
        else
            iCommand->InsertElementL( KImpsFalse, EImpsKeyValue );
        iCommand->PopKeyL( );
        index++;
        }

    if( aProperties->IsPublic( ) != EImpsPropUndef )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
        iCommand->InsertElementL( KImpsType, EImpsKeyName );
        if( aProperties->IsPublic( ) == EImpsPropYes )
            iCommand->InsertElementL( KImpsPublic, EImpsKeyValue );
        else
            iCommand->InsertElementL( KImpsPrivate, EImpsKeyValue );
        iCommand->PopKeyL( );
        index++;
        }

    if( aProperties->IsSearchable( ) != EImpsPropUndef )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
        iCommand->InsertElementL( KImpsSearchable, EImpsKeyName );
        if( aProperties->IsSearchable( ) == EImpsPropYes )
            iCommand->InsertElementL( KImpsTrue, EImpsKeyValue );
        else
            iCommand->InsertElementL( KImpsFalse, EImpsKeyValue );
        iCommand->PopKeyL( );
        index++;
        }

    if( aProperties->Topic( ).Length() ) 
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
        iCommand->InsertElementL( KImpsTopic, EImpsKeyName );
        iCommand->InsertElementL( aProperties->Topic( ), EImpsKeyValue );
        iCommand->PopKeyL( );
        index++;
        }

      // AutoDelete for CSP 1.2
      TReal version (0);
      TRAPD( err, version = TImpsDataUtils::GetCenRepRealValueL( 
       			KCRUIDWVEngineVariation, KWVEngineCspVersion )	);
	 if ( !err && version == 1.2 )
	      {
	      if( aProperties->AutoDelete( ) != EImpsPropUndef )
	          {
	          iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
	          iCommand->InsertElementL( KImpsAutoDelete, EImpsKeyName );
	          if( aProperties->AutoDelete( ) == EImpsPropYes )
	            {
		          iCommand->InsertElementL( KImpsTrue, EImpsKeyValue );	
		          }
	            else
		          {
		          iCommand->InsertElementL( KImpsFalse, EImpsKeyValue );	
		          }
            iCommand->PopKeyL( );
            index++;
            }
        }
    iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
    iCommand->InsertElementL( KImpsMaxActiveUsers, EImpsKeyName );
    TInt  i = aProperties->MaxNbrOfUsers( );
    TBuf<10> nro; 
    nro.Format( KMaxNro, i );
    iCommand->InsertElementL( nro, EImpsKeyValue );

    iCommand->PopKeyL( );

    TPtrC wellcome = aProperties->Welcome( );
    if( wellcome.Length( ) )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyWelcomeNote, 0) );
        iCommand->InsertElementL( KImpsContentType, EImpsKeyContentType );
        iCommand->InsertElementL( wellcome, EImpsKeyContentData );
        iCommand->PopKeyL( ); // Pop out welcomenote
        }
    iCommand->PopKeyL( ); // Pop out group properties
    iCommand->InsertElementL( aJoinGroup, EImpsKeyJoinGroup );

    if( aScreenName.Length() ) 
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyScreenName, 0) );
        iCommand->InsertElementL( aScreenName, EImpsKeySName );
        iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
        iCommand->PopKeyL( ); // ScreenName
        }

    iCommand->InsertElementL( aSubscribeNotification, 
                              EImpsKeySubscribeNotification );

    iCommand->PackAndSendL( EImpsCreateGroupReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::DeleteGroupL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::DeleteGroupL(
    const TDesC& aGroupId )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KDeleteGroupElements,
                           sizeof( KDeleteGroupElements ) /
                           sizeof( KDeleteGroupElements[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->PackAndSendL( EImpsDeleteGroupReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::JoinGroupL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::JoinGroupL(
            const TDesC& aGroupId,
            const TDesC& aScreenName,
            TBool aUsers,
            TBool aSubscribeNotification )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KJoinGroupElements,
                           sizeof( KJoinGroupElements ) /
                           sizeof( KJoinGroupElements[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->AddKeyL( CREATEKEY( EImpsKeyScreenName, 0) );
    iCommand->InsertElementL( aScreenName, EImpsKeySName );
    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->PopKeyL( ); // ScreenName
    iCommand->InsertElementL( aUsers, EImpsKeyJoinedRequest );
    iCommand->InsertElementL( aSubscribeNotification , 
                              EImpsKeySubscribeNotification );
    iCommand->PackAndSendL( EImpsJoinGroupReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::LeaveGroupL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::LeaveGroupL(
    const TDesC& aGroupId )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KLeaveGroupElements,
                           sizeof( KLeaveGroupElements ) /
                           sizeof( KLeaveGroupElements[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->PackAndSendL( EImpsLeaveGroupReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::GroupMembersL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::GroupMembersL(
    const TDesC& aGroupId )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KGetGroupGroupElements,
                           sizeof( KGetGroupGroupElements ) /
                           sizeof( KGetGroupGroupElements[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->PackAndSendL( EImpsGroupMembersReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::AddMembersL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::AddMembersL(
    const TDesC& aGroupId,
    const MDesCArray& aUserList,
    const MDesCArray& aScreenNameList )
    {
    TInt count1 = aUserList.MdcaCount();
    TInt count2 = aScreenNameList.MdcaCount();
    if ( !count1 && !count2 )
        {
        User::Leave( KErrArgument );
        }

    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KAddGroupMembersElements,
                           sizeof( KAddGroupMembersElements ) /
                           sizeof( KAddGroupMembersElements[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->AddKeyL( CREATEKEY( EImpsKeyUserList, 0 ) );

    TInt i = 0;
    for( i =0; i<count1; i++ )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyUser, i ) );
        iCommand->InsertElementL( aUserList.MdcaPoint( i ), EImpsKeyUserID );
        iCommand->PopKeyL( ); // Pop the Userkey out
        }
    for( i=0; i<count2; i++ )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyScreenName, i ) );
        iCommand->InsertElementL( aScreenNameList.MdcaPoint( i ), 
                                  EImpsKeySName );
        iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
        iCommand->PopKeyL( ); // Pop the EImpsKeyScreenName out
        }
    iCommand->PackAndSendL( EImpsAddGroupMembersReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::RemoveMembersL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::RemoveMembersL(
    const TDesC& aGroupId,
    const MDesCArray& aUserList )
    {
    TInt count = aUserList.MdcaCount();
    if ( !count )
        {
        User::Leave( KErrArgument );
        }

    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KRemoveGroupMembersElements,
                           sizeof( KRemoveGroupMembersElements ) /
                           sizeof( KRemoveGroupMembersElements[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->AddKeyL( CREATEKEY( EImpsKeyUserList, 0 ) );

    for( TInt i=0; i<count; i++ )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyUser, i ) );
        iCommand->InsertElementL( aUserList.MdcaPoint( i ), EImpsKeyUserID );
        iCommand->PopKeyL( ); // Pop the Userkey out
        }
    iCommand->PackAndSendL( EImpsRemoveGroupMembersReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::ModifyMembersAccessL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::ModifyMembersAccessL(
    const TDesC& aGroupId,
    const MDesCArray* aAdminList,
    const MDesCArray* aModerList,
    const MDesCArray* aOrdinaryList )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KModifyGroupMembersElements,
                           sizeof( KModifyGroupMembersElements ) /
                           sizeof( KModifyGroupMembersElements[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );

    if( aAdminList )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyAdmin, 0 ) );
        iCommand->AddKeyL( CREATEKEY( EImpsKeyUserList, 0 ) );
        TInt count = aAdminList->MdcaCount();
        for( TInt i=0; i < count; i++ )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyUser, i ) );
            iCommand->InsertElementL( aAdminList->MdcaPoint( i ), 
                                      EImpsKeyUserID );
            iCommand->PopKeyL( ); // Pop the Userkey out
            }
        iCommand->PopKeyL( );
        iCommand->PopKeyL( );
        }

    if( aOrdinaryList )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyUsers, 0 ) );
        iCommand->AddKeyL( CREATEKEY( EImpsKeyUserList, 0 ) );
        TInt count = aOrdinaryList->MdcaCount( );
        for( TInt i=0; i < count; i++ )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyUser, i ) );
            iCommand->InsertElementL( aOrdinaryList->MdcaPoint( i ), 
                                      EImpsKeyUserID );
            iCommand->PopKeyL( ); // Pop the Userkey out
            }
        iCommand->PopKeyL( );
        iCommand->PopKeyL( );
        }
    if( aModerList )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyMod, 0 ) );
        iCommand->AddKeyL( CREATEKEY( EImpsKeyUserList, 0 ) );
        TInt count = aModerList->MdcaCount( );
        for( TInt i=0; i < count; i++ )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyUser, i ) );
            iCommand->InsertElementL( aModerList->MdcaPoint( i ), 
                                      EImpsKeyUserID );
            iCommand->PopKeyL( ); // Pop the Userkey out
            }
        }

    iCommand->PackAndSendL( EImpsGroupMemberAccessReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::GroupPropertiesL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::GroupPropertiesL(
    const TDesC& aGroupId )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KGetGroupGroupPropertiesElements,
                           sizeof( KGetGroupGroupPropertiesElements ) /
                           sizeof( KGetGroupGroupPropertiesElements[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->PackAndSendL( EImpsGroupPropertiesReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::SetGroupPropertiesL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::SetGroupPropertiesL(
    const TDesC& aGroupId,
    const CImpsCommonGroupProps* aGroupProps,
    const CImpsPrivateGroupProps* aOwnProps )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KSetGroupGroupPropertiesElements,
                           sizeof( KSetGroupGroupPropertiesElements ) /
                           sizeof( KSetGroupGroupPropertiesElements[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    if ( aGroupProps )
        {
        TInt index(0);
        iCommand->AddKeyL( CREATEKEY( EImpsKeyGroupProperties, 0 ) );
        iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
        iCommand->InsertElementL( KImpsName, EImpsKeyName );
        iCommand->InsertElementL( aGroupProps->GroupName( ), EImpsKeyValue );
        iCommand->PopKeyL( );
        index++;

        if( aGroupProps->IsOpen( ) != EImpsPropUndef )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
            iCommand->InsertElementL( KImpsAccessType, EImpsKeyName );
            if( aGroupProps->IsOpen( ) == EImpsPropYes )
                iCommand->InsertElementL( KImpsOpen, EImpsKeyValue );
            else
                iCommand->InsertElementL( KImpsRestricted, EImpsKeyValue );
            iCommand->PopKeyL( );
            index++;
            }

        if( aGroupProps->IsPrivateAllowed( ) != EImpsPropUndef )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
            iCommand->InsertElementL( KImpsPrivateMessaging, EImpsKeyName );
            if( aGroupProps->IsPrivateAllowed( ) == EImpsPropYes )
                iCommand->InsertElementL( KImpsTrue, EImpsKeyValue );
            else
                iCommand->InsertElementL( KImpsFalse, EImpsKeyValue );
            iCommand->PopKeyL( );
            index++;
            }

        if( aGroupProps->IsPublic( ) != EImpsPropUndef )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
            iCommand->InsertElementL( KImpsType, EImpsKeyName );
            if( aGroupProps->IsPublic( ) == EImpsPropYes )
                iCommand->InsertElementL( KImpsPublic, EImpsKeyValue );
            else
                iCommand->InsertElementL( KImpsPrivate, EImpsKeyValue );
            iCommand->PopKeyL( );
            index++;
            }

        if( aGroupProps->IsSearchable( ) != EImpsPropUndef )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
            iCommand->InsertElementL( KImpsSearchable, EImpsKeyName );
            if( aGroupProps->IsSearchable( ) == EImpsPropYes )
                iCommand->InsertElementL( KImpsTrue, EImpsKeyValue );
            else
                iCommand->InsertElementL( KImpsFalse, EImpsKeyValue );
            iCommand->PopKeyL( );
            index++;
            }

        iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
        iCommand->InsertElementL( KImpsTopic, EImpsKeyName );
        iCommand->InsertElementL( aGroupProps->Topic( ), EImpsKeyValue );
        iCommand->PopKeyL( );
        index++;

        // AutoDelete for CSP 1.2
        TReal version (0);
    	TRAPD( err, version = TImpsDataUtils::GetCenRepRealValueL( 
        			KCRUIDWVEngineVariation, KWVEngineCspVersion )	);
	    if ( !err && version == 1.2 )
	        {
	        if( aGroupProps->AutoDelete( ) != EImpsPropUndef )
	            {
	            iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
	            iCommand->InsertElementL( KImpsAutoDelete, EImpsKeyName );
	            if( aGroupProps->AutoDelete( ) == EImpsPropYes )
		            {
		            iCommand->InsertElementL( KImpsTrue, EImpsKeyValue );	
		            }
	            else
		            {
		            iCommand->InsertElementL( KImpsFalse, EImpsKeyValue );	
		            }
	            iCommand->PopKeyL( );
	            index++;
	            }
	        }
        
        iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
        iCommand->InsertElementL( KImpsMaxActiveUsers, EImpsKeyName );
        TInt  i = aGroupProps->MaxNbrOfUsers( );
        TBuf<10> nro; 
        nro.Format( KMaxNro, i );
        iCommand->InsertElementL( nro, EImpsKeyValue );
        iCommand->PopKeyL( );

        TPtrC wellcome = aGroupProps->Welcome( );
        if( wellcome.Length( ) )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyWelcomeNote, 0) );
            iCommand->InsertElementL( KImpsContentType, EImpsKeyContentType );
            iCommand->InsertElementL( wellcome, EImpsKeyContentData );
            iCommand->PopKeyL( );
            }
        iCommand->PopKeyL( );
        }
    if ( aOwnProps )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyOwnProperties, 0 ) );

        TInt index(0);
        if( aOwnProps->IsPrivateAllowed( ) != EImpsPropUndef )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
            iCommand->InsertElementL( KImpsPrivateMessaging, EImpsKeyName );
            if( aOwnProps->IsPrivateAllowed( ) == EImpsPropYes )
                iCommand->InsertElementL( KImpsTrue, EImpsKeyValue );
            else
                iCommand->InsertElementL( KImpsFalse, EImpsKeyValue );
            iCommand->PopKeyL( );
            index++;
            }

        if( aOwnProps->IsMember( ) != EImpsPropUndef )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
            iCommand->InsertElementL( KImpsIsMember, EImpsKeyName );
            if( aOwnProps->IsMember( ) == EImpsPropYes )
                iCommand->InsertElementL( KImpsTrue, EImpsKeyValue );
            else
                iCommand->InsertElementL( KImpsFalse, EImpsKeyValue );
            iCommand->PopKeyL( );
            index++;
            }

        if( aOwnProps->Privileges( ) != EImpsUserUndef )
            {
            iCommand->AddKeyL( CREATEKEY( EImpsKeyProperty, index ) );
            iCommand->InsertElementL( KImpsPriviledgeLevel, EImpsKeyName );
            switch( aOwnProps->Privileges( ) )
                {
                case EImpsOrdinary:
                    iCommand->InsertElementL( KImpsUser, EImpsKeyValue );
                    break;
                case EImpsModerator:
                    iCommand->InsertElementL( KImpsMod, EImpsKeyValue );
                    break;
                case EImpsAdmin:
                    iCommand->InsertElementL( KImpsAdmin, EImpsKeyValue );
                    break;
                default:
                    // notice: OPa for Thumb build
                    break;
                }
            }
        }
    iCommand->PackAndSendL( EImpsSetGroupPropertiesReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::SetRejectListL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::SetRejectListL(
    const TDesC& aGroupId,
    const MDesCArray* aRejectedList,
    const MDesCArray* aEnabledList )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KRejectListElements,
                           sizeof( KRejectListElements ) /
                           sizeof( KRejectListElements[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );

    if( aRejectedList )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyAddList, 0 ) );
        TInt count = aRejectedList->MdcaCount( );
        for( TInt i=0; i < count; i++ )
            {
            iCommand->InsertElementL( aRejectedList->MdcaPoint( i ), 
                                      EImpsKeyUserID, i);
            }
        iCommand->PopKeyL( );
        }

    if( aEnabledList )
        {
        iCommand->AddKeyL( CREATEKEY( EImpsKeyRemoveList, 0 ) );
        TInt count = aEnabledList->MdcaCount( );
        for( TInt i=0; i < count; i++ )
            {
            iCommand->InsertElementL( aEnabledList->MdcaPoint( i ), 
                                      EImpsKeyUserID, i);
            }
        }
    iCommand->PackAndSendL( EImpsGroupRejectListReq, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::SubscribeL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::SubscribeL(
    const TDesC& aGroupId )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KImpsSubscribeGroupNoticeRequest,
                           sizeof( KImpsSubscribeGroupNoticeRequest ) /
                           sizeof( KImpsSubscribeGroupNoticeRequest[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->InsertElementIntL( EImpsS, EImpsKeySubscribeType );
    iCommand->PackAndSendL( EImpsSubsGroupNoticeSet, iOpId );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::UnsubscribeL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::UnsubscribeL(
    const TDesC& aGroupId )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KImpsSubscribeGroupNoticeRequest,
                           sizeof( KImpsSubscribeGroupNoticeRequest ) /
                           sizeof( KImpsSubscribeGroupNoticeRequest[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->InsertElementIntL( EImpsU, EImpsKeySubscribeType );
    iCommand->PackAndSendL( EImpsSubsGroupNoticeUnset, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::CheckSubscriptionL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsGroupClient2::CheckSubscriptionL(
    const TDesC& aGroupId )
    {
    IncreaseOpId( );
    iCommand->ResetMembers();
    iCommand->InitialiseL( KImpsSubscribeGroupNoticeRequest,
                           sizeof( KImpsSubscribeGroupNoticeRequest ) /
                           sizeof( KImpsSubscribeGroupNoticeRequest[0] ) );

    iCommand->InsertElementL( aGroupId, EImpsKeyGroupID );
    iCommand->InsertElementIntL( EImpsG, EImpsKeySubscribeType );
    iCommand->PackAndSendL( EImpsSubsGroupNoticeGet, iOpId  );
    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsGroupClient2::Handler
// ----------------------------------------------------------------------------
MImpsGroupHandler2* RImpsGroupClient2::Handler( )
    {
    return iHandlerCallBack;
    }
    
    
// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File 



