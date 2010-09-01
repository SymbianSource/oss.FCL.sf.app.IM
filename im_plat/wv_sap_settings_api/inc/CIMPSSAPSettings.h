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
* Description:  SAP settings container.
*
*/

#ifndef __CIMPSSAPSETTINGS_H
#define __CIMPSSAPSETTINGS_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <impssapsettings.h>


// FORWARD DECLARATIONS
class CIMPSSAPSettingsBody;
class CIMPSSAPKeyValuePairs;
class RDesWriteStream;

// CLASS DECLARATION
/**
 * SAP Settings container.
 *
 * SAP settings identify the network SAP to access
 * to NW services and has various settings needed
 * in session establish. SAP settings has key-value
 * pair support to extend SAP settings data.
 *
 * SAP settings:
 * -------------
 * - Uid:
 *   SAP UID. UID's are maintained by the
 *   SAP Settings Store. Each SAP stored in certain
 *   SAP Settings Store has solitary UID.
 *
 * - SAPName:
 *   Free form string that can is used to identify the SAP.
 *   Usually names are given by user. SAP Settings Store
 *   enforces SAP Nameto be unique among stored SAPs.
 *
 * - SAPAddress:
 *   Network address for SAP. Usually URL or similar.
 *
 * - SAPPort:
 *   Port to use when accessing the SAP.
 *
 * - SAPUserId:
 *   User ID to identify the user with the SAP.
 *
 * - SAPUserPassword:
 *   User password to identify the user with the SAP.
 *
 * - HTTPProxyAddress:
 *   Proxy to use when accessing the SAP.
 *
 * - HTTPProxyPort:
 *   Proxy port to use when accessing the SAP.
 *
 * - HTTPProxyUserId
 *   Proxy user id to use when accessing the SAP.
 *
 * - HTTPProxyUserPassword:
 *   Proxy user password to use when accessing the SAP.
 *
 * - AccessPoint:
 *   Accesspoint to use when accessing the SAP.
 *   Accesspoint ID is a link to CommsDB IAP settings.
 *
 * - ClientId:
 *   Client ID string to use identofy the client SW to SAP.
 *
 * - HighLevelServices:
 *	 Bitfield that identifies which high level services
 *	 are supported.
 *
 * - ServerAuthenticationName
 *	 Name for network server authentication
 *
 * - ServerAuthenticationPassword
 *	 Password for network server authentication
 *
 * - ServerAcceptedContentType
 *   MIME content type string that tells what content types
 *   are accepted by network server
 *
 * - AuthorizationMode
 *	 Authorization mode
 *
 * - Key-value pairs.
 *   Key-value pair mechanism for generic SAP Settings extensions.
 *
 * @lib WVSAPSettingsStore.dll
 * @since 3.0
 */

class CIMPSSAPSettings : public CBase
    {

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since 3.0
         */
        IMPORT_C static CIMPSSAPSettings* NewL();


        /**
         * Two-phased constructor.
         *
         * @since 3.0
         */
        IMPORT_C static CIMPSSAPSettings* NewLC();


        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIMPSSAPSettings();


    private:  // Constructors

        /**
         * C++ default constructor.
         */
        CIMPSSAPSettings();


    public:  // SAP Data

        /**
         * Getters to SAP Settings data.
         *
         * They return const TDesC& reference or like.
         * If values aren't set, they return reference to empty
         * string or zero.
         *
         * @since 3.0
         */
        IMPORT_C TUint32 Uid() const;
        IMPORT_C const TDesC& SAPName() const;
        IMPORT_C const TDesC& SAPAddress() const;
        IMPORT_C TInt32 SAPPort() const;
        IMPORT_C const TDesC& SAPUserId() const;
        IMPORT_C const TDesC& SAPUserPassword() const;
        IMPORT_C const TDesC& HTTPProxyAddress() const;
        IMPORT_C TInt32 HTTPProxyPort() const;
        IMPORT_C const TDesC& HTTPProxyUserId() const;
        IMPORT_C const TDesC& HTTPProxyUserPassword() const;
        IMPORT_C TUint32 AccessPoint() const;
        IMPORT_C const TDesC& ClientId() const;
        IMPORT_C TUint32 HighLevelServices() const;
        IMPORT_C const TDesC& ServerAuthenticationName() const;
        IMPORT_C const TDesC& ServerAuthenticationPassword() const;
        IMPORT_C const TDesC& ServerAcceptedContentType() const;
        IMPORT_C TUint32 AuthorizationMode() const;

        /**
         * Setters to SAP Settings data.
         *
         * They take const TDesC& reference or like
         * and copy given data to internal buffer.
         *
         * Note! SAP UID and store ID values can't be set by client.
         * SAP UID and store ID value are set when the SAP's
         * data is read from / written to store.
         *
         * If editing protected data field, a leave with
         * KErrAccessDenied is raised from leavable methods.
         * Non leavable methods return KErrAccessDenied.
         *
         * @since 3.0
         */

        IMPORT_C void SetSAPNameL( const TDesC& aName );
        IMPORT_C void SetSAPAddressL( const TDesC& aAddress );
        IMPORT_C TInt SetSAPPort( TInt32 aPort );
        IMPORT_C void SetSAPUserIdL( const TDesC& aUserId );
        IMPORT_C void SetSAPUserPasswordL( const TDesC& aPassword );
        IMPORT_C void SetHTTPProxyAddressL( const TDesC& aAddress );
        IMPORT_C TInt SetHTTPProxyPort( TInt32 aPort );
        IMPORT_C void SetHTTPProxyUserIdL( const TDesC& UserId );
        IMPORT_C void SetHTTPProxyUserPasswordL( const TDesC& aPassword );
        IMPORT_C TInt SetAccessPoint( TUint32 aAP );
        IMPORT_C void SetClientIdL( const TDesC& aClientId );
        IMPORT_C TInt SetHighLevelServices( TUint32 aServices );
        IMPORT_C void SetServerAuthenticationNameL( const TDesC& aAuthenticationName );
        IMPORT_C void SetServerAuthenticationPasswordL( const TDesC& aAuthenticationPassword );
        IMPORT_C void SetServerAcceptedContentTypeL( const TDesC& aAcceptedContentType );
        IMPORT_C TInt SetAuthorizationMode( TUint32 aMode );

        /**
         * Protects this SAP with desired protection level.
         *
         * SAP protection is used to restrict what SAP fields can
         * be edited or what storage operations can be performed to it.
         * See TIMPSSAPProtection enumeration (IMPSSAPSettings.h)for more
         * information about available protection levels and operations
         * allowed and restricted by them.
         *
         * When trying to perform some restricted operation, a leave
         * with KErrAccessDenied is raised or errocode KErrAccessDenied
         * is returned from method.
         *
         * NOTE!! If the SAP is allready protected, setting new
         * protection level leaves also with KErrAccessDenied.
         *
         * By default SAP has no protection (ESAPNoProtection).
         *
         * @param aProtection The protection level.
         * If given  protection level isn't proper one, leaves with KErrArgument.
         *
         * @since 3.0
         */
        IMPORT_C void ProtectL( TIMPSSAPProtection aProtection );


        /**
         * Gets this SAP's protection level.
         *
         * SAP protection is used to restrict what SAP fields can
         * be edited or what storage operations can be performed to it.
         * See TIMPSSAPProtection enumeration (IMPSSAPSettings.h) for
         * more information about available protection levels and
         * operations allowed by them.
         *
         * @return This SAP's protection level.
         *
         * @since 3.0
         */
        IMPORT_C TIMPSSAPProtection Protection() const;


        /**
         * Increases this SAP's login count by one.
         *
         * SAP login count is used to keep track how many
         * times to certain SAP has been logged in. (This
         * information can be used for example to determine
         * is certain SAP proper for automatic login etc.)
         * Client using SAP information to perform login,
         * must itself call this method to increase SAP
         * login count after succesfull login and then use
         * CIMPSSAPSettingsStore interace to store the changed
         * SAP back to store.
         *
         * Changing any of SAP data fields (except the SAP name),
         * resets the login count to zero. (Setting the same data
         * as previously to certain data field, doesn't reset
         * the login count. Changing the SAP name doesn't affect
         * to login count.)
         *
         * @since 3.0
         */
        IMPORT_C void IncreaseLoginCount();


        /**
         * Gets this SAP's login count.
         *
         * SAP login count is used to keep track how many
         * times to certain SAP has been logged in. (This
         * information can be used for example to determine
         * is certain SAP proper for automatic login etc.)
         *
         * @see IncreaseLoginCount()
         * @return The SAP login count.
         */
        IMPORT_C TUint32 LoginCount() const;


    public: //SAP extension

        /**
         * Key-value pair setters.
         *
         * Stores  arbitrary data for given keyword.
         * Key-value pairs are stored to flat data structure
         * in side the target SAP. Different data types
         * may not share same key name.
         * Keyword may not contain string "||". Also, the
         * maximum length for key+value is
         * NCentralRepositoryConstants::KMaxUnicodeStringLength-5
         *
         * @since 3.0
         * @param aKey Keyword to use to identify the value.
         * @param aValue Descriptor / integer value to store.
         * @return KErrNone: Storing the key value was succesful.
         *         KErrGeneral: Different data type was previously
         *         associated with given key. Old value will remain
         *         KErrNoMemory: Out of memory. Old value will remain
         *		   KErrTooBig: Key+value are too big for storing
         */
        IMPORT_C TInt SetOpaqueInt( const TDesC& aKey, TInt aValue );
        IMPORT_C TInt SetOpaqueDesC8( const TDesC& aKey, const TDesC8& aValue );
        IMPORT_C TInt SetOpaqueDesC16( const TDesC& aKey, const TDesC16& aValue );


        /**
         * Key-value pair getters.
         *
         * Gets data previously stored for given keyword.
         *
         * @since 3.0
         * @param aKey Keyword to identify the value.
         * @param aValue Pointer descriptor / integer where the value is get.
         *        Pointer descriptor is put to point to requested data.
         *        Data is owned by the SAP container and stays valid
         *        untill the container object is destroyed or new
         *        value for key is assigned.
         * @return KErrNone: Retrieving the key value was succesful.
         *         KErrNotFound: No value associated with given key.
         *         KErrGeneral: Different data type associated with key.
         */
        IMPORT_C TInt GetOpaqueInt( const TDesC& aKey, TInt& aValue ) const;
        IMPORT_C TInt GetOpaqueDesC8( const TDesC& aKey, TPtrC8& aValue ) const;
        IMPORT_C TInt GetOpaqueDesC16( const TDesC& aKey, TPtrC16& aValue ) const;


        /**
         * Key-value pair cleanup.
         *
         * Deletes previously stored key-value pair.
         *
         * @since 3.0
         * @param aKey Keyword to identify the key-value pair.
         * @return KErrNone: Key-value pair removed.
         *         KErrNotFound: Key-value pair not found.
         */
        IMPORT_C TInt DeleteOpaque( const TDesC& aKey );


    public: // UtilityFunctions

        /**
         * Checks if SAP's connection settings are equal or not.
         *
         * These compared settings are:
         *   - SAP address
         *   - SAP port
         *   - Proxy address
         *   - Proxy port
         *
         * @param aOther CIMPSSAPSettings to compare.
         * @return ETrue if settings are identical, else EFalse.
         *
         * @since 3.0
         */
        IMPORT_C TBool EqualConnectionSettings( const CIMPSSAPSettings& aOther ) const;


        /**
         * Checks if SAP's user settings are equal or not.
         *
         * These compared settings are:
         *   - SAP user id,
         *   - SAP user password,
         *   - Proxy user id,
         *   - Proxy user password
         *
         * @param aOther CIMPSSAPSettings to compare.
         * @return ETrue if settings are identical, else EFalse.
         *
         * @since 3.0
         */
        IMPORT_C TBool EqualUserSettings( const CIMPSSAPSettings& aOther ) const;


        /**
         * Resets SAPs settings.
         *
         * After calling reset, SAP container is on same initial state
         * as after construction. Getters return reference to empty
         * string or zero, uid is zero and login count is zero. Also
         * protection is reset to ESAPNoProtection and all key-value
         * pairs removed.
         *
         * @since 3.0
         */
        IMPORT_C void Reset();


        /**
         * Copies all SAP settings details from given source
         * container to self. If copying failes due OOM, target
         * container state is undetermined.
         *
         * @since 3.0
         *
         * @param aSource The source container copy from.
         */
        IMPORT_C void CopyL( const CIMPSSAPSettings& aSource );

        /**
         * Gets SAPs access group.
         *
         * @return The access group this SAP belongs to, EIMPSNoAccessGroup
         * if the group is not set
         * @since 3.0
         */
        IMPORT_C TIMPSAccessGroup AccessGroup() const;

        /**
         * Sets SAPs access group.
         *
         * @param aGroup SAPs access group
         * @since 3.0
         */

        IMPORT_C void SetAccessGroup( TIMPSAccessGroup aGroup );

    public:  // Streaming support

        /**
         * Externalises the SAP container object
         * to given stream.
         * @param aStream stream to write to.
         *
         * @since 3.0
         */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;


        /**
         * Internalises the SAP container object
         * from given stream.
         * @param aStream stream to read from.
         *
         * @since 3.0
         */
        IMPORT_C void InternalizeL( RReadStream& aStream );


        /**
         * Approximates the SAP data size.
         *
         * @since 3.0
         * @return The SAP data size.
         */
        IMPORT_C TInt DataSize() const;

    public: // Packing support

        /**
         * Packs SAP container object to buffer and returns
         * buffer to caller. Caller is responsible
         * of deleting the buffer. Leaves the created
         * buffer to CleanupStack.
         *
         * @return SAP container object as flat buffer.
         *
         * @since 3.0
         */
        IMPORT_C HBufC8* PackLC() const;

        /**
         * Initializes SAP container object from a package
         * previously created with PackLC().
         *
         * @param aPackage SAP container package
         *        where to read from.
         *
         * @since 3.0
         */
        IMPORT_C void UnPackL( const TDesC8& aPackage );


    public: // Not exported functions for IMPSSAPSettingsStore internal usage

        /**
         * Gets the SAP settings body.
         * @return The SAP settings body,
         *
         * @since 3.0
         */
        CIMPSSAPSettingsBody& Body();
        const CIMPSSAPSettingsBody& Body() const;

        /**
         * Gets the Key-Value pairs.
         * @return Key-Value paisr,
         *
         * @since 3.0
         */

        CIMPSSAPKeyValuePairs& KeyValuePairs();
        const CIMPSSAPKeyValuePairs& KeyValuePairs() const;



    private:    // Data

        CIMPSSAPSettingsBody*   iBody; ///< OWN: Body
        CIMPSSAPKeyValuePairs*  iKeyValuePairs; ///< OWN: key value pairs

    };

#endif      // __CIMPSSAPSETTINGS_H

//  End of File

