/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
* data accessor class for imps fields. 
*
*/

// INCLUDE FILES
#include    <e32std.h>
#include    "impsfields.h"
#include    "impsconst.h"
#include    "impskey.h"
#include    "impserrors.h"
#include    "impsutils.h"
#include    "impstdataaccessor.h"
#include    "impsdatautils.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TImpsDataAccessor::TImpsDataAccessor
// ---------------------------------------------------------
//
EXPORT_C TImpsDataAccessor::TImpsDataAccessor()
    {
    iImpsFields = NULL;
    }
// ---------------------------------------------------------
// TImpsDataAccessor::TImpsDataAccessor
// ---------------------------------------------------------
//
EXPORT_C TImpsDataAccessor::TImpsDataAccessor(CImpsFields* aImpsFields)
    {
    iImpsFields = aImpsFields;
    }
// ---------------------------------------------------------
// TImpsDataAccessor::SetImpsFields
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataAccessor::SetImpsFields( CImpsFields* aImpsFields )
    {
    iImpsFields = aImpsFields;
    }

// ---------------------------------------------------------
// TImpsDataAccessor::GetImpsFields
// ---------------------------------------------------------
//
EXPORT_C CImpsFields* TImpsDataAccessor::GetImpsFields( ) const
    {
    return iImpsFields;
    }

// ---------------------------------------------------------
// TImpsDataAccessor::~TImpsDataAccessor()
// ---------------------------------------------------------
//
EXPORT_C TImpsDataAccessor::~TImpsDataAccessor()
    {
    }

// ---------------------------------------------------------
// TImpsDataAccessor::NewKeyL
// ---------------------------------------------------------
//
EXPORT_C MImpsKey* TImpsDataAccessor::NewKeyL()
    {
    CImpsKey* key = CImpsKey::NewL();
    return key;
    }

// ---------------------------------------------------------
// TImpsDataAccessor::StoreDescL
// mutator for symbian descriptor value
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataAccessor::StoreDescL( MImpsKey* aKey, TPtrC aValue )
    {
    iData.iType = EStringType;
    iData.iValue.Set( aValue );

    DoStoreL( static_cast<CImpsKey*>( aKey ) );
    }

// ---------------------------------------------------------
// TImpsDataAccessor::StoreDescL8
// mutator for symbian 8-bit descriptor value
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataAccessor::StoreDesc8L( MImpsKey* aKey, TPtrC8 aValue)
    {
    iData.iType = EString8Type;
    iData.iValue8.Set( aValue );

    DoStoreL( static_cast<CImpsKey*>( aKey ) );
    }

// ---------------------------------------------------------
// TImpsDataAccessor::StoreInteger
// mutator for symbian integer value
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataAccessor::StoreIntegerL( MImpsKey* aKey, TInt aValue )
    {
    iData.iType = EIntegerType;
    iData.iValueInt = aValue;

    DoStoreL( static_cast<CImpsKey*>( aKey ) );
    }

// ---------------------------------------------------------
// TImpsDataAccessor::StoreBooleanL
// mutator for symbian boolean value
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataAccessor::StoreBooleanL( MImpsKey* aKey, TBool aValue )
    {
    iData.iType = EBooleanType;
    iData.iValueBool = aValue;

    DoStoreL( static_cast<CImpsKey*>( aKey ) );
    }

// ---------------------------------------------------------
// TImpsDataAccessor::StoreEmptyL
// mutator for symbian empty value
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataAccessor::StoreEmptyL( MImpsKey* aKey)
    {
    iData.iType = EEmptyType;

    DoStoreL( static_cast<CImpsKey*>( aKey ) );
    }
// ---------------------------------------------------------
// TImpsDataAccessor::StoreExtL
// mutator for extension attribute
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataAccessor::StoreExtL( MImpsKey* /*aKey*/, 
                                            TPtrC8 /*aName*/, 
                                            TPtrC8 /*aValue*/)
    {
    }
//----------------------------------------------------------
// TImpsDataAccessor::RestoreDescL
// accessor for symbian descriptor value
// ---------------------------------------------------------
//

EXPORT_C TBool TImpsDataAccessor::RestoreDescL( MImpsKey* aKey, TDesC*& aDes)
    {
    iData.iType = EStringType;

    TBool ret = DoRestoreL( static_cast<CImpsKey*>( aKey ) );
    if( ret )
        {
        if( iData.iValuePointer )
            {
            aDes = iData.iValuePointer;
            }
        else
            {
            aDes = (TDesC*)&KNullDesC;
            }

        }
    return ret;
    }

//----------------------------------------------------------
// TImpsDataAccessor::RestoreDesc8L
// accessor for symbian 8-bit descriptor value
// ---------------------------------------------------------
//

EXPORT_C TBool TImpsDataAccessor::RestoreDesc8L( MImpsKey* aKey, 
                                                 TDesC8*& aDes )
    {
    iData.iType = EString8Type;

    TBool ret = DoRestoreL( static_cast<CImpsKey*>( aKey ) );
    if( ret )
        {
        if( iData.iValuePointer8 )
            {
            aDes = iData.iValuePointer8;
            }
        else
            {
            aDes = (TDesC8*)&KNullDesC8;
            }

        }
    return ret;
    }

//----------------------------------------------------------
// TImpsDataAccessor::RestoreIntegerL
// accessor for integer value
// (covers also boolean and enum types
// ---------------------------------------------------------
//

EXPORT_C TBool TImpsDataAccessor::RestoreIntegerL( MImpsKey* aKey, TInt& aInt )
    {
    iData.iType = EIntegerType;

    TBool ret = DoRestoreL( static_cast<CImpsKey*>( aKey ) );
    if( ret )
        aInt = iData.iValueInt;
    return ret;
    }

//----------------------------------------------------------
// TImpsDataAccessor::RestoreBooleanL
// accessor for integer value
// (covers also boolean and enum types
// ---------------------------------------------------------
//

EXPORT_C TBool TImpsDataAccessor::RestoreBooleanL( MImpsKey* aKey, 
                                                   TBool& aBool )
    {
    iData.iType = EBooleanType;

    TBool ret = DoRestoreL( static_cast<CImpsKey*>( aKey ) );
    if( ret )
        aBool = iData.iValueBool;
    return ret;
    }

//----------------------------------------------------------
// TImpsDataAccessor::RestoreEmptyL
// accessor for integer value
// (covers also boolean and enum types
// ---------------------------------------------------------
//
EXPORT_C TBool TImpsDataAccessor::RestoreEmptyL( MImpsKey* aKey )
    {
    iData.iType = EEmptyType;

    return DoRestoreL( static_cast<CImpsKey*>( aKey ) );
    }

//----------------------------------------------------------
// TImpsDataAccessor::RestoreDesc8L
// accessor for symbian 8-bit descriptor value
// ---------------------------------------------------------
//
EXPORT_C TBool TImpsDataAccessor::RestoreExtL( MImpsKey* /* aKey */,  
                                               TDesC8*& /*aName*/, 
                                               TDesC8*& /*aValue*/)
    {
    return EFalse;
    }

// ---------------------------------------------------------
// TImpsDataAccessor::KeyTypeL
// method returns type of related to given key
// ---------------------------------------------------------
//
EXPORT_C TImpsDataType TImpsDataAccessor::KeyTypeL( MImpsKey* aKey )
    {

    // Just search the last element type ( or ask from data storage?)

    TInt eCount = aKey->Count();
    if ( !eCount )
        {
        User::Leave( KErrArgument );
        }

    TImpsDataType dataType(EImpsDataTypeNotSupported);
    TImpsKeyType keyType;
    TInt myVal = aKey->GetElementL( eCount - 1, keyType );
    TInt myElem = GET_ENUM( myVal );
    if( keyType == EImpsKeyTypePre )
        {
        switch ( myElem )
            {
            case EImpsKeyPRPresenceSubList:
            case EImpsKeyPRPresenceValue:
            case EImpsKeyContentType:
            // CSP 1.2
            case EImpsKeyPRText:

                dataType = EImpsDataTypeDesc;
                break;

            // added
            case EImpsKeyPRDevManufacturer:
            case EImpsKeyPRClientProducer:
            case EImpsKeyPRModel:
            case EImpsKeyPRClientVersion:
            case EImpsKeyPRLanguage:
            case EImpsKeyPRZone:
            case EImpsKeyPRLongitude:
            case EImpsKeyPRLatitude:
            case EImpsKeyPRCountry:  //ENUM
            case EImpsKeyPRCity:
            case EImpsKeyPRStreet:
            case EImpsKeyPRCrossing1:
            case EImpsKeyPRCrossing2:
            case EImpsKeyPRBuilding:
            case EImpsKeyPRNamedArea:
            case EImpsKeyPRContact:
            case EImpsKeyPRNote:
            case EImpsKeyPRPrefC: //enum
            case EImpsKeyPRCaddr:// enum
            case EImpsKeyPRCstatus: //enum
            case EImpsKeyPRCname:
            case EImpsKeyPRDirectContent:
            case EImpsKeyPRReferredContent:
            case EImpsKeyPRContainedvCard:
            case EImpsKeyPRReferredvCard:
            //CSP 1.2
            case EImpsKeyPRLink:
                dataType = EImpsDataTypeDesc8;
                break;

            case EImpsKeyPRStatus:
            case EImpsKeyPRCap:
                //added
            case EImpsKeyPRClientType:
            case EImpsKeyPRAltitude:
            case EImpsKeyPRAccuracy:
            case EImpsKeyPRCpriority:
                dataType = EImpsDataTypeInt;
                break;

            case EImpsKeyPRQualifier:
                dataType = EImpsDataTypeBoolean;
                break;

            case EImpsKeyPROnlineStatus:
            case EImpsKeyPRUserAvailability:
            case EImpsKeyPRCommCap:
            case EImpsKeyPRCommC:
            // added
            case EImpsKeyPRRegistration:
            case EImpsKeyPRFreeTextLocation:
            case EImpsKeyPRPLMN:
            case EImpsKeyPRPreferredLanguage:
            case EImpsKeyPRStatusText:
            case EImpsKeyPRStatusMood:
            case EImpsKeyPRAlias:
            case EImpsKeyPRClientInfo:
            case EImpsKeyPRTimeZone:
            case EImpsKeyPRGeoLocation:
            case EImpsKeyPRAddress:
            case EImpsKeyPRPreferredContacts:
            case EImpsKeyPRAddrPref:
            case EImpsKeyPRStatusContent:
            case EImpsKeyPRContactInfo:
            case EImpsKeyPRInf_link:
            case EImpsKeyPRInfoLink:

                dataType = EImpsDataTypeNone;
                break;

            case EImpsKeyPRExt:
                dataType = EImpsDataTypeExt;
            default:
                dataType = EImpsDataTypeNotSupported;
                break;
            };
        return dataType;
        }
    // TImpsContent
    switch ( myElem )
        {
        // The following ones are enums
        case EImpsKeySessionType:    // cannot contain non-ASCII characters
        case EImpsKeyTransactionMode:
        case EImpsKeyDigestSchema:
        case EImpsKeyClientType:
        case EImpsKeyInitialDeliveryMethod:
        case EImpsKeySupportedBearer:
        case EImpsKeySupportedCIRMethod:
        case EImpsKeySearchElement:
        case EImpsKeyInviteType:
        case EImpsKeyDeliveryMethod:
        case EImpsKeySubscribeType:
        case EImpsKeyContentEncoding:
        case EImpsKeyAcceptedTransferEncoding:
        // new in CSP 1.2
        case EImpsKeyReactiveAuthState:
        case EImpsKeyWatcherStatus:
        // Integer values, these are pure integer values
        case EImpsKeyContentSize:
        case EImpsKeyAcceptedCharSet:
        case EImpsKeyAcceptedContentLength:
        case EImpsKeyMultiTrans:
        case EImpsKeyParserSize:
        case EImpsKeyUDPPort:
        case EImpsKeyTCPPort:
        case EImpsKeyServerPollMin:
        case EImpsKeyCode:
        case EImpsKeySearchLimit:
        case EImpsKeySearchID:
        case EImpsKeySearchIndex:
        case EImpsKeySearchFindings:
        case EImpsKeyKeepAliveTime:
        case EImpsKeyTimeToLive:
        case EImpsKeyValidity:
        case EImpsKeyMessageCount:
        // new in CSP 1.2
        case EImpsKeyHistoryPeriod:
        case EImpsKeyMaxWatcherList:
            dataType = EImpsDataTypeInt;
            break;

        // 16-bit unicode
        case EImpsKeySessionID:
        case EImpsKeySessionCookie:
        case EImpsKeyUserID:
        case EImpsKeyTransactionID:
        case EImpsKeyTransactionContent:
        case EImpsKeyDeliveryTime:
        case EImpsKeyPassword:
        case EImpsKeyAcceptedContentType:
        case EImpsKeyTCPAddress:
        case EImpsKeyDescription:
        case EImpsKeyGroupID:
        case EImpsKeyMessageID:
        case EImpsKeyMessageURI:
        case EImpsKeyURL:
        case EImpsKeyMSISDN:
        case EImpsKeyContactList:
        case EImpsKeyDefaultContactList:
        case EImpsKeySearchString:
        case EImpsKeyInviteID:
        case EImpsKeyInviteNote:
        case EImpsKeyResponseNote:
        case EImpsKeyPresenceSubList:
        case EImpsKeySName:
        case EImpsKeyName:
        case EImpsKeyValue:
        case EImpsKeyDateTime:
        case EImpsKeyContentType:
        case EImpsKeyDefaultLanguage:
        // new in CSP 1.2
        case EImpsKeyExtendedData:
        case EImpsKeyAPIClient:
            dataType = EImpsDataTypeDesc;
            break;

        // 8-bit data. 
        case EImpsKeyPureData:
        case EImpsKeyNonce:
        case EImpsKeyDigestBytes:
        // new in CSP 1.2
        case EImpsKeyPresenceAttributeNSName:
        case EImpsKeySessionNSName:
        case EImpsKeyTransactionNSName:
        case EImpsKeyDomain:
            dataType = EImpsDataTypeDesc8;
            break;

        // Boolean data types
        case EImpsKeyPoll:
        case EImpsKeyCompletionFlag:
        case EImpsKeyJoinGroup:
        case EImpsKeySubscribeNotification:
        case EImpsKeyCapabilityRequest:
        case EImpsKeyAnyContent:
        case EImpsKeyDefaultList:
        case EImpsKeyAcceptance:
        case EImpsKeyDeliveryReport:
        case EImpsKeyInUse:
        case EImpsKeyJoinedRequest:
        case EImpsKeyAllFunctionsRequest:
        // new in CSP 1.2
        case EImpsKeyReceiveList:
        case EImpsKeyAuto_Subscribe:
        case EImpsKeyCIR:
            dataType = EImpsDataTypeBoolean;
            break;
        // special case    
        case EImpsKeyContentData:
            dataType = EImpsDataTypeContentData;
            break;
              
        // Elements not having value
        case EImpsKeySession:
        case EImpsKeySessionDescriptor:
        case EImpsKeyTransaction:
        case EImpsKeyTransactionDescriptor:
        case EImpsKeyStatus:
        case EImpsKeyPolling_Request:
        case EImpsKeyLogin_Request:
        case EImpsKeyLogin_Response:
        case EImpsKeyService_Request:
        case EImpsKeyService_Response:
        case EImpsKeyClientCapability_Request:
        case EImpsKeyClientCapability_Response:
        case EImpsKeyLogout_Request:
        case EImpsKeyDisconnect:
        case EImpsKeyKeepAlive_Request:
        case EImpsKeyKeepAlive_Response:
        case EImpsKeyGetSPInfo_Request:
        case EImpsKeyGetSPInfo_Response:
        case EImpsKeySearch_Request:
        case EImpsKeyStopSearch_Request:
        case EImpsKeySearch_Response:
        case EImpsKeyInvite_Request:
        case EImpsKeyInvite_Response:
        case EImpsKeyInviteUser_Request:
        case EImpsKeyInviteUser_Response:
        case EImpsKeyCancelInvite_Request:
        case EImpsKeyCancelInviteUser_Request:
        case EImpsKeySubscribePresence_Request:
        case EImpsKeyUnsubscribePresence_Request:
        case EImpsKeyPresenceNotification_Request:
        case EImpsKeyGetWatcherList_Request:
        case EImpsKeyGetWatcherList_Response:
        case EImpsKeyGetPresence_Request:
        case EImpsKeyGetPresence_Response:
        case EImpsKeyPresenceAuth_Request:
        case EImpsKeyPresenceAuth_User:
        case EImpsKeyCancelAuth_Request:
        case EImpsKeyUpdatePresence_Request:
        case EImpsKeyGetList_Request:
        case EImpsKeyGetList_Response:
        case EImpsKeyCreateList_Request:
        case EImpsKeyDeleteList_Request:
        case EImpsKeyListManage_Request:
        case EImpsKeyListManage_Response:
        case EImpsKeyCreateAttributeList_Request:
        case EImpsKeyDeleteAttributeList_Request:
        case EImpsKeyGetAttributeList_Request:
        case EImpsKeyGetAttributeList_Response:
        case EImpsKeySendMessage_Request:
        case EImpsKeySendMessage_Response:
        case EImpsKeySetDeliveryMethod_Request:
        case EImpsKeyGetMessageList_Request:
        case EImpsKeyGetMessageList_Response:
        case EImpsKeyRejectMessage_Request:
        case EImpsKeyMessageNotification:
        case EImpsKeyGetMessage_Request:
        case EImpsKeyGetMessage_Response:
        case EImpsKeyNewMessage:
        case EImpsKeyMessageDelivered:
        case EImpsKeyDeliveryReport_Request:
        case EImpsKeyForwardMessage_Request:
        case EImpsKeyGetBlockedList_Request:
        case EImpsKeyGetBlockedList_Response:
        case EImpsKeyBlockEntity_Request:
        case EImpsKeyCreateGroup_Request:
        case EImpsKeyDeleteGroup_Request:
        case EImpsKeyJoinGroup_Request:
        case EImpsKeyJoinGroup_Response:
        case EImpsKeyLeaveGroup_Request:
        case EImpsKeyLeaveGroup_Response:
        case EImpsKeyGetGroupMembers_Request:
        case EImpsKeyGetGroupMembers_Response:
        case EImpsKeyAddGroupMembers_Request:
        case EImpsKeyRemoveGroupMembers_Request:
        case EImpsKeyMemberAccess_Request:
        case EImpsKeyGetGroupProps_Request:
        case EImpsKeyGetGroupProps_Response:
        case EImpsKeySetGroupProps_Request:
        case EImpsKeyRejectList_Request:
        case EImpsKeyRejectList_Response:
        case EImpsKeySubscribeGroupNotice_Request:
        case EImpsKeySubscribeGroupNotice_Response:
        case EImpsKeyGroupChangeNotice:
        case EImpsKeyCapabilityList:
        case EImpsKeyResult:
        case EImpsKeyDetailedResult:
        case EImpsKeySender:
        case EImpsKeyRecipient:
        case EImpsKeyUser:
        case EImpsKeyGroup:
        case EImpsKeyClientID:
        case EImpsKeyScreenName:
        case EImpsKeyNickName:
        case EImpsKeyURLList:
        case EImpsKeyGroupList:
        case EImpsKeyUserList:
        case EImpsKeyNickList:
        case EImpsKeyAddNickList:
        case EImpsKeyRemoveNickList:
        case EImpsKeySearchPairList:
        case EImpsKeySearchResult:
        case EImpsKeyDefaultAttributeList:
        case EImpsKeyPresence:
        case EImpsKeyMessageInfo:
        case EImpsKeyBlockList:
        case EImpsKeyGrantList:
        case EImpsKeyEntityList:
        case EImpsKeyAddList:
        case EImpsKeyRemoveList:
        case EImpsKeyContactListProperties:
        case EImpsKeyGroupProperties:
        case EImpsKeyOwnProperties:
        case EImpsKeyProperty:
        case EImpsKeyWelcomeNote:
        case EImpsKeyAdmin:
        case EImpsKeyMod:
        case EImpsKeyUsers:
        case EImpsKeyJoined:
        case EImpsKeyLeft:
        case EImpsKeyLogo:
        case EImpsKeyAllFunctions:
        case EImpsKeyFunctions:
        case EImpsKeyWVCSPFeat:
        case EImpsKeyFundamentalFeat:
        case EImpsKeyPresenceFeat:
        case EImpsKeyIMFeat:
        case EImpsKeyGroupFeat:
        case EImpsKeyServiceFunc:
        case EImpsKeySearchFunc:
        case EImpsKeyInviteFunc:
        case EImpsKeyContListFunc:
        case EImpsKeyPresenceAuthFunc:
        case EImpsKeyPresenceDeliverFunc:
        case EImpsKeyAttListFunc:
        case EImpsKeyIMSendFunc:
        case EImpsKeyIMReceiveFunc:
        case EImpsKeyIMAuthFunc:
        case EImpsKeyGroupMgmtFunc:
        case EImpsKeyGroupUseFunc:
        case EImpsKeyGroupAuthFunc:
        // new in CSP 1.2
        case EImpsKeyExtBlock:
        case EImpsKeyAgreedCapabilityList:
        case EImpsKeyExtended_Request:
        case EImpsKeyExtended_Response:
        case EImpsKeyOtherServer:
        case EImpsKeyVerifyID_Request:
        case EImpsKeyGetReactiveAuthStatus_Request:
        case EImpsKeyGetReactiveAuthStatus_Response:
        case EImpsKeyAdminMapList:
        case EImpsKeyAdminMapping:
        case EImpsKeyGetJoinedUsers_Request:
        case EImpsKeyGetJoinedUsers_Respone:
        case EImpsKeyMapping:
        case EImpsKeyUserMapList:
        case EImpsKeyUserMapping:
        case EImpsKeyVerifyIDFunc:
        case EImpsKeyIDList:
        case EImpsKeyReactiveAuthStatus:
        case EImpsKeyReactiveAuthStatusList:
        case EImpsKeyWatcher:
        case EImpsKeyVersionList:
             dataType = EImpsDataTypeNone;
             break;

        case EImpsKeyGETSPI:
        case EImpsKeySRCH:
        case EImpsKeySTSRC:
        case EImpsKeyINVIT:
        case EImpsKeyCAINV:
        case EImpsKeyGCLI:
        case EImpsKeyCCLI:
        case EImpsKeyDCLI:
        case EImpsKeyMCLS:
        case EImpsKeyGETWL:
        case EImpsKeyREACT:
        case EImpsKeyCAAUT:
        case EImpsKeyGETPR:
        case EImpsKeyUPDPR:
        case EImpsKeyCALI:
        case EImpsKeyDALI:
        case EImpsKeyGALS:
        case EImpsKeyMDELIV:
        case EImpsKeyFWMSG:
        case EImpsKeySETD:
        case EImpsKeyGETLM:
        case EImpsKeyGETM:
        case EImpsKeyREJCM:
        case EImpsKeyNOTIF:
        case EImpsKeyNEWM:
        case EImpsKeyGLBLU:
        case EImpsKeyBLENT:
        case EImpsKeyCREAG:
        case EImpsKeyDELGR:
        case EImpsKeyGETGP:
        case EImpsKeySETGP:
        case EImpsKeySUBGCN:
        case EImpsKeyGRCHN:
        case EImpsKeyGETGM:
        case EImpsKeyADDGM:
        case EImpsKeyRMVGM:
        case EImpsKeyMBRAC:
        case EImpsKeyREJEC:
        // new in CSP 1.2
        case EImpsKeyMF:
        case EImpsKeyMG:
        case EImpsKeyMM:
        case EImpsKeyGETAUT:
        case EImpsKeyGETJU:
        case EImpsKeyMP:
        case EImpsKeyVRID:
            
            dataType = EImpsDataTypeNone;
            break;

        default:
            User::Leave( KErrArgument );

        };
    return dataType;
    }

// ---------------------------------------------------------
// TImpsDataAccessor::RestoreAlternativeL
// ---------------------------------------------------------
//
EXPORT_C TBool TImpsDataAccessor::RestoreAlternativeL( const MImpsKey* aKey, 
                                                    TInt& aEnum, 
                                                    TInt& aIndex, 
                                                    TImpsKeyType& aType ) const
    {
    const CImpsKey* keyIn = static_cast<const CImpsKey*>( aKey );
    const CImpsData* data = iImpsFields->Data( );
    aEnum = 0;
    aIndex = 0;
    aType = EImpsKeyTypeIM;

    const CImpsKey* foundkey = data->CheckKey( keyIn, EPartialKeyLonger );
    if( foundkey )
        {
        // The found key can be longer than the search key
        // Since the beginning of the aKey and foundKey are the same 
        // we want the first different element from the key
        // If the lengths are the same then there is no alternative
        TInt len1 = foundkey->Count( );
        TInt len2 = aKey->Count( );
        if( len1 == len2 )
            return EFalse;
        TInt elem = foundkey->GetElementL( len2, aType );
        aIndex = GET_INDEX( elem );
        aEnum = GET_ENUM( elem );
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// TImpsDataAccessor::CheckBranchExistenceL
// ---------------------------------------------------------
//
EXPORT_C TBool TImpsDataAccessor::CheckBranchExistenceL( 
                                            const MImpsKey* aKey ) const
    {
    const CImpsKey* keyIn = static_cast<const CImpsKey*>( aKey );
    const CImpsData* data = iImpsFields->Data( );

    const CImpsKey* foundkey = data->CheckKey( keyIn, EPartialKey );
    if( foundkey )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// TImpsDataAccessor::NbrOfTransactionsL
// ---------------------------------------------------------
//
EXPORT_C TInt TImpsDataAccessor::NbrOfTransactionsL( ) const
    {
    TInt ret = 0;
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0) );
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0) );

    while( CheckBranchExistenceL( myKey ) )
        {
        ++ret;
        myKey->ReplaceLastL( CREATEKEY( EImpsKeyTransaction, ret ) );
        }
    CleanupStack::PopAndDestroy(1);
    return ret;
    }

// ---------------------------------------------------------
// TImpsDataAccessor::GetTransactionL
// ---------------------------------------------------------
//
EXPORT_C TBool TImpsDataAccessor::GetTransactionL( 
    TInt aIndex, CImpsFields* aFields ) const
    {
    if( !iImpsFields )
        return EFalse;
    if( aFields )
        {
        TBool ret = EFalse;
        CImpsData* dataDes = aFields->Data( );
        CImpsData* dataSrc = iImpsFields->Data( );

        CImpsKey* myKey = CImpsKey::NewLC();
        myKey->AddL( CREATEKEY( EImpsKeySession, 0) );
        myKey->AddL( CREATEKEY( EImpsKeyTransaction, aIndex) );

        if( CheckBranchExistenceL( myKey ) )
            {
            // Copy first transaction
            dataDes->CopyL( myKey,dataSrc );

            // Now we have to reset the new transaction id to 0
            dataDes->SetKeyIndex( myKey, 0 );

            // Copy SessionDescriptor stuff
            myKey->ReplaceLastL( CREATEKEY( EImpsKeySessionDescriptor, 0) );
            dataDes->CopyL( myKey,dataSrc );

            aFields->SetStatus( iImpsFields->Status( ) );

            CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( aFields );
            aFields->SetMessageType( TImpsDataUtils::GetMessageTypeL( myAc ) );
            CleanupStack::PopAndDestroy(1 );     // myAc
            ret = ETrue;
            }
        CleanupStack::PopAndDestroy(1);  // myKey
        return ret;
        }
    else
        {
        CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( iImpsFields );
        iImpsFields->SetMessageType( TImpsDataUtils::GetMessageTypeL( myAc ) );
        CleanupStack::PopAndDestroy( 1 );     // myAc
        return ETrue;
        }
    }

// ---------------------------------------------------------
// TImpsDataAccessor::CopyDataL
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataAccessor::CopyDataL( 
            MImpsDataAccessor& aSource,
            MImpsKey* aKey )
    {
    TImpsDataAccessor& iSource = static_cast<TImpsDataAccessor&>(aSource);
    CImpsKey* iKey = static_cast<CImpsKey*>( aKey );
    // source data
    CImpsData* sp  = iSource.iImpsFields->Data();

    // destination data
    CImpsData* dp  = iImpsFields->Data();

    // Copy data
    dp->CopyL( iKey, sp );
    }

// ---------------------------------------------------------
// TImpsDataAccessor::DoStoreL
// Main storing function
// ---------------------------------------------------------
//
void TImpsDataAccessor::DoStoreL( const CImpsKey* aKey )
    {
    StoreHeadersL( aKey );
    }

//----------------------------------------------------------
// TImpsDataAccessor::DoRestoreL
// Main restoring function
// ---------------------------------------------------------
//
TBool TImpsDataAccessor::DoRestoreL( const CImpsKey* aKey )
    {
    return RestoreHeadersL( aKey );
    }

//----------------------------------------------------------
// TImpsDataAccessor::StoreHeadersL
// ---------------------------------------------------------
//
void TImpsDataAccessor::StoreHeadersL( const CImpsKey* aKey )
    {
    CImpsData* data = iImpsFields->Data( );
    if( iData.iType == EStringType )
        {
        data->StoreStringL( aKey, iData.iValue );
        return;
        }
    if( iData.iType == EString8Type )
        {
        data->StoreString8L( aKey, iData.iValue8 );
        return;
        }
    if( iData.iType == EIntegerType )
        {
        data->StoreIntL( aKey, iData.iValueInt );
        return;
        }
    if( iData.iType == EBooleanType )
        {
        data->StoreBooleanL( aKey, iData.iValueBool );
        return;
        }
    if( iData.iType == EEmptyType )
        {
        data->StoreEmptyL( aKey );
        return;
        }
    User::Leave( KErrArgument );
    }

//----------------------------------------------------------
// TImpsDataAccessor::RestoreHeadersL
// ---------------------------------------------------------
//
TBool TImpsDataAccessor::RestoreHeadersL( const CImpsKey* aKey )
    {
    TBool ret = EFalse;
    CImpsData* data = iImpsFields->Data( );
    if( iData.iType == EStringType )
        {
        TDesC *val;
        ret = data->RestoreString( aKey, val );
        if( ret )
            iData.iValuePointer = val;
        }
    if( iData.iType == EString8Type )
        {
        TDesC8 *val;
        ret = data->RestoreString8( aKey, val );
        if( ret )
            iData.iValuePointer8 = val;
        }
    if( iData.iType == EIntegerType )
        {
        ret = data->RestoreInt( aKey, iData.iValueInt );
        }
    if( iData.iType == EBooleanType )
        {
        ret = data->RestoreBoolean( aKey, iData.iValueBool );
        }
    if( iData.iType == EEmptyType )
        {
        ret = data->RestoreEmpty( aKey );
        }
    return ret;
    }

#ifdef _DEBUG
//----------------------------------------------------------
// TImpsDataAccessor::DumpToFileL
// ---------------------------------------------------------
//
void TImpsDataAccessor::DumpToFileL(RFs& aFs, const TDesC& aFilename)
	{
	iImpsFields->DumpToFileL(aFs, aFilename);
	}

#endif
//  End of File  
