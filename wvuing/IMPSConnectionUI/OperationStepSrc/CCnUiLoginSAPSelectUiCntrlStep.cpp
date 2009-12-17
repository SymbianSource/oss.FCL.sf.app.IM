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
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsList.h>

#include "CCnUiLoginSAPSelectUiCntrlStep.h"
#include "CnUiSapLoginDataRefresher.h"
#include "cnuitermsofusedialoghandler.h"

#include "MCnUiUiControlContext.h"
#include "MCnUiUiFacade.h"
#include "MCnUiSignaller.h"

#include "CnUiErrors.h"

#include <barsc.h>
#include "CnUiResourceFileName.h"

#include "MIMPSSharedData.h"
#include "CIMPSSharedDataFactory.h"
#include "impspresenceconnectionuiconstsng.h"
#include <impsconnectionuing.rsg>

#include 	"VariantKeys.h"
#include 	<centralrepository.h>

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiLoginSAPSelectUiCntrlStep* CCnUiLoginSAPSelectUiCntrlStep::NewLC(
    MCnUiUiControlContext& aCCntxt,
    CIMPSSAPSettings& aLoginSap,
    TIMPSLoginType aLoginType,
    TBool aLastLoginSap /*= EFalse*/ )
    {
    CCnUiLoginSAPSelectUiCntrlStep* self = new ( ELeave )
    CCnUiLoginSAPSelectUiCntrlStep( aCCntxt,
                                    aLoginSap,
                                    aLoginType,
                                    aLastLoginSap );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CCnUiLoginSAPSelectUiCntrlStep::~CCnUiLoginSAPSelectUiCntrlStep()
    {
    iRFs.Close();
    delete iSharedData;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiLoginSAPSelectUiCntrlStep::CCnUiLoginSAPSelectUiCntrlStep( MCnUiUiControlContext& aCCntxt,
                                                                CIMPSSAPSettings& aLoginSap,
                                                                TIMPSLoginType aLoginType,
                                                                TBool aLastLoginSap /*= EFalse*/  )
        : iCCntxt( aCCntxt ),
        iLoginSap( aLoginSap ),
        iLoginType( aLoginType ),
        iLoginSapUpdateNeeded( EFalse ),
        iLoginSapUid( KIMPSSettingsNullUid ),
        iLastLoginSap( aLastLoginSap )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiLoginSAPSelectUiCntrlStep::ConstructL()
    {
    iSharedData = CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL( NULL, KBrandingUid );
    }


// -----------------------------------------------------------------------------
// CCnUiLoginSAPSelectUiCntrlStep::RunStepL()
// -----------------------------------------------------------------------------
//
TInt CCnUiLoginSAPSelectUiCntrlStep::RunStepL()
    {
    //First make sure that there isn't already another login
    //operation running (if there is, then no need to select SAP...)
    if ( iCCntxt.Signaller().OperationRunning() )
        {
        ShowSimultanousLoginOperationNoteL();
        return KCnUiErrorLoginOperationAlreadyInUse;
        }

    CIMPSSAPSettings* defaultSap = CIMPSSAPSettings::NewLC();
    TInt loadStatus( LoadDefaultSapL( *defaultSap ) );
    // an error will occur if there is no default sap
    // in this case we can ignore the error
    if ( loadStatus == KErrNone )
        {
        // to get the first dialog branded also
        iCCntxt.ProcessObserverProxy().NotifyAccessingSapL( *defaultSap );
        }

    //load SAP
    loadStatus = KErrNone;
    if ( !iLastLoginSap )
        {
        if ( ( iLoginType == EIMPSApplicationLaunch ) ||
             ( iLoginType == EIMPSAAConnectionStart ) )
            {
            loadStatus = LoadDefaultSapL( iLoginSap );
            }
        else
            {
            loadStatus = SelectSapManuallyL( iLoginSap );
            }
        }
    //quit if loading failed
    if ( loadStatus != KErrNone )
        {
        iLoginSap.Reset();
        CleanupStack::PopAndDestroy( defaultSap );
        return loadStatus;
        }

    //SAP selected successfully
    //notify observers from accessing the login SAP
    //this must be done here before reading the resource
    //since there might be branding involved in selecting the server
    iCCntxt.ProcessObserverProxy().NotifyAccessingSapL( iLoginSap );

    //Signal to other clients that login operation is started.
    if ( iCCntxt.Signaller().SignalOperationL( iLoginSap.SAPName() ) == KErrInUse )
        {
        //some other client has started the login
        //during SAP selection...
        ShowSimultanousLoginOperationNoteL();
        return KCnUiErrorLoginOperationAlreadyInUse;
        }

    //if the SAP is missing both user details, it needs to be
    //saved after succesful login
    if ( ( iLoginSap.SAPUserId().Length() == 0 ) &&
         ( iLoginSap.SAPUserPassword().Length() == 0 ) )
        {
        iLoginSapUid = iLoginSap.Uid();
        iLoginSapUpdateNeeded = ETrue;
        }

    //initial login data query in manual login mode
    if ( iLoginType == EIMPSManualLogin )
        {
        if ( !CnUiSapLoginDataRefresher::RefreshLoginDataL( iCCntxt.Ui(),
                                                            iLoginSap,
                                                            EFalse ) )
            {
            //user didn't accept the initial login data query
            iCCntxt.Ui().ShowNoteL( ECnUiConnCanceled );
            CleanupStack::PopAndDestroy( defaultSap );
            return KErrCancel;
            }
        }

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
            // method, switch to default SAP and return KErrCancel
            iCCntxt.ProcessObserverProxy().NotifyAccessingSapL( *defaultSap );
            CleanupStack::PopAndDestroy( defaultSap );
            return retVal;
            }
        }


    CleanupStack::PopAndDestroy( defaultSap );
    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CCnUiLoginSAPSelectUiCntrlStep::HandleCompleteL()
// -----------------------------------------------------------------------------
//
TCnUiHandleCompleteStatus CCnUiLoginSAPSelectUiCntrlStep::HandleCompleteL()
    {
    iCCntxt.Signaller().CancelOperationSignal();

    if ( iLoginSapUpdateNeeded )
        {
        CIMPSSAPSettingsStore& sapStore = iCCntxt.SapStoreL();
        CIMPSSAPSettings* tmpSap = CIMPSSAPSettings::NewLC();

        //try load logged in SAP
        TRAPD( err, sapStore.GetSAPL( iLoginSapUid, tmpSap ) );
        if ( err == KErrNone )
            {
            //could load succesfully the login SAP
            //update the new username / password to it as needed
            //NOTE! Can't directly store whole SAP because some
            //other steps might have altered it...

            TInt showUidPwQueries = ReadResourceIntValueL( RSC_CHAT_VARIATION_IMPSCU_SAVE_PW_QUERY );

            if ( ! showUidPwQueries )
                {
                // must not store the user id, password here.
                // IM application will do it in this case.
                tmpSap->SetSAPUserIdL( iLoginSap.SAPUserId()  );
                tmpSap->SetSAPUserPasswordL( iLoginSap.SAPUserPassword() );
                sapStore.UpdateOldSAPL( tmpSap, iLoginSapUid );
                }
            }

        if ( err == KErrNotFound ) //ignore login SAP not found error
            {
            err = KErrNone;
            }

        //handle errors by leaving
        User::LeaveIfError( err );
        CleanupStack::PopAndDestroy( tmpSap ); //tmpSap
        }

    return ECnUiStepContinueTeardown;
    }


// -----------------------------------------------------------------------------
// CCnUiLoginSAPSelectUiCntrlStep::UndoStepL()
// -----------------------------------------------------------------------------
//
void CCnUiLoginSAPSelectUiCntrlStep::UndoStepL()
    {
    iCCntxt.Signaller().CancelOperationSignal();
    }


// -----------------------------------------------------------------------------
// CCnUiLoginSAPSelectUiCntrlStep::ShowSimultanousLoginOperationNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiLoginSAPSelectUiCntrlStep::ShowSimultanousLoginOperationNoteL()
    {
    HBufC* serverName = NULL;
    iCCntxt.Signaller().OperationDetailsL( serverName );

    //OperationDetailsL() returns the HBufC ownership
    CleanupStack::PushL( serverName );
    iCCntxt.Ui().ShowNoteL( ECnUiConnOperationAllreadyRunning, *serverName );
    CleanupStack::PopAndDestroy( serverName ); //serverName
    }



// -----------------------------------------------------------------------------
// CCnUiLoginSAPSelectUiCntrlStep::LoadDefaultSapL()
// -----------------------------------------------------------------------------
//
TInt CCnUiLoginSAPSelectUiCntrlStep::LoadDefaultSapL( CIMPSSAPSettings& aSap )
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
// CCnUiLoginSAPSelectUiCntrlStep::SelectSapManuallyL()
// SAP load.
// -----------------------------------------------------------------------------
//
TInt CCnUiLoginSAPSelectUiCntrlStep::SelectSapManuallyL( CIMPSSAPSettings& aSap )
    {
    TInt indexToHiglight = KErrNotFound;  //UI handles this by higlighting the first
    TInt selection = KErrNotFound;
    TInt retStatus = KErrNone;

    CIMPSSAPSettingsList* servers = ServerListLC( indexToHiglight );
    if ( servers->MdcaCount() == 0 )
        {
        //No SAPs at all
        retStatus = KCnUiErrorNoProperDefaultSap;
        }

    else if ( servers->MdcaCount() == 1 )
        {
        //one SAP -> use it
        LoadSapByServerListL( *servers, 0, aSap ); //one SAP --> its index is zero
        retStatus = KErrNone;
        }

    else
        {
        if ( iCCntxt.Ui().ServerToUseQueryL( *servers,
                                             indexToHiglight,
                                             selection ) )
            {
            //load sap according the user selection
            LoadSapByServerListL( *servers, selection, aSap );
            retStatus = KErrNone;
            }

        else
            {
            //User declined the query
            retStatus = KErrCancel;
            }
        }

    CleanupStack::PopAndDestroy(); //servers
    return retStatus;
    }


// -----------------------------------------------------------------------------
// CCnUiLoginSAPSelectUiCntrlStep::ServerListLC()
// SAP loading
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsList* CCnUiLoginSAPSelectUiCntrlStep::ServerListLC( TInt& aIndexToHighlight )
    {
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();

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
    sapStore.PopulateSAPSettingsListL( *sapList, accessGroup );

    //get the default server index
    TUint32 defaultUid( KIMPSSettingsNullUid );

    TRAPD( err, sapStore.GetDefaultL( defaultUid, accessGroup ) );
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

    return sapList; //returns the ownership of sapList
    }



// -----------------------------------------------------------------------------
// CCnUiLoginSAPSelectUiCntrlStep::LoadSapByServerListL()
// SAP loading
// -----------------------------------------------------------------------------
//
void CCnUiLoginSAPSelectUiCntrlStep::LoadSapByServerListL( const CIMPSSAPSettingsList& aList,
                                                           TInt aIndexOfSelectedServer,
                                                           CIMPSSAPSettings& aSap )
    {
    if ( ( aIndexOfSelectedServer < 0 ) ||
         ( aIndexOfSelectedServer > aList.Count() ) )
        {
        User::Leave( KErrArgument );
        }

    TUint32 uid = aList.UidForIndex( aIndexOfSelectedServer );

    CIMPSSAPSettingsStore& sapStore = iCCntxt.SapStoreL();
    sapStore.GetSAPL( uid, &aSap );
    }

// -----------------------------------------------------------------------------
// CCnUiLoginSAPSelectUiCntrlStep::ConnectionClient()
// Client information loading
// -----------------------------------------------------------------------------
//
TIMPSConnectionClient CCnUiLoginSAPSelectUiCntrlStep::ConnectionClient()
    {
    // rvct does not support casting from TAny* to enumeration value
    TInt returnValue = ( TInt )Dll::Tls();
    return ( TIMPSConnectionClient )returnValue;
    }

// -----------------------------------------------------------------------------
// CCnUiLoginSAPSelectUiCntrlStep::OpenResourceFileLC()
// !!!Notice!!!. Two variables in cleanupstack after call of this method.
// -----------------------------------------------------------------------------
//
void CCnUiLoginSAPSelectUiCntrlStep::OpenResourceFileLC( RResourceFile& aResourceFile )
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
// CCnUiLoginSAPSelectUiCntrlStep::ReadResourceIntValueL()
// -----------------------------------------------------------------------------
//
TInt CCnUiLoginSAPSelectUiCntrlStep::ReadResourceIntValueL( TInt aResourceId )
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
