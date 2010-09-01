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
* Description:  SAP settings container.
*
*/

// INCLUDE FILES
#include    <e32std.h>
#include	<centralrepository.h>
#include    <s32mem.h>

#include    "cimpssapsettings.h"
#include    "CIMPSSAPSettingsBody.h"
#include    "CIMPSSAPKeyValuePairs.h"
#include    "CIMPSSAPKeyValuePair.h"

#include    "IMPSSAPObjectHandler.h"
#include	"IMPSSAPSettingsStoreDefinitions.h"
#include	"IMPSSAPSettingsStorePanics.h"


// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CIMPSSAPSettings::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettings* CIMPSSAPSettings::NewL()
    {
    CIMPSSAPSettings* self = CIMPSSAPSettings::NewLC();
    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettings* CIMPSSAPSettings::NewLC()
    {
    CIMPSSAPSettings* self = new ( ELeave ) CIMPSSAPSettings();
    CleanupStack::PushL( self );
    self->iKeyValuePairs = CIMPSSAPKeyValuePairs::NewL();
    self->iBody = CIMPSSAPSettingsBody::NewL();
    return self;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::~CIMPSSAPSettings()
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettings::~CIMPSSAPSettings()
    {
    delete iBody;
    delete iKeyValuePairs;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::CIMPSSAPSettings()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSAPSettings::CIMPSSAPSettings()
    {
    }





// -----------------------------------------------------------------------------
// CIMPSSAPSettings::Uid()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIMPSSAPSettings::Uid() const
    {
    return iBody->Uid();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SAPName()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::SAPName() const
    {
    return iBody->SAPName();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SAPAddress()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::SAPAddress() const
    {
    return iBody->SAPAddress();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SAPPort()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CIMPSSAPSettings::SAPPort() const
    {
    return iBody->SAPPort();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SAPUserId()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::SAPUserId() const
    {
    return iBody->SAPUserId();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SAPUserPassword()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::SAPUserPassword() const
    {
    return iBody->SAPUserPassword();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::HTTPProxyAddress()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::HTTPProxyAddress() const
    {
    return iBody->HTTPProxyAddress();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::HTTPProxyPort()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CIMPSSAPSettings::HTTPProxyPort() const
    {
    return iBody->HTTPProxyPort();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::HTTPProxyUserId()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::HTTPProxyUserId() const
    {
    return iBody->HTTPProxyUserId();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::HTTPProxyUserPassword()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::HTTPProxyUserPassword() const
    {
    return iBody->HTTPProxyUserPassword();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::AccessPoint()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIMPSSAPSettings::AccessPoint() const
    {
    return iBody->AccessPoint();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::ClientId()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::ClientId() const
    {
    return iBody->ClientId();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::ServerAuthenticationName()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::ServerAuthenticationName() const
    {
    return iBody->ServerAuthenticationName();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::ServerAuthenticationPassword()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::ServerAuthenticationPassword() const
    {
    return iBody->ServerAuthenticationPassword();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::ServerAcceptedContentType()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettings::ServerAcceptedContentType() const
    {
    return iBody->ServerAcceptedContentType();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::HighLevelServices()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIMPSSAPSettings::HighLevelServices() const
    {
    return iBody->HighLevelServices();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetSAPNameL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetSAPNameL( const TDesC& aName )
    {
    iBody->SetSAPNameL( aName );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetSAPAddressL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetSAPAddressL( const TDesC& aAddress )
    {
    iBody->SetSAPAddressL( aAddress );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetSAPPort()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::SetSAPPort( TInt32 aPort )
    {
    return iBody->SetSAPPort( aPort );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetSAPUserIdL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetSAPUserIdL( const TDesC& aUserId )
    {
    iBody->SetSAPUserIdL( aUserId );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetSAPUserPasswordL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetSAPUserPasswordL( const TDesC& aPassword )
    {
    iBody->SetSAPUserPasswordL( aPassword );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetHTTPProxyAddressL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetHTTPProxyAddressL( const TDesC& aAddress )
    {
    iBody->SetHTTPProxyAddressL( aAddress );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetHTTPProxyPort()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::SetHTTPProxyPort( TInt32 aPort )
    {
    return iBody->SetHTTPProxyPort( aPort );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetHTTPProxyUserIdL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetHTTPProxyUserIdL( const TDesC& aUserId )
    {
    iBody->SetHTTPProxyUserIdL( aUserId );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetHTTPProxyUserPasswordL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetHTTPProxyUserPasswordL( const TDesC& aPassword )
    {
    iBody->SetHTTPProxyUserPasswordL( aPassword );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetAccessPoint()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::SetAccessPoint( TUint32 aAP )
    {
    return iBody->SetAccessPoint( aAP );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetClientIdL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetClientIdL( const TDesC& aClientId )
    {
    iBody->SetClientIdL( aClientId );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetServerAuthenticationNameL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetServerAuthenticationNameL( const TDesC& aClientId )
    {
    iBody->SetServerAuthenticationNameL( aClientId );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetServerAuthenticationPasswordL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetServerAuthenticationPasswordL( const TDesC& aClientId )
    {
    iBody->SetServerAuthenticationPasswordL( aClientId );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetServerAcceptedContentTypeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetServerAcceptedContentTypeL( const TDesC& aClientId )
    {
    iBody->SetServerAcceptedContentTypeL( aClientId );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetHighLevelServices()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::SetHighLevelServices( TUint32 aServices )
    {
    return iBody->SetHighLevelServices( aServices );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetAuthorizationMode()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::SetAuthorizationMode( TUint32 aMode )
    {
    return iBody->SetAuthorizationMode( aMode );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::ProtectL()
// Protection setters/getters.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::ProtectL( TIMPSSAPProtection aProtection )
    {
    iBody->ProtectL( aProtection );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::Protection()
// Protection setters/getters.
// -----------------------------------------------------------------------------
//
EXPORT_C TIMPSSAPProtection CIMPSSAPSettings::Protection() const
    {
    return iBody->Protection();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::IncreaseLoginCoun()
// Utility method.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::IncreaseLoginCount()
    {
    iBody->IncreaseLoginCount();
    }



// -----------------------------------------------------------------------------
// CIMPSSAPSettings::LoginCount()
// Utility method.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIMPSSAPSettings::LoginCount() const
    {
    return iBody->LoginCount();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetOpaqueInt()
// Key-value pairs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::SetOpaqueInt( const TDesC& aKey, TInt aValue )
    {
    __ASSERT_ALWAYS( aKey.Find( KKeyValuePairFieldSeparator ) == KErrNotFound,
                     Panic( EIMPSInvalidKeyValue ) );

    TInt err = iKeyValuePairs->SetValueInt( aKey, aValue );
    if ( err == KErrNone )
        {
        iBody->DoResetLoginCount();
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetOpaqueDesC8()
// Key-value pairs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::SetOpaqueDesC8( const TDesC& aKey,
                                                const TDesC8& aValue )
    {
    __ASSERT_ALWAYS( aKey.Find( KKeyValuePairFieldSeparator ) == KErrNotFound,
                     Panic( EIMPSInvalidKeyValue ) );

    TInt err = iKeyValuePairs->SetValueDesC8( aKey, aValue );
    if ( err == KErrNone )
        {
        iBody->DoResetLoginCount();
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetOpaqueDesC16()
// Key-value pairs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::SetOpaqueDesC16( const TDesC& aKey,
                                                 const TDesC16& aValue )
    {
    __ASSERT_ALWAYS( aKey.Find( KKeyValuePairFieldSeparator ) == KErrNotFound,
                     Panic( EIMPSInvalidKeyValue ) );

    TInt err = iKeyValuePairs->SetValueDesC16( aKey, aValue );
    if ( err == KErrNone )
        {
        iBody->DoResetLoginCount();
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::GetOpaqueInt()
// Key-value pairs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::GetOpaqueInt( const TDesC& aKey, TInt& aValue ) const
    {
    return iKeyValuePairs->GetValueInt( aKey, aValue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::GetOpaqueDesC8()
// Key-value pairs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::GetOpaqueDesC8( const TDesC& aKey, TPtrC8& aValue ) const
    {
    return iKeyValuePairs->GetValueDesC8( aKey, aValue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::GetOpaqueDesC16()
// Key-value pairs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::GetOpaqueDesC16( const TDesC& aKey,
                                                 TPtrC16& aValue ) const
    {
    return iKeyValuePairs->GetValueDesC16( aKey, aValue );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::DeleteOpaque()
// Key-value pairs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::DeleteOpaque( const TDesC& aKey )
    {
    return iKeyValuePairs->DeletePair( aKey );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::EqualConnectionSettings()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIMPSSAPSettings::EqualConnectionSettings( const CIMPSSAPSettings& aOther ) const
    {
    return iBody->EqualConnectionSettings( aOther.Body() );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::EqualUserSettings()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIMPSSAPSettings::EqualUserSettings( const CIMPSSAPSettings& aOther ) const
    {
    return iBody->EqualUserSettings( aOther.Body() );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::Reset()
// Utility method.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::Reset()
    {
    iBody->Reset();
    iKeyValuePairs->Reset();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::CopyL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::CopyL( const CIMPSSAPSettings& aSource )
    {
    IMPSSAPObjectHandler::CopyL( aSource.Body(), *iBody );
    IMPSSAPObjectHandler::CopyL( *aSource.iKeyValuePairs, *iKeyValuePairs );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::AccessGroup()
// -----------------------------------------------------------------------------
//

EXPORT_C TIMPSAccessGroup CIMPSSAPSettings::AccessGroup() const
    {
    return iBody->AccessGroup();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::SetAccessGroup()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::SetAccessGroup( TIMPSAccessGroup aGroup )
    {
    iBody->DoSetAccessGroup( aGroup );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::AuthorizationMode()
// -----------------------------------------------------------------------------
//

EXPORT_C TUint32 CIMPSSAPSettings::AuthorizationMode() const
    {
    return iBody->AuthorizationMode();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::ExternalizeL()
// Streaming support
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::ExternalizeL( RWriteStream& aStream ) const
    {
    IMPSSAPObjectHandler::ExternalizeL( *iBody, aStream );
    IMPSSAPObjectHandler::ExternalizeL( *iKeyValuePairs, aStream );
    }



// -----------------------------------------------------------------------------
// CIMPSSAPSettings::InternalizeL()
// Streaming support
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::InternalizeL( RReadStream& aStream )
    {
    IMPSSAPObjectHandler::InternalizeL( *iBody, aStream );
    IMPSSAPObjectHandler::InternalizeL( *iKeyValuePairs, aStream );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettings::DataSize()
// Streaming support
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettings::DataSize() const
    {
    return IMPSSAPObjectHandler::DataSize( *iBody ) +
           IMPSSAPObjectHandler::DataSize( *iKeyValuePairs );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::PackLC()
// -----------------------------------------------------------------------------
//

EXPORT_C HBufC8* CIMPSSAPSettings::PackLC() const
    {
    HBufC8* packBuffer = HBufC8::NewLC( DataSize() );
    TPtr8 pack( packBuffer->Des() );

    RDesWriteStream ws;
    ws.Open( pack );	// CSI: 65 #
    // This Open() returns void
    CleanupClosePushL( ws );

    ExternalizeL( ws );

    ws.CommitL();
    CleanupStack::PopAndDestroy(); //ws

    packBuffer = packBuffer->ReAllocL( packBuffer->Length() );
    CleanupStack::Pop();
    CleanupStack::PushL( packBuffer ); //Due realloc

    return packBuffer;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::UnPackL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettings::UnPackL( const TDesC8& aPackage )
    {
    RDesReadStream rs;
    rs.Open( aPackage );	// CSI: 65 #
    // This Open() returns void
    CleanupClosePushL( rs );

    InternalizeL( rs );

    CleanupStack::PopAndDestroy(); //rs
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::Body()
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsBody& CIMPSSAPSettings::Body()
    {
    return *iBody;
    }

const CIMPSSAPSettingsBody& CIMPSSAPSettings::Body() const
    {
    return *iBody;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettings::KeyValuePairs()
// -----------------------------------------------------------------------------
//
CIMPSSAPKeyValuePairs& CIMPSSAPSettings::KeyValuePairs()
    {
    return *iKeyValuePairs;
    }

const CIMPSSAPKeyValuePairs& CIMPSSAPSettings::KeyValuePairs() const
    {
    return *iKeyValuePairs;
    }



//  End of File

