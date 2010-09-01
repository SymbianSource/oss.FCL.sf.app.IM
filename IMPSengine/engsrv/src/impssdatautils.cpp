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
* Description: Utils for IM data.
*
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "impskey.h"
#include    "impssdatautils.h"
#include    "impsdataaccessor.h"
#include    "impsdigestbytes.h"
#include    "impserrors.h"
#include    "impsliterals.h"
#include    "ImpsVariantAPI.h"
#include    "impssrvutils.h"
#include    "impsipcirwatcherapi.h"

// CONSTANTS

// UTF-8 MIB enum
const TInt KImpsMIBeUTF8 = 106;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TImpsSDataUtils::TImpsSDataUtils()
    {
    }




// -----------------------------------------------------------------------------
// TImpsSDataUtils::SubGroupNoticeResponseL
// -----------------------------------------------------------------------------
//
TBool TImpsSDataUtils::SubGroupNoticeResponseL(
    MImpsKey* aKey,
    MImpsDataAccessor* aAc )
    {
    TDesC*  myPtr;
    TImpsSDataUtils::AddValuesFromArrayL(
        aKey,
        KTransContentElements,
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );
    aKey->AddL( CREATEKEY( EImpsKeySubscribeGroupNotice_Response, 0 ) );
    aKey->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
    if ( aAc->RestoreDescL( aKey, myPtr ) )
        {
        if ( myPtr->Compare( KImpsTrue ) == 0 )
            {
            return ETrue;
            }
        }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// TImpsSDataUtils::SetMessageDeliveredIdL
// -----------------------------------------------------------------------------
//
void TImpsSDataUtils::SetMessageDeliveredIdL(
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    const TDesC& aId )
    {

    aKey->Reset();

    TImpsSDataUtils::AddValuesFromArrayL(
        aKey,
        KTransContentElements,
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );

    aKey->AddL( CREATEKEY( EImpsKeyMessageDelivered, 0 ) );
    aKey->AddL( CREATEKEY( EImpsKeyMessageID, 0 ) );
    aAc->StoreDescL( aKey, aId );

    }

// -----------------------------------------------------------------------------
// TImpsSDataUtils::CreateLoginReqL
// -----------------------------------------------------------------------------
//
void TImpsSDataUtils::CreateLoginReqL(
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    const TDesC& aUserId,
    const TDesC& aClientId,
    const TDesC& aPwd,
    const TDesC& aCookie,
    TInt aTime )
    {
    aKey->Reset();

    TImpsSDataUtils::AddValuesFromArrayL(
        aKey,
        KTransContentElements,
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );

    aKey->AddL( CREATEKEY( EImpsKeyLogin_Request, 0 ) );
    aKey->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
    aAc->StoreDescL( aKey, aUserId );

    aKey->PopL( );
    TImpsSDataUtils::CreateClientIdL( aKey, aAc, aClientId );

    aKey->AddL( CREATEKEY( EImpsKeyPassword, 0 ) );
    aAc->StoreDescL( aKey, aPwd );

    aKey->ReplaceLastL( CREATEKEY( EImpsKeySessionCookie, 0 ) );
    aAc->StoreDescL( aKey, aCookie );

    aKey->ReplaceLastL( CREATEKEY( EImpsKeyTimeToLive, 0 ) );
    aAc->StoreIntegerL( aKey, aTime );

    }

// -----------------------------------------------------------------------------
// TImpsSDataUtils::CreateLoginReqPhaseOneL
// -----------------------------------------------------------------------------
//
void TImpsSDataUtils::CreateLoginReqPhaseOneL(
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    const TDesC& aUserId,
    const TDesC& aClientId,
    const TImpsDigestSchema aSchema,
    const TDesC& aCookie,
    TInt aTime )
    {
    if ( aUserId.Length() == 0 /*|| aClientId.Length() == 0 */
         || aCookie.Length() == 0 )
        {
        User::Leave( KImpsErrorValidate );
        }

    aKey->Reset();

    TImpsSDataUtils::AddValuesFromArrayL(
        aKey,
        KTransContentElements,
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );

    aKey->AddL( CREATEKEY( EImpsKeyLogin_Request, 0 ) );
    aKey->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
    aAc->StoreDescL( aKey, aUserId );

    aKey->PopL( );
    TImpsSDataUtils::CreateClientIdL( aKey, aAc, aClientId );

    aKey->AddL( CREATEKEY( EImpsKeyDigestSchema, 0 ) );
    aAc->StoreIntegerL( aKey, aSchema );

    aKey->ReplaceLastL( CREATEKEY( EImpsKeySessionCookie, 0 ) );
    aAc->StoreDescL( aKey, aCookie );

    aKey->ReplaceLastL( CREATEKEY( EImpsKeyTimeToLive, 0 ) );
    aAc->StoreIntegerL( aKey, aTime );

    }

// -----------------------------------------------------------------------------
// TImpsSDataUtils::CreateLoginReqPhaseTwoL
// -----------------------------------------------------------------------------
//
void TImpsSDataUtils::CreateLoginReqPhaseTwoL(
    TBool aSendPwd,
    MImpsDataAccessor* aAc,
    const TDesC& /* aCSPSessionId */,
    const TDesC& aTid,
    const TDesC& aUserId,
    const TDesC& aClientId,
    const TDesC& aPwd,
    const TDesC8* aNonce,
    const TDesC& aCookie,
    TInt aTime,
    HBufC* aKey1,
    HBufC* aKey2 )
    {
    if ( aTid.Length() == 0 ||
         aUserId.Length() == 0 ||
         aPwd.Length() == 0 || !aNonce || aNonce->Length() == 0 ||
         aCookie.Length() == 0 )
        {
        User::Leave( KImpsErrorValidate );
        }

    CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey
    myKey->AddL( CREATEKEY( EImpsKeySession, 0 ) );
    aAc->StoreEmptyL( myKey );
    myKey->AddL( CREATEKEY( EImpsKeySessionDescriptor, 0 ) );
    aAc->StoreEmptyL( myKey );
    myKey->AddL( CREATEKEY( EImpsKeySessionType, 0 ) );
    aAc->StoreIntegerL( myKey, EImpsOutband );
    myKey->PopL( 2 );

    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0 ) );
    aAc->StoreEmptyL( myKey );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionDescriptor, 0 ) );
    aAc->StoreEmptyL( myKey );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionMode, 0 ) );
    aAc->StoreIntegerL( myKey, EImpsRequest );
    myKey->PopL( 1 );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionID, 0 ) );
    aAc->StoreDescL( myKey, aTid );
    // poll missing
    myKey->PopL( 2 );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionContent, 0 ) );
    aAc->StoreEmptyL( myKey );
    myKey->AddL( CREATEKEY( EImpsKeyLogin_Request, 0 ) );
    aAc->StoreEmptyL( myKey );
    myKey->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
    aAc->StoreDescL( myKey, aUserId );
    myKey->PopL( );

    TImpsSDataUtils::CreateClientIdL( myKey, aAc, aClientId );

    TImpsDigestSchema aSchema = EImpsMD5;
    TImpsDigestBytes db;    //lint !e1502 no nonstatic members there
    if ( aKey1 != NULL && aKey2 != NULL )
        {
        db.CreateAolDigestBytesL ( aAc, aSchema, aPwd, *aNonce, aClientId, aKey1, aKey2 );
        }
    else if ( aSendPwd )
        {
        myKey->AddL( CREATEKEY( EImpsKeyPassword, 0 ) );
        aAc->StoreDescL( myKey, aPwd );
        myKey->PopL( );
        }
    else
        {
        db.CreateDigestBytesL ( aAc, aSchema, aPwd, *aNonce );
        }

    myKey->AddL( CREATEKEY( EImpsKeySessionCookie, 0 ) );
    aAc->StoreDescL( myKey, aCookie );

    myKey->ReplaceLastL( CREATEKEY( EImpsKeyTimeToLive, 0 ) );
    aAc->StoreIntegerL( myKey, aTime );

    CleanupStack::PopAndDestroy( 1 ); // >>> myKey

    }


// -----------------------------------------------------------------------------
// TImpsSDataUtils::CreateServiceRequestL
// Notice: the granularity of details is in feature level now.
// This means that function and capability level details are hard coded now.
// -----------------------------------------------------------------------------
//
void TImpsSDataUtils::CreateServiceRequestL(
    MImpsDataAccessor* aAc,
    TImpsServices& aServices,
    const TDesC& aSID,
    const TDesC& aTID,
    const TDesC& aCID,
    TBool aReactive )
    {
    TImpsSDataUtils::SetSessionIDL( aAc, aSID );
    TImpsSDataUtils::SetTransactionIDL( aAc, aTID );

    CImpsKey* myKey = CImpsKey::NewLC();   // <<< myKey

    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KClientServiceReqElements,
        sizeof( KClientServiceReqElements ) /
        sizeof( KClientServiceReqElements[0] ) );

    // Client id
    // WV 1.2 missing ClientId
    CImpsFields* f = aAc->GetImpsFields();
    if ( f->CspVersion() == EImpsCspVersion11 )
        {
        TImpsSDataUtils::CreateClientIdL(
            myKey,
            aAc,
            aCID );
        }

    // Do not ask AllFunctionsRequest
    myKey->AddL( CREATEKEY( EImpsKeyAllFunctionsRequest, 0 ) );
    aAc->StoreBooleanL( myKey, EFalse );

    // Functions and details are hard coded now.
    // Notice: update this if new features are available
    // This section must be in line with ReadConfig().

    myKey->ReplaceLastL( CREATEKEY( EImpsKeyFunctions, 0 ) );
    // WVCSPFeat
    myKey->AddL( CREATEKEY( EImpsKeyWVCSPFeat, 0 ) );
    // Check IM
    TImpsFeature* myFeat = aServices.Feature( EIMFeat );
    if ( myFeat->IsSupported() )
        {
        myKey->AddL( CREATEKEY( EImpsKeyIMFeat, 0 ) );
        aAc->StoreEmptyL( myKey );
        if ( myFeat->Function( EIMReceiveFunc ) )
            {
            myKey->AddL( CREATEKEY( EImpsKeyIMReceiveFunc, 0 ) );
            aAc->StoreEmptyL( myKey );
            if ( myFeat->Capability( KImpsServiceNEWM ) )
                {
                myKey->AddL( CREATEKEY( EImpsKeyNEWM, 0 ) );
                aAc->StoreEmptyL( myKey );
                myKey->PopL();
                }
            myKey->ReplaceLastL( CREATEKEY( EImpsKeyIMAuthFunc, 0 ) );
            aAc->StoreEmptyL( myKey );
            }
        }

    // Group features
    myFeat = aServices.Feature( EGroupFeat );
    if ( myFeat->IsSupported() )
        {
        myKey->Reset();
        TImpsSDataUtils::AddValuesFromArrayL(
            myKey,
            KClientServiceReqElements,
            sizeof( KClientServiceReqElements ) /
            sizeof( KClientServiceReqElements[0] ) );
        myKey->AddL( CREATEKEY( EImpsKeyFunctions, 0 ) );
        myKey->AddL( CREATEKEY( EImpsKeyWVCSPFeat, 0 ) );
        myKey->AddL( CREATEKEY( EImpsKeyGroupFeat, 0 ) );
        aAc->StoreEmptyL( myKey );
        }

    // Presence features
    myFeat = aServices.Feature( EPresenceFeat );
    if ( myFeat->IsSupported() )
        {
        myKey->Reset();
        TImpsSDataUtils::AddValuesFromArrayL(
            myKey,
            KClientServiceReqElements,
            sizeof( KClientServiceReqElements ) /
            sizeof( KClientServiceReqElements[0] ) );
        myKey->AddL( CREATEKEY( EImpsKeyFunctions, 0 ) );
        myKey->AddL( CREATEKEY( EImpsKeyWVCSPFeat, 0 ) );
        myKey->AddL( CREATEKEY( EImpsKeyPresenceFeat, 0 ) );

        // If reactive authentication then the entire presence
        // is supported
        if ( !aReactive )
            {
            // These PRESENCE functions are supported if
            // not reactive authentication
            // PresenceDeliveryFunc
            myKey->AddL( CREATEKEY( EImpsKeyPresenceDeliverFunc, 0 ) );
            aAc->StoreEmptyL( myKey );
            // AttListFunc supported
            myKey->ReplaceLastL( CREATEKEY( EImpsKeyAttListFunc, 0 ) );
            aAc->StoreEmptyL( myKey );
            // Contact Lists
            myKey->ReplaceLastL( CREATEKEY( EImpsKeyContListFunc, 0 ) );
            aAc->StoreEmptyL( myKey );
            // Presence authentication
            myKey->ReplaceLastL( CREATEKEY( EImpsKeyPresenceAuthFunc, 0 ) );
            myKey->AddL( CREATEKEY( EImpsKeyGETWL, 0 ) );
            }
        // Add last presence element, either EImpsKeyPresenceFeat
        // or EImpsKeyGETWL
        aAc->StoreEmptyL( myKey );
        }

    // Fundamental features
    myFeat = aServices.Feature( EFundamentalFeat );
    if ( myFeat->IsSupported() )
        {
        myKey->Reset();
        TImpsSDataUtils::AddValuesFromArrayL(
            myKey,
            KClientServiceReqElements,
            sizeof( KClientServiceReqElements ) /
            sizeof( KClientServiceReqElements[0] ) );
        myKey->AddL( CREATEKEY( EImpsKeyFunctions, 0 ) );
        myKey->AddL( CREATEKEY( EImpsKeyWVCSPFeat, 0 ) );
        myKey->AddL( CREATEKEY( EImpsKeyFundamentalFeat, 0 ) );
        myKey->AddL( CREATEKEY( EImpsKeySearchFunc, 0 ) );
        aAc->StoreEmptyL( myKey );
        myKey->ReplaceLastL( CREATEKEY( EImpsKeyInviteFunc, 0 ) );
        aAc->StoreEmptyL( myKey );
        }

    // Add TImpsTransactionMode now
    myKey->Reset();
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KTransModeElements,
        sizeof( KTransModeElements ) /
        sizeof( KTransModeElements[0] ) );
    aAc->StoreIntegerL( myKey, EImpsRequest );

    CleanupStack::PopAndDestroy( 1 ); // >>> myKey

    }

// -----------------------------------------------------------------------------
// TImpsSDataUtils::CreateClientCapabilityReqL
// Notice: Many details are hard coded now.
// Param aSettings cannot be null.
// -----------------------------------------------------------------------------
//
void TImpsSDataUtils::CreateClientCapabilityReqL(
    MImpsDataAccessor* aAc,
    const TDesC& aSID,
    const TDesC& aTID,
    const TDesC& aCID,
    TInt aMultiTrans,
    TInt aPollTime,
    TImpsSrvSettings& aSettings )
    {
    TImpsSDataUtils::SetSessionIDL( aAc, aSID );
    TImpsSDataUtils::SetTransactionIDL( aAc, aTID );
    CImpsKey* myKey = CImpsKey::NewLC();   // <<< myKey

    const TImpsContent* myContent = KClientCapabilityReqElements;
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        myContent,
        sizeof( KClientCapabilityReqElements ) /
        sizeof( KClientCapabilityReqElements[0] ) );

    // Client id
    // WV 1.2 missing ClientId
    CImpsFields* f = aAc->GetImpsFields();
    if ( f->CspVersion() == EImpsCspVersion11 )
        {
        TImpsSDataUtils::CreateClientIdL(
            myKey,
            aAc,
            aCID );
        }

    // capabilitylist
    myKey->AddL( CREATEKEY( EImpsKeyCapabilityList, 0 ) );

    // client type
    myKey->AddL( CREATEKEY( EImpsKeyClientType, 0 ) );
    aAc->StoreIntegerL( myKey, EImpsMOBILE_PHONE );
    myKey->PopL();

    // Initial delivery method
    myKey->AddL( CREATEKEY( EImpsKeyInitialDeliveryMethod, 0 ) );
    aAc->StoreIntegerL( myKey, EImpsP );
    myKey->PopL();

    // Content types
    myKey->AddL( CREATEKEY( EImpsKeyAnyContent, 0 ) );
    aAc->StoreBooleanL( myKey, ETrue );
    myKey->ReplaceLastL( CREATEKEY( EImpsKeyAcceptedCharSet, 0 ) );
    aAc->StoreIntegerL( myKey, KImpsMIBeUTF8 );
    myKey->PopL();

    // accepted content length
    myKey->AddL( CREATEKEY( EImpsKeyAcceptedContentLength, 0 ) );
    aAc->StoreIntegerL( myKey, aSettings.MaximumMessageSize() );
    myKey->PopL();

    // accepted transfer encoding
    myKey->AddL( CREATEKEY( EImpsKeyAcceptedTransferEncoding, 0 ) );
    aAc->StoreIntegerL( myKey, EImpsBASE64 );
    myKey->PopL();

    // SupportedBearer
    myKey->AddL( CREATEKEY( EImpsKeySupportedBearer, 0 ) );
    aAc->StoreIntegerL( myKey, EImpsHTTP );
    myKey->PopL();

    // SupportedCIRMethods
    CImpsVariant* myVariant = CImpsVariant::NewLC( );
    TInt index( 0 );
    myKey->AddL( CREATEKEY( EImpsKeySupportedCIRMethod, index ) );
    // WAP SMS CIR
    if ( aSettings.SMSWAPCIR() )
        {
        index++;
        aAc->StoreIntegerL( myKey, EImpsWAPSMS );
        }
    // WAP UDP CIR
    if ( myVariant->IsFeatureSupportedL( EWAPUDP ) && aSettings.UDPWAPCIR() )
        {
        myKey->ReplaceLastL( CREATEKEY( EImpsKeySupportedCIRMethod, index++ ) );
        aAc->StoreIntegerL( myKey, EImpsWAPUDP );
        }
    // TCP standalone CIR
    if ( myVariant->IsFeatureSupportedL( EIpCirStandalone ) && aSettings.TCPSCIR() )
        {
        myKey->ReplaceLastL( CREATEKEY( EImpsKeySupportedCIRMethod, index++ ) );
        aAc->StoreIntegerL( myKey, EImpsSTCP );
        }
    // UDP standalone CIR
    if ( myVariant->IsFeatureSupportedL( EIpCirStandalone ) && aSettings.UDPSCIR() )
        {
        myKey->ReplaceLastL( CREATEKEY( EImpsKeySupportedCIRMethod, index++ ) );
        aAc->StoreIntegerL( myKey, EImpsSUDP );
        // Default KDefaultUDPPortNumber in the message due to CCAPAB-10
        myKey->ReplaceLastL( CREATEKEY( EImpsKeyUDPPort, 0 ) );
        aAc->StoreIntegerL( myKey, KDefaultUDPPortNumber );
        }
    CleanupStack::PopAndDestroy( ); // << myVariant
    myKey->PopL();

    // Multitrans
    myKey->AddL( CREATEKEY( EImpsKeyMultiTrans, 0 ) );
    aAc->StoreIntegerL( myKey, aMultiTrans );
    myKey->PopL();

    // Parser size
    myKey->AddL( CREATEKEY( EImpsKeyParserSize, 0 ) );
    aAc->StoreIntegerL( myKey, aSettings.MaximumParserSize() );
    myKey->PopL();

    // Poll min initial EImpsKeyServerPollMin
    myKey->AddL( CREATEKEY( EImpsKeyServerPollMin, 0 ) );
    aAc->StoreIntegerL( myKey, aPollTime );

    // Add TImpsTransactionMode now
    myKey->Reset();
    myContent = KTransModeElements;
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        myContent,
        sizeof( KTransModeElements ) /
        sizeof( KTransModeElements[0] ) );
    aAc->StoreIntegerL( myKey, EImpsRequest );

    CleanupStack::PopAndDestroy( 1 );   // >>> mKey

    }

// -----------------------------------------------------------------------------
// TImpsSDataUtils::GetKeepAliveL
// -----------------------------------------------------------------------------
//
TBool TImpsSDataUtils::GetKeepAliveL(
    MImpsDataAccessor* aAc,
    TInt& aTime )
    {
    TInt time( 0 );
    CImpsKey* myk = CImpsKey::NewLC();    // <<< myk
    TImpsSDataUtils::AddValuesFromArrayL(
        myk,
        KTransContentElements,
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );
    myk->AddL( CREATEKEY( EImpsKeyKeepAlive_Response, 0 ) );
    // Check if this is KeepAliveResponse
    if ( aAc->CheckBranchExistenceL( myk ) )
        {
        myk->AddL( CREATEKEY( EImpsKeyKeepAliveTime, 0 ) );
        }
    else
        {
        // It should be LoginResponse
        myk->ReplaceLastL( CREATEKEY( EImpsKeyLogin_Response, 0 ) );
        myk->AddL( CREATEKEY( EImpsKeyKeepAliveTime, 0 ) );
        }
    if ( aAc->RestoreIntegerL( myk, time ) )
        {
        aTime = time;
        }
    CleanupStack::PopAndDestroy( 1 );   // >>> myk
    return ( aTime ? ETrue : EFalse );
    }

// -----------------------------------------------------------------------------
// TImpsSDataUtils::GetLoginSessionIDL
// -----------------------------------------------------------------------------
//
void TImpsSDataUtils::GetLoginSessionIDL( MImpsDataAccessor* aAc, TPtrC& aID  )
    {
    TDesC* myPtr;
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionContent, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyLogin_Response, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeySessionID, 0 ) );
    if ( aAc->RestoreDescL( myKey, myPtr ) )
        {
        aID.Set( *myPtr );
        }
    else
        {
        aID.Set( KNullDesC );
        }
    CleanupStack::PopAndDestroy( );  //myKey
    }

// -----------------------------------------------------------------------------
// TImpsSDataUtils::ContentDataTypeL
// -----------------------------------------------------------------------------
//
void TImpsSDataUtils::ContentDataTypeL(
    MImpsDataAccessor* aAc,
    TPtrC& aContent )
    {
    TInt msgType = GetMessageTypeL( aAc );
    TDesC* myPtr;
    CImpsKey* myKey = CImpsKey::NewLC();
    aContent.Set( KNullDesC );

    switch ( msgType )
        {
        case EImpsNewMessage:
            {
            TImpsDataUtils::AddValuesFromArrayL(
                myKey,
                KNewMessageElements,
                sizeof( KNewMessageElements ) /
                sizeof( KNewMessageElements[0] ) );
            myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );
            myKey->AddL( CREATEKEY( EImpsKeyContentType, 0 ) );
            if ( aAc->RestoreDescL( myKey, myPtr ) )
                {
                aContent.Set( *myPtr );
                }
            break;
            }
        // Notice: Presence is not checked here
        default:
            break;
        };

    CleanupStack::PopAndDestroy( );  //myKey
    }


// -----------------------------------------------------------------------------
// TImpsSDataUtils::GetClientIDL
// Only new message is supported now!
// -----------------------------------------------------------------------------
//
void TImpsSDataUtils::GetApplicationIDL( MImpsDataAccessor* aAc, TPtrC& aID  )
    {
    TDesC* myPtr;
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyExtBlock, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyAPIClient, 0 ) );
    if ( aAc->RestoreDescL( myKey, myPtr ) )
        {
        aID.Set( *myPtr );
        }
    else
        {
        aID.Set( KNullDesC );
        }
    CleanupStack::PopAndDestroy( );  //myKey
    }

// -----------------------------------------------------------------------------
// TImpsSDataUtils::GetCIRL
// CIR error element exists in WV 1.2 and higher only
// -----------------------------------------------------------------------------
//
TBool TImpsSDataUtils::GetCIRL( MImpsDataAccessor* aAc  )
    {
    TBool ret( EFalse );
    // first check that not WV 1.1 message
    CImpsFields* f = aAc->GetImpsFields();
    if ( f->CspVersion() == EImpsCspVersion11 )
        {
        return ret;
        }

    CImpsKey* myKey = CImpsKey::NewLC();
    TImpsDataUtils::AddValuesFromArrayL(
        myKey, KGetCIRElements,
        sizeof( KGetCIRElements ) / sizeof( KGetCIRElements[0] ) );
    aAc->RestoreBooleanL( myKey, ret );

    CleanupStack::PopAndDestroy( );  //myKey
    return ret;
    }

// -----------------------------------------------------------------------------
// TImpsSDataUtils::GetPureMessageTypeL
// -----------------------------------------------------------------------------
//
TInt TImpsSDataUtils::GetPureMessageTypeL( MImpsDataAccessor* aAc )
    {
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionContent, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyPureData, 0 ) );
    TInt enumi( 0 );
    TInt index( 0 );
    TImpsKeyType type( EImpsKeyTypeIM );
    // try to get the original message type
    if ( aAc->RestoreAlternativeL( myKey, enumi, index, type ) )
        {
        CleanupStack::PopAndDestroy( );  //myKey
        // if the message type is not stored correctly,
        // the default value is EImpsMessageNone
        if ( enumi == EImpsMessageNone )
            {
            return EImpsPureData;
            }
        return enumi;
        }

    CleanupStack::PopAndDestroy( );  //myKey
    return EImpsPureData;

    }
//  End of File

