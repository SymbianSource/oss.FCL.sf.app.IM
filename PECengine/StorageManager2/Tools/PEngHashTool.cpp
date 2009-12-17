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
* Description:  Hashing tool
*
*/


// INCLUDE FILES
#include "PEngHashTool.h"
// #include <e32std.h>
#include <hash.h>
#include <imcvcodc.h>
#include "PEngStorageGlobals.h"


// -----------------------------------------------------------------------------
// PEngHashTool::HashDescriptorL()
// -----------------------------------------------------------------------------
//
HBufC* PEngHashTool::HashDescriptorL(
    const TDesC& aDesToHash )
    {
    HBufC8* temHashBuf = HBufC8::NewLC( aDesToHash.Length() + KMinimalHashSize );
    temHashBuf->Des().Copy( aDesToHash );
    temHashBuf->Des().LowerCase();
    CSHA1* sHA1 = CSHA1::NewL();
    CleanupStack::PushL( sHA1 );
    sHA1->Reset();

    TPtrC8 hash ( sHA1->Hash( *temHashBuf ) );

    TImCodecB64 b64;
    b64.Initialise();

    TPtr8 encodeBuffer( temHashBuf->Des() );
    encodeBuffer.Zero();
    b64.Encode( hash, encodeBuffer );

    HBufC* hashedId = HBufC::NewL( encodeBuffer.Length() );
    TPtr tmp( hashedId->Des() );
    tmp.Copy( encodeBuffer );
    CleanupStack::PopAndDestroy( 2 ); // sHA1, temHashBuf


    ReplaceCharacterInDescriptor( tmp, '/', '_' );
    ReplaceCharacterInDescriptor( tmp, ':', '_' );
    ReplaceCharacterInDescriptor( tmp, '@', '_' );
    return hashedId;
    }


// -----------------------------------------------------------------------------
// PEngHashTool::HashSubfolderL()
// -----------------------------------------------------------------------------
//
HBufC* PEngHashTool::HashStateNameL(
    const TDesC& aServerAddress,
    const TDesC& aUserName )
    {
    HBufC* address = HBufC::NewLC( aServerAddress.Length() + aUserName.Length() );
    address->Des().Append( aServerAddress );
    address->Des().Append( aUserName );
    address->Des().LowerCase();
    HBufC* folder = HashDescriptorL( *address );
    CleanupStack::PopAndDestroy();//address
    CleanupStack::PushL( folder );
    // KPEngStorageStateSuffix if client wants to attach some
    folder = folder->ReAllocL( folder->Length() +
                               KPEngStorageStateSuffix().Length() );
    CleanupStack::Pop(); // folder
    folder->Des().Append( KPEngStorageStateSuffix );
    return folder;
    }



// -----------------------------------------------------------------------------
// PEngHashTool::ReplaceCharacterInDescriptor()
// Replace characters in descriptor with defined
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void PEngHashTool::ReplaceCharacterInDescriptor(
    TPtr& aDescriptor,
    const TText aConvertFrom,
    const TText aConvertTo  )
    {
    TInt offset;
    while ( ( offset = aDescriptor.Locate( aConvertFrom ) ) != KErrNotFound )
        {
        aDescriptor[ offset ] = aConvertTo;
        }
    }


//  End of File
