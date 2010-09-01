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
* Description:  IM client plug-in.
*
*/

// INCLUDE FILES
#include "CCnUiClientPluginIM.h"
#include "CIMPSSharedDataFactory.h"
#include "IMPSSharedDataDefs.h"
#include "IMPSUIDDefs.h"
#include "impscommonuibuilddefinitions.h"

//PEC attributes
#include <CPEngAttributeTransaction2.h>
#include <CPEngAttributeStore2.h>
#include <MPEngPresenceAttrModel2.h>
#include <MPEngPresenceAttrTypeProperties2.h>
#include <PEngWVPresenceAttributes2.h>

//PEC attribute lists
#include <CPEngAttributeListStore2.h>
#include <CPEngAttributeListTransaction2.h>
#include <MPEngAttributeList2.h>

//PEC contact lists
#include <CPEngContactListTransaction2.h>
#include <CPEngContactListStore2.h>
#include <MPEngContactList2.h>
#include <PEngWVServices2.h>

// built-in resource reader
#include "CnUiResourceFileName.h"

#include "PAppVariationNG.hrh"

#include <IMPSConnectionUiNG.rsg>

#include <eikenv.h>
#include <StringLoader.h>
#include <barsread.h>
#include <ImpsServices.h>
#include <PEngWVPresenceErrors2.h>
#include <WVSettingsSharedDataNG.h>
#include <E32std.h>
#include <e32property.h>
#include <CIMPSSAPSettings.h>

#include 	"VariantKeys.h"
#include 	<centralrepository.h>
#include "CIMPSSharedDataFactory.h"
#include "MIMPSSharedData.h"


// CONSTANTS

_LIT( KPanicAttribute, "IMPSCommonUi Attribute" );

enum TIMAuthValues
    {
    EIMAuthValueAll = 0,
    EIMAuthValueFriends,
    EIMAuthValueNobody
    };

// Authorize IM presence
_LIT( KIMPresenceAuth,  "IMAuthIMPr" );
// Table of userId specific Aliases
_LIT( KIMOwnAlias,      "IMOwnAlias" );

// Separators for Alias table, to separate items from each other
// e.g [userid][itemsep][alias][tablesep][userid][itemsep][alias] etc...
//
const TUint KAliasItemSeparator  = 0x10;
const TUint KAliasTableSeparator = 0x11;
const TInt  KSeparatorSize       = 2; // bytes

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiClientPluginIM* CCnUiClientPluginIM::NewL( CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiClientPluginIM* self = new ( ELeave ) CCnUiClientPluginIM();

    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self ); //self

    return self;
    }


// Destructor
CCnUiClientPluginIM::~CCnUiClientPluginIM()
    {
    delete iSharedData;
    iRFs.Close();
    delete iAlias;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiClientPluginIM::CCnUiClientPluginIM()
        : CCnUiClientPluginBase( EIMPSConnClientIM )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiClientPluginIM::ConstructL( CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    CCnUiClientPluginBase::ConstructL( aNWSessionSlotID );
    iSharedData = CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL( NULL, KBrandingUid );

    iRAUsed = 1 == ReadResourceIntValueL( RSC_CHAT_VARIATION_IMPSCU_REACTIVE_AUTHORIZATION );
    iAliasUsed = 1 == ReadResourceIntValueL( RSC_CHAT_VARIATION_USE_ALIAS_ATTRIBUTE );
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::CurrentConnectionModeSettingL()
// From MCnUiClientPlugin
// -----------------------------------------------------------------------------
//
TCnUiConnectionMode CCnUiClientPluginIM::CurrentConnectionModeSettingL()
    {
    MIMPSSharedData* sharedData = CIMPSSharedDataFactory::CreatePermanentKeyHandlerL(
                                      NULL,
                                      KIMPSServSettUid );
    TInt loginTypeSetting( EWVSettingsChatLoginManual );

    // ignore return value since loginTypeSettings is unchanged from default if error occurs
    sharedData->GetIntKey( EIMPSSharedKeysIMLogin, loginTypeSetting );

    delete sharedData;

    TCnUiConnectionMode cMode( ECnUiCMAutomatic );
    if ( ( loginTypeSetting == EWVSettingsChatLoginManual ) ||
         ( loginTypeSetting == EWVSettingsChatLoginApplicationLaunch ) )
        {
        cMode = ECnUiCMManual;
        }

    return cMode;
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::ReWakeAutomaticConnectionModeL()
// From MCnUiClientPlugin
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::ReWakeAutomaticConnectionModeL()
    {
    MIMPSSharedData* sharedData = CIMPSSharedDataFactory::CreatePermanentKeyHandlerL(
                                      NULL,
                                      KIMPSServSettUid );

    TInt loginTypeSetting( EWVSettingsChatLoginManual );

    // ignore return value since loginTypeSettings is unchanged from default if error occurs
    sharedData->GetIntKey( EIMPSSharedKeysIMLogin, loginTypeSetting );
    delete sharedData;


    if ( ( loginTypeSetting == EWVSettingsChatLoginAutoAlways ) ||
         ( loginTypeSetting == EWVSettingsChatLoginAutoInHomeNW ) )
        {
        //current mode setting is automatic
        //==> re-wake with same mode
        MIMPSSharedData* loginTypeNotifier = CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL(
                                                 NULL,
                                                 KIMPSServSettNotifyUid );
        // just set the key
        TInt err = loginTypeNotifier->SetIntKey( EIMPSSharedKeysServSettLoginTypeChangedIM, 0 );
        delete loginTypeNotifier;
        if ( err )
            {
            User::Leave( err );
            }
        }
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::ConnectionOpenInitL()
// From MCnUiClientPlugin - connection open related methods
// Overloaded from CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::ConnectionOpenInitL( TBool aSapConnectionOpen,
                                               const CIMPSSAPSettings& aSap )
    {
    //let first the base class to do its things like disable synchronization
    //of unknown lists
    CCnUiClientPluginBase::ConnectionOpenInitL( aSapConnectionOpen, aSap );

    // Get presence authorization level value
    iPresenceAuth = 0;  // use 0 as default value
    aSap.GetOpaqueInt( KIMPresenceAuth, iPresenceAuth );

    // Get and cache alias
    TPtrC aliasPtr( KNullDesC );
    if ( aSap.GetOpaqueDesC16( KIMOwnAlias, aliasPtr ) == KErrNone )
        {
        HBufC* userId = GetSharedDataL( EIMPSSharedKeysIMUserId );
        CleanupStack::PushL( userId );
        if ( userId->Length() != 0 )
            {
            TInt index = KErrNotFound;
            TInt length = 0;
            LocateAliasL( aliasPtr, *userId, index, length );
            if ( index != KErrNotFound )
                {
                // found correct alias
                iAlias = aliasPtr.Mid( index, length ).AllocL();
                }
            }
        CleanupStack::PopAndDestroy( userId );
        }

    // default value
    if ( !iAlias )
        {
        iAlias = KNullDesC().AllocL();
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::DoVerifySapCapabilities()
// From CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
TBool CCnUiClientPluginIM::DoVerifySapCapabilities( TPEngWVCspServicesTree2 aCurrentNwServices )
    {
    // For IM + Presence use WV server must support all of
    // PEC engine needed main features + IM Specific ones...
    // Logical and is used
    //==> if any of these is EFalse, result will be EFalse

    TBool capabOK = ETrue;

    capabOK = aCurrentNwServices.iPresenceFeat.FeatureSupported();
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncGCLI );
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncMCLS );

    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncGETPR );
    capabOK &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncUPDPR );
    capabOK &= aCurrentNwServices.iPresenceFeat.FunctionSupported( KPEngWVPresDelivFunction );


    // IM feature capability check
    capabOK &= aCurrentNwServices.iIMFeat.FeatureSupported();
    capabOK &= aCurrentNwServices.iIMFeat.SubFunctionSupported( KPEngIMSubFuncNEWM );

    iAttribListsSupported = aCurrentNwServices.iPresenceFeat.FeatureSupported();
    iAttribListsSupported &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncCALI );
    iAttribListsSupported &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncDALI );
    iAttribListsSupported &= aCurrentNwServices.iPresenceFeat.SubFunctionSupported( KPEngWVSubFuncGALS );

    return capabOK;
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::DoClientKnownCntListsLC()
// Template method from CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
CDesCArray* CCnUiClientPluginIM::DoClientKnownCntListsLC()
    {
    //no way to know the number of lists so putting 1 as granularity
    CDesCArrayFlat* lists = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( lists );

    CPEngContactListStore2* contactListStore =
        CPEngContactListStore2::NewLC( *iNWSessionSlotID );

    // get all contact lists
    const MDesCArray& array = contactListStore->AllContactListsL();

    for ( TInt i = 0; i < array.MdcaCount(); i++ )
        {
        TPtrC list = array.MdcaPoint( i );

        // now adding all list, except watcher list
        // (also other old PEC UI -lists)
        if ( 0 != list.CompareF( KPEngWatcherList ) )
            {
            // this seems to be IM's list
            lists->AppendL( list );
            }
        }
    CleanupStack::PopAndDestroy( contactListStore );

    return lists;
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::DoStartExplicitPostLoginSyncL()
// From CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::DoStartExplicitPostLoginSyncL()
    {
    // first do the base synchronization
    if ( CurrentConnectionModeSettingL() == ECnUiCMAutomatic )
        {
        // When AO is active we need to basesync lists so that we can
        // update our attribute lists.
        User::LeaveIfError( iPEngCntListPublisher->BaseSynchronizeContactLists( *this ) );
        SetStepProcessingActive();
        iStepId = ECntListPublish;
        }
    else // Manual login mode
        {
        // we can directly publish our attributes.
        iStepId = EAttribListPublish;
        IssueIMAttributeUpdateAndPublishIfNeededL( iPresenceAuth );
        }
    }

// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::DoHandleExplicitPostLoginSyncStepCompleteL()
// From CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::DoHandleExplicitPostLoginSyncStepCompleteL( TStepId aStepId,
                                                                      TInt& aStepStatus )
    {

    if ( aStepId == EBaseCntListSynch )
        {
        // We end up here only if AO and BG handling are both active.
        // We don't want to synchronize lists here so we skip that step.
        aStepId = ECntListPublish;
        }

    if ( ( aStepId == ECntListPublish ) && !iAttribListsSupported )
        {
        // if attribute lists are not suppored we can skip the next
        // step so we put EAttribListPublish as stepid
        aStepId = EAttribListPublish;
        }

    switch ( aStepId )
        {
        case EBaseCntListSynch:
            {
            if ( aStepStatus == KErrNone )
                {
                //doing explicit synchronization
                MDesCArray* knownLists = DoClientKnownCntListsLC();

                //and synchronize known IM lists explicitly with network server
                //If list synchronization is actually done, then also attribute
                //lists & attributes are synchronized. However, in some cases
                //list synchronization isn't done and in those cases IM attributes
                //must be pushed explicitly

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
            //initialize the local attribute lists & attributes as needed
            MDesCArray* knownLists = DoClientKnownCntListsLC();

            if ( iRAUsed )
                {
                // reactive authorization,
                // delete all the attribute lists
                // including default attribute list
                SetAuthorizationToNoneL( *knownLists );
                //initialize the local attribute lists & attributes as needed
                iPEngAttributeListStore->DeleteDefaultAttributeListL();
                User::LeaveIfError( iPEngAttributeListPublisher->PublishAttributeLists( *this ) );
                SetStepProcessingActive();
                iStepId = EAttribListPublish;
                }
            else if ( ( aStepStatus == KErrNone ) ||
                      ( aStepStatus == KPEngNwErrPartiallySuccessful ) )
                {
                UpdateIMAttributeListsL( iPresenceAuth, *knownLists );

                //Contact lists are now successfully synchronized
                aStepStatus = KErrNone;

                User::LeaveIfError( iPEngAttributeListPublisher->PublishAttributeLists( *this ) );
                SetStepProcessingActive();
                iStepId = EAttribListPublish;
                }

            CleanupStack::PopAndDestroy();  //knownLists
            break;
            }

        case EAttribListPublish:
            {
            if ( ( aStepStatus == KErrNone ) ||
                 ( aStepStatus == KPEngNwErrPartiallySuccessful ) )
                {
                //Attribute lists are now successfully synchronized
                aStepStatus = KErrNone;
                IssueIMAttributeUpdateAndPublishIfNeededL( iPresenceAuth );
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
// CCnUiClientPluginIM::DoStartPreLogoutPublishL()
// From CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::DoStartPreLogoutPublishL()
    {
    // nothing to do here as server takes care of OnlineStatus updating
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::DoHandlePreLogoutPublishStepCompleteL()
// From CCnUiClientPluginBase
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::DoHandlePreLogoutPublishStepCompleteL( TStepId /*aStepId*/,
                                                                 TInt& aStepStatus )
    {
    //only attribute publish step is issued
    //ignore errors coming from it
    aStepStatus = KErrNone;
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::IMAttributePublishLevelL()
// Private helper
// -----------------------------------------------------------------------------
//
TInt CCnUiClientPluginIM::IMAttributePublishLevelL()
    {

    MIMPSSharedData* sharedData = CIMPSSharedDataFactory::CreatePermanentKeyHandlerL(
                                      NULL,
                                      KIMPSChatClientKeyUid );

    TInt value( 0 );
    TInt err = sharedData->GetIntKey( EIMPSSharedKeysIMPresenceAuthSettingKey, value );

    if ( err == KErrNotFound )
        {
        // we can ignore not finding the key, since then the correct value is 0
        err = KErrNone;
        }

    delete sharedData;

    // leave on other errors
    User::LeaveIfError( err );

    return value;
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::UpdateIMAttributeListsL()
// Private helper
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::UpdateIMAttributeListsL( TInt aPublishLevel, MDesCArray& aContactLists )
    {
    if ( aPublishLevel == EIMAuthValueAll )
        {
        SetAuthorizationToAllL( aContactLists );
        }
    else if ( aPublishLevel == EIMAuthValueFriends )
        {
        SetAuthorizationToFriendsL( aContactLists );
        }
    else
        {
        SetAuthorizationToNoneL( aContactLists );
        }
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::DefaultAttributeListLC()
// Private helper
// -----------------------------------------------------------------------------
//
MPEngAttributeList2* CCnUiClientPluginIM::DefaultAttributeListLC(
    CPEngAttributeListStore2& aAttributeListFactory )
    {
    MPEngAttributeList2* list = NULL;

    //Loading error can be safely ígnored.
    //If network list isn't available / can't be constructed
    //(reported with leave) ==> blank list needs to be generated
    TInt safelyIgnored;
    TRAP( safelyIgnored, list = aAttributeListFactory.GetDefaultAttributeListL(
                                    EPEngNetworkAttributeLists ) );

    if ( !list ) // If network list is not available, then create it.
        {
        list = aAttributeListFactory.CreateEmptyAttributeListL();
        }

    CleanupClosePushL( *list );
    return list;
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::ReadIMAttributesL()
// Private helper
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::ReadIMAttributesL(
    RPointerArray<MPEngPresenceAttrModel2>& aLockedOwnAttributes )
    {
    RResourceFile resFile;
    OpenResourceFileLC( resFile ); // Two items in cleanupstack.

    // read the data to a buffer
    TInt plainResourceId = 0x00000fff &  // Remove offset from id
                           RSC_CHAT_VARIATION_IMPSCU_LOGIN_ATTRIBUTES;
    HBufC8* rawDataBuf = resFile.AllocReadLC( plainResourceId );

    // and create resource reader for it
    TResourceReader reader;
    reader.SetBuffer( rawDataBuf );

    // read attributes
    TInt attributeCount( reader.ReadInt16() );
    for ( TInt i( 0 ); i < attributeCount; ++i )
        {
        TInt attribute( reader.ReadInt16() );
        TInt qualifier( reader.ReadInt16() );
        TInt value( reader.ReadInt16() );
        TBool valueUsed( ETrue );

        MPEngPresenceAttrModel2* attr = NULL;
        TUint32 presenceAttr( 0 );
        TInt attrData( 0 );
        TInt attrField( 0 );
        TInt attrGroup( KPEngDefaultAttrValueGroup );

        switch ( attribute )
            {
            case EIMOnlineAttr:
                {
                presenceAttr = KUidPrAttrOnlineStatus;
                switch ( value )
                    {
                    case EIMOnlineNoChange:
                        {
                        valueUsed = EFalse;
                        break;
                        }
                    default:
                        {
                        User::Panic( KPanicAttribute, KErrArgument );
                        }
                    }
                break;
                }
            case EIMUserAvailabilityAttr:
                {
                presenceAttr = KUidPrAttrUserAvailability;
                attrField = EPEngUsrAvailability;
                switch ( value )
                    {
                    case EIMUserAvailabilityNoChange:
                        {
                        valueUsed = EFalse;
                        break;
                        }
                    case EIMUserAvailabilityNotAvailable:
                        {
                        attrData = EPEngUsrAvailabilityOffline;
                        break;
                        }
                    case EIMUserAvailabilityDiscreet:
                        {
                        attrData = EPEngUsrAvailabilityDiscreet;
                        break;
                        }
                    case EIMUserAvailabilityAvailable:
                        {
                        attrData = EPEngUsrAvailabilityOnline;
                        break;
                        }
                    default:
                        {
                        User::Panic( KPanicAttribute, KErrArgument );
                        }
                    }
                break;
                }
            case EIMCommCapAttr:
                {
                presenceAttr = KUidPrAttrCommCap;
                attrField = EPEngCommCapStatus;
                attrGroup = EPEngCommCapIMClient;
                switch ( value )
                    {
                    case EIMCommCapNoChange:
                        {
                        valueUsed = EFalse;
                        break;
                        }
                    case EIMCommCapClosed:
                        {
                        attrData = EPEngCommCapStatusClosed;
                        break;
                        }
                    case EIMCommCapOpen:
                        {
                        attrData = EPEngCommCapStatusOpen;
                        break;
                        }
                    default:
                        {
                        User::Panic( KPanicAttribute, KErrArgument );
                        }
                    }
                break;
                }
            case EIMClientTypeAttr:
                {
                presenceAttr = KUidPrAttrClientInfo;
                attrField = EPEngCliInfDeviceType ;
                valueUsed = EFalse;
                break;
                }
            default:
                {
                User::Panic( KPanicAttribute, KErrArgument );
                }
            }

        TInt err( iPEngAttributeStore->GetAndLockOwnAttribute( presenceAttr, attr ) );
        if ( err == KErrNone )
            {
            CleanupClosePushL( *attr );
            // set value
            if ( valueUsed )
                {
                attr->SetDataIntL( attrData, attrField, attrGroup );
                }

            // set qualifier
            attr->SetQualifier( qualifier == EIMQualifierTrue );

            aLockedOwnAttributes.AppendL( attr );
            CleanupStack::Pop(); // attr
            }
        else if ( err != KErrLocked )
            {
            // if the model was locked, we want to continue with the other attributes
            User::Leave( err );
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // rawDataBuf, resFile
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::IssueIMAttributeUpdateAndPublishIfNeededL()
// Private helper
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::IssueIMAttributeUpdateAndPublishIfNeededL( TInt aPublishLevel )
    {
    if ( ( aPublishLevel != EIMAuthValueNobody ) || iRAUsed )
        {
        RPointerArray<MPEngPresenceAttrModel2> models;
        CleanupStack::PushL( TCleanupItem( DestroyCloseModelArray, &models ) );

        // read own attributes to be published from resource
        ReadIMAttributesL( models );

        // Set the statustext
        MPEngPresenceAttrModel2* statusText = NULL;
        TInt err( iPEngAttributeStore->GetAndLockOwnAttribute( KUidPrAttrStatusText,
                                                               statusText ) );
        if ( err == KErrNone )
            {
            CleanupClosePushL( *statusText );
            statusText->SetQualifier( ETrue );
            HBufC* statusMessage = GetFirstStatusMessageL();
            CleanupStack::PushL( statusMessage );
            statusText->SetDataDesC16L( *statusMessage, EPEngStatusText );
            CleanupStack::PopAndDestroy( statusMessage );
            models.AppendL( statusText );
            CleanupStack::Pop(); // statusText
            }
        else if ( err != KErrLocked )
            {
            // if the model was locked, we want to continue with the other attributes
            User::Leave( err );
            }

        if ( iAliasUsed )
            {
            MPEngPresenceAttrModel2* alias = NULL;
            err = iPEngAttributeStore->GetAndLockOwnAttribute( KUidPrAttrAlias,
                                                               alias );
            if ( err == KErrNone )
                {
                CleanupClosePushL( *alias );
                alias->SetQualifier( ETrue );
                if ( iAlias->Length() == 0 )
                    {
                    CleanupStack::PopAndDestroy(); // alias
                    }
                else
                    {
                    alias->SetDataDesC16L( *iAlias, EPEngStatusText );
                    models.AppendL( alias );
                    CleanupStack::Pop(); // alias
                    }
                }
            else if ( err != KErrLocked )
                {
                // if the model was locked, we want to continue with the other attributes
                User::Leave( err );
                }
            }

        // publish and unlock
        User::LeaveIfError( iPEngAttributePublisher->PublishAndUnLockOwnAttributes(
                                models, *this ) );
        CleanupStack::Pop(); // models

        SetStepProcessingActive();
        iStepId = EAttributePublish;
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::LoginCancelledL()
//
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::LoginCancelledL()
    {
    MPEngPresenceAttrModel2* commCap = NULL;
    TInt err = iPEngAttributeStore->GetAndLockOwnAttribute( KUidPrAttrCommCap, commCap );
    if ( err )
        {
        // we can just return, since if we can't edit commcap, there's nothing
        // we can do about it
        return;
        }
    CleanupClosePushL( *commCap );
    commCap->SetDataIntL( EPEngCommCapStatusClosed, EPEngCommCapStatus, EPEngCommCapIMClient );
    commCap->SetQualifier( ETrue );
    iPEngAttributeStore->StoreOwnAttribute( *commCap );
    CleanupStack::PopAndDestroy(); // commCap
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::OpenResourceFileLC()
// !!!Notice!!!. Two variables in cleanupstack after call of this method.
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::OpenResourceFileLC( RResourceFile& aResourceFile )
    {
    TFileName resourceFileName;
    iRFs.Close();
    User::LeaveIfError( iRFs.Connect() );
    CnUiResourceFileName::NearestForCurrentLanguage( iRFs, resourceFileName  );
    aResourceFile.OpenL( iRFs, resourceFileName );
    CleanupClosePushL( aResourceFile );
    aResourceFile.ConfirmSignatureL();
    }



// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::OpenVariationResourceFileLC()
// !!!Notice!!!. Two variables in cleanupstack after call of this method.
// -----------------------------------------------------------------------------
//

//Ease of IM Branding
void CCnUiClientPluginIM::OpenVariationResourceFileLC( RResourceFile& aResourceFile )
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
// CCnUiClientPluginIM::ReadResourceIntValueL()
// -----------------------------------------------------------------------------
//
TInt CCnUiClientPluginIM::ReadResourceIntValueL( TInt aResourceId )
    {

    TInt val( 0 );
    TInt err ( KErrNone );

    CRepository* rep = 0;

    TRAP( err,  rep = CRepository::NewL( KCRUidIMNG ) );

    if ( err == KErrNone )
        {
        TInt key = aResourceId + KIMCUStartVariationID;

        err = rep->Get( key, val );

        delete rep;
        }

    if ( err != KErrNone )
        {

        RResourceFile resFile;
        OpenVariationResourceFileLC( resFile ); // Two items in cleanup stack.

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

// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::DestroyCloseModelArray()
//
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::DestroyCloseModelArray( TAny* aObject )
    {
    reinterpret_cast<RPointerArray<MPEngPresenceAttrModel2>*>( aObject )->ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::SetAuthorizationToAllL
// Sets presence authorization mode to all
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::SetAuthorizationToAllL( MDesCArray& aContactLists )
    {
    // Add CommCap/IM and OnlineStatus to default attribute list
    MPEngAttributeList2* attrList = DefaultAttributeListLC( *iPEngAttributeListStore );
    attrList->AddPresenceAttributeL( KUidPrAttrCommCap );
    attrList->AddPresenceAttributeL( KUidPrAttrOnlineStatus );
    attrList->AddPresenceAttributeL( KUidPrAttrUserAvailability );
    attrList->AddPresenceAttributeL( KUidPrAttrStatusText );

    if ( iAliasUsed )
        {
        attrList->AddPresenceAttributeL( KUidPrAttrAlias );
        }

    iPEngAttributeListStore->SetAsDefaultAttributeListL( *attrList );
    CleanupStack::PopAndDestroy( attrList );

    // remove possible authorizations from all contact lists

    // Deattach attribute-lists from all our contact-lists
    DetachAttributeListL( aContactLists );
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::SetAuthorizationToNoneL
// Sets presence authorization mode to none
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::SetAuthorizationToNoneL( MDesCArray& aContactLists )
    {

    // Remove CommCap/IM from default attribute list
    MPEngAttributeList2* attrList = DefaultAttributeListLC( *iPEngAttributeListStore );
    attrList->RemoveAllAttributes();
    iPEngAttributeListStore->SetAsDefaultAttributeListL( *attrList );
    CleanupStack::PopAndDestroy( attrList );

    // remove possible authorizations from all contact lists

    // Deattach attribute-lists from all our contact-lists
    DetachAttributeListL( aContactLists );
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::SetAuthorizationToFriendsL
// Sets presence authorization mode to friends
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::SetAuthorizationToFriendsL(
    MDesCArray& aContactLists )
    {
    // Remove CommCap/IM from default attribute list
    MPEngAttributeList2* defaultAttrList =
        DefaultAttributeListLC( *iPEngAttributeListStore );
    defaultAttrList->RemoveAllAttributes();
    iPEngAttributeListStore->SetAsDefaultAttributeListL( *defaultAttrList );
    CleanupStack::PopAndDestroy( defaultAttrList );

    // Create attribute-list with CommCap-attribute
    MPEngAttributeList2* attrList =
        iPEngAttributeListStore->CreateEmptyAttributeListL();
    CleanupClosePushL( *attrList );
    attrList->AddPresenceAttributeL( KUidPrAttrCommCap );
    attrList->AddPresenceAttributeL( KUidPrAttrOnlineStatus );
    attrList->AddPresenceAttributeL( KUidPrAttrUserAvailability );
    attrList->AddPresenceAttributeL( KUidPrAttrStatusText );

    if ( iAliasUsed )
        {
        attrList->AddPresenceAttributeL( KUidPrAttrAlias );
        }

    // Attach created attribute-list to all contact-lists we have
    AttachAttributeListL( aContactLists, *attrList );

    CleanupStack::PopAndDestroy( attrList ); // attrList
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::AttachAttributeListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::AttachAttributeListL( MDesCArray& aContactLists,
                                                MPEngAttributeList2& aAttributeList )
    {
    TInt count( aContactLists.MdcaCount() );

    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC listId( aContactLists.MdcaPoint( i ) );

        // Attach created attribute-list to our given contact-list
        iPEngAttributeListStore->
        AttachAttributeListToContactListL( listId, aAttributeList );
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::DetachAttributeListL
// Cancels pending requests to network side.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCnUiClientPluginIM::DetachAttributeListL(  MDesCArray& aContactLists )
    {
    TInt count( aContactLists.MdcaCount() );

    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC listId( aContactLists.MdcaPoint( i ) );

        iPEngAttributeListStore->
        DeleteAttributeListFromContactListL( listId );
        }
    }

// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::GetFirstStatusMessageL
// Get first status message for online status.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCnUiClientPluginIM::GetFirstStatusMessageL() const
    {
    RBuf buffer;
    HBufC* message = NULL;
    buffer.CreateL( RProperty::KMaxPropertySize );
    CleanupClosePushL( buffer );

    MIMPSSharedData* sharedData = CIMPSSharedDataFactory::CreatePermanentKeyHandlerL(
                                      NULL,
                                      KIMPSChatClientKeyUid );
    // We can ignore errors
    sharedData->GetStringKey( EIMPSSharedKeysIMStatusMsgOnlineKey, buffer );
    delete sharedData;

    RBuf header;
    CleanupClosePushL( header );
    TInt offset( 1 ); // First character is for header length.
    TInt headerLength( 0 );
    TInt err( KErrNone );
    if ( buffer.Length() )
        {
        TLex lexer( buffer.Left( 1 ) );
        err = lexer.Val( headerLength );
        }
    if ( !err && headerLength )
        {
        header.CreateL( headerLength );
        TPtrC ptr( buffer.Mid( offset ) );
        if ( ptr.Length() > headerLength )
            {
            header.Copy( ptr.Left( headerLength ) );
            header.Trim();
            TLex lexer( header );
            offset += headerLength;
            TInt messageLength( 0 );
            TInt err( lexer.Val( messageLength ) );
            if ( err == KErrNone )
                {
                ptr.Set( buffer.Mid( offset ) );
                if ( ptr.Length() >= messageLength )
                    {
                    // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
                    // this method cannot leave after this line
                    message = ptr.Left( messageLength ).AllocL(); // CSI: 35 # See above
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // header, buffer
    if ( !message )
        {
        message = KNullDesC().AllocL();
        }
    return message;
    }

// -----------------------------------------------------------------------------
// CCnUiClientPluginIM::GetAliasL
// Get first status message for online status.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCnUiClientPluginIM::GetSharedDataL( TIMPSSharedKeys aKey ) const
    {
    RBuf buffer;
    HBufC* message = NULL;
    buffer.CreateL( RProperty::KMaxPropertySize );
    CleanupClosePushL( buffer );

    MIMPSSharedData* sharedData = CIMPSSharedDataFactory::CreatePermanentKeyHandlerL(
                                      NULL,
                                      KIMPSChatClientKeyUid );
    // We can ignore errors
    sharedData->GetStringKey( aKey, buffer );
    delete sharedData;

    // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
    // This method cannot leave after this line
    message = buffer.AllocL(); // CSI: 35 # See above
    CleanupStack::PopAndDestroy(); // buffer

    if ( !message )
        {
        message = KNullDesC().AllocL();
        }
    return message;
    }

// ---------------------------------------------------------
// CCASettingsManager::LocateAliasL()
// ---------------------------------------------------------
//
void CCnUiClientPluginIM::LocateAliasL( const TDesC& aAliasTable,
                                        const TDesC& aUserId,
                                        TInt& aIndex,
                                        TInt& aLength )
    {
    aIndex = KErrNotFound;
    aLength = 0;
    if ( aAliasTable.Length() == 0 || aUserId.Length() == 0 )
        {
        // nothing to search!
        return;
        }

    HBufC* findPattern = HBufC::NewLC( aUserId.Length() +
                                       KSeparatorSize );
    TPtr find( findPattern->Des() );
    find.Copy( aUserId );
    TInt len = find.Length();
    find.Append( TChar( KAliasItemSeparator ) );
    TInt len2 = find.Length();

    aIndex = aAliasTable.Find( find );
    if ( aIndex == KErrNotFound )
        {
        // not found
        CleanupStack::PopAndDestroy( findPattern );
        return;
        }

    // found it, get length
    aIndex += find.Length();
    TPtrC rest( aAliasTable.Mid( aIndex ) );
    aLength = rest.Locate( TChar( KAliasTableSeparator ) );
    if ( aLength == KErrNotFound )
        {
        // this was last item
        aLength = rest.Length();
        }
    CleanupStack::PopAndDestroy( findPattern );
    return;
    }

//  End of File
