/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
* class for imps key.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "impskey.h"
#include    "impserrors.h"
#include    "impsconst.h"

#ifdef _DEBUG
#include    <f32file.h>
#endif

// CONSTANTS
const TInt KDefaultKeySize = 20;
const TInt KAddRoom        = 20;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CImpsKey::CImpsKey() :
iBuffer(NULL),
iCount(0)
    {
    }

// Symbian OS default constructor can leave.
void CImpsKey::ConstructL( )
    {
    iBuffer = HBufC16::NewL( KDefaultKeySize ); // Insert room for the elements
    }

// Two-phased constructor.
EXPORT_C CImpsKey* CImpsKey::NewL( )
    {
    CImpsKey* self = new (ELeave) CImpsKey;
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( );
    return self;
    }

// Two-phased constructor.
EXPORT_C CImpsKey* CImpsKey::NewLC( )
    {
    CImpsKey* self = new (ELeave) CImpsKey;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

    
// Destructor
EXPORT_C CImpsKey::~CImpsKey()
    {
    delete iBuffer;    
    }


// ---------------------------------------------------------
// CImpsKey::AddL
// Adds new element value the key
// ---------------------------------------------------------
//
void CImpsKey::AddL(
    const TInt aElemvalue ,
    enum TImpsKeyType aKeyType)
    {
    if( iBuffer->Length( ) + 2 > iBuffer->Des().MaxLength() )
        {
        iBuffer = iBuffer->ReAllocL( iBuffer->Length( ) + KAddRoom ); // Insert room
        }
    TPtr16 buffer = iBuffer->Des();
    buffer.AppendFill( 0,2 );
    TUint16 val = STATIC_CAST(TUint16,GET_INDEX(aElemvalue));
    if(aKeyType == EImpsKeyTypeIM)
        val &= 0x7FFF; // High bit is zero
    else
        val |= 0x8000; // High bit is one
    buffer[iCount*2] = val;  // Index value
    buffer[iCount*2+1] = STATIC_CAST(TUint16,GET_ENUM(aElemvalue));       // Enum value
    iCount++;
    }

// ---------------------------------------------------------
// CImpsKey::PopL
// Remove the last item from the key
// ---------------------------------------------------------
//
void CImpsKey::PopL( TInt aCount )
   {
    if( iCount - aCount < 0 )
        User::Leave( KImpsErrorKeyEmpty );
    TPtr16 buffer = iBuffer->Des( );
    TInt size = buffer.Length( );
    buffer.SetLength( size - 2*aCount );
    iCount -= aCount;
   }

// ---------------------------------------------------------
// CImpsKey::GetElementL
// Remove the last item from the key
// ---------------------------------------------------------
//
TInt CImpsKey::GetElementL( 
   const TInt aIndex, TImpsKeyType& aKeyType ) const
   {
    if( aIndex < 0 )
        User::Leave( KImpsErrorKeyIndexInvalid );
    if( iCount == 0 )  
        User::Leave( KImpsErrorKeyEmpty );
    if( aIndex>=iCount )
        User::Leave( KImpsErrorKeyIndexTooLarge );
    TInt highval = (*iBuffer)[aIndex*2];
    if( IS_PRESENCE( highval*0x10000 ) )
        aKeyType = EImpsKeyTypePre;
    else 
        aKeyType = EImpsKeyTypeIM;
    highval &= 0x7FFF; // Leave out the high bit
    TInt ret = STATIC_CAST( TInt, highval * 0x10000 + (*iBuffer)[aIndex*2+1] );
    return ret;
   }

// ---------------------------------------------------------
// CImpsKey::ReplaceLastL
// Replace last with new key
// ---------------------------------------------------------
//
void CImpsKey::ReplaceLastL( 
   const TInt aElemvalue, 
   enum TImpsKeyType aKeyType )
   {
    PopL( );
    AddL( aElemvalue, aKeyType );
   }

// ---------------------------------------------------------
// CImpsKey::Reset
// Reset to empty key
// ---------------------------------------------------------
//
void CImpsKey::Reset()
   {
    TPtr16 buffer = iBuffer->Des( );
    buffer.SetLength( 0 );
    iCount = 0;
    }

// ---------------------------------------------------------
// CImpsKey::Compare
// ---------------------------------------------------------
//
TInt CImpsKey::Compare( MImpsKey& aKey )
   {
   CImpsKey& alien = (CImpsKey&)aKey;
   return (*this==alien);
   }

// ---------------------------------------------------------
// CImpsKey::Copy
// Copy the key
// ---------------------------------------------------------
//
CImpsKey* CImpsKey::CopyL( ) const
    {
    CImpsKey* newKey = new (ELeave) CImpsKey;
    CleanupStack::PushL( newKey );
    HBufC16* newString = iBuffer->AllocL( );
    newKey->iBuffer = newString;
    newKey->iCount = iCount;
    CleanupStack::Pop( );
    return newKey;
    }

// ---------------------------------------------------------
// CImpsKey::CheckKey
// ---------------------------------------------------------
//
TBool CImpsKey::CheckKey(
    const CImpsKey *aKey,
    TImpsKeyOptions aOptions ) const
    {
    // Find the string from the buffer
    // if the string is not found return EFalse
    TInt index = iBuffer->Find( aKey->iBuffer->Des() );
    if(index == KErrNotFound )
      return EFalse;

    // If aOptions is PARTIALKEY and index is 0 we know that the
    // key matched to the start of the key.
    // If aOptionms is PARTIALKEYLONGER we want the key to little bit more
    // than just the plain key. The key is not wholekey
    // If aOptions is EWholeKey and the index is 0 and also the
    // lengths must be the same then we have macthed whole key
    switch ( aOptions )
        {
        case EPartialKey:
            if( index==0 )
              return ETrue;
            break;
        case EPartialKeyLonger:
            if( index==0 && aKey->Count( ) < Count( ))
              return ETrue;
            break;

        case EWholeKey:
            if( index == 0 && aKey->Count( ) == Count( ) )
              return ETrue;
            break;
        default:
          break;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CImpsKey::Size
// Size of the key
// ---------------------------------------------------------
//
TInt CImpsKey::Size( ) const
    {
    TInt size = sizeof (TInt); // iCount
    size += iBuffer->Size( );
    return size;
    }

// ---------------------------------------------------------
// CImpsKey::Pack
// Pack the key
// ---------------------------------------------------------
//
TInt CImpsKey::Pack(
    const TUint8*& aPtrStart ) const
    {
    Mem::Copy( (void*)aPtrStart, &iCount, sizeof( iCount ) );
    aPtrStart = aPtrStart + sizeof( iCount );
    
    if( iCount )
        {
//        TInt32 tempSize = 0;
/*
        tempSize = iBuffer->Size();
        Mem::Copy( (void*)aPtrStart, &tempSize, sizeof( tempSize) );
        aPtrStart = aPtrStart + sizeof( tempSize );
        if ( iCount )
            {
            Mem::Copy( (void*)aPtrStart, iBuffer->Ptr(), tempSize);
            aPtrStart = aPtrStart + tempSize ;
            }
*/
        //        tempSize = iBuffer->Size();
        //        Mem::Copy( (void*)aPtrStart, &tempSize, sizeof( tempSize) );
        //        aPtrStart = aPtrStart + sizeof( tempSize );
        if ( iCount )
            {
            Mem::Copy( (void*)aPtrStart, iBuffer->Ptr(), iCount*4);
            aPtrStart = aPtrStart + iCount*4 ;
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------
// CImpsKey::UnPackL
// Pack the key
// ---------------------------------------------------------
//
void CImpsKey::UnPackL(
    TUint8*& aPtr )
    {
    Mem::Copy( &iCount, aPtr, sizeof( iCount ) );
    aPtr += sizeof( iCount );
    if( iCount )
        {
        const TUint8* textPtr = aPtr;
//        TInt32 tempSize = 0;

/*
        Mem::Copy( &tempSize, textPtr, sizeof( tempSize) );
        textPtr = textPtr + sizeof( tempSize );
        if ( iCount > 0 )
            {
            delete iBuffer;
            iBuffer = NULL;
            iBuffer = HBufC16::NewL( tempSize );  
            Mem::Copy( (void*)iBuffer->Ptr(), textPtr, tempSize );
            iBuffer->Des().SetLength( tempSize ); 
            textPtr = textPtr + tempSize; 
*/
        //        Mem::Copy( &tempSize, textPtr, sizeof( tempSize) );
        //        textPtr = textPtr + sizeof( tempSize );
        if ( iCount > 0 )
            {
            delete iBuffer;
            iBuffer = NULL;
            iBuffer = HBufC16::NewL( iCount*2 );  
            Mem::Copy( (void*)iBuffer->Ptr(), textPtr, iCount*4 );
            iBuffer->Des().SetLength( iCount*2 ); 
            textPtr = textPtr + iCount*4; 
            }
        if ( iCount < 0 )
            {
            User::Leave( KErrCorrupt );
            }
        aPtr = (TUint8*) textPtr;
        }
    }


// ---------------------------------------------------------
// CImpsKey::Destroy
// Destroy the key
// ---------------------------------------------------------
//
void CImpsKey::Destroy()
    {
    delete this;
    }

#ifdef _DEBUG

// ---------------------------------------------------------
// CImpsKey::DumpToFileL
// ---------------------------------------------------------
//
void CImpsKey::DumpToFileL( RFile& aFile ) const
    {
    _LIT8( KImpsKeyDumpBegin, "      ImpsKey dump begin\r\n");
    aFile.Write( KImpsKeyDumpBegin );
  
    TInt count = iCount;
    TBuf8<256> message; 
    _LIT8(KCount,"        Impskey number of elements = %d\r\n");
    _LIT8(KElement,"       Impskey element %d\r\n");
    _LIT8(KElementIndex,"       Impskey element index %d\r\n");
    _LIT8(KElementEnum,"       Impskey element enum %x\r\n");

    message.Format( KCount, count );
    aFile.Write( message );


    for( TInt k = 0; k< count; k++ )
        {
          TImpsKeyType keyType;
          TInt elem = GetElementL( k, keyType );
          message.Format( KElement, k );
          aFile.Write( message );
          if( keyType == EImpsKeyTypeIM )
              {
              _LIT8( KImpsKeyTypeIM, "      ImpsKey type = EImpsKeyTypeIM\r\n");
              aFile.Write( KImpsKeyTypeIM );
              }
         else
              {
              _LIT8( KImpsKeyTypePresence, "      ImpsKey type = EImpsKeyTypePre\r\n");
              aFile.Write( KImpsKeyTypePresence );
              }
          message.Format( KElementIndex, GET_INDEX(elem) );
          aFile.Write( message );
          message.Format( KElementEnum, GET_ENUM(elem) );
          aFile.Write( message );
        }
    
    _LIT8( KImpsKeyDumpEnd, "      ImpsKey dump end\r\n");
    aFile.Write( KImpsKeyDumpEnd );
    }
#endif

// ---------------------------------------------------------
// CImpsKey::SetElementIndex
// ---------------------------------------------------------
//
void CImpsKey::SetElementIndex( TInt aCount, TInt aNewIndex )
    {
    if( iCount == 0 )  
        return;
    if( aCount < 0 )
        return;
    if( aCount >= iCount )
        return;
    TInt highval = (*iBuffer)[aCount*2];
    TUint16 val = STATIC_CAST(TUint16,aNewIndex );
    if( IS_PRESENCE( highval*0x10000 ) )
        val |= 0x8000;
    else 
        val &= 0x7FFF;
    TPtr16 buffer = iBuffer->Des();
    buffer[aCount*2] = val;  // Index value
    }

//  End of File  

