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
* Description:  Process Component for Login,Logout
*
*/



// INCLUDE FILES
#include "CCAEngine.h"
#include "PublicEngineDefinitions.h"
#include "ImpsCSPAllErrors.h"
#include "MCASettings.h"
#include "MCAAccessInterface.h"
#include "SServerPrefers.h"
#include "impsbuilddefinitions.h"
#include "IMUtils.h"
#include <chatng.rsg>
#include <CPEngNWSessionSlotID2.h>
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsList.h>
#include <WVSettingsSharedDataNG.h>
#include <IMPSServiceSettingsUINGInternalCRKeys.h>
#include <aknenv.h>
#include <barsread.h>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlot2.h>
#include <cimpspresenceconnectionuing.h>
#include "CCAStorageManagerFactory.h"
#include "MCAStoredContacts.h"
#include "CAUtils.h"

#include "CCALoginPC.h"
#include "MCABackgroundTaskObserver.h"
#include "CCAMainViewArrayPC.h"
#include "MCALoginRefreshObserverPC.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCALoginPC::NewL
// -----------------------------------------------------------------------------
//
CCALoginPC* CCALoginPC::NewL( CCAEngine& aEngine )
    {

    CCALoginPC* self = new ( ELeave ) CCALoginPC( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCALoginPC::~CCALoginPC()
    {
    delete iLoggedUserId;
    delete iOldLoggedUserId;
    delete iMainViewArrayPC;
    if ( iLoggedInSap )
        {
        delete iLoggedInSap;
        iLoggedInSap = NULL;
        }
    if ( iRefreshObserver )
        {
        iRefreshObserver = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCALoginPC::CCALoginPC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CCALoginPC::CCALoginPC( CCAEngine& aEngine )
        : iEngine( aEngine )

    {
    MCAChatInterface& chatInterface = iEngine.ChatInterface();
    iFirstLogin = ETrue;
    }

// -----------------------------------------------------------------------------
// CCALoginPC::ConstructL
// Symbian Second Phased Constructor, contain any code that
// might leave.
// -----------------------------------------------------------------------------
//
void CCALoginPC::ConstructL()

    {
    MCAChatInterface& chatInterface = iEngine.ChatInterface();
    iMainViewArrayPC = CCAMainViewArrayPC::
                       NewL( iEngine, iEngine.ContactListModel(), chatInterface );
    iLoggedInSap = NULL;
    }


// -----------------------------------------------------------------------------
// CCALoginPC::LoginL
// -----------------------------------------------------------------------------
//

TBool CCALoginPC::LoginL( TNetworkState aState, CIMPSSAPSettings* aSettings,
                          CPEngNWSessionSlotID2* aSessionSlotID,
                          TPEngWVCspServicesTree2 &aImpsServices )

    {

    HBufC* accessPoint = aSessionSlotID->ServiceAddress().AllocLC();
    HBufC* userId = aSessionSlotID->UserId().AllocLC();

    TImpsCspIdentifier cspId( *accessPoint, *userId );

    TRAPD( err, iEngine.GetAccessInterface()->LoginL( cspId,
                                                      aSettings->SAPUserPassword(),
                                                      aSettings->ClientId(),
                                                      aSettings->AccessPoint(),
                                                      &aSettings->ServerAuthenticationName(),
                                                      &aSettings->ServerAuthenticationPassword() ) );
    if ( err != KErrNone )
        {
        return EFalse;
        }

    // set logged in user id
    MCASettings* settings = iEngine.GetInternalSettingsInterface();
    settings->SetValueL( MCASettings::EOwnWVUserID, *userId );
    settings->SetValueL( MCASettings::EServiceAddress, *accessPoint );

    CleanupStack::PopAndDestroy( 2, accessPoint ); // userId, accessPoint, sap


    PerformNetworkStateChangeL( aState, aSessionSlotID, aImpsServices );
    // this is deleted in the destructor and this called
    // only when the application is exited.
    // so when ever the login/logout is done each time this is created
    // but not deleted

    if ( iLoggedInSap )
        {
        delete iLoggedInSap;
        iLoggedInSap = NULL;
        }

    iLoggedInSap = CIMPSSAPSettings::NewL();

    // this was leaking, changed to NewLC()
    CIMPSPresenceConnectionUi* connUI = CIMPSPresenceConnectionUi::NewLC( EIMPSConnClientIM );
    connUI->GetLoggedInSapL( *aSessionSlotID, *iLoggedInSap );
    CleanupStack::PopAndDestroy( connUI );

    //Store the user id when logged in for the first time.
    //here the old user id an new user id will be the same on first login.
    if ( iFirstLogin )
        {
        iOldLoggedUserId = aSessionSlotID->UserId().AllocL();
        iFirstLogin = EFalse;
        iMainViewArrayPC->ResetArray( ETrue );
        iMainViewArrayPC->SetCurrentItemIndexL( ETrue );
        }
    //delete the previous login id
    delete iLoggedUserId;
    iLoggedUserId = NULL;
    //Store the currently logged in user id for every login
    iLoggedUserId = aSessionSlotID->UserId().AllocL();

    if ( FreshLoginL() )
        {
        iEngine.ChatInterface().CloseAllContainers(); //Only if fresh login
        iMainViewArrayPC->ResetArray( ETrue );
        }
    else
        {
        iMainViewArrayPC->ResetArray( EFalse );
        }
    iMainViewArrayPC->ResetGroupCountDownOnLogout();
    iMainViewArrayPC->PopulateGroupsListL();


    if ( iRefreshObserver )
        {
        iRefreshObserver->HandleLoginRefreshCompleteL();
        }
    // call back if conveversation view is active

    return ETrue;

    }

// -----------------------------------------------------------------------------
// CCALoginPC::PerformNetworkStateChangeL
// -----------------------------------------------------------------------------
//
void CCALoginPC::PerformNetworkStateChangeL( TNetworkState aState,
                                             CPEngNWSessionSlotID2* aSessionSlotID,
                                             TPEngWVCspServicesTree2 &aImpsServices )
    {
    // build up the server preferences structure (variated things to
    // pass on to the engine)

    SServerPrefers serverPrefers;
    serverPrefers.iUseGrant =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVEGRANT );

    serverPrefers.iAliasUsed =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVE_ALIAS );

    serverPrefers.iLocalEchoInGroup =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_LOCAL_ECHO_USED_IN_GROUP );

    serverPrefers.iReactiveAuthorization =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_REACTIVE_PRESENCE_AUTH );


    iEngine.HandleNetworkStateChangeL( aState, &aImpsServices, serverPrefers,
                                       aSessionSlotID );

    }

// -----------------------------------------------------------------------------
// CCALoginPC::GetArrayInterface
// -----------------------------------------------------------------------------
//

EXPORT_C MCAMainViewArrayPC* CCALoginPC::GetArrayInterface() const
    {
    return iMainViewArrayPC;
    }


// -----------------------------------------------------------------------------
// CCALoginPC::NotifyEngineForLogoutL
// -----------------------------------------------------------------------------
//
TBool CCALoginPC::NotifyEngineForLogoutL( TNetworkState aState,
                                          CPEngNWSessionSlotID2* aSessionSlotID,
                                          TPEngWVCspServicesTree2 &aImpsServices )
    {
    SServerPrefers serverPrefers;
    serverPrefers.iUseGrant =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVEGRANT );

    serverPrefers.iAliasUsed =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVE_ALIAS );

    serverPrefers.iLocalEchoInGroup =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_LOCAL_ECHO_USED_IN_GROUP );

    serverPrefers.iReactiveAuthorization =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_REACTIVE_PRESENCE_AUTH );


    TRAPD( err, iEngine.HandleNetworkStateChangeL( aState, &aImpsServices, serverPrefers,
                                                   aSessionSlotID ) );

    if ( err != KErrNone )
        {
        return EFalse;
        }


    return ETrue;

    }
// -----------------------------------------------------------------------------
// CCALoginPC::LogoutL
// Once engine has been fully constructed full, invoke logout on the
// AcessInterface  reference
// -----------------------------------------------------------------------------
//
void CCALoginPC::LogoutL()
    {
    MCAAccessInterface* accessIf = iEngine.GetAccessInterface();
    if ( accessIf )
        {
        // the interface exists only if engine had been fully constructed
        accessIf->LogoutL();
        iMainViewArrayPC->ResetArray( EFalse );
        iMainViewArrayPC->SetCurrentItemIndexL();
        }
    }
// -----------------------------------------------------------------------------
// CCALoginPC::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CCALoginPC::CancelAllRequests()
    {
    iEngine.CancelAllRequests();
    }

// -----------------------------------------------------------------------------
// CCALoginPC::ReadyForShutdown()
// -----------------------------------------------------------------------------
//
TBool CCALoginPC::ReadyForShutdown()
    {
    return iEngine.ReadyForShutdown();
    }

// -----------------------------------------------------------------------------
// CCALoginPC::SetShowNicknameL
// -----------------------------------------------------------------------------
//
void CCALoginPC::SetShowNicknameL( TBool aShowNickname )
    {
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    contacts->SetShowNickname( aShowNickname );

    }


// -----------------------------------------------------------------------------
// CCALoginPC::SetAliasL
// -----------------------------------------------------------------------------
//
void CCALoginPC::SetAliasL( const TDesC& aAlias )
    {
    CCAStorageManagerFactory::ContactListInterfaceL()->
    OwnStatus().SetAliasL( aAlias );

    }

// ---------------------------------------------------------
// CCALoginPC::FreshLoginL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCALoginPC::FreshLoginL( )
    {
    if ( !iLoggedInSap )
        {
        return EFalse;
        }

    TBool isFreshLogin = EFalse;

    // check if iLoggedUserId & iOldLoggedUserId are not null;
    if ( iLoggedUserId && iOldLoggedUserId )
        {
        if ( CAUtils::NeutralCompare( *iOldLoggedUserId, *iLoggedUserId ) != 0 )
            {
            isFreshLogin = ETrue;
            if ( iOldLoggedUserId )
                {
                delete iOldLoggedUserId;
                iOldLoggedUserId = NULL;
                }
            iOldLoggedUserId = iLoggedUserId->AllocL();
            }
        else
            {
            isFreshLogin = EFalse;
            }
        }

    return isFreshLogin;

    }

// ---------------------------------------------------------
// CCALoginPC::AddObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCALoginPC::AddObserver( MCALoginRefreshObserverPC *aObserver )
    {
    iRefreshObserver = aObserver;
    }

// ---------------------------------------------------------
// CCALoginPC::RemoveObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALoginPC::RemoveObserver()
    {
    iRefreshObserver = NULL;
    }


// ---------------------------------------------------------
// CCALoginPC::RegisterGroupSyncObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCALoginPC::RegisterBackGroundTaskObserver( MCABackGroundTaskObserver *aObserver )
    {
    iEngine.RegisterBackGroundTaskObserver( aObserver );
    }

// ---------------------------------------------------------
// CCALoginPC::UnRegisterGroupSyncObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCALoginPC::UnRegisterBackGroundTaskObserver( MCABackGroundTaskObserver *aObserver )
    {
    iEngine.UnRegisterBackGroundTaskObserver( aObserver );
    }

// ---------------------------------------------------------
// CCALoginPC::IsBackgroundTaskPending
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCALoginPC::IsBackgroundTaskPending()
    {
    return iEngine.IsBackgroundTaskPending();
    }

//    End of File
