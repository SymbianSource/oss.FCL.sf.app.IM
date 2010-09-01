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
* Description:  Presence Engine Contact List extensions.
*
*/

// INCLUDE FILES
#include "PEngWVContactListExtensions2.h"
#include <cntdef.h>
#include <S32Strm.h>
#include <S32mem.h>
#include <E32Base.h>


//CONSTS
const TInt  KStreamExpandSize = 1000;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngContactItemExtensionCodec2::PackContactIdsLC()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* PEngContactItemExtensionCodec2::PackContactIdsLC(
    const CContactIdArray& aContactIds )
    {
    CBufFlat* dynBuffer = CBufFlat::NewL( KStreamExpandSize );
    CleanupStack::PushL( dynBuffer );

    RBufWriteStream wstream;
    wstream.Open( *dynBuffer );
    CleanupClosePushL( wstream );

    aContactIds.ExternalizeL( wstream );

    wstream.CommitL();
    CleanupStack::PopAndDestroy(); //wstream

    TPtrC8 dynBufferPtr( dynBuffer->Ptr( 0 ) );
    HBufC8* heapBuffer = dynBufferPtr.AllocL();
    CleanupStack::PopAndDestroy(); //dynBuffer
    CleanupStack::PushL( heapBuffer );
    return heapBuffer;
    }


// -----------------------------------------------------------------------------
// PEngContactItemExtensionCodec2::UnpackContactIdsL()
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngContactItemExtensionCodec2::UnpackContactIdsL(
    const TDesC8& aContactIdsPckg,
    CContactIdArray& aContactIds )
    {
    aContactIds.Reset();

    if ( aContactIdsPckg.Length() > 0 )
        {
        RDesReadStream rstream;
        rstream.Open( aContactIdsPckg );
        CleanupClosePushL( rstream );

        aContactIds.InternalizeL( rstream );

        CleanupStack::PopAndDestroy(); //rs
        }
    }


// End of File



