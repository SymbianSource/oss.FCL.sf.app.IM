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
* Description:  Login SAP select UI control.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettings.h>
#include <PEngWVPresenceErrors2.h>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlotManager2.h>
#include <PEngPresenceEngineConsts2.h>
#include <barsc.h>
#include <impsconnectionuing.rsg>

#include "CCnUiLoginUiCntrlStep.h"
#include "CnUiSapLoginDataRefresher.h"
#include "CCnUiConnOpener.h"
#include "MCnUiUiControlContext.h"
#include "MCnUiUiFacade.h"
#include "MCnUiConnectionHandler.h"
#include "MCnUiConnModeHandler.h"
#include "MCnUiClientPlugin.h"
#include "MCnUiConnModeRewaker.h"
#include "CnUiSapDataPacker.h"

#include "CnUiErrors.h"

#include "CnUiClientId.h"
#include "impscommonuibuilddefinitions.h"
#include "IMPSCommonUiDebugPrint.h"

#include "cnuitermsofusedialoghandler.h"
#include "mimpsshareddata.h"
#include "cimpsshareddatafactory.h"
#include "cnuiresourcefilename.h"

#include 	"VariantKeys.h"
#include 	<centralrepository.h>

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiLoginUiCntrlStep* CCnUiLoginUiCntrlStep::NewLC( MCnUiUiControlContext& aCCntxt,
                                                     CIMPSSAPSettings& aLoginSap,
                                                     TIMPSLoginType aLoginType,
                                                     CPEngNWSessionSlotID2& aNWSessionSlotID,
                                                     TIMPSConnectionClient aClient )
    {
    CCnUiLoginUiCntrlStep* self = new ( ELeave ) CCnUiLoginUiCntrlStep( aCCntxt,
                                                                        aLoginSap,
                                                                        aNWSessionSlotID,
                                                                        aClient,
                                                                        aLoginType );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CCnUiLoginUiCntrlStep::~CCnUiLoginUiCntrlStep()
    {
    if ( iConnOpener->IsActive() )
        {
        iConnOpener->Cancel();
        }
    delete iConnOpener;
    delete iWaitNote;
    iRFs.Close();
    delete iSharedData;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiLoginUiCntrlStep::CCnUiLoginUiCntrlStep( MCnUiUiControlContext& aCCntxt,
                                              CIMPSSAPSettings& aLoginSap,
                                              CPEngNWSessionSlotID2& aNWSessionSlotID,
                                              TIMPSConnectionClient aClient,
                                              TIMPSLoginType aLoginType )
        : iCCntxt( aCCntxt ),
        iLoginSap( aLoginSap ),
        iLoginType( aLoginType ),
        iSessionSlotID( aNWSessionSlotID ),
        iClient( aClient )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiLoginUiCntrlStep::ConstructL()
    {
    iConnOpener = CCnUiConnOpener::NewL();
    iSharedData = CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL( NULL, KBrandingUid );
    }


// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::RunStepL()
// from MCnUiCntrlStep
// -----------------------------------------------------------------------------
//
TInt CCnUiLoginUiCntrlStep::RunStepL()
    {
    TInt loginTrieStatus = KErrNone;

    TBool loginRetryOngoing( EFalse );
    TBool loginRetryAllowed = ETrue;
    while ( loginRetryAllowed )
        {
        //initialization is needed on every round since
        //there might have run some sub steps when handling login result
        TIMPSConnectionClient clientToLogIn;
        TInt initErr = DoInitLoginStepL(  clientToLogIn );
        if ( initErr != ECnUiLoginInitContinue )
            {
            return initErr;
            }

        if ( ( iLoginSap.SAPUserId().Length() == 0 ) ||
             ( iLoginSap.SAPUserPassword().Length() == 0 ) )
            {
            if ( !CnUiSapLoginDataRefresher::RefreshLoginDataL( iCCntxt.Ui(),
                                                                iLoginSap,
                                                                EFalse ) )
                {
                //user canceled the login data query
                iCCntxt.Ui().ShowNoteL( ECnUiConnCanceled );
                return KErrCancel;
                }
            else
                {
                // User ID may have changed
                // Check Terms of use variation
                if ( ReadResourceIntValueL( RSC_CHAT_VARIATION_IMPSCU_TOU_MESSAGE )
                     && iLoginSap.SAPUserId().Length() > 0 )
                    {
                    TInt retVal = CnUiTermsOfUseDialogHandler::HandleTermsOfUseDialogL(
                                      iLoginSap, iCCntxt.Ui(), iCCntxt.SapStoreL() );

                    if ( retVal == KErrCancel )
                        {
                        // User canceled ToU query, login cancelled
                        // note is already shown inside HandleTermsOfUseDialogL
                        // method, return KErrCancel
                        return retVal;
                        }
                    }
                }
            }

        if ( iLoginSap.ClientId().Length() == 0 )
            {
            iLoginSap.SetClientIdL( KClientId() );
            }

        // set the network session slot ID
        iSessionSlotID.SetServiceAddressL( iLoginSap.SAPAddress() );
        iSessionSlotID.SetUserIdL( iLoginSap.SAPUserId() );

        if ( iClient == EIMPSConnClientIM )
            {
            iSessionSlotID.SetAppIdL( KPEngAppIdIM() );
            }
        else
            {
            iSessionSlotID.SetAppIdL( KPEngAppIdPEC() );
            }

        // create a network session slot to PEC engine
        CPEngNWSessionSlotManager2* slotManager = CPEngNWSessionSlotManager2::NewLC();

        TInt err = slotManager->CreateNWSessionSlot( iSessionSlotID );
        // we can ignore KErrAlreadyExist, since if the error is that
        // we can just start using the slot
        if ( ( err != KErrNone ) && ( err != KErrAlreadyExists ) )
            {
            User::Leave( err );
            }
        CleanupStack::PopAndDestroy( slotManager );

        //proceed with login, notify first observers
        iCCntxt.ProcessObserverProxy().NotifyAccessingSapL( iLoginSap );


        //store clients needing AA rewake to login SAP
        RArray< TIMPSConnectionClient > clientsNeedingAARewake;
        CleanupClosePushL( clientsNeedingAARewake );
        iCCntxt.ConnModeRewaker().GetAARewakeListL( clientsNeedingAARewake );

        CnUiSapDataPacker::PackAAClientsL( clientsNeedingAARewake, iLoginSap );
        CleanupStack::PopAndDestroy(); //clientsNeedingAARewake

        TBool refreshPlugin( EFalse );
        if ( loginRetryOngoing )
            {
            refreshPlugin = ETrue;
            loginRetryOngoing = EFalse;
            }
        //Open the client connection using the connection opener
        //control listens opener progress events
        //and sets up proper wait notes according the opener states
        loginTrieStatus = iConnOpener->MakeLoginForClient(
                              clientToLogIn,
                              iLoginSap,
                              iCCntxt.ClientPluginL( clientToLogIn, iSessionSlotID, refreshPlugin ),
                              iCCntxt.ConnHandler(),
                              this,
                              iSessionSlotID );

        //connection opener finished its processing
        //clear any existing wait note set by the progress step.
        //own wait note container used ==> direct deletion ok.
        delete iWaitNote;
        iWaitNote = NULL;

        //handle success
        if ( loginTrieStatus == KErrNone )
            {
            //Rewake list is now stored in logged in SAP
            //==> clear existing rewake list
            iCCntxt.ConnModeRewaker().ClearAARewakeList();

            //and notify about user level login
            if ( iLoginType == EIMPSAAConnectionStart )
                {
                iCCntxt.ConnModeHandler().SendUserSelectionL( EUserLevelAAConnectionStart,
                                                              clientToLogIn );
                }
            else
                {
                iCCntxt.ConnModeHandler().SendUserSelectionL( EUserLevelLogin,
                                                              clientToLogIn );
                }

            //login is completed
            return KErrNone;
            }


        //handle errors & determine wheter to try again
        //only here listed errors may cause the restart from begining
        switch ( loginTrieStatus )
            {
            case KPEngNwErrInvalidPassword:
            case KPEngNwErrUnknownUser:
            case KPEngNwErrInvalidOrUnSupportedUserProperties:
            case KPEngNwErrUnauthorized:
                {
                loginTrieStatus = HandleLoginDataFailureL( loginTrieStatus );
                if ( !loginTrieStatus )
                    {
                    loginRetryOngoing = ETrue;
                    }
                break;
                }

            case KCnUiErrorAlreadyConnectedToAnotherSap:
                {
                loginTrieStatus = HandleConflictingSapConnectionL( loginTrieStatus,
                                                                   clientToLogIn );
                break;
                }

            default:
                {
                //in all other cases restart isn't allowed but some note is needed
                iCCntxt.Ui().ShowLoginErrorNoteL( loginTrieStatus );
                break;
                }
            }

        if ( loginTrieStatus != KErrNone )
            {
            //No retry allowed ==> stop login tries here
            loginRetryAllowed = EFalse;
            }
        }

    return loginTrieStatus;
    }



// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::HandleCompleteL()
// from MCnUiCntrlStep
// -----------------------------------------------------------------------------
//
TCnUiHandleCompleteStatus CCnUiLoginUiCntrlStep::HandleCompleteL()
    {
    //nothing to complete
    return ECnUiStepContinueTeardown;
    }


// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::UndoStepL()
// from MCnUiCntrlStep
// -----------------------------------------------------------------------------
//
void CCnUiLoginUiCntrlStep::UndoStepL()
    {
    //nothing to undo
    }


// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::HandleWaitNoteCancel()
// from MCnUiWaitNoteObserver
// -----------------------------------------------------------------------------
//
void CCnUiLoginUiCntrlStep::HandleWaitNoteCancel()
    {
    iConnOpener->CancelLogin();
    }


// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::HandleProgressStateEnter()
// -----------------------------------------------------------------------------
//
void CCnUiLoginUiCntrlStep::HandleProgressStateEnter( TInt aStateId,
                                                      TIMPSConnectionClient aClient )
    {
    IMPSCUI_DP_FUNC_ENTER( "CCnUiLoginUiCntrlStep::HandleProgressStateEnter()" );
    TRAPD( leaveErr, DoShowLoginStateSpecificWaitnoteL( aStateId, aClient ) )
    if ( ( aStateId == ECnOpenerProcessingPlugin ) && ( !leaveErr ) )
        {
        //show login ok note
        TRAP( leaveErr, iCCntxt.Ui().ShowNoteL( ECnUiConnectionOK ) );
        }
    iCCntxt.Ui().HandleIfError( leaveErr );
    IMPSCUI_DP_FUNC_DONE( "CCnUiLoginUiCntrlStep::HandleProgressStateEnter()" );
    }


// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::DoInitLoginStepL()
// -----------------------------------------------------------------------------
//
TInt CCnUiLoginUiCntrlStep::DoInitLoginStepL( TIMPSConnectionClient& aClient )
    {
    TIMPSConnectionClient clientId = iCCntxt.ControlledClient();
    if ( iCCntxt.ConnHandler().TheClientLoggedInL( clientId ) )
        {
        //current client is already logged in
        return KCnUiErrorClientAlreadyConnected;
        }

    aClient = clientId;
    return ECnUiLoginInitContinue;
    }


// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::DoShowLoginStateSpecificWaitnoteL()
// -----------------------------------------------------------------------------
//
void CCnUiLoginUiCntrlStep::DoShowLoginStateSpecificWaitnoteL( TInt aStateId,
                                                               TIMPSConnectionClient aClient )
    {
    switch ( aStateId )
        {
            //FLOW TROUGH
        case ECnOpenerOpeningSAPConnection:
        case ECnOpenerClosingInterferingSAPConnection:
            {
            if ( !iWaitNote )
                {
                // opening SAP connection
                if ( aClient == EIMPSConnClientIM )
                    {
                    iWaitNote = iCCntxt.Ui().WaitNoteL( ECnUiWaitNoteConnectingChat,
                                                        iLoginSap.SAPName(),
                                                        this );
                    }

                else
                    {
                    //client is considered to be PEC
                    iWaitNote = iCCntxt.Ui().WaitNoteL( ECnUiWaitNoteConnectingPEC,
                                                        iLoginSap.SAPName(),
                                                        this );
                    }
                }

            break;
            }
        case ECnOpenerProcessingPlugin:
        case ECnOpenerIdle:
        case ECnOpenerClosingOwnedSAPConnection:
        default:
            {
            //clear any existing wait note set by the previous
            //progress steps. No wait note used during these steps.
            //own wait note container used ==> direct deletion ok.
            delete iWaitNote;
            iWaitNote = NULL;

            break;
            }
        }
    }



// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::HandleLoginDataFailureL()
// -----------------------------------------------------------------------------
//
TInt CCnUiLoginUiCntrlStep::HandleLoginDataFailureL( TInt aTrieError )
    {
    //login has failed to improper user name or password
    //show a note for the reason & refresh the login data
    iCCntxt.Ui().ShowLoginErrorNoteL( aTrieError );
    if ( CnUiSapLoginDataRefresher::RefreshLoginDataL( iCCntxt.Ui(), iLoginSap, ETrue ) )
        {
        return KErrNone; //user accepted the login data query ==> retry
        }

    else
        {
        //user canceled the login data query
        iCCntxt.Ui().ShowNoteL( ECnUiConnCanceled );
        return KErrCancel;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::HandleConflictingSapConnectionL()
// -----------------------------------------------------------------------------
//
TInt CCnUiLoginUiCntrlStep::HandleConflictingSapConnectionL(
    TInt aTrieError,
    TIMPSConnectionClient aTriedLoginClient )
    {
    TBool dropExistingConnection = EFalse;
    if ( aTriedLoginClient == EIMPSConnClientPEC )
        {
        //PEC trying to connect
        //==> the other existing client must be IM
        dropExistingConnection = iCCntxt.Ui().ConfirmationQueryL( ECnUiDropIMLogin,
                                                                  iLoginSap.SAPName() );
        }
    else
        {
        dropExistingConnection = iCCntxt.Ui().ConfirmationQueryL( ECnUiDropPECLogin,
                                                                  iLoginSap.SAPName() );
        }


    if ( !dropExistingConnection )
        {
        //user doesn't want to terminate existing connection
        iCCntxt.Ui().ShowNoteL( ECnUiConnCanceled );
        return KErrCancel;
        }

    //user selected to drop existing connection
    //if disconnecting succeeds, then retry is allowed
    //else return the original error
    if ( TryDisconnectOtherConnectionsL() )
        {
        return KErrNone;
        }
    else
        {
        return aTrieError;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::TryDisconnectOtherConnectionsL()
// -----------------------------------------------------------------------------
//
TBool CCnUiLoginUiCntrlStep::TryDisconnectOtherConnectionsL()
    {
    TBool subOperationOk = EFalse;
    CIMPSSAPSettings* disconnectedSap = CIMPSSAPSettings::NewLC();
    RArray< TIMPSConnectionClient > disconnectedClients;
    CleanupClosePushL( disconnectedClients );


    TInt disconnStatus = iCCntxt.SubOpDisconnectAllL( *disconnectedSap,
                                                      disconnectedClients,
                                                      iSessionSlotID );
    if ( ( disconnStatus == KErrNone ) ||
         ( disconnStatus == KCnUiErrorNoClientsToDisconnect ) )
        {
        //sub operation succeeded
        subOperationOk = ETrue;

        //Add disconnected clients to AA mode rewaker
        const TInt disconnectedClientsCount = disconnectedClients.Count();
        for ( TInt ii = 0; ii < disconnectedClientsCount; ii++ )
            {
            TIMPSConnectionClient client = disconnectedClients[ ii ];
            MCnUiClientPlugin& clientPlugin = iCCntxt.ClientPluginL( client, iSessionSlotID );

            iCCntxt.ConnModeRewaker().AddToAAModeRewakeListL( clientPlugin, iSessionSlotID );
            }
        }

    CleanupStack::PopAndDestroy( 2 ); //disconnectedClients, disconnectedSap
    return subOperationOk;
    }

// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::OpenResourceFileLC()
// !!!Notice!!!. Two variables in cleanupstack after call of this method.
// -----------------------------------------------------------------------------
//
void CCnUiLoginUiCntrlStep::OpenResourceFileLC( RResourceFile& aResourceFile )
    {
    TFileName resourceFileName;
    iRFs.Close();
    User::LeaveIfError( iRFs.Connect() );

    TInt err = iSharedData->GetStringKey( ( TIMPSSharedKeys )KBrandingResourceKey, resourceFileName );
    if ( err || !resourceFileName.Length() )
        {
        CnUiResourceFileName::NearestVariationForCurrentLanguage( iRFs, resourceFileName  );
        }
    aResourceFile.OpenL( iRFs, resourceFileName );
    CleanupClosePushL( aResourceFile );
    aResourceFile.ConfirmSignatureL();
    }

// -----------------------------------------------------------------------------
// CCnUiLoginUiCntrlStep::ReadResourceIntValueL()
// -----------------------------------------------------------------------------
//
TInt CCnUiLoginUiCntrlStep::ReadResourceIntValueL( TInt aResourceId )
    {


    TInt val( 0 );
    TInt err ( KErrNone );

    CRepository* rep = 0;

    TRAP( err, rep = CRepository::NewL( KCRUidIMNG ) );

    if ( err == KErrNone )
        {
        TInt key = aResourceId + KIMCUStartVariationID;

        err = rep->Get( key, val );

        delete rep;
        }

    if ( err != KErrNone )
        {

        RResourceFile resFile;
        OpenResourceFileLC( resFile ); // Two items in cleanup stack.

        aResourceId = aResourceId + RSC_CRRSS_CHAT_VARIATION_IMPSCU_START_ID;

        // read the data to a buffer
        TInt plainResourceId = 0x00000fff & aResourceId; // Remove offset from id
        HBufC8* rawDataBuf = resFile.AllocReadLC( plainResourceId );

        // it's now as ascii code: \x00 for 0, \x01 for 1, etc.
        TUint value = ( *rawDataBuf )[ 0 ];

        CleanupStack::PopAndDestroy( 2 ); // rawDataBuf, resFile

        val = value;
        }

    return val;

    }

//  End of File


