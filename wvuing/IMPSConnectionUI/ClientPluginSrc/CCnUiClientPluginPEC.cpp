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
* Description:  PEC client plugin.
*
*/

// INCLUDE FILES
#include <E32std.h>

#include "MIMPSSharedData.h"
#include "CIMPSSharedDataFactory.h"
#include "IMPSUIDDefs.h"

//PEC attributes
#include <CPEngAttributeTransaction2.h>
#include <CPEngAttributeStore2.h>
#include <MPEngPresenceAttrModel2.h>
#include <MPEngPresenceAttrTypeProperties2.h>

//PEC attribute lists
#include <CPEngAttributeListStore2.h>
#include <CPEngAttributeListTransaction2.h>
#include <MPEngAttributeList2.h>

//PEC contact lists
#include <CPEngContactListStore2.h>
#include <CPEngContactListTransaction2.h>
#include <MPEngContactList2.h>

// PEC
#include <CPEngNWSessionSlotID2.h>
#include <PEngWVServices2.h>

#include <ImpsServices.h>
#include <PEngWVPresenceErrors2.h>
#include <PEngWVPresenceAttributes2.h>

#include <WVSettingsSharedDataNG.h>
#include "CCnUiClientPluginPEC.h"
#include "IMPSPublishLevelPolicy.h"
#include "impspresenceconnectionuiconstsng.h"


// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiClientPluginPEC* CCnUiClientPluginPEC::NewL( CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiClientPluginPEC* self = new ( ELeave ) CCnUiClientPluginPEC();

    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self ); //self

    return self;
    }


// Destructor
CCnUiClientPluginPEC::~CCnUiClientPluginPEC()
    {
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiClientPluginPEC::CCnUiClientPluginPEC()
        : CCnUiClientPluginBase( EIMPSConnClientPEC )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiClientPluginPEC::ConstructL( CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiClientPluginBase::ConstructL( aNWSessionSlotID );
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::CurrentConnectionModeSettingL()
// From MCnUiClientPlugin
// -----------------------------------------------------------------------------
//
TCnUiConnectionMode CCnUiClientPluginPEC::CurrentConnectionModeSettingL()
    {
    // dummy implementation
    TCnUiConnectionMode cMode( ECnUiCMAutomatic );
    return cMode;
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::ReWakeAutomaticConnectionModeL()
// From MCnUiClientPlugin
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginPEC::ReWakeAutomaticConnectionModeL()
    {
    // this is no longer needed
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::ConnectionOpenInitL()
// From MCnUiClientPlugin - connection open related methods
// Overloaded from CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginPEC::ConnectionOpenInitL( TBool aSapConnectionOpen,
                                                const CIMPSSAPSettings& aSap )
    {
    //let first the base class to do its things like disable synchronization
    //of unknown lists
    CCnUiClientPluginBase::ConnectionOpenInitL( aSapConnectionOpen, aSap );

    //if opening real NW connection, initialize the local
    //attribute lists and attributes here as needed
    //==>they will get synchronized with PEC engine login & synchronization
    if ( !aSapConnectionOpen )
        {
        IMPSPublishLevelPolicy::InitializePublishLevelL( *iNWSessionSlotID );
        }
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::DoPostLoginProsessingL()
// From MCnUiClientPlugin - connection open related methods
// Overloaded from CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginPEC::DoPostLoginProsessingL( TRequestStatus& aStatus )
    {

    //do first the client specific things (enable profiles presence updates)
    MIMPSSharedData* sharedData = CIMPSSharedDataFactory::CreatePermanentKeyHandlerL(
                                      NULL,
                                      KIMPSProfileEngineKeyUid );

    //From SharedDataKeys.h
    //1 == allowed to update Presence attributes
    //ignore possible settings errors, all this is done with best effort way...
    sharedData->SetIntKey( EIMPSSharedKeysProEngAllowUpdatePresence, 1 );

    delete sharedData;

    //and the let the base class take the charge...
    //NOTE!! Order must be this becase the base might start something
    //asynhronous & and after that leaving isn't allowd...

    CCnUiClientPluginBase::DoPostLoginProsessingL( aStatus );
    }




// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::DoPreLogoutProsessingL()
// From MCnUiClientPlugin - connection close related methods
// Overloaded from CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginPEC::DoPreLogoutProsessingL( TRequestStatus& aStatus )
    {

    //do first the client specific things (disable profiles presence updates)
    MIMPSSharedData* sharedData = CIMPSSharedDataFactory::CreatePermanentKeyHandlerL(
                                      NULL,
                                      KIMPSProfileEngineKeyUid );

    //From SharedDataKeys.h
    //0 == not allowed to update Presence attributes, default
    //ignore possible settings errors, all this is done with best effort way...
    sharedData->SetIntKey( EIMPSSharedKeysProEngAllowUpdatePresence, 0 );

    delete sharedData;

    //and the let the base class take the charge...
    //NOTE!! Order must be this becase the base might start something
    //asynhronous & and after that leaving isn't allowd...
    CCnUiClientPluginBase::DoPreLogoutProsessingL( aStatus );
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::DoVerifySapCapabilities()
// From CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
TBool CCnUiClientPluginPEC::DoVerifySapCapabilities( TPEngWVCspServicesTree2 aCurrentNwServices )
    {
    // For PEC WV server must support all of these main features
    // Logical and is used
    //==> if any of these is EFalse, result will be EFalse

    TBool capabOK = aCurrentNwServices.iFundamentalFeat.FeatureSupported();
    capabOK &= aCurrentNwServices.iFundamentalFeat.SubFunctionSupported( KPEngFFSubFuncSRCH );
    capabOK &= aCurrentNwServices.iFundamentalFeat.SubFunctionSupported( KPEngFFSubFuncSTSRC );

    capabOK &= aCurrentNwServices.iPresenceFeat.FeatureSupported();
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncGCLI );
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncCCLI );
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncDCLI );
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncMCLS );

    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncGETWL );
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncGETPR );
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncUPDPR );

    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncCALI );
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncDALI );
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncGALS );



    return capabOK;
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::DoClientKnownCntListsLC()
// Template method from CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
CDesCArray* CCnUiClientPluginPEC::DoClientKnownCntListsLC()
    {
    CDesCArrayFlat* lists = new ( ELeave ) CDesCArrayFlat( 4 ); //4 known lists
    CleanupStack::PushL( lists );

    //enumerate here those lists that are used by PEC & needs to be
    //synchronized when PEC is doing login

    CPEngContactListStore2* contactListStore = CPEngContactListStore2::NewLC( *iNWSessionSlotID );

    const MDesCArray& array = contactListStore->AllContactListsL();

    for ( TInt i = 1; i < array.MdcaCount(); i++ )
        {
        TPtrC list = array.MdcaPoint( i );
        if ( list.FindF( KIMPSPECContactListIdentifier() ) != KErrNotFound  )
            {
            // this seems to be PECUi's list
            lists->AppendL( list );
            }
        }
    CleanupStack::PopAndDestroy( contactListStore );

    return lists;
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::DoStartExplicitPostLoginSyncL()
// From CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginPEC::DoStartExplicitPostLoginSyncL()
    {
    // first do the base synchronization
    User::LeaveIfError( iPEngCntListPublisher->BaseSynchronizeContactLists( *this ) );
    SetStepProcessingActive();
    iStepId = EBaseCntListSynch;
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::DoHandleExplicitPostLoginSyncStepCompleteL()
// From CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginPEC::DoHandleExplicitPostLoginSyncStepCompleteL( TStepId aStepId,
                                                                       TInt& aStepStatus )
    {
    switch ( aStepId )
        {
        case EBaseCntListSynch:
            {
            if ( aStepStatus == KErrNone )
                {
                //initialize the local attribute lists as needed
                IMPSPublishLevelPolicy::InitializePublishLevelL( *iNWSessionSlotID );

                //and synchronize known PEC list explicitly with network server
                MDesCArray* knownLists = DoClientKnownCntListsLC();
                User::LeaveIfError( iPEngCntListPublisher->SynchronizeContactLists(
                                        *knownLists,
                                        *this ) );
                CleanupStack::PopAndDestroy();  //knownLists
                SetStepProcessingActive();
                iStepId = ECntListPublish;
                }
            break;
            }

        case ECntListPublish:
            {
            if ( ( aStepStatus == KErrNone ) ||
                 ( aStepStatus == KPEngNwErrPartiallySuccessful ) )
                {
                //Contact lists are now successfully synchronized
                aStepStatus = KErrNone;

                User::LeaveIfError( iPEngAttributeListPublisher->PublishAttributeLists( *this ) );
                SetStepProcessingActive();
                iStepId = EAttribListPublish;
                }
            break;
            }

        case EAttribListPublish:
            {
            if ( ( aStepStatus == KErrNone ) ||
                 ( aStepStatus == KPEngNwErrPartiallySuccessful ) )
                {
                //Attribute lists are now successfully synchronized


                aStepStatus = KErrNone;
                UpdateAttributesL();
                SetStepProcessingActive();
                iStepId = EAttributePublish;
                }
            break;
            }


        case EAttributePublish:
            {
            //ignore possibly from attribute publish propagating errors
            aStepStatus = KErrNone;
            break;
            }


        default:
            {
            //nothing to do
            break;
            }
        }
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::DoStartPreLogoutPublishL()
// From CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginPEC::DoStartPreLogoutPublishL()
    {
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::DoHandlePreLogoutPublishStepCompleteL()
// From CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginPEC::DoHandlePreLogoutPublishStepCompleteL( TStepId /*aStepId*/,
                                                                  TInt& /*aStepStatus*/ )
    {
    }

// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::DoHandlePreLogoutPublishStepCompleteL()
//
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginPEC::UpdateAttributesL()
    {
    RPointerArray<MPEngPresenceAttrModel2> models;
    CleanupStack::PushL( TCleanupItem( DestroyCloseModelArray, &models ) );

    MPEngPresenceAttrModel2* userAvail = NULL;
    TInt err = iPEngAttributeStore->GetAndLockOwnAttribute( KUidPrAttrUserAvailability,
                                                            userAvail );

    // if the model was locked, the pointer is still null, but we want to continue with
    // the other attributes
    if ( ( err != KErrNone ) && ( err != KErrLocked  ) )
        {
        User::Leave( err );
        }
    else
        {
        CleanupClosePushL( *userAvail );
        models.AppendL( userAvail );
        CleanupStack::Pop(); // userAvail
        }

    MPEngPresenceAttrModel2* onlineStatus = NULL;
    err = iPEngAttributeStore->GetAndLockOwnAttribute( KUidPrAttrOnlineStatus, onlineStatus );

    // if the model was locked, the pointer is still null, but we want to continue with
    // the other attributes
    if ( ( err != KErrNone ) && ( err != KErrLocked  ) )
        {
        User::Leave( err );
        }
    else
        {
        CleanupClosePushL( *onlineStatus );
        onlineStatus->SetQualifier( ETrue );
        models.AppendL( onlineStatus );
        CleanupStack::Pop(); // onlineStatus
        }

    MPEngPresenceAttrModel2* clientInfo = NULL;
    err = iPEngAttributeStore->GetAndLockOwnAttribute( KUidPrAttrClientInfo, clientInfo );

    // if the model was locked, the pointer is still null, but we want to continue with
    // the other attributes
    if ( ( err != KErrNone ) && ( err != KErrLocked  ) )
        {
        User::Leave( err );
        }
    else
        {
        CleanupClosePushL( *clientInfo );
        models.AppendL( clientInfo );
        CleanupStack::Pop(); // clientInfo
        }

    MPEngPresenceAttrModel2* statusText = NULL;
    err = iPEngAttributeStore->GetAndLockOwnAttribute( KUidPrAttrStatusText, statusText );

    // if the model was locked, the pointer is still null, but we want to continue with
    // the other attributes
    if ( ( err != KErrNone ) && ( err != KErrLocked  ) )
        {
        User::Leave( err );
        }
    else
        {
        CleanupClosePushL( *statusText );
        models.AppendL( statusText );
        CleanupStack::Pop(); // statusText
        }

    MPEngPresenceAttrModel2* statusContent = NULL;
    err = iPEngAttributeStore->GetAndLockOwnAttribute( KUidPrAttrStatusContent, statusContent );

    // if the model was locked, the pointer is still null, but we want to continue with
    // the other attributes
    if ( ( err != KErrNone ) && ( err != KErrLocked  ) )
        {
        User::Leave( err );
        }
    else
        {
        CleanupClosePushL( *statusContent );
        models.AppendL( statusContent );
        CleanupStack::Pop(); // statusContent
        }

    User::LeaveIfError( iPEngAttributePublisher->PublishAndUnLockOwnAttributes( models, *this ) );
    CleanupStack::Pop(); // models
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginPEC::DestroyCloseModelArray()
//
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginPEC::DestroyCloseModelArray( TAny* aObject )
    {
    reinterpret_cast<RPointerArray<MPEngPresenceAttrModel2>*>( aObject )->ResetAndDestroy();
    }

//  End of File




