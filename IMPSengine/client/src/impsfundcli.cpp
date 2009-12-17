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
* fundemental feature client interface.            
* Issues WV requests belonging to the Fundamental API
*
*
*/


// INCLUDE FILES
#include    "impsfundhandler.h"
#include    "impsfundcommand.h"
#include    "impsfundcli.h"
#include    "impscdatautils.h"
#include    "impsutils.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RImpsFundClient2::RImpsFundClient2
// ---------------------------------------------------------
EXPORT_C RImpsFundClient2::RImpsFundClient2() :
        RImpsClient2(),
        iCommand( NULL ),
        iSearchCallBack( NULL ),
        iInviteCallBack( NULL )
    {
    SetOpIdRange( ); 
    }
  
// ---------------------------------------------------------
// RImpsFundClient2::RegisterL
// ---------------------------------------------------------
EXPORT_C void RImpsFundClient2::RegisterL( 
    RImpsEng& aEngine, 
    MImpsSearchHandler2* aSearchObs,
    MImpsInviteHandler2* aInviteObs,
    const TDesC& aClientId,
    TBool aReceiveNew,
    TInt aPriority )
    {
    iSearchCallBack = aSearchObs;
    iInviteCallBack = aInviteObs;
    iHandler = CImpsFundHandler2::NewL( *this, aPriority );
    iCommand = CImpsFundCommand2::NewL( *this );
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

// ----------------------------------------------------------------------------
// RImpsFundClient2::Unregister()
// ----------------------------------------------------------------------------
EXPORT_C void RImpsFundClient2::Unregister()
    {
    DoUnregister();
    }

// ----------------------------------------------------------------------------
// RImpsFundClient2::DoRegister()
// ----------------------------------------------------------------------------
TInt RImpsFundClient2::DoRegister(
   RImpsEng& aEngine,
   CImpsHandler2* aHandler )
   {
   return DoRegister2( aEngine, aHandler, EImpsFundRegister );
   }

// ----------------------------------------------------------------------------
// RImpsFundClient2::SearchFirstL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsFundClient2::SearchFirstL( const CSearchPairs& aPairs, 
                                             TInt aSearchLimit )
    {
    IncreaseOpId( );
    iCommand->InitialiseL( KSearchRequestElements,
                           sizeof( KSearchRequestElements ) /
                           sizeof( KSearchRequestElements[0] ) );
    TInt count = aPairs.Count();
    if( count > 0 )
        {
        iCommand->MakeSearchPairListL( aPairs );
        iCommand->InsertIntegerElementL( aSearchLimit, EImpsKeySearchLimit, 0 );
        }
    else
        User::Leave( KErrCorrupt);
    iCommand->PackAndSendL( EImpsSearchReq, iOpId );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsFundClient2: SearchFirstL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif
    return iOpId;
    }
        
// ----------------------------------------------------------------------------
// RImpsFundClient2::SearchNextL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsFundClient2::SearchNextL( TInt aSearchID, TInt aIndex )
    {
    IncreaseOpId( );
    iCommand->InitialiseL( KSearchRequestElements,
                           sizeof( KSearchRequestElements ) /
                           sizeof( KSearchRequestElements[0] ) );
    iCommand->InsertIntegerElementL( aSearchID, EImpsKeySearchID, 0 );
    iCommand->InsertIntegerElementL( aIndex, EImpsKeySearchIndex, 0 );
    iCommand->PackAndSendL( EImpsSearchReq, iOpId );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsFundClient2: SearchNextL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif

    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsFundClient2::StopSearchL
//
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsFundClient2::StopSearchL( TInt aSearchID )
    {
    IncreaseOpId( );
    iCommand->InitialiseL( KSearchStopRequestElements,
                           sizeof( KSearchStopRequestElements ) /
                           sizeof( KSearchStopRequestElements[0] ) );
    iCommand->InsertIntegerElementL( aSearchID, EImpsKeySearchID, 0 );
    iCommand->PackAndSendL( EImpsStopSearchReq, iOpId );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsFundClient2: StopSearchL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif

    return iOpId;
    }

// ----------------------------------------------------------------------------
// RImpsFundClient2::GroupInviteL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsFundClient2::GroupInviteL( const TDesC& aInviteID,
                                    const MDesCArray* aUserID,
                                    const MDesCArray* aScreenName,
                                    const MDesCArray* aGroupName,
                                    const TDesC& aInviteGroup,
                                    const TDesC& aOwnScreenName,
                                    const TDesC& aOwnGroupName,
                                    const TDesC& aInviteReason,
                                    const TInt aValidityPeriod )
    {
    IncreaseOpId( );
    TBool recipients = EFalse;
    iCommand->InitialiseL( KInviteRequestElements,
                           sizeof( KInviteRequestElements ) /
                           sizeof( KInviteRequestElements[0] ) );
    iCommand->InsertDescriptorElementL( aInviteID, EImpsKeyInviteID, 0 );
    iCommand->InsertIntegerElementL( EImpsGR, EImpsKeyInviteType, 0 );
    iCommand->InsertEmptyElementL( EImpsKeyRecipient, 0 );
    if( aUserID != NULL && aUserID->MdcaCount() > 0 )
    {
        iCommand->SetUserIDsL( aUserID );
        recipients = ETrue;
        }
    if( aScreenName != NULL && aScreenName->MdcaCount() > 0 )
        {
        iCommand->InsertEmptyElementL( EImpsKeyGroup, 0 );
        iCommand->SetScreenNamesL( aScreenName, aGroupName );
        recipients = ETrue;
        }
    iCommand->PopElementL();  //recipient
    //No recipients defined, please try again...
    if( !recipients )
        User::Leave( KErrCorrupt );
    //This is mandatory
    iCommand->InsertDescriptorElementL( aInviteGroup, EImpsKeyGroupID, 0 );
    //This is not
    if( aInviteReason.Length() > 0 )
        iCommand->InsertDescriptorElementL( aInviteReason, 
                                            EImpsKeyInviteNote, 0 );
    //Nor are these
    if( aOwnScreenName.Length() > 0 )
        {
        iCommand->InsertEmptyElementL( EImpsKeyScreenName, 0 );
        iCommand->InsertDescriptorElementL( aOwnScreenName, EImpsKeySName, 0 );
        iCommand->InsertDescriptorElementL( aOwnGroupName, EImpsKeyGroupID, 0 );
        iCommand->PopElementL();  // screen name
        }
    iCommand->InsertIntegerElementL( aValidityPeriod, EImpsKeyValidity, 0 );
    iCommand->PackAndSendL( EImpsInviteReq, iOpId );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsFundClient2: GroupInviteL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif

    return iOpId;
    }
    
// ----------------------------------------------------------------------------
// RImpsFundClient2::ImInviteL
// ----------------------------------------------------------------------------
// 
EXPORT_C TInt RImpsFundClient2::ImInviteL( const TDesC& aInviteID,
                                 const TDesC& aUserID,                              
                                 const TDesC& aInviteReason,
                                 const TInt aValidityPeriod )
    {
    IncreaseOpId( );

    iCommand->InitialiseL( KInviteRequestElements,
                           sizeof( KInviteRequestElements ) /
                           sizeof( KInviteRequestElements[0] ) );
    iCommand->InsertDescriptorElementL( aInviteID, EImpsKeyInviteID, 0 );
    iCommand->InsertIntegerElementL( EImpsIM, EImpsKeyInviteType, 0 );
    iCommand->InsertEmptyElementL( EImpsKeyRecipient, 0 );
    iCommand->InsertDescriptorElementL( aUserID, EImpsKeyUserID, 0 );   
    iCommand->PopElementL();  // recipient
    if( aInviteReason.Length() > 0 )
        {
        iCommand->InsertDescriptorElementL( aInviteReason, 
                                            EImpsKeyInviteNote, 0 );
    }
    iCommand->InsertIntegerElementL( aValidityPeriod, EImpsKeyValidity, 0 );
    iCommand->PackAndSendL( EImpsInviteReq, iOpId );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsFundClient2: GroupInviteL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif

    return iOpId;
   
    }
    
// ----------------------------------------------------------------------------
// RImpsFundClient2::CancelInviteL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsFundClient2::CancelInviteL( const TDesC& aInviteId,
                                     const MDesCArray* aUserID,
                                     const MDesCArray* aScreenName,
                                     const MDesCArray* aGroupName,
                                     const TDesC& aCancelReason, 
                                     const TDesC& aOwnScreenName,
                                     const TDesC& aOwnGroupName )
    {
    IncreaseOpId( );
    TBool recipients = EFalse;
    iCommand->InitialiseL( KInviteCancelRequestElements,
                           sizeof( KInviteCancelRequestElements ) /
                           sizeof( KInviteCancelRequestElements[0] ) );
    iCommand->InsertDescriptorElementL( aInviteId, EImpsKeyInviteID, 0 );
    iCommand->InsertEmptyElementL( EImpsKeyRecipient, 0 );
    if( aUserID != NULL && aUserID->MdcaCount() > 0 )
        {
        iCommand->SetUserIDsL( aUserID );
        recipients = ETrue;
        }
    if( aScreenName != NULL && aScreenName->MdcaCount() > 0 )
        {
        iCommand->InsertEmptyElementL( EImpsKeyGroup, 0 );
        iCommand->SetScreenNamesL( aScreenName, aGroupName );
        recipients = ETrue;
        }
    iCommand->PopElementL( 2 );  //group, recipient
    //No recipients defined, please try again...
    if( !recipients )
        User::Leave( KErrCorrupt );
    if( aCancelReason.Length() != 0 )
        iCommand->InsertDescriptorElementL( aCancelReason, EImpsKeyInviteNote, 0 );
    if( aOwnScreenName.Length() > 0 )
    {
        iCommand->InsertEmptyElementL( EImpsKeyScreenName, 0 );
        iCommand->InsertDescriptorElementL( aOwnScreenName, EImpsKeySName, 0 );
        iCommand->InsertDescriptorElementL( aOwnGroupName, EImpsKeyGroupID, 0 );
    }
    iCommand->PackAndSendL( EImpsCancelInviteReq, iOpId );

#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsFundClient2: CancelInviteL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif

    return iOpId;
    }
    
// ----------------------------------------------------------------------------
// RImpsFundClient2::InviteResponseL
// ----------------------------------------------------------------------------
EXPORT_C TInt RImpsFundClient2::InviteResponseL( const TDesC& aInviteId,
                                       TBool aAcceptance,
                                       const TDesC& aInviteResponse,
                                       const TDesC& aOwnScreenName,
                                       const TDesC& aOwnGroupName )

    {
    IncreaseOpId( );
    iCommand->InitialiseL( KInviteUserResponseElements,
                           sizeof( KInviteUserResponseElements ) /
                           sizeof( KInviteUserResponseElements[0] ) );
    iCommand->InsertDescriptorElementL( aInviteId, EImpsKeyInviteID, 0 );
    iCommand->InsertBooleanElementL( aAcceptance, EImpsKeyAcceptance, 0 );
    if( aInviteResponse.Length() != 0 )
        {
        iCommand->InsertDescriptorElementL( aInviteResponse, EImpsKeyResponseNote, 0 );
        }
    if( aOwnScreenName.Length() != 0 )
        {
        iCommand->InsertEmptyElementL( EImpsKeyScreenName, 0 );
        iCommand->InsertDescriptorElementL( aOwnScreenName, EImpsKeySName, 0 );
        iCommand->InsertDescriptorElementL( aOwnGroupName, EImpsKeyGroupID, 0 );
        }
    iCommand->PackAndSendL( EImpsInviteUserRes, iOpId );
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log(_L("RImpsFundClient2: InviteResponseL opid=%d cli=%d"), 
        iOpId, (TInt)this );
#endif
    return iOpId;
    }



//  End of File  


