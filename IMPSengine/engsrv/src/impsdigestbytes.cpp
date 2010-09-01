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
* Description: Class for imps digest.
*
*
*/

#include <hash.h>
#include <imcvcodc.h>
#include "impsdigestbytes.h"
#include "impskey.h"
#include "impsdataaccessor.h"

const TInt KDbLength = 200;     // 24
const TInt KAolDbLength = 200;  // 32
const TInt KMaxPwdLength = 50;
const TInt KMaxKeyLength = 50;

// -----------------------------------------------------------------------------
// ImpsDigestBytes::TImpsDigestBytes
// -----------------------------------------------------------------------------
//
TImpsDigestBytes::TImpsDigestBytes()
    {
    }
// -----------------------------------------------------------------------------
// ImpsDigestBytes::CreateDigestBytesL
// -----------------------------------------------------------------------------
//
void TImpsDigestBytes::CreateDigestBytesL ( MImpsDataAccessor* aAc,
                                            TImpsDigestSchema /*aSchema*/,
                                            const TDesC& aPassword,
                                            const TDesC8& aNonce )
    {
    TInt size = KMaxPwdLength + aNonce.Length();
    HBufC8* str = HBufC8::NewL( size );
    CleanupStack::PushL( str );  // <<< str
    TPtr8 strPtr = str->Des();
    strPtr.Append( aNonce );
    strPtr.Append( aPassword );

    CMD5* md5 = CMD5::NewL();
    CleanupStack::PushL( md5 );  // <<< md5
    TPtrC8 digestBytes = md5->Hash( *str );

    HBufC8* str2 = HBufC8::NewL( KDbLength );
    CleanupStack::PushL( str2 );  // <<< str2
    TPtr8 strPtr2 = str2->Des();
    TImCodecB64 codec;
    ( void )codec.Encode( digestBytes, strPtr2 );
    CImpsKey* myKey = CImpsKey::NewLC();  // <<< myKey
    myKey->AddL( CREATEKEY( EImpsKeySession, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionContent, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyLogin_Request, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyDigestBytes, 0 ) );
    aAc->StoreDesc8L( myKey, strPtr2 );

    CleanupStack::PopAndDestroy( 4 );
    }

// -----------------------------------------------------------------------------
// ImpsDigestBytes::CreateAolDigestBytesL
// -----------------------------------------------------------------------------
//
void TImpsDigestBytes::CreateAolDigestBytesL ( MImpsDataAccessor* aAc,
                                               TImpsDigestSchema /*aSchema*/,
                                               const TDesC& aPassword,
                                               const TDesC8& aNonce,
                                               const TDesC& aClientId,
                                               HBufC* aKey1,
                                               HBufC* aKey2 )
    {
    // nonce, password, privateKey1
    TInt length = aNonce.Length() + KMaxPwdLength + KMaxKeyLength;
    HBufC8* str = HBufC8::NewL( length );
    CleanupStack::PushL( str );             // <<< str
    TPtr8 p1 = str->Des();
    p1.Append( aNonce );
    p1.Append( aPassword );
    p1.Append( aKey1->Des() );
    CMD5* md51 = CMD5::NewL();
    CleanupStack::PushL( md51 );             // <<< md51
    TPtrC8 bytes = md51->Hash( *str );


    HBufC8* digestBytes = HBufC8::NewL( KAolDbLength );
    CleanupStack::PushL( digestBytes );     // <<< digestBytes
    TPtr8 db = digestBytes->Des();
    db.Append( bytes );

    // nonce, normalized password, privateKey1
    HBufC8* pwd = HBufC8::NewL( KMaxPwdLength );
    CleanupStack::PushL( pwd );             // <<< pwd
    TPtr8 passw = pwd->Des();
    for ( TInt j = 0; j < aPassword.Length(); j++ )
        {
        passw.Append( TChar( aPassword[j] ) );
        }
    passw.TrimAll();
    for ( TInt k = 0; k < passw.Length(); k++ )
        {
        TChar in = passw[k];
        if ( in.IsSpace() )
            passw.Delete( k, 1 );
        }
    passw.LowerCase();
    HBufC8* str2 = HBufC8::NewL( length );
    CleanupStack::PushL( str2 );            // <<< str2
    TPtr8 p2 = str2->Des();
    p2.Append( aNonce );
    p2.Append( passw );
    p2.Append( aKey1->Des() );
    CMD5* md52 = CMD5::NewL();
    CleanupStack::PushL( md52 );             // <<< md52
    TPtrC8 bytes2 = md52->Hash( *str2 );
    db.Append( bytes2 );

    // nonce, clientId, privateKey2
    length = aNonce.Length() + KMaxPwdLength + KMaxKeyLength;
    HBufC8* str3 = HBufC8::NewL( length );
    CleanupStack::PushL( str3 );            // <<< str3
    TPtr8 p3 = str3->Des();
    p3.Append( aNonce );
    p3.Append( aClientId );
    p3.Append( aKey2->Des() );
    CMD5* md53 = CMD5::NewL();
    CleanupStack::PushL( md53 );             // <<< md53
    TPtrC8 bytes3 = md53->Hash( *str3 );
    db.Append( bytes3 );

    HBufC8* hex = HBufC8::NewL( KAolDbLength );
    CleanupStack::PushL( hex );     // <<< hex
    TPtr8 hexPtr = hex->Des();
    for ( TInt i = 0; i < db.Length(); i++ )
        {
        TChar in = db[i];
        hexPtr.AppendNumFixedWidth( in, EHex, 2 );
        }
    CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey
    myKey->AddL( CREATEKEY( EImpsKeySession, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionContent, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyLogin_Request, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyDigestBytes, 0 ) );
    aAc->StoreDesc8L( myKey, hexPtr );

    CleanupStack::PopAndDestroy( 10 );

    }

//  End of File

