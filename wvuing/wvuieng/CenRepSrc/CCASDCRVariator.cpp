/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Persistent preference store variation
*
*/


//  Include Files
#include "CCASDCRVariator.h"
#include "MCASettings.h"
#include "imnginternalcrkeys.h"
#include "IMPrivateCRKeys.h"
#include "IMInternalPSKeys.h"
#include "ChatDebugPrint.h"

#include <e32property.h>
#include <centralrepository.h>

_LIT_SECURITY_POLICY_S0( KIMWritePolicy, 0x101F4673 );
_LIT_SECURITY_POLICY_S0( KIMReadPolicy, 0x101F4673 );

// CONSTRUCTION

// -----------------------------------------------------------------------------
// CCASDCRVariator::NewL
// Static constructor. Identifier ignored with central repository.
// -----------------------------------------------------------------------------
//
CCASDCRVariator* CCASDCRVariator::NewL( TUid aIdentifier )
    {
    CCASDCRVariator* self = new ( ELeave ) CCASDCRVariator;
    CleanupStack::PushL( self );
    self->ConstructL( aIdentifier );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCASDCRVariator::~CCASDCRVariator
// Destructor (virtual by CBase)
// -----------------------------------------------------------------------------
//
CCASDCRVariator::~CCASDCRVariator()
    {
    CHAT_DP_FUNC_ENTER( "~CCASDCRVariator" );
    delete iCenRep;
    }

// -----------------------------------------------------------------------------
// CCASDCRVariator::ConstructL
// Second phase construct
// -----------------------------------------------------------------------------
//
void CCASDCRVariator::ConstructL( TUid /* aIdentifier */ )
    {
    CHAT_DP_FUNC_ENTER( "ConstructL - Central repository" );
    iCenRep = CRepository::NewL( KCRUidIM );
    CHAT_DP_FUNC_DONE( "ConstructL" );
    }

// -----------------------------------------------------------------------------
// CCASDCRVariator::CCASDCRVariator
// Constructor
// -----------------------------------------------------------------------------
//
CCASDCRVariator::CCASDCRVariator()
    {
    // empty
    }

// -----------------------------------------------------------------------------
// CCASDCRVariator::GetInt
// -----------------------------------------------------------------------------
//
TInt CCASDCRVariator::GetInt( TInt aSetting, TInt& aValue )
    {
    CHAT_DP_FUNC_ENTER( "GetInt" );
    TInt key = GenerateKey( aSetting );
    return iCenRep->Get( key, aValue );
    }

// -----------------------------------------------------------------------------
// CCASDCRVariator::SetInt
// -----------------------------------------------------------------------------
//
TInt CCASDCRVariator::SetInt( TInt aSetting, TInt aValue )
    {
    CHAT_DP_FUNC_ENTER( "SetInt" );
    TInt key = GenerateKey( aSetting );
    return iCenRep->Set( key, aValue );
    }

// -----------------------------------------------------------------------------
// CCASDCRVariator::FlushData
// -----------------------------------------------------------------------------
//
TInt CCASDCRVariator::FlushData()
    {
    CHAT_DP_FUNC_ENTER( "FlushData" );
    // in central repository the data is written immediately,
    // no need to flush
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCASDCRVariator::GetString
// -----------------------------------------------------------------------------
//
TInt CCASDCRVariator::GetString( TInt aSetting, TDes& aValue )
    {
    CHAT_DP_FUNC_ENTER( "GetString" );
    TInt key = GenerateKey( aSetting );

    switch ( aSetting )
        {
        case MCASettings::EServiceAddress:
            {
            return RProperty::Get( KPSUidIMEngine, KIMServiceAddress, aValue );
            }
        default:
            {
            return iCenRep->Get( key, aValue );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCASDCRVariator::SetString
// -----------------------------------------------------------------------------
//
TInt CCASDCRVariator::SetString( TInt aSetting, const TDesC& aValue )
    {
    CHAT_DP_FUNC_ENTER( "SetString" );
    TInt key = GenerateKey( aSetting );

    switch ( aSetting )
        {
        case MCASettings::EServiceAddress:
            {
            RProperty::Define( KPSUidIMEngine, KIMServiceAddress,
                               RProperty::EText, KIMReadPolicy, KIMWritePolicy );
            return RProperty::Set( KPSUidIMEngine, KIMServiceAddress, aValue );
            }
        default:
            {
            return iCenRep->Set( key, aValue );
            }
        }
    }

// ---------------------------------------------------------
// CCASDCRVariator::GenerateKey
// ---------------------------------------------------------
//
TInt CCASDCRVariator::GenerateKey( const TInt aSettingNumber ) const
    {
    CHAT_DP_FUNC_ENTER( "GenerateKey" );
    // we could use a table but this is just as fast, if not
    // faster. consider it an unrolled loop. e.g. we can't use
    // the aSettingNumber as index, since the enumerations
    // are not consecutive and have different bases where they start
    switch ( aSettingNumber )
        {
        case MCASettings::EFirstLoginDone :
            return KIMFirstLogin;

        case MCASettings::EDefaultScreenNameInUse :
            return KIMScreenNameInUse;

        case MCASettings::EAutomaticPresenceUpdate :
            return KIMAutoPresence;

        case MCASettings::EShowHistory :
            return KIMShowHistory;

        case MCASettings::EAuthorizeIMPresence :
            return KIMAuthPresence;

        case MCASettings::EReceiveIMessages :
            return KIMAuthIM;

        case MCASettings::EReceiveInvitations :
            return KIMAuthInvite;

        case MCASettings::EMessageFlowSettingLevel :
            return KIMMsgFlow;

        case MCASettings::EFriendsListOrdering :
            return KIMOrderFriends;

        case MCASettings::EDefaultScreenName :
            return KIMDefScreenName;

        case MCASettings::EOwnWVUserID :
            return KIMOwnUserId;

        case MCASettings::EOwnAlias :
            return KIMAliasName;

        case MCASettings::EServiceAddress :
            return KIMServiceAddress;

        case MCASettings::EShowOffline :
            return KIMOfflineContacts;

        case MCASettings::EStatusMsgOnline:
            return KIMStatusMsgOnline;

        case MCASettings::EStatusMsgAway:
            return KIMStatusMsgAway;

        case MCASettings::EStatusMsgBusy:
            return KIMStatusMsgBusy;
        default:
            break;
        }

    return -1;
    }


//  End of File
