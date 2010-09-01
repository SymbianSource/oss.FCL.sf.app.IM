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
* Description:  UI control context implementation.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettings.h>
#include <mimpsconnprocessobserverng.h>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlotManager2.h>
#include <PEngPresenceEngineConsts2.h>
#include <IMPSConnectionUiNG.rsg>
#include <BarsRead.h>
#include <coemain.h>
#include <e32property.h>

#include "CCnUiUiControlContext.h"
#include "MCnUiUiFacade.h"


//steps & driver
#include "CCnUiCntrlStepDriver.h"
#include "CCnUiLoginSAPSelectUiCntrlStep.h"
#include "CCnUiLoginUiCntrlStep.h"
#include "CCnUiLogoutUiCntrlStep.h"
#include "CCnUiNWDisconnectUiCntrlStep.h"
#include "CCnUiReconnectAllUiCntrlStep.h"
#include "CCnUiAppExitLogoutUiCntrlStep.h"

#include "ccnuiroamingcntrlstep.h"
#include "IMPSUIDDefs.h"
#include "MIMPSSharedData.h"

#include "IMPSCommonUiDebugPrint.h"
#include "variantkeys.h"

/**
 * Private helper class to manage process
 * observer notifying.
 * @since 2.1
 */
class TProcessObserverProxy : public MCnUiConnProcessObserverProxy
    {
    public:  //constructor

        /**
         * C++ constructor.
         * @param aObserver The process observer, can be NULL
         * @param aProcessUi The generic process UI, can be NULL
         */
        TProcessObserverProxy( MIMPSConnProcessObserver* aObserver,
                               MIMPSConnProcessUi* aProcessUi );

    public:  //Methods from MIMPSConnProcessObserverProxy

        /**
         * From MIMPSConnProcessObserverProxy
         *
         * Implements the observer SAP access
         * notification.
         *
         * @since 2.1
         * @param aAccessedSap The accessed SAP.
         */
        void NotifyAccessingSapL( const CIMPSSAPSettings& aAccessedSap );

    private: //data

        ///<Access controlled observer, not owned
        MIMPSConnProcessObserver*       iObserver;

        //generic process UI, not owned
        MIMPSConnProcessUi*             iProcessUi;
    };



// =========== TPROCESSOBSERVERPROXY MEMBER FUNCTIONS =============
// C++ default constructor can NOT contain any code, that
// might leave.
//
TProcessObserverProxy::TProcessObserverProxy( MIMPSConnProcessObserver* aObserver,
                                              MIMPSConnProcessUi* aProcessUi )
        : iObserver( aObserver ),
        iProcessUi( aProcessUi )
    {
    }

// -----------------------------------------------------------------------------
// TProcessObserverProxy::NotifyAccessingSapL()
// From MIMPSConnProcessObserverProxy
// -----------------------------------------------------------------------------
//
void TProcessObserverProxy::NotifyAccessingSapL( const CIMPSSAPSettings& aAccessedSap )
    {
    if ( iObserver )
        {
        //NOTE!! Its on observers responsibility to handle NULL ui...
        iObserver->HandleSapAccessEventEventL( aAccessedSap.SAPName(),
                                               aAccessedSap.SAPAddress(),
                                               iProcessUi );
        }
    }




// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiUiControlContext* CCnUiUiControlContext::NewLC( TIMPSConnectionClient aClient,
                                                     CCnUiSapStoreProxy& aSapStoreProxy,
                                                     MCnUiConnModeRewaker& aConnModeRewaker,
                                                     MCnUiConnectionHandler& aConnHandler,
                                                     MIMPSConnProcessObserver* aObserver )
    {
    CCnUiUiControlContext* self = new ( ELeave ) CCnUiUiControlContext( aClient,
                                                                        aSapStoreProxy,
                                                                        aConnModeRewaker,
                                                                        aConnHandler );
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );

    return self;
    }


// Destructor
CCnUiUiControlContext::~CCnUiUiControlContext()
    {
    delete iProcessObserverProxy;
    delete iUi;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiUiControlContext::CCnUiUiControlContext( TIMPSConnectionClient aClient,
                                              CCnUiSapStoreProxy& aSapStoreProxy,
                                              MCnUiConnModeRewaker& aConnModeRewaker,
                                              MCnUiConnectionHandler& aConnHandler )
        : CCnUiBaseControlContext( aClient, aSapStoreProxy, aConnHandler ),
        iConnModeRewaker( aConnModeRewaker )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiUiControlContext::ConstructL( MIMPSConnProcessObserver* aObserver )
    {
    CCnUiBaseControlContext::ConstructL();
    iUi = CreateUiFacadeL();

    iProcessObserverProxy = new ( ELeave ) TProcessObserverProxy( aObserver,
                                                                  &iUi->ConnProcessUi() );
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::DoLoginL()
// -----------------------------------------------------------------------------
//
TInt CCnUiUiControlContext::DoLoginL( TIMPSLoginType aLoginType,
                                      CPEngNWSessionSlotID2& aNWSessionSlotID,
                                      CIMPSSAPSettings* aSap /*= NULL*/ , TBool aLastLoginSap /*=EFalse*/ )
    {

    if ( aLastLoginSap && aSap )
        {
        // if user want to connect to the last logged in server
        CCnUiCntrlStepDriver* driver = CCnUiCntrlStepDriver::NewLC();

        // Check for indication variant.
        // Show WAP registration query, if variated so
        TBool showRoamingWarning = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_ROAMING_WARNING );

        //add control steps
        if ( showRoamingWarning )
            {
            // Check if  property is set
            RProperty roaming;
            TBool isRoaming;
            User::LeaveIfError( roaming.Get( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoaming, isRoaming ) );

            if ( isRoaming )
                {
                TBool isRoamingDlgShown( EFalse );
                User::LeaveIfError( roaming.Get( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoamingDlgShown, isRoamingDlgShown ) );

                if ( !isRoamingDlgShown )
                    {
                    driver->AppendStepL( CCnUiRoamingCntrlStep::NewLC() );
                    CleanupStack::Pop(); // CCnUiRoamingCntrlStep instance
                    }
                }
            }

        driver->AppendStepL( CCnUiLoginSAPSelectUiCntrlStep::NewLC( *this, *aSap, aLoginType, aLastLoginSap ) );
        CleanupStack::Pop(); // CCnUiLoginSAPSelectUiCntrlStep instance

        TIMPSConnectionClient client = ControlledClient();

        driver->AppendStepL( CCnUiLoginUiCntrlStep::NewLC( *this,
                                                           *aSap,
                                                           aLoginType,
                                                           aNWSessionSlotID,
                                                           client ) );
        CleanupStack::Pop(); // CCnUiLoginUiCntrlStep instance

        //and run
        return ExecuteDriverInUiLX( *driver, 1 ); //1  driver
        }
    else
        {
        // user want to connect for default one
        CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();
        CCnUiCntrlStepDriver* driver = CCnUiCntrlStepDriver::NewLC();

        // Check for indication variant.
        // Show WAP registration query, if variated so
        TBool showRoamingWarning = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_ROAMING_WARNING );

        //add control steps
        if ( showRoamingWarning )
            {
            // Read the property value if user is in
            RProperty roaming;
            TBool isRoaming;
            User::LeaveIfError( roaming.Get( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoaming, isRoaming ) );

            // Check if  property is set
            if ( isRoaming )
                {
                TBool isRoamingDlgShown( EFalse );
                User::LeaveIfError( roaming.Get( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoamingDlgShown, isRoamingDlgShown ) );

                if ( !isRoamingDlgShown )
                    {
                    driver->AppendStepL( CCnUiRoamingCntrlStep::NewLC() );
                    CleanupStack::Pop(); // CCnUiRoamingCntrlStep instance
                    }
                }
            }

        driver->AppendStepL( CCnUiLoginSAPSelectUiCntrlStep::NewLC( *this, *sap, aLoginType ) );
        CleanupStack::Pop(); // CCnUiLoginSAPSelectUiCntrlStep instance

        TIMPSConnectionClient client = ControlledClient();

        driver->AppendStepL( CCnUiLoginUiCntrlStep::NewLC( *this,
                                                           *sap,
                                                           aLoginType,
                                                           aNWSessionSlotID,
                                                           client ) );
        CleanupStack::Pop(); // CCnUiLoginUiCntrlStep instance

        //and run
        return ExecuteDriverInUiLX( *driver, 2 ); //2 == sap & driver
        }
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::IntResourceValueL()
// -----------------------------------------------------------------------------
//
TInt CCnUiUiControlContext::IntResourceValueL( TInt aResourceId )
    {
    TInt value( KErrNone );
    TResourceReader reader;
    CCoeEnv* coeEnv = CCoeEnv::Static();
    coeEnv->CreateResourceReaderLC( reader, aResourceId );
    value = ResourceUtils::ReadTInt32L( reader );
    CleanupStack::PopAndDestroy(); // reader
    return value;
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::DoLogoutL()
// -----------------------------------------------------------------------------
//
TInt CCnUiUiControlContext::DoLogoutL( const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();
    CCnUiCntrlStepDriver* driver = CCnUiCntrlStepDriver::NewLC();

    //add control steps
    driver->AppendStepL( CCnUiLogoutUiCntrlStep::NewLC( *this, *sap, aNWSessionSlotID ) );
    CleanupStack::Pop(); // CCnUiLogoutUiCntrlStep instance

    // Check if warning feature is present
    TBool showRoamingWarning = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_ROAMING_WARNING );
    if ( showRoamingWarning )
        {
        RProperty roaming;
        User::LeaveIfError( roaming.Set( KIMPSConnUiTemporaryUid, EIMPSSharedKeysRoamingDlgShown, EFalse ) );
        }

    //and run
    return ExecuteDriverInUiLX( *driver, 2 ); //2 == sap & driver
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::DoHandleApplicationExitL()
// -----------------------------------------------------------------------------
//
TInt CCnUiUiControlContext::DoHandleApplicationExitL(
    TIMPSExitType aExitType,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();
    CCnUiCntrlStepDriver* driver = CCnUiCntrlStepDriver::NewLC();

    //add control steps
    driver->AppendStepL( CCnUiAppExitLogoutUiCntrlStep::NewLC( *this,
                                                               *sap,
                                                               aExitType,
                                                               aNWSessionSlotID ) );
    CleanupStack::Pop(); // CCnUiAppExitLogoutUiCntrlStep instance

    //and run
    return ExecuteDriverInUiLX( *driver, 2 ); //2 == sap & driver
    }



// -----------------------------------------------------------------------------
// CCnUiUiControlContext::Ui()
// From MCnUiUiControlContext
// -----------------------------------------------------------------------------
//
MCnUiUiFacade& CCnUiUiControlContext::Ui()
    {
    return *iUi;
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::ProcessObserverProxy()
// From MCnUiUiControlContext
// -----------------------------------------------------------------------------
//
MCnUiConnProcessObserverProxy& CCnUiUiControlContext::ProcessObserverProxy()
    {
    return *iProcessObserverProxy;
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::ConnModeRewaker()
// From MCnUiUiControlContext
// -----------------------------------------------------------------------------
//
MCnUiConnModeRewaker& CCnUiUiControlContext::ConnModeRewaker()
    {
    return iConnModeRewaker;
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::SubOpDisconnectAllL()
// From MCnUiUiControlContext
// -----------------------------------------------------------------------------
//
TInt CCnUiUiControlContext::SubOpDisconnectAllL(
    CIMPSSAPSettings& aDisconnectedSap,
    RArray< TIMPSConnectionClient >& aDisconnectedClients,
    const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiCntrlStepDriver* driver = CCnUiCntrlStepDriver::NewLC();

    //add control steps
    driver->AppendStepL( CCnUiNWDisconnectUiCntrlStep::NewLC( *this,
                                                              aDisconnectedSap,
                                                              aDisconnectedClients,
                                                              EFalse,
                                                              aNWSessionSlotID ) );
    CleanupStack::Pop(); // CCnUiNWDisconnectUiCntrlStep instance

    //and run
    return ExecuteDriverInUiLX( *driver, 1 ); //1 == driver
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::SubOpReconnectAllL()
// From MCnUiUiControlContext
// -----------------------------------------------------------------------------
//
TInt CCnUiUiControlContext::SubOpReconnectAllL( CIMPSSAPSettings& aReconnectedSap,
                                                CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiCntrlStepDriver* driver = CCnUiCntrlStepDriver::NewLC();

    TIMPSConnectionClient client = ControlledClient();
    //add control steps
    driver->AppendStepL( CCnUiReconnectAllUiCntrlStep::NewLC( *this,
                                                              aReconnectedSap,
                                                              client,
                                                              aNWSessionSlotID ) );
    CleanupStack::Pop(); // CCnUiReconnectAllUiCntrlStep instance

    //and run
    return ExecuteDriverInUiLX( *driver, 1 ); //1 == driver
    }



// -----------------------------------------------------------------------------
// CCnUiUiControlContext::ConnHandler()
// Forward to base class.
// -----------------------------------------------------------------------------
//
MCnUiConnectionHandler& CCnUiUiControlContext::ConnHandler()
    {
    return CCnUiBaseControlContext::ConnHandler();
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::Signaller()
// Forward to base class.
// -----------------------------------------------------------------------------
//
MCnUiSignaller& CCnUiUiControlContext::Signaller()
    {
    return CCnUiBaseControlContext::Signaller();
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::ConnModeHandler()
// Forward to base class.
// -----------------------------------------------------------------------------
//
MCnUiConnModeHandler& CCnUiUiControlContext::ConnModeHandler()
    {
    return CCnUiBaseControlContext::ConnModeHandler();
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::ControlledClient()
// Forward to base class.
// -----------------------------------------------------------------------------
//
TIMPSConnectionClient CCnUiUiControlContext::ControlledClient()
    {
    return CCnUiBaseControlContext::ControlledClient();
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::SapStoreL()
// Forward to base class.
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsStore& CCnUiUiControlContext::SapStoreL()
    {
    return CCnUiBaseControlContext::SapStoreL();
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::ClientPluginL()
// Forward to base class.
// -----------------------------------------------------------------------------
//
MCnUiClientPlugin& CCnUiUiControlContext::ClientPluginL( TIMPSConnectionClient aClient,
                                                         CPEngNWSessionSlotID2& aNWSessionSlotID,
                                                         TBool aRefreshPlugin /* = EFalse */ )
    {
    return CCnUiBaseControlContext::ClientPluginL( aClient, aNWSessionSlotID, aRefreshPlugin );
    }


// -----------------------------------------------------------------------------
// CCnUiUiControlContext::ClientPluginL()
// Forward to base class.
// -----------------------------------------------------------------------------
//
MCnUiGlobalNotificationUiFacade& CCnUiUiControlContext::GlobalNotificationUiL()
    {
    return CCnUiBaseControlContext::GlobalNotificationUiL();
    }

// -----------------------------------------------------------------------------
// CIMPSPresenceConnectionUiImp::DisplayDomainSelectionQueryL
// -----------------------------------------------------------------------------
//
TInt CCnUiUiControlContext::DisplayDomainSelectionQueryL(
    TDes& aSelectedDomain,
    CIMPSSAPSettings* aSap )
    {
    MCnUiUiFacade* facade = CreateUiFacadeL();
    CleanupDeletePushL( facade );
    TInt retVal = facade->DisplayDomainSelectionQueryL( aSelectedDomain, aSap );
    CleanupStack::PopAndDestroy( facade );
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCnUiUiControlContext::ExecuteDriverInUiLX()
// -----------------------------------------------------------------------------
//
TInt CCnUiUiControlContext::ExecuteDriverInUiLX( CCnUiCntrlStepDriver& aDriver,
                                                 TInt aPopAndDestroyCount )
    {
    iUi->CommandAbsorbOnLC();
    TInt status = aDriver.ExecuteL();
    CleanupStack::PopAndDestroy( 1 + aPopAndDestroyCount ); //Command absorber + objects from outer level
    return status;
    }


//  End of File


