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
* Description: Object handler implementation
*
*/

//  INCLUDES
#include    "IMPSSAPObjectHandler.h"
#include    <s32mem.h>

// size to use if the source object size is 0 in copy
const TInt KTargetObjectSize = 200;

// ================= LOCAL FUNCTIONS =======================

// -----------------------------------------------------------------------------
// ImplementationVersionsLeft()
// -----------------------------------------------------------------------------
//
TBool ImplementationVersionsLeft( const MIMPSSapObject& aObj, TInt aVersionID )
    {
    return ( aVersionID < aObj.ObjectVersion() );
    }


// -----------------------------------------------------------------------------
// WriteZeroVersionMarkL()
// -----------------------------------------------------------------------------
//
TStreamPos WriteZeroExtensionMarkL( RWriteStream& aStream )
    {
    //Write extension mark & return its position
    TStreamPos extMarkPos = aStream.Sink()->TellL( MStreamBuf::EWrite );
    aStream.WriteInt32L( 0 ); //Zero length extension
    return extMarkPos;
    }



// -----------------------------------------------------------------------------
// PatchVersionMarkL()
// -----------------------------------------------------------------------------
//
void PatchExtensionMarkL( const TStreamPos& aExtMarkPos,
                          RWriteStream& aStream,
                          TInt aExtensionLength )
    {
    aStream.Sink()->SeekL( MStreamBuf::EWrite, aExtMarkPos );
    aStream.WriteInt32L( aExtensionLength );
    }


// -----------------------------------------------------------------------------
// ReadExtensionMarkL()
// -----------------------------------------------------------------------------
//
TInt ReadExtensionMarkL( RReadStream& aStream )
    {
    return aStream.ReadInt32L();
    }


// -----------------------------------------------------------------------------
// ExternalizeVersionL()
// -----------------------------------------------------------------------------
//
TInt ExternalizeVersionL( const MIMPSSapObject& aObj, TInt aVersionID, RWriteStream& aStream )
    {
    TStreamPos versionStartPos = aStream.Sink()->TellL( MStreamBuf::EWrite );

    //Write version data & zero extension mark
    aObj.ExternalizeVersionDataL( aVersionID, aStream );
    TStreamPos extMarkPos = WriteZeroExtensionMarkL( aStream );

    //Calculate version length
    TInt extensionLength = 0;
    TInt versionLenght = aStream.Sink()->TellL( MStreamBuf::EWrite ) -
                         versionStartPos;


    //Recurse to next version if needed & patch
    //the extension mark to contain cumulative length
    //all data stored during recursion
    if ( ImplementationVersionsLeft( aObj, aVersionID ) )
        {
        extensionLength = ExternalizeVersionL( aObj, ++aVersionID, aStream );
        PatchExtensionMarkL( extMarkPos, aStream, extensionLength );
        }


    return versionLenght + extensionLength;
    }


// -----------------------------------------------------------------------------
// InternalizeVersionL()
// -----------------------------------------------------------------------------
//
void InternalizeVersionL( MIMPSSapObject& aObj,
                          TInt aVersionID,
                          RReadStream& aStream )
    {
    //Three cases:
    // 1. Stream has same versions than implementaion.
    // 2. Stream is missing data version compared to the implementation.
    // 3. Stream has higher data version than implementation supports.

    aObj.InternalizeVersionDataL( aVersionID, aStream );

    TInt extensionLength = ReadExtensionMarkL( aStream );

    //Proceed if stream has subsequent extensions
    if ( extensionLength > 0 )
        {
        if ( ImplementationVersionsLeft( aObj, aVersionID ) )
            {
            //Current implementation is capable to read more versions
            //==> recurse to next one
            InternalizeVersionL( aObj, ++aVersionID, aStream );
            }
        else
            {
            //Implementation isn't capable to read more version
            //==> consume remaining extension blocks
            aStream.ReadL( extensionLength );
            }
        }
    }







// ================= MEMBER FUNCTIONS =======================




// -----------------------------------------------------------------------------
// IMPSSAPObjectHandler::DataSize()
// -----------------------------------------------------------------------------
//
TInt IMPSSAPObjectHandler::DataSize( const MIMPSSapObject& aObj )
    {
    // Object id: 4 bytes
    // Object version: 4 bytes
    TInt dataSize = 8;


    //For each version: version extension mark, 4 bytes
    TInt versionCount = aObj.ObjectVersion() - KIMPSSapObjVersionInitial;
    dataSize += ( versionCount * 4 );


    //and actual data size
    dataSize += aObj.DataSize();


    return dataSize;
    }



// -----------------------------------------------------------------------------
// IMPSSAPObjectHandler::ExternalizeL()
// -----------------------------------------------------------------------------
//
void IMPSSAPObjectHandler::ExternalizeL( const MIMPSSapObject& aObj,
                                         RWriteStream& aStream )
    {
    //Externalize object ID's
    aStream.WriteInt32L( aObj.ObjectID() );
    aStream.WriteInt32L( aObj.ObjectVersion() );


    TStreamPos pos = aStream.Sink()->TellL( MStreamBuf::EWrite );

    //Recursively go through all data versions
    TInt length = ExternalizeVersionL( aObj, KIMPSSapObjVersionInitial, aStream );

    //And correct the stream write point
    aStream.Sink()->SeekL( MStreamBuf::EWrite, ( pos + length ) );
    }


// -----------------------------------------------------------------------------
// IMPSSAPObjectHandler::InternalizeL()
// -----------------------------------------------------------------------------
//
void IMPSSAPObjectHandler::InternalizeL( MIMPSSapObject& aObj, RReadStream& aStream )
    {
    aObj.Reset();

    //Object ID
    TIMPSSapObjectID id = static_cast<TIMPSSapObjectID> ( aStream.ReadInt32L() );
    if ( id != aObj.ObjectID() )
        {
        User::Leave( KErrCorrupt );
        }

    //Object version
    aStream.ReadInt32L();

    //and recursively go through all available version datas
    InternalizeVersionL( aObj, KIMPSSapObjVersionInitial, aStream );
    }


// -----------------------------------------------------------------------------
// IMPSSAPObjectHandler::CopyL()
// -----------------------------------------------------------------------------
//
void IMPSSAPObjectHandler::CopyL( const MIMPSSapObject& aSource, MIMPSSapObject& aTarget )
    {
    //Don't check here object ID's because underlying stream
    //implementations might be capable to convert objects to another

    TInt objectSize = IMPSSAPObjectHandler::DataSize( aSource );
    if ( objectSize == 0 )
        {
        objectSize = KTargetObjectSize;
        }

    CBufSeg* buffer = CBufSeg::NewL( objectSize );
    CleanupStack::PushL( buffer );

    RBufWriteStream wstream;
    wstream.Open( *buffer );
    CleanupClosePushL( wstream );

    IMPSSAPObjectHandler::ExternalizeL( aSource, wstream );
    CleanupStack::PopAndDestroy(); //wstream

    RBufReadStream rstream;
    rstream.Open( *buffer );
    CleanupClosePushL( rstream );

    IMPSSAPObjectHandler::InternalizeL( aTarget, rstream );

    CleanupStack::PopAndDestroy( 2 ); //rstream, buffer
    }



//  End of File
