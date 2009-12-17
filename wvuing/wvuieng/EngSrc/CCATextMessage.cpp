/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message containing text.
*
*/


#include "CCATextMessage.h"
#include <stringloader.h>
#include <caengineNG.rsg>

//-----------------------------------------------------------------------------
// CCATextMessage::CCATextMessage
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCATextMessage::CCATextMessage()
    {
    }

//-----------------------------------------------------------------------------
// CCATextMessage::~CCATextMessage
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCATextMessage::~CCATextMessage()
    {
    }

//-----------------------------------------------------------------------------
// CCATextMessage::NewL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCATextMessage* CCATextMessage::NewL(   TInt aOpCode,
                                        const TDesC& aSender,
                                        const TDesC& aRecipient,
                                        const MDesCArray* aRecipients,
                                        const MDesCArray* aScreenNames,
                                        const TDesC& aData )
    {
    CCATextMessage* self = CCATextMessage::NewLC( aOpCode, aSender, aRecipient, aRecipients, aScreenNames, aData );
    CleanupStack::Pop( self );
    return self;
    }

//-----------------------------------------------------------------------------
// CCATextMessage::NewL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCATextMessage* CCATextMessage::NewL()
    {
    return CCATextMessage::NewL( 0, KNullDesC, KNullDesC, NULL, NULL, KNullDesC );
    }

//-----------------------------------------------------------------------------
// CCATextMessage::NewLC
// ( Other items commented in header )
//-----------------------------------------------------------------------------
CCATextMessage* CCATextMessage::NewLC(  TInt aOpCode,
                                        const TDesC& aSender,
                                        const TDesC& aRecipient,
                                        const MDesCArray* aRecipients,
                                        const MDesCArray* aScreenNames,
                                        const TDesC& aData )
    {
    CCATextMessage* self = new ( ELeave ) CCATextMessage();
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    self->CCAMessageBase::ConstructL( aOpCode, aSender, aRecipient, aRecipients, aScreenNames );
    return self;
    }

//-----------------------------------------------------------------------------
// CCATextMessage::ConstructL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCATextMessage::ConstructL( const TDesC& aData )
    {
    iText = aData.AllocL();
    }

//-----------------------------------------------------------------------------
// CCATextMessage::SizeInBytes
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCATextMessage::SizeInBytes() const
    {
    return  CCAMessageBase::SizeInBytes() +
            iText->Size() +
            sizeof( *this );
    }

//-----------------------------------------------------------------------------
// CCATextMessage::ContentType
// ( Other items commented in header )
//-----------------------------------------------------------------------------
MCAMessage::TContentType CCATextMessage::ContentType() const
    {
    return EContentText;
    }

//-----------------------------------------------------------------------------
// CCATextMessage::SetFailed
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCATextMessage::SetFailedL()
    {
    SetFailed();

    CDesCArray* array = new( ELeave )CDesCArrayFlat( 2 ); // two items
    CleanupStack::PushL( array );

    array->AppendL( KNullDesC );
    array->AppendL( *iText );

    // %0U Failed: %1U string is used
    HBufC* failed = StringLoader::LoadLC( R_CHATCLIENT_MESSAGE_FAILED, *array );
    HBufC* temp = HBufC::NewLC( failed->Length() );
    TPtr tempPtr( temp->Des() );
    tempPtr.Zero();
    tempPtr.Append( *failed );
    CleanupStack::Pop( temp );
    CleanupStack::PopAndDestroy( 2, array );
    delete iText;
    iText = temp;
    }


// end of file
