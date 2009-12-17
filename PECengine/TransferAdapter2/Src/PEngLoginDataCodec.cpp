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
* Description:  Login data codec.
*
*/

// INCLUDE FILES
#include    "PEngLoginDataCodec.h"
#include    "PresenceDebugPrint.h"
#include    <E32Std.h>
#include    <CIMPSSAPSettings.h>
#include    <s32strm.h>
#include    <s32mem.h>


// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngLoginDataCodec::PackLoginDataL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* PEngLoginDataCodec::PackLoginDataL( const CIMPSSAPSettings& aSAP )
    {
    PENG_DP_TXT( "PEngLoginDataCodec::PackLoginDataL() - CIMPSSAPSettings" );

    HBufC8* packBuffer = HBufC8::NewLC( 5000 );
    TPtr8 pack( packBuffer->Des() );

    RDesWriteStream ws;
    ws.Open( pack ); // CSI: 65 #
    CleanupClosePushL( ws );

    aSAP.ExternalizeL( ws );

    ws.CommitL();
    CleanupStack::PopAndDestroy(); //ws


    packBuffer = packBuffer->ReAllocL( packBuffer->Length() );
    CleanupStack::Pop();
    return packBuffer;
    }


// -----------------------------------------------------------------------------
// PEngLoginDataCodec::UnPackLoginDataL()
// -----------------------------------------------------------------------------
//
EXPORT_C void PEngLoginDataCodec::UnPackLoginDataL( const TDesC8& aLoginData,
                                                    CIMPSSAPSettings& aSAP )
    {
    PENG_DP_TXT( "PEngLoginDataCodec::UnPackLoginDataL() - CIMPSSAPSettings" );

    RDesReadStream rs;
    rs.Open( aLoginData ); // CSI: 65 #
    CleanupClosePushL( rs );

    aSAP.Reset();
    aSAP.InternalizeL( rs );

    CleanupStack::PopAndDestroy(); //rs
    }


//  End of File
