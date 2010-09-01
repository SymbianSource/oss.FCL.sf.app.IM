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
* Description:  Templated attribute data helpers.
*
*/


// -----------------------------------------------------------------------------
// DoSetDescL()
// -----------------------------------------------------------------------------
//
template< class TBufType, class TDataType >
inline void DoSetDescL( TBufType*& aPtr, const TDataType& aData )
    {
    TBufType* tmp = aData.AllocL();
    delete aPtr;
    aPtr = tmp;
    }



// -----------------------------------------------------------------------------
// DoGetDesc()
// -----------------------------------------------------------------------------
//
template< class TBufType, class TPtrType >
inline void DoGetDesc( const TBufType* const& aBuffer, TPtrType& aData )
    {
    if( aBuffer )
        {
        aData.Set( *aBuffer );
        }
    else
        {
        aData.Set( NULL, 0 );
        }
    }



// -----------------------------------------------------------------------------
// DoGetBufExtSize()
// -----------------------------------------------------------------------------
//
template< class TBufType >
inline TInt DoGetBufExtSize( const TBufType* const& aBuffer )
    {
    TInt size( 4 ); //Buffer length indicator: 4 bytes

    if( aBuffer && aBuffer->Length() != 0 )
        {
        size += aBuffer->Size();
        }

    return size;
    }


// -----------------------------------------------------------------------------
// DoExtrBufL()
// -----------------------------------------------------------------------------
//
template< class TBufType >
inline void DoExtrBufL( const TBufType* const& aBuffer, RWriteStream& aStream )
    {
    if( aBuffer && aBuffer->Length() != 0 )
        {
        aStream.WriteInt32L( aBuffer->Length() );
        aStream.WriteL( *aBuffer );
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }
    }



// -----------------------------------------------------------------------------
// DoIntrBufL()
// -----------------------------------------------------------------------------
//
template< class TBufType, class TPtrType >
inline void DoIntrBufL( TBufType*& aBufPtr, TPtrType aPtr, RReadStream& aStream )
    {
    TBufType* buffer = NULL;

    TInt length = aStream.ReadInt32L();
    if( length != 0 )
        {
        buffer = TBufType::NewLC( length );
        aPtr.Set( buffer->Des() );
        aStream.ReadL( aPtr, length );
        CleanupStack::Pop(); //buffer
        }

    delete aBufPtr;
    aBufPtr = buffer;
    }


//  End of File

