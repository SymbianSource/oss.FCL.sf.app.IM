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
* Description:  Container class for settings view
*
*/

// INCLUDE FILES
#include	"CCASettingsViewContainer.h"
#include	"CCAAppUi.h"
#include	"CCAStatusPaneHandler.h"
#include	"CCAAppSettingsDialog.h"
#include	"MCAViewSwitcher.h"
#include    "CAExternalInterface.h"
#include	"ChatDefinitions.h"
#include	"CCAApp.h"
#include	"CCAVariantFactory.h"
#include	"MCAOpBrandVariant.h"
#include    "ChatDebugPrint.h"
#include    "CCAUISessionManager.h"
#include    "chatngclient.hrh"
#include    "CCAUISessionManager.h"
#include 	"CCAAppSettingsSAPExt.h"
#include    "impsbuilddefinitions.h"

#include	<barsread.h>
#include	<chatNG.rsg>
#include	<bldvariant.hrh>
#include	<csxhelp/imng.hlp.hrh>
#include	<CWVSettingsUINGDialog.h>
#include	<CIMPSSAPSettingsStore.h>
#include	<CIMPSSAPSettings.h>
#include	<bautils.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
// CONSTANTS
_LIT( KWVSettingsResFileName, "CWVSETTINGSUING.RSC" );
// The variation resource file (the settings) is added now.
_LIT( KIMPSCUVariationResFileName, "IMConnectionUIVariationNG.RSC" );


// ================= MEMBER FUNCTIONS =======================
//Default constructor
CCASettingsViewContainer::CCASettingsViewContainer( MCAViewSwitcher& aViewSwitcher,
                                                    MCASettingsPC& aSettings )
        : iViewSwitcher( aViewSwitcher ),
        iSettingsPC( aSettings )
    {
    }
// Symbian OS default constructor can leave.
void CCASettingsViewContainer::ConstructL( const TRect& aRect )
    {
    iSAPSettingsStore = CIMPSSAPSettingsStore::NewL();

    CCAApp* application =
        static_cast<CCAApp*>( iEikonEnv->EikAppUi()->Application() );
    CCAVariantFactory* var = application->VariantFactory();
    iOpBrand = var->OpBrandVariantL();
    CreateWindowL();

    iAppUi = static_cast<CCAAppUi*>( iEikonEnv->AppUi() );
    iAppUi->AddLayoutChangeObserver( this );

    iListBox = new ( ELeave ) CAknSettingStyleListBox;
    iListBox->SetContainerWindowL( *this );
    TResourceReader rr;
    iEikonEnv->CreateResourceReaderLC( rr, R_SETTINGS_LISTBOX );
    iListBox->ConstructFromResourceL( rr );
    CleanupStack::PopAndDestroy(); //RR

    iListBox->SetListBoxObserver( this );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto );

    //List items from resource
    CDesCArray* itemList =
        static_cast<CDesCArrayFlat*>( iListBox->Model()->ItemTextArray() );
    //There is always items, because loading from resource
    TInt itemCount( itemList->MdcaCount() );
    //We need new array that contains formatted items
    CDesCArray* newItemList = new ( ELeave ) CDesCArrayFlat( itemCount );
    CleanupStack::PushL( newItemList );

    HBufC* itemTxt = NULL;
    TPtr itemTxtPtr( 0, NULL );

    for ( TInt i( 0 ); i < itemCount; i++ )
        {
        itemTxt = HBufC::NewLC( itemList->MdcaPoint( i ).Length() +
                                KTab().Length() );
        itemTxtPtr.Set( itemTxt->Des() );

        itemTxtPtr.Copy( KTab );
        itemTxtPtr.Append( itemList->MdcaPoint( i ) );
        newItemList->AppendL( itemTxtPtr );

        CleanupStack::PopAndDestroy( itemTxt );
        }

    //Lets set new array for the list box
    CleanupStack::Pop( newItemList );
    iListBox->Model()->SetItemTextArray( newItemList );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    SetRect( aRect );
    ActivateL();
    }

// Destructor
CCASettingsViewContainer::~CCASettingsViewContainer()
    {
    if ( iAppUi && !iAppUi->IsUnderDestruction() )
        {
        iAppUi->RemoveLayoutChangeObserver( this );
        }

    delete iListBox;
    delete iSAPSettingsStore;
    }

// -----------------------------------------------------------------------------
// CCASettingsViewContainer::Listbox
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAknSettingStyleListBox* CCASettingsViewContainer::Listbox() const
    {
    return iListBox;
    }

// -----------------------------------------------------------------------------
// CCASettingsViewContainer::LaunchAppSettingsDialogL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASettingsViewContainer::LaunchAppSettingsDialogL()
    {

    CCAAppSettingsDialog* dlg = new ( ELeave ) CCAAppSettingsDialog(
        *( static_cast<CCAAppUi*>( iEikonEnv->AppUi() )->CAStatusPane() ),
        iSettingsPC,
        static_cast<CCAAppUi*>( iEikonEnv->AppUi() )->UISessionManager(), NULL );

    CleanupStack::PushL( dlg );
    dlg->ConstructL( R_CHATCLIENT_APPSETTINGS_MENUBAR );
    CleanupStack::Pop( dlg );

    dlg->ExecuteLD( R_CHATCLIENT_APPSETTINGS_DLG );

    }

// -----------------------------------------------------------------------------
// CCASettingsViewContainer::LaunchServSettingsDialogL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASettingsViewContainer::LaunchServSettingsDialogL(
    TBool aForcedLaunch /*= EFalse */ )
    {
    TFileName resPath;
    // The IM Variation Resource file which contains the settings should also be loaded during the launch of dialog
    TFileName resIMCUVariationPath;

    // if there are no SAPs then don't try the get default
    if ( iSAPSettingsStore->SAPCountL( EIMPSIMAccessGroup ) > 0 )
        {
        CIMPSSAPSettings* sap = static_cast<CCAAppUi*>( iEikonEnv->AppUi() )
                                ->UISessionManager().CurrentSAPLC();
        TPtrC sapName( sap->SAPName() );
        TPtrC wvSettRes( KWVSettingsResFileName );
        // The IM UI Variation Resource file is loaded
        TPtrC wvIMUIVariationRes( KIMPSCUVariationResFileName );


        iOpBrand->CurrentResourceFileL( sapName, wvSettRes, resPath );
        // The IM UI Variation Resource file is used to read the settings (if settings are not present in Cenrep)
        iOpBrand->CurrentResourceFileL( sapName, wvIMUIVariationRes, resIMCUVariationPath );

        if ( resPath.Length() != 0 )
            {
            BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), resPath );
            }

        if ( resIMCUVariationPath.Length() != 0 )
            {
            BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), resIMCUVariationPath );
            }

        CleanupStack::PopAndDestroy( sap );
        }

    TInt exitReason( 0 );
    iServSettingsDialog = CWVSettingsUIDialog::NewL();

#ifdef RD_SERVICE_ENHANCEMENTS
    iServSettingsDialogActive = ETrue;
#endif  // RD_SERVICE_ENHANCEMENTS

    // CodeScanner warning ignored because iServSettingsDialog
    // is not owned by us
    TInt retVal( iServSettingsDialog->RunDialogLD( *iSAPSettingsStore, // CSI: 50 # See comment above
                                                   this,
                                                   resPath,
                                                   resIMCUVariationPath,
                                                   exitReason,
                                                   NULL,
                                                   &iServSettingsDialog,
                                                   aForcedLaunch ) );
#ifdef RD_SERVICE_ENHANCEMENTS
    if ( exitReason != EForcedExit )
        {
        // Set dialog not active
        iServSettingsDialogActive = EFalse;
        }
#endif  // RD_SERVICE_ENHANCEMENTS

    iServSettingsDialog = NULL;

    // clear the navipane from unvanted tabs
    static_cast<CCAAppUi*>( iEikonEnv->AppUi() )->CAStatusPane()->ClearNaviPaneL();

    // this is for updating settings view
    if ( iNeedRefresh )
        {
        iViewSwitcher.SwitchViewL( KUidRefreshView );
        }

    if ( retVal == EWVSettingsViewExitCalled )
        {
        static_cast<CCAAppUi*>( iEikonEnv->AppUi() )->HandleCommandL( EChatClientCmdExit );
        }

    // update the title
    static_cast<CCAAppUi*>( iEikonEnv->AppUi() )->CAStatusPane()
    ->SetTitleL( iEikonEnv, R_CHATCLIEN_SETTINGS_VIEW_TITLE );
    }

#ifdef RD_SERVICE_ENHANCEMENTS
// -----------------------------------------------------------------------------
// CCASettingsViewContainer::IsServSettingsDialogActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASettingsViewContainer::IsServSettingsDialogActive() const
    {
    return iServSettingsDialogActive;
    }
#endif // RD_SERVICE_ENHANCEMENTS

// -----------------------------------------------------------------------------
// CCASettingsViewContainer::HandleListBoxEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASettingsViewContainer::HandleListBoxEventL( CEikListBox* aListBox,
                                                    TListBoxEvent aEventType )
    {
    if ( aEventType == EEventEnterKeyPressed ||
         aEventType == EEventItemDoubleClicked )
        {
        switch ( aListBox->CurrentItemIndex() )
            {
            case EApplicationSettings:
                {
                LaunchAppSettingsDialogL();
                break;
                }
            case EServiceSettings:
                {
                LaunchServSettingsDialogL();
                // this is for updating settings view
                if ( iNeedRefresh )
                    {
                    iViewSwitcher.SwitchViewL( KUidRefreshView );
                    }
                break;
                }
            default:
                {

                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCASettingsViewContainer::CountComponentControls
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCASettingsViewContainer::CountComponentControls() const
    {
    if ( iListBox )
        {
        return 1;
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// CCASettingsViewContainer::ComponentControl
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCoeControl* CCASettingsViewContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return iListBox;
            }
        default:
            {
            return NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCASettingsViewContainer::SizeChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASettingsViewContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// -----------------------------------------------------------------------------
// CCASettingsViewContainer::OfferKeyEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CCASettingsViewContainer::OfferKeyEventL( const TKeyEvent& aEvent,
                                                       TEventCode aType )
    {
    return iListBox->OfferKeyEventL( aEvent, aType );
    }

// ---------------------------------------------------------
// CCASettingsViewContainer::GetHelpContext
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASettingsViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_SETTINGS;
    }

// ---------------------------------------------------------
// CCASettingsViewContainer::FocusChanged
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASettingsViewContainer::FocusChanged( TDrawNow /* aDrawNow */ )
    {
    if ( iListBox )
        {
        // give focus to list box so that highlight animations
        // are done properly
        iListBox->SetFocus( IsFocused() );
        }
    }

// -----------------------------------------------------------------------------
// CCASettingsViewContainer::ServerChangedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCASettingsViewContainer::ServerChangedL ( const TDesC& aNewServerName, TBool /* aDefaultServerchanged  = EFalse */ )
    {
    CHAT_DP( D_CHAT_LIT( "CCASettingsViewContainer::ServerChangedL(%S)" ), &aNewServerName );
    if ( aNewServerName.Length() == 0 ||
         iSAPSettingsStore->SAPCountL( EIMPSIMAccessGroup ) == 0 )
        {
        // no servers
        return;
        }

    CCAAppUi* appUi = static_cast<CCAAppUi*>( iEikonEnv->EikAppUi() );
    iNeedRefresh = appUi->UISessionManager().BrandUIL( EFalse, aNewServerName );

    // Stop here if brand did not actually change

    if ( ! iNeedRefresh && ! iServSettingsDialog )
        {
        return;
        }

    // Update status-pane
    appUi->UISessionManager().SetStatusPaneIconsL();

    // Get resource file for settings dialog
    TFileName resPath;
    TFileName resIMCUVariationPath;

    TPtrC resId( KWVSettingsResFileName() );
    TPtrC resIMCUVariationId ( KIMPSCUVariationResFileName() );

    iOpBrand->CurrentResourceFileL( aNewServerName, resId, resPath );
    iOpBrand->CurrentResourceFileL( aNewServerName, resIMCUVariationId, resIMCUVariationPath );

    if ( resPath.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), resPath );
        }

    if ( resIMCUVariationPath.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), resIMCUVariationPath );
        }

    CHAT_DP( D_CHAT_LIT( "CCASettingsViewContainer::ServerChangedL, signalling service settings about resPath: %S" ), &resPath );
    iServSettingsDialog->ChangeResourceFileL( resPath, resIMCUVariationPath );
    }

// ---------------------------------------------------------
// CCASettingsViewContainer::LayoutChangedL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASettingsViewContainer::LayoutChangedL( TInt aType )
    {
    // updates tabgroup
    iViewSwitcher.CAStatusPane()->ShowTabGroupL( KUidSettingsView );

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    SetRect( mainPaneRect );
    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CCASettingsViewContainer::CurrentServerNameLC
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CCASettingsViewContainer::CurrentServerNameLC()
    {
    HBufC* sapName = NULL;
    TRAPD( err,
           CIMPSSAPSettings* sap = static_cast<CCAAppUi*>( iEikonEnv->AppUi() )->UISessionManager().CurrentSAPLC();
           // CodeScanner warning ignored because sapName is pushed to
           // cleanupstack later and there is no leaving code before it
           sapName = sap->SAPName().AllocL();  // CSI: 35 # See comment above
           CleanupStack::PopAndDestroy( sap );
         );

    if ( err != KErrNone || !sapName )
        {
        sapName = KNullDesC().AllocL();
        }

    CleanupStack::PushL( sapName );
    return sapName;
    }

//  End of File
