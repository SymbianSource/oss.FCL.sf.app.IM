/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SAP settings container body.
*
*/

// INCLUDE FILES
#include    "CIMPSSAPSettingsBody.h"
#include	"CIMPSSAPKeyValuePair.h"
#include    "IMPSSAPProtectionHandler.h"
#include    "IMPSSAPSerializer.h"
#include    "IMPSSAPSettingsStorePanics.h"

#include    <e32std.h>
#include    <s32mem.h>

// ================= LOCAL FUNCTIONS =======================


// -----------------------------------------------------------------------------
// ReleaseBuffer()
// Deletes given buffer and sets the pointer NULL.
//
// @param aBuffer The buffer to delete.
// -----------------------------------------------------------------------------
//
void ReleaseBuffer( HBufC*& aBuffer )
    {
    delete aBuffer;
    aBuffer = NULL;
    }


// -----------------------------------------------------------------------------
// DoGet()
// Returns given buffer or KNullDesC if the buffer was NULL..
//
// @param aBuffer The buffer to get.
// -----------------------------------------------------------------------------
//
const TDesC& DoGet( const HBufC* const & aBuffer )
    {
    if ( aBuffer )
        {
        return *aBuffer;
        }
    return KNullDesC;
    }


// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsBody* CIMPSSAPSettingsBody::NewL()
    {
    CIMPSSAPSettingsBody* self = new ( ELeave ) CIMPSSAPSettingsBody;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::~CIMPSSAPSettingsBody()
// Destructor
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsBody::~CIMPSSAPSettingsBody()
    {
    delete iClientId;
    delete iSAPName;
    delete iSAPAddress;
    delete iSAPUserId;
    delete iSAPUserPassword;
    delete iHTTPProxyAddress;
    delete iHTTPProxyUserId;
    delete iHTTPProxyUserPassword;
    delete iServerAuthenticationName;
    delete iServerAuthenticationPassword;
    delete iServerAcceptedContentType;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::CIMPSSAPSettingsBody()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsBody::CIMPSSAPSettingsBody()
    {
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::ConstructL)
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::ConstructL()
    {
    }



// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::ObjectID()
// From generic object interface.
// -----------------------------------------------------------------------------
//
TIMPSSapObjectID CIMPSSAPSettingsBody::ObjectID() const
    {
    return EIMPSSapObjSapSettings;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::ObjectVersion()
// From generic object interface.
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPSettingsBody::ObjectVersion() const
    {
    return KIMPSSapObjVersionInitial;
    }



// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::ExternalizeVersionDataL()
// From generic object interface.
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::ExternalizeVersionDataL( TInt aVersion,
                                                    RWriteStream& aStream ) const
    {
    switch ( aVersion )
        {
        case  KIMPSSapObjVersionInitial:
            {
            //ID's
            aStream.WriteInt32L( iAccessGroup );

            aStream.WriteUint32L( iUID );

            aStream.WriteUint32L( iAP );
            IMPSSAPSerializer::ExternalizeBufferL( iClientId, aStream );

            aStream.WriteUint32L( iProtection );
            aStream.WriteUint32L( iLoginCount );

            //Address
            IMPSSAPSerializer::ExternalizeBufferL( iSAPName, aStream );
            aStream.WriteInt32L( iSAPPort );
            IMPSSAPSerializer::ExternalizeBufferL( iSAPAddress, aStream );
            IMPSSAPSerializer::ExternalizeBufferL( iSAPUserId, aStream );
            IMPSSAPSerializer::ExternalizeBufferL( iSAPUserPassword, aStream );

            //Proxy
            IMPSSAPSerializer::ExternalizeBufferL( iHTTPProxyAddress, aStream );
            aStream.WriteInt32L( iHTTPProxyPort );
            IMPSSAPSerializer::ExternalizeBufferL( iHTTPProxyUserId, aStream );
            IMPSSAPSerializer::ExternalizeBufferL( iHTTPProxyUserPassword, aStream );

            // High level services

            aStream.WriteInt32L( iHighLevelServices );

            aStream.WriteInt32L( iAuthorizationMode );

            // Server specific
            IMPSSAPSerializer::ExternalizeBufferL( iServerAuthenticationName, aStream );
            IMPSSAPSerializer::ExternalizeBufferL( iServerAuthenticationPassword, aStream );
            IMPSSAPSerializer::ExternalizeBufferL( iServerAcceptedContentType, aStream );

            break;
            }

        default:
            {
            //Unknown version ID in externalization
            Panic( EIMPSSetStoreUnknownObjectVersionExternalize );
            break;
            }
        };
    }





// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::InternalizeVersionDataL()
// From generic object interface.
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::InternalizeVersionDataL( TInt aVersion,
                                                    RReadStream& aStream )
    {
    Reset();
    switch ( aVersion )
        {
        case  KIMPSSapObjVersionInitial:
            {

            //IDs
            iAccessGroup = static_cast<TIMPSAccessGroup>( aStream.ReadUint32L() );

            iUID = aStream.ReadUint32L();

            iAP = aStream.ReadUint32L();
            iClientId = IMPSSAPSerializer::InternalizeBufferL( aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iClientId );

            iProtection = static_cast<TIMPSSAPProtection> ( aStream.ReadUint32L() );
            IMPSSAPProtectionHandler::ChechkIsProperProtectionLevelL( iProtection );

            iLoginCount = aStream.ReadUint32L();

            //Address
            iSAPName = IMPSSAPSerializer::InternalizeBufferL(  aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iSAPName );

            iSAPPort = aStream.ReadInt32L();
            iSAPAddress = IMPSSAPSerializer::InternalizeBufferL( aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iSAPAddress );

            iSAPUserId = IMPSSAPSerializer::InternalizeBufferL( aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iSAPUserId );

            iSAPUserPassword = IMPSSAPSerializer::InternalizeBufferL( aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iSAPUserPassword );

            //Proxy
            iHTTPProxyAddress = IMPSSAPSerializer::InternalizeBufferL( aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iHTTPProxyAddress );
            iHTTPProxyPort = aStream.ReadInt32L();
            iHTTPProxyUserId = IMPSSAPSerializer::InternalizeBufferL( aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iHTTPProxyUserId );
            iHTTPProxyUserPassword = IMPSSAPSerializer::InternalizeBufferL( aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iHTTPProxyUserPassword );

            //High level services
            iHighLevelServices = aStream.ReadInt32L();

            //Authorization mode
            iAuthorizationMode = aStream.ReadInt32L();

            //Server specific
            iServerAuthenticationName = IMPSSAPSerializer::InternalizeBufferL( aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iServerAuthenticationName );
            iServerAuthenticationPassword = IMPSSAPSerializer::InternalizeBufferL( aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iServerAuthenticationPassword );
            iServerAcceptedContentType = IMPSSAPSerializer::InternalizeBufferL( aStream );
            iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( iServerAcceptedContentType );

            break;
            }

        default:
            {
            //Unknown version ID in internalization.
            Panic( EIMPSSetStoreUnknownObjectVersionInternalize );
            break;
            }
        };
    }



// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::DataSize()
// From generic object interface.
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPSettingsBody::DataSize() const
    {
    /*
    Integers/enums
    - iUID
    - iAP
    - iProtection
    - iLoginCount
    - iSAPPort
    - iHTTPProxyPort
    - iHighLevelServices
    - iAccessGroup
    - iAuthorizationMode

    Buffer lengths
    - iClientID
    - iSAPName
    - iSAPAddress
    - iSAPUserId
    - iSAPUserPassword
    - iHTTPProxyAddress
    - iHTTPProxyUserId
    - iHTTPProxyUserPassword
    - iServerAuthenticationName
    - iServerAuthenticationPassword
    - iServerAcceptedContentType

    ==> (9+11) * 4 bytes ==> 160 bytes
    */

    return iDynamicDataSize + 160;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::Reset()
// From generic object interface.
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::Reset()
    {
    iDynamicDataSize = 0;
    iUID = 0;

    iAP = 0;
    ReleaseBuffer( iClientId );

    iProtection = ESAPNoProtection;
    iLoginCount = 0;

    ReleaseBuffer( iSAPName );
    iSAPPort = 0;
    ReleaseBuffer( iSAPAddress );
    ReleaseBuffer( iSAPUserId );
    ReleaseBuffer( iSAPUserPassword );

    ReleaseBuffer( iHTTPProxyAddress );
    iHTTPProxyPort = 0;
    ReleaseBuffer( iHTTPProxyUserId );
    ReleaseBuffer( iHTTPProxyUserPassword );
    iHighLevelServices = 0;
    ReleaseBuffer( iServerAuthenticationName );
    ReleaseBuffer( iServerAuthenticationPassword );
    ReleaseBuffer( iServerAcceptedContentType );
    iAccessGroup = EIMPSNoAccessGroup;
    iAuthorizationMode = EIMPSAuthModeUnspecified;
    }



// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::Uid()
// -----------------------------------------------------------------------------
//
TUint32 CIMPSSAPSettingsBody::Uid() const
    {
    return iUID;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SAPName()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::SAPName() const
    {
    return DoGet( iSAPName );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SAPAddress()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::SAPAddress() const
    {
    return DoGet( iSAPAddress );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SAPPort()
// -----------------------------------------------------------------------------
//
TInt32 CIMPSSAPSettingsBody::SAPPort() const
    {
    return iSAPPort;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SAPUserId()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::SAPUserId() const
    {
    return DoGet( iSAPUserId );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SAPUserPassword()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::SAPUserPassword() const
    {
    return DoGet( iSAPUserPassword );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::HTTPProxyAddress()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::HTTPProxyAddress() const
    {
    return DoGet( iHTTPProxyAddress );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::HTTPProxyPort()
// -----------------------------------------------------------------------------
//
TInt32 CIMPSSAPSettingsBody::HTTPProxyPort() const
    {
    return iHTTPProxyPort;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::HTTPProxyUserId()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::HTTPProxyUserId() const
    {
    return DoGet( iHTTPProxyUserId );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::HTTPProxyUserPassword()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::HTTPProxyUserPassword() const
    {
    return DoGet( iHTTPProxyUserPassword );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::AccessPoint()
// -----------------------------------------------------------------------------
//
TUint32 CIMPSSAPSettingsBody::AccessPoint() const
    {
    return iAP;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::ClientId()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::ClientId() const
    {
    return DoGet( iClientId );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::HighLevelServices()
// -----------------------------------------------------------------------------
//
TUint32 CIMPSSAPSettingsBody::HighLevelServices() const
    {
    return iHighLevelServices;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::ServerAuthenticationName()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::ServerAuthenticationName() const
    {
    return DoGet( iServerAuthenticationName );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::ServerAuthenticationPassword()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::ServerAuthenticationPassword() const
    {
    return DoGet( iServerAuthenticationPassword );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::ServerAcceptedContentType()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPSettingsBody::ServerAcceptedContentType() const
    {
    return DoGet( iServerAcceptedContentType );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::AccessGroup()
// -----------------------------------------------------------------------------
//
TIMPSAccessGroup CIMPSSAPSettingsBody::AccessGroup() const
    {
    return iAccessGroup;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::AuthorizationMode()
// -----------------------------------------------------------------------------
//
TUint32 CIMPSSAPSettingsBody::AuthorizationMode() const
    {
    return iAuthorizationMode;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetSAPNameL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetSAPNameL( const TDesC& aName )
    {
    DoSetBufferL( iSAPName, aName, EOperationSetSAPName, EFalse );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetSAPAddressL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetSAPAddressL( const TDesC& aAddress )
    {
    DoSetBufferL( iSAPAddress, aAddress, EOperationSetSAPAddress, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetSAPPort()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPSettingsBody::SetSAPPort( TInt32 aPort )
    {
    return DoSetNumber( iSAPPort, aPort, EOperationSetSAPPort, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetSAPUserIdL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetSAPUserIdL( const TDesC& aUserId )
    {
    DoSetBufferL( iSAPUserId, aUserId, EOperationSetSAPUserId, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetSAPUserPasswordL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetSAPUserPasswordL( const TDesC& aPassword )
    {
    DoSetBufferL( iSAPUserPassword, aPassword, EOperationSetSAPUserPassword, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetHTTPProxyAddressL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetHTTPProxyAddressL( const TDesC& aAddress )
    {
    DoSetBufferL( iHTTPProxyAddress, aAddress, EOperationSetHTTPProxyAddress, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetHTTPProxyPort()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPSettingsBody::SetHTTPProxyPort( TInt32 aPort )
    {
    return DoSetNumber( iHTTPProxyPort, aPort, EOperationSetHTTPProxyPort, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetHTTPProxyUserIdL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetHTTPProxyUserIdL( const TDesC& aUserId )
    {
    DoSetBufferL( iHTTPProxyUserId, aUserId, EOperationSetHTTPProxyUserId, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetHTTPProxyUserPasswordL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetHTTPProxyUserPasswordL( const TDesC& aPassword )
    {
    DoSetBufferL( iHTTPProxyUserPassword, aPassword, EOperationSetHTTPProxyUserPassword, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetAccessPoint()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPSettingsBody::SetAccessPoint( TUint32 aAP )
    {
    return DoSetNumber( iAP, aAP, EOperationSetAP, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetClientIdL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetClientIdL( const TDesC& aClientId )
    {
    DoSetBufferL( iClientId, aClientId, EOperationSetClientId, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetHighLevelServices()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPSettingsBody::SetHighLevelServices( TUint32 aHighLevelServices )
    {
    return DoSetNumber( iHighLevelServices, aHighLevelServices, EOperationSetHighLevelServices, ETrue );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetServerAuthenticationNameL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetServerAuthenticationNameL( const TDesC& aServerAuthenticationName )
    {
    DoSetBufferL( iServerAuthenticationName, aServerAuthenticationName, EOperationSetServerAuthenticationName, ETrue );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetServerAuthenticationPasswordL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetServerAuthenticationPasswordL( const TDesC& aServerAuthenticationPassword )
    {
    DoSetBufferL( iServerAuthenticationPassword, aServerAuthenticationPassword, EOperationSetServerAuthenticationPassword, ETrue );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetServerAcceptedContentTypeL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::SetServerAcceptedContentTypeL( const TDesC& aServerAcceptedContentType )
    {
    DoSetBufferL( iServerAcceptedContentType, aServerAcceptedContentType, EOperationSetServerAcceptedContentType, ETrue );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::SetAuthorizationMode()
// -----------------------------------------------------------------------------
//

TInt CIMPSSAPSettingsBody::SetAuthorizationMode( TUint32 aMode )
    {
    return DoSetNumber( iAuthorizationMode, aMode, EOperationSetAuthorizationMode, ETrue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::ProtectL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::ProtectL( TIMPSSAPProtection aProtection )
    {
    IMPSSAPProtectionHandler::ValidateOperationL( iProtection, EOperationSetProtection );
    IMPSSAPProtectionHandler::ChechkIsProperProtectionLevelL( aProtection );
    iProtection = aProtection;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::Protection()
// -----------------------------------------------------------------------------
//
TIMPSSAPProtection CIMPSSAPSettingsBody::Protection() const
    {
    return iProtection;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::IncreaseLoginCount()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::IncreaseLoginCount()
    {
    iLoginCount++;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::LoginCount()
// -----------------------------------------------------------------------------
//
TUint32 CIMPSSAPSettingsBody::LoginCount() const
    {
    return iLoginCount;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::EqualConnectionSettings()
// -----------------------------------------------------------------------------
//
TBool CIMPSSAPSettingsBody::EqualConnectionSettings( const CIMPSSAPSettingsBody& aOther ) const
    {
    TInt result;

    result = iSAPAddress && iSAPAddress->Compare( aOther.SAPAddress() );
    result |= iHTTPProxyAddress && iHTTPProxyAddress->Compare( aOther.HTTPProxyAddress() );
    result |= !( iSAPPort == aOther.SAPPort() );
    result |= !( iHTTPProxyPort == aOther.HTTPProxyPort() );

    return !result;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::EqualUserSettings()
// -----------------------------------------------------------------------------
//
TBool CIMPSSAPSettingsBody::EqualUserSettings( const CIMPSSAPSettingsBody& aOther ) const
    {
    TInt result;

    result = iSAPUserId && iSAPUserId->Compare( aOther.SAPUserId() );
    result |= iSAPUserPassword && iSAPUserPassword->Compare( aOther.SAPUserPassword() );
    result |= iHTTPProxyUserId && iHTTPProxyUserId->Compare( aOther.HTTPProxyUserId() );
    result |= iHTTPProxyUserPassword && iHTTPProxyUserPassword->Compare( aOther.HTTPProxyUserPassword() );

    return !result;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::DoSetLoginCount()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::DoSetLoginCount( TUint32 aLoginCount )
    {
    iLoginCount = aLoginCount;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::DoResetLoginCount()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::DoResetLoginCount()
    {
    iLoginCount = 0;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::DoResetProtection()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::DoResetProtection()
    {
    iProtection = ESAPNoProtection;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::DoSetUid()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::DoSetUid( TUint32 aUid )
    {
    iUID = aUid;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::DoSetAccessGroup()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::DoSetAccessGroup( TIMPSAccessGroup aGroup )
    {
    iAccessGroup = aGroup;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::DoSetBufferL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPSettingsBody::DoSetBufferL( HBufC*& aPtr, const TDesC& aData, TInt aOperation, TBool aAffectsToLoginCount )
    {
    IMPSSAPProtectionHandler::ValidateOperationL( iProtection, aOperation );

    HBufC* tmp = aData.AllocL(); //alloc first the new data

    if ( aAffectsToLoginCount )
        {
        if ( !aPtr ||
             ( aPtr && ( aPtr->Compare( aData ) != 0 ) ) )
            {
            iLoginCount = 0;
            }
        }

    //calculate dynamic data size change
    iDynamicDataSize -= IMPSSAPSerializer::BufferSizeInBytes( aPtr );
    iDynamicDataSize += IMPSSAPSerializer::BufferSizeInBytes( tmp );

    //assign the new value
    delete aPtr;
    aPtr = tmp;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsBody::DoSetNumber()
// -----------------------------------------------------------------------------
//
template< class T >
TInt CIMPSSAPSettingsBody::DoSetNumber( T& aOldData,
                                        T aData,
                                        TInt aOperation,
                                        TBool aAffectsToLoginCount )
    {
    TInt err = IMPSSAPProtectionHandler::ValidateOperation( iProtection, aOperation );
    if ( err != KErrNone )
        {
        return err;
        }

    if ( aAffectsToLoginCount )
        {
        if ( aOldData != aData )
            {
            iLoginCount = 0;
            }
        }

    aOldData = aData;

    return KErrNone;
    }




//  End of File


