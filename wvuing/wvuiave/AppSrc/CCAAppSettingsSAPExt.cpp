/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Application settings SAP settings extension
*
*/



// INCLUDE FILES
#include "CCAAppSettingsSAPExt.h"
#include "ChatDefinitions.h"
#include "ChatDebugPrint.h"

#include <cimpspresenceconnectionuing.h>
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsList.h>
#include <CPEngNWSessionSlotID2.h>
#include <PathInfo.h>
#include <bautils.h>
#include <eikenv.h>

// ============================ MEMBER FUNCTIONS ===============================

//Removed the ownership of iConnUI from this file and made it a reference
//APPUI owns this connection UI instance and shares the same with this component.


// -----------------------------------------------------------------------------
// CCAAppSettingsSAPExt::CCAAppSettingsSAPExt
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAAppSettingsSAPExt::CCAAppSettingsSAPExt( CIMPSPresenceConnectionUi& aConnUi,
                                            TBool aPreserveSessionIdInExit )
        :	iConnUI( aConnUi ),
        iPreserveSessionIdInExit( aPreserveSessionIdInExit )
    {
    }

// -----------------------------------------------------------------------------
// CCAAppSettingsSAPExt::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAAppSettingsSAPExt::ConstructL(  CIMPSSAPSettings* aSap )
    {
    iSapSettings = CIMPSSAPSettingsStore::NewL();
    iSap = aSap;
    }

// -----------------------------------------------------------------------------
// CCAAppSettingsSAPExt::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAAppSettingsSAPExt* CCAAppSettingsSAPExt::NewL( CIMPSPresenceConnectionUi& aConnUi,
                                                  TBool aPreserveSessionIdInExit /* = EFalse */,
                                                  CIMPSSAPSettings* aSap /* = NULL */ )

    {
    CCAAppSettingsSAPExt* self = CCAAppSettingsSAPExt::NewLC( aConnUi,
                                                              aPreserveSessionIdInExit, aSap );

    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCAAppSettingsSAPExt::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAAppSettingsSAPExt* CCAAppSettingsSAPExt::NewLC( CIMPSPresenceConnectionUi& aConnUi,
                                                   TBool aPreserveSessionIdInExit /* = EFalse */,
                                                   CIMPSSAPSettings* aSap /* = NULL */ )
    {
    CCAAppSettingsSAPExt* self = new( ELeave ) CCAAppSettingsSAPExt( aConnUi,
                                                                     aPreserveSessionIdInExit );
    CleanupStack::PushL( self );
    self->ConstructL( aSap );
    return self;
    }


// Destructor
// CodeScanner warning ignored because HandleApplicationExitL
// needs to be called to keep connection alive.
// Otherwise destroying iConnUI would close the connection.
CCAAppSettingsSAPExt::~CCAAppSettingsSAPExt()   // CSI: 23 # See comment above
    {
    if ( iPreserveSessionIdInExit )
        {
        TRAPD( err,
            {
            // this class was invoked from IM Launcher call chain,
            // when Always Online is active. now make sure the IMPSCUI
            // Session Id is given to an instance existing in Always Online
            // (otherwise it'll be deleted and logout will happen)
            CPEngNWSessionSlotID2* sessionSlotID =
            iConnUI.GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
            CleanupStack::PushL( sessionSlotID );
            iConnUI.HandleApplicationExitL( EIMPSConnClientIM,
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

    delete iSapSettings;
    }


// -----------------------------------------------------------------------------
// CCAAppSettingsSAPExt::SetIntValueL
// Sets int value to SAP settings
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppSettingsSAPExt::SetIntValueL( const TDesC& aKey, TInt aValue )
    {
    CHAT_DP_TXT( "CCAAppSettingsSAPExt::SetIntValueL" );
    CIMPSSAPSettings* sap = NULL;
    if ( !iSap )
        {
        sap = StoredSAPLC();
        }
    else
        {
        sap = iSap;
        }
    if ( sap )
        {
        TInt id( sap->Uid() );
        CHAT_DP( D_CHAT_LIT( "found sap (%d), setting value %d" ), id, aValue );
        sap->SetOpaqueInt( aKey, aValue );

        // update sap
        iSapSettings->UpdateOldSAPL( sap, id );
        if ( !iSap )
            {
            CleanupStack::PopAndDestroy( sap );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAAppSettingsSAPExt::IntValueL
// Gets int value from SAP settings
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAAppSettingsSAPExt::IntValueL( const TDesC& aKey )
    {
    CHAT_DP_TXT( "CCAAppSettingsSAPExt::IntValueL" );
    TInt ret( KErrNotFound );
    CIMPSSAPSettings* sap = NULL;
    if ( !iSap )
        {
        sap = StoredSAPLC();
        }
    else
        {
        sap = iSap;
        }
    if ( sap )
        {
#ifdef _DEBUG
        TInt id( sap->Uid() );
        CHAT_DP( D_CHAT_LIT( "found sap (%d), getting value.." ), id );
#endif//_DEBUG 
        sap->GetOpaqueInt( aKey, ret );
        if ( !iSap )
            {
            CleanupStack::PopAndDestroy( sap );
            }
        }
    CHAT_DP( D_CHAT_LIT( "value %d" ), ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CCAAppSettingsSAPExt::SetToneFileNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppSettingsSAPExt::SetToneFileNameL( const TDesC& aToneFileName )
    {
    CIMPSSAPSettings* sap = NULL;
    if ( !iSap )
        {
        sap = StoredSAPLC();
        }
    else
        {
        sap = iSap;
        }

    // populate the list of sapsettings
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    iSapSettings->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

    // and find our sap.. because logged in SAP has UID of zero, we must
    // find the correct UID manually from list
    TInt index( KErrNotFound );
    sapList->FindNameL( sap->SAPName(), index );

    if ( index != KErrNotFound )
        {
        // found it, update the correct sap
        CIMPSSAPSettings* storedSap = CIMPSSAPSettings::NewLC();

        TUint32 sapUid = sapList->UidForIndex( index );
        iSapSettings->GetSAPL( sapUid, storedSap );
        storedSap->SetOpaqueDesC16( KIMAlertTonePath(), aToneFileName );
        iSapSettings->UpdateOldSAPL( storedSap, sapUid );
        CleanupStack::PopAndDestroy( storedSap );
        }
    if ( !iSap )
        {
        CleanupStack::PopAndDestroy( 2, sap );
        }
    else
        {
        CleanupStack::PopAndDestroy( sapList );
        }
    }

// -----------------------------------------------------------------------------
// CCAAppSettingsSAPExt::GetToneFileNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAAppSettingsSAPExt::GetToneFileNameL( TDes& aToneFileName )
    {
    CIMPSSAPSettings* sap = NULL;
    if ( !iSap )
        {
        sap = StoredSAPLC();
        }
    else
        {
        sap = iSap;
        }

    // populate the list of sapsettings
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    iSapSettings->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

    // and find our sap.. because logged in SAP has UID of zero, we must
    // find the correct UID manually from list
    TInt index( KErrNotFound );
    sapList->FindNameL( sap->SAPName(), index );

    if ( index != KErrNotFound )
        {
        // found it, update the correct sap
        CIMPSSAPSettings* storedSap = CIMPSSAPSettings::NewLC();
        iSapSettings->GetSAPL( sapList->UidForIndex( index ), storedSap );

        TPtrC audioPath;
        storedSap->GetOpaqueDesC16( KIMAlertTonePath(), audioPath );
        aToneFileName.Copy( audioPath.Left( aToneFileName.MaxLength() ) );
        CleanupStack::PopAndDestroy( storedSap );
        }
    else
        {
        aToneFileName.Copy( KNullDesC() );
        }

    if ( !iSap )
        {
        CleanupStack::PopAndDestroy( 2, sap );
        }
    else
        {
        CleanupStack::PopAndDestroy( sapList );
        }
    }



// -----------------------------------------------------------------------------
// CCAAppSettingsSAPExt::StoredSAPLC
// Gets editable stored SAP
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CIMPSSAPSettings* CCAAppSettingsSAPExt::StoredSAPLC()
    {
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();

    TBool isLoggedIn( EFalse );

    TInt err;
    TRAP( err, isLoggedIn = iConnUI.LoggedInL( EIMPSConnClientIM ) );

    if ( isLoggedIn )
        {
        CPEngNWSessionSlotID2* sessionSlotId =
            iConnUI.GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
        CleanupStack::PushL( sessionSlotId );
        iConnUI.GetLoggedInSapL( *sessionSlotId, *sap );

        // populate the list of sapsettings
        CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
        iSapSettings->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

        // and find our sap.. because logged in SAP has UID of zero, we must
        // find the correct UID manually from list
        TInt index( KErrNotFound );
        TPtrC name( sap->SAPName() );
        CHAT_DP( D_CHAT_LIT( "logged to to %S, trying to find sap" ), &name );
        sapList->FindNameL( name, index );

        if ( index != KErrNotFound )
            {
            CHAT_DP( D_CHAT_LIT( "found it in index %d" ), index );
            // found it, update the correct sap
            CIMPSSAPSettings* storedSap = CIMPSSAPSettings::NewLC();
            iSapSettings->GetSAPL( sapList->UidForIndex( index ), storedSap );
            CleanupStack::Pop( storedSap );
            CleanupStack::PopAndDestroy( 3, sap ); // sapList, sessionslot, sap
            CleanupStack::PushL( storedSap );
            sap = storedSap;
            }
        else
            {
            CHAT_DP( D_CHAT_LIT( "not found" ) );
            CleanupStack::PopAndDestroy( 3, sap ); // sapList, sessionSlotId, sap
            return NULL;
            }
        }
    else if ( iSapSettings )
        {
        iSapSettings->GetDefaultL( sap, EIMPSIMAccessGroup );
        }

    return sap;
    }

// -----------------------------------------------------------------------------
// CCAAppSettingsSAPExt::CurrentSAPLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CIMPSSAPSettings* CCAAppSettingsSAPExt::CurrentSAPLC()
    {
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();

    TBool isLoggedIn( EFalse );
    TInt err;
    TRAP( err, isLoggedIn = iConnUI.LoggedInL( EIMPSConnClientIM ) );

    if ( isLoggedIn )
        {
        CPEngNWSessionSlotID2* sessionSlotId = iConnUI.GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
        CleanupStack::PushL( sessionSlotId );
        iConnUI.GetLoggedInSapL( *sessionSlotId, *sap );
        CleanupStack::PopAndDestroy( sessionSlotId );
        }
    else
        {
        iSapSettings->GetDefaultL( sap, EIMPSIMAccessGroup );
        }
    return sap;
    }

//  End of File
