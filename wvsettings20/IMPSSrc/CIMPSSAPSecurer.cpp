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


//  INCLUDES
#include 	"CIMPSSAPSecurer.h"
#include	"IMPSSAPSettingsStoreDefinitions.h"

#include    <3des.h> 		//3des encryption/decryption
#include	<etelmm.h> 		// for getting the IMEI
#include	<mmtsy_names.h>
#include 	<symmetric.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSSAPSecurer::CIMPSSAPSecurer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSAPSecurer::CIMPSSAPSecurer()
    {
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSecurer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMPSSAPSecurer::ConstructL()
    {
    // Get the encryption key
    iEncryptionKey = HBufC8::NewL( KSSS3DESKeySize );
    TPtr8 keyPtr( iEncryptionKey->Des() );
    GetPwdKeyL( keyPtr );

    // Create encryptor and decryptor
    iEncryptor = C3DESEncryptor::NewL( keyPtr );
    iDecryptor = C3DESDecryptor::NewL( keyPtr );

    iTransformBlock = HBufC8::NewMaxL( iEncryptor->BlockSize() );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSecurer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSSAPSecurer* CIMPSSAPSecurer::NewL()
    {
    CIMPSSAPSecurer* self = new( ELeave ) CIMPSSAPSecurer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// Destructor
CIMPSSAPSecurer::~CIMPSSAPSecurer()
    {
    delete iTransformBlock;
    delete iEncryptionKey;
    delete iEncryptor;
    delete iDecryptor;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSecurer::EncryptPwdL()
// Encrypts password
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CIMPSSAPSecurer::EncryptPwdL( const TDesC& aPwdBuffer )
    {
    HBufC* encrypted;
    // first check for 0 length buffers
    if ( aPwdBuffer.Length() == 0 )
        {
        encrypted = HBufC::NewMaxL( 0 );
        return encrypted;
        }

    // character for padding
    TChar KPad( 0x00 );

    TUint8* tempPtr = ( TUint8* ) aPwdBuffer.Ptr();
    TPtr8 buf( tempPtr, aPwdBuffer.Size(), aPwdBuffer.Size() );
    TInt bufSize = aPwdBuffer.Size();

    const TInt blocksize = iEncryptor->BlockSize();
    const TInt remainder = bufSize % blocksize;

    // make the result buffer big enough for the padding ==>
    // round upwards to next multiple of blocksize
    if ( remainder == 0 )
        {
        encrypted = HBufC::NewMaxLC( bufSize / 2 );
        }
    else
        {
        encrypted = HBufC::NewMaxLC( ( bufSize + ( blocksize - remainder )  ) / 2 );
        }

    TUint8* tempEncPtr = ( TUint8* ) encrypted->Ptr();
    TPtr8 encPtr( tempEncPtr, NULL, encrypted->Des().MaxSize() );
    TPtr8 transformPtr( iTransformBlock->Des() );

    TInt offset( 0 );
    while ( offset <= bufSize - blocksize )
        {
        transformPtr = buf.Mid( offset, blocksize );
        iEncryptor->Transform( transformPtr );
        encPtr.Append( transformPtr );
        offset += blocksize;
        }

    // Handle last block. If last block is not 8 characters long add padding
    // nulls to end of it.
    if ( remainder != 0 )
        {
        transformPtr = buf.Mid( offset );
        transformPtr.AppendFill( KPad, blocksize - remainder );
        iEncryptor->Transform( transformPtr );
        encPtr.Append( transformPtr );
        }

    CleanupStack::Pop( encrypted );
    return encrypted;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSecurer::DecryptPwdL()
// Decrypts password
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CIMPSSAPSecurer::DecryptPwdL( const TDesC& aPwdBuffer )
    {
    HBufC* decrypted;
    // first check for 0 length buffers
    if ( aPwdBuffer.Length() == 0 )
        {
        decrypted = HBufC::NewMaxL( 0 );
        return decrypted;
        }
    // character for padding
    TChar KPad( 0x00 );

    TUint8* tempPtr = ( TUint8* ) aPwdBuffer.Ptr();
    TPtr8 buf( tempPtr, aPwdBuffer.Size(), aPwdBuffer.Size() );
    TInt bufSize = aPwdBuffer.Size();

    const TInt blocksize = iEncryptor->BlockSize();
    const TInt remainder = bufSize % blocksize;

    // make the result buffer big enough for the padding ==>
    // round upwards to next multiple of blocksize
    if ( remainder == 0 )
        {
        decrypted = HBufC::NewMaxLC( bufSize / 2 );
        }
    else
        {
        decrypted = HBufC::NewMaxLC( ( bufSize + ( blocksize - remainder ) ) / 2 );
        }

    TUint8* tempDecPtr = ( TUint8* ) decrypted->Ptr();
    TPtr8 decPtr( tempDecPtr, NULL, decrypted->Des().MaxSize() );
    TPtr8 transformPtr( iTransformBlock->Des() );

    TInt offset( 0 );
    while ( offset <= bufSize - blocksize )
        {
        transformPtr = buf.Mid( offset, blocksize );
        iDecryptor->Transform( transformPtr );
        decPtr.Append( transformPtr );
        offset += blocksize;
        }

    // Remove padding nulls from end of the decrypted password
    if ( remainder != 0 )
        {
        transformPtr = buf.Mid( offset );
        transformPtr.AppendFill( KPad, blocksize - remainder );
        iDecryptor->Transform( transformPtr );
        decPtr.Append( transformPtr );
        }

    TInt padStart( decrypted->Des().Locate( KPad ) );
    if ( padStart != KErrNotFound )
        {
        decrypted->Des().SetLength( padStart );
        }

    CleanupStack::Pop( decrypted );
    return decrypted;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSecurer::FetchIMEI()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSAPSecurer::FetchIMEI( TPtr8& aImei )
    {

    // Create a fake serial number for emulator or for fallback if
    // we're unable to get the real IMEI from hardware
    TBuf<RMobilePhone::KPhoneSerialNumberSize> fakeSerialNumber( KSAPFakeIMEI );
    aImei.Copy( fakeSerialNumber );


#ifndef __WINS__
    RMobilePhone::TMobilePhoneIdentityV1 phoneIdentity;
    // initializations
    RMobilePhone phone;
    RTelServer server;

    TRAPD( err,
        {
        err = server.Connect();
        User::LeaveIfError( err );
        CleanupClosePushL( server );

        err = server.LoadPhoneModule( KMmTsyModuleName );
        if ( err != KErrNone && err != KErrAlreadyExists )
            {
            User::Leave( err );
            }
        err = phone.Open( server, KMmTsyPhoneName );
        User::LeaveIfError( err );
        CleanupClosePushL( phone );

        TUint32 identityCaps;
        err = phone.GetIdentityCaps( identityCaps );
        User::LeaveIfError( err );

        if ( identityCaps & RMobilePhone::KCapsGetModel )
            {
            TRequestStatus status;
            phone.GetPhoneId( status, phoneIdentity );
            // No choice but to wait for the request to complete.
            User::WaitForRequest( status ); // CSI: 94 #
            User::LeaveIfError( status.Int() ); // CSI: 42 #
            // Inside trap
            }
        else // In case we didn't have capabilities to get real IMEI
            {
            User::Leave( KErrNotSupported ); // CSI: 42 #
            // Inside trap
            }

        CleanupStack::PopAndDestroy( 2 ); // server, phone
        } );

    // Fetching the IMEI went fine.
    if ( err == KErrNone )
        {
        aImei.Copy( phoneIdentity.iSerialNumber );
        }

#endif // __WINS__

    }


// -----------------------------------------------------------------------------
// CIMPSSAPSecurer::GetPwdKeyL()
// Generates new the encryption key
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CIMPSSAPSecurer::GetPwdKeyL( TPtr8& aKey )
    {
    // Copy the defined key
    aKey.Copy( KSAPPwdDefinedKey );

    // Fetch the IMEI
    HBufC8* imeiBuf = HBufC8::NewLC( RMobilePhone::KPhoneSerialNumberSize );
    TPtr8 imeiPtr = imeiBuf->Des();
    FetchIMEI( imeiPtr );

    // XOR defined key with IMEI
    TText8* character = ( TText8* ) aKey.Ptr();
    TText8* characterEnd = aKey.Length() + ( TText8* ) aKey.Ptr();
    TText8 asNumber;

    const TInt imeiLen = imeiPtr.Length();
    TInt encryptCharIndex = 0;
    for ( ; character < characterEnd; character++ )
        {
        asNumber = *character;
        TInt imeiIndex( encryptCharIndex % imeiLen );
        asNumber ^= imeiPtr[ imeiIndex ];
        *character = asNumber;

        encryptCharIndex++;
        }

    CleanupStack::PopAndDestroy( imeiBuf );
    }


// End of File
