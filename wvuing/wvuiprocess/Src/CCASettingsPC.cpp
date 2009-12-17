/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Process Component for Settings
*
*/



// INCLUDE FILES
#include "CCASettingsPC.h"
#include <CPEngNWSessionSlotID2.h>
#include "CCAEngine.h"
#include "PublicEngineDefinitions.h"
#include "MCASettings.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCASettingsPC::NewL
// -----------------------------------------------------------------------------
//
CCASettingsPC* CCASettingsPC::NewL( CCAEngine& aEngine )
    {

    CCASettingsPC* self = new ( ELeave ) CCASettingsPC( aEngine );

    return self;

    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCASettingsPC::~CCASettingsPC()
    {

    }

// -----------------------------------------------------------------------------
// CCASettingsPC::CCASettingsPC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCASettingsPC::CCASettingsPC( CCAEngine& aEngine )
        : iEngine( aEngine ), iSettings( iEngine.GetInternalSettingsInterface() )
    {

    }

// -----------------------------------------------------------------------------
// CCASettingsPC::SetDefaultSapL
// -----------------------------------------------------------------------------
//
void CCASettingsPC::SetDefaultSapL( CIMPSSAPSettings* aSap )
    {

    iSettings->SetDefaultSapL( aSap );

    }

// -----------------------------------------------------------------------------
// CCASettingsPC::SetValueL
// -----------------------------------------------------------------------------
//
void CCASettingsPC::SetValueL( TEnumsPC::TCASettingValues aSetting, TBool aValue )
    {
    //switch case used coz there may be need to add other cases
    switch ( aSetting )
        {
        case TEnumsPC::EAutomaticPresenceUpdate:
            {
            iSettings->SetValueL( MCASettings::EAutomaticPresenceUpdate, aValue );
            break;
            }


        default:
            {
            break;
            }

        }

    }

//------------------------------------------------------------------------------------
//  CCASettingsPC::GetBoolValuePC
//   gets SapSettings values
//------------------------------------------------------------------------------------

TBool CCASettingsPC::GetBoolValuePC( TEnumsPC::TCASettingValues aSetting, CIMPSSAPSettings* aSap )
    {
    TBool value = EFalse;
    switch ( aSetting )
        {
        case TEnumsPC::EDefaultScreenNameInUse:
            {
            value =  iSettings->Value( MCASettings::EDefaultScreenNameInUse, aSap );
            break;
            }


        case TEnumsPC::EShowTimeStamps:
            {
            value =  iSettings->Value( MCASettings::EShowTimeStamps, aSap );
            break;
            }

        case TEnumsPC::EAutomaticPresenceUpdate:
            {
            value =  iSettings->Value( MCASettings::EAutomaticPresenceUpdate, aSap );
            break;
            }

        case TEnumsPC::EShowOffline:
            {
            value =  iSettings->Value( MCASettings::EShowOffline, aSap );
            break;
            }

        default:
            {
            break;    //EFalse;
            }

        }

    return value;
    }

//------------------------------------------------------------------------------------
// CCASettingsPC::GetIntValuePC
// gets SapSettings values
//------------------------------------------------------------------------------------
TInt CCASettingsPC::GetIntValuePC( TEnumsPC::TCASettingValues aSetting, CIMPSSAPSettings* aSap )
    {
    TInt intValue = KErrNone;
    switch ( aSetting )
        {
        case TEnumsPC::EAuthorizeIMPresence:
            {
            intValue =  iSettings->Value( MCASettings::EAuthorizeIMPresence, aSap );
            break;
            }

        case TEnumsPC::EReceiveIMessages:
            {
            intValue = iSettings->Value( MCASettings::EReceiveIMessages, aSap );
            break;
            }

        case TEnumsPC::EReceiveInvitations:
            {
            intValue = iSettings->Value( MCASettings::EReceiveInvitations, aSap );
            break;
            }

        case TEnumsPC::EMessageFlowSettingLevel:
            {
            intValue = iSettings->Value( MCASettings::EMessageFlowSettingLevel, aSap );
            break;
            }

        case TEnumsPC::EFriendsListOrdering:
            {
            intValue = iSettings->Value( MCASettings::EFriendsListOrdering, aSap );
            break;
            }

        default:
            {
            break;
            }

        }

    return intValue;
    }
//------------------------------------------------------------------------------------
// CCASettingsPC::IsAlphabeticalPC
// checks for ordering of IM contacts
//------------------------------------------------------------------------------------
TBool CCASettingsPC::IsAlphabeticalPC(  CIMPSSAPSettings* aSap )
    {
    TInt Isalpha = iSettings->Value( MCASettings::EFriendsListOrdering, aSap );
    if ( Isalpha == MCASettings::EAlphabetical )
        {
        return ETrue;
        }
    return EFalse;
    }

//----------------------------------------------------------------------------------
// CCASettingsPC ::GetSapSettingValuePC
// gets the default screenname
//-----------------------------------------------------------------------------------
HBufC* CCASettingsPC ::GetSapSettingValuePCL(
    TEnumsPC::TCASettingStrings aSetting,
    CIMPSSAPSettings* aSap  )

    {
    switch ( aSetting )
        {
        case TEnumsPC::EDefaultScreenName:
            {
            return iSettings->ValueL( MCASettings::EDefaultScreenName, aSap );
            }

        case TEnumsPC::EOwnAlias:
            {
            return iSettings->OwnAliasL();
            }
        case TEnumsPC::EStatusMsgOnline:
            {
            return iSettings->ValueL( MCASettings::EStatusMsgOnline, aSap );
            }
        case TEnumsPC::EStatusMsgAway:
            {
            return iSettings->ValueL( MCASettings::EStatusMsgAway, aSap );
            }
        case TEnumsPC::EStatusMsgBusy:
            {
            return iSettings->ValueL( MCASettings::EStatusMsgBusy, aSap );
            }
        case TEnumsPC::EOwnWVUserID:
            {
            return iSettings->ValueL( MCASettings::EOwnWVUserID, aSap );
            }

        default:
            {
            return NULL;
            }
        }
    }

//----------------------------------------------------------------------------------
// CCASettingsPC::SetBoolValuePCL
// sets the SapSettings values
//----------------------------------------------------------------------------------
void CCASettingsPC::SetBoolValuePCL(
    TEnumsPC::TCASettingValues aSetting,
    TBool aValue,
    RArray<TInt>* aIgnoreRollback,
    CIMPSSAPSettings* aSap )
    {
    switch ( aSetting )
        {
        case TEnumsPC::EDefaultScreenNameInUse:
            {
            iSettings->SetValueL( MCASettings::EDefaultScreenNameInUse,
                                  aValue, aIgnoreRollback , aSap );
            break;
            }
        case TEnumsPC::EShowTimeStamps:
            {
            iSettings->SetValueL( MCASettings::EShowTimeStamps, aValue,
                                  aIgnoreRollback , aSap );
            break;
            }
        case TEnumsPC::EAutomaticPresenceUpdate:
            {
            iSettings->SetValueL( MCASettings::EAutomaticPresenceUpdate,
                                  aValue, aIgnoreRollback , aSap );
            break;
            }
        case TEnumsPC::EShowOffline:
            {
            iSettings->SetValueL( MCASettings::EShowOffline, aValue,
                                  aIgnoreRollback , aSap );
            break;
            }
        default:
            {
            //return;
            break;
            }
        }
    }

//------------------------------------------------------------------------------------
// CCASettingsPC::SetIntValuePCL
// sets the SapSettings values
//------------------------------------------------------------------------------------

void CCASettingsPC::SetIntValuePCL(
    TEnumsPC::TCASettingValues aSetting,
    TInt aValue,
    RArray<TInt>* aIgnoreRollback,
    CIMPSSAPSettings* aSap )
    {
    switch ( aSetting )
        {
        case TEnumsPC::EAuthorizeIMPresence:
            {
            iSettings->SetValueL( MCASettings::EAuthorizeIMPresence,
                                  aValue, aIgnoreRollback, aSap );
            break;
            }
        case TEnumsPC::EReceiveIMessages:
            {
            iSettings->SetValueL( MCASettings::EReceiveIMessages,
                                  aValue, aIgnoreRollback, aSap );
            break;
            }
        case TEnumsPC::EReceiveInvitations:
            {
            iSettings->SetValueL( MCASettings::EReceiveInvitations,
                                  aValue, aIgnoreRollback, aSap );
            break;
            }
        case TEnumsPC::EMessageFlowSettingLevel:
            {
            iSettings->SetValueL( MCASettings::EMessageFlowSettingLevel,
                                  aValue, aIgnoreRollback, aSap );
            break;
            }
        case TEnumsPC::EFriendsListOrdering:
            {
            iSettings->SetValueL( MCASettings::EFriendsListOrdering,
                                  aValue, aIgnoreRollback, aSap );
            break;
            }
        default:
            {
            //return
            break;
            }
        }

    }

//------------------------------------------------------------------------------------
// CCASettingsPC::SetSapSettingValuePCL
// sets the DefaultScreenName
//------------------------------------------------------------------------------------

void  CCASettingsPC::SetSapSettingValuePCL(
    TEnumsPC::TCASettingStrings aSetting,
    const TDesC& aValue,
    RArray<TInt>* aIgnoreRollback ,
    CIMPSSAPSettings* aSap  )

    {
    switch ( aSetting )
        {
        case TEnumsPC::EDefaultScreenName:
            {
            iSettings->SetValueL( MCASettings::EDefaultScreenName,
                                  aValue, aIgnoreRollback, aSap );
            break;
            }

        case TEnumsPC::EOwnAlias:
            {
            iSettings->SetOwnAliasL( aValue );
            break;
            }
        case TEnumsPC::EStatusMsgOnline:
            {
            iSettings->SetValueL( MCASettings::EStatusMsgOnline,
                                  aValue, aIgnoreRollback, aSap );
            break;
            }
        case TEnumsPC::EStatusMsgAway:
            {
            iSettings->SetValueL( MCASettings::EStatusMsgAway,
                                  aValue, aIgnoreRollback, aSap );
            break;
            }
        case TEnumsPC::EStatusMsgBusy:
            {
            iSettings->SetValueL( MCASettings::EStatusMsgBusy,
                                  aValue, aIgnoreRollback, aSap );
            break;
            }
        default:
            {
            break;
            }
        }
    }

//----------------------------------------------------------------------------
// CCASettingsPC:: FlushDataPC
//----------------------------------------------------------------------------


void CCASettingsPC:: FlushDataPC()
    {
    iSettings->FlushData();
    }
//    End of File












