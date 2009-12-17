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
* Description:  Content writer for the XML Serializer.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "TPEngWriter.h"



// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
TPEngWriter::TPEngWriter( TDes8& aBuffer )
        : iBuffer( aBuffer )
    {
    }


// -----------------------------------------------------------------------------
// TPEngWriter::WriteL()
//
// Writes the text to buffer, leaves if the buffer overflows.
// Returns pointer descriptor pointing to just written text
// (pointer desriptor stays valid until the actual buffer stays valid).
// -----------------------------------------------------------------------------
//
const TPtrC8 TPEngWriter::WriteL( const TDesC8& aText )
    {
    if ( iBuffer.MaxLength() < ( iBuffer.Length() + aText.Length() ) )
        {
        User::Leave( KErrOverflow );
        }

    else
        {
        TInt startOfNew( iBuffer.Length() );
        iBuffer.Append( aText );
        return iBuffer.Mid( startOfNew );
        }

    return TPtrC8( NULL, 0 );
    }


// -----------------------------------------------------------------------------
// TPEngWriter::WriteL()
//
// Writes one character to buffer, leaves if the buffer overflows.
// -----------------------------------------------------------------------------
//
void TPEngWriter::WriteL( TUint8 aCharacter )
    {
    if ( iBuffer.MaxLength() < ( iBuffer.Length() + 1 ) ) //here magic number 1 means that we are
        //writing just one character at time
        {
        User::Leave( KErrOverflow );
        }

    else
        {
        iBuffer.Append( &aCharacter, 1 );  //here magic number 1 means that we are
        //writing just one character at time
        }
    }

// -----------------------------------------------------------------------------
// TPEngWriter::CurrentLength()
// -----------------------------------------------------------------------------
//
TInt TPEngWriter::CurrentLength()
    {
    return iBuffer.Length();
    }



// -----------------------------------------------------------------------------
// TPEngWriter::ReverseTo()
// -----------------------------------------------------------------------------
//
TInt TPEngWriter::ReverseTo( TInt aLength )
    {
    if ( aLength > iBuffer.Length() )
        {
        return KErrUnderflow;
        }

    iBuffer.SetLength( aLength );
    return KErrNone;
    }


//  End of File




