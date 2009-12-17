/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Generic event and message channel for Connection UI.
*
*/


// INCLUDE FILES
#include <E32std.h>
#include <bldvariant.hrh>

#include "CCnUiGroupChannel.h"
#include "ChatNGCommonUIDs.h"
#include "IMPSCommonUiDebugPrint.h"
#include "IMPSSharedDataDefs.h"
#include "CIMPSSharedDataFactory.h"
#include "IMPSUIDDefs.h"



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCnUiGroupChannel::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCnUiGroupChannel* CCnUiGroupChannel::NewLC( TInt aGroupId,
                                             TGCChannelID aChannelId,
                                             TBool aTemporary )
    {
    CCnUiGroupChannel* self = new ( ELeave ) CCnUiGroupChannel( aGroupId,
                                                                aChannelId );
    CleanupStack::PushL( self );
    self->ConstructL( aTemporary );
    return self;
    }


// -----------------------------------------------------------------------------
// CCnUiGroupChannel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCnUiGroupChannel* CCnUiGroupChannel::NewL( TInt aGroupId,
                                            TGCChannelID aChannelId,
                                            TBool aTemporary )
    {
    CCnUiGroupChannel* self = CCnUiGroupChannel::NewLC( aGroupId, aChannelId, aTemporary );
    CleanupStack::Pop( self ); //self
    return self;
    }



// Destructor
CCnUiGroupChannel::~CCnUiGroupChannel()
    {
    CancelListen();
    iKeys.Close();
    delete iSharedData;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiGroupChannel::CCnUiGroupChannel( TInt aGroupId,
                                      TGCChannelID aChannelId )
        :   iGroupId( aGroupId ),
        iChannelId( aChannelId ),
        iChannelSignaled( EFalse )
    {
    }


// EPOC default constructor can leave.
void CCnUiGroupChannel::ConstructL( TBool aTemporary )
    {
    __ASSERT_DEBUG( ( iChannelId >= 0 ), User::Leave( KErrArgument ) );
    __ASSERT_DEBUG( ( iChannelId <=  KMaxChannelIdValue ), User::Leave( KErrArgument ) );

    __ASSERT_DEBUG( ( iGroupId >= 0 ), User::Leave( KErrArgument ) );
    __ASSERT_DEBUG( ( iGroupId <=  KMaxChannelIdValue ), User::Leave( KErrArgument ) );

    //make channel keys based the global / local mode
    if ( aTemporary == ECnUiGlobalGroup )
        {
        //global ones
        iChannelKey.Format( KGlobalChannelKeyNameFormatter, iChannelId );
        iChannelDataKey.Format( KGlobalChannelDataKeyNameFormatter, iChannelId );

        iSharedData = CIMPSSharedDataFactory::CreatePermanentKeyHandlerL(
                          this, KIMPSConnUiPermanentUid );
        iUid = KIMPSConnUiPermanentUid;
        }

    else
        {
        //group ones
        iChannelKey.Format( KGroupChannelKeyNameFormatter, iGroupId, iChannelId );
        iChannelDataKey.Format( KGroupChannelDataKeyNameFormatter, iGroupId, iChannelId );
        iSharedData = CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL(
                          this, KIMPSConnUiTemporaryUid );
        iUid = KIMPSConnUiTemporaryUid;
        }

    //insert permanent / temporary identifier
    if ( aTemporary )
        {
        iChannelKey.Insert( KGlobalChannelTypeIdPosition,
                            KGlobalChannelTemporaryKeyIdentifier );

        iChannelDataKey.Insert( KGlobalChannelTypeIdPosition,
                                KGlobalChannelTemporaryKeyIdentifier );
        }
    else
        {
        iChannelKey.Insert( KGlobalChannelTypeIdPosition,
                            KGlobalChannelPermanentKeyIdentifier );

        iChannelDataKey.Insert( KGlobalChannelTypeIdPosition,
                                KGlobalChannelPermanentKeyIdentifier );
        }

    User::LeaveIfError( iSharedData->ConvertSharedDataKey( iChannelKey, iKey ) );
    User::LeaveIfError( iSharedData->ConvertSharedDataKey( iChannelDataKey, iDataKey ) );

    // initialize "channel signalled"-flag properly
    TInt current = 0;
    TInt err = iSharedData->GetIntKey( iKey, current );
    if ( err == KErrNotFound )
        {
        // if it was not found, correct value is 0
        err = KErrNone;
        }

    User::LeaveIfError( err );
    iChannelSignaled = current > 0;
    }


// -----------------------------------------------------------------------------
// CCnUiGroupChannel::WriteL()
// -----------------------------------------------------------------------------
//
void CCnUiGroupChannel::WriteL( TInt aMsg )
    {
    User::LeaveIfError( iSharedData->SetIntKey( iKey, aMsg ) );
    }

// -----------------------------------------------------------------------------
// CCnUiGroupChannel::WriteL()
// -----------------------------------------------------------------------------
//
void CCnUiGroupChannel::WriteL( TInt aMsg, const TDesC& aExtraData )
    {
    //write first the extra data
    WriteExtraDataL( aExtraData );

    //and then signal the channel with message
    WriteL( aMsg );
    }



// -----------------------------------------------------------------------------
// CCnUiGroupChannel::Read()
// -----------------------------------------------------------------------------
//
TInt CCnUiGroupChannel::Read( TInt& aMsg )
    {
    TInt msg = 0;

    if ( iSharedData->GetIntKey( iKey, msg ) == KErrNone )
        {
        aMsg = msg;
        IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiGroupChannel msg[%d] <== [%S]" ),
                    aMsg, &iChannelKey );
        return KErrNone;
        }

    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiGroupChannel [msg not found] <== [%S]" ),
                &iChannelKey );
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CCnUiGroupChannel::ReadL()
// -----------------------------------------------------------------------------
//
void CCnUiGroupChannel::ReadL( HBufC*& aExtraData )
    {
    //read the extra data

    HBufC* shBuffer = HBufC::NewLC( KMaxValueSize );
    TPtr ptr = shBuffer->Des();
    TInt err = iSharedData->GetStringKey( iDataKey, ptr );

    if ( err != KErrNone )
        {
        shBuffer->Des().Copy( KNullDesC() );
        }

    aExtraData = shBuffer->AllocL();

    CleanupStack::PopAndDestroy( shBuffer );

    }



// -----------------------------------------------------------------------------
// CCnUiGroupChannel::SignalL()
// -----------------------------------------------------------------------------
//
TInt CCnUiGroupChannel::SignalL()
    {
    if ( iChannelSignaled )
        {
        return KErrInUse;
        }

    User::LeaveIfError( iSharedData->Signal( iKey ) );

    iChannelSignaled = ETrue;

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CCnUiGroupChannel::SignalL()
// -----------------------------------------------------------------------------
//
TInt CCnUiGroupChannel::SignalL( const TDesC& aExtraData )
    {
    if ( iChannelSignaled )
        {
        return KErrInUse;
        }

    //write first the extra data
    WriteExtraDataL( aExtraData );

    //and then signal the channel
    return SignalL();
    }


// -----------------------------------------------------------------------------
// CCnUiGroupChannel::CancelSignal()
// -----------------------------------------------------------------------------
//
void CCnUiGroupChannel::CancelSignal()
    {
    if ( iChannelSignaled )
        {
        // we can't do anything else except ignore this error
        iSharedData->CancelSignal( iKey );
        iChannelSignaled = EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiGroupChannel::ListenL()
// -----------------------------------------------------------------------------
//
void CCnUiGroupChannel::ListenL( MCnUiGroupChannelListener* aListener )
    {
    __ASSERT_ALWAYS( !iListener, User::Leave( KErrInUse ) );
    __ASSERT_ALWAYS( aListener, User::Leave( KErrArgument ) );

    User::LeaveIfError( iSharedData->SubscribeSet( iUid, iKey ) );
    iListener = aListener;
    }


// -----------------------------------------------------------------------------
// CCnUiGroupChannel::CancelListen()
// -----------------------------------------------------------------------------
//
void CCnUiGroupChannel::CancelListen()
    {
    if ( iSharedData )
        {
        iSharedData->UnSubscribe( iUid, iKey );
        }
    iListener = NULL;
    }


// -----------------------------------------------------------------------------
// CCnUiGroupChannel::GroupID()
// -----------------------------------------------------------------------------
//
TInt CCnUiGroupChannel::GroupID()
    {
    return iGroupId;
    }


// -----------------------------------------------------------------------------
// CCnUiGroupChannel::ChannelId()
// -----------------------------------------------------------------------------
//
TGCChannelID CCnUiGroupChannel::ChannelId()
    {
    return iChannelId;
    }


// -----------------------------------------------------------------------------
// CCnUiGroupChannel::WriteExtraDataL()
// -----------------------------------------------------------------------------
//
void CCnUiGroupChannel::WriteExtraDataL( const TDesC& aExtraData )
    {
    User::LeaveIfError( iSharedData->SetStringKey( iDataKey, aExtraData ) );
    }


// ---------------------------------------------------------
// CCnUiGroupChannel::HandleTemporaryKeyNotifyL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCnUiGroupChannel::HandleTemporaryKeyNotifyL( const TUid aUid,
                                                   const TIMPSSharedKeys aKey  )
    {
    if ( ( aUid != iUid ) || ( aKey != iKey ) )
        {
        return;
        }

    //Notify came trough the channel key
    //ignore key value corruptions, key value will be
    //rewritten on next client specific change

    TInt receivedMsg( 0 );
    TInt error = iSharedData->GetIntKey( aKey, receivedMsg );

    if ( error == KErrNone )
        {
        IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiGroupChannel [%S] ==> event[%d] " ),
                    &iChannelKey, receivedMsg );

        if ( iListener )
            {
            iListener->HandleChannelMsg( iGroupId, iChannelId, receivedMsg );
            }
        }

#ifdef IMPSCUI_ENABLE_DEBUG_PRINT
    else
        {
        IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiGroupChannel [%S] ==> event conversion error [%d] " ),
                    &iChannelKey, error );
        }
#endif //IMPSCUI_ENABLE_DEBUG_PRINT
    }

// ---------------------------------------------------------
// CCnUiGroupChannel::HandlePermanentKeyNotifyL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCnUiGroupChannel::HandlePermanentKeyNotifyL( const TUid aUid,
                                                   const TIMPSSharedKeys aKey  )
    {
    if ( ( aUid != iUid ) || ( aKey != iKey ) )
        {
        return;
        }

    //Notify came trough the channel key
    //ignore key value corruptions, key value will be
    //rewritten on next client specific change

    TInt receivedMsg( 0 );
    TInt error = iSharedData->GetIntKey( aKey, receivedMsg );

    if ( error == KErrNone )
        {
        IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiGroupChannel [%S] ==> event[%d] " ),
                    &iChannelKey, receivedMsg );

        if ( iListener )
            {
            iListener->HandleChannelMsg( iGroupId, iChannelId, receivedMsg );
            }
        }

#ifdef IMPSCUI_ENABLE_DEBUG_PRINT
    else
        {
        IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiGroupChannel [%S] ==> event conversion error [%d] " ),
                    &iChannelKey, error );
        }
#endif //IMPSCUI_ENABLE_DEBUG_PRINT
    }

// end of file
