/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for header information of history file
*
*/


//  Include Files

#include "CCALoggerMessageHeader.h"
#include "PrivateEngineDefinitions.h"
#include "PrivateEngineDefinitions.h"
#include "CAUtils.h"

#include <s32strm.h>
#include <badesca.h>
#include <StringLoader.h>
#include <caengineNG.rsg>

// Header information of buffers. 4 bytes per buffer. 3 buffers. 3*4 = 12
const TInt KHeaderInformationLength = 12;

// CONSTRUCTION
// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
CCALoggerMessageHeader* CCALoggerMessageHeader::NewL(	const TDesC &aName,
                                                      const TDesC &aId,
                                                      TChatType aChatType )
    {
    CCALoggerMessageHeader* self = new ( ELeave ) CCALoggerMessageHeader;
    CleanupStack::PushL( self );
    self->ConstructL( aName, aId, aChatType );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::NewL
// Static constructor for empty messageheader.
// -----------------------------------------------------------------------------
//
CCALoggerMessageHeader* CCALoggerMessageHeader::NewL()
    {
    CCALoggerMessageHeader* self = new ( ELeave ) CCALoggerMessageHeader;
    CleanupStack::PushL( self );
    self->ConstructL( KNullDesC, KNullDesC );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::~CCALoggerMessageHeader
// Destructor (virtual by CBase)
// -----------------------------------------------------------------------------
//
CCALoggerMessageHeader::~CCALoggerMessageHeader()
    {
    delete iName;
    delete iGroupId;
    delete iFilename;
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::ConstructL
// Second phase construct
// -----------------------------------------------------------------------------
//
void CCALoggerMessageHeader::ConstructL( const TDesC& aName, const TDesC& aId,
                                         TChatType aChatType )
    {
    iName = aName.AllocL();

    iGroupId = aId.AllocL();

    iChatType = aChatType;

    // Time of creation of header or footer
    iStartTime.HomeTime();
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::CCALoggerMessageHeader
// Constructor
// -----------------------------------------------------------------------------
//
CCALoggerMessageHeader::CCALoggerMessageHeader()
    {
    }

//---------------------------------------------------------------
//----------------------- OWN METHODS ---------------------------
//---------------------------------------------------------------

//---------------------------------------------------------------
//----------------- INHERITED METHODS ---------------------------
//---------------------------------------------------------------
// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::InternalizeL
// Internalize message header
// -----------------------------------------------------------------------------
//
void CCALoggerMessageHeader::InternalizeL( RReadStream& aStream )
    {
    HBufC* filename = CAUtils::InternalizeBufferFromStreamL( aStream );
    delete iFilename;
    iFilename = filename;

    HBufC* groupid = CAUtils::InternalizeBufferFromStreamL( aStream );
    delete iGroupId;
    iGroupId = groupid;

    HBufC* name = CAUtils::InternalizeBufferFromStreamL( aStream );
    delete iName;
    iName = name;

    //Use help variables high and low, because if we use
    //MAKE_TINT64( aStream.ReadUint32L, aStream.ReadUint32L ) directly
    //they are read in wrong order. First the low one and second the high.
    TUint32 high = aStream.ReadUint32L();
    TUint32 low = aStream.ReadUint32L();

    iStartTime = MAKE_TINT64( high, low );

    high = aStream.ReadUint32L();
    low = aStream.ReadUint32L();

    iEndTime = MAKE_TINT64( high, low );

    iChatType = static_cast< TChatType >( aStream.ReadUint8L() );
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::ExternalizeL
// Externalize message header
// -----------------------------------------------------------------------------
//
void CCALoggerMessageHeader::ExternalizeL( RWriteStream& aStream ) const
    {
    CAUtils::ExternalizeBufferToStreamL( *iFilename, aStream );
    TPtrC id = CAUtils::DisplayId( *iGroupId );
    CAUtils::ExternalizeBufferToStreamL( id, aStream );
    CAUtils::ExternalizeBufferToStreamL( *iName, aStream );
    aStream.WriteUint32L( I64HIGH( iStartTime.Int64() ) );
    aStream.WriteUint32L( I64LOW( iStartTime.Int64() ) );
    aStream.WriteUint32L( I64HIGH( iEndTime.Int64() ) );
    aStream.WriteUint32L( I64LOW( iEndTime.Int64() ) );
    aStream.WriteUint8L( iChatType );
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::MessageSizeInBytesL
// Size of message header in bytes
// -----------------------------------------------------------------------------
//
TInt CCALoggerMessageHeader::MessageSizeInBytesL() const
    {
    if ( !iFilename || !iGroupId || !iName )
        {
        User::Leave( KErrNotReady );
        }

    // Data count by bytes and header information of buffers. 4 bytes per buffer.
    // If header information changes, remember to update KHeaderInformationLength
    return (	iFilename->Size() +
             CAUtils::DisplayId( *iGroupId ).Size() +
             iName->Size() ) * KCharacterLengthInBytes +
           KHeaderInformationLength;
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::FilenameL
// Real file name of history file
// -----------------------------------------------------------------------------
//
const TDesC& CCALoggerMessageHeader::FilenameL()
    {
    if ( !iFilename )
        {
        User::Leave( KErrNotReady );
        }
    return *iFilename;
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::SetFilenameL
// Set real filename of history file.
// -----------------------------------------------------------------------------
//
void CCALoggerMessageHeader::SetFilenameL( const TDesC& aFilename )
    {
    HBufC* filename = aFilename.AllocL();
    delete iFilename;
    iFilename = filename;
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::EndLogging
// Get time when logging ends
// -----------------------------------------------------------------------------
//
void CCALoggerMessageHeader::EndLogging()
    {
    iEndTime.HomeTime();
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::ChatType
// ChatType of logged data
// -----------------------------------------------------------------------------
//
TChatType CCALoggerMessageHeader::ChatType() const
    {
    return iChatType;
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::Name
// name of history data
// -----------------------------------------------------------------------------
//
const TDesC& CCALoggerMessageHeader::Name() const
    {
    return *iName;
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::StartTime
// startime of history data
// -----------------------------------------------------------------------------
//
const TTime& CCALoggerMessageHeader::StartTime() const
    {
    return iStartTime;
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::EndTime
// end time of history data
// -----------------------------------------------------------------------------
//
const TTime& CCALoggerMessageHeader::EndTime() const
    {
    return iEndTime;
    }

// -----------------------------------------------------------------------------
// CCALoggerMessageHeader::Identification
// identification
// -----------------------------------------------------------------------------
//
const TDesC& CCALoggerMessageHeader::Identification() const
    {
    return *iGroupId;
    }

//  End of File
