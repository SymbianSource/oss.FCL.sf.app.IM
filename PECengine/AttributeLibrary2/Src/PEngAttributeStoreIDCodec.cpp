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
* Description:  Codec to code attribute type and presence id.
*
*/

// INCLUDE FILES
#include    "PEngAttributeStoreIDCodec.h"
#include    <E32Base.h>


//CONSTS
const TUint32 KAttributeLibraryUid = 0x101FB0E2;



// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// PEngAttributeStoreIDCodec::GenerateStoreIdL()
// -----------------------------------------------------------------------------
//
HBufC* PEngAttributeStoreIDCodec::GenerateStoreIdL( TUint32 aAttributeType,
                                                    const TDesC& aPresenceId )
    {
    TInt storeIdLength = aPresenceId.Length() + 4; //2 x TUInt32 ==> 4 x 16bytes
    HBufC* storeIdBuf = HBufC::NewL( storeIdLength );
    TPtr storeId( storeIdBuf->Des() );


    storeId.Append( ( TUint16* ) &KAttributeLibraryUid, 2 );  //TUInt32 ==> 2 x 16bytes
    storeId.Append( ( TUint16* ) &aAttributeType, 2 ); //TUInt32 ==> 2 x 16bytes
    storeId.Append( aPresenceId );
    return storeIdBuf;
    }


// -----------------------------------------------------------------------------
// PEngAttributeStoreIDCodec::ResolveStoreId()
// -----------------------------------------------------------------------------
//
TInt PEngAttributeStoreIDCodec::ResolveStoreId( const TDesC& aStoreId,
                                                TUint32& aAttributeType,
                                                TPtrC& aPresenceId )

    {
    if ( aStoreId.Length() < 4 )
        {
        return KErrCorrupt;
        }

    TUint32 sidUid = *( TUint32* ) aStoreId.Ptr();
    if ( sidUid != KAttributeLibraryUid )
        {
        return KErrCorrupt;
        }


    aAttributeType = *( TUint32* ) aStoreId.Mid( 2 ).Ptr();
    aPresenceId.Set( aStoreId.Mid( 4 ) );

    return KErrNone;
    }




// -----------------------------------------------------------------------------
// PEngAttributeStoreIDCodec::AttributeTypePresentInStoreId()
// -----------------------------------------------------------------------------
//
TBool PEngAttributeStoreIDCodec::AttributeTypePresentInStoreId( const TDesC& aStoreId,
                                                                TUint32 aAttributeType )
    {
    if ( aStoreId.Length() >= 4 )
        {
        //valid store id
        TUint32 sidUid = *( TUint32* ) aStoreId.Ptr();
        if ( sidUid == KAttributeLibraryUid )
            {
            TUint32 attributeType = *( TUint32* ) aStoreId.Mid( 2 ).Ptr();
            if ( attributeType == aAttributeType )
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }



// -----------------------------------------------------------------------------
// PEngAttributeStoreIDCodec::PresenceIdPresentInStoreId()
// -----------------------------------------------------------------------------
//
TBool PEngAttributeStoreIDCodec::PresenceIdPresentInStoreId( const TDesC& aStoreId,
                                                             const TDesC& aPresenceId )
    {
    if ( aStoreId.Length() >= 4 )
        {
        //valid store id
        TPtrC storeIdPresenceIdPart = aStoreId.Mid( 4 );
        if ( aPresenceId.Compare( storeIdPresenceIdPart ) == 0 )
            {
            return ETrue;
            }
        }

    return EFalse;
    }



//  End of File


