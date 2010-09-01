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
* Description:  UI Session Manager - does the UI related things like branding
*                setting status pane icon etc during login, logout, etc.
*
*/



// INCLUDE FILES
#include "CAExternalInterface.h"
#include "CCAUISessionManager.h"
#include "ChatDebugPrint.h"
#include "ChatDebugAssert.h"
#include "CCAStatusPaneHandler.h"
#include "CCAAppUI.h"
#include "CCAApp.h"
#include "MCASkinVariant.h"
#include "CCAVariantFactory.h"
#include "IMDialogUtils.h"
#include "IMNoteMapper.h"
#include "MCAOpBrandVariant.h"
#include "chatngclient.hrh"
#include "MCAViewSwitcher.h"
#include "IMUtils.h"
#include "CCAIdleWait.h"
#include "ImpsCSPAllErrors.h"
#include "CAViewCreator.h"
#include "cacontexticonmapping.h"
#include "impsbuilddefinitions.h"
#include "CCACommandManagerFactory.h"
#include "CCACommandManager.h"
#include "MCALoginPC.h"
#include "MCAProcessManager.h"
#include "MCASettingsPC.h"
#include "CCAReactiveAuthorization.h"
#include "CAPresenceManager.h"
#include "MCAReactiveAuthPC.h"
#include "MCAMainViewArrayPC.h"

#include <bautils.h>
#include <chatNG.rsg>
#include <gulicon.h>
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsList.h>
#include <CWVSettingsUINGDialog.h>
#include <ImpsServices.h>
#include <bldvariant.hrh>
#include <centralrepository.h>
#include <IMPSServiceSettingsUINGInternalCRKeys.h>
#include <aknenv.h>
#include <StringLoader.h>
#include <barsread.h>
#include <mimpsconnprocessobserverng.h>
#include <MProfileEngine.h>
#include <MProfile.h>
#include <Profile.hrh>
#include <CPEngNWSessionSlotID2.h>
#include <CPEngNWSessionSlot2.h>
#include <LayoutMetaData.cdl.h>
#include <AknLayoutScalable_Avkon.cdl.h>
#include <AknIconUtils.h>
#include <e32property.h>
#include <alwaysonlinemanagerclient.h>
#include    <AknDlgShut.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"


// CONSTANTS

// default always online connection times
// when we set the always online we have to set it
// really on always, thus start and end time are the same here
_LIT( KDefaultScheduleStartTime, "12:00" );
_LIT( KDefaultScheduleEndTime, "12:00" );
_LIT( KIMConnUiResourceFile, "IMPSConnectionUiNG.rsc" );
_LIT( KIMPasswordSaved, "IMPasswordSaved" );

// ============================ MEMBER FUNCTIONS ===============================

//Removed the ownership of iConnUI from this file and made it a reference
//APPUI owns this connection UI instance and shares the same with this component.

// -----------------------------------------------------------------------------
// CCAUISessionManager::CCAUISessionManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAUISessionManager::CCAUISessionManager(
    CIMPSPresenceConnectionUi& aConnUI,
    MCASkinVariant* aSkin,
    MCAOpBrandVariant* aBrand,
    MEikCommandObserver* aCommandObserver,
    MCAViewSwitcher* aViewSwitcher,
    CCAStatusPaneHandler* aStatusPane,
    TDes* aBrandMbmFullPath )

        : iConnUI( aConnUI ),
        iCommandObserver( aCommandObserver ), iSkin( aSkin ), iBrand( aBrand ),
        iStatusPane( aStatusPane ), iBrandMbmFullPath( aBrandMbmFullPath ),
        iViewSwitcher( aViewSwitcher ), iAliasDes( NULL, 0 ),
        iCancelLogin( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CCAUISessionManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAUISessionManager::ConstructL()
    {

    iProfileApi = CreateProfileEngineL();

    CCAAppUi* appUi = static_cast<CCAAppUi*>(
                          CCoeEnv::Static()->AppUi() );


    // reactive authorization
    MCAReactiveAuthPC* reactiveAuthPC = appUi->GetProcessManager().GetReactiveAuthInterface();

    if ( reactiveAuthPC )
        {
        iReactiveAuth = CCAReactiveAuthorization::NewL( *appUi, reactiveAuthPC );
        reactiveAuthPC->AddReactiveAuthObserver( iReactiveAuth );
        }

    }

// -----------------------------------------------------------------------------
// CCAUISessionManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAUISessionManager* CCAUISessionManager::NewL( CIMPSPresenceConnectionUi& aConnUI,
                                                MCASkinVariant* aSkin,
                                                MCAOpBrandVariant* aBrand,
                                                MEikCommandObserver* aCommandObserver,
                                                MCAViewSwitcher* aViewSwitcher,
                                                CCAStatusPaneHandler* aStatusPane,
                                                TDes* aBrandMbmFullPath )
    {
    CCAUISessionManager* self = new( ELeave ) CCAUISessionManager( aConnUI, aSkin,
                                                                   aBrand,
                                                                   aCommandObserver,
                                                                   aViewSwitcher,
                                                                   aStatusPane,
                                                                   aBrandMbmFullPath );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAUISessionManager::LoginL
// -----------------------------------------------------------------------------
//
TBool CCAUISessionManager::LoginL( MCAUiLoginCmdCB::TLoginParams aLoginParams )

    {
	CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
	appUi->SetFSWExitFlag( EFalse );
	
    if ( aLoginParams == MCAUiLoginCmdCB::ELogin )
        {
        if ( IsLoggedIn() )
            {
            return ETrue; // already logged in
            }
        }

    if ( aLoginParams == MCAUiLoginCmdCB::ELogout )
        {
        if ( !IsLoggedIn() )
            {
            return ETrue; // not logged in
            }
        }

	//CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    CCACommandManager* commandManager =  CCACommandManagerFactory::InstanceL()->GetCommandManager();

    MCACommand* loginCommand = static_cast<MCACommand*>( commandManager->NewCommandLC( EChatClientMainViewCmdLogin,
                                                         appUi->GetProcessManager(), *this ) );
    iLoginParams = aLoginParams;
    loginCommand->ExecuteCommandL();
    CleanupStack::PopAndDestroy(); //loginCommand

    return IsLoggedIn();

    }


// Destructor
CCAUISessionManager::~CCAUISessionManager()
    {
    if ( iProfileApi )
        {
        iProfileApi->Release();
        }
    delete iSessionSlotID;
    delete iAlias;


    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    MCAReactiveAuthPC* reactiveAuthPC = appUi->GetProcessManager().GetReactiveAuthInterface();
    if ( reactiveAuthPC )
        {
        reactiveAuthPC->RemoveReactiveAuthObserver( );
        }

    delete iReactiveAuth;

    }

// -----------------------------------------------------------------------------
// CCAUISessionManager::IsLoggedIn
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAUISessionManager::IsLoggedIn() const
    {
    TBool returnValue( EFalse );


    if ( IsOfflineProfileOn() )
        {
        CHAT_DP( D_CHAT_LIT(
                     "CCAUISessionManager::IsLoggedIn, offline profile is ON, returning %d" ),
                 returnValue );
        return returnValue;
        }


    TRAPD( err, returnValue = iConnUI.LoggedInL( EIMPSConnClientIM ) );

    if ( err )
        {
        returnValue = EFalse;
        }

    CHAT_DP( D_CHAT_LIT( "CCAUISessionManager::IsLoggedIn returning %d" ),
             returnValue );

    // if there are no slots in the system then we get KErrNotFound
    // and we don't show it to the user
    if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
        {
        CActiveScheduler::Current()->Error( err );
        }


    return returnValue;
    }
// -----------------------------------------------------------------------------
// CCAUISessionManager::ShowProcessingNoteLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAUISessionManager::ShowProcessingNoteLC()
    {
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    appUi->ShowWaitDialogLC( R_QTN_GEN_NOTE_PROCESSING, ETrue );
    }



// ---------------------------------------------------------
// CCAUISessionManager::SetStatusPaneIconsL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::SetStatusPaneIconsL( TBool aLandscapeOnly /*= EFalse*/ )
    {
    TBool landscape( Layout_Meta_Data::IsLandscapeOrientation() );
    if ( !landscape && aLandscapeOnly )
        {
        // do nothing if we're not in landscape mode and update
        // was requested only for landscape
        return;
        }

    // If client has not gave handle to statuspane or skin then do nothing
    if ( !iStatusPane || !iSkin )
        {
        return;
        }
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    CAknTitlePane* title = static_cast< CAknTitlePane* >
                           ( CEikonEnv::Static()->AppUiFactory()->StatusPane()->ControlL(
                                 TUid::Uid( EEikStatusPaneUidTitle ) ) );
    // get the visibility of context pane
    CEikStatusPane* statusPane = CEikonEnv::Static()->AppUiFactory()->
                                 StatusPane();
    CAknContextPane* contextPane = static_cast<CAknContextPane*>( statusPane->ControlL(
                                                                      TUid::Uid( EEikStatusPaneUidContext ) ) );
    TBool cpVisible( contextPane ? contextPane->IsVisible() : EFalse );

    TBool useTitle( !cpVisible || landscape );

    // Load online-icon
    TInt32 iconId( useTitle ? EMenu_im_off_title : EMenu_im_off );
    TInt32 iconMaskId( useTitle ? EMenu_im_off_title_mask : EMenu_im_off_mask );


    TBool alreadyLoggedOut( EFalse );
    CCACommandManager* commandManager =  CCACommandManagerFactory::InstanceL()->GetCommandManager();
    MCASessionHandlerCmd* sessHandlerCmd =  commandManager->GetSessionHandlerCmd() ;
    if ( sessHandlerCmd )
        {
        alreadyLoggedOut = sessHandlerCmd->IsAlreadyLoggedOut();
        }

    if ( IsLoggedIn() && !alreadyLoggedOut )
        {
        //CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
        MCAMainViewArrayPC* mainViewArrayPC = appUi->GetProcessManager().GetArrayInterface();

        // Resolve icon by own status & screen orientation
        switch (  mainViewArrayPC->StatusL() )
            {
            case TEnumsPC::EOffline:
                {
                if ( useTitle )
                    {
                    iconId = EMenu_im_invisible_title;
                    iconMaskId = EMenu_im_invisible_title_mask;
                    }
                else // portrait
                    {
                    iconId = EMenu_im_invisible;
                    iconMaskId = EMenu_im_invisible_mask;
                    }
                break;
                }
            case TEnumsPC::EAway:
                {
                if ( useTitle )
                    {
                    iconId = EMenu_im_away_title;
                    iconMaskId = EMenu_im_away_title_mask;
                    }
                else // portrait
                    {
                    iconId = EMenu_im_away;
                    iconMaskId = EMenu_im_away_mask;
                    }
                break;
                }
            case TEnumsPC::EBusy:
                {
                if ( useTitle )
                    {
                    iconId = EMenu_im_busy_title;
                    iconMaskId = EMenu_im_busy_title_mask;
                    }
                else // portrait
                    {
                    iconId = EMenu_im_busy;
                    iconMaskId = EMenu_im_busy_mask;
                    }
                break;
                }
            case TEnumsPC::EOnline: // Flowthrough
            default:
                {
                if ( useTitle )
                    {
                    iconId = EMenu_im_on_title;
                    iconMaskId = EMenu_im_on_title_mask;
                    }
                else // portrait
                    {
                    iconId = EMenu_im_on;
                    iconMaskId = EMenu_im_on_mask;
                    }
                break;
                }
            }

        }


    CGulIcon* connStatusIcon = iSkin->LoadBitmapL( iconId,
                                                   iconMaskId,
                                                   *iBrandMbmFullPath );
    connStatusIcon->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::PushL( connStatusIcon );


    // get the visibility of context pane

    if ( useTitle )
        {
        //Icon is not required in Settings
        if ( appUi->IsSettingsDialogActive() )
            {
            title->SetSmallPicture( NULL, NULL, EFalse );
            }
        else
            {
            // with thin status pane we are setting the status icon to title pane
            title->SetSmallPicture( connStatusIcon->Bitmap(), connStatusIcon->Mask(), ETrue );
            // ownership was transferred
            connStatusIcon->SetBitmapsOwnedExternally( ETrue );
            }

        }
    else
        {
        //Icon is not required in Settings
        if ( appUi->IsSettingsDialogActive() )
            {
            iStatusPane->SetPicture( NULL, NULL );
            }
        else
            {
            // in portrait mode we are setting the status icon to status pane
            iStatusPane->SetPicture( connStatusIcon->Bitmap(), connStatusIcon->Mask() );
            // ownership was transferred
            connStatusIcon->SetBitmapsOwnedExternally( ETrue );
            }



        if ( title )
            {
            // and title pane icon must be cleared
            title->SetSmallPicture( NULL, NULL, EFalse );
            }
        }

    CleanupStack::PopAndDestroy( connStatusIcon );
    }



// ---------------------------------------------------------
// CCAUISessionManager::CheckCurrentServerDataL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAUISessionManager::CheckCurrentServerDataL(
    TBool /*aShowNote*/ /*= ETrue*/ )
    {
    // SAP settings store
    CIMPSSAPSettingsStore* sapSettingsStore = CIMPSSAPSettingsStore::NewLC();
    TInt exitReason( 0 );

    if ( sapSettingsStore->SAPCountL( EIMPSIMAccessGroup ) == 0 )
        {

        TUid activeView( TUid::Null() );
        if ( iStatusPane && iViewSwitcher )
            {
            iStatusPane->ClearNaviPaneL();
            //iStatusPane->ProhibitTabGroup(); //For Later Ref.
            activeView = iViewSwitcher->ActiveViewId();
            }

        CWVSettingsUIDialog* dlg = CWVSettingsUIDialog::NewL();
        CleanupStack::PushL( dlg );

        TPtrC empty( KNullDesC );

        // The fourth argument is newly added, that is for aResourceVariationFile, which is passed as empty
        TInt retVal( dlg->RunDialogLD( *sapSettingsStore,
                                       NULL,
                                       empty,
                                       empty,
                                       exitReason,
                                       NULL,
                                       &dlg,
                                       EFalse,
                                       EWVSettingsDefineServer ) );
        CleanupStack::Pop(); // dlg
        CCAAppUi* appUi = static_cast<CCAAppUi*>(  CCoeEnv::Static()->AppUi() );

        appUi->GetProcessManager().GetArrayInterface()->ResetArray( ETrue );

        if ( iStatusPane && iViewSwitcher )
            {
            //iStatusPane->ProhibitTabGroup( EFalse ); //For Later Ref.
            iStatusPane->ShowTabGroupL( activeView );
            }

        if ( retVal == EWVSettingsViewExitCalled && iCommandObserver )
            {
            iCommandObserver->ProcessCommandL( EChatClientCmdExit );
            }

        if ( exitReason == EServerSaved )
            {
            // update branding
            BrandUIL();

            }
        else
            {
            // If user didn't create any servers, he/she must be able
            // to use other views also (eg. settings) so let's finalize the
            // construction
            CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
            appUi->FinalizeChatConstructionL();
            }
        }
    CleanupStack::PopAndDestroy( sapSettingsStore );

    return exitReason == EServerSaved; // return "did user save server"
    }
// ---------------------------------------------------------
// CCAUISessionManager::HandleSapAccessEventEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::HandleSapAccessEventEventL( const TDesC& aServerName,
                                                      const TDesC& /*aServerURL*/,
                                                      MIMPSConnProcessUi* aConnProcessUi )
    {
    // If no branding support available
    if ( !iBrand )
        {
        return;
        }

    TFileName brandedResource;
    TPtrC resId( KIMConnUiResourceFile() );
    TPtrC server( aServerName );

    iBrand->CurrentResourceFileL( server, resId, brandedResource );

    if ( brandedResource.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( CEikonEnv::Static()->FsSession(),
                                        brandedResource );
        }

#ifdef _DEBUG

    TPtrC ptr( brandedResource );
    CHAT_DP( D_CHAT_LIT(
                 "CCAUISessionManagerCmd::HandleSapAccessEventEventL \
            setting %S to be connUI resfile" ),
             &ptr );
#endif

    aConnProcessUi->UseResourceFileL( brandedResource );

    TInt err = RProperty::Set( KBrandingUid, KBrandingResourceKey, brandedResource );

    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    appUi->ChangeConnUiAgentResourceL( brandedResource );
    }


// ---------------------------------------------------------
// CCAUISessionManager::ShowPostLoginNotesL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::ShowPostLoginNotesL ( TBool aCurrentLoginIsFirst,
                                                TBool aManualLogin )
    {

    iCurrentLoginIsFirst = aCurrentLoginIsFirst;


    // Show post-login notes if not always online case
    if ( aManualLogin )
        {
        // make sure the order stays intact, otherwise
        // certain flags (iUidAndPasswordSaved and iCurrentLoginIsFirst)
        // will not be set correctly and behaviour will be buggy
        ShowWelcomeNoteL();
        if ( !IsLoggedIn() )
            {
            // Forced log out may happen while
            // note is being displayed, don't continue
            // with login
            return;
            }
        ShowPasswordQueriesL();
        if ( !IsLoggedIn() )
            {
            // Forced log out may happen while
            // queries are being displayed, don't continue
            // with login
            return;
            }
        ShowApplicationLaunchQueryL();
        if ( !IsLoggedIn() )
            {
            // Forced log out may happen while
            // query is being displayed, don't continue
            // with login
            return;
            }
        ShowAlwaysOnlineAfterFirstLoginL();
        if ( !IsLoggedIn() )
            {
            // Forced log out may happen while
            // query is being displayed, don't continue
            // with login
            return;
            }
        }


    // ask for own alias if it's not defined (if alias is supported)
    AskAliasL();

    if ( !IsLoggedIn() )
        {
        // Forced log out may happen while
        // query is being displayed, don't continue
        // with login
        return;
        }

    }
// ---------------------------------------------------------
// CCAUISessionManager::AskAliasL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::AskAliasL()
    {
    iSupportAlias = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_HAVE_ALIAS );
    if ( ! iSupportAlias )
        {
        // no alias
        return;
        }

    TBool aliasMandatory =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_ALIAS_MANDATORY );

    // check if there's already an alias in the client

    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );

    HBufC* ownAlias = appUi->GetProcessManager().GetSettingsInterface()->
                      GetSapSettingValuePCL( TEnumsPC::EOwnAlias );

    TBool aliasDefined = ( ownAlias != NULL ); // CSI: 64 # pointer -> boolean.

    CleanupStack::PushL( ownAlias );
    if ( ! aliasDefined )
        {
        // 1. Show "you need to define alias"-infonote with OK
        // - show only be shown when alias is mandatory
        if ( aliasMandatory )
            {
            IMDialogUtils::DisplayQueryDialogL( R_QTN_CHAT_NO_ALIAS_DEF );
            }

        if ( !IsLoggedIn() )
            {
            // Forced log out may happen while
            // note is being displayed, don't continue
            // with text query
            CleanupStack::PopAndDestroy( ownAlias );
            return;
            }


        // Show alias data query
        if ( iAlias )
            {
            delete iAlias;
            iAlias = NULL;
            }

        iAlias = HBufC::NewL( KAliasMaxLength );
        iAliasDes.Set( iAlias->Des() );

        TInt result = IMDialogUtils::DisplayTextQueryDialogL( iAliasDes,
                                                              R_QTN_CHAT_ALIAS_PROMPT,
                                                              R_CHATCLIENT_LOGIN_ALIAS_QUERY,
                                                              ETrue // T9
                                                            );

        if ( !IsLoggedIn() )
            {
            // Forced log out may happen while
            // query is being displayed, don't continue
            // with alias publishing
            CleanupStack::PopAndDestroy( ownAlias );
            return;
            }

        if ( result == EAknSoftkeyDone || result == EAknSoftkeyOk )
            {
            aliasDefined = ETrue;
            iCancelLogin = EFalse;
            }
        else
            {
            if ( aliasMandatory )
                {
                // when the user cancels we must return to the
                // state before the alias query,
                //  so try to cancel the login
                iCancelLogin = ETrue;
                }
            else
                {
                //  continue login normally with empty alias
                iCancelLogin = EFalse;
                aliasDefined = EFalse;
                TRAP_IGNORE( appUi->GetProcessManager().GetSettingsInterface()->
                             SetSapSettingValuePCL ( TEnumsPC::EOwnAlias , KNullDesC ) );
                }
            }

        CleanupStack::PopAndDestroy( ownAlias );
        }
    else
        {
        // already have an alias defined
        delete iAlias;
        iAlias = ownAlias;
        iAliasDes.Set( iAlias->Des() );
        CleanupStack::Pop( ownAlias );
        }

    if ( aliasDefined )
        {
        // publish and subscribe here all variated attributes.

        TBool exitLoop( EFalse );
        while ( !exitLoop )
            {
            // show saving alias name wait note
            CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
            appUi->ShowWaitDialogL( R_QTN_CHAT_SAVING_ALIAS, ETrue );

            // store alias to settings: PUBLISH alias attribute in IM Presence
            // when it gets the settings changed event.


            TRAPD( err, appUi->GetProcessManager().GetSettingsInterface()->
                   SetSapSettingValuePCL ( TEnumsPC::EOwnAlias , iAliasDes ) );

            TBool invalidAlias( EFalse );

            if ( err == ECSPInvalidPresenceValue )
                {
                // this means the alias is too long or invalid
                // we cannot show the note "in the normal" way since this
                // error requires a special note in this case
                // so now we give KErrNone to DismissWaitDialogL and show
                // the note here
                invalidAlias = ETrue;
                err = KErrNone;
                }
            // dismiss wait
            appUi->DismissWaitDialogL( err, EFalse, KNullDesC );

            if ( invalidAlias )
                {
                // show error note
                HBufC* text = StringLoader::LoadLC( R_QTN_CHAT_TOO_LONG_ALIAS_TEXT );
                IMDialogUtils::DisplayErrorNoteL( *text );
                CleanupStack::PopAndDestroy( text );
                // return to editing
                TInt result = IMDialogUtils::DisplayTextQueryDialogL( iAliasDes,
                                                                      R_QTN_CHAT_ALIAS_PROMPT,
                                                                      R_CHATCLIENT_LOGIN_ALIAS_QUERY,
                                                                      ETrue // T9
                                                                    );
                if ( result == EAknSoftkeyDone || result == EAknSoftkeyOk )
                    {
                    aliasDefined = ETrue;
                    iCancelLogin = EFalse;
                    }
                else
                    {
                    if ( aliasMandatory )
                        {
                        //  when the user cancels we must return to the
                        //  state before the alias query,
                        //  so try to cancel the login
                        iCancelLogin = ETrue;
                        }
                    else
                        {
                        //  continue login normally with empty alias
                        iCancelLogin = EFalse;
                        aliasDefined = ETrue;
                        TRAP_IGNORE( appUi->GetProcessManager().GetSettingsInterface()->
                                     SetSapSettingValuePCL ( TEnumsPC::EOwnAlias , KNullDesC ) );
                        }
                    exitLoop = ETrue;
                    }

                }
            else
                {
                // set the alias for own status item
                exitLoop = ETrue;
                MCALoginPC*  loginPC = appUi->GetProcessManager().GetLogInInterface();
                loginPC->SetAliasL( iAliasDes );
                }
            }
        }

    }
// ---------------------------------------------------------
// CCAUISessionManager::ShowWelcomeNoteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::ShowWelcomeNoteL()
    {
    if ( ! IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_WELCOME_NOTE  ) )
        {
        // welcome note not enabled
        return;
        }

    if ( iCurrentLoginIsFirst )
        {
        HBufC* welcomeTxt =
            CCoeEnv::Static()->AllocReadResourceLC(
                R_CHAT_WELCOME_NOTE );

        HBufC* welcomeHeading =
            CCoeEnv::Static()->AllocReadResourceLC(
                R_CHAT_WELCOME_NOTE_HEADING );

        if ( welcomeTxt->Length() > 0 )
            {
            IMDialogUtils::DisplayMessageQueryDialogL(
                R_CHATCLIENT_WELCOME_NOTE_DIALOG,
                *welcomeTxt, *welcomeHeading,
                EFalse  // T9
            );
            }

        CleanupStack::PopAndDestroy( 2, welcomeTxt );
        }

    }
// ---------------------------------------------------------
// CCAUISessionManager::CancelLoginValue
// ---------------------------------------------------------
//
TBool CCAUISessionManager::CancelLoginValue()
    {
    return iCancelLogin;
    }
// ---------------------------------------------------------
// CCAUISessionManager::IMUIPIConForcedClear
// ---------------------------------------------------------
//
void CCAUISessionManager::IMUIPIConForcedClear()
    {
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    appUi->IMUIPIConForcedClear();
    }

// ---------------------------------------------------------
// CCAUISessionManager::IMUIPIConForcedClear
// ---------------------------------------------------------
//
void CCAUISessionManager::ShutDialogsL()
    {
    CHAT_DP_FUNC_DP( "CCAUISessionManager", "ShutDialogsL..." );
    AknDialogShutter::ShutDialogsL( *CEikonEnv::Static() );
    }

// ---------------------------------------------------------
// CCAUISessionManager::CaptureEventsL
// ---------------------------------------------------------
//
void CCAUISessionManager::CaptureEventsL()
    {
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    appUi->CaptureEventsL();
    }
// ---------------------------------------------------------
// CCAUISessionManager::ReleaseCapturingL
// ---------------------------------------------------------
//
void CCAUISessionManager::ReleaseCapturingL()
    {
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    appUi->ReleaseCapturingL();
    }
// ---------------------------------------------------------
// CCAUISessionManager::RefreshViewAfterLoginL
// ---------------------------------------------------------
//
void CCAUISessionManager::RefreshViewAfterLoginL()
    {
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    CCAIdleWait::IdleWaitL( &( appUi->RefreshWaitFlag() ) );
    }


// ---------------------------------------------------------
// CCAUISessionManager::ShowPasswordQueriesL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::ShowPasswordQueriesL()
    {
    // Password save query logic
    // 1. if first login and id/pw not already saved, then ask to save pw
    // 2. if password has changed, then ask to save pw
    // 3. if uid has changed, then ask to save uid + pw

    iUidAndPasswordSaved = EFalse;

    TBool passwordQueries =
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SAVE_PW_QUERY );

    if ( ! passwordQueries )
        {
        // not enabled
        return;
        }

    CIMPSSAPSettings* currentSAP = CurrentSAPLC();

    // get logged in SAP from IMPSCUI
    CIMPSSAPSettings* loggedInSap = CIMPSSAPSettings::NewLC();
    iConnUI.GetLoggedInSapL( *iSessionSlotID, *loggedInSap );

    // get current SAP from SSS
    CIMPSSAPSettingsStore* sapStore = CIMPSSAPSettingsStore::NewLC();
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    sapStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );
    TInt index = KErrNotFound;

    sapList->FindNameL( currentSAP->SAPName(), index );
    if ( index == KErrNotFound )
        {
        // something went wrong, didn't find it
        User::Leave( index );
        }

    CIMPSSAPSettings* storedSap = CIMPSSAPSettings::NewLC();
    TUint32 sapUid = sapList->UidForIndex( index );
    sapStore->GetSAPL( sapUid, storedSap );

    // now we have the logged in SAP and the SAP from SSS,
    // find out userid and passwords
    const TDesC& loggedInUid = loggedInSap->SAPUserId();
    const TDesC& loggedInPassWd = loggedInSap->SAPUserPassword();

    const TDesC& storedUid = storedSap->SAPUserId();
    const TDesC& storedPassWd = storedSap->SAPUserPassword();

    // find out any changes
    TBool uidSame = 0 == storedUid.Compare( loggedInUid );
    TBool pwSame  = 0 == storedPassWd.Compare( loggedInPassWd );

    // Check if pw has been saved earlier
    TInt pwSaved = 0;
    TInt sapError = -1;
    sapError = storedSap->GetOpaqueInt( KIMPasswordSaved, pwSaved );

    if ( sapError != KErrNotFound )
        {
        User::LeaveIfError( sapError );
        }

    if ( iCurrentLoginIsFirst && ( !pwSame || !uidSame ) )
        {
        // first login to this server (without saved login data)
        TInt savePw = IMDialogUtils::DisplayYesNoConfirmationDialogL( R_QTN_CHAT_LOGIN_SAVE_PW );
        if ( savePw )
            {
            ShowPasswordSavingWarningL();
            // save username + password to SAP settings
            storedSap->SetSAPUserIdL( loggedInUid );
            storedSap->SetSAPUserPasswordL( loggedInPassWd );
            // set password saved indication
            storedSap->SetOpaqueInt( KIMPasswordSaved, ETrue );

            // save directly to SSS
            sapStore->UpdateOldSAPL( storedSap, sapUid );

            iUidAndPasswordSaved = ETrue;
            }
        else
            {
            // save only the username to SAP settings
            storedSap->SetSAPUserIdL( loggedInUid );

            // save directly to SSS
            sapStore->UpdateOldSAPL( storedSap, sapUid );
            }
        }
    else if ( pwSaved && !pwSame && uidSame && !iCurrentLoginIsFirst )
        {
        // Not first login, saved password (not uid) has changed
        TInt savePw = IMDialogUtils::DisplayYesNoConfirmationDialogL( R_QTN_CHAT_LOGIN_SAVE_PW );
        if ( savePw )
            {
            ShowPasswordSavingWarningL();
            // save new password to SAP settings
            storedSap->SetSAPUserPasswordL( loggedInPassWd );

            // save directly to SSS
            sapStore->UpdateOldSAPL( storedSap, sapUid );

            iUidAndPasswordSaved = ETrue;
            }

        }
    else if ( !uidSame )
        {
        // not first login and user id has changed
        if ( IMDialogUtils::DisplayYesNoConfirmationDialogL( R_QTN_CHAT_LOGIN_SAVE_USERNAME_PW ) )
            {
            ShowPasswordSavingWarningL();
            // user wants to save both uid and pw
            storedSap->SetSAPUserIdL( loggedInUid );
            storedSap->SetSAPUserPasswordL( loggedInPassWd );

            // save directly to SSS
            sapStore->UpdateOldSAPL( storedSap, sapUid );

            iUidAndPasswordSaved = ETrue;
            }
        }
    else if ( iCurrentLoginIsFirst && pwSame && uidSame )
        {
        // This is first login with already saved login data
        iUidAndPasswordSaved = ETrue;
        }

    CleanupStack::PopAndDestroy( 5, currentSAP ); // storedSap, sapList, sapStore, loggedInSap, currentSAP
    }

// ---------------------------------------------------------
// CCAUISessionManager::ApplicationLaunchQueryL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::ShowApplicationLaunchQueryL()
    {
    TBool launchQuery = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_APP_LAUNCH_QUERY );

    if ( ! launchQuery || ! iUidAndPasswordSaved )
        {
        // not enabled or userid + password were not saved
        return;
        }

    TInt chatLoginType = IMUtils::WVSettingsChatLoginValue();
    TBool isDefault = CurrentServerIsDefaultL();

    if ( chatLoginType == EWVSettingsChatLoginApplicationLaunch
         && isDefault )
        {
        // application launch active and this server is default server, so
        // get out
        return;
        }

    TInt activate = IMDialogUtils::DisplayYesNoConfirmationDialogL( R_QTN_CHAT_LOGIN_ACTIVATE_APPLAUNCH );

    if ( activate )
        {
        ShowPasswordSavingWarningL();
        // change login type for Application Launch
        SetLoginTypeL( EWVSettingsChatLoginApplicationLaunch );

        // change default server to be the current one
        SetCurrentServerAsDefaultL();
        }
    else
        {
        // login type will become Manual
        SetLoginTypeL( EWVSettingsChatLoginManual );
        }

    }

// ---------------------------------------------------------
// CCAUISessionManager::ShowAlwaysOnlineAfterFirstLogin
// ---------------------------------------------------------

void CCAUISessionManager::ShowAlwaysOnlineAfterFirstLoginL()
    {
    // check if login type need to be set as always online in home
    // network according to variation
    if ( ! IMUtils::IntResourceValueL(
             RSC_CHAT_VARIATION_ALWAYSONLINE_AFTER_FIRST_LOGIN ) )
        {
        return;
        }

    if ( iCurrentLoginIsFirst )
        {
        SetCurrentServerAsDefaultL();

        if ( LoginType() == EIMPSManualLogin )
            {
            TInt retVal(
                IMDialogUtils::DisplayYesNoConfirmationDialogL(
                    R_QTN_CHAT_SET_AO_ON ) );

            ShowPasswordSavingWarningL();


            if ( retVal == EAknSoftkeyOk || retVal == EAknSoftkeyYes )
                {
                // change login type to always online in home network
                SetLoginTypeL( EWVSettingsChatLoginAutoAlways );
                }
            else
                {
                // change login type to application launch
                SetLoginTypeL( EWVSettingsChatLoginApplicationLaunch );
                }
            }
        }
    }
// ---------------------------------------------------------
// CCAUISessionManager::SetLoginTypeL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::SetLoginTypeL( TWVSettingsChatLoginValues aLoginType )
    {
    CRepository* repository = CRepository::NewLC( KWVSettingsCenRepUid );

    if ( aLoginType == EWVSettingsChatLoginAutoAlways ||
         aLoginType == EWVSettingsChatLoginAutoInHomeNW )
        {
        User::LeaveIfError(
            repository->Set( KIMPSCRIMConnDays, EWVSettingsAll ) );
        User::LeaveIfError(
            repository->Set( KIMPSCRIMConnStart, KDefaultScheduleStartTime ) );
        User::LeaveIfError(
            repository->Set( KIMPSCRIMConnEnd, KDefaultScheduleEndTime ) );
        }

    User::LeaveIfError( repository->Set( KIMPSCRChatLogin, aLoginType ) );
    CleanupStack::PopAndDestroy( repository );
#ifndef RD_30_COMPATIBILITY_MODE

    if ( aLoginType == EWVSettingsChatLoginAutoAlways ||
         aLoginType == EWVSettingsChatLoginAutoInHomeNW )
        {
        // Start AO plugin.
        RAlwaysOnlineClientSession session;
        User::LeaveIfError( session.Connect() );

        TUid uid = { KIMPSALWAYSONLINEUID3 };
        TPckgBuf< TUid > param = uid;

        session.RelayCommandL( EServerAPIBaseCommandStart, param );
        session.Close();
#endif // RD_30_COMPATIBILITY_MODE
        }
    }

// ---------------------------------------------------------
// CCAUISessionManager::LoginType
// Central repository version
// (other items were commented in a header).
// ---------------------------------------------------------
//
TIMPSLoginType CCAUISessionManager::LoginType()
    {
    CHAT_DP( D_CHAT_LIT( "CCAUISessionManager::LoginType" ) );

    TInt readValue( KErrCouldNotConnect ); // initialize to an error value

    TIMPSLoginType loginType( EIMPSManualLogin );

    CRepository* cenrep = NULL;
    TRAPD( err, cenrep = CRepository::NewL( KWVSettingsCenRepUid ) );

    if ( err != KErrNone )
        {
        // creation of cenrep failed -> return
        return loginType;
        }

    TRAP( err,
          CleanupStack::PushL( cenrep );
          err = cenrep->Get( KIMPSCRChatLogin, readValue );
          CleanupStack::PopAndDestroy( cenrep );
        ); // TRAP
    cenrep = NULL;

    if ( err != KErrNone )
        {
        // there was an error
        return loginType;
        }

    if ( ( readValue == EWVSettingsChatLoginAutoAlways ) ||
         ( readValue == EWVSettingsChatLoginAutoInHomeNW ) )
        {
        return EIMPSAAConnectionStart;
        }
    else if ( readValue == EWVSettingsChatLoginApplicationLaunch )
        {
        return EIMPSApplicationLaunch;
        }
    else
        {
        return EIMPSManualLogin;
        }
    }


// -----------------------------------------------------------------------------
// CCAUISessionManager::BrandUIL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAUISessionManager::BrandUIL( TBool aRefreshView /*= ETrue */,
                                     const TDesC& aServerName /*= KNullDesC*/ )
    {
    // If needed handles are not given, then do not brand
    if ( !iBrand || !iBrandMbmFullPath )
        {
        return EFalse;
        }

    CHAT_DP( D_CHAT_LIT( "CCAUISessionManager::BrandUIL( %d, %S )" ),
             aRefreshView, &aServerName );
    TPtrC pName;
    CIMPSSAPSettings* sap = NULL;

    if ( aServerName != KNullDesC )
        {
        pName.Set( aServerName );
        }
    else
        {
        // Get the name of logged in server
        sap = CurrentSAPLC();
        pName.Set( sap->SAPName() );
        }

    CHAT_DP( D_CHAT_LIT( "CCAUISessionManager::BrandUIL activating brand for %S" ),
             &pName );

    // Activate branded UI based on servername
    TFileName tempPath = *iBrandMbmFullPath;
    iBrand->ActivateBrandL( pName, *iBrandMbmFullPath );
    TBool refreshNeeded( tempPath.Compare( *iBrandMbmFullPath ) != 0 );

    // Set branding flag for connection UI
    TFileName brandedResource;
    TPtrC resId( KIMConnUiResourceFile() );

    iBrand->CurrentResourceFileL( pName, resId, brandedResource );

    if ( brandedResource.Length() > 0 )
        {
        BaflUtils::NearestLanguageFile( CEikonEnv::Static()->FsSession(),
                                        brandedResource );
        }

    TInt err = RProperty::Set( KBrandingUid, KBrandingResourceKey, brandedResource );

    CHAT_DP( D_CHAT_LIT(
                 "CCAUISessionManager::BrandUIL activation result (need to refresh): %d" ),
             refreshNeeded );

    // Check if anything really changed to avoid unnecessary drawing
    if ( aRefreshView || refreshNeeded )
        {
        // Update status pane
        SetStatusPaneIconsL();
        iRefresh = ETrue;
        }

    if ( sap )
        {
        CleanupStack::PopAndDestroy( sap );
        }

    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    appUi->ReadBrandedResourcesL();

    if ( refreshNeeded )
        {
        // notify resource observers
        appUi->NotifyResourceChangeL();
        }

    // Inform storage about the new "show nickname" value
    TBool showNickname = IMUtils::IntResourceValueL(
                             RSC_CHAT_VARIATION_HAVE_NICKNAME );

    MCALoginPC*  loginPC = appUi->GetProcessManager().GetLogInInterface();
    loginPC->SetShowNicknameL( showNickname );

    return refreshNeeded;
    }

// -----------------------------------------------------------------------------
// CCAUISessionManager::CurrentSAPLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CIMPSSAPSettings* CCAUISessionManager::CurrentSAPLC()
    {
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();

    // If session is active, then current user-id should be retrieved from
    // connection UI instead of SAP settings store
    if ( IsLoggedIn() )
        {
        CHAT_DP_TXT(
            "CCAUISessionManager::CurrentSAPLC returning SAP based on ConnUI" );
        if ( !iSessionSlotID )
            {
            iSessionSlotID =
                iConnUI.GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
            }
        iConnUI.GetLoggedInSapL( *iSessionSlotID, *sap );
        }
    else
        {
        CHAT_DP_TXT(
            "CCAUISessionManager:: \
                CurrentSAPLC returning SAP based on SAPSettingsStore" );
        CIMPSSAPSettingsStore* sapStore = CIMPSSAPSettingsStore::NewLC();
        sapStore->GetDefaultL( sap, EIMPSIMAccessGroup );
        CleanupStack::PopAndDestroy( sapStore );
        }

    CHAT_DP( D_CHAT_LIT( "CCAUISessionManager::CurrentSAPLC sap uid %d" ),
             sap->Uid() );
    return sap;

    }


// -----------------------------------------------------------------------------
// CCAUISessionManager::RefreshCurrentViewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAUISessionManager::RefreshCurrentViewL()
    {
    if ( iViewSwitcher && iRefresh )
        {
        // this is for updating settings view
        iViewSwitcher->SwitchViewL( KUidRefreshView );
        iRefresh = EFalse;
        return ETrue;
        }
    return EFalse;
    }



// -----------------------------------------------------------------------------
// CCAUISessionManager::SetIMPSServices()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAUISessionManager::SetIMPSServices( TPEngWVCspServicesTree2 aImpsServices,
                                           TBool aBrandUIFlag, TBool aRefreshFlag  )
    {
    iImpsServices = aImpsServices;
    if ( aBrandUIFlag )
        {
        TRAP_IGNORE( BrandUIL( aRefreshFlag ) );
        }
    }

// -----------------------------------------------------------------------------
// CCAUISessionManager::IsOfflineProfileOn
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAUISessionManager::IsOfflineProfileOn() const
    {
    return ( EProfileOffLineId == iProfileApi->ActiveProfileId() );
    }


// ---------------------------------------------------------
// CCAUISessionManager::SetcurrentServerAsDefaultL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::SetCurrentServerAsDefaultL()
    {
    CIMPSSAPSettings* currentSAP = CurrentSAPLC();

    CIMPSSAPSettingsStore* sapStore = CIMPSSAPSettingsStore::NewLC();
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    sapStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

    // find our sap.. because logged in SAP has UID of zero, we must
    // find the correct UID manually from list
    TInt index( KErrNotFound );
    sapList->FindNameL( currentSAP->SAPName(), index );
    if ( index == KErrNotFound )
        {
        User::Leave( index );
        }

    CIMPSSAPSettings* defaultsap = CIMPSSAPSettings::NewLC();
    sapStore->GetDefaultL( defaultsap, EIMPSIMAccessGroup );

    // check if current sap is default, compare to EXACT name.
    if ( 0 != currentSAP->SAPName().Compare( defaultsap->SAPName() ) )
        {
        // it's not, so change current sap as default sap
        sapStore->SetToDefaultL( sapList->UidForIndex( index ),
                                 EIMPSIMAccessGroup );
        }

    CleanupStack::PopAndDestroy( 4, currentSAP ); // defaultsap, saplist, sapStore, currentSAP
    }

// ---------------------------------------------------------
// CCAUISessionManager::CurrentServerIsDefaultL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAUISessionManager::CurrentServerIsDefaultL()
    {
    CIMPSSAPSettings* currentSAP = CurrentSAPLC();

    CIMPSSAPSettingsStore* sapStore = CIMPSSAPSettingsStore::NewLC();
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    sapStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

    // find our sap.. because logged in SAP has UID of zero, we must
    // find the correct UID manually from list
    TInt index( KErrNotFound );
    sapList->FindNameL( currentSAP->SAPName(), index );
    if ( index == KErrNotFound )
        {
        User::Leave( index );
        }

    CIMPSSAPSettings* defaultsap = CIMPSSAPSettings::NewLC();
    sapStore->GetDefaultL( defaultsap, EIMPSIMAccessGroup );

    // check if current sap is default, compare to EXACT name.
    TBool isDefault = 0 == currentSAP->SAPName().Compare( defaultsap->SAPName() );
    CleanupStack::PopAndDestroy( 4, currentSAP ); // defaultsap, saplist, sapStore, currentSAP

    return isDefault;
    }
// ---------------------------------------------------------
// CCAUISessionManager::DisplayDomainSelectionQueryL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAUISessionManager::DisplayDomainSelectionQueryL( TDes& aSelectedDomain )
    {
    CIMPSSAPSettings* sap = CurrentSAPLC();
    TInt retVal = iConnUI.DisplayDomainSelectionQueryL( aSelectedDomain, sap );
    CleanupStack::PopAndDestroy( sap );
    return retVal;
    }

// ---------------------------------------------------------
// CCAUISessionManager::IsBackgroundTaskRunning
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAUISessionManager::IsBackgroundTaskRunning()
    {
    CCACommandManager* commandManager =  CCACommandManagerFactory::InstanceL()->GetCommandManager();
    MCASessionHandlerCmd* sessionHandlerCmd = commandManager->GetSessionHandlerCmd();
    return sessionHandlerCmd->IsBackgroundTaskRunning();
    }

// ---------------------------------------------------------
// CCAUISessionManager::IsSupported
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAUISessionManager::IsSupported( TSupportedFeature aFeature )
    {
    switch ( aFeature )
        {
        case EGroup :
            {
            return iImpsServices.iGroupFeat.FeatureSupported();
            }
        case ESearch :
            {
            return iImpsServices.iFundamentalFeat.FunctionSupported(
                       KPEngFFSearchFunction );
            }
        case EInvite :
            {
            return iImpsServices.iFundamentalFeat.FunctionSupported(
                       KPEngFFInviteFunction );
            }
        case EBlock :
            {
            return iImpsServices.iIMFeat.FunctionSupported(
                       KPEngIMAuthorFunctions );
            }
        case EAttrList :
            {
            return iImpsServices.iPresenceFeat.FunctionSupported(
                       KPEngWVAttrListFunction );
            }
        case ECLIMod:
            {
            // contact list creation/deletion is possible only if
            // both CCLI and DCLI are supported
            TBool returnValue( ETrue );
            returnValue &=
                iImpsServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncCCLI );
            returnValue &=
                iImpsServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncDCLI );
            return returnValue;
            }
        case EGroupRejectList:
            {
            return iImpsServices.iGroupFeat.SubFunctionSupported(
                       KPEngGRSubFuncREJEC );
            }
        default:
            {
            return EFalse; // Unknown features are not supported
            }
        }
    }

// ---------------------------------------------------------
// CCAUISessionManager::IsServerDefined
// (other items were commented in a header).
// ---------------------------------------------------------
//

TBool CCAUISessionManager::IsServerDefined()
    {
    // SAP settings store
    TInt count( 0 );
    TRAP_IGNORE(
        CIMPSSAPSettingsStore* sapSettingsStore = CIMPSSAPSettingsStore::NewLC();
        count = sapSettingsStore->SAPCountL( EIMPSIMAccessGroup );
        CleanupStack::PopAndDestroy( sapSettingsStore );
    );
    if ( count == 0 )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }
// ---------------------------------------------------------
// CCAUISessionManager::GetParameters
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::GetParameters( MCAUiLoginCmdCB::TLoginParams& aLoginParams )
    {
    aLoginParams = iLoginParams;
    }


// ---------------------------------------------------------
// CCAUISessionManager::FinalizeChatConstructionL
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCAUISessionManager::FinalizeChatConstructionL()
    {
    // Construct views
    CCAAppUi* appUi = static_cast<CCAAppUi*>(
                          CCoeEnv::Static()->AppUi() );
    appUi->FinalizeChatConstructionL();
    }



// ---------------------------------------------------------
// CCAUISessionManager::ShowPasswordSavingWarningL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::ShowPasswordSavingWarningL() const
    {
    if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SAVE_PW_WARNING ) )
        {
        HBufC* warningText = CCoeEnv::Static()->AllocReadResourceLC(
                                 R_CHAT_SAVE_PW_WARNING );

        HBufC* warningHeading = CCoeEnv::Static()->AllocReadResourceLC(
                                    R_CHAT_SAVE_PW_WARNING_HEADING );

        IMDialogUtils::DisplayMessageQueryDialogL(
            R_CHATCLIENT_MESSAGE_QUERY_DIALOG,
            *warningText,
            *warningHeading
        );

        CleanupStack::PopAndDestroy( 2, warningText );
        }
    }
// ---------------------------------------------------------
// CCAUISessionManager::DismissProcessingNoteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUISessionManager::DismissProcessingNoteL( TInt aError, TBool aValue )
    {
    CCAAppUi* appUi = static_cast<CCAAppUi*>( CCoeEnv::Static()->AppUi() );
    appUi->DismissWaitDialogL( aError, aValue );
    }



//  End of File

