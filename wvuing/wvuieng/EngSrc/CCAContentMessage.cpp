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
* Description:  Message containing picture.
*
*/


#include "CCAContentMessage.h"
#include "PublicEngineDefinitions.h"

#include <fbs.h>

/// Constants
_LIT8( KMimeJpeg, "image/jpeg" );
_LIT8( KMimeGif, "Image/gif" );

//-----------------------------------------------------------------------------
// CCAContentMessage::CCAContentMessage
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAContentMessage::CCAContentMessage()
    {
    iThumbnailSize.SetSize( 10, 10 );
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::~CCAContentMessage
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAContentMessage::~CCAContentMessage()
    {
    __ASSERT_DEBUG( iOwnerCount == 0, User::Panic( KCAEnginePanicCategory ,
                                                   KErrInUse ) );
    delete iThumbnail;
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::NewL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAContentMessage* CCAContentMessage::NewL( TInt aOpCode,
                                            const TDesC& aSender,
                                            const TDesC& aRecipient,
                                            const MDesCArray* aRecipients,
                                            const MDesCArray* aScreenNames,
                                            const TDesC8& aContentType,
                                            const TDesC8& aContentData,
                                            const TDesC& aContentAddress )
    {
    CCAContentMessage* self = CCAContentMessage::NewLC( aOpCode,
                                                        aSender,
                                                        aRecipient,
                                                        aRecipients,
                                                        aScreenNames,
                                                        aContentType,
                                                        aContentData,
                                                        aContentAddress );
    CleanupStack::Pop( self );
    return self;
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::NewL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAContentMessage* CCAContentMessage::NewL()
    {
    return CCAContentMessage::NewL( 0, KNullDesC, KNullDesC, NULL, NULL,
                                    KNullDesC8, KNullDesC8, KNullDesC );
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::NewLC
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCAContentMessage* CCAContentMessage::NewLC( TInt aOpCode,
                                             const TDesC& aSender,
                                             const TDesC& aRecipient,
                                             const MDesCArray* aRecipients,
                                             const MDesCArray* aScreenNames,
                                             const TDesC8& aContentType,
                                             const TDesC8& aContentData,
                                             const TDesC& aContentAddress )
    {
    CCAContentMessage* self = new ( ELeave ) CCAContentMessage();
    CleanupStack::PushL( self );
    self->ConstructL( aContentType, aContentData, aContentAddress );
    self->CCAMessageBase::ConstructL( aOpCode, aSender, aRecipient, aRecipients,
                                      aScreenNames );
    return self;
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::ConstructL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAContentMessage::ConstructL( const TDesC8& aContentType,
                                    const TDesC8& aContentData,
                                    const TDesC& aContentAddress )
    {
    iMimeType = aContentType.AllocL();
    iContentData = aContentData.AllocL();
    iText = aContentAddress.AllocL();

    if ( !( aContentType.CompareF( KMimeGif ) == 0 ||
            aContentType.CompareF( KMimeJpeg ) == 0 ) )
        {
        iContentType = EContentOther;
        }
    else
        {
        iContentType = EContentPicture;
        }

    if ( iContentType == EContentOther )
        {
        SetProcessState( EContentNotSupported );
        }
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::SizeInBytes
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAContentMessage::SizeInBytes() const
    {
    return  CCAMessageBase::SizeInBytes() +
            iMimeType->Size() +
            iContentData->Size() +
            sizeof( *this );
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::ContentType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage::TContentType CCAContentMessage::ContentType() const
    {
    return iContentType;
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::ContentType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAContentMessage::ReleaseTemporaryContentData()
    {
    SetProcessState( EContentReady );
    delete iThumbnail;
    iThumbnail = NULL;
    }

// ---------------------------------------------------------
// CCAContentMessage::InternalizeL()
// ---------------------------------------------------------
//
void CCAContentMessage::InternalizeL( RReadStream& aStream )
    {
    CCAMessageBase::InternalizeL( aStream );

    if ( !( iMimeType->CompareF( KMimeGif ) == 0 ||
            iMimeType->CompareF( KMimeJpeg ) == 0 ) )
        {
        iContentType = EContentOther;
        }
    else
        {
        iContentType = EContentPicture;
        }

    if ( iContentType == EContentOther )
        {
        SetProcessState( EContentNotSupported );
        }
    else
        {
        SetProcessState( EContentReady );
        }
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::ThumbnailSize
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TSize CCAContentMessage::ThumbnailSize() const
    {
    return iThumbnailSize;
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::SetThumbnailSize
// ( Other items commented in header )
//-----------------------------------------------------------------------------
EXPORT_C void CCAContentMessage::SetThumbnailSize( TSize aSize )
    {
    iThumbnailSize = aSize;
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::SetThumbnail
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAContentMessage::SetThumbnail( CFbsBitmap* aThumbnail )
    {
    delete iThumbnail;
    iThumbnail = aThumbnail;
    }

//-----------------------------------------------------------------------------
// CCAContentMessage::Thumbnail
// ( Other items commented in header )
//-----------------------------------------------------------------------------
EXPORT_C CFbsBitmap* CCAContentMessage::Thumbnail() const
    {
    return iThumbnail;
    }

// end of file
