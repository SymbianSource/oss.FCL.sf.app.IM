/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of creator of MCALoggerMessages and headers.
*
*/


// INCLUDE FILES

#include "TCALoggerMessageFactory.h"
#include "CCALoggerMessageHeader.h"
#include "CCATextmessage.h"
#include "CCAContentMessage.h"
#include <s32strm.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CALoggerMessageFactory::CreateLoggerMessageHeaderL
//  IM 2.1 uses notepad to save history files. There is no need to read messages
//  from file.
// -----------------------------------------------------------------------------
//
MCALoggerMessage* TCALoggerMessageFactory::CreateLoggerMessageL( RReadStream& aStream )
    {
    TUint16 contentType = static_cast< MCAMessage::TContentType >( aStream.ReadUint16L() );
    TUint16 versionNum = aStream.ReadUint16L();
    if ( versionNum > KVersionNum )
        {
        User::Leave( KErrNotSupported );
        }
    switch ( contentType )
        {
        case MCAMessage::EContentText:
            {
            CCATextMessage* message = CCATextMessage::NewL();
            message->SetVersionNumber( versionNum );
            return message;
            }
        case MCAMessage::EContentOther: // flowthrough
        case MCAMessage::EContentPicture:
            {
            CCAContentMessage* message = CCAContentMessage::NewL();
            message->SetVersionNumber( versionNum );
            return message;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            return NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CALoggerMessageFactory::CreateLoggerMessageHeaderL
// Create logger message header
// -----------------------------------------------------------------------------
//
MCALoggerMessageHeader* TCALoggerMessageFactory::CreateLoggerMessageHeaderL()
    {
    return CCALoggerMessageHeader::NewL();
    }

// end of file

