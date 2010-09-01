/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Dialog for setting server settings
*
*/


// INCLUDE FILES
#include	"CWVSettingsUIDialogImp.h"
#include	"CWVSettingsUI.hrh"
#include	"CWVSettingsUIDefs.h"
#include	"CWVSettingsUITlsHolder.h"
#include	"CWVSettingsUIDefineNewServerDialog.h"
#include	"CWVSettingsUIServerListDialog.h"
#include	"CWVSettingsUIServerSelector.h"
#include	"WVSettingsSharedDataNG.h"
#include	"WVSettingsUIPanics.h"
#include	"CWVSettingsUIDialogTitlePaneHandler.h"
#include	"CWVSettingsUISettingItemCreator.h"
#include	"CWVSettingsUISettingItemEditor.h"

#include    "CIMPSSharedDataFactory.h"
#include	"MIMPSSharedData.h"
#include	"IMPSServiceSettingsUINGInternalCRKeys.h"
#include	"IMPSUIDDefs.h"
#include    "MWVSettingsCASettingsNG.h"
#include    "impscommonuidebugprint.h"
#include	"ChatNGCommonUIDs.h"
#include	<csxhelp/imng.hlp.hrh>

#include	<aknlists.h>
#include	<aknsettingitemlist.h>
#include	<aknsettingpage.h>
#include	<akntextsettingpage.h>
#include	<akntitle.h>
#include	<aknquerydialog.h>
#include	<cimpssapsettingsstore.h>
#include	<cimpssapsettings.h>
#include	<cwvsettingsUIng.rsg>
#include	<coemain.h>
#include	<bautils.h>
#include	<stringloader.h>
#include	<aknnotewrappers.h> // branding
#include    <hlplch.h>
#include    <featmgr.h>
#include    <cimpspresenceconnectionuing.h>
#include    <impspresenceconnectionuiconstsng.h>
#include	<AknCheckBoxSettingPage.h>
#include	<avkon.rsg>
#include	<CIMPSSAPSettingsList.h>
//#include	<shareddatakeysvariant.h>
#include    <data_caging_path_literals.hrh>
#include	<AknIconArray.h>
#include	<avkon.mbg>
#include	<PathInfo.h>
#include    <alwaysonlinemanagerclient.h>
#include    <cpengnwsessionslotid2.h>
#include    <centralrepository.h>
#include    <aknmessagequerydialog.h>
#include	<CAVariationNG.rsg>

#include	"CAExternalInterface.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here

#include 	"VariantKeys.h"
#include 	<centralrepository.h>


#ifdef RD_MULTIPLE_DRIVE

#include <akncommondialogsdynmem.h>
#include <caknmemoryselectiondialogmultidrive.h>
#include <driveinfo.h>
const TInt KMaxLengthOfDriveInformation = 100;

#endif // RD_MULTIPLE_DRIVE


const TInt KIMFeatHighlightLoggedInServer = 32;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWVSettingsUIDialogImp::CWVSettingsUIDialogImp()
        : iServerInUsePtr( NULL, 0 ),
        iImLoginTypeEdited ( EFalse ),
        iImLoginType ( KWVSettingsDefaultIMLoginType ),
        iConnDaysIMPtr( NULL, 0 ),
        iConnHoursIMPtr( NULL, 0 ),
        iConnStartIMPtr( NULL, 0 ),
        iConnEndIMPtr( NULL, 0 ),
        iSchedulingSupported ( EFalse ),
        iForcedLoginMade( EFalse ),
        iServListDialog( NULL )
#ifdef RD_MULTIPLE_DRIVE
        , iMemorySelectionPtr( NULL, 0 )
#endif // RD_MULTIPLE_DRIVE
        , iIsOpenedTargettDlg( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWVSettingsUIDialogImp* CWVSettingsUIDialogImp::NewL()
    {
    CWVSettingsUIDialogImp* self = new ( ELeave ) CWVSettingsUIDialogImp();
    return self;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::RunDialogLD
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TWVSettingsDialogReturnValue CWVSettingsUIDialogImp::RunDialogLD(
    CIMPSSAPSettingsStore& aSAPSettingsStore,
    MWVSettingsObserver* aObserver,
    const TDesC& aResourceFile,
    const TDesC& aResourceVariationFile,
    TInt& aExitReason,
    MWVSettingsCASettings* aCAServerSettings,
    CWVSettingsUIDialog** aSelfPtr,
    TBool aForcedLaunch, /*= EFalse */
    TWVSettingsDialogType aDialogType )
    {
    iSAPSettingsStore = &aSAPSettingsStore;
    iObserver = aObserver;
    iSelfPtr = aSelfPtr;
    iCAServerSettings = aCAServerSettings;
    iSettingItemEditor = new ( ELeave ) CWVSettingsUISettingItemEditor();
    iForcedLaunch = aForcedLaunch;

    TFileName path = aResourceFile;
    TFileName pathVariation = aResourceVariationFile;

    if ( path.Length() == 0 )
        {
        Dll::FileName( path );
        Dll::FileName( pathVariation );

        IMPSCUI_DP( D_IMPSCUI_LIT( "CWVSettingsUIDialogImp::RunDialogL - got path %S" ), &path );

        TDriveName drive( TParsePtrC( path ).Drive( ) );
        path.Zero();
        path.Append( drive );
        path.Append( KDC_RESOURCE_FILES_DIR() );
        path.Append( KResourceName );

        TDriveName driveVar( TParsePtrC( pathVariation ).Drive( ) );
        pathVariation.Zero();
        pathVariation.Append( driveVar );
        pathVariation.Append( KDC_RESOURCE_FILES_DIR() );
        pathVariation.Append( KIMCUVariationResourceName );

        IMPSCUI_DP( D_IMPSCUI_LIT( "CWVSettingsUIDialogImp::RunDialogL - Constructed path %S" ), &path );
        }

    iTlsHolder = CWVSettingsUITlsHolder::InstanceL();

    if ( !( iTlsHolder->ResourceIsLoaded() ) )
        {
        // the resource is not loaded
        RFs& fs = iCoeEnv->FsSession();

        if ( path.Length() > 0 )
            {
            // when baflutils gets an empty string, it returns "C:",
            // which breaks things
            BaflUtils::NearestLanguageFile( fs, path );
            }
        if ( pathVariation.Length() > 0 )
            {
            // when baflutils gets an empty string, it returns "C:",
            // which breaks things
            BaflUtils::NearestLanguageFile( fs, pathVariation );

            }
        iResourceFileOffset = CEikonEnv::Static()->AddResourceFileL( path );
        iResourceVariationFileOffset = CEikonEnv::Static()->AddResourceFileL( pathVariation );

        iTlsHolder->ResourceLoaded();
        }

    TInt returnValue ( 0 );
    switch ( aDialogType )
        {
        case EWVSettingsNormal:
            {
            iTitlePaneHandler = new ( ELeave ) CWVSettingsUIDialogTitlePaneHandler();
            // add the correct menubar
            this->ConstructL( R_WVSETTINGSVIEW_SERVSETTINGS_MENUBAR );
            // store the titlepane text
            iTitlePaneHandler->StoreTitlePaneTextL();
            // create an instance of connection UI
            iConnectionUI = CIMPSPresenceConnectionUi::NewL( EIMPSConnClientIM, iSAPSettingsStore );

            returnValue = this->ExecuteLD( R_WVSETTINGSVIEW_SERVSETTINGS_DLG );
            break;
            }
        case EWVSettingsDefineServer:
            {
            HBufC* text = CEikonEnv::Static()->AllocReadResourceLC(
                              R_WVSETTINGSVIEW_SERVERINUSE_NOSERV );
            CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog(
                CAknQueryDialog::ENoTone );
            CleanupStack::PushL( dlg );
            dlg->SetPromptL( *text );
            CleanupStack::Pop( dlg );
            CleanupStack::PopAndDestroy( text );
            TInt ret( dlg->ExecuteLD( R_DELETE_CONFIRMATION_QUERY ) );

            if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
                {
                // user accepted
                CWVSettingsUIDialogTitlePaneHandler* titlePaneHandler =
                    new ( ELeave ) CWVSettingsUIDialogTitlePaneHandler();
                CleanupStack::PushL( titlePaneHandler );
                // store the titlepane text
                titlePaneHandler->StoreTitlePaneTextL();
                returnValue = LaunchDefineNewServerDialogL( aSAPSettingsStore,
                                                            *aObserver,
                                                            *titlePaneHandler,
                                                            aExitReason );
                // restore the titlepane text
                titlePaneHandler->RestoreTitlePaneText();
                CleanupStack::PopAndDestroy( titlePaneHandler );
                }
            else
                {
                aExitReason = ENotSaved;
                }

            if ( iTlsHolder->OkToUnloadResource() )
                {
                CEikonEnv* eikonEnv = CEikonEnv::Static();
                eikonEnv->DeleteResourceFile( iResourceFileOffset );
                eikonEnv->DeleteResourceFile( iResourceVariationFileOffset );
                iTlsHolder->ResourceUnLoaded();
                }
            delete this;
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }

    if ( returnValue == EWVSettingsViewCmdExit )
        {
        return EWVSettingsViewExitCalled;
        }
    else
        {
        if ( returnValue == EAknSoftkeyExit && iForcedLoginMade )
            {
            // Dialog is shutdown by framework during login
            // procedure when activating refresh view
            aExitReason = EForcedExit;
            }
        return EWVSettingsViewOk;
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::LaunchDefineNewServerDialogL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUIDialogImp::LaunchDefineNewServerDialogL (
    CIMPSSAPSettingsStore& aSAPSettingsStore,
    MWVSettingsObserver& aObserver,
    CWVSettingsUIDialogTitlePaneHandler& aTitlePaneHandler,
    TInt& aExitReason ) const
    {
    TUint32 newUid( 0 );
    CWVSettingsUIDefineNewServerDialog* defineNewServerDialog = new ( ELeave )
    CWVSettingsUIDefineNewServerDialog(
        aExitReason,
        aSAPSettingsStore,
        CWVSettingsUIDefineNewServerDialog::ENewServer,
        aObserver,
        aTitlePaneHandler,
        NULL,
        newUid,
        iConnectionUI );
    CleanupStack::PushL( defineNewServerDialog );

    defineNewServerDialog->ConstructL( R_WVSETTINGSVIEW_DEFNEWSERVER_MENUBAR );
    CleanupStack::Pop( defineNewServerDialog );
    TInt returnValue = defineNewServerDialog->ExecuteLD( R_WVSETTINGSVIEW_DEFNEWSERVER_DLG );
    return returnValue;
    }

// Destructor
CWVSettingsUIDialogImp::~CWVSettingsUIDialogImp()
    {
    delete iSharedData;
    delete iSettingsChangeNotifier;

    if ( iSAPSettingsStore )
        {
        iSAPSettingsStore->RemoveObserver( this );
        }

    if ( iSettingItemArray )
        {
        iSettingItemArray->ResetAndDestroy();
        }

    delete iSettingItemArray;
    delete iServerInUse;
    delete iTitle;
    if ( iTitlePaneHandler )
        {
        // restore the titlepane text
        iTitlePaneHandler->RestoreTitlePaneText();
        delete iTitlePaneHandler;
        }

    if ( iConnectionUI )
        {
        TBool loggedIn = EFalse;
        TRAP_IGNORE( loggedIn = iConnectionUI->LoggedInL( EIMPSConnClientIM ) );

        if ( iForcedLoginMade || loggedIn )
            {
            // Keep connection open
            TRAPD( err,
                {
                CPEngNWSessionSlotID2* sessionSlotID =
                iConnectionUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
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
            }
        }

    delete iConnectionUI;
    delete iSettingItemEditor;

    delete iConnDaysIM;
    delete iConnHoursIM;
    delete iConnStartIM;
    delete iConnEndIM;

    if ( iSelectedDaysIM )
        {
        iSelectedDaysIM->ResetAndDestroy();
        delete iSelectedDaysIM;
        }


    if ( iTlsHolder )
        {
        if ( iTlsHolder->OkToUnloadResource() )
            {
            // if we leave in changeresourcefileL we have set 0 to offset
            // and we should not try to unload the resource again
            if ( iResourceFileOffset )
                {
                CEikonEnv::Static()->DeleteResourceFile( iResourceFileOffset );
                }

            if ( iResourceVariationFileOffset )
                {
                CEikonEnv::Static()->DeleteResourceFile( iResourceVariationFileOffset );
                }
            iTlsHolder->ResourceUnLoaded();
            }
        iTlsHolder->DestroySingletonsAndHolder();
        }

    delete iIconArray;
    if ( iDlgLauncher && iDlgLauncher->IsActive() )
        {
        iDlgLauncher->Cancel();
        }
    delete iDlgLauncher;

#ifdef RD_MULTIPLE_DRIVE
    delete iMemorySelection;
#endif
    // to cancel all request when dialog closed
    CCoeEnv* env = CCoeEnv::Static();
    if ( env->IsActive() )
        {
        env->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::UpdateServerInUseItemL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::UpdateServerInUseItemL()
    {
    HBufC* name = CurrentServerNameL();

    // CurrentServerNameL returns NULL if there is no default SAP
    if ( name )
        {
        CleanupStack::PushL( name );

        // check if the server name has changed
        if ( name->Compare( *iServerInUse ) )
            {
            if ( iObserver )
                {
                iObserver->ServerChangedL ( *name, ETrue );
                }
            iServerInUsePtr.Copy( name->Left( iServerInUsePtr.MaxLength() ) );
            ChangeLoginTypeToManualL();
            }
        CleanupStack::PopAndDestroy( ); // name
        }
    else
        {
        iServerInUsePtr.Copy( KEmptyDesC );
        // No default SAP, make sure that login type
        // setting is set to manual connection
        ChangeLoginTypeToManualL();
        }

    TInt index( EWVSettingsServerInUse );
    if ( !iCAServerSettings )
        {
        // if we don't show CA settings item, we have 1 item less
        // in the listbox -> thus -1 to the item to be loaded
        index--;
        }
    iSettingItemArray->At( index )->LoadL();
    iSettingItemArray->At( index )->UpdateListBoxTextL();
    DrawNow();
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::ChangeLoginTypeToManualL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::ChangeLoginTypeToManualL()
    {
    TBool refresh( EFalse );
    if ( ( iImLoginType == EWVSettingsChatLoginAutoAlways )
         || ( iImLoginType == EWVSettingsChatLoginAutoInHomeNW ) )
        {
        iImLoginTypeEdited = ETrue;
        iImLoginType = EWVSettingsChatLoginManual;
        // save this straight away, since default server is also saved straight away
        StoreSettingValueL( iImLoginType, EIMPSSharedKeysIMLogin );
        refresh = ETrue;
        }
    if ( refresh )
        {
        // Init to EFalse
        iForcedLoginMade = EFalse;

        // Show note about manual mode activation
        HBufC* text = CEikonEnv::Static()->AllocReadResourceLC(
                          R_WVSETTINGSVIEW_MANUAL_ACTIVATED );

        CAknInformationNote* note = new ( ELeave ) CAknInformationNote();
        note->ExecuteLD( *text );

        CleanupStack::PopAndDestroy( text );
        RefreshDialogL();
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::QueryNewServerL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::QueryNewServerL( TBool * aExitReason /*=NULL*/ )
    {

    HBufC* text = CEikonEnv::Static()->AllocReadResourceLC(
                      R_WVSETTINGSVIEW_SERVERINUSE_NOSERV );

    CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog(
        CAknQueryDialog::ENoTone );

    CleanupStack::PushL( dlg );
    dlg->SetPromptL( *text );
    CleanupStack::Pop( dlg );
    CleanupStack::PopAndDestroy( text );

    TInt ret( dlg->ExecuteLD( R_DELETE_CONFIRMATION_QUERY ) );

    TWVSettingsDialogReturnValue returnValue = EWVSettingsViewOk;

    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        TInt exitReason( 0 );
        TUint32 newUid( 0 );
        CWVSettingsUIDefineNewServerDialog* dlg = new ( ELeave )
        CWVSettingsUIDefineNewServerDialog(
            exitReason,
            *iSAPSettingsStore,
            CWVSettingsUIDefineNewServerDialog::ENewServer,
            *iObserver,
            *iTitlePaneHandler,
            this,
            newUid,
            iConnectionUI );

        CleanupStack::PushL( dlg );
        dlg->ConstructL( R_WVSETTINGSVIEW_DEFNEWSERVER_MENUBAR );
        CleanupStack::Pop( dlg );

        dlg->ExecuteLD( R_WVSETTINGSVIEW_DEFNEWSERVER_DLG );


        //if exitReason = EApplicationExit, then exit from IM application.
        if ( exitReason == EApplicationExit )
            {
            *aExitReason = EFalse;
            ProcessCommandL( EWVSettingsViewCmdExit );
            }


        if ( !exitReason )
            {
            returnValue = ExecuteServerListDialogL();
            }
        }

    if ( returnValue == EWVSettingsViewExitCalled )
        {
        ProcessCommandL ( EWVSettingsViewCmdExit );
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::ExecuteServerListDialogL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TWVSettingsDialogReturnValue CWVSettingsUIDialogImp::ExecuteServerListDialogL()
    {
    CWVSettingsUIServerListDialog* serverListDialog = new ( ELeave ) CWVSettingsUIServerListDialog(
        *iSAPSettingsStore,
        *iObserver,
        *iTitlePaneHandler,
        *this,
        iConnectionUI );

    CleanupStack::PushL( serverListDialog );
    serverListDialog->ConstructL( R_WVSETTINGSVIEW_SERVERLIST_MENUBAR );
    CleanupStack::Pop( serverListDialog );

    iServListDialog = serverListDialog;
    TInt returnValue = 0;
    // Trapped to make sure iServListDialog gets
    // nullified in case of leave
    TRAPD( err, returnValue = serverListDialog->ExecuteLD( R_WVSETTINGSVIEW_SERVER_LIST_DLG ) );
    iServListDialog = NULL;
    User::LeaveIfError( err );

    // this is customized return code to detect dialog shutter
    if ( returnValue == EEikBidCancel )
        {
        // Dialog shutter forced the dialogs to close and dialogs are already deleted.
        // quickly exit the dialog code.
        return EWVSettingsViewForceExit;
        }

    UpdateServerInUseItemL();
    RefreshTitleL();

    if ( returnValue == EWVSettingsViewCmdExit )
        {
        return EWVSettingsViewExitCalled;
        }
    return EWVSettingsViewOk;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::UpdateLoginTypesL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::UpdateLoginTypesL ()
    {
    // initialize as max integer, so that we know if something went wrong
    // LoadSettingValue will return 0 if it cannot load the key
    TInt loadedValue ( KMaxTInt );

    LoadSettingValueL( loadedValue, EIMPSSharedKeysIMLogin );

    SetIMLoginTypeL( loadedValue );

    iSettingItemArray->At( EWVSettingsIMLoginType )->LoadL();
    iSettingItemArray->At( EWVSettingsIMLoginType )->UpdateListBoxTextL();
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::UpdateListboxL()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::UpdateListboxL()
    {
    if ( iSettingItemArray )
        {
        TInt count ( iSettingItemArray->Count() );
        for ( TInt i = 0; i < count; i++ )
            {
            iSettingItemArray->At( i )->LoadL();
            iSettingItemArray->At( i )->UpdateListBoxTextL();
            }
        }
    DrawNow();
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::SetIMLoginTypeL()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::SetIMLoginTypeL( TInt aLoadedValue )
    {
    if ( ( aLoadedValue > EWVSettingsChatMinLoginType )
         && ( aLoadedValue < EWVSettingsChatMaxLoginType ) )
        {
        iImLoginType = aLoadedValue;
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::SetConnectionDaysL( TInt aLoadedValue )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::SetConnectionDaysL( TInt aLoadedValue,
                                                 CSelectionItemList& aSelectionItemList )
    {
    TInt index ( 0 );

    if ( iStartDayOfWeek == ESunday )
        {
        // normally in the array monday is first, but
        //
        // index of the days in the array
        index++;
        }
    if ( aLoadedValue & EWVSettingsMonday )
        {
        aSelectionItemList.At( index )->SetSelectionStatus( ETrue );
        }
    else
        {
        aSelectionItemList.At( index )->SetSelectionStatus( EFalse );
        }
    index++;

    if ( aLoadedValue & EWVSettingsTuesday )
        {
        aSelectionItemList.At( index )->SetSelectionStatus( ETrue );
        }
    else
        {
        aSelectionItemList.At( index )->SetSelectionStatus( EFalse );
        }
    index++;

    if ( aLoadedValue & EWVSettingsWednesday )
        {
        aSelectionItemList.At( index )->SetSelectionStatus( ETrue );
        }
    else
        {
        aSelectionItemList.At( index )->SetSelectionStatus( EFalse );
        }
    index++;

    if ( aLoadedValue & EWVSettingsThursday )
        {
        aSelectionItemList.At( index )->SetSelectionStatus( ETrue );
        }
    else
        {
        aSelectionItemList.At( index )->SetSelectionStatus( EFalse );
        }
    index++;

    if ( aLoadedValue & EWVSettingsFriday )
        {
        aSelectionItemList.At( index )->SetSelectionStatus( ETrue );
        }
    else
        {
        aSelectionItemList.At( index )->SetSelectionStatus( EFalse );
        }
    index++;

    if ( aLoadedValue & EWVSettingsSaturday )
        {
        aSelectionItemList.At( index )->SetSelectionStatus( ETrue );
        }
    else
        {
        aSelectionItemList.At( index )->SetSelectionStatus( EFalse );
        }
    index++;

    if ( aLoadedValue & EWVSettingsSunday )
        {
        if ( iStartDayOfWeek == ESunday )
            {

            aSelectionItemList.At( 0 )->SetSelectionStatus( ETrue );
            }
        else
            {

            aSelectionItemList.At( index )->SetSelectionStatus( ETrue );
            }
        }
    else
        {
        if ( iStartDayOfWeek == ESunday )
            {

            aSelectionItemList.At( 0 )->SetSelectionStatus( EFalse );
            }
        else
            {

            aSelectionItemList.At( index )->SetSelectionStatus( EFalse );
            }
        }

    UpdateConnectionDaysL( *iSelectedDaysIM );
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::GetConnectionDaysL( CSelectionItemList& aSelectionItemList )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUIDialogImp::GetConnectionDaysL( CSelectionItemList& aSelectionItemList )
    {
    TInt index ( 0 );

    if ( iStartDayOfWeek == ESunday )
        {
        // normally in the array monday is first, but
        //
        // index of the days in the array
        index++;
        }

    TInt returnValue ( 0 );

    if ( aSelectionItemList.At( index )->SelectionStatus() )
        {
        returnValue |= EWVSettingsMonday;
        }
    index++;

    if ( aSelectionItemList.At( index )->SelectionStatus() )
        {
        returnValue |= EWVSettingsTuesday;
        }
    index++;

    if ( aSelectionItemList.At( index )->SelectionStatus() )
        {
        returnValue |= EWVSettingsWednesday;
        }
    index++;

    if ( aSelectionItemList.At( index )->SelectionStatus() )
        {
        returnValue |= EWVSettingsThursday;
        }
    index++;

    if ( aSelectionItemList.At( index )->SelectionStatus() )
        {
        returnValue |= EWVSettingsFriday;
        }
    index++;

    if ( aSelectionItemList.At( index )->SelectionStatus() )
        {
        returnValue |= EWVSettingsSaturday;
        }
    index++;

    if ( iStartDayOfWeek == ESunday )
        {

        if ( ( &aSelectionItemList )->At( 0 )->SelectionStatus() )
            {
            returnValue |= EWVSettingsSunday;
            }
        }
    else
        {
        if ( aSelectionItemList.At( index )->SelectionStatus() )
            {

            returnValue |= EWVSettingsSunday;
            }
        }

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::UpdateConnectionDaysL( CSelectionItemList& aSelectionItemList, TInt aItem )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::UpdateConnectionDaysL( CSelectionItemList& aSelectionItemList )
    {
    if ( iSelectedDaysIM )
        {
        TBool allSelected ( ETrue );

        TInt count = ( &aSelectionItemList )->Count();
        // check if all days are selected
        for ( TInt i = 0; i < count; i++ )
            {
            if ( !( &aSelectionItemList )->At( i )->SelectionStatus() )
                {
                allSelected = EFalse;
                }
            }

        HBufC* text = NULL;

        if ( allSelected )
            {
            text = StringLoader::LoadLC( R_WVSETTINGSVIEW_SETTING_PEC_CONN_ALLDAYS );
            }
        else
            {
            text = StringLoader::LoadLC( R_WVSETTINGSVIEW_SETTING_PEC_CONN_SELDAYS );
            }

        if ( text )
            {
            iConnDaysIMPtr.Copy( text->Left( iConnDaysIMPtr.MaxLength() ) );
            CleanupStack::PopAndDestroy( text );
            }
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::UpdateConnectionHoursTextL( TInt aItem, TBool aAllDay )
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::UpdateConnectionHoursTextL( TBool aAllDay )
    {
    HBufC* text = NULL;
    if ( aAllDay )
        {
        text = StringLoader::LoadLC( R_WVSETTINGSVIEW_SETTING_PEC_CONN_ALLHOURS );
        iConnHoursIMInt = EWVSettingsAllDay;
        iConnHoursIMPtr.Copy( text->Left( iConnHoursIMPtr.MaxLength() ) );
        CleanupStack::PopAndDestroy( text );
        }
    else
        {
        // array granularity = 2 since there are 2 items
        CDesCArray* noteTexts = new( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( noteTexts );

        // Arabic & Hebrew conversion if needed.
        HBufC* startBuf = iConnStartIM->AllocLC();
        HBufC* endBuf = iConnEndIM->AllocLC();

        TPtr startPtr( startBuf->Des() );
        TPtr endPtr( endBuf->Des() );

        AknTextUtils::LanguageSpecificNumberConversion( startPtr );
        AknTextUtils::LanguageSpecificNumberConversion( endPtr );

        noteTexts->AppendL( startPtr );
        noteTexts->AppendL( endPtr );

        text = StringLoader::LoadLC( R_WVSETTINGSVIEW_SETTING_PEC_CONN_SELECTED_HOURS, *noteTexts );
        iConnHoursIMInt = EWVSettingsBetweenHours;
        iConnHoursIMPtr.Copy( text->Left( iConnHoursIMPtr.MaxLength() ) );
        CleanupStack::PopAndDestroy( text );
        CleanupStack::PopAndDestroy( 2 ); // startBuf, endBuf
        CleanupStack::PopAndDestroy( noteTexts );
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::QueryConnectionHoursL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::QueryConnectionHoursL()
    {
    TTime startTime;
    TTime endTime;
    // set the current time
    startTime.HomeTime();
    endTime.HomeTime();

    TDateTime dateTime = startTime.DateTime();

    // we can ignore the error, since if the Parse fails startTime remains in hometime
    // get the start time for IM
    // read the hour part of the time
    dateTime.SetHour( iConnStartHourIM );
    // read the minute part of the time
    dateTime.SetMinute( iConnStartMinuteIM );
    // set the start time
    startTime = dateTime;

    // get the end time for IM
    // read the hour part
    dateTime.SetHour( iConnEndHourIM );
    // read the minute part
    dateTime.SetMinute( iConnEndMinuteIM );
    // set the end time
    endTime = dateTime;

    CAknMultiLineDataQueryDialog* dlg = CAknMultiLineDataQueryDialog::NewL( startTime, endTime );
    TInt ret( dlg->ExecuteLD( R_WVSETTINGSVIEW_CONN_HOURS_QUERY ) );

    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        // get and format the time/date string
        HBufC* tFormat = CCoeEnv::Static()->AllocReadResourceLC(
                             R_QTN_TIME_USUAL_WITH_ZERO );

        // user entered text
        startTime.FormatL( iConnStartIMPtr, *tFormat );
        endTime.FormatL( iConnEndIMPtr, *tFormat );
        dateTime = startTime.DateTime();
        iConnStartHourIM = dateTime.Hour();
        iConnStartMinuteIM = dateTime.Minute();
        dateTime = endTime.DateTime();
        iConnEndHourIM = dateTime.Hour();
        iConnEndMinuteIM = dateTime.Minute();
        CleanupStack::PopAndDestroy( tFormat );
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::RefreshTitleL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::RefreshTitleL()
    {
    HBufC* tempTitle = iEikonEnv->AllocReadResourceL( RSC_WVSETTINGSVIEW_SETT_NEW_SERVER );
    delete iTitle;
    iTitle = NULL;
    iTitle = tempTitle;

    if ( iServListDialog )
        {
        // Server list dialog is active, refresh title in there
        iServListDialog->RefreshTitleL();
        }
    else
        {
        iTitlePaneHandler->SetTitlePaneTextL( *iTitle );
        }
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::ProcessCommandL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::ProcessCommandL( TInt aCommand )
    {
    HideMenu();

    switch ( aCommand )
        {


        case EWVSettingsViewCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( CEikonEnv::Static()->WsSession(),
                                                 ( CCoeEnv::Static()->AppUi() )->AppHelpContextL () );

            break;
            }

        case EWVSettingsViewCmdExit:
            {
            TryExitL( EWVSettingsViewCmdExit );
            break;
            }

        // Flow-through
        case EAknSoftkeyOpen:   // MSK command
        case EAknSoftkeyEdit:   // Flowthrough, MSK command
        case EWVSettingsViewServSetChange:
        case EWVSettingsViewServSetOpen:
        case EWVSettingsViewSettingsEdit:
            {
            LaunchCorrectDialog();
            break;
            }

        default:
            {
            CEikonEnv::Static()->EikAppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::DynInitMenuPaneL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {

    // if we're displaying general menu and help feature is not supported..
    if ( aResourceId == R_WVSETTINGSVIEW_GENERAL_MENU &&
         !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        // dimm the help menu item
        aMenuPane->SetItemDimmed( EWVSettingsViewCmdHelp, ETrue );
        }

    if ( aResourceId == R_WVSETTINGSVIEW_SERVSETTINGS_MENUPANE )
        {
        TInt index = iListbox->CurrentItemIndex();
        if ( !iCAServerSettings )
            {
            // if we don't show CA settings item, we have 1 item less
            // in the listbox -> thus +1 to the index
            index++;
            }
        switch ( index )
            {
            case EWVSettingsCASettings:
                {
                aMenuPane->DeleteMenuItem( EWVSettingsViewServSetChange );
                aMenuPane->DeleteMenuItem( EWVSettingsViewSettingsEdit );
                break;
                }
            case EWVSettingsServers:
                {
                aMenuPane->DeleteMenuItem( EWVSettingsViewServSetChange );
                aMenuPane->DeleteMenuItem( EWVSettingsViewSettingsEdit );
                break;
                }
            case EWVSettingsServerInUse:
                {
                aMenuPane->DeleteMenuItem( EWVSettingsViewServSetOpen );
                aMenuPane->DeleteMenuItem( EWVSettingsViewSettingsEdit );
                break;
                }
            case EWVSettingsIMLoginType:
            case EWVSettingsIMConnDays:
            case EWVSettingsIMConnHours:
                {
                aMenuPane->DeleteMenuItem( EWVSettingsViewServSetOpen );
                aMenuPane->DeleteMenuItem( EWVSettingsViewServSetChange );
                break;
                }
#ifdef RD_MULTIPLE_DRIVE
            case EWVSettingsSaveChatsTo:
                {
                aMenuPane->DeleteMenuItem( EWVSettingsViewServSetOpen );
                aMenuPane->DeleteMenuItem( EWVSettingsViewServSetChange );
                break;
                }
#endif
            default:
                {
                // Nothing to do if neither indexes were correct
                User::Leave ( EIncorrectListBoxItemIndex );
                break;
                }
            }
        }
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::PreLayoutDynInitL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::PreLayoutDynInitL()
    {
    RefreshTitleL();

    if ( iSAPSettingsStore )
        {
        iSAPSettingsStore->AddObserverL( this, EIMPSIMAccessGroup );
        }

    iSharedData = CIMPSSharedDataFactory::CreatePermanentKeyHandlerL( this, KIMPSServSettUid );

    iSettingsChangeNotifier =
        CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL( this, KIMPSServSettNotifyUid );

    // check if scheduling is supported
    iSchedulingSupported = CWVSettingsUISettingItemCreator::SchedulingSupported();
    if ( iSchedulingSupported )
        {
        TLocale* locale = new ( ELeave ) TLocale;
        iStartDayOfWeek = locale->StartOfWeek();
        delete locale;
        // create an array of selected days for scheduling

        // create an array of selected days. Granularity 7 since there are 7 days in a week
        iSelectedDaysIM = new ( ELeave ) CSelectionItemList( 7 );
        CreateSelectedDaysArrayL( *iSelectedDaysIM );

        iConnDaysIM = HBufC::NewL( KWVSettingsConnDaysTextMaxLength );
        iConnDaysIMPtr.Set( iConnDaysIM->Des() );

        iConnHoursIM = HBufC::NewL( KWVSettingsConnHoursTextMaxLength );
        iConnHoursIMPtr.Set( iConnHoursIM->Des() );
        iConnHoursIMInt = EWVSettingsBetweenHours;
        }

#ifdef RD_MULTIPLE_DRIVE

    //qtn_chat_setting_chatlogin and wvsettingsview_setting_memory_selection_type
    iMemorySelection = HBufC::NewL( KMaxLengthOfDriveInformation );

    iMemorySelectionPtr.Set( iMemorySelection->Des() );

#endif // RD_MULTIPLE_DRIVE

    LoadSettingsL(); //load settings from shared data and presence engine

    iDlgLauncher = new ( ELeave ) CAsyncCallBack( TCallBack( LaunchDlgCallback, this ),
                                                  CActive::EPriorityStandard );

    iServerInUse = HBufC::NewL( KWVSettingsServerNameMaxLength );
    iServerInUsePtr.Set( iServerInUse->Des() );

    HBufC* name = CurrentServerNameL();

    if ( name )
        {
        iServerInUsePtr.Copy( ( *name ).Left( iServerInUsePtr.MaxLength() ) );
        delete name;
        }
    else
        {
        iServerInUsePtr.Copy( KEmptyDesC );
        }

    User::LeaveIfError(
        iSharedData->SubscribeChange( KIMPSServSettUid, EIMPSSharedKeysIMLogin ) );
    if ( iSchedulingSupported )
        {
        User::LeaveIfError(
            iSharedData->SubscribeChange( KIMPSServSettUid, EIMPSSharedKeysIMConnDays ) );
        User::LeaveIfError(
            iSharedData->SubscribeChange( KIMPSServSettUid, EIMPSSharedKeysIMConnStart ) );
        User::LeaveIfError(
            iSharedData->SubscribeChange( KIMPSServSettUid, EIMPSSharedKeysIMConnEnd ) );
        }

    iListbox =
        static_cast< CAknSettingStyleListBox* >( Control( EWVSettingsViewControlIdServSettings ) );

    TBool caSettings( EFalse );
    if ( iCAServerSettings )
        {
        caSettings = ETrue;
        }
    iSettingItemArray =
        CWVSettingsUISettingItemCreator::CreateSettingItemListL(
            caSettings,
            iServerInUsePtr,
            iImLoginType,
#ifdef RD_MULTIPLE_DRIVE
            iMemorySelectionPtr,
#endif
            iConnDaysIMPtr,
            iConnHoursIMPtr );

    CTextListBoxModel* model = iListbox->Model();
    model->SetItemTextArray( iSettingItemArray );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray ); // Ownership retained by us

    iSettingItemArray->RecalculateVisibleIndicesL();
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto );
    iListbox->HandleItemAdditionL();
    iListbox->UpdateScrollBarsL();
    iListbox->SetListBoxObserver( this );

    if ( iForcedLaunch )
        {
        // Set index to login type item
        TInt index = EWVSettingsIMLoginType;
        if ( !iCAServerSettings )
            {
            // if CA setting item is not present,
            // index needs to be decremented by one
            index--;
            }
        iListbox->SetCurrentItemIndex( index );
        }
    UpdateCbaL();
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::CreateSelectedDaysArrayL( CSelectionItemList& aSelectionItemList )
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::CreateSelectedDaysArrayL( CSelectionItemList& aSelectionItemList )
    {
    // create a selectable item for each week day and append it into the array
    CSelectableItem* selectableItem = NULL;

    if ( iStartDayOfWeek == ESunday )
        {
        selectableItem = CreateSelectableItemLC( R_QTN_WEEK_LONG_SUNDAY );
        ( &aSelectionItemList )->AppendL( selectableItem );
        CleanupStack::Pop( selectableItem );
        }

    selectableItem = CreateSelectableItemLC( R_QTN_WEEK_LONG_MONDAY );
    ( &aSelectionItemList )->AppendL( selectableItem );
    CleanupStack::Pop( selectableItem );

    selectableItem = CreateSelectableItemLC( R_QTN_WEEK_LONG_TUESDAY );
    ( &aSelectionItemList )->AppendL( selectableItem );
    CleanupStack::Pop( selectableItem );

    selectableItem = CreateSelectableItemLC( R_QTN_WEEK_LONG_WEDNESDAY );
    ( &aSelectionItemList )->AppendL( selectableItem );
    CleanupStack::Pop( selectableItem );

    selectableItem = CreateSelectableItemLC( R_QTN_WEEK_LONG_THURSDAY );
    ( &aSelectionItemList )->AppendL( selectableItem );
    CleanupStack::Pop( selectableItem );

    selectableItem = CreateSelectableItemLC( R_QTN_WEEK_LONG_FRIDAY );
    ( &aSelectionItemList )->AppendL( selectableItem );
    CleanupStack::Pop( selectableItem );

    selectableItem = CreateSelectableItemLC( R_QTN_WEEK_LONG_SATURDAY );
    ( &aSelectionItemList )->AppendL( selectableItem );
    CleanupStack::Pop( selectableItem );

    if ( iStartDayOfWeek != ESunday )
        {
        selectableItem = CreateSelectableItemLC( R_QTN_WEEK_LONG_SUNDAY );
        ( &aSelectionItemList )->AppendL( selectableItem );
        CleanupStack::Pop( selectableItem );
        }

    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::CreateSelectableItemLC()
//
// (other items were commented in a header).
// ---------------------------------------------------------
CSelectableItem* CWVSettingsUIDialogImp::CreateSelectableItemLC( TInt aResourceId )
    {
    // get the text for the item
    HBufC* text = CEikonEnv::Static()->AllocReadResourceLC( aResourceId );

    // create a selectable item, which is selected by default
    CSelectableItem* selectableItem = new( ELeave ) CSelectableItem( *text, ETrue );
    CleanupStack::PushL( selectableItem );
    selectableItem->ConstructL();
    // pop the selectable item, so that we can destory text
    CleanupStack::Pop( selectableItem );
    CleanupStack::PopAndDestroy( text );

    // push selectableItem back into the cleanup stack
    CleanupStack::PushL( selectableItem );

    return selectableItem;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::CurrentServerNameL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CWVSettingsUIDialogImp::CurrentServerNameL()
    {
    HBufC* serverName = NULL;

    CIMPSSAPSettings* sapSettings = CIMPSSAPSettings::NewLC();
    TRAPD( err, iSAPSettingsStore->GetDefaultL( sapSettings, EIMPSIMAccessGroup ) );

    if ( err == KErrNone )
        {
        // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
        // There is no possibility to leave in this method after this line
        serverName = sapSettings->SAPName().AllocL(); // CSI: 35 # See above
        }
    else if ( err == KErrNotFound )
        {
        // we don't want to leave if there is no default sap
        }
    else
        {
        User::Leave( err );
        }

    CleanupStack::PopAndDestroy( sapSettings );

    return serverName;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::RefreshDialogL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::RefreshDialogL()
    {
    // refresh title
    RefreshTitleL();

    TBool caSettings( EFalse );
    if ( iCAServerSettings )
        {
        caSettings = ETrue;
        }


    TBuf<KMaxLengthOfDriveInformation> drive( KWVSettingsDefaultMemoryDrive );

    if ( iMemorySelectionPtr.Find( drive.Left( 2 ) ) != KErrNotFound )
        {
        HBufC* phoneMemoryInfo = StringLoader::LoadLC( R_QTN_CHAT_SETTINGS_PHONE_MEMORY,
                                                       iMemorySelectionPtr.Left( 2 ) );
        iMemorySelectionPtr.Copy( phoneMemoryInfo->Des() );
        CleanupStack::PopAndDestroy( phoneMemoryInfo );
        }

    // create a new setting item list
    CAknSettingItemArray* tempSettingItemList =
        CWVSettingsUISettingItemCreator::CreateSettingItemListL(
            caSettings,
            iServerInUsePtr,
            iImLoginType,
#ifdef RD_MULTIPLE_DRIVE
            iMemorySelectionPtr,
#endif
            iConnDaysIMPtr,
            iConnHoursIMPtr );

    // delete the setting item array
    if ( iSettingItemArray )
        {
        iSettingItemArray->ResetAndDestroy();
        }
    delete iSettingItemArray;
    iSettingItemArray = tempSettingItemList;

    CTextListBoxModel* model = iListbox->Model();

    model->SetItemTextArray( iSettingItemArray );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray ); // Ownership retained by us

    iSettingItemArray->RecalculateVisibleIndicesL();
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto );
    iListbox->HandleItemAdditionL();
    iListbox->UpdateScrollBarsL();
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::OfferKeyEventL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CWVSettingsUIDialogImp::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {

    if ( aType == EEventKey &&
         ( aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK ) )
        {
        LaunchCorrectDialog();
        return EKeyWasConsumed;
        }

    else if ( aKeyEvent.iCode == EKeyEscape )
        {
        TryExitL( EAknSoftkeyExit );
        return EKeyWasConsumed;
        }


    else if ( aKeyEvent.iCode == EKeyPhoneSend )
        {
        return EKeyWasConsumed;
        }

    TKeyResponse res = iListbox->OfferKeyEventL( aKeyEvent, aType );
    if ( res == EKeyWasConsumed )
        {
        UpdateCbaL();
        }

    return res;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::CopySelectedDaysArrayL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CSelectionItemList* CWVSettingsUIDialogImp::CopySelectedDaysArrayL(
    CSelectionItemList* aSelectedDaysArray )
    {
    CSelectionItemList* newSelectedDaysArray = new ( ELeave ) CSelectionItemList( 7 );
    CleanupStack::PushL( newSelectedDaysArray );
    CreateSelectedDaysArrayL( *newSelectedDaysArray );
    CleanupStack::Pop( newSelectedDaysArray );

    TInt count = aSelectedDaysArray->Count();
    for ( TInt index( 0 ); index < count; index++ )
        {
        newSelectedDaysArray->At( index )->SetSelectionStatus(
            aSelectedDaysArray->At( index )->SelectionStatus() );
        }

    return newSelectedDaysArray;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::LaunchCorrectDialogL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::LaunchCorrectDialogL()
    {

    // check if we have servers
    if ( !CheckAnyStoredServers() )
        return;


    TInt index = iListbox->CurrentItemIndex();
    if ( !iCAServerSettings )
        {
        // if we don't show CA settings item, we have 1 item less
        // in the listbox -> thus +1 to the index
        index++;
        }
    switch ( index )
        {
        case EWVSettingsCASettings:
            {
            LaunchCASettingsL();
            break;
            }
        case EWVSettingsServers: // edit server
            {
            TWVSettingsDialogReturnValue returnValue = ExecuteServerListDialogL();
            if ( returnValue == EWVSettingsViewExitCalled )
                {
                TryExitL( EWVSettingsViewCmdExit );
                }
            else if ( returnValue != EWVSettingsViewForceExit )
                {
                DrawNow();
                }
			else 
				{
				TryExitL( EAknSoftkeyCancel );
				}
            break;
            }
        case EWVSettingsServerInUse: // select server
            {

            TBool loggedIn( iConnectionUI->LoggedInL( EIMPSConnClientIM ) );

            HBufC* buffer = NULL;
            TIMPSConnectionOperation connOperation ( iConnectionUI->CurrentConnectionOperationL( buffer ) );
            CleanupStack::PushL( buffer );

            if ( loggedIn )
                {
                // connection active, we cannot edit this setting
                // ask confirmation about logging out
                CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );
                CleanupStack::PushL( dlg );
                // Set prompt
                HBufC* text = CEikonEnv::Static()->AllocReadResourceLC(
                                  R_WVSETTINGSVIEW_ERROR_ID_ACTIVE_CONNECTION );
                dlg->SetPromptL( *text );
                CleanupStack::PopAndDestroy( text );
                CleanupStack::Pop( dlg );
                TInt ret( dlg->ExecuteLD( R_ACTIVE_CONNECTION_CONFIRMATION_QUERY ) );

                if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
                    {
                    // user confirmed logout -> logout all open connections
                    CPEngNWSessionSlotID2* sessionSlotID =
                        iConnectionUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM );

                    CleanupStack::PushL( sessionSlotID );
                    User::LeaveIfError( iConnectionUI->LogoutL( *sessionSlotID ) );
                    CleanupStack::PopAndDestroy( sessionSlotID );
                    iForcedLoginMade = EFalse;

                    CWVSettingsUIServerSelector::ExecuteSelectionQueryLD( *iSAPSettingsStore, *this );
                    UpdateServerInUseItemL();
                    }
                }
            else if ( connOperation == EIMPSConnOppClientLogin )
                {
                // a login operation is ongoing, we cannot let the user edit the settings
                HBufC* prompt = StringLoader::LoadLC( R_QTN_CHAT_LOGIN_ONGOING_NOTE, *buffer, iCoeEnv );
                CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote( ETrue );
                dlg->ExecuteLD( *prompt );
                CleanupStack::PopAndDestroy( prompt );
                }
            else
                {
                // there is no active connection, we can edit the setting
                CWVSettingsUIServerSelector::ExecuteSelectionQueryLD( *iSAPSettingsStore, *this );
                UpdateServerInUseItemL();
                }
            CleanupStack::PopAndDestroy( buffer );
            break;
            }
        case EWVSettingsIMLoginType: // IM settings
            {
            // handle editing
            iSettingItemArray->At( index )->EditItemL( ETrue );
            iListbox->DrawItem( index );
            DrawNow();
            break;
            }
#ifdef RD_MULTIPLE_DRIVE
        case EWVSettingsSaveChatsTo: // select server
            {
            // launch memory selection drive dialog box...

            // prompt the user to save into a different drive.......

            CAknMemorySelectionDialogMultiDrive* dlg = CAknMemorySelectionDialogMultiDrive::NewL(
                                                           ECFDDialogTypeSave,
                                                           0 ,//default resource id
                                                           EFalse,
                                                           AknCommonDialogsDynMem::EMemoryTypePhone |
                                                           AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage |
                                                           AknCommonDialogsDynMem::EMemoryTypeMMCExternal );



            CCoeEnv* coeEnv = CCoeEnv::Static();

            // Read texts from resources
            HBufC* lskTextOk = coeEnv->AllocReadResourceLC( R_QTN_CHAT_MULTIDRIVE_OK );

            dlg->SetLeftSoftkeyL( *lskTextOk );

            CleanupStack::PopAndDestroy( lskTextOk );



            TInt driveNo = 0;
            TInt err = RFs::CharToDrive( iMemorySelectionPtr[0], driveNo );//converting drive letter to drive No.
            TDriveNumber driveNumber = TDriveNumber( driveNo );

            CleanupStack::PushL( dlg );

            TBool ret( dlg->ExecuteL( driveNumber, NULL, NULL ) );

            if ( ret )
                {
                TInt memoryIndex = dlg->FindIndexByDrive( driveNumber );//getting memory index corresponding to drive No. from dialog.

                if ( memoryIndex >= 0 )//if drive not found, we get index -1 and GetItem throws exception.
                    {
                    dlg->GetItem( memoryIndex, iMemorySelectionPtr );//collecting drive name in iMemorySelectionPtr.
                    }

                RefreshDialogL();


                // update selecte memory drive here...
#ifdef RD_MULTIPLE_DRIVE
                User::LeaveIfError( iSharedData->SetStringKey( EIMPSSharedKeysMemoryDrive, iMemorySelectionPtr ) );
#endif
                }

            CleanupStack::PopAndDestroy( dlg );

            break;
            }
#endif
        default:
            {
            // Nothing to do if neither indexes were correct
            User::Leave( EIncorrectListBoxItemIndex );
            break;
            }
        }
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::HandlePermanentKeyNotifyL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void  CWVSettingsUIDialogImp::HandlePermanentKeyNotifyL( const TUid aUid,
                                                         const TIMPSSharedKeys aKey )
    {
    if ( ( aUid == KIMPSServSettUid ) &&
         ( aKey == EIMPSSharedKeysIMLogin ) )
        {
        UpdateLoginTypesL();
        }

    else if ( aUid == KIMPSServSettUid &&
              ( aKey == EIMPSSharedKeysIMConnStart ) )
        {
        LoadConnectionHoursSettingValuesL();
        }
#ifdef RD_MULTIPLE_DRIVE
    else if ( aUid == KIMPSServSettUid &&
              ( aKey == EIMPSSharedKeysMemoryDrive ) )
        {
        LoadMemoryDriveSettingValueL();
        }
#endif
    else if ( aUid == KIMPSServSettUid &&
              ( aKey == EIMPSSharedKeysIMConnDays ) )
        {
        TInt loadedValue( KWVSettingsConnDaysAllSelected );
        LoadSettingValueL( loadedValue, EIMPSSharedKeysIMConnDays );
        SetConnectionDaysL( loadedValue, *iSelectedDaysIM );
        UpdateConnectionDaysL( *iSelectedDaysIM );
        }
    UpdateListboxL();

    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::HandleTemporaryKeyNotifyL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::HandleTemporaryKeyNotifyL( const TUid /* aUid */,
                                                        const TIMPSSharedKeys /* aKey */ )
    {

    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::LoadSettingsL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::LoadSettingsL()
    {
    // load the pec and chat login values
    // we don't need the return value here, since

    // set this to max integer so we know that we get some data
    TInt loadedValue ( KMaxTInt );

    LoadSettingValueL( loadedValue, EIMPSSharedKeysIMLogin );
    SetIMLoginTypeL( loadedValue );

    if ( iSchedulingSupported )
        {
        // load the connection days setting value
        loadedValue = KWVSettingsConnDaysAllSelected;
        LoadSettingValueL( loadedValue, EIMPSSharedKeysIMConnDays );
        SetConnectionDaysL( loadedValue, *iSelectedDaysIM );

        LoadConnectionHoursSettingValuesL();
        }
#ifdef RD_MULTIPLE_DRIVE
    LoadMemoryDriveSettingValueL();
#endif
    }

#ifdef RD_MULTIPLE_DRIVE

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::LoadMemoryDriveSettingValueL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::LoadMemoryDriveSettingValueL()
    {
    TInt err( iSharedData->GetStringKey( EIMPSSharedKeysMemoryDrive, iMemorySelectionPtr ) );

	TInt driveNo = 0;
	RFs::CharToDrive(iMemorySelectionPtr[0], driveNo );//converting drive letter to drive No.
	TDriveNumber driveNumber = TDriveNumber(driveNo); 
    RFs iFs;
    User::LeaveIfError( iFs.Connect() );
	if ( (err == KErrNotFound ) || ( err == KErrTooBig ) || (!iFs.IsValidDrive(driveNumber)))
        {
        iMemorySelectionPtr.Copy( KWVSettingsDefaultMemoryDrive );
        }
    else
        {
        User::LeaveIfError( err );
	    TUint driveStatus( 0 );
	    TBool massStorage = EFalse;
	    User::LeaveIfError( DriveInfo::GetDriveStatus( iFs, driveNumber, driveStatus ) );
        if ( ( driveStatus & DriveInfo::EDriveInternal ) && 
             ( driveStatus & DriveInfo::EDriveExternallyMountable ) )
            	{
            	massStorage = ETrue;
        }
	    if ( !massStorage )
	    	{
		    TVolumeInfo volInfo;    
		    TInt ret( iFs.Volume( volInfo, driveNumber ) );
		    if ( ret == KErrNone )
		    	{
			    int length = iMemorySelectionPtr.Length() - 3; // The lengthe of drive name and space is 3. 
			    if (  iMemorySelectionPtr.Mid(3, length).Compare(volInfo.iName) ) // Check that if the labels area the same. 
			        {
			        iMemorySelectionPtr.Replace(3, length, volInfo.iName);
			        User::LeaveIfError( iSharedData->SetStringKey( EIMPSSharedKeysMemoryDrive, iMemorySelectionPtr ) );
			        }	
		    	}	    
		    }
	    }
	iFs.Close();
    TBuf<KMaxLengthOfDriveInformation> drive( KWVSettingsDefaultMemoryDrive );

    if ( iMemorySelectionPtr.Find( drive.Left( 2 ) ) != KErrNotFound )
        {
        HBufC* phoneMemoryInfo = StringLoader::LoadLC( R_QTN_CHAT_SETTINGS_PHONE_MEMORY,
                                                       iMemorySelectionPtr.Left( 2 ) );
        iMemorySelectionPtr.Copy( phoneMemoryInfo->Des() );
        CleanupStack::PopAndDestroy( phoneMemoryInfo );
        }

    }
#endif

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::LoadSettingValueL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::LoadSettingValueL( TInt& aSettingValueToLoad,
                                                const TIMPSSharedKeys aNameKey )
    {
    TInt readValue ( 0 );

    TInt err( iSharedData->GetIntKey( aNameKey, readValue ) );
    // if one of these errors occur, the value passed is not changed
    // so we can just return
    if ( ( err == KErrNotFound ) ||
         ( err == KErrGeneral ) ||
         ( err == KErrOverflow ) )
        {
        //load failed, let value be as it was
        return;
        }

    //Handle other shared data reading erros by leaving
    User::LeaveIfError( err );

    aSettingValueToLoad = readValue;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::LoadConnectionHoursSettingValuesL)
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::LoadConnectionHoursSettingValuesL()
    {
    // get and format the time/date string
    HBufC* tFormat = CCoeEnv::Static()->AllocReadResourceLC(
                         R_QTN_TIME_USUAL_WITH_ZERO );
    // IM scheduling settings
    HBufC* tempBuf = LoadStringSettingValueL ( EIMPSSharedKeysIMConnStart, EWVSettingsConnStart );
    delete iConnStartIM;
    iConnStartIM = tempBuf;
    iConnStartIMPtr.Set( iConnStartIM->Des() );

    tempBuf = LoadStringSettingValueL ( EIMPSSharedKeysIMConnEnd, EWVSettingsConnEnd );
    delete iConnEndIM;
    iConnEndIM = tempBuf;
    iConnEndIMPtr.Set( iConnEndIM->Des() );

    // get the start time for IM
    // read the hour part of the time
    iConnStartHourIM = Hour( iConnStartIMPtr, KDefaultStartHour );
    TDateTime dateTime;
    dateTime.SetHour( iConnStartHourIM );
    // read the minute part of the time
    iConnStartMinuteIM = Minute( iConnStartIMPtr, KDefaultMinute );
    dateTime.SetMinute( iConnStartMinuteIM );
    // set the start time
    TTime startTime = dateTime;

    // get the end time for IM
    // read the hour part
    iConnEndHourIM = Hour( iConnEndIMPtr, KDefaultEndHour );
    dateTime.SetHour( iConnEndHourIM );
    // read the minute part
    iConnEndMinuteIM = Minute( iConnEndIMPtr, KDefaultMinute );
    dateTime.SetMinute( iConnEndMinuteIM );
    // set the end time
    TTime endTime = dateTime;

    // user entered text
    startTime.FormatL( iConnStartIMPtr, *tFormat );
    endTime.FormatL( iConnEndIMPtr, *tFormat );

    CleanupStack::PopAndDestroy( tFormat );

    // update IM connection hours text in the listbox
    if ( iConnStartIM && iConnEndIM )
        {
        if ( ( iConnHoursIMInt == EWVSettingsAllDay ) ||
             ( !iConnStartIM->Compare( iConnEndIM->Des() ) ) )
            {
            UpdateConnectionHoursTextL( ETrue );
            }
        else
            {
            UpdateConnectionHoursTextL( EFalse );
            }
        }
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::Hour()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CWVSettingsUIDialogImp::Hour( const TDesC& aTime, TInt aDefault )
    {
    TPtrC hourPtr = aTime.Left( KTimeValueLength );

    TLex lex( hourPtr );
    TInt hourInt ( aDefault );
    // we can ignore the error, if it fails we set the default
    lex.Val( hourInt );
    return hourInt;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::Minute()
//
// (other items were commented in a header).
// ---------------------------------------------------------
TInt CWVSettingsUIDialogImp::Minute( const TDesC& aTime, TInt aDefault )
    {
    TPtrC minPtr = aTime.Right( KTimeValueLength );
    TLex lex( minPtr );
    TInt minuteInt ( aDefault );
    // we can ignore the error, if it fails we set the default
    lex.Val( minuteInt );
    return minuteInt;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::LoadStringSettingValueL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CWVSettingsUIDialogImp::LoadStringSettingValueL(
    const TIMPSSharedKeys aNameKey,
    TConnHoursSetting aSetting )
    {
    HBufC* buffer = HBufC::NewLC( KWVSettingsConnHoursTextMaxLength );

    TPtr bufferPtr ( buffer->Des() );

    TInt err( iSharedData->GetStringKey( aNameKey, bufferPtr ) );

    // if one of these errors occur, the value passed is not changed
    // so we can just return
    if ( ( err == KErrNotFound ) ||
         ( err == KErrTooBig ) )
        {
        // put the default time in the buffer
        if ( aSetting == EWVSettingsConnStart )
            {
            bufferPtr.Copy( KWVSettingsDefaultConnStart );
            }
        else
            {
            bufferPtr.Copy( KWVSettingsDefaultConnEnd );
            }
        //load failed, let value be as it was
        CleanupStack::Pop( buffer );
        return buffer;
        }

    //Handle other shared data reading erros by leaving
    User::LeaveIfError( err );

    CleanupStack::Pop( buffer );
    return buffer;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::OkToExitL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CWVSettingsUIDialogImp::OkToExitL( TInt aKeycode )
    {
    if ( iIsOpenedTargettDlg )
    	{
    	return EFalse;
    	}
    if ( aKeycode == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        return EFalse;
        }

    //Direct exit request causes silent exit.
    //In this case edited settings are discarded.
    else if ( aKeycode == EAknSoftkeyExit )
        {
        return ETrue;
        }
    else if ( aKeycode == EAknSoftkeyOpen || aKeycode == EAknSoftkeyEdit )
        {
        // Don't allow exit with MSK commands
        ProcessCommandL( aKeycode );
        return EFalse;
        }

    SaveSettingsL();
    return ETrue;
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::SaveSettingsL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::SaveSettingsL()
    {
    //get data from editors
    TInt count( iSettingItemArray->Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        iSettingItemArray->At( i )->StoreL();
        }

    if ( iSchedulingSupported )
        {
        // IM Login type settings
        StoreSettingValueL( GetConnectionDaysL( *iSelectedDaysIM ), EIMPSSharedKeysIMConnDays );

        // format the times correctly for saving (HH:MM)
        iConnStartIMPtr.Format( KTimeFormat, iConnStartHourIM, iConnStartMinuteIM );

        iConnEndIMPtr.Format( KTimeFormat,  iConnEndHourIM, iConnEndMinuteIM );

        StoreStringSettingValueL( *iConnStartIM, EIMPSSharedKeysIMConnStart );
        StoreStringSettingValueL( *iConnEndIM, EIMPSSharedKeysIMConnEnd );

        // set the notifier keys for schedules
        iSettingsChangeNotifier->SetIntKey( EIMPSSharedKeysServSettScheduleChangedPEC, 0 );
        iSettingsChangeNotifier->SetIntKey( EIMPSSharedKeysServSettScheduleChangedIM, 0 );
        }

    if ( iImLoginTypeEdited )
        {
        // save the IM login type
        StoreSettingValueL( iImLoginType, EIMPSSharedKeysIMLogin );

        // Start AO plugin.
        RAlwaysOnlineClientSession session;

        User::LeaveIfError( session.Connect() );

        TUid uid = { KIMPSALWAYSONLINEUID3 };
        TPckgBuf< TUid > param = uid;

        if ( iImLoginType == EWVSettingsChatLoginAutoAlways ||
             iImLoginType == EWVSettingsChatLoginAutoInHomeNW )
            {
            session.RelayCommandL( EServerAPIBaseCommandStart, param );
            }
        // Handle situation where login is ongoing and we are
        // stopping plugin.
        /*else
            {
            session.RelayCommandL( EServerAPIBaseCommandStop, param );
            }*/
        session.Close();

        // set the notifier key for login type
        iSettingsChangeNotifier->SetIntKey( EIMPSSharedKeysServSettLoginTypeChangedIM, 0 );
        }

    // update selecte memory drive here...
#ifdef RD_MULTIPLE_DRIVE
    User::LeaveIfError( iSharedData->SetStringKey( EIMPSSharedKeysMemoryDrive, iMemorySelectionPtr ) );
#endif

    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::StoreSettingValueL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::StoreSettingValueL( TInt aSettingValueToStore,
                                                 const TIMPSSharedKeys aNameKey )
    {
    __ASSERT_DEBUG( ( aSettingValueToStore != KMaxTInt ), Panic( EWVSettingsViewPanicStoreValue ) );

    iSharedData->SetIntKey( aNameKey, aSettingValueToStore );
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::StoreStringSettingValueL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::StoreStringSettingValueL( const TDesC& aSettingValueToStore,
                                                       const TIMPSSharedKeys aNameKey )
    {
    User::LeaveIfError( iSharedData->SetStringKey( aNameKey, aSettingValueToStore ) );
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::HandleListBoxEventL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                  TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed: // Fall through
        case EEventEditingStarted:
        case EEventItemDoubleClicked:
            {
            LaunchCorrectDialog();
            break;
            }
        default:
            {
            break;
            }
        }
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::ChangeResourceFileL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::ChangeResourceFileL ( const TDesC& aResourceFileName,
                                                   const TDesC& aResourceVariationFile )
    {

    TFileName path = aResourceFileName;
    TFileName pathVariation = aResourceVariationFile;

    TFileName connUiResFile = KNullDesC();
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    if ( path.Length() == 0 )
        {
        Dll::FileName( path );
        Dll::FileName( pathVariation );
        TDriveName drive( TParsePtrC( path ).Drive( ) );
        path.Zero();
        path.Append( drive );
        path.Append( KDC_RESOURCE_FILES_DIR() );
        path.Append( KResourceName );
        BaflUtils::NearestLanguageFile( eikonEnv->FsSession(), path );

        TDriveName driveVar( TParsePtrC( pathVariation ).Drive( ) );
        pathVariation.Zero();
        pathVariation.Append( driveVar );
        pathVariation.Append( KDC_RESOURCE_FILES_DIR() );
        pathVariation.Append( KIMCUVariationResourceName );
        BaflUtils::NearestLanguageFile( eikonEnv->FsSession(), pathVariation );
        }
    else
        {
        TChar backSlash( KBackSlash );
        // Locate first backslash starting from the end of
        // the path
        TInt pos = path.LocateReverse( backSlash );
        if ( pos != KErrNotFound )
            {
            connUiResFile.Append( path.Left( pos ) );
            connUiResFile.Append( KDirSeparator );
            connUiResFile.Append( KConnUiResourceName );
            BaflUtils::NearestLanguageFile( eikonEnv->FsSession(), connUiResFile );
            }
        }

    // test if we have enough memory to add the resource file
    // this  like this, since AddResourceFileL does
    // not add a resourcefile with same signature twice.
    // so we have to first delete the old one and then add the new one
    // With this check we atleast test if there is some amount of memory
    // for loading the resource. If we leave from AddResourceFileL the
    // application will panic, since we have no resource file loaded at all.
    TEntry entry;
    User::LeaveIfError( eikonEnv->FsSession().Entry( path, entry ) );
    TInt fileSize( entry.iSize );
    // test if there's enough heap available, this one leaves if not and resources won't get changed
    HBufC8* allocTest = HBufC8::NewL( fileSize );
    delete allocTest;

    // delete the current resource file
    eikonEnv->DeleteResourceFile( iResourceFileOffset );
    eikonEnv->DeleteResourceFile( iResourceVariationFileOffset );

    // load the new resource file
    iResourceFileOffset = 0;
    iResourceVariationFileOffset = 0;

    iResourceFileOffset = eikonEnv->AddResourceFileL( path );
    iResourceVariationFileOffset = eikonEnv->AddResourceFileL( pathVariation );

    // Change branding in iConnectionUI
    User::LeaveIfError( RProperty::Set( KBrandingUid, KBrandingResourceKey, connUiResFile ) );

    // refresh the setting items
    LoadSettingsL();
    RefreshDialogL();
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::GetChatLoginType()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CWVSettingsUIDialogImp::GetChatLoginType() const
    {
    return iImLoginType;
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::HandleActiveDbEvent()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::HandleSAPEvent( TSAPEvent aEvent )
    {
    if ( aEvent == ESAPSettingChanged )
        {
        // we cannot leave here, if updating fails, only wrong name shown
        TInt ignore;
        TRAP( ignore, UpdateServerInUseItemL() );
        }
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::ServerListLC()
// SAP loading
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsList* CWVSettingsUIDialogImp::ServerListLC( TInt& aIndexToHighlight )
    {
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();

    TIMPSConnectionClient client( EIMPSConnClientIM );
    TIMPSAccessGroup accessGroup;
    if ( client == EIMPSConnClientIM )
        {
        accessGroup = EIMPSIMAccessGroup;
        }
    else
        {
        accessGroup = EIMPSPECAccessGroup;
        }
    iSAPSettingsStore->PopulateSAPSettingsListL( *sapList, accessGroup );

    // Check CR variation
    TInt val( 0 );

    TRAPD( err,
           CRepository* rep = CRepository::NewL( KCRUidIMVariation );
           rep->Get( KIMVariationKey, val );
           delete rep;
         );

    if ( err != KErrNotFound && err != KErrPathNotFound )
        {
        // "Not found" errors are ignored, because it's ok to
        // return zero if key is not found
        User::LeaveIfError( err );
        }

    TBool highlightDefaultServ = ETrue;
    TBool highlightLoggedInServ = val & KIMFeatHighlightLoggedInServer;

    if ( highlightLoggedInServ )
        {
        highlightDefaultServ = EFalse;

        CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();

        TBool isLoggedIn( EFalse );
        if ( iConnectionUI )
            {
            TInt err;
            TRAP( err, isLoggedIn =
                      iConnectionUI->LoggedInL( EIMPSConnClientIM ) );
            }

        if ( isLoggedIn )
            {
            CPEngNWSessionSlotID2* sessionSlotId =
                iConnectionUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
            CleanupStack::PushL( sessionSlotId );
            iConnectionUI->GetLoggedInSapL( *sessionSlotId, *sap );

            // and find our sap.. because logged in SAP has UID of zero, we must
            // find the correct UID manually from list
            TInt index( KErrNotFound );
            TPtrC name( sap->SAPName() );
            sapList->FindNameL( name, index );

            if ( index != KErrNotFound )
                {
                // found it, update the correct sap
                CIMPSSAPSettings* storedSap = CIMPSSAPSettings::NewLC();
                iSAPSettingsStore->GetSAPL( sapList->UidForIndex( index ),
                                            storedSap );

                // we are logged in a server, and it's index was found
                // -> highlight it
                aIndexToHighlight = sapList->IndexForUid( storedSap->Uid() );

                CleanupStack::PopAndDestroy( 3, sap ); // storedSap,
                // sessionslot,sap
                }
            else
                {
                highlightDefaultServ = ETrue;
                // Could not find the logged in server
                CleanupStack::PopAndDestroy( 2, sap );
                //,sessionSlotId, sap
                }
            }
        else
            {
            highlightDefaultServ = ETrue;
            CleanupStack::PopAndDestroy( sap );
            }
        }

    if ( highlightDefaultServ )
        {
        //get the default server index
        TUint32 defaultUid( KIMPSSettingsNullUid );

        TRAPD( err, iSAPSettingsStore->GetDefaultL( defaultUid, accessGroup ) );
        if ( err == KErrNone )
            {
            //default was defined and could retrieve a UID for it
            //==>report its index
            aIndexToHighlight = sapList->IndexForUid( defaultUid );
            }

        else if ( err == KErrNotFound )
            {
            //if there wasn't a default sap defined for some reason,
            //return the index as KErrNotFound
            aIndexToHighlight = KErrNotFound;
            err = KErrNone;
            }

        //Handle other errors by leaving
        User::LeaveIfError( err );
        }

    return sapList; //returns the ownership of sapList
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::LoadSapByServerListL()
// SAP loading
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::LoadSapByServerListL( const CIMPSSAPSettingsList& aList,
                                                   TInt aIndexOfSelectedServer,
                                                   CIMPSSAPSettings& aSap )
    {
    if ( ( aIndexOfSelectedServer < 0 ) ||
         ( aIndexOfSelectedServer > aList.Count() ) )
        {
        User::Leave( KErrArgument );
        }

    TUint32 uid = aList.UidForIndex( aIndexOfSelectedServer );

    iSAPSettingsStore->GetSAPL( uid, &aSap );
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::ServerToUseQueryL()
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUIDialogImp::ServerToUseQueryL( MDesCArray& aServers,
                                                 TInt aIndexToHighlight,
                                                 TInt& aSelectedIndex )
    {
    if ( aServers.MdcaCount() == 0 || aServers.MdcaCount() == 1 )
        {
        User::Leave( KErrNotSupported );
        }

    TRAPD( err, LoadServerIconsL( aServers ) );
    if ( err != KErrNone )
        {
        // didn't manage to load bitmaps, try to show the list without them
        delete iIconArray;
        iIconArray = NULL;
        }

    TInt selection( -1 );
    TInt retVal( -1 );
    // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
    // CAknIconArray::AppendL takes ownership of the icon
    CAknListQueryDialog* listQuery =
        new ( ELeave ) CAknListQueryDialog( &selection ); // CSI: 35 # See above

    if ( iIconArray )
        {
        // icon array exists, construct the query with icons
        listQuery->PrepareLC( R_SERVER_TO_LOGIN_LISTQUERY_GRAPHIC );
        listQuery->SetIconArrayL( iIconArray );
        iIconArray = NULL; // listbox has the ownership of icon array

        // construct formatted text array
        TInt count( aServers.MdcaCount() );
        CDesCArray* items = new( ELeave )CDesCArrayFlat( count );
        CleanupStack::PushL( items );
        for ( TInt i( 0 ); i < count; i++ )
            {
            TPtrC item( aServers.MdcaPoint( i ) );

            // 5 is more than enough for formatted number in front of server list
            HBufC* temp = HBufC::NewLC( item.Length() + KTabulator().Length() + 5 );
            TPtr tempPtr( temp->Des() );
            tempPtr.Num( i );
            tempPtr.Append( KTabulator );
            tempPtr.Append( item );

            items->AppendL( tempPtr );
            CleanupStack::PopAndDestroy( temp );
            }

        listQuery->SetItemTextArray( items );
        listQuery->SetOwnershipType( ELbmOwnsItemArray );
        CleanupStack::Pop( items ); // listbox model owns the item array
        }
    else
        {
        // icon array doesn't exist, construct the query without icons
        listQuery->PrepareLC( R_SERVER_TO_LOGIN_LISTQUERY );

        listQuery->SetItemTextArray( &aServers );
        listQuery->SetOwnershipType( ELbmDoesNotOwnItemArray ); // listbox model doesn't own the item array
        }

    //check is the index to highlight on the range...
    if ( ( aIndexToHighlight > 0 ) && ( aIndexToHighlight < aServers.MdcaCount() ) )
        {
        listQuery->ListBox()->SetCurrentItemIndex( aIndexToHighlight );
        }

    retVal = listQuery->RunLD(); //execute the selection


    // if selection made and not cancelled
    if ( ( selection >= 0 ) && ( retVal != 0 ) )
        {
        aSelectedIndex = selection;
        return ETrue;
        }


    return EFalse;
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::LoadServerIconsL()
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::LoadServerIconsL( MDesCArray& aServers )
    {
    TInt count( aServers.MdcaCount() );
    __ASSERT_DEBUG( count > 0, User::Panic( KIMPSConnUi_PanicCat, KErrGeneral ) );
    iIconArray = new( ELeave )CAknIconArray( count );

    // load icons
    TBool allEmpty( ETrue );
    HBufC* pathBuf = HBufC::NewLC( KMaxFileName );
    TPtr path( pathBuf->Des() );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( !ConstructCompletePathAndCheck( path, KModuleId,
                                             aServers.MdcaPoint( i ), KBitmapFileServerList ) )
            {
            // not found, use empty
            AknIconUtils::CreateIconLC( bitmap, mask, AknIconUtils::AvkonIconFileName(),
                                        EMbmAvkonQgn_prop_empty, EMbmAvkonQgn_prop_empty_mask );
            CleanupStack::Pop( 2 ); // bitmap, mask
            // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
            // CAknIconArray::AppendL takes ownership of the icon
            CGulIcon* gulIcon = CGulIcon::NewL( bitmap, mask ); // CSI: 35 # See above
            iIconArray->AppendL( gulIcon );
            }
        else
            {
            // load the icon that was found
            AknIconUtils::CreateIconLC( bitmap, mask, path, 0, 1 );
            CleanupStack::Pop( 2 ); // bitmap, mask
            // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
            // CAknIconArray::AppendL takes ownership of the icon
            CGulIcon* gulIcon = CGulIcon::NewL( bitmap, mask ); // CSI: 35 # See above
            iIconArray->AppendL( gulIcon );
            allEmpty = EFalse;
            }
        }
    CleanupStack::PopAndDestroy( pathBuf );

    if ( allEmpty )
        {
        // every icon is empty, delete whole array and show list without icons
        delete iIconArray;
        iIconArray = NULL;
        }
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::ConstructCompletePathAndCheck
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUIDialogImp::ConstructCompletePathAndCheck( TDes& aResult,
                                                             const TDesC& aModuleId,
                                                             const TDesC& aAlternative,
                                                             const TDesC& aFileName )
    {
    // Construct full path for wanted resource
    // MMC drive letter must be here to make sure that TFindFile searches first
    // MMC for the correct brand
    MMCDrive( aResult );
    aResult.Append( KRelativeBrandPath() );
    aResult.Append( aModuleId );
    aResult.Append( KDirSeparator );
    aResult.Append( aAlternative );
    aResult.Append( KDirSeparator );
    aResult.Append( aFileName );

    // Find the actual file
    TFindFile fileFinder( CEikonEnv::Static()->FsSession() );
    TInt err( fileFinder.FindByDir( aFileName, aResult ) );

    // If file was found, then take the first appearance of it
    // (order is correct aready)
    if ( err == KErrNone )
        {
        aResult.Zero();
        aResult.Append( fileFinder.File() );
        }

#ifdef _DEBUG
    RDebug::Print( _L( "ConstructCompletePathAndCheck, TFindFile returned %d for %S" ), err, &aResult );
#endif

    // return was the file found or not
    return err == KErrNone;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::MMCDrive
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::MMCDrive( TDes& aDrive ) const
    {
    TParsePtrC mmc( PathInfo::MemoryCardRootPath() );
    aDrive.Zero();
    aDrive.Append( mmc.Drive() );
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::UpdateCbaL
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDialogImp::UpdateCbaL()
    {
    TInt index = iListbox->CurrentItemIndex();

    if ( !iCAServerSettings )
        {
        // If we don't show CA settings item, we have 1 item less
        // in the listbox -> thus +1 to the index
        index++;
        }

    TInt cbaRes = R_WVSETTINGSVIEW_SOFTKEYS_OPTIONS_BACK__CHANGE;
    switch ( index )
        {
        case EWVSettingsCASettings: // Flowthrough
        case EWVSettingsServers:
            {
            cbaRes = R_WVSETTINGSVIEW_SOFTKEYS_OPTIONS_BACK__OPEN;
            break;
            }
        case EWVSettingsServerInUse:
            {
            // Needs cmd id open
            cbaRes = R_WVSETTINGSVIEW_SOFTKEYS_OPTIONS_BACK__CHANGE2;
            break;
            }
        default:
            {
            break;
            }
        }

    ButtonGroupContainer().SetCommandSetL( cbaRes );
    ButtonGroupContainer().DrawNow();
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::LaunchCASettingsL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::LaunchCASettingsL()
    {

    // check if we have servers
    if ( !CheckAnyStoredServers() )
        return;


    CIMPSSAPSettings* sapSettings = CIMPSSAPSettings::NewLC();

    TInt indexToHiglight = KErrNotFound;  //UI handles this by higlighting the first
    TInt selection = KErrNotFound;
    TInt retStatus = KErrNone;

    CIMPSSAPSettingsList* servers = ServerListLC( indexToHiglight );
    if ( servers->MdcaCount() == 0 )
        {
        //No SAPs at all
        retStatus = KErrArgument;
        }

    else if ( servers->MdcaCount() == 1 )
        {
        //one SAP -> use it
        LoadSapByServerListL( *servers, 0, *sapSettings ); //one SAP --> its index is zero
        retStatus = KErrNone;
        }

    else
        {
        if ( ServerToUseQueryL( *servers,
                                indexToHiglight,
                                selection ) )
            {
            //load sap according the user selection
            LoadSapByServerListL( *servers, selection, *sapSettings );
            retStatus = KErrNone;
            }

        else
            {
            //User declined the query
            retStatus = KErrCancel;
            }
        }

    CleanupStack::PopAndDestroy( servers ); //servers
    if ( retStatus == KErrNone )
        {
        HBufC* oldServer = NULL;
        MWVSettingsObserver* observer = iObserver;
        if ( iObserver )
            {
            // iObserver might not exist if we didn't get here through application settings
            // update previous server name
            oldServer = iObserver->CurrentServerNameLC();
            // load resource branding for this server (settings contain branded items)
            HBufC* newServer = sapSettings->SAPName().AllocLC();
            iObserver->ServerChangedL( *newServer );
            CleanupStack::PopAndDestroy( newServer );
            }
        CWVSettingsUIDialog** selfPtr = iSelfPtr;

        TInt retVal = iCAServerSettings->LaunchSettingsL( *sapSettings );

        if ( retVal == EAknSoftkeyExit && selfPtr )
            {
            *selfPtr = NULL;
            }
        if ( observer )
            {
            observer->ServerChangedL( *oldServer );
            CleanupStack::PopAndDestroy( oldServer );
            }

        if ( retVal != EAknSoftkeyExit )
            {
            RefreshTitleL();
            }
        }
    CleanupStack::PopAndDestroy( sapSettings );
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::ActivateCurrentItemL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::ActivateCurrentItemL()
    {
    TInt index = iListbox->CurrentItemIndex();
    TInt settingItem = index;
    TInt oldLoginType = iImLoginType;
    if ( !iCAServerSettings )
        {
        // if we don't show CA settings item, we have 1 item less
        // in the listbox -> thus +1 to the index
        settingItem++;
        }
    switch ( settingItem )
        {
        case EWVSettingsCASettings:
            {
            LaunchCASettingsL();
            break;
            }
        case EWVSettingsServers: // Fall through
        case EWVSettingsServerInUse:
#ifdef RD_MULTIPLE_DRIVE
        case EWVSettingsSaveChatsTo:
#endif
                {
                LaunchCorrectDialogL();
                break;
                }
        default:
            {

            // check if we have servers
            if ( !CheckAnyStoredServers() )
                return;

            TBool cancelled ( ETrue );
            TBool connHoursIMEdited( EFalse );

            // copy the selected days arrays
            CSelectionItemList* newSelectedDaysIM = CopySelectedDaysArrayL( iSelectedDaysIM );
            CleanupStack::PushL( newSelectedDaysIM );
            iSettingItemEditor->EditSettingItemL( ETrue,
                                                  index,
                                                  *iSettingItemArray,
                                                  *iSAPSettingsStore,
                                                  iImLoginType,
                                                  iImLoginTypeEdited,
                                                  newSelectedDaysIM,
                                                  iConnHoursIMInt,
                                                  connHoursIMEdited,
                                                  cancelled,
                                                  settingItem,
                                                  iConnectionUI,
                                                  &iForcedLoginMade );
            CleanupStack::Pop( newSelectedDaysIM );

            if ( iImLoginTypeEdited )
                {
                // Save IM login type right away to avoid always online queries
                // to be shown in chat ui side and to keep value correct if user
                // decides to do something that causes ChangeResourceFileL to be called
                // before exiting settings dialog.
                StoreSettingValueL( iImLoginType, EIMPSSharedKeysIMLogin );
                }
            if ( iForcedLoginMade )
                {
                // Set P&S flag to indicate forced login done
                TInt err = RProperty::Define( KBrandingUid,
                                              KForcedLoginKey,
                                              RProperty::EInt,
                                              KIMPSPubSubReadPolicy, KIMPSPubSubWritePolicy );
                if ( err != KErrAlreadyExists )
                    {
                    // Ignore KErrAlreadyExists errors
                    User::LeaveIfError( err );
                    }
                err = RProperty::Set( KBrandingUid, KForcedLoginKey, ETrue );
                User::LeaveIfError( err );
                }
            else
                {
                // Clear flag just to be sure
                TInt err = RProperty::Set( KBrandingUid, KForcedLoginKey, EFalse );
                if ( err != KErrNotFound )
                    {
                    // Ignore KErrNotFound
                    User::LeaveIfError( err );
                    }
                }

            if ( iSchedulingSupported )
                {
                if ( !cancelled )
                    {
                    // not cancelled -> take the new selected days arrays into use
                    iSelectedDaysIM->ResetAndDestroy();
                    delete iSelectedDaysIM;
                    iSelectedDaysIM = newSelectedDaysIM;
                    newSelectedDaysIM = NULL;

                    UpdateConnectionDaysL( *iSelectedDaysIM );

                    if ( connHoursIMEdited )
                        {
                        // the IM connection hours item has been edited
                        QueryConnectionHoursL();
                        }

                    // update IM connection hours text in the listbox
                    if ( iConnStartIM && iConnEndIM )
                        {
                        if ( ( iConnHoursIMInt == EWVSettingsAllDay )
                             || ( !iConnStartIM->Compare( iConnEndIM->Des() ) ) )
                            {
                            UpdateConnectionHoursTextL( ETrue );
                            }
                        else
                            {
                            UpdateConnectionHoursTextL( EFalse );
                            }
                        }
                    }
                }

            if ( newSelectedDaysIM )
                {
                newSelectedDaysIM->ResetAndDestroy();
                delete newSelectedDaysIM;
                }

            iSettingItemArray->At( index )->LoadL();
            iSettingItemArray->At( index )->UpdateListBoxTextL();

            if ( iImLoginTypeEdited )
                {
                // If changed to something else than manual
                // show warning if variated so
                if ( iImLoginType != EWVSettingsChatLoginManual &&
                     ( oldLoginType != iImLoginType ) )
                    {
                    // Check variation about password saving warning
                    TBool showWarning = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_SAVE_PW_WARNING );

                    if ( showWarning )
                        {
                        CCoeEnv* coeEnv = CCoeEnv::Static();

                        // Read texts from resources
                        HBufC* warningText = coeEnv->AllocReadResourceLC(
                                                 R_CHAT_SAVE_PW_WARNING );
                        HBufC* warningHeading = coeEnv->AllocReadResourceLC(
                                                    R_CHAT_SAVE_PW_WARNING_HEADING );

                        // Show warning with message query dialog
                        // CodeScanner warning ignored because CS does not
                        // notice that PrepareLC puts the dialog to cleanupstack
                        CAknMessageQueryDialog* dlg = new ( ELeave ) CAknMessageQueryDialog(); // CSI: 35 # See comment above
                        dlg->PrepareLC( R_WVSETTINGSVIEW_MESSAGE_QUERY_DIALOG );
                        dlg->QueryHeading()->SetTextL( *warningHeading );
                        dlg->SetMessageTextL( *warningText );
                        dlg->RunLD();

                        CleanupStack::PopAndDestroy( 2, warningText );
                        }
                    }
                RefreshDialogL();
                }

            DrawNow();
            break;
            }
        }
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::LaunchDlgCallback()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CWVSettingsUIDialogImp::LaunchDlgCallback( TAny* aDlg )
    {
    CWVSettingsUIDialogImp* dlg = static_cast<CWVSettingsUIDialogImp*>( aDlg );

    dlg->SetTargetDlgFlag( ETrue );
    TRAP_IGNORE( dlg->ActivateCurrentItemL() );
    dlg->SetTargetDlgFlag( EFalse );
    return EFalse;
    }

// ---------------------------------------------------------
// CWVSettingsUIDialogImp::LaunchCorrectDialog()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::LaunchCorrectDialog()
    {

    if ( GetTargetDlgFlag() )
        {
        return;
        }

    if ( !iDlgLauncher->IsActive() )
        {
        iDlgLauncher->Call();
        }
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::SetTargetDlgFlag()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIDialogImp::SetTargetDlgFlag( TBool aFlag )
    {
    iIsOpenedTargettDlg = aFlag;
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::SetTargetDlgFlag()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CWVSettingsUIDialogImp::GetTargetDlgFlag()
    {
    return iIsOpenedTargettDlg;
    }


// ---------------------------------------------------------
// CWVSettingsUIDialogImp::CheckAnyStoredServers()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CWVSettingsUIDialogImp::CheckAnyStoredServers()
    {
    if ( iSAPSettingsStore->SAPCountL( EIMPSIMAccessGroup ) == 0 )
        {

        //exitReason indicate UpdateServerInUseItemL() is required to call or not.
        TBool exitReason = ETrue;
        QueryNewServerL( &exitReason );
        if ( exitReason )
            {
            UpdateServerInUseItemL();
            }

        return EFalse;
        }
    else
        return ETrue;
    }
// -----------------------------------------------------------------------------
// CWVSettingsUIDialogImp::IntResourceValueL()
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUIDialogImp::IntResourceValueL( TInt aResourceId )
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

        TResourceReader reader;

        aResourceId = aResourceId + RSC_CRRSS_CHAT_VARIATION_IMPSCU_START_ID;

        CCoeEnv::Static()->CreateResourceReaderLC(
            reader,
            aResourceId );
        val = reader.ReadInt32();
        CleanupStack::PopAndDestroy(); // reader

        }

    return val;

    }


/**
* CWVSettingsUI panic handler
* @param aReason The reason for panic.
*/
GLDEF_C void Panic( TWVSettingsViewPanicReason aReason )
    {
    User::Panic( KPanicText, aReason );
    }


/**
 * Help key support - GetHelpContext is called by the framework on
 * event EAknCmdHelp
 * --------------------------------------------------------------------------------
 * CWVSettingsUIDefineNewServerDialog::GetHelpContext
 * @param 	aContext	The context dependent on the current view is passed by the framework.
 */
void CWVSettingsUIDialogImp::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_SETTINGS ;
    }
void CWVSettingsUIDialogImp::ChangeToManualL()
	{
	ChangeLoginTypeToManualL();
	}
//  End of File
