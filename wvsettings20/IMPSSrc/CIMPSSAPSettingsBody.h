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

#ifndef __CIMPSSAPSETTINGSBODY_H
#define __CIMPSSAPSETTINGSBODY_H

//  INCLUDES
#include "MIMPSSapObject.h"
#include "CIMPSSAPKeyValuePairs.h"


#include <e32base.h>
#include <s32strm.h>
#include "impssapsettings.h"


// CLASS DECLARATION
/**
 * SAP settings container body.
 */
NONSHARABLE_CLASS( CIMPSSAPSettingsBody ): public CBase,
        public MIMPSSapObject
    {

public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @since 1.2
     */
    static CIMPSSAPSettingsBody* NewL();

    /**
     * Destructor.
     */
    virtual ~CIMPSSAPSettingsBody();


protected:  // Constructors

    /**
     * C++ default constructor.
     */
    CIMPSSAPSettingsBody();


    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public: //Generic object interface from MIMPSSapObject

    TIMPSSapObjectID ObjectID() const;
    TInt ObjectVersion() const;
    void ExternalizeVersionDataL( TInt aVersion, RWriteStream& aStream ) const;
    void InternalizeVersionDataL( TInt aVersion, RReadStream& aStream );
    TInt DataSize() const;
    void Reset();

public: // New functions

    /**
     * @see CIMPSSAPSettings.
     * @since 3.0
     */
    TUint32 Uid() const;
    const TDesC& SAPName() const;
    const TDesC& SAPAddress() const;
    TInt32 SAPPort() const;
    const TDesC& SAPUserId() const;
    const TDesC& SAPUserPassword() const;
    const TDesC& HTTPProxyAddress() const;
    TInt32 HTTPProxyPort() const;
    const TDesC& HTTPProxyUserId() const;
    const TDesC& HTTPProxyUserPassword() const;
    TUint32 AccessPoint() const;
    const TDesC& ClientId() const;
    TUint32 HighLevelServices() const;
    const TDesC& ServerAuthenticationName() const;
    const TDesC& ServerAuthenticationPassword() const;
    const TDesC& ServerAcceptedContentType() const;
    TIMPSAccessGroup AccessGroup() const;
    TUint32 AuthorizationMode() const;

    /**
     * @see CIMPSSAPSettings.
     * @since 3.0
     */
    void SetSAPNameL( const TDesC& aName );
    void SetSAPAddressL( const TDesC& aAddress );
    TInt SetSAPPort( TInt32 aPort );
    void SetSAPUserIdL( const TDesC& aUserId );
    void SetSAPUserPasswordL( const TDesC& aPassword );
    void SetHTTPProxyAddressL( const TDesC& aAddress );
    TInt SetHTTPProxyPort( TInt32 aPort );
    void SetHTTPProxyUserIdL( const TDesC& UserId );
    void SetHTTPProxyUserPasswordL( const TDesC& aPassword );
    TInt SetAccessPoint( TUint32 aAP );
    void SetClientIdL( const TDesC& aClientId );
    TInt SetHighLevelServices( TUint32 aServices );
    void SetServerAuthenticationNameL( const TDesC& aAuthenticationName );
    void SetServerAuthenticationPasswordL( const TDesC& aAuthenticationPassword );
    void SetServerAcceptedContentTypeL( const TDesC& aAcceptedContentType );
    TInt SetAuthorizationMode( TUint32 aMode );

    /**
     * @see CIMPSSAPSettings.
     * @since 3.0
     */
    void ProtectL( TIMPSSAPProtection aProtection );
    TIMPSSAPProtection Protection() const;
    void IncreaseLoginCount();
    TUint32 LoginCount() const;
    TBool EqualConnectionSettings( const CIMPSSAPSettingsBody& aOther ) const;
    TBool EqualUserSettings( const CIMPSSAPSettingsBody& aOther ) const;


public: // Internal setters and helpers

    /**
     * Setters for internal usage.
     * @see CIMPSSAPSettings.
     * @since 3.0
     */
    void DoSetUid( TUint32 aUid );
    void DoSetLoginCount( TUint32 aLoginCount );
    void DoResetLoginCount();
    void DoResetProtection();
    void DoSetAccessGroup( TIMPSAccessGroup aGroup );

private: // Own helper functions


    /**
     * Allocs given data to given HBufC.
     * If the new passed data and old data differ,
     * resets the login count if desired. Checks also is the
     * operation allowed on current protection level.
     *
     * @param aPtr Reference to pointer, where to allocate.
     * @param aData Data to allocate.
     * @param aOperation The operation id for protection handler.
     * @param aAffectsToLoginCount Does the setting affect to login count.
     */
    void DoSetBufferL( HBufC*& aPtr, const TDesC& aData,
                       TInt aOperation, TBool aAffectsToLoginCount );


    /**
     * Sets passed integer to passed integer reference.
     * If the new passed data and old data differ,
     * resets the login count if desired. Checks also is the
     * operation allowed on current protection level.
     * Templated method to handle both TUint32's and
     * TInt32's sameway.
     * @param aOldData Reference to old data.
     * @param aData Data to assign.
     * @param aOperation The operation id for protection handler.
     * @param aAffectsToLoginCount Does the setting affect to login count.
     */
    template<class T>
    TInt DoSetNumber( T& aOldData, T aData, TInt aOperation, TBool aAffectsToLoginCount );


private:    // Data

    TInt    iDynamicDataSize;       ///<Cached dynamic data size, owned


    TUint32 iUID;                   ///< SAPSettings identifier, owned

    TUint32 iAP;                    ///< Associated AccessPoint, owned
    HBufC*  iClientId;              ///< SAP client id, owned

    TIMPSSAPProtection iProtection;   ///< SAP protection, owned
    TUint32 iLoginCount;            ///< SAP login count, owned

    HBufC*  iSAPName;               ///< ServiceAccessPoint name, owned
    TInt32  iSAPPort;               ///< ServiceAccessPoint port, owned
    HBufC*  iSAPAddress;            ///< ServiceAccessPoint address, owned
    HBufC*  iSAPUserId;             ///< ServiceAccessPoint user id, owned
    HBufC*  iSAPUserPassword;       ///< ServiceAccessPoint user password, owned

    HBufC*  iHTTPProxyAddress;      ///< HTTPProxy address, owned
    TInt32  iHTTPProxyPort;         ///< HTTPProxy port, owned
    HBufC*  iHTTPProxyUserId;       ///< HTTPProxy user id, owned
    HBufC*  iHTTPProxyUserPassword; ///< HTTPProxy user password, owned

    HBufC*  iServerAuthenticationName; ///< Server authentication name, owned
    HBufC*  iServerAuthenticationPassword; ///< Server authentication password, owned
    HBufC*  iServerAcceptedContentType; ///< Content type accepted by the server, owned

    TUint32	iHighLevelServices;		///< High level services, owned
    TIMPSAccessGroup iAccessGroup; ///< Access group, owned
    TUint32	iAuthorizationMode;	///< Authorization mode, owned
    };

#endif      // __CIMPSSAPSETTINGSBODY_H

//  End of File
