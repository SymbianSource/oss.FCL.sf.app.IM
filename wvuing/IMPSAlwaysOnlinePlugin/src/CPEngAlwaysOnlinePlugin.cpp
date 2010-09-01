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
* Description:  Implements the CAlwaysOnlineEComInterface interface
*
*/

// INCLUDE FILES
#include "CPEngAlwaysOnlinePlugin.h"
#include "CIMPSAlwaysOnlinePluginImp.h"
#include "IMPSCommonUiDebugPrint.h"
#include "ChatNGCommonUIDs.h"
#include "CPEngAOCallObserver.h"
#include "CIMPSReconnectNoteHandler.h"
#include "IMPSUIDDefs.h"

#include <e32std.h>
#include <ImplementationProxy.h>
#include <alwaysonlinemanagercommon.h>
#include <FeatMgr.h>
#include <coemain.h>
#include <GulUtil.h>
#include <e32property.h>

#include "impscommonuibuilddefinitions.h"
#include "cimpsresourcereader.h"

CPEngAlwaysOnlinePlugin::CPEngAlwaysOnlinePlugin()
        : CAlwaysOnlineEComInterface()
    {
    }

// ----------------------------------------------------------------------------
// CPEngAlwaysOnlinePlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CPEngAlwaysOnlinePlugin::ConstructL()
    {
    IMPSCUI_DP_FUNC_ENTER( "ConstructL" );

    FeatureManager::InitializeLibL();

    CleanupStack::PushL( TCleanupItem( CleanupFeatureManager, this ) );

    if ( FeatureManager::FeatureSupported( KFeatureIdIm ) )
        {
        iIMAOPlugin = CIMPSAlwaysOnlinePluginImp::NewL();

        CIMPSResourceReader* resourceReader = CIMPSResourceReader::NewLC();
        TBool showRoamingWarning = resourceReader->IsRoamingWarningRequiredL();
        if ( showRoamingWarning )
            {
            iIMAOPlugin->SetShowRoamingWarning( showRoamingWarning );
            }
        CleanupStack::PopAndDestroy( resourceReader );
        }

    CleanupStack::PopAndDestroy(); // CleanupFeatureManager

    IMPSCUI_DP_FUNC_DONE( "ConstructL" );
    }

// Destructor
CPEngAlwaysOnlinePlugin::~CPEngAlwaysOnlinePlugin()
    {
    delete iIMAOPlugin;
    }

// ----------------------------------------------------------------------------
// CPEngAlwaysOnlinePlugin::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CPEngAlwaysOnlinePlugin* CPEngAlwaysOnlinePlugin::NewL()
    {
    CPEngAlwaysOnlinePlugin* self = new ( ELeave ) CPEngAlwaysOnlinePlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CPEngAlwaysOnlinePlugin::CleanupFeatureManager
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
void CPEngAlwaysOnlinePlugin::CleanupFeatureManager( TAny* /*aPtr*/ )
    {
    FeatureManager::UnInitializeLib();
    }
// ---------------------------------------------------------
// CPEngAlwaysOnlinePlugin::HandleServerCommandL
// Handles the events coming from the Always Online Server
// ---------------------------------------------------------
//
TAny* CPEngAlwaysOnlinePlugin::HandleServerCommandL( TInt aCommand,
                                                     TDesC8* /*aParameters*/ )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CPEngAlwaysOnlinePlugin::HandleServerCommandL [%d]" ), aCommand );
    TInt err( KErrNone );

    switch ( aCommand )
        {
        case EAOManagerPluginStart:
            {
            //do your plugin start here
            if ( iIMAOPlugin )
                {
                iIMAOPlugin->StartPluginL();
                }
            break;
            }
        case EAOManagerPluginStop:
            {
            // we should make a logout here
            if ( iIMAOPlugin )
                {
                iIMAOPlugin->StopPlugin();
                }
            break;
            }
        case EAOManagerStartedRoaming:
            {
            // tell the plugin that we started
            if ( iIMAOPlugin )
                {
                iIMAOPlugin->SetRoaming( ETrue );
                }

            // Show  warning, if variated so
            CIMPSResourceReader* resourceReader = CIMPSResourceReader::NewLC();
            TBool showRoamingWarning = resourceReader->IsRoamingWarningRequiredL();
            CleanupStack::PopAndDestroy( resourceReader );

            if ( showRoamingWarning )
                {
                User::LeaveIfError( RProperty::Set( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoaming, ETrue ) );

                if ( iIMAOPlugin )
                    {
                    // Dont do any action if login type is Auto in home n/w
                    if ( !iIMAOPlugin->IsLoginAutoInHomeNW() )
                        {
                        TBool roamingDlgShown( EFalse );
                        User::LeaveIfError( RProperty::Get( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoamingDlgShown, roamingDlgShown ) );

                        if ( !roamingDlgShown && iIMAOPlugin->IsUserLoggedInL() )
                            {
                            iIMAOPlugin->HandleRoamingL();

                            // Set the property
                            roamingDlgShown = ETrue;
                            User::LeaveIfError( RProperty::Set( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoamingDlgShown, roamingDlgShown ) );
                            }
                        }
                    }
                }
            else
                {
                err = KErrNotSupported;
                }

            break;
            }
        case EAOManagerStoppedRoaming:
            {
            // tell the plugin that we are back in home network
            if ( iIMAOPlugin )
                {
                iIMAOPlugin->SetRoaming( EFalse );
                }

            // If  warning is to be shown, then Set the roaing property
            CIMPSResourceReader* resourceReader = CIMPSResourceReader::NewLC();
            TBool showRoamingWarning = resourceReader->IsRoamingWarningRequiredL();
            CleanupStack::PopAndDestroy( resourceReader );

            if ( showRoamingWarning )
                {
                User::LeaveIfError( RProperty::Set( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoaming, EFalse ) );
                }
            else
                {
                err = KErrNotSupported;
                }
            break;
            }
        case EAOManagerNWOpsAllowed:
            {
            // tell the implementation part that the network ops are allowed
            if ( iIMAOPlugin )
                {
                iIMAOPlugin->SetNetworkOpsAllowed( ETrue );
                }
            break;
            }
        case EAOManagerNWOpsNotAllowed:
            {
            // tell the implementation part that the network ops are disallowed
            if ( iIMAOPlugin )
                {
                iIMAOPlugin->SetNetworkOpsAllowed( EFalse );
                }
            break;
            }
        case EAOManagerSuicideQuery:
            {
            err = ETrue;
            if ( iIMAOPlugin )
                {
                err = !iIMAOPlugin->CheckPluginNeed();
                }
            break;
            }
        default:
            {
            err = KErrNotSupported;
            break;
            }
        }

    return ( TAny* )err;
    }

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Use the plugin UID and Implementation factory method
// as a pair for ECom instantiation.
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KIMPSALWAYSONLINEUID3, CPEngAlwaysOnlinePlugin::NewL )
    };

// Give the table to the caller
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( ImplementationTable[0] );
    return ImplementationTable;
    }

