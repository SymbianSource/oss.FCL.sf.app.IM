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
* Description:   Implementation for Settings Interface for getting/setting tonefilename etc
*
*/



// INCLUDE FILES

#include <PathInfo.h>
#include <bautils.h>
#include <eikenv.h>
#include <cimpspresenceconnectionuing.h>
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsList.h>
#include <CPEngNWSessionSlotID2.h>


#include "ChatDefinitions.h"
#include "ChatDebugPrint.h"
#include "CCACommandSettingSapExtCmd.h"



//----------------------------------------------------------------------------
// .CCACommandSettingSapExtCmd::NewLC
// (other items were commented in a header).
//----------------------------------------------------------------------------

CCACommandSettingSapExtCmd* CCACommandSettingSapExtCmd::NewLC(
    MCASettingsPC* aSettingsPC,
    TBool aPreserveSessionIdInExit,/*= EFalse*/
    CIMPSSAPSettings* aSAPSettings /*= NULL*/ )
    {
    CCACommandSettingSapExtCmd* self = new( ELeave ) CCACommandSettingSapExtCmd(
        aPreserveSessionIdInExit,
        aSettingsPC );
    CleanupStack::PushL( self );
    self->ConstructL( aSAPSettings );
    return self;
    }


//----------------------------------------------------------------------------
// CCACommandSettingSapExtCmd::NewL()
// Two-phased constructor.
//----------------------------------------------------------------------------

CCACommandSettingSapExtCmd* CCACommandSettingSapExtCmd::NewL(
    MCASettingsPC* aSettingsPC,
    TBool aPreserveSessionIdInExit,/*= EFalse*/
    CIMPSSAPSettings* aSAPSettings /*= NULL*/ )
    {
    CCACommandSettingSapExtCmd* self =
        CCACommandSettingSapExtCmd::NewLC( aSettingsPC,
                                           aPreserveSessionIdInExit,
                                           aSAPSettings );
    CleanupStack::Pop( self );
    return self;
    }


//----------------------------------------------------------------------------
// CCACommandSettingSapExtCmd::ConstructL()
// Symbian 2nd phase constructor can leave.
//----------------------------------------------------------------------------

void CCACommandSettingSapExtCmd::ConstructL( CIMPSSAPSettings* aSAPSettings )
    {
    iSAPSettings = aSAPSettings;
    iConnectionUI = CIMPSPresenceConnectionUi::NewL( EIMPSConnClientIM );
    iSAPSettingsStore = CIMPSSAPSettingsStore::NewL();

    }

//----------------------------------------------------------------------------
// CCACommandSettingSapExtCmd::CCACommandSettingSapExtCmd
// C++ default constructor can NOT contain any code, that
// might leave.
//----------------------------------------------------------------------------


CCACommandSettingSapExtCmd::CCACommandSettingSapExtCmd(
    TBool aPreserveSessionIdInExit,
    MCASettingsPC* aSettingsPC )
        : iSettingsPC( aSettingsPC ),
        iPreserveSessionIdInExit( aPreserveSessionIdInExit )
    {

    }

//----------------------------------------------------------------------------
// CCACommandSettingSapExtCmd::~CCACommandSettingSapExtCmd
// Destructor
//----------------------------------------------------------------------------

CCACommandSettingSapExtCmd::~CCACommandSettingSapExtCmd()
    {
    if ( iPreserveSessionIdInExit && iConnectionUI )
        {
        TRAPD( err,
            {
            // this class was invoked from IM Launcher call chain,
            // when Always Online is active. now make sure the IMPSCUI
            // Session Id is given to an instance existing in Always Online
            // (otherwise it'll be deleted and logout will happen)
            CPEngNWSessionSlotID2* sessionSlotID =
            iConnectionUI->GetActiveNWSessionSlotIDL(
                EIMPSConnClientIM );
            CleanupStack::PushL( sessionSlotID );
            iConnectionUI->HandleApplicationExitL( EIMPSConnClientIM,
                                                   EIMPSLeaveSessionOpenExit,
                                                   *sessionSlotID );
            CleanupStack::PopAndDestroy( sessionSlotID );
            } );    // TRAPD

        if ( err != KErrNone )
            {
            CActiveScheduler::Current()->Error( err );
            }
        // now it's safe to exit even in Always Online case
        }
    if ( iConnectionUI )
        {
        delete iConnectionUI;
        }

    if ( iSAPSettingsStore )
        {
        delete iSAPSettingsStore;
        }


    }


//----------------------------------------------------------------------------
// CCACommandSettingSapExtCmd::SetIntValueL
// Sets int value to SAP settings
// (other items were commented in a header).
//----------------------------------------------------------------------------

void CCACommandSettingSapExtCmd::SetIntValueL( const TDesC& aKey,
                                               TInt aValue )
    {
    CHAT_DP_TXT( "CCAAppSettingsSAPExt::SetIntValueL" );
    CIMPSSAPSettings* sapSettings = NULL;
    if ( !iSAPSettings )
        {
        sapSettings = StoredSAPLC();
        }
    else
        {
        sapSettings = iSAPSettings;
        }
    if ( sapSettings )
        {
        TInt id( sapSettings->Uid() );
        CHAT_DP( D_CHAT_LIT( "found sap (%d), setting value %d" ), id, aValue );
        sapSettings->SetOpaqueInt( aKey, aValue );

        // update sapSettings
        iSAPSettingsStore->UpdateOldSAPL( sapSettings, id );
        if ( !iSAPSettings )
            {
            CleanupStack::PopAndDestroy( sapSettings );
            }
        }
    }

//----------------------------------------------------------------------------
//  CCACommandSettingSapExtCmd::IntValueL
// Gets int value from SAP settings
// (other items were commented in a header).
//----------------------------------------------------------------------------

TInt CCACommandSettingSapExtCmd::IntValueL( const TDesC& aKey )
    {
    CHAT_DP_TXT( "CCAAppSettingsSAPExt::IntValueL" );
    TInt returnCode( KErrNotFound );
    CIMPSSAPSettings* sapSettings = NULL;
    if ( !iSAPSettings )
        {
        sapSettings = StoredSAPLC();
        }
    else
        {
        sapSettings = iSAPSettings;
        }
    if ( sapSettings )
        {
#ifdef _DEBUG
        TInt id( sapSettings->Uid() );
        CHAT_DP( D_CHAT_LIT( "found sap (%d), getting value.." ), id );
#endif//_DEBUG 
        sapSettings->GetOpaqueInt( aKey, returnCode );
        if ( !iSAPSettings )
            {
            CleanupStack::PopAndDestroy( sapSettings );
            }
        }
    CHAT_DP( D_CHAT_LIT( "value %d" ), returnCode );
    return returnCode;
    }


//----------------------------------------------------------------------------
//  CCACommandSettingSapExtCmd::SetToneFileNameL
// (other items were commented in a header).
//----------------------------------------------------------------------------

void CCACommandSettingSapExtCmd::SetToneFileNameL( const TDesC& aToneFileName )
    {
    CIMPSSAPSettings* sapSettings = NULL;
    if ( !iSAPSettings )
        {
        sapSettings = StoredSAPLC();
        }
    else
        {
        sapSettings = iSAPSettings;
        }

    // populate the list of sapSettings
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    iSAPSettingsStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

    // and find our sapSettings.. because logged in SAP has UID of zero, we must
    // find the correct UID manually from list
    TInt index( KErrNotFound );
    sapList->FindNameL( sapSettings->SAPName(), index );

    if ( index != KErrNotFound )
        {
        // found it, update the correct sap
        CIMPSSAPSettings* storedSAPSettings = CIMPSSAPSettings::NewLC();

        TUint32 sapUid = sapList->UidForIndex( index );
        iSAPSettingsStore->GetSAPL( sapUid, storedSAPSettings );
        storedSAPSettings->SetOpaqueDesC16( KIMAlertTonePath(), aToneFileName );
        iSAPSettingsStore->UpdateOldSAPL( storedSAPSettings, sapUid );
        CleanupStack::PopAndDestroy( storedSAPSettings );
        }
    if ( !iSAPSettings )
        {
        CleanupStack::PopAndDestroy( 2, sapSettings );
        }
    else
        {
        CleanupStack::PopAndDestroy( sapList );
        }
    }


//----------------------------------------------------------------------------
//  CCACommandSettingSapExtCmd::GetToneFileNameL
// (other items were commented in a header).
//----------------------------------------------------------------------------

void CCACommandSettingSapExtCmd::GetToneFileNameL( TDes& aToneFileName )
    {
    CIMPSSAPSettings* sapSettings = NULL;
    if ( !iSAPSettings )
        {
        sapSettings = StoredSAPLC();
        }
    else
        {
        sapSettings = iSAPSettings;
        }

    // populate the list of sapsettings
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    iSAPSettingsStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

    // and find our sap.. because logged in SAP has UID of zero, we must
    // find the correct UID manually from list
    TInt index( KErrNotFound );
    sapList->FindNameL( sapSettings->SAPName(), index );

    if ( index != KErrNotFound )
        {
        // found it, update the correct sap
        CIMPSSAPSettings* storedSAPSettings = CIMPSSAPSettings::NewLC();
        iSAPSettingsStore->GetSAPL( sapList->UidForIndex( index ),
                                    storedSAPSettings );

        TPtrC audioPath;
        storedSAPSettings->GetOpaqueDesC16( KIMAlertTonePath(),
                                            audioPath );
        
        if ( BaflUtils::FileExists( CEikonEnv::Static()->FsSession(), audioPath ) )
        	{
        aToneFileName.Copy( audioPath );
        	}
        else
        	{
        	aToneFileName.Copy( KNullDesC() );
        	}
        
        CleanupStack::PopAndDestroy( storedSAPSettings );
        }
    else
        {
        aToneFileName.Copy( KNullDesC() );
        }

    if ( !iSAPSettings )
        {
        CleanupStack::PopAndDestroy( 2, sapSettings );
        }
    else
        {
        CleanupStack::PopAndDestroy( sapList );
        }
    }


//----------------------------------------------------------------------------
// CCACommandSettingSapExtCmd::StoredSAPLC
// Gets editable stored SAP
// (other items were commented in a header).
//----------------------------------------------------------------------------

CIMPSSAPSettings* CCACommandSettingSapExtCmd::StoredSAPLC()
    {
    CIMPSSAPSettings* sapSettings = CIMPSSAPSettings::NewLC();

    TBool isLoggedIn( EFalse );
    if ( iConnectionUI )
        {
        TInt err;
        TRAP( err, isLoggedIn = iConnectionUI->LoggedInL( EIMPSConnClientIM ) );
        }

    if ( isLoggedIn )
        {
        CPEngNWSessionSlotID2* sessionSlotId =
            iConnectionUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
        CleanupStack::PushL( sessionSlotId );
        iConnectionUI->GetLoggedInSapL( *sessionSlotId, *sapSettings );

        // populate the list of sapsettings
        CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
        iSAPSettingsStore->PopulateSAPSettingsListL( *sapList,
                                                     EIMPSIMAccessGroup );

        // and find our sap.. because logged in SAP has UID of zero, we must
        // find the correct UID manually from list
        TInt index( KErrNotFound );
        TPtrC name( sapSettings->SAPName() );
        CHAT_DP( D_CHAT_LIT( "logged to to %S, trying to find sapSettings" ),
                 &name );
        sapList->FindNameL( name, index );

        if ( index != KErrNotFound )
            {
            CHAT_DP( D_CHAT_LIT( "found it in index %d" ), index );
            // found it, update the correct sap
            CIMPSSAPSettings* storedSAPSettings = CIMPSSAPSettings::NewLC();
            iSAPSettingsStore->GetSAPL( sapList->UidForIndex( index ),
                                        storedSAPSettings );
            CleanupStack::Pop( storedSAPSettings );
            CleanupStack::PopAndDestroy( 3, sapSettings );
            // sapList, sessionslot, sapSettings
            CleanupStack::PushL( storedSAPSettings );
            sapSettings = storedSAPSettings;
            }
        else
            {
            CHAT_DP( D_CHAT_LIT( "not found" ) );
            CleanupStack::PopAndDestroy( 3, sapSettings );
            // sapList, sessionSlotId, sapSettings
            return NULL;
            }
        }
    else if ( iSAPSettingsStore )
        {
        iSAPSettingsStore->GetDefaultL( sapSettings, EIMPSIMAccessGroup );
        }

    return sapSettings;
    }


//----------------------------------------------------------------------------
// CCACommandSettingSapExtCmd::CurrentSAPLC
// (other items were commented in a header).
//----------------------------------------------------------------------------

CIMPSSAPSettings* CCACommandSettingSapExtCmd::CurrentSAPLC()
    {
    CIMPSSAPSettings* sapSettings = CIMPSSAPSettings::NewLC();

    TBool isLoggedIn( EFalse );
    TInt err;
    TRAP( err, isLoggedIn = iConnectionUI->LoggedInL( EIMPSConnClientIM ) );

    if ( isLoggedIn )
        {
        CPEngNWSessionSlotID2* sessionSlotId =
            iConnectionUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
        CleanupStack::PushL( sessionSlotId );
        iConnectionUI->GetLoggedInSapL( *sessionSlotId, *sapSettings );
        CleanupStack::PopAndDestroy( sessionSlotId );
        }
    else
        {
        iSAPSettingsStore->GetDefaultL( sapSettings, EIMPSIMAccessGroup );
        }
    return sapSettings;
    }

