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
* Description:  IMPS SAP Settings Store implementation.
*
*/


#ifndef __CIMPSSAPSECURER_H__
#define __CIMPSSAPSECURER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class C3DES; 				// 3DES block ciphers for encryption/decryption
class CBlockTransformation;	// block transformer

// CONST
const TInt KSSS3DESKeySize = 24; // 24 bytes

/**
*  Implementation for CIMPSSAPSettingsStore.
*  Handles the 3DES encryption and decryption of SAP passwords.
*
*  @see CIMPSSAPSettingStore
*  @lib WVSAPSettings.dll
*  @since 3.1
*/
NONSHARABLE_CLASS( CIMPSSAPSecurer ) : public CBase
    {

public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CIMPSSAPSecurer* NewL();

    /**
    * Destructor.
    */
    virtual ~CIMPSSAPSecurer();

private: //Constructors

    /**
    * C++ default constructor.
    */
    CIMPSSAPSecurer();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

public:

    /**
     * Encrypts given buffer and returns ownership
     * of the buffer to calling object
     *
     * @param aPwdBuffer The password to be encrypted
     * @return HBufC* Result of encryption
     * @since 3.1
     */
    HBufC* EncryptPwdL( const TDesC& aPwdBuffer );

    /**
     * Decrypts given buffer and returns ownership
     * of the buffer to calling object
     *
     * @param aPwdBuffer The password to be decrypted
     * @return HBufC* Result of decryption
     * @since 3.1
     */
    HBufC* DecryptPwdL( const TDesC& aPwdBuffer );

public:

    /**
     * Fetches the phone's serial number (IMEI number).
     *
     * @param aImei Pointer to descriptor where the IMEI is saved.
     * @since 3.1
     */
    void FetchIMEI( TPtr8& aImei );

    /**
     * Gets the encryption key for use with 3DES encryption.
     *
     * @param aKey Pointer to descriptor where the key is saved.
     * @since 3.1
     */

    void GetPwdKeyL( TPtr8& aKey );

private: // data

    // Key for encrypting/decrypting user password in SAP container (own)
    HBufC8* iEncryptionKey;

    // A buffer for encryption/decryption handling to avoid
    // reoccurring allocs (own)
    HBufC8* iTransformBlock;

    // Password encryptor (own)
    CBlockTransformation* iEncryptor;

    // Password decryptor (own)
    CBlockTransformation* iDecryptor;

    };



#endif // __CIMPSSAPSECURER_H__

