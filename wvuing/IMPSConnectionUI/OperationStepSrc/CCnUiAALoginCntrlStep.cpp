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
* Description:  AA login control.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <PEngWVPresenceErrors2.h>
#include <impserrors.h>
#include <IMPSConnectionUiNG.rsg>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlotManager2.h>
#include <PEngPresenceEngineConsts2.h>

#include "CCnUiAALoginCntrlStep.h"
#include "CCnUiConnOpener.h"
#include "CCnUiConnCloser.h"

#include "MCnUiBaseControlContext.h"
#include "MCnUiConnectionHandler.h"
#include "MCnUiSignaller.h"
#include "MCnUiGlobalNotificationUiFacade.h"

#include "CnUiErrors.h"

#include "CnUiClientId.h"


// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiAALoginCntrlStep* CCnUiAALoginCntrlStep::NewLC( MCnUiBaseControlContext& aCCntxt,
                                                     CIMPSSAPSettings& aLoginSap,
                                                     TBool aShowDetailedError,
                                                     CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiAALoginCntrlStep* self = new ( ELeave ) CCnUiAALoginCntrlStep( aCCntxt, aLoginSap,
                                                                        aShowDetailedError, aNWSessionSlotID );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CCnUiAALoginCntrlStep::~CCnUiAALoginCntrlStep()
    {
    delete iConnOpener;
    delete iConnCloser;
    delete iLogoutPlugin;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiAALoginCntrlStep::CCnUiAALoginCntrlStep( MCnUiBaseControlContext& aCCntxt,
                                              CIMPSSAPSettings& aLoginSap,
                                              TBool aShowDetailedError,
                                              CPEngNWSessionSlotID2& aNWSessionSlotID )
        : iCCntxt( aCCntxt ),
        iLoginSap( aLoginSap ),
        iShowDetailedError( aShowDetailedError ),
        iNWSessionSlotID( aNWSessionSlotID )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiAALoginCntrlStep::ConstructL()
    {
    iConnOpener = CCnUiConnOpener::NewL();
    iConnCloser = CCnUiConnCloser::NewL();
    }


// -----------------------------------------------------------------------------
// CCnUiAALoginCntrlStep::RunStepL()
// -----------------------------------------------------------------------------
//
TInt CCnUiAALoginCntrlStep::RunStepL()
    {
    TInt mappedError = KErrNone;
    TInt errorDetail = KErrNone;

    switch ( DoRunStepL() )
        {
        case KErrNone: //  Login succeeded
            {
            //this is returned as is
            mappedError = KErrNone;
            break;
            }

        case KCnUiErrorClientAlreadyConnected:
            {
            //Current client is already logged in. (Thus not a real error.)
            mappedError = KErrAlreadyExists;
            break;
            }


        case KErrAccessDenied:
        case KCnUiErrorNetworkConnectionNotAllowed: //  //network access denied
            {
            mappedError = KErrAccessDenied;
            errorDetail = R_QTN_CHAT_LOGIN_CONERROR_NOTE;
            break;
            }


        case KCnUiErrorSapMissingCompulsoryFields:
        case KCnUiErrorNoProperDefaultSap:
            {
            //Missing the whole default SAP
            //or it was missing some required fields.
            mappedError = KErrNotFound;
            errorDetail = R_QTN_CHAT_SETT_COMP_MISS;
            break;
            }


        case KPEngNwErrInvalidPassword:
        case KPEngNwErrUnknownUser:
        case KPEngNwErrInvalidOrUnSupportedUserProperties:
        case KPEngNwErrUnauthorized:
            {
            //wrong username / password
            //free first plug-in dynamic resources
            TIMPSConnectionClient clientId = iCCntxt.ControlledClient();
            iCCntxt.ClientPluginL( clientId, iNWSessionSlotID ).ReleaseDynamicResources();

            iCCntxt.GlobalNotificationUiL().ShowLoginUserDataErrorL( iLoginSap.SAPName() );
            mappedError = KErrArgument;
            break;
            }

        case KCnUiErrorAlreadyConnectedToAnotherSap:
            {
            // user declined disconnect from manually logged in sap
            mappedError = KErrCouldNotDisconnect;
            break;
            }

        case KPEngNwErrForbidden:// 403
            {
            errorDetail = R_QTN_CHAT_LOGIN_ACCOUNT_LOST;
            mappedError = KErrAccessDenied;
            break;
            }

        //flow through
        case KPEngNwErrServiceNotSupported: // 405
        case KPEngNwErrServiceNotAgreed: // 506
            {
            mappedError = KErrNotSupported;
            errorDetail = R_QTN_CHAT_ERROR_SERVICE_ERR;
            break;
            }

        // flow through
        case KPEngNwErrServiceUnavailable: // 503
        case KPEngNwErrMessageQueueIsFull: // 507
            {
            mappedError = KErrCouldNotConnect;
            errorDetail = R_QTN_CHAT_ERROR_SERV_BUSY;
            break;
            }

        case KPEngNwErrVersionNotSupported: // 505
            {
            mappedError = KErrGeneral;
            errorDetail = R_QTN_CHAT_ERROR_GEN_ERROR;
            break;
            }

        // flow through
        case KPEngNwErrSessionExpired: // 600
        case KErrTimedOut:
        case KErrCouldNotConnect:
        case KImpsErrorBearerSuspended:
            {
            mappedError = KErrCouldNotConnect;
            errorDetail = R_QTN_CHAT_LOGIN_CONERROR_NOTE;
            break;
            }

        default:
            {
            //General failure during login (no connection to network, etc..)
            mappedError = KErrGeneral;
            errorDetail = R_QTN_CHAT_LOGIN_CONERROR_NOTE;
            break;
            }
        }

    // if there was some other error than login data error, we must
    // show a detailed error note if aa plugin requested so
    if ( ( mappedError != KErrNone ) && ( mappedError != KErrArgument ) && ( iShowDetailedError ) )
        {
        //free first plug-in dynamic resources
        TIMPSConnectionClient clientId = iCCntxt.ControlledClient();
        iCCntxt.ClientPluginL( clientId, iNWSessionSlotID ).ReleaseDynamicResources();

        iCCntxt.GlobalNotificationUiL().ShowLoginDetailedErrorL( iLoginSap.SAPName(), errorDetail );
        }

    return mappedError;
    }


// -----------------------------------------------------------------------------
// CCnUiAALoginCntrlStep::DoRunStepL()
// -----------------------------------------------------------------------------
//
TInt CCnUiAALoginCntrlStep::DoRunStepL()
    {
    TIMPSConnectionClient clientId = iCCntxt.ControlledClient();

    //First make sure that there isn't already another login
    //operation running (if there is, then no need to select SAP...)
    if ( iCCntxt.Signaller().OperationRunning() )
        {
        return KCnUiErrorLoginOperationAlreadyInUse;
        }

    // Check if client is already logged to DEFAULT server
    CIMPSSAPSettings* defaultSap = CIMPSSAPSettings::NewLC();
    LoadDefaultSapL( *defaultSap );
    TCnUiSapCnStatus status = iCCntxt.ConnHandler().SapConnectionStatusL( *defaultSap, clientId );
    CleanupStack::PopAndDestroy( defaultSap );

    if ( status == ECnUiSCS_SapConnected )
        {
        // connected to the same server. otherwise logout
        //
        // or different server)
        return KCnUiErrorClientAlreadyConnected;
        }

    //load SAP & handle its errors
    TInt loadStatus = LoadDefaultSapL( iLoginSap );
    if ( loadStatus != KErrNone )
        {
        iLoginSap.Reset();
        return loadStatus;
        }

    //Signal to other clients that login operation is started.
    if ( iCCntxt.Signaller().SignalOperationL( iLoginSap.SAPName() ) == KErrInUse )
        {
        //some other client has started the login during SAP load...
        return KCnUiErrorLoginOperationAlreadyInUse;
        }

    if ( iLoginSap.ClientId().Length() == 0 )
        {
        iLoginSap.SetClientIdL( KClientId() );
        }


    // set the network session slot ID
    iNWSessionSlotID.SetServiceAddressL( iLoginSap.SAPAddress() );
    iNWSessionSlotID.SetUserIdL( iLoginSap.SAPUserId() );

    if ( clientId == EIMPSConnClientIM )
        {
        iNWSessionSlotID.SetAppIdL( KPEngAppIdIM() );
        }
    else
        {
        iNWSessionSlotID.SetAppIdL( KPEngAppIdPEC() );
        }

    // create a network session slot to PEC engine
    CPEngNWSessionSlotManager2* slotManager = CPEngNWSessionSlotManager2::NewLC();

    TInt err = slotManager->CreateNWSessionSlot( iNWSessionSlotID );
    // we can ignore KErrAlreadyExist, since if the error is that
    // we can just start using the slot
    if ( ( err != KErrNone ) && ( err != KErrAlreadyExists ) )
        {
        User::Leave( err );
        }
    CleanupStack::PopAndDestroy( slotManager );
    //ok to try login
    TInt loginTrieStatus = iConnOpener->MakeLoginForClient(
                               clientId,
                               iLoginSap,
                               iCCntxt.ClientPluginL( clientId, iNWSessionSlotID ),
                               iCCntxt.ConnHandler(),
                               NULL,
                               iNWSessionSlotID );


    if ( loginTrieStatus == KCnUiErrorAlreadyConnectedToAnotherSap )
        {
        // we are already connected to another sap, so we need to logout first

        // create global notification ui facade for sending channel message
        MCnUiGlobalNotificationUiFacade* uiFacade = &( iCCntxt.GlobalNotificationUiL() );

        RArray<TIMPSConnectionClient> loggedInClients;
        CleanupClosePushL( loggedInClients );
        iCCntxt.ConnHandler().GetLoggedInClientsL( loggedInClients );

        // ask a confirmation from the user
        if ( uiFacade->CGQActiveConnectionSuppressForScheduledConnectionL(
                 iLoginSap.SAPName(),
                 loggedInClients.Array() ) )
            {
            loginTrieStatus = KErrNone;

            // user accepted logout of query timed out -> do the logout
            CIMPSSAPSettings* logoutSap = CIMPSSAPSettings::NewLC();
            //get SAP from where logging out..
            iCCntxt.ConnHandler().GetLoggedInSapL( *logoutSap, clientId );

            CPEngNWSessionSlotID2* logoutID = NULL;
            logoutID = iCCntxt.GetActiveNWSessionSlotIDL( clientId );
            CleanupStack::PushL( logoutID );
            TInt logoutStatus( KErrNone );

            delete iLogoutPlugin;
            iLogoutPlugin = NULL;
            iLogoutPlugin = CreateClientPluginL( clientId, *logoutID );

            for ( TInt index = 0; index < loggedInClients.Count(); index++ )
                {
                //close the client connection using the connection closer
                logoutStatus = iConnCloser->MakeLogoutForClient( loggedInClients.Array()[ index ],
                                                                 *logoutSap,
                                                                 *iLogoutPlugin,
                                                                 iCCntxt.ConnHandler(),
                                                                 NULL,
                                                                 *logoutID );
                if ( logoutStatus != KErrNone )
                    {
                    // logout did not succeed, exit the loop
                    loginTrieStatus = logoutStatus;
                    break;
                    }
                }

            if ( loginTrieStatus == KErrNone )
                {
                // logout(s) successful -> do the login
                loginTrieStatus = iConnOpener->MakeLoginForClient( clientId,
                                                                   iLoginSap,
                                                                   iCCntxt.ClientPluginL( clientId, iNWSessionSlotID ),
                                                                   iCCntxt.ConnHandler(),
                                                                   NULL,
                                                                   iNWSessionSlotID );
                }
            delete iLogoutPlugin;
            iLogoutPlugin = NULL;
            CleanupStack::PopAndDestroy( logoutID );
            CleanupStack::PopAndDestroy( logoutSap );
            }
        CleanupStack::PopAndDestroy(); // loggedInClients
        }

    return loginTrieStatus;
    }



// -----------------------------------------------------------------------------
// CCnUiAALoginCntrlStep::HandleCompleteL()
// -----------------------------------------------------------------------------
//
TCnUiHandleCompleteStatus CCnUiAALoginCntrlStep::HandleCompleteL()
    {
    iCCntxt.Signaller().CancelOperationSignal();
    return ECnUiStepContinueTeardown;
    }


// -----------------------------------------------------------------------------
// CCnUiAALoginCntrlStep::HandleCompleteL()
// -----------------------------------------------------------------------------
//
void CCnUiAALoginCntrlStep::UndoStepL()
    {
    iCCntxt.Signaller().CancelOperationSignal();
    }


// -----------------------------------------------------------------------------
// CCnUiAALoginCntrlStep::LoadDefaultSapL()
// -----------------------------------------------------------------------------
//
TInt CCnUiAALoginCntrlStep::LoadDefaultSapL( CIMPSSAPSettings& aSap )
    {
    CIMPSSAPSettingsStore& sapStore = iCCntxt.SapStoreL();

    TIMPSConnectionClient client( ConnectionClient() );
    TIMPSAccessGroup accessGroup;
    if ( client == EIMPSConnClientIM )
        {
        accessGroup = EIMPSIMAccessGroup;
        }
    else
        {
        accessGroup = EIMPSPECAccessGroup;
        }


    //SAP store might not have any SAP => get default leaves
    TRAPD( err, sapStore.GetDefaultL( &aSap, accessGroup ) );
    if ( err == KErrNotFound )
        {
        //handle not found error by returning it
        return KCnUiErrorNoProperDefaultSap;
        }

    //and others by leaving
    User::LeaveIfError( err );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCnUiAALoginCntrlStep::DoShowLogoutOkNoteL()
// -----------------------------------------------------------------------------
//
TIMPSConnectionClient CCnUiAALoginCntrlStep::ConnectionClient()
    {
    // rvct does not support casting from TAny* to enumeration value
    TInt returnValue = ( TInt )Dll::Tls();
    return ( TIMPSConnectionClient )returnValue;
    }
//  End of File
