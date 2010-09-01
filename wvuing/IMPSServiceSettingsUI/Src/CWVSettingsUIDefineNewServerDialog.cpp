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
* Description:  Dialog for defining new server
*
*/


// INCLUDE FILES
#include    "CWVSettingsUIDefineNewServerDialog.h"
#include    "CWVSettingsUIDefs.h"
#include    "CWVSettingsUI.hrh"
#include    "MWVSettingsObserverng.h"
#include    "CWVSettingsUINGDialog.h"
#include    "WVSettingsUIPanics.h"
#include    "CWVSettingsUIDialogTitlePaneHandler.h"
#include    "CWVSettingsUIDialogImp.h"
#include    "IMPSPresenceLoginDetailValidator.h"
#include    "cwvsettingsuseridsettingpage.h"

#include	<csxhelp/imng.hlp.hrh>

#include    <aknlists.h>
#include    <commdb.h>
#include    <ApSettingsHandlerUI.h>
#include    <aputils.h>
#include    <aknsettingitemlist.h>
#include    <aknsettingpage.h>
#include    <akntextsettingpage.h>
#include    <AknPopupSettingPage.h>
#include    <akntitle.h>
#include    <aknquerydialog.h>
#include    <e32math.h>
#include    <cimpssapsettingsstore.h>
#include    <cimpssapsettings.h>
#include    <cimpssapsettingslist.h>
#include    <aknnotewrappers.h> // branding
#include    <stringloader.h>
#include    <AknIconArray.h>
#include    <data_caging_path_literals.hrh>
#include	<apsettings.mbg>
#include    <AknsUtils.h>
#include    <AknsConstants.h>
#include    <pathinfo.h>
#include    <aknmessagequerydialog.h>

#include    <hlplch.h>
#include    <featmgr.h>
#include    <aknsettingpage.h>
#include	<aknpasswordsettingpage.h>
#include    <cimpspresenceconnectionuing.h>
#include    <impspresenceconnectionuiconstsng.h>

#include    <CPEngNWSessionSlotID2.h>

#include    <barsread.h>
#include    <bautils.h>
#include    <sysutil.h>

#include    <CWVSettingsUIng.rsg>
#include    <aknutils.h>

#include	"CAExternalInterface.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// the enums for keys and central repository header is added here
// New rss file added for 401-1815 Ease of Instant Messaging branding
// These rss flags will be made available in ZCenrep also..

#include 	"VariantKeys.h"
#include 	<centralrepository.h>

// CONSTANTS

const TUint KWhiteSpace16( ' ' );


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::CWVSettingsUIDefineNewServerDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWVSettingsUIDefineNewServerDialog::CWVSettingsUIDefineNewServerDialog(
    TInt& aExitReason,
    CIMPSSAPSettingsStore& aSAPSettingsStore,
    TEditMode aEditMode,
    MWVSettingsObserver& aObserver,
    CWVSettingsUIDialogTitlePaneHandler& aTitlePaneHandler,
    CWVSettingsUIDialogImp* aDialog,
    TUint32& aNewUid,
    CIMPSPresenceConnectionUi* aConnectionUI,
    CIMPSSAPSettings* aServer /* = NULL  */ ):
        iExitReason( aExitReason ),
        iSAPSettingsStore( &aSAPSettingsStore  ),
        iEditMode( aEditMode ),
        iSAP( aServer ),
        iIAPNamePtr( NULL, 0 ),
        iServerNamePtr( NULL, 0 ),
        iWVUserIdPtr( NULL, 0 ),
        iPasswordPtr( NULL, 0 ),
        iURLPtr( NULL, 0 ),
        iObserver( &aObserver ),
        iTitlePaneHandler ( &aTitlePaneHandler ),
        iDialog ( aDialog ),
        iStoreOldId ( ETrue ),
        iOldUserIdPtr( NULL, 0 ),
        iEdited ( EFalse ),
        iNewUid( aNewUid ),
        iProtected( EFalse ),
        iConnectionUI( aConnectionUI ),
	iIsDeleted( EFalse ),
	isDisplayedSubDialog ( EFalse ) 
    {
    iCoeEnv = CCoeEnv::Static();
    }

// Destructor
CWVSettingsUIDefineNewServerDialog::~CWVSettingsUIDefineNewServerDialog()
    {
    if ( iSettingItemArray )
        {
        iSettingItemArray->ResetAndDestroy();
        }

    if ( iConnectionUIOwned )
        {
        delete iConnectionUI;
        }

    delete iOldServerName;

    delete iSettingItemArray;
    iSettingItemArray = NULL;
    delete iIAPName;
    delete iServerName;
    delete iWVUserId;
    delete iPassword;
    delete iURL;
    delete iAPUtils;
    delete iAPSettingsHandler;
    delete iCommsDb;
    delete iOldUserId;
    delete iSAP;
    delete iOldPassword;

    if( iDlgLauncher && iDlgLauncher->IsActive() ) 
        {
        iDlgLauncher->Cancel();
        }
    delete iDlgLauncher;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::EditSapSettingsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUIDefineNewServerDialog::EditSapSettingsL()
    {

    TUint32 wapAp( 0 );

    if ( iAPUtils->IAPExistsL( iIAP ) )
        {
        wapAp = iAPUtils->WapIdFromIapIdL( iIAP );
        }

    TUint32 previousAp( wapAp );
    TInt returnFlags ( iAPSettingsHandler->RunSettingsL( wapAp, wapAp ) );

    // was some item selected. Selecting with Pen doesn't cause
    // KApUiEventSelected event.
    TBool selected( returnFlags & KApUiEventSelected || wapAp != previousAp );

    if ( selected )
        {
        //got an new wap id - needs to be converted to IAP id
        iIAP = iAPUtils->IapIdFromWapIdL( wapAp );
        iIAPHasValue = ETrue;
        }

    // check if access point has been edited, selected or deleted
    if ( ( selected ) ||
         ( returnFlags & KApUiEventEdited ) ||
         ( returnFlags & KApUiEventDeleted ) )
        {
        iEdited = ETrue;

        //update IAP name field when selected, edited, or deleted
        //if current has been deleted -> NameL() leaves

        TRAPD( err, iAPUtils->NameL( wapAp, iIAPNamePtr ) )

        if ( err == KErrNotFound )
            {
            // this error happens, if access point has been deleted
            // we just empty the access point name
            iIAPNamePtr.Zero();
            err = KErrNone;      //ignore this error
            }
        else
            {
            //handle errors by leaving..
            User::LeaveIfError( err );
            }

        iSettingItemArray->At( ESAP )->LoadL();
        iSettingItemArray->At( ESAP )->UpdateListBoxTextL();
        }

    // it is checked later on if Exit was selected
    return returnFlags;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::StoreAllItemsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CWVSettingsUIDefineNewServerDialog::StoreAllItemsL()
    {

    if ( iSAP )
        {
        if ( iSAP->Protection() ==  ESAPBrandProtection )
            {
            iSAP->SetSAPUserIdL( *iWVUserId );
            iSAP->SetSAPUserPasswordL( *iPassword );
            iSAP->SetAccessPoint( iIAP );
            }
        else
            {
            iSAP->SetSAPNameL( *iServerName );
            iSAP->SetSAPAddressL( *iURL );
            iSAP->SetSAPUserIdL( *iWVUserId );
            iSAP->SetSAPUserPasswordL( *iPassword );
            iSAP->SetAccessPoint( iIAP );
            }

        if ( iEditMode == ENewServer )
            {
            iNewUid = iSAPSettingsStore->StoreNewSAPL( iSAP, EIMPSIMAccessGroup );
            }
        else // Edit-mode
            {
            iSAPSettingsStore->UpdateOldSAPL( iSAP, iSAP->Uid() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::AddItemL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::AddItemL( TDes& aBuffer,
                                                   TInt aId,
                                                   TInt aTitleResource,
                                                   TInt aDlgResource,
                                                   TBool aCompulsory /*= EFalse*/,
                                                   TBool aIconNeeded, /* = EFalse */
                                                   TItemType aItemType /*= ETextItem*/
                                                 )
    {
    CAknSettingItem* settingItem = NULL;
    TInt associatedResource( 0 );

    switch ( aItemType )
        {
        case ETextItem:
            {
            settingItem = new ( ELeave ) CAknTextSettingItem( aId,  aBuffer );
            associatedResource = EEikCtEdwin;
            break;
            }
        case EPasswordItem:
            {
            settingItem = new ( ELeave )
            CAknPasswordSettingItem( aId, CAknPasswordSettingItem::EAlpha, aBuffer );
            associatedResource = EEikCtSecretEd;
            break;
            }
        default:
            {
            User::Leave( EServerDialogUnknownItemType );
            break;
            }
        }

    CleanupStack::PushL( settingItem );

    HBufC* tempTitle = iEikonEnv->AllocReadResourceLC( aTitleResource );
    TBuf< KWVSettingsServerNameMaxLength > title ( *tempTitle );
    HBufC* itemTitle = title.AllocL();
    CleanupStack::PopAndDestroy( tempTitle );
    CleanupStack::PushL( itemTitle );

    // Construct correct compulsory indicator string which might also include
    // the protected icon.
    HBufC* compulsory = HBufC::NewLC( KCompulsoryChar().Length() +
                                      KIMPSIconIndex().Length() );
    TPtr compPtr( compulsory->Des() );

    if ( aCompulsory )
        {
        HBufC* text = CEikonEnv::Static()->AllocReadResourceLC( R_SELECT_SETTING_COMPULSORY );
        settingItem->SetEmptyItemTextL( *text );
        CleanupStack::PopAndDestroy( text );
        compPtr.Append( KCompulsoryChar() );
        settingItem->SetSettingPageFlags( CAknTextSettingPage::EZeroLengthNotOffered );
        }
    else
        {
        settingItem->SetEmptyItemTextL( KEmptyDesC );
        settingItem->SetSettingPageFlags( CAknTextSettingPage::EZeroLengthAllowed );
        }

    settingItem->ConstructL( EFalse, iOrdinal, title, NULL, aDlgResource, associatedResource );

    if ( iProtected && aIconNeeded )
        {
        // this item needs the protected icon
        compPtr.Append( KIMPSIconIndex() );
        }

    // This is used not only to set the compulsory icon, but also the
    // protected icon.
    settingItem->SetCompulsoryIndTextL( compPtr );

    iSettingItemArray->AppendL( settingItem );
    CleanupStack::PopAndDestroy( 2, itemTitle ); // compulsory, itemTitle
    CleanupStack::Pop( settingItem );

    iOrdinal++;
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::LoadExistingItemL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::LoadExistingItemL()
    {
    iServerNamePtr.Copy( iSAP->SAPName().Left( iServerNamePtr.MaxLength() ) );
    iURLPtr.Copy( iSAP->SAPAddress().Left( iURLPtr.MaxLength() ) );
    iWVUserIdPtr.Copy( iSAP->SAPUserId().Left( iWVUserIdPtr.MaxLength() ) );
    iPasswordPtr.Copy( iSAP->SAPUserPassword().Left( iPasswordPtr.MaxLength() ) );
    iIAP = iSAP->AccessPoint();
    iIAPHasValue = ETrue;

    //update IAP name field when selected, edited, or deleted
    //if current has been deleted -> NameL() leaves
    TRAPD( err, iAPUtils->NameL( iAPUtils->WapIdFromIapIdL( iIAP ),
                                 iIAPNamePtr ) );

    if ( ( err == KErrNotFound ) && ( iIAP != 0 ) )
        {
        // iIAP has a value, but it's not found
        // so we can reset the value to zero
        iIAP = 0;
        }

    if ( ( err == KErrNoMemory ) || ( err == KErrDiskFull ) )
        {
        User::Leave( err );
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::CompulsoryItemsFilled
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUIDefineNewServerDialog::CompulsoryItemsFilled()
    {
    TBool passed( ETrue );
    iFailedFields = 0;

    TInt compValues = GetCompulsoryItems();

    if ( compValues & EWVSettingsViewServerName )
        {
        if ( iServerNamePtr.Length() == 0 )
            {
            passed = EFalse;
            iFailedFields = iFailedFields | GenerateFlagValue( EServerName );
            }
        }

    if ( compValues & EWVSettingsViewIAPName )
        {
        if ( iIAPNamePtr.Length() == 0 )
            {
            passed = EFalse;
            iFailedFields = iFailedFields | GenerateFlagValue( ESAP );
            }
        }

    if ( compValues & EWVSettingsViewURL )
        {
        if ( iURLPtr.Length() == 0 )
            {
            passed = EFalse;
            iFailedFields = iFailedFields | GenerateFlagValue( EURL );
            }
        }

    if ( compValues & EWVSettingsViewWVUserId )
        {
        if ( iWVUserIdPtr.Length() == 0 )
            {
            passed = EFalse;
            iFailedFields = iFailedFields | GenerateFlagValue ( EWVId );
            }
        }

    if ( compValues & EWVSettingsViewPassword )
        {
        if ( iPasswordPtr.Length() == 0 )
            {
            passed = EFalse;
            iFailedFields = iFailedFields | GenerateFlagValue ( EPassword );
            }
        }

    return passed;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::CompulGetCompulsoryItems
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUIDefineNewServerDialog::GetCompulsoryItems (  ) const
    {
    return EWVSettingsViewServerName | EWVSettingsViewIAPName | EWVSettingsViewURL;
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::SignalStoreLToAllL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::SignalStoreLToAllL() const
    {
    TInt itemCount( iSettingItemArray->Count() );

    for ( TInt index = 0; index < itemCount; ++index )
        {
        iSettingItemArray->At( index )->StoreL();
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::FormUniqueServerNameL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUIDefineNewServerDialog::FormUniqueServerNameL()
    {
    HBufC* title = iEikonEnv->AllocReadResourceL(
                       RSC_WVSETTINGSVIEW_DEFAULT_SERVERNAME );
    iServerNamePtr.Copy( title->Left( iServerNamePtr.MaxLength() ) );
    delete title;

    TBool retStatus( EFalse );

    //get existing server names
    CDesCArrayFlat* serverNames = ServerListL();
    if ( serverNames )
        {
        CleanupStack::PushL( serverNames );
        retStatus = FormUniqueNameL( *serverNames, iServerNamePtr );
        CleanupStack::PopAndDestroy( serverNames );
        }

    return retStatus;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::FormUniqueNameL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUIDefineNewServerDialog::FormUniqueNameL( const CDesCArray& aExistingNames,
                                                           TDes& aNameToForm )
    {
    //calculate trailer length
    TInt nameTrailerTotalLength = KWVSettingsViewServerNameTrailerDecorationStart().Length() +
                                  KWVSettingsViewServerNameTrailerNumberWidth +
                                  KWVSettingsViewServerNameTrailerDecorationEnd().Length();
    TInt position( 0 );
    TBool nameChanged( EFalse );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( aNameToForm );
    if ( aExistingNames.Find(  aNameToForm, position, ECmpFolded ) == 0 )
        {
        //the name is found among the existing names
        //search for passfull trailer

        //initial trailer number
        TUint trailerNumber( KWVSettingsViewServerNameTrailerMinNumber );

        //setup temp buff
        HBufC* nameTryBuff = HBufC::NewLC( aNameToForm.MaxLength() );
        TPtr nameTry( nameTryBuff->Des() );

        //truncate the server name copy, to hold the trailer
        TInt availableSpaceForActualName( aNameToForm.MaxLength() - nameTrailerTotalLength );

        //find passfull server name
        TInt result( 0 );

        TBuf<KWVSettingsServerNameMaxLength> tempBuf( KNullDesC );
        while ( result == 0 )
            {
            // check how many numbers we need to have in the index
            tempBuf.Zero();
            tempBuf.AppendNum( trailerNumber );
            TInt length ( tempBuf.Length() );
            // but we want to have atleast 2
            if ( length < 2 )
                {
                length = 2;
                }

            nameTry.Zero();
            nameTry.Copy( aNameToForm.Left( availableSpaceForActualName ) );
            nameTry.Append( KWVSettingsViewServerNameTrailerDecorationStart );
            nameTry.AppendNumFixedWidth( trailerNumber, EDecimal, length );
            nameTry.Append( KWVSettingsViewServerNameTrailerDecorationEnd );

            //result == 0, if a matching descriptor is found.
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( nameTry );
            result = aExistingNames.Find( nameTry, position, ECmpFolded );
            trailerNumber++;
            }

        if ( result != 0 )
            {
            //free nameslot found
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( nameTry );
            aNameToForm.Zero();
            aNameToForm.Copy( nameTry.Left( aNameToForm.MaxLength() ) );
            nameChanged = ETrue;
            }

        CleanupStack::PopAndDestroy( nameTryBuff ); // nameTryBuff
        }
    return nameChanged;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::ChangeFocusToInvalidCompulsoryField
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::ChangeFocusToInvalidCompulsoryField() const
    {
    TBool found( EFalse );
    TInt itemIndex( 0 );

    do
        {
        if ( iFailedFields & GenerateFlagValue( itemIndex ) )
            {
            iListbox->SetCurrentItemIndexAndDraw( itemIndex );
            found = ETrue;
            }
        else
            {
            itemIndex++;
            }

        } while ( itemIndex < ELastItemMarker && !found );
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::ProcessCommandL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::ProcessCommandL( TInt aCommand )
    {
    HideMenu();

    switch ( aCommand )
        {
        case EAknSoftkeyEdit:   // Flowthrough, MSK command
        case EWVSettingsViewDefNewServerChange:
            {
            CEikListBox* listBox =
                static_cast <CEikListBox*> ( Control( EWVSettingsViewControlIdDefNewServer ) );
            HandleListBoxEventL( listBox, EEventEditingStarted );
            break;
            }
        // Help key support EWVSettingsViewCmdHelp is assigned EAknCmdHelp in
        // CWSettingsUI.hrh
        case EWVSettingsViewCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( CEikonEnv::Static()->WsSession(),
                                                 ( CCoeEnv::Static()->AppUi() )->AppHelpContextL () );
            break;
            }
        case EWVSettingsViewCmdExit:
            {
            TryExitL ( aCommand );
            iIsDeleted = ETrue;
            break;
            }
        default:
            {
            CEikonEnv::Static()->EikAppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::HandleListBoxEventL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::HandleListBoxEventL( CEikListBox* aListBox,
                                                              TListBoxEvent aEventType )
    {
    if ( aEventType == EEventEnterKeyPressed ||
         aEventType == EEventEditingStarted ||
         aEventType == EEventItemDoubleClicked )
        {
        TBool isPopup( aEventType == EEventEditingStarted );
        TInt index( aListBox->CurrentItemIndex() );

        if ( iConnectionUI )
            {
            CPEngNWSessionSlotID2* sessionSlotID = NULL;
            TRAPD( error, sessionSlotID =
                       iConnectionUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM ) );
            CleanupStack::PushL( sessionSlotID );
            TInt retVal( KErrNone );
            TIMPSConnectionOperation connOperation( EIMPSConnOppUnknown );

            if ( error == KErrNone )
                {
                // check if IM or PEC is connected to this server
                CIMPSSAPSettings* loggedInSap = CIMPSSAPSettings::NewLC();
                retVal = iConnectionUI->GetLoggedInSapL( *sessionSlotID, *loggedInSap );

                // check if we were logged in
                if ( retVal != KErrNone )
                    {
                    // we were not logged in
                    // check if the log in is ongoing
                    HBufC* buffer = NULL;

                    // if connOperation is != EAppConnOppClientLogin, then buffer is not
                    // initialised
                    connOperation = iConnectionUI->CurrentConnectionOperationL( buffer );
                    // check if there was an ongoing login operation
                    if ( connOperation == EIMPSConnOppClientLogin )
                        {
                        // login was ongoing, set the sap name for later checks
                        CleanupStack::PushL( buffer );
                        loggedInSap->SetSAPNameL( *buffer );
                        CleanupStack::PopAndDestroy( buffer );
                        }
                    }

                // check if there was an active connection or an ongoing login operation
                if ( ( retVal == KErrNone ) || ( connOperation == EIMPSConnOppClientLogin ) )
                    {

                    if ( loggedInSap->SAPName().CompareF ( iSAP->SAPName() ) == 0 )
                        {
                        if ( connOperation == EIMPSConnOppClientLogin )
                            {
                            // a login operation is ongoing, we cannot let the user edit the settings
                            HBufC* prompt = StringLoader::LoadLC(
                                                R_QTN_CHAT_LOGIN_ONGOING_NOTE,
                                                loggedInSap->SAPName(),
                                                iCoeEnv );

                            CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote( ETrue );
                            dlg->ExecuteLD( *prompt );
                            CleanupStack::PopAndDestroy( 3, sessionSlotID ); // prompt, loggedInSap, sessionSlotID
                            return;
                            }
                        // this is the server we are logged in to
                        // we have to query if we should log out
                        else if ( !ConfirmLogoutL() )
                            {
                            // user declined logout, we cannot delete this server
                            CleanupStack::PopAndDestroy( 2, sessionSlotID ); // loggedInSap, sessionSlotID
                            return;
                            }
                        }
                    }
                CleanupStack::PopAndDestroy( loggedInSap );
                }
            else if ( error != KErrNotFound )
                {
                // we ignore not found error, since then we know that we dont have a connection
                User::Leave( error );
                }
            CleanupStack::PopAndDestroy( sessionSlotID );
            }

        switch ( index )
            {
            case ESAP:
                {
                TInt returnFlags = EditSapSettingsL();

                if ( ( returnFlags == KApUiEventExitRequested ) ||
                     ( returnFlags == KApUiEventShutDownRequested ) )
                    {
                    // Exit was selected from the menu
                    TryExitL ( EWVSettingsViewCmdExit );
                    iIsDeleted = ETrue;
                    return;
                    }
                else
                    {
                    break;
                    }
                }


            case EServerName:
                {
                if ( iSAP->Protection() == ESAPBrandProtection )
                    {
                    // show note
                    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
                    CleanupStack::PushL( dlg );
                    HBufC* noteText = iEikonEnv->AllocReadResourceL( R_QTN_WVSETTINGSVIEW_BRAND_SERVER_INFO );
                    CleanupStack::Pop( dlg );
                    CleanupStack::PushL( noteText );
                    dlg->ExecuteLD( *noteText );
                    CleanupStack::PopAndDestroy( noteText );
                    }
                else
                    {
                    TInt returnFlags = EditServerNameL( index );
                    if ( returnFlags == EEikBidCancel )
                    	{                    	
                        if( !iDlgLauncher->IsActive() )
                            {
                            iDlgLauncher->Call();
                            }
                    	return;
                        }
                    }
                break;
                }

            case EURL:
                {
                if ( iProtected )
                    {
                    // this sap is protected, we do not have URL item in the listbox
                    // so this item is actually user id
                    iStoreOldId = ETrue;
                    EditUserIdL ( index );
                    }
                else
                    {
                    HBufC* urlTitle = StringLoader::LoadL( R_WVSETTINGSVIEW_SETTING_CONF_URL_TITLE );
                    CleanupStack::PushL( urlTitle );

                    // launch a T9-enabled text setting page
                    CAknSettingPage* dlg = new( ELeave ) CAknTextSettingPage(
                        urlTitle,
                        EAknSettingPageNoOrdinalDisplayed, // setting number (not used)
                        EAknCtSettingPage, // control type
                        0, // editor resource id (not used)
                        R_WVSETTINGSVIEW_SETTING_CONF_URL,  // setting page resource
                        iURLPtr,
                        CAknTextSettingPage::EPredictiveTextEntryPermitted );

                    dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );

                    iSettingItemArray->At( index )->LoadL();
                    CleanupStack::PopAndDestroy( urlTitle );

                    iEdited = ETrue;
                    }
                break;
                }


            case EWVSettingsViewWVUserIdIndex:
                {
                if ( iProtected )
                    {
                    // this sap is protected, we do not have URL item in the listbox
                    // so this item is actually password
                    iStoreOldId = ETrue;
                    EditPasswordL ( index );
                    }
                else
                    {
                    iStoreOldId = ETrue;
                    EditUserIdL ( index );
                    }
                break;
                }

            case EWVSettingsViewPasswordIndex:
                {
                iStoreOldId = ETrue;
                EditPasswordL ( index );
                break;
                }

            default:
                {
                iSettingItemArray->At( index )->EditItemL( isPopup );
                break;
                }
            }
        if ( iSettingItemArray )
        	{
            iSettingItemArray->At( index )->UpdateListBoxTextL();

            // Update title-pane if needed
            if( index == EServerName )
                {
                iTitlePaneHandler->SetTitlePaneTextL ( iSettingItemArray->At( index )->SettingTextL() );
                }
            DrawNow();
        	}
        }
    }

TInt CWVSettingsUIDefineNewServerDialog::LaunchDlgCallback( TAny* aDlg ) 
    {
    CWVSettingsUIDefineNewServerDialog* dlg = static_cast<CWVSettingsUIDefineNewServerDialog*>( aDlg );   
    TRAP_IGNORE( dlg->CloseDialogL() );
    return EFalse;
    }



void CWVSettingsUIDefineNewServerDialog::CloseDialogL() 
    {
    TryExitL ( EEikBidCancel );
    }
// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::HandleSettingPageEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::HandleSettingPageEventL(
    CAknSettingPage* aSettingPage,
    TAknSettingPageEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventSettingChanged:
            {
            CCoeControl* ctrl = aSettingPage->EditorControl();
            if ( ctrl )
                {
                CEikEdwin* edwin = static_cast<CEikEdwin*>( ctrl );
                if ( edwin->TextLength() == 0 && iDomainSelectionQuery )
                    {
                    // User has cleared user id field, pop-up domain
                    // selection query
                    if ( iConnectionUI->DisplayDomainSelectionQueryL( iWVUserIdPtr, iSAP ) )
                        {
                        edwin->SetTextL( &iWVUserIdPtr );
                        edwin->SetCursorPosL( 0, EFalse );
                        }
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::EditUserIdL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::EditUserIdL( TInt aIndex )
    {
    __ASSERT_DEBUG( aIndex >= 0, User::Panic( KPanicText, EWVSettingsListArrayIndexOutOfBounds ) );
    __ASSERT_DEBUG( aIndex < iSettingItemArray->Count(),
                    User::Panic( KPanicText, EWVSettingsListArrayIndexOutOfBounds ) );

    if ( iStoreOldId )
        {
        iOldUserIdPtr.Copy( iWVUserIdPtr.Left( iWVUserIdPtr.MaxLength() ) );
        }

    // change input mode according to resource variation
    // This function is provided to get the setting (mode of input) from Cenrep/Resource file.
    TBool textualInput = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_USERID_TEXTUAL_INPUTMODE );

    // Get setting item
    CAknSettingItem* item = ( *iSettingItemArray )[ aIndex ];

    TBool domainAccepted = EFalse;
    if ( iDomainSelectionQuery && iWVUserIdPtr.Length() == 0 )
        {
        // If domain query variation is enabled and user id length is 0
        // show domain selection query
        if ( !iConnectionUI )
            {
            // iConnectionUI is NULL if define new server dialog
            // is launched in application start up when there aren't
            // any servers defined
            iConnectionUI = CIMPSPresenceConnectionUi::NewL( EIMPSConnClientIM );
            iConnectionUIOwned = ETrue;
            }

        if ( iConnectionUI->DisplayDomainSelectionQueryL( iWVUserIdPtr, iSAP ) == 0 )
            {
            // User cancelled dialog
            iStoreOldId = ETrue;
            item->LoadL();
            item->UpdateListBoxTextL();
            iListbox->DrawItem( aIndex );
            return;
            }
        else
            {
            domainAccepted = ETrue;
            }
        }

    CAknTextSettingPage* settingPage = new ( ELeave ) CWVSettingsUserIDSettingPage (
        R_WVSETTINGSVIEW_SETTING_CONF_WVUID,
        iWVUserIdPtr,
        CAknTextSettingPage::EZeroLengthAllowed
        | CAknTextSettingPage::EPredictiveTextEntryPermitted );

    CleanupStack::PushL( settingPage );

    settingPage->ConstructL();
    CleanupStack::Pop( settingPage );

    // Set flags to edwin -> domain not selected in editor when launched
    if ( domainAccepted )
        {
        settingPage->TextControl()->AddFlagToUserFlags( CEikEdwin::ENoAutoSelection );
        }

    if ( ! textualInput )
        {
        // enable numeric input for the WVID field
        settingPage->TextControl()->SetAknEditorInputMode( EAknEditorNumericInputMode );
        }

    TInt mode = CAknSettingPage::EUpdateWhenAccepted;
    if ( iDomainSelectionQuery )
        {
        mode = CAknSettingPage::EUpdateWhenChanged;
        static_cast<CWVSettingsUserIDSettingPage*>( settingPage )->SetSettingPageObserver( this );
        }

    if ( settingPage->ExecuteLD( ( CAknSettingPage::TAknSettingPageUpdateMode )mode ) )
        {
        iEdited = ETrue;
        // accepted
        if ( iWVUserId->Length() > 0 )
            {
            // check wv id
            if ( !( IMPSPresenceLoginDetailValidator::ValidWVLoginIdL( *iWVUserId ) ) )
                {
                // show a note about the wrong wvid and return to editing
                HBufC* prompt = StringLoader::LoadLC( R_WVSETTINGSVIEW_ERROR_WRONG_WVID, *iWVUserId );
                // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
                // PrepareLC pushes the dialog into cleanupstack
                CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog( // CSI: 35 # See above
                    CAknNoteDialog::EErrorTone , CAknNoteDialog::ELongTimeout );

                dlg->PrepareLC( R_WVSETTINGSVIEW_ERROR_NOTE_TEMPLATE );
                dlg->SetTextL( *prompt );
                dlg->RunLD();

                CleanupStack::PopAndDestroy( prompt );
                iStoreOldId = EFalse;
                EditUserIdL ( aIndex );
                }
            }
        else
            {
            if ( CheckAutoLoginValuesL() )
                {
                // login type was automatic, we have to rollback changes
                iWVUserIdPtr.Copy( iOldUserIdPtr.Left( iOldUserIdPtr.MaxLength() ) );
                }
            }
        }
    else
        {
        // cancel pressed
        iWVUserIdPtr.Copy( iOldUserIdPtr.Left( iOldUserIdPtr.MaxLength() ) );
        }

    iStoreOldId = ETrue;
    item->LoadL();
    item->UpdateListBoxTextL();
    iListbox->DrawItem( aIndex );
    }


// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::EditPasswordL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::EditPasswordL( TInt aIndex )
    {
    __ASSERT_DEBUG( aIndex >= 0, User::Panic( KPanicText, EWVSettingsListArrayIndexOutOfBounds ) );
    __ASSERT_DEBUG( aIndex < iSettingItemArray->Count(),
                    User::Panic( KPanicText, EWVSettingsListArrayIndexOutOfBounds ) );

    //first just edit name
    CAknSettingItem* item = ( *iSettingItemArray )[ aIndex ];

    if ( iStoreOldId )
        {
        TPtr oldPasswordPtr ( iOldPassword->Des() );
        oldPasswordPtr.Copy( iPasswordPtr.Left( iPasswordPtr.MaxLength() ) );
        }

    CAknAlphaPasswordSettingPage* settingPage = new ( ELeave )
    CAknAlphaPasswordSettingPage(
        R_WVSETTINGSVIEW_SETTING_CONF_PW,
        iPasswordPtr ,
        iPasswordPtr );
    CleanupStack::PushL( settingPage );

    settingPage->ConstructL();
    CleanupStack::Pop( settingPage );

    if ( settingPage->ExecuteLD() )
        {
        // Check variation about password saving warning
        // This function is provided to get the setting (password saving warning) from Cenrep/Resource file.
        TBool showWarning = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_SAVE_PW_WARNING );

        if ( showWarning && iPassword->Length() > 0 )
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

        iEdited = ETrue;
        // accepted
        if ( iPassword->Length() == 0 )
            {
            if ( CheckAutoLoginValuesL() )
                {
                // login type was automatic, we have to rollback the changes
                TPtr oldPasswordPtr ( iOldPassword->Des() );
                iPasswordPtr.Copy( oldPasswordPtr.Left( oldPasswordPtr.MaxLength() ) );
                }
            }
        }
    else
        {
        // cancel pressed
        TPtr oldPasswordPtr ( iOldPassword->Des() );
        iPasswordPtr.Copy( oldPasswordPtr.Left( oldPasswordPtr.MaxLength() ) );
        }
    iStoreOldId = ETrue;
    item->LoadL();
    item->UpdateListBoxTextL();
    iListbox->DrawItem( aIndex );
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::CheckAutoLoginValuesL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUIDefineNewServerDialog::CheckAutoLoginValuesL()
    {
    TUint32 defaultSAPId( 0 );

    if ( !iDialog )
        {
        // if we dont have base dialog it means that chat launched this dialog byitself
        // and that means that automatic login cannot be on so we can just return false
        return EFalse;
        }

    // Query for default SAP uid
    iSAPSettingsStore->GetDefaultL( defaultSAPId, EIMPSIMAccessGroup );

    if ( iSAP->Uid() == defaultSAPId )
        {
        // check if the login type is automatic
        // or automatic in home network
        TInt imLoginType = iDialog->GetChatLoginType();
        if ( ( imLoginType == EWVSettingsChatLoginAutoAlways )
             || ( imLoginType == EWVSettingsChatLoginAutoInHomeNW )
             || ( imLoginType == EWVSettingsChatLoginApplicationLaunch ) )
            {
            // show an information note, password cannot be removed if automatic login

            CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
            CleanupStack::PushL( dlg );
            HBufC* noteText = iEikonEnv->AllocReadResourceL( R_WVSETTINGSVIEW_ERROR_ID_PASSWORD_MISSING );
            CleanupStack::Pop( dlg );
            CleanupStack::PushL( noteText );
            dlg->ExecuteLD( *noteText );
            CleanupStack::PopAndDestroy( noteText );



            // login type was automatic
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::EditServerNameL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUIDefineNewServerDialog::EditServerNameL( TInt aIndex )
    {
    __ASSERT_ALWAYS( aIndex >= 0, User::Panic( KPanicText, EWVSettingsListArrayIndexOutOfBounds ) );
    __ASSERT_ALWAYS( aIndex < iSettingItemArray->Count(),
                     User::Panic( KPanicText, EWVSettingsListArrayIndexOutOfBounds ) );

    HBufC* srvTitle = StringLoader::LoadL( R_WVSETTINGSVIEW_SETTING_CONF_SERVER_TITLE );
    CleanupStack::PushL( srvTitle );

    HBufC* oldServerName = iServerName->AllocLC();

    // get the setting item, used later
    CAknSettingItem* item = ( *iSettingItemArray )[ aIndex ];
    isDisplayedSubDialog = ETrue;
    // launch a T9-enabled text setting page
    CAknSettingPage* dlg = new( ELeave ) CAknTextSettingPage(
        srvTitle,
        EAknSettingPageNoOrdinalDisplayed, // setting number (not used)
        EAknCtSettingPage, // control type
        0, // editor resource id (not used)
        R_WVSETTINGSVIEW_SETTING_CONF_SERVER,  // setting page resource
        iServerNamePtr,
        CAknTextSettingPage::EPredictiveTextEntryPermitted );

    TBool returnvalue(dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ));  
    if ( iIsDeleted )
    	{    	
    	CleanupStack::PopAndDestroy( 2, srvTitle ); // oldServerName, srvTitle
    	isDisplayedSubDialog = EFalse;
    	return EEikBidCancel;
    	}    	
    // update setting item too
    item->LoadL();

    //get edited text and get rid of listbox control characters

    if ( *iServerName != *oldServerName )
        {
        // the server name was edited, check that it does not become a duplicate

        TPtr name = iServerName->Des();
        AknTextUtils::ReplaceCharacters( name,
                                         KAknReplaceListControlChars,
                                         TChar( KWhiteSpace16 ) );
        FormUniqueServerL();
        item->LoadL();
        // set on the flag that we should save the server
        iEdited = ETrue;
        }

    CleanupStack::PopAndDestroy( 2, srvTitle ); // oldServerName, srvTitle

    item->UpdateListBoxTextL();
    iListbox->DrawItem( aIndex );

    iTitlePaneHandler->SetTitlePaneTextL ( *iServerName );
	isDisplayedSubDialog = EFalse;
	return returnvalue;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::PreLayoutDynInitL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::PreLayoutDynInitL()
    {
    // Check disk space, use CCoeEnv's file server session
    if ( SysUtil::FFSSpaceBelowCriticalLevelL(
             &ControlEnv()->FsSession(), KWVSettingsServerSizeEstimate ) )
        {
        // Don't show any own notes here
        User::Leave( KErrDiskFull );
        }

    iCommsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    iAPUtils = CApUtils::NewLC( *iCommsDb );
    CleanupStack::Pop( iAPUtils );

    iDlgLauncher = new (ELeave) CAsyncCallBack( TCallBack( LaunchDlgCallback, this), 
                        		                                            CActive::EPriorityStandard );
    CApSettingsHandler* apSettingsHnadler = CApSettingsHandler::NewLC(	ETrue,							// Start with selection ?
                                                                       EApSettingsSelListIsPopUp,      // desired list type
                                                                       EApSettingsSelMenuSelectNormal, // desired menu type
                                                                       KEApIspTypeAll,                 // Filtering criteria on ISP type
                                                                       KWVSettingsViewBearerTypes,     // Filtering criteria on bearer type
                                                                       KEApSortNameAscending			// Specifies the sort order to use
                                                                       //- WLAN APN can not be used from IM Application
                                                                       //
                                                                       //EIPv4|EIPv6
                                                                       //End of
                                                                       //The error was raised for 3.1 and we have fixed the same to 3.2 also)
                                                                     );

    /*  To be uncommented once the CApSettingsHandler provides NewL methods.
    // Code scanner warning ignored LC method is the only available and it is popped right after
    iAPSettingsHandler = CApSettingsHandler::NewLC( // CSI: 49 # See above
      ETrue,							// Start with selection ?
      EApSettingsSelListIsPopUp,      // desired list type
      EApSettingsSelMenuSelectNormal, // desired menu type
      KEApIspTypeAll,                 // Filtering criteria on ISP type
      KWVSettingsViewBearerTypes,     // Filtering criteria on bearer type
      KEApSortNameAscending,			// Specifies the sort order to use
      //WLAN APN can not be used from IM Application
      EIPv4|EIPv6
      //End
      //The error was raised for 3.1 and we have fixed the same to 3.2 also

      ); */
    iAPSettingsHandler = apSettingsHnadler;

    CleanupStack::Pop( apSettingsHnadler );

    HBufC* popUpPrompt = iCoeEnv->AllocReadResourceLC( R_WVSETTINGSVIEW_SETTING_CONF_SAP_TITLE );
    iAPSettingsHandler->SetTextOverrideL( EPopupPromptText, *popUpPrompt );
    CleanupStack::PopAndDestroy( popUpPrompt ); //popUpPrompt

    iServerName = HBufC::NewL( KWVSettingsServerNameMaxLength );
    iServerNamePtr.Set( iServerName->Des() );

    iIAPName = HBufC::NewL( KWVSettingsServerSAPMaxLength );
    iIAPNamePtr.Set( iIAPName->Des() );

    iWVUserId = HBufC::NewL( KWVSettingsServerWVUserIdMaxLength );
    iWVUserIdPtr.Set( iWVUserId->Des() );

    iOldUserId = HBufC::NewL( KWVSettingsServerWVUserIdMaxLength );
    iOldUserIdPtr.Set ( iOldUserId->Des() );

    iPassword = HBufC::NewL( KWVSettingsServerPasswordMaxLength );
    iPasswordPtr.Set( iPassword->Des() );

    iURL = HBufC::NewL( KWVSettingsServerURLMaxlength );
    iURLPtr.Set( iURL->Des() );

    iOldPassword = HBufC::NewL( KWVSettingsServerPasswordMaxLength );

    if ( iEditMode == EEditServer )
        {
        LoadExistingItemL();
        }
    else
        {
        iSAP = CIMPSSAPSettings::NewL();
        FormUniqueServerNameL();
        }

    if ( iObserver )
        {
        // iObserver might not exist if we didn't get here through application settings
        // update previous server name
        iOldServerName = iObserver->CurrentServerNameLC();
        CleanupStack::Pop();
        // load resource branding for this server (settings contain branded items)
        // if this is different server than old server
        if ( iOldServerName->Compare( *iServerName ) != 0 )
            {
            iObserver->ServerChangedL( *iServerName );
            }
        }

    iTitlePaneHandler->SetTitlePaneTextL ( *iServerName );

    // check if the sap is protected
    if ( iSAP )
        {
        // check if the sap is protected
        if ( iSAP->Protection() == ESAPBrandProtection )
            {
            iProtected = ETrue;
            }
        }

    iListbox =
        static_cast< CAknSettingStyleListBox* >( Control( EWVSettingsViewControlIdDefNewServer ) );

    // Create item-array starting from ordinal 0
    iSettingItemArray = new ( ELeave ) CAknSettingItemArray( ELastItemMarker, EFalse, 1 );

    if ( iProtected )
        {
        // protected server -> we need to load icon array
        LoadIconArrayL();
        }

    AddItemL( iServerNamePtr,
              EServerName,
              R_WVSETTINGSVIEW_SETTING_CONF_SERVER_TITLE,
              R_WVSETTINGSVIEW_SETTING_CONF_SERVER,
              ETrue,
              iProtected );

    AddItemL( iIAPNamePtr,
              ESAP,
              R_WVSETTINGSVIEW_SETTING_CONF_SAP_TITLE,
              R_WVSETTINGSVIEW_SETTING_CONF_SAP,
              ETrue );

    // if the sap is protected we do not show the URL field to the user
    if ( !iProtected )
        {
        AddItemL( iURLPtr,
                  EURL,
                  R_WVSETTINGSVIEW_SETTING_CONF_URL_TITLE,
                  R_WVSETTINGSVIEW_SETTING_CONF_URL,
                  ETrue );
        }


    AddItemL( iWVUserIdPtr,
              EWVId,
              R_WVSETTINGSVIEW_SETTING_CONF_WVUID_TITLE,
              R_WVSETTINGSVIEW_SETTING_CONF_WVUID );

    AddItemL( iPasswordPtr,
              EPassword,
              R_WVSETTINGSVIEW_SETTING_CONF_PW_TITLE,
              R_WVSETTINGSVIEW_SETTING_CONF_PW,
              EFalse,
              EFalse,
              EPasswordItem );

    CTextListBoxModel* model = iListbox->Model();
    model->SetItemTextArray( iSettingItemArray );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray ); // Ownership retained by us

    iSettingItemArray->RecalculateVisibleIndicesL();
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto );
    iListbox->HandleItemAdditionL();
    iListbox->SetListBoxObserver( this );
    iListbox->UpdateScrollBarsL();

    // Check domain query variation
    // This function is provided to get the setting (domain query variation) from Cenrep/Resource file.
    iDomainSelectionQuery = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_DOMAIN_SELECTION );

    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::DynInitMenuPaneL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::DynInitMenuPaneL( TInt aResourceId,
                                                           CEikMenuPane* aMenuPane )
    {
    // if we're displaying general menu and help feature is not supported..
    if ( aResourceId == R_WVSETTINGSVIEW_GENERAL_MENU &&
         !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        // dimm the help menu item
        aMenuPane->SetItemDimmed( EWVSettingsViewCmdHelp, ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::PostLayoutDynInitL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::PostLayoutDynInitL()
    {
    if ( iEditMode == EEditServer )
        {
        ChangeFocusToInvalidCompulsoryField();
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::OkToExitL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUIDefineNewServerDialog::OkToExitL( TInt aButtonId )
    {


    if ( iExitReason == EApplicationExit )
        {
        return EFalse;
        }

    // show the menu
    if ( aButtonId == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        return EFalse;
        }

    if ( aButtonId == EAknSoftkeyEdit )
        {
        // MSK command
        ProcessCommandL( aButtonId );
        return EFalse;
        }

    // Take old servername and ask UI components to extrernalize their state
    TBuf<KWVSettingsServerNameMaxLength> serverName( *iServerName );
    SignalStoreLToAllL();

    // If all the data is filled, then save, otherwise ask for deletion
    if ( CompulsoryItemsFilled() )
        {

        if ( serverName != *iServerName )
            {
            FormUniqueServerL();
            }

        // If everything went ok, then store server
        // only try to save if something is edited
        if ( iEdited )
            {
            StoreAllItemsL();
            }

        if ( ( aButtonId == EWVSettingsViewCmdExit ) || ( aButtonId == EAknSoftkeyExit ) )
            {
            iExitReason = EApplicationExit;
            }
        else
            {
            iExitReason = EServerSaved;
            }
        }
    else
        {
        // if application exit requested, we don't show the query about deleting the server
        if ( ( aButtonId == EAknSoftkeyExit ) || ( aButtonId == EWVSettingsViewCmdExit ) )
            {
            iExitReason = EApplicationExit;
            return ETrue;
            }
        CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );
        CleanupStack::PushL( dlg );

        // Set prompt
        HBufC* text = CEikonEnv::Static()->AllocReadResourceLC( RSC_WVSETTINGSVIEW_COMPUL_QUERY );
        dlg->SetPromptL( *text );
        CleanupStack::PopAndDestroy( text );

        CleanupStack::Pop( dlg );
        TInt ret( dlg->ExecuteLD( R_DELETE_CONFIRMATION_QUERY ) );

        if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
            {
            iExitReason = EMissingCompulsory;
            }
        else
            {
            // User declined delete -> return to form
            ChangeFocusToInvalidCompulsoryField();
            return EFalse;
            }
        }

    // load resource branding for the previous server (settings contain branded items)
    if ( iObserver )
        {
        // iObserver might not exist if we didn't get here through application settings
        // If edited server wasr different than old server, load old server resources
        if ( iOldServerName->Compare( *iServerName ) != 0 )
            {
            iObserver->ServerChangedL( *iOldServerName );
            }
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::GenerateFlagValue
// Generates safe value for given seed (enum). This needs
// because we use this number in single master-flag with OR-operations
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUIDefineNewServerDialog::GenerateFlagValue( TInt aSeed ) const
    {
    TInt generatedNumber( 1 );

    for ( TInt i( 0 ); i < aSeed; ++i )
        {
        generatedNumber *= 2;
        }

    return generatedNumber;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::ServerListL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CDesCArrayFlat* CWVSettingsUIDefineNewServerDialog::ServerListL() const
    {
    TInt srvCount( iSAPSettingsStore->SAPCountL( EIMPSIMAccessGroup ) );

    if ( srvCount > 0 )
        {
        CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
        iSAPSettingsStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

        //get existing server names
        CDesCArrayFlat* serverNames = new( ELeave ) CDesCArrayFlat( srvCount );
        CleanupStack::PushL( serverNames );

        for ( TInt ii( 0 ); ii < srvCount; ++ii )
            {
            serverNames->AppendL( sapList->MdcaPoint( ii ) );
            }

        CleanupStack::Pop( serverNames );
        CleanupStack::PopAndDestroy( sapList );
        return serverNames;
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::FormUniqueServerL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::FormUniqueServerL()
    {
    TInt pos( 0 );
    CDesCArrayFlat* serverNames = ServerListL();

    if ( !serverNames )
        {
        return;
        }

    CleanupStack::PushL( serverNames );

    TBool serverExists( serverNames->Find( *iServerName, pos ) == 0 );

    if ( serverExists )
        {
        // the name is already in the list modify the new name
        TBuf<KWVSettingsServerNameMaxLength> serverName;
        serverName.Copy( ( *iServerName ).Left( serverName.MaxLength() ) );

        if ( FormUniqueNameL( *serverNames, serverName ) )
            {
            iServerNamePtr.Copy( serverName.Left( iServerNamePtr.MaxLength() ) );
            }
        }

    CleanupStack::PopAndDestroy( serverNames );
    }


// ---------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::OfferKeyEventL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CWVSettingsUIDefineNewServerDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if ( aKeyEvent.iCode == EKeyEscape )
        {
		if ( isDisplayedSubDialog )
			{
			iIsDeleted = ETrue;
			}
		else
			{
			TryExitL( EAknSoftkeyCancel );
			}
        return EKeyWasConsumed;
        }

    if ( iIsDeleted )
        {
        return EKeyWasConsumed;
        }
    else
        {
        return iListbox->OfferKeyEventL( aKeyEvent, aType );
        }
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::ConfirmLogoutL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUIDefineNewServerDialog::ConfirmLogoutL()
    {

    // check if pec and chat are logged in
    TBool loggedIn = iConnectionUI->LoggedInL( EIMPSConnClientIM );

    if ( loggedIn )
        {
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
            // user confirmed logout -> disconnect all open connections
            CPEngNWSessionSlotID2* sessionSlotID =
                iConnectionUI->GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
            CleanupStack::PushL( sessionSlotID );
            User::LeaveIfError( iConnectionUI->LogoutL( *sessionSlotID ) );
            CleanupStack::PopAndDestroy( sessionSlotID );
            return ETrue;
            }
        else
            {
            // the user declined the logout, we can just return EFalse
            return EFalse;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::LoadIconArrayL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUIDefineNewServerDialog::LoadIconArrayL()
    {
    // Create array for listbox graphics
    CAknIconArray* icons = new ( ELeave ) CAknIconArray( 1 ); // one icon in array
    CleanupStack::PushL( icons );
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    TFileName bitmapfilepath;
    bitmapfilepath.Append( TParsePtrC( PathInfo::RomRootPath() ).Drive() );
    bitmapfilepath.Append( KDC_APP_BITMAP_DIR() );
    bitmapfilepath.Append( KBitmapFile() );

    AknsUtils::CreateIconLC( AknsUtils::SkinInstance(),
                             KAknsIIDQgnIndiSettProtectedAdd,
                             bitmap, mask, bitmapfilepath,
                             EMbmApsettingsQgn_indi_sett_protected_add,
                             EMbmApsettingsQgn_indi_sett_protected_add_mask );
    icons->AppendL( CGulIcon::NewL( bitmap, mask ) );
    CleanupStack::Pop( 2 ); // mask, bitmap

    CArrayPtr<CGulIcon>* oldIconArray = iListbox->ItemDrawer()->ColumnData()->IconArray();

    if ( oldIconArray )
        {
        oldIconArray->ResetAndDestroy();
        delete oldIconArray;
        }

    // Transfer ownership to listbox
    iListbox->ItemDrawer()->ColumnData()->SetIconArray( icons );
    CleanupStack::Pop( icons );
    }
// ---------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::MappedCommandId()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CWVSettingsUIDefineNewServerDialog::MappedCommandId( TInt aButtonId )
    {
    // change cancel id so avkon don't remap it to 0
    if ( aButtonId == EEikBidCancel )
        {
        // This value has to be something else than EEikBidCancel
        // returned value in here is NOT the return value of ExecuteLD.
        return EEikBidCancel + 1;
        }
    return aButtonId;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDefineNewServerDialog::IntResourceValueL()
// @param aResourceId 	The resource ID to be fetched from Cenrep (if present) or else from the variation file
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUIDefineNewServerDialog::IntResourceValueL( TInt aResourceId )
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
 * Help key support - GetHelpContext is called by the framework on
 * event EAknCmdHelp
 * --------------------------------------------------------------------------------
 * CWVSettingsUIDefineNewServerDialog::GetHelpContext
 * @param 	aContext	The context dependent on the current view is passed by the framework.
 */
void CWVSettingsUIDefineNewServerDialog::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_DEFSERVER ;
    }



//  End of File
