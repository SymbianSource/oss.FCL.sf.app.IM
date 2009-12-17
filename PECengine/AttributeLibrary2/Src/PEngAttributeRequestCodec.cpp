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
* Description:  Codec to code attribute requests.
*
*/

// INCLUDE FILES
#include "PEngAttributeRequestCodec.h"
#include "MPEngPresenceAdvancedAttrModel2.h"
#include "MPEngPresenceAttrManager.h"


#include <E32Base.h>
#include <MPEngPresenceAttrModel2.h>
#include <S32strm.h>
#include <S32mem.h>


//CONSTS
const TInt KStreamExpandSize = 500;


// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// AlignBufferTo2L()
// -----------------------------------------------------------------------------
//
void AlignBufferTo2L( CBufFlat& aBuffer, RWriteStream& aWStream )
    {
    //align stream size to 2
    if ( ( aBuffer.Size() % 2 ) == 1 )
        {
        //pad with zero
        aWStream.WriteInt8L( 0 );
        }
    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngAttributeRequestCodec::PackModelArrayL()
// -----------------------------------------------------------------------------
//
HBufC16* PEngAttributeRequestCodec::PackModelArrayL(
    const RPointerArray<MPEngPresenceAttrModel2>& aModels )
    {
    //Init stream
    CBufFlat* dynBuffer = CBufFlat::NewL( KStreamExpandSize );
    CleanupStack::PushL( dynBuffer );

    RBufWriteStream wstream;
    wstream.Open( *dynBuffer );
    CleanupClosePushL( wstream );


    //Serialize the data
    const TInt modelCount = aModels.Count();
    wstream.WriteInt32L( modelCount );
    for ( TInt ii = 0; ii < modelCount; ii++ )
        {
        const MPEngPresenceAdvancedAttrModel2* model = aModels[ ii ]->Advanced();
        wstream.WriteUint32L( model->Type() );
        WritePresenceIdL( wstream, model->PresenceID() );

        model->ExternalizeL( wstream );
        }


    AlignBufferTo2L( *dynBuffer, wstream );

    //Cleanup
    wstream.CommitL();
    CleanupStack::PopAndDestroy(); //wstream


    //make 16 bit presentation of data
    TPtrC8 bufDataPtr8( dynBuffer->Ptr( 0 ) );
    TPtrC16 bufDataPtr16( ( TUint16* ) bufDataPtr8.Ptr(), ( bufDataPtr8.Size() / 2 ) );

    HBufC16* heapBuffer = bufDataPtr16.AllocL();
    CleanupStack::PopAndDestroy( dynBuffer );

    return heapBuffer;
    }



// -----------------------------------------------------------------------------
// PEngAttributeRequestCodec::UnpackModelArrayL()
// -----------------------------------------------------------------------------
//
void PEngAttributeRequestCodec::UnpackModelArrayL(
    const TDesC16& aModelPkg,
    TInt aInstanceOptions,
    RPointerArray<MPEngPresenceAttrModel2>& aModels,
    MPEngPresenceAttrManager& aAttributeManager )

    {
    //make 8 bit presentation of data
    TPtrC8 modelPkg8( ( TUint8* ) aModelPkg.Ptr(), ( aModelPkg.Size() * 2 ) );


    //Init stream
    RDesReadStream rstream;
    rstream.Open( modelPkg8 );
    CleanupClosePushL( rstream );

    //Deserialize
    const TInt modelCount = rstream.ReadInt32L();
    for ( TInt ii = 0; ii < modelCount; ii++ )
        {
        //Read model details
        TUint32 type = rstream.ReadUint32L();
        HBufC* presenceId = ReadPresenceIdLC( rstream );

        //Create model
        MPEngPresenceAttrModel2* model = NULL;
        aAttributeManager.InstantiateAttributeLC( model,
                                                  *presenceId,
                                                  type,
                                                  aInstanceOptions );

        model->Advanced()->InternalizeL( rstream );
        aModels.AppendL( model );
        CleanupStack::Pop(); //model
        CleanupStack::PopAndDestroy( presenceId );
        }


    CleanupStack::PopAndDestroy(); //rstream
    }


// -----------------------------------------------------------------------------
// PEngAttributeRequestCodec::PackFetchRequestL()
// -----------------------------------------------------------------------------
//
HBufC16* PEngAttributeRequestCodec::PackFetchRequestL(
    const MDesCArray& aPresenceIDs,
    const TArray<TUint32>& aTypes )
    {
    CBufFlat* dynBuffer = CBufFlat::NewL( KStreamExpandSize );
    CleanupStack::PushL( dynBuffer );

    RBufWriteStream wstream;
    wstream.Open( *dynBuffer );
    CleanupClosePushL( wstream );


    //Presence IDs
    const TInt presenceIdCount = aPresenceIDs.MdcaCount();
    wstream.WriteInt32L( presenceIdCount );
    for ( TInt ii = 0; ii < presenceIdCount; ii++ )
        {
        WritePresenceIdL( wstream, aPresenceIDs.MdcaPoint( ii ) );
        }

    //Attribute types
    const TInt typeCount = aTypes.Count();
    wstream.WriteInt32L( typeCount );
    for ( TInt jj = 0; jj < typeCount; jj++ )
        {
        wstream.WriteUint32L( aTypes[ jj ] );
        }

    AlignBufferTo2L( *dynBuffer, wstream );

    wstream.CommitL();
    CleanupStack::PopAndDestroy(); //wstream


    //make 16 bit presentation of data
    TPtrC8 bufDataPtr8( dynBuffer->Ptr( 0 ) );
    TPtrC16 bufDataPtr16( ( TUint16* ) bufDataPtr8.Ptr(), ( bufDataPtr8.Size() / 2 ) );

    HBufC16* heapBuffer = bufDataPtr16.AllocL();
    CleanupStack::PopAndDestroy( dynBuffer );


    return heapBuffer;
    }


// -----------------------------------------------------------------------------
// PEngAttributeRequestCodec::UnpackFetchRequestL()
// -----------------------------------------------------------------------------
//
void PEngAttributeRequestCodec::UnpackFetchRequestL( const TDesC16& aFetchRequestPkg,
                                                     CDesCArray& aPresenceIDs,
                                                     RArray<TUint32>& aTypes )
    {
    //make 8 bit presentation of data
    TPtrC8 fetchRequestPkg8( ( TUint8* ) aFetchRequestPkg.Ptr(), ( aFetchRequestPkg.Size() * 2 ) );

    //Init stream
    RDesReadStream rstream;
    rstream.Open( fetchRequestPkg8 );
    CleanupClosePushL( rstream );

    //Deserialize Presence IDs
    const TInt presenceIdCount = rstream.ReadInt32L();
    for ( TInt ii = 0; ii < presenceIdCount; ii++ )
        {
        HBufC* presenceId = ReadPresenceIdLC( rstream );
        aPresenceIDs.AppendL( *presenceId );
        CleanupStack::PopAndDestroy( presenceId );
        }


    //Deserialize Attribute types
    const TInt typeCount = rstream.ReadInt32L();
    for ( TInt jj = 0; jj < typeCount; jj++ )
        {
        TUint32 type = rstream.ReadUint32L();
        aTypes.AppendL( type );
        }

    CleanupStack::PopAndDestroy(); //rstream
    }


// -----------------------------------------------------------------------------
// PEngAttributeRequestCodec::CopyModelDataL()
// -----------------------------------------------------------------------------
//
void PEngAttributeRequestCodec::CopyModelDataL( const MPEngPresenceAttrModel2& aSource,
                                                MPEngPresenceAttrModel2& aDest )
    {
    const MPEngPresenceAdvancedAttrModel2* sourceAdv = aSource.Advanced();
    TInt size = sourceAdv->ExternalizeSize();

    //Init stream
    CBufFlat* dynBuffer = CBufFlat::NewL( size );
    CleanupStack::PushL( dynBuffer );

    RBufWriteStream wstream;
    wstream.Open( *dynBuffer );
    CleanupClosePushL( wstream );

    //Serialize the data
    sourceAdv->ExternalizeL( wstream );
    wstream.CommitL();
    CleanupStack::PopAndDestroy(); //wstream


    //And deserialize data to dest. model
    RBufReadStream rstream;
    rstream.Open( *dynBuffer );
    CleanupClosePushL( rstream );

    aDest.Advanced()->InternalizeL( rstream );

    CleanupStack::PopAndDestroy(); //rstream
    CleanupStack::PopAndDestroy( dynBuffer );
    }


// -----------------------------------------------------------------------------
// PEngAttributeRequestCodec::WritePresenceIdL()
// -----------------------------------------------------------------------------
//
void PEngAttributeRequestCodec::WritePresenceIdL( RWriteStream& aWStream,
                                                  const TDesC& aPresenceId )
    {
    aWStream.WriteInt32L( aPresenceId.Length() );
    aWStream.WriteL( aPresenceId );
    }


// -----------------------------------------------------------------------------
// PEngAttributeRequestCodec::ReadPresenceIdLC()
// -----------------------------------------------------------------------------
//
HBufC* PEngAttributeRequestCodec::ReadPresenceIdLC( RReadStream& aRStream )
    {
    const TInt presenceIdLength = aRStream.ReadInt32L();
    HBufC* presenceId = HBufC::NewLC( presenceIdLength );
    TPtr ptr( presenceId->Des() );
    aRStream.ReadL( ptr, presenceIdLength );

    return presenceId;
    }


//  End of File


