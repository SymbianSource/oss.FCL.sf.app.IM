/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: rovides the methods for pureData handling.
*
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    <miutconv.h>
#include    "impsxmlutils.h"
#include    "impsdataaccessorapi.h"
#include    "impsliterals.h"
#include    "impscommonenums.h"
#include    "impserrors.h"
#include    "impspresence.h"
#include    "impsfundamental.h"
#include    "impsutils.h"

// CONSTANTS
const TInt KImpsMaxStringSize100 = 100;
const TInt KImpsBaseIndex = 0;
const TInt KImpsExtraSpace = 30;
const TInt KIntegerBufSize = 10;

const TInt KInitBufSize = 1000;
const TInt KExpandBufSize = 500;

const TInt KTrConStartLen = 19;
const TInt KTagLen = 1;
const TInt KEndTagLen = 2;

_LIT8( KTrConStart,  "<TransactionContent");
_LIT8( KTrConEnd,    "</TransactionContent>");


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CImpsXmlUtils::CImpsXmlUtils()
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
CImpsXmlUtils::CImpsXmlUtils(): 
iPosition( 0 ),
iBufSize( KImpsMaxStringSize100 ),
iEncodeBufferSize( KInitBufSize )
    {
    }
// ---------------------------------------------------------
// CImpsXmlUtils::ConstructL()
// EPOC default constructor can leave.
// ---------------------------------------------------------
void CImpsXmlUtils::ConstructL()
    {
    iElementValue = HBufC8::NewL( iBufSize );
    iElementData  = HBufC::NewL( iBufSize );
    iIntegerValue = HBufC8::NewL( KIntegerBufSize );
    iEncodeBuffer = CBufFlat::NewL( 50 );
    iEncodeBuffer->ResizeL( iEncodeBufferSize );
    }
// ---------------------------------------------------------
// CImpsXmlUtils::ResetL()
// ---------------------------------------------------------
void CImpsXmlUtils::ResetL()
    {
    iPosition = 0;
    if ( iElementValue == NULL )
        {
        iBufSize = KImpsMaxStringSize100;
        iElementValue = HBufC8::NewL( iBufSize );
        }
    if ( iElementData == NULL )
        {
        iBufSize = KImpsMaxStringSize100;
        iElementData  = HBufC::NewL( iBufSize );
        }
    if ( iIntegerValue == NULL )
        {
        iIntegerValue = HBufC8::NewL( KIntegerBufSize );
        }
    if ( iEncodeBuffer == NULL )
        {
        iEncodeBuffer = CBufFlat::NewL( 50 );
        iEncodeBufferSize = KInitBufSize;
        iEncodeBuffer->ResizeL( iEncodeBufferSize );
        }
    }
// ---------------------------------------------------------
// CImpsXmlUtils::NewL()
// Two-phased constructor.
// ---------------------------------------------------------
CImpsXmlUtils* CImpsXmlUtils::NewL()
    {
    CImpsXmlUtils* self = new ( ELeave ) CImpsXmlUtils;   
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
// ---------------------------------------------------------
// CImpsXmlUtils::~CImpsXmlUtils()
// Destructor
// ---------------------------------------------------------
CImpsXmlUtils::~CImpsXmlUtils()
    {
    delete iElementValue;
    delete iIntegerValue;
    delete iElementData;
    delete iEncodeBuffer;
    }
// ---------------------------------------------------------
// CImpsXmlUtils::TransactionContentToXmlL
// ---------------------------------------------------------
void CImpsXmlUtils::TransactionContentToXmlL( MImpsDataAccessor& /*aImpsData*/ )
    {
    }
// ---------------------------------------------------------
// CImpsXmlUtils::StorePureDataL
// ---------------------------------------------------------
void CImpsXmlUtils::StorePureDataL( MImpsDataAccessor& aImpsData, TInt aIndex, TInt aMessageType )
    {
//    if ( iAccessKey ) 
//        {
//        iAccessKey->Destroy();
//        }

    iEncodeBufferSize = iPosition;
    iEncodeBuffer->ResizeL( iPosition );

    iAccessKey = aImpsData.NewKeyL();
    iAccessKey->AddL( CREATEKEY( EImpsKeySession, KImpsBaseIndex ) );
    iAccessKey->AddL( CREATEKEY( EImpsKeyTransaction, aIndex ) );
    iAccessKey->AddL( CREATEKEY( EImpsKeyTransactionContent, KImpsBaseIndex ) );
    iAccessKey->AddL( CREATEKEY( EImpsKeyPureData, KImpsBaseIndex ) );
    aImpsData.StoreDesc8L( iAccessKey, iEncodeBuffer->Ptr(0 ) );
    // store the actual message type
    iAccessKey->AddL( CREATEKEY( aMessageType, KImpsBaseIndex ) );
    aImpsData.StoreEmptyL( iAccessKey );
    
    if ( iEncodeBufferSize > KInitBufSize )
        {
        iEncodeBufferSize = KInitBufSize;
        iEncodeBuffer->ResizeL( iEncodeBufferSize );
        iEncodeBuffer->Compress();
        }
    iAccessKey->Destroy();
    }
// ---------------------------------------------------------
// CImpsXmlUtils::Convert16To8L
// ---------------------------------------------------------
//TPtrC8 CImpsXmlUtils::Convert16To8L( TDesC* aDes )
//    {   
//    TPtr8 ptr8 = iElementValue->Des();
//    ptr8.Zero();
//    if ( aDes != NULL )
//        {
//        TInt size = aDes->Length();
//        if ( size <= 0 )
//            return ptr8;
//        if ( size > iBufSize )
//            {
//            delete iElementValue;
//            iElementValue = NULL;
//            iElementValue = HBufC8::NewL( size + KImpsExtraSpace );
//            iBufSize = size + KImpsExtraSpace;
//            TPtr8 ptr = iElementValue->Des();
//            TInt rcode = CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr, *aDes );
//            iBufSize = size;
//            if ( rcode != KErrNone )
//                {
//                User::Leave( KImpsErrorDecode );
//                }
//            return EscapeXmlL( );
//            }
//        TInt rcode = CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr8, *aDes );
//        if ( rcode != KErrNone )
//            {
//            User::Leave( KImpsErrorDecode );
//            }
//        }
//    return EscapeXmlL( );
//    }
// ---------------------------------------------------------
// CImpsXmlUtils::ConvertBooleanToChar
// ---------------------------------------------------------
//TPtrC8 CImpsXmlUtils::ConvertBooleanToChar( TBool aBool )
//    {
//    if ( aBool )
//        return TPtrC8( KTrue );
//    else
//        return TPtrC8( KFalse );
//    }
// ---------------------------------------------------------
// CImpsXmlUtils::ConvertIntToPtr
// ---------------------------------------------------------
//TPtrC8 CImpsXmlUtils::ConvertIntToPtr( TInt aInteger )
//    {
//    TPtr8 ptr = iIntegerValue->Des();
//    ptr.Zero();
//    ptr.AppendNum( aInteger );
//    return ptr;
//    }
// ---------------------------------------------------------
// CImpsXmlUtils::WriteToBufferL
// ---------------------------------------------------------
void CImpsXmlUtils::WriteToBufferL( const TDesC8& aData )
    {

	if ( (iPosition + aData.Length() ) > iEncodeBufferSize )
        {
        iEncodeBufferSize += ( KExpandBufSize + aData.Length() );
        iEncodeBuffer->ResizeL( iEncodeBufferSize );
        }

    iEncodeBuffer->Write( iPosition, aData );
    iPosition += aData.Length();
    }
// ---------------------------------------------------------
// CImpsXmlUtils::StartElementL
// ---------------------------------------------------------
void CImpsXmlUtils::StartElementL( const TDesC8& aTag, const TDesC8& aAttribute )
    {
    WriteToBufferL( TPtrC8( KStartOfTag ) );
    WriteToBufferL( aTag );
    WriteToBufferL( aAttribute );
    WriteToBufferL( TPtrC8( KEndOfTag ) );
    }
// ---------------------------------------------------------
// CImpsXmlUtils::EndElementL
// ---------------------------------------------------------
void CImpsXmlUtils::EndElementL( const TDesC8& aTag )
    {
    WriteToBufferL( TPtrC8( KElementTerminator ) );
    WriteToBufferL( aTag );
    WriteToBufferL( TPtrC8( KEndOfTag ) );
    }
// ---------------------------------------------------------
// CImpsXmlUtils::WriteEmptyElementL
// ---------------------------------------------------------
void CImpsXmlUtils::WriteEmptyElementL( const TDesC8& aTag )
    {
    WriteToBufferL( TPtrC8( KStartOfTag ) );
    WriteToBufferL( aTag ); 
    WriteToBufferL( TPtrC8( KEndTag ) );
    }
// ---------------------------------------------------------
// CImpsXmlUtils::EncodeIntegerL
// ---------------------------------------------------------
//void CImpsXmlUtils::EncodeIntegerL( TInt aKey, 
//                                    TPtrC8 aElementName, 
//                                    TImpsKeyType aKeyType )
//    {
//    iAccessKey->AddL( CREATEKEY(aKey, KImpsBaseIndex ), aKeyType );
//    TInt value;
//    if ( iImpsData->RestoreIntegerL( iAccessKey, value ) )
//        {
//        StartElementL( aElementName, KNullDesC8 );
//        TPtrC8 data = ConvertIntToPtr ( value );
//        WriteToBufferL( data );
//        EndElementL( aElementName );
//        }
//    iAccessKey->PopL();
//    }
// ---------------------------------------------------------
// CImpsXmlUtils::NameToKeyPresenceElements
// ---------------------------------------------------------
TImpsPresenceContent CImpsXmlUtils::NameToKeyPresenceElements ( TDesC8* aName )
    {
    if ( aName->CompareF( KQualifier ) == 0 )              { return EImpsKeyPRQualifier; }
    else if ( aName->CompareF( KPresenceValue ) == 0 )     { return EImpsKeyPRPresenceValue; }
    else if ( aName->CompareF( KOnlineStatus ) == 0 )      { return EImpsKeyPROnlineStatus; }
    else if ( aName->CompareF( KRegistration ) == 0 )      { return EImpsKeyPRRegistration; }
    else if ( aName->CompareF( KFreeTextLocation ) == 0 )  { return EImpsKeyPRFreeTextLocation; }
    else if ( aName->CompareF( KPLMN ) == 0 )              { return EImpsKeyPRPLMN; }
    else if ( aName->CompareF( KUserAvailability ) == 0 )  { return EImpsKeyPRUserAvailability; }
    else if ( aName->CompareF( KPreferredLanguage ) == 0 ) { return EImpsKeyPRPreferredLanguage; }
    else if ( aName->CompareF( KStatusText ) == 0 )        { return EImpsKeyPRStatusText; }
    else if ( aName->CompareF( KStatusMood ) == 0 )        { return EImpsKeyPRStatusMood; }
    else if ( aName->CompareF( KAlias ) == 0 )             { return EImpsKeyPRAlias; }
    else if ( aName->CompareF( KClientInfo ) == 0 )        { return EImpsKeyPRClientInfo; }
    else if ( aName->CompareF( KClientType ) == 0 )        { return EImpsKeyPRClientType; }
    else if ( aName->CompareF( KDevManufacturer ) == 0 )   { return EImpsKeyPRDevManufacturer; }
    else if ( aName->CompareF( KClientProducer ) == 0 )    { return EImpsKeyPRClientProducer; }
    else if ( aName->CompareF( KModel ) == 0 )             { return EImpsKeyPRModel; }
    else if ( aName->CompareF( KClientVersion ) == 0 )     { return EImpsKeyPRClientVersion; }
    else if ( aName->CompareF( KLanguage ) == 0 )          { return EImpsKeyPRLanguage; }
    else if ( aName->CompareF( KTimeZone ) == 0 )          { return EImpsKeyPRTimeZone; }
    else if ( aName->CompareF( KZone ) == 0 )              { return EImpsKeyPRZone; }
    else if ( aName->CompareF( KGeoLocation ) == 0 )       { return EImpsKeyPRGeoLocation; }
    else if ( aName->CompareF( KLongitude ) == 0 )         { return EImpsKeyPRLongitude; }
    else if ( aName->CompareF( KLatitude ) == 0 )          { return EImpsKeyPRLatitude; }
    else if ( aName->CompareF( KAltitude ) == 0 )          { return EImpsKeyPRAltitude; }
    else if ( aName->CompareF( KAccuracy ) == 0 )          { return EImpsKeyPRAccuracy; }
    else if ( aName->CompareF( KAddress ) == 0 )           { return EImpsKeyPRAddress; }
    else if ( aName->CompareF( KCountry ) == 0 )           { return EImpsKeyPRCountry; }
    else if ( aName->CompareF( KCity ) == 0 )              { return EImpsKeyPRCity; }
    else if ( aName->CompareF( KStreet ) == 0 )            { return EImpsKeyPRStreet; }
    else if ( aName->CompareF( KCrossing1 ) == 0 )         { return EImpsKeyPRCrossing1; }
    else if ( aName->CompareF( KCrossing2 ) == 0 )         { return EImpsKeyPRCrossing2; }
    else if ( aName->CompareF( KBuilding ) == 0 )          { return EImpsKeyPRBuilding; }
    else if ( aName->CompareF( KNamedArea ) == 0 )         { return EImpsKeyPRNamedArea; }
    else if ( aName->CompareF( KCommCap ) == 0 )           { return EImpsKeyPRCommCap; }
    else if ( aName->CompareF( KCommC ) == 0 )             { return EImpsKeyPRCommC; }
    else if ( aName->CompareF( KCap ) == 0 )               { return EImpsKeyPRCap; }
    else if ( aName->CompareF( KStatus ) == 0 )            { return EImpsKeyPRStatus; }
    else if ( aName->CompareF( KContact ) == 0 )           { return EImpsKeyPRContact; }
    else if ( aName->CompareF( KNote ) == 0 )              { return EImpsKeyPRNote; }
    else if ( aName->CompareF( KPreferredContacts ) == 0 ) { return EImpsKeyPRPreferredContacts; }
    else if ( aName->CompareF( KAddrPref ) == 0 )          { return EImpsKeyPRAddrPref; }
    else if ( aName->CompareF( KPrefC ) == 0 )             { return EImpsKeyPRPrefC; }
    else if ( aName->CompareF( KCaddr ) == 0 )             { return EImpsKeyPRCaddr; }
    else if ( aName->CompareF( KCstatus ) == 0 )           { return EImpsKeyPRCstatus; }
    else if ( aName->CompareF( KCname ) == 0 )             { return EImpsKeyPRCname; }
    else if ( aName->CompareF( KCpriority ) == 0 )         { return EImpsKeyPRCpriority; }
    else if ( aName->CompareF( KStatusContent ) == 0 )     { return EImpsKeyPRStatusContent; }
    else if ( aName->CompareF( KDirectContent ) == 0 )     { return EImpsKeyPRDirectContent; }
    else if ( aName->CompareF( KReferredContent ) == 0 )   { return EImpsKeyPRReferredContent; }
    else if ( aName->CompareF( KContactInfo ) == 0 )       { return EImpsKeyPRContactInfo; }
    else if ( aName->CompareF( KContainedvCard ) == 0 )    { return EImpsKeyPRContainedvCard; }
    else if ( aName->CompareF( KReferredvCard ) == 0 )     { return EImpsKeyPRReferredvCard; }
    else if ( aName->CompareF( KInf_link ) == 0 )          { return EImpsKeyPRInf_link; }
    else if ( aName->CompareF( KInfoLink ) == 0 )          { return EImpsKeyPRInfoLink; }
    else if ( aName->CompareF( KLink ) == 0 )              { return EImpsKeyPRLink; }
    else if ( aName->CompareF( KStatusText ) == 0 )        { return EImpsKeyPRText; }
    else
        return EImpsKeyPREND;               
    }
// ---------------------------------------------------------
// CImpsXmlUtils::NameToKeyPresencePrimitives
// ---------------------------------------------------------
TImpsContent CImpsXmlUtils::NameToKeyPresencePrimitives ( TDesC8* aName )
    {   
    if ( aName->CompareF( KSubscribePresenceRequest ) == 0 )        
        { 
        iPrimitive = EImpsKeySubscribePresence_Request; 
        return EImpsKeySubscribePresence_Request;
        }
    else if ( aName->CompareF( KUnsubscribePresenceRequest ) == 0 ) 
        { 
        iPrimitive = EImpsKeyUnsubscribePresence_Request;
        return EImpsKeyUnsubscribePresence_Request;
        }
    else if ( aName->CompareF( KGetPresenceRequest ) == 0 )         
        { 
        iPrimitive = EImpsKeyGetPresence_Request; 
        return EImpsKeyGetPresence_Request;
        }
    else if ( aName->CompareF( KUpdatePresenceRequest ) == 0 )      
        { 
        iPrimitive = EImpsKeyUpdatePresence_Request;
        return EImpsKeyUpdatePresence_Request;
        }
    else if ( aName->CompareF( KGetAttributeListRequest ) == 0 )    
        { 
        iPrimitive = EImpsKeyGetAttributeList_Request;
        return EImpsKeyGetAttributeList_Request;
        }
    else if ( aName->CompareF( KDeleteAttributeListRequest ) == 0 ) 
        { 
        iPrimitive = EImpsKeyDeleteAttributeList_Request;
        return EImpsKeyDeleteAttributeList_Request;
        }
    else if ( aName->CompareF( KCreateAttributeListRequest ) == 0 ) 
        { 
        iPrimitive = EImpsKeyCreateAttributeList_Request;
        return EImpsKeyCreateAttributeList_Request;
        }
    else if ( aName->CompareF( KCancelAuthRequest ) == 0 )          
        { 
        iPrimitive = EImpsKeyCancelAuth_Request;
        return EImpsKeyCancelAuth_Request;
        }
    else if ( aName->CompareF( KPresenceAuthUser) == 0 )           
        { 
        iPrimitive = EImpsKeyPresenceAuth_User;
        return EImpsKeyPresenceAuth_User;
        }
    else if ( aName->CompareF( KGetWatcherListRequest ) == 0 )      
        { 
        iPrimitive = EImpsKeyGetWatcherList_Request;
        return EImpsKeyGetWatcherList_Request;
        } 
    else if ( aName->CompareF( KGetListRequest ) == 0 )         
        { 
        iPrimitive = EImpsKeyGetList_Request;
        return EImpsKeyGetList_Request;
        }
    else if ( aName->CompareF( KCreateListRequest ) == 0 )          
        { 
        iPrimitive = EImpsKeyCreateList_Request;
        return EImpsKeyCreateList_Request;
        }
    else if ( aName->CompareF( KDeleteListRequest ) == 0 )          
        { 
        iPrimitive = EImpsKeyDeleteList_Request;
        return EImpsKeyDeleteList_Request;
        }
    else if ( aName->CompareF( KListManageRequest ) == 0 )          
        { 
        iPrimitive = EImpsKeyListManage_Request;
        return EImpsKeyListManage_Request;
        }

    else if ( aName->CompareF( KGetReactiveAuthStatusRequest ) == 0 ) 
        {
        iPrimitive = EImpsKeyGetReactiveAuthStatus_Request;
        return EImpsKeyGetReactiveAuthStatus_Request;
        }
    
    else if ( aName->CompareF( KStatus ) == 0 )
        { 
        if ( iPrimitive == KErrNotFound )
            {
            iPrimitive = EImpsKeyStatus; 
            return EImpsKeyStatus;
            }
        return EImpsKeyEND;
    }    
    else if ( aName->CompareF( KTransactionContent ) == 0 )   { return EImpsKeyTransactionContent; }
    else if ( aName->FindF( KPresenceSubList ) != KErrNotFound)            
        { 
        return EImpsKeyPresenceSubList;
        }
    // These are here because they are not in PA namespace
    else if ( aName->CompareF( KUser ) == 0 )                 { return EImpsKeyUser; }
    else if ( aName->CompareF( KUserID ) == 0 )               { return EImpsKeyUserID; }
    else if ( aName->CompareF( KAcceptance ) == 0 )           { return EImpsKeyAcceptance; }
    else if ( aName->CompareF( KCode ) == 0 )                 { return EImpsKeyCode; }
    else if ( aName->CompareF( KContactList ) == 0 )          { return EImpsKeyContactList; }
    else if ( aName->CompareF( KDetailedResult ) == 0 )       { return EImpsKeyDetailedResult; }
    else if ( aName->CompareF( KPresence ) == 0 )             { return EImpsKeyPresence; } 
    else if ( aName->CompareF( KUser ) == 0 )                 { return EImpsKeyUser; }
    else if ( aName->CompareF( KURL ) == 0 )                  { return EImpsKeyURL; }
    else if ( aName->CompareF( KScreenName ) == 0 )           { return EImpsKeyScreenName; }
    else if ( aName->CompareF( KDefaultContactList ) == 0 )   { return EImpsKeyDefaultContactList; }
    else if ( aName->CompareF( KDefaultList ) == 0 )          { return EImpsKeyDefaultList; }
    else if ( aName->CompareF( KClientID ) == 0 )             { return EImpsKeyClientID; }
    else if ( aName->CompareF( KGroupID ) == 0 )              { return EImpsKeyGroupID; }
    else if ( aName->CompareF( KMessageID ) == 0 )            { return EImpsKeyMessageID; }
    else if ( aName->CompareF( KNickList ) == 0 )             { return EImpsKeyNickList; }
    else if ( aName->CompareF( KName ) == 0 )                 { return EImpsKeyName; }
    else if ( aName->CompareF( KNickName ) == 0 )             { return EImpsKeyNickName; }
    else if ( aName->CompareF( KDefaultAttributeList ) == 0 ) { return EImpsKeyDefaultAttributeList; }
    else if ( aName->CompareF( KResult ) == 0 )               { return EImpsKeyResult; }
    else if ( aName->CompareF( KProperty ) == 0 )             { return EImpsKeyProperty; }
    else if ( aName->CompareF( KValue ) == 0 )                { return EImpsKeyValue; }
    else if ( aName->CompareF( KContactListProperties ) == 0 ){ return EImpsKeyContactListProperties; }
    else if ( aName->CompareF( KAddNickList ) == 0 )          { return EImpsKeyAddNickList; }
    else if ( aName->CompareF( KRemoveNickList ) == 0 )       { return EImpsKeyRemoveNickList; }
    else if ( aName->CompareF( KResult ) == 0 )               { return EImpsKeyResult; }
    else if ( aName->CompareF( KDescription ) == 0 )          { return EImpsKeyDescription; }
    else if ( aName->CompareF( KContentType ) == 0 )          { return EImpsKeyContentType; }
    else if ( aName->CompareF( KReceiveList ) == 0 )		  {	return EImpsKeyReceiveList; }
    else if ( aName->CompareF( KAutoSubscribe ) == 0 )        { return EImpsKeyAuto_Subscribe; }
    else 
        return EImpsKeyEND;
    }
// ---------------------------------------------------------
// CImpsXmlUtils::DoParseL
// ---------------------------------------------------------
void CImpsXmlUtils::DoParseL( )
    {
    iAccessKey->AddL( CREATEKEY( EImpsKeySession, KImpsBaseIndex ) );
    iAccessKey->AddL( CREATEKEY( EImpsKeyTransaction, KImpsBaseIndex ) );
    iAccessKey->AddL( CREATEKEY( EImpsKeyTransactionContent, KImpsBaseIndex ) );
    iAccessKey->AddL( CREATEKEY( EImpsKeyPureData, KImpsBaseIndex ) );

    iPosition = 0;
    TDesC8* pureData;
    if ( iImpsData->RestoreDesc8L( iAccessKey, pureData ) )
        {
        // remove pureData from key      
        iAccessKey->PopL(); 

        TInt end = pureData->Find( KTrConEnd );
        if ( end == KErrNotFound )
            {
            User::Leave( KImpsErrorValidate );
            }
        // handle trCon start
        iData.Set( pureData->Mid( iPosition ) );
        if ( iData.Find( KTrConStart ) == KErrNotFound )
            {
            User::Leave( KImpsErrorValidate );
            }
        TInt trcon = iData.Find( KEndOfTag);      
        TPtrC8 data = iData.Mid( KTrConStartLen, ( trcon - KTrConStartLen ) );
      
        SaveDataL( &data, EImpsKeyTransactionContent );
        UpdatePosition( trcon + KTagLen );
        UpdatePosition( iData.Mid( iPosition ).Find( KStartOfTag ) );

        ParseElementsL();
        }
    }
// ---------------------------------------------------------
// CImpsXmlUtils::XmlToTransactionContentL
// ---------------------------------------------------------
TInt CImpsXmlUtils::XmlToTransactionContentL( MImpsDataAccessor& aImpsData )
    {
    TInt error( KErrNone );
    iPrimitive = KErrNotFound;
    iImpsData = &aImpsData;
    iAccessKey = aImpsData.NewKeyL();

    TRAP ( error, DoParseL() )

    iAccessKey->Destroy();

    if ( error != KErrNone )
        {
        User::Leave( error );
        }
    return iPrimitive;
    }
// ---------------------------------------------------------
// CImpsXmlUtils::UpdatePosition
// ---------------------------------------------------------
void CImpsXmlUtils::UpdatePosition ( TInt aLength )
    {
    iPosition += aLength;
    if ( iPosition >= iData.Size() )
        {
        iContinue = EFalse;
        }
    }
// ---------------------------------------------------------
// CImpsXmlUtils::FindNextTag
// ---------------------------------------------------------
TInt CImpsXmlUtils::FindNextTag ( )
    {
    TPtrC8 next = iData.Mid( iPosition );
    return next.Find ( KStartOfTag );
    }
// ---------------------------------------------------------
// CImpsXmlUtils::CreateKeyL
// ---------------------------------------------------------
TInt CImpsXmlUtils::CreateKeyL( TInt aLength )
    {
    TPtrC8 elementName = iData.Mid( iPosition, aLength ); 
    
    TImpsKeyType keyType ( EImpsKeyTypeIM );
    TInt key = NameToKeyPresencePrimitives ( &elementName );
    if ( key == EImpsKeyEND )
        {
        key = NameToKeyPresenceElements( &elementName );
        if ( key != EImpsKeyPREND )
            {
            keyType = EImpsKeyTypePre;
            }
        }
        if ( key != EImpsKeyEND && key != EImpsKeyPREND )
            {
            TBool found ( ETrue );  
            TInt index ( 0 );
            while( found )
                {
                iAccessKey->AddL( CREATEKEY( key, index ), keyType );
                if ( iImpsData->CheckBranchExistenceL( iAccessKey ) )
                    {
                    iAccessKey->PopL();
                    index++;
                    }
                else
                    found = EFalse;
                }
            }      
    return key;
    }
// ---------------------------------------------------------
// CImpsXmlUtils::ParseElementsL
// ---------------------------------------------------------
void CImpsXmlUtils::ParseElementsL( )
    {
    iContinue = ETrue;
    while ( iContinue )
        {
        TPtrC8 ptr = iData.Mid( iPosition );              //  < 
        
        TInt tagEnd = ptr.Find( KEndOfTag );              //  >
        TInt emptyTag = ptr.Find( KEndTag );              //  />
        TInt elementEnd = ptr.Find( KElementTerminator ); //  </

        TImpsParseState state ( EImpsUndefined );

        if ( elementEnd == 0 )
            {
            state = EImpsEndElement;
            }
        else if ( tagEnd != KErrNotFound && emptyTag != KErrNotFound )
            {
            if ( tagEnd < emptyTag )
                state = EImpsStartElement;
            else
                state = EImpsEmptyElement;
            }
        else if ( tagEnd != KErrNotFound )
            {
            state = EImpsStartElement;      
            }
        else if ( emptyTag != KErrNotFound )
            { 
            state = EImpsEmptyElement;
            }
        else
            {
            iContinue = EFalse;
            }

        switch ( state )
            {
            case EImpsStartElement:
                {
                TInt length =  tagEnd - KTagLen;  
                UpdatePosition( KTagLen);       
                TInt key = CreateKeyL( length );
                UpdatePosition( length + KTagLen );          
                
                TInt next = FindNextTag();
                TPtrC8 data = iData.Mid( iPosition, next );

                UpdatePosition( data.Size( ) );
                SaveDataL( &data, key ); 
                }
            break;
            case EImpsEmptyElement:
                {
                TInt length = emptyTag - KTagLen; 
                UpdatePosition( KTagLen );
                TInt key = CreateKeyL( length );
                UpdatePosition( length + KEndTagLen );

                TInt next = FindNextTag();
                TPtrC8 data;
                if (next != KErrNotFound )
                    {
                    data.Set( iData.Mid( iPosition, 0 ) );
                    UpdatePosition( next );
                    SaveDataL( &data, key );
                    }
                iAccessKey->PopL();
                }
            break;
            case EImpsEndElement:
                {
                iAccessKey->PopL();           
                UpdatePosition( elementEnd + KTagLen );
                
                TInt next = FindNextTag();
                if ( next != KErrNotFound )
                    {
                    UpdatePosition( next );
                    }
                else
                    {
                    // the end
                    TInt end = iData.Mid( iPosition ).Find ( KEndOfTag );
                    if ( end != KErrNotFound )
                        {
                        UpdatePosition( end + KEndTagLen );
                        }
                    }
                }
            break;
            default:
            break;
            } 
        }
    }
// ---------------------------------------------------------
// CImpsXmlUtils::SaveDataL
// ---------------------------------------------------------
void CImpsXmlUtils::SaveDataL( TDesC8* aData, TInt aKey )  
    {
    TImpsDataType dataType = iImpsData->KeyTypeL( iAccessKey );
    
    switch ( dataType )
        {
        case EImpsDataTypeInt:
            if ( aData->Length() > 0 )
                {
                iImpsData->StoreIntegerL( iAccessKey, ConvertToIntL( aData, aKey ) );
                }
        break;
        case EImpsDataTypeDesc:
            {
            iElementValue->Des().Zero();
            TInt size = aData->Length();
            if ( size > iBufSize )
                {
                delete iElementValue;
                iElementValue = NULL;
                iElementValue = HBufC8::NewL( size + KImpsExtraSpace );
                iBufSize = size + KImpsExtraSpace;
                }
            TPtr8 ptr = iElementValue->Des();
            ptr.Copy(*aData);
            UnescapeL();

            TPtr dataPtr = iElementData->Des();
            dataPtr.Zero();
            TInt rcode = CnvUtfConverter::ConvertToUnicodeFromUtf8( dataPtr, *iElementValue );
            if ( rcode != KErrNone )
                {
                // should we leave here or not?
                }
            iImpsData->StoreDescL( iAccessKey, *iElementData );
            }
        break;
        case EImpsDataTypeDesc8: 
            {
            iElementValue->Des().Zero();
            TInt size = aData->Length();
            if ( size > iBufSize )
                {
                delete iElementValue;
                iElementValue = NULL;
                iElementValue = HBufC8::NewL( size + KImpsExtraSpace );
                iBufSize = size + KImpsExtraSpace;
                }
            TPtr8 ptr = iElementValue->Des();
            ptr.Copy(*aData);
            UnescapeL();
            iImpsData->StoreDesc8L( iAccessKey, *iElementValue );
            }
        break;
        case EImpsDataTypeBoolean: 
            if ( aData->Length() > 0 )
                {
                iImpsData->StoreBooleanL( iAccessKey, ConvertToBoolean ( aData ) );
                }
        break;
        case EImpsDataTypeNone: 
            iImpsData->StoreEmptyL( iAccessKey );
        break;
        case EImpsDataTypeNotSupported: 
            break;
        default:
            break;
        } 
    }
// ---------------------------------------------------------
// CImpsXmlUtils::ConvertToIntL
// ---------------------------------------------------------
TInt CImpsXmlUtils::ConvertToIntL( TDesC8* aData, TInt aKey )
    {
    if ( aData->CompareF( KInband ) == 0 )
        return EImpsInband;         
    else if ( aData->CompareF( KOutband ) == 0 )
        return EImpsOutband;        
    else if ( aData->CompareF( KRequest ) == 0 )
        return EImpsRequest;    
    else if ( aData->CompareF( KResponse ) == 0 )
        return EImpsResponse;       
    else if ( aData->CompareF( KURL ) == 0 )
        return EImpsCliURL;             
    else if ( aData->CompareF( KMSISDN ) == 0 )
        return EImpsCliMSISDN;      
    else if ( aData->CompareF( KPWD ) == 0 ) 
        return EImpsPWD;
    else if ( aData->CompareF( KSHA ) == 0 ) 
        return EImpsSHA;
    else if ( aData->CompareF( KMD4 ) == 0 ) 
        return EImpsMD4;
    else if ( aData->CompareF( KMD5 ) == 0 ) 
        return EImpsMD5;
    else if ( aData->CompareF( KMD6 ) == 0 )     
        return EImpsMD6;
    else if ( aData->CompareF( KMOBILE_PHONE ) == 0 )    
        return EImpsMOBILE_PHONE; 
    else if ( aData->CompareF( KCOMPUTER ) == 0 )    
        return EImpsCOMPUTER;
    else if ( aData->CompareF( KPDA ) == 0 ) 
        return EImpsPDA;
    else if ( aData->CompareF( KCLI ) == 0 ) 
        return EImpsCLI;
    else if ( aData->CompareF( KOTHER ) == 0 ) 
        return EImpsOTHER;
    else if ( aData->CompareF( KNotify ) == 0 ) 
        return EImpsN;
    else if ( aData->CompareF( KPush ) == 0 )    
        return  EImpsP;
    else if ( aData->CompareF( KSMS ) == 0 ) 
        {
        if ( aKey == EImpsKeySupportedBearer )
            return EImpsSMS;
        else
            return EImpsCapSMS;
        }
    else if ( aData->CompareF( KWSP ) == 0 ) 
        return EImpsWSP;
    else if ( aData->CompareF( KHTTP ) == 0 )    
        return EImpsHTTP;
    else if ( aData->CompareF( KHTTPS ) == 0 ) 
        return EImpsHTTPS;
    else if ( aData->CompareF( KWAPSMS ) == 0 ) 
        return EImpsWAPSMS; 
    else if ( aData->CompareF( KWAPUDP ) == 0 )  
        return EImpsWAPUDP;
    else if ( aData->CompareF( KSUDP ) == 0 )    
        return EImpsSUDP;
    else if ( aData->CompareF( KSTCP ) == 0 )    
        return EImpsSTCP;
    else if ( aData->CompareF( KUSER_ID ) == 0 ) 
        return EImpsUserID;
    else if ( aData->CompareF( KUSER_FIRST_NAME ) == 0 ) 
        return EImpsUserFirstName;
    else if ( aData->CompareF( KUSER_LAST_NAME ) == 0 )  
        return EImpsUserLastName;
    else if ( aData->CompareF( KUSER_EMAIL_ADDRESS ) == 0 )
        return EImpsUserEmailAddress;
    else if ( aData->CompareF( KUSER_ALIAS ) == 0 )
        return EImpsUserAlias;
    else if ( aData->CompareF( KUSER_ONLINE_STATUS ) == 0 )
        return EImpsUserOnlineStatus;
    else if ( aData->CompareF( KUSER_MOBILE_NUMBER ) == 0 ) 
        return EImpsUserMobileNumber;
    else if ( aData->CompareF( KGROUP_ID ) == 0 )
        return EImpsGroupID;
    else if ( aData->CompareF( KGROUP_NAME ) == 0 )
        return EImpsGroupName;
    else if ( aData->CompareF( KGROUP_TOPIC ) == 0 )
        return EImpsGroupTopic;
    else if ( aData->CompareF( KGROUP_USER_ID_JOINED ) == 0 )
        return EImpsGroupUserIDJoined;
    else if ( aData->CompareF( KGROUP_USER_ID_OWNER ) == 0 )
        return EImpsGroupUserIDOwner;
    else if ( aData->CompareF( KGR ) == 0 )
        return EImpsGR;
    else if ( aData->CompareF( KIM ) == 0 )
        return EImpsIM;
    else if ( aData->CompareF( KPR ) == 0 )
        return EImpsPR;
    else if ( aData->CompareF( KSC ) == 0 ) 
        return EImpsSC;  
    else if ( aData->CompareF( KGet ) == 0 ) 
        return EImpsG;
    else if ( aData->CompareF( KSet ) == 0 )
        return EImpsS;
    else if ( aData->CompareF( KUnset ) == 0 )
        return EImpsU;
    else if ( aData->CompareF( KNone ) == 0 )
        return EImpsNone;
    else if ( aData->CompareF( KBASE64 ) == 0 ) 
        return EImpsBASE64; 
    else if ( aData->CompareF( KIM ) == 0 )
        return EImpsCapIM;
    else if ( aData->CompareF( KCALL ) == 0 )
        return EImpsCapCALL;
    else if ( aData->CompareF( KSMS ) == 0 )
        return EImpsCapSMS;
    else if ( aData->CompareF( KMMS ) == 0 )
        return EImpsCapMMS;
    else if ( aData->CompareF( KEMAIL ) == 0 )
        return EImpsCapEMAIL;
    else if ( aData->CompareF( KAVAILABLE ) == 0 )
        return EImpsAvailable;
    else if ( aData->CompareF( KNOT_AVAILABLE ) == 0 )
        return EImpsNotAvailable;
    else if ( aData->CompareF( KDISCREET ) == 0 )
        return EImpsDiscreet;
    else if ( aData->CompareF( KOPEN ) == 0 )
        return 1;
    else if ( aData->CompareF( KCLOSED ) == 0 )
        return 2;
    else
        {       
        TLex8 lex( *aData );
        TInt converted;
        if ( KErrNone == lex.Val( converted ) )
            {
            return converted;
            }
        else
            {   
            User::Leave( KImpsErrorDecode );
            }
        // this is just for the compiler
        return KImpsErrorDecode;
        }
    }
// ---------------------------------------------------------
// CImpsXmlUtils::ConvertToBoolean
// ---------------------------------------------------------
TBool CImpsXmlUtils::ConvertToBoolean ( TDesC8* aData ) 
    {
    if ( aData->CompareF( KTrue ) == 0 )
        {
        return ETrue;
        }
    else   
        {
        return EFalse;
        }
    }
// ---------------------------------------------------------
// CImpsXmlUtils::EscapeXmlL 
// ---------------------------------------------------------
TPtrC8 CImpsXmlUtils::EscapeXmlL ( const TDesC8& aDes )
    {

    if ( iElementValue ) 
        {
        delete iElementValue;
        iElementValue = NULL;
        }

    iElementValue = aDes.AllocL();
    iBufSize = aDes.Length();

    TPtr8 ptr8a = iElementValue->Des();
    TBool rep ( EFalse );
    TInt found ( 0 );
    // count escaped characters
    for ( TInt i = 0; i < iElementValue->Length(); ++i )
        {
        TUint ch = ptr8a[ i ];
        TPtrC8 p ( iElementValue->Mid( i, 1) );
        if ( ch == '&'
            || ch == '<'
            || ch == '>'
            || ch == '"'
            || !p.Compare( KApos8 ) ) // '
            {
            ++found;
            }
        }
    // check the buffer size
    TInt len = iElementValue->Length() + found * 5;
    if ( len > iBufSize )
        {
            iElementValue = iElementValue->ReAllocL( len );
            iBufSize = len;
        }
    TPtr8 ptr8 = iElementValue->Des();

    // escape &
    // this is special case because the
    // escaping literal contains '&'
    found = 0;
    while ( found != KErrNotFound )
        {
        found = ptr8.Locate('&');
        if ( found != KErrNotFound )
            {
            ptr8.Replace( found, 1, KReplace );
            rep = ETrue;
            }
        }
    found = 0;
    if ( rep )
        {
        while ( found != KErrNotFound )
            {
            found = ptr8.Find( KReplace );
            if ( found != KErrNotFound )
                {
                ptr8.Replace( found, 5, KAndRep8 );
                }
            }
        }
    // escape <
    found = 0;
    while ( found != KErrNotFound )
        {
        found = ptr8.Locate('<');
        if ( found != KErrNotFound )
            {
            ptr8.Replace( found, 1, KltRep8 );
            }
        }
    // escape >
    found = 0;
    while ( found != KErrNotFound )
        {
        found = ptr8.Locate('>');
        if ( found != KErrNotFound )
            {
            ptr8.Replace( found, 1, KgtRep8 );
            }
        }
    // escape "
    found = 0;
    while ( found != KErrNotFound )
        {
        found = ptr8.Locate('"');
        if ( found != KErrNotFound )
            {
            ptr8.Replace( found, 1, KQuotRep8 );
            }
        }
    // escape '
    found = 0;
    while ( found != KErrNotFound )
        {
        found = ptr8.Find( KApos8);
        if ( found != KErrNotFound )
            {
            ptr8.Replace( found, 1, KAposRep8 );
            }
        }    
    return ptr8;
    }
// ---------------------------------------------------------
// CImpsXmlUtils::ReplaceXmlSpecialCharacters ()
// ---------------------------------------------------------
void CImpsXmlUtils::UnescapeL()
    {
    TPtr8 ptr = iElementValue->Des();
    // &amp;
    TInt found ( 0 );
    while ( found != KErrNotFound )
        {
        found = ptr.Find( KAndRep8 );
        if ( found != KErrNotFound )
            ptr.Replace( found, 5, KAnd8 );
        }
    // &lt;
    found = 0;
    while ( found != KErrNotFound )
        {
        found = ptr.Find( KltRep8 );
        if ( found != KErrNotFound )
            ptr.Replace( found, 4, Klt8 );
        }
    // &gt;
    found = 0;
    while ( found != KErrNotFound )
        {
        found = ptr.Find( KgtRep8 );
        if ( found != KErrNotFound )
            ptr.Replace( found, 4, Kgt8 );
        }
    // &quot;
    found = 0;
    while ( found != KErrNotFound )
        {
        found = ptr.Find( KQuotRep8 );
        if ( found != KErrNotFound )
            ptr.Replace( found, 6, KQuot8 );
        }
    // &apos;
    found = 0;
    while ( found != KErrNotFound)
        {
        found = ptr.Find(KAposRep8);
        if ( found != KErrNotFound )
            ptr.Replace( found, 4, KApos8 );
        }
    }

