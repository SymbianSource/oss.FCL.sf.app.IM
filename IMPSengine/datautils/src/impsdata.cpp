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
* Description: Container class for stored IMPS engine inner classes
* 
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "impsdata.h"

// LOCAL CONSTANTS AND MACROS
const TInt KDefaultNumberOfPairs = 10;

// C++ default constructor can NOT contain any code, that
// might leave.
//

CImpsElement::CImpsElement( ) :
iBool(EFalse),
iValueType(EImpsDataTypeNone),
iIsEmpty(EFalse)
    {
    }

// Symbian OS default constructor can leave.
void CImpsElement::ConstructL( )
    {
    }

// Two-phased constructor.
CImpsElement* CImpsElement::NewL( )
    {
    CImpsElement* self = new (ELeave) CImpsElement;
    
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( );

    return self;
    }

// Destructor
CImpsElement::~CImpsElement( )
    {
    delete iKey;
    delete iString;
    delete iString8;
    }


// ---------------------------------------------------------
// CImpsElement::SetStringL
// Stores string with given key
// ---------------------------------------------------------
//
void CImpsElement::SetStringL(
    const CImpsKey* aKey, 
    const TDesC& aBuf )
    {
    iKey = aKey->CopyL(  );
    HBufC* newString = aBuf.AllocL( );
    iString = newString;
    iValueType = EImpsDataTypeDesc;
    }

// ---------------------------------------------------------
// CImpsElement::SetString8L
// Stores string8 with given key
// ---------------------------------------------------------
//
void CImpsElement::SetString8L(
    const CImpsKey* aKey, 
    const TDesC8& aBuf )
    {
    iKey = aKey->CopyL(  );
    HBufC8* newString = aBuf.AllocL( );
    iString8 = newString;
    iValueType = EImpsDataTypeDesc8;
    }

// ---------------------------------------------------------
// CImpsElement::SetIntL
// Stores integervalue with given key
// ---------------------------------------------------------
//
void CImpsElement::SetIntL(
    const CImpsKey* aKey, 
    TInt aInt )
    {
    iKey = aKey->CopyL(  );
    iInt = aInt;
    iValueType = EImpsDataTypeInt;
    }

// ---------------------------------------------------------
// CImpsElement::SetBooleanL
// Stores boolean value with given key
// ---------------------------------------------------------
//
void CImpsElement::SetBooleanL(
    const CImpsKey* aKey, 
    TBool aBool )
    {
    iKey = aKey->CopyL(  );
    iBool = aBool;
    iValueType = EImpsDataTypeBoolean;
    }

// ---------------------------------------------------------
// CImpsElement::SetEmptyL
// Stores Empty value with given key
// ---------------------------------------------------------
//
void CImpsElement::SetEmptyL(
    const CImpsKey* aKey )
    {
    iKey = aKey->CopyL(  );
    iIsEmpty = ETrue;
    iValueType = EImpsDataTypeNone;
    }

// ---------------------------------------------------------
// CImpsElement::Size
// Calculates the size of the element
// ---------------------------------------------------------
//
TInt CImpsElement::Size( ) const
    {
    TInt size = 4 * sizeof (TInt); // iInt, iBool, iIsEmpty, iValueType
    size += iKey->Size( );
    if( iString )
        size += iString->Size( ) + sizeof (TInt32);
    if( iString8 ) 
        size += iString8->Size( ) + sizeof (TInt32);
    return size;
    }

// ---------------------------------------------------------
// CImpsElement::Pack
// ---------------------------------------------------------
//
TInt CImpsElement::Pack(
    const TUint8*& aPtrStart ) const
    {
    Mem::Copy( (void*)aPtrStart, &iInt, sizeof( iInt ) );
    aPtrStart = aPtrStart + sizeof( iInt );

    Mem::Copy( (void*)aPtrStart, &iBool, sizeof( iBool ) );
    aPtrStart = aPtrStart + sizeof( iBool );

    Mem::Copy( (void*)aPtrStart, &iValueType, sizeof( iValueType ) );
    aPtrStart = aPtrStart + sizeof( iValueType );

    Mem::Copy( (void*)aPtrStart, &iIsEmpty, sizeof( iIsEmpty ) );
    aPtrStart = aPtrStart + sizeof( iIsEmpty );

    if( iValueType == EImpsDataTypeDesc )
        {
        TInt32 tempSize = 0;
        tempSize = iString->Size();
        Mem::Copy( (void*)aPtrStart, &tempSize, sizeof( tempSize) );
        aPtrStart = aPtrStart + sizeof( tempSize );
        if ( tempSize )
            {
            Mem::Copy( (void*)aPtrStart, iString->Ptr(), tempSize);
            aPtrStart = aPtrStart + tempSize;
            }
        }
    if( iValueType == EImpsDataTypeDesc8 )
        {
        TInt32 tempSize = 0;

        tempSize = iString8->Size();
        Mem::Copy( (void*)aPtrStart, &tempSize, sizeof( tempSize) );
        aPtrStart = aPtrStart + sizeof( tempSize );
        if ( tempSize )
            {
            Mem::Copy( (void*)aPtrStart, iString8->Ptr(), tempSize);
            aPtrStart = aPtrStart + tempSize ;
            }
        }
    TInt ret = iKey->Pack( aPtrStart );
    return ret;
    }

// ---------------------------------------------------------
// CImpsData::UnPack
// ---------------------------------------------------------
//
void CImpsElement::UnPackL(
    TUint8*& aPtr )
    { 
    Mem::Copy( &iInt, aPtr, sizeof( iInt ) );
    aPtr += sizeof( iInt );

    Mem::Copy( &iBool, aPtr, sizeof( iBool ) );
    aPtr += sizeof( iBool );

    Mem::Copy( &iValueType, aPtr, sizeof( iValueType ) );
    aPtr += sizeof( iValueType );

    Mem::Copy( &iIsEmpty, aPtr, sizeof( iIsEmpty ) );
    aPtr += sizeof( iIsEmpty );

    if( iValueType == EImpsDataTypeDesc )
        {
        const TUint8* textPtr = aPtr;
        TInt32 tempSize = 0;

        Mem::Copy( &tempSize, textPtr, sizeof( tempSize) );
        textPtr = textPtr + sizeof( tempSize );
        if ( tempSize > 0 )
            {
            // No need to put CleanupStack because of other methods do not leave.
            iString  = HBufC::NewL( tempSize / 2 );  
            Mem::Copy( (void*)iString->Ptr(), textPtr, tempSize );
            iString->Des().SetLength( tempSize / 2 ); 
            textPtr = textPtr + tempSize; 
            }
        if ( tempSize < 0 )
            {
            User::Leave( KErrCorrupt );
            }

        aPtr = (TUint8*) textPtr;
    

        }
    if( iValueType == EImpsDataTypeDesc8 )
        {
        const TUint8* textPtr = aPtr;
        TInt32 tempSize = 0;

        Mem::Copy( &tempSize, textPtr, sizeof( tempSize) );
        textPtr = textPtr + sizeof( tempSize );
        if ( tempSize > 0 )
            {
            iString8 = HBufC8::NewL( tempSize );  
            Mem::Copy( (void*)iString8->Ptr(), textPtr, tempSize );
            iString8->Des().SetLength( tempSize ); 
            textPtr = textPtr + tempSize; 
            }
        if ( tempSize < 0 )
            {
            User::Leave( KErrCorrupt );
            }

        aPtr = (TUint8*) textPtr;
        }

    iKey = CImpsKey::NewL( );
    iKey->UnPackL( aPtr );
    }


#ifdef _DEBUG

// ---------------------------------------------------------
// CImpsElement::DumpToFile
// ---------------------------------------------------------
//
void CImpsElement::DumpToFileL( RFile& aFile ) const
    {
    _LIT8( KImpsElementDumpBegin, "  Element dump begin\r\n");
    aFile.Write( KImpsElementDumpBegin );

    _LIT8(KIntegerValue,"    Integer value = %d\r\n");
    _LIT8(KDescStart,"    ");
    _LIT8(KDescEnd,"\r\n");
    TBuf8<256> message; 
    HBufC8* messageString;

    switch ( iValueType )
        {
        case EImpsDataTypeNone:
            aFile.Write( _L8( "    Impselement type is EImpsDataTypeNone\r\n" ));
            break;
        case EImpsDataTypeInt:
            aFile.Write( _L8( "    Impselement type is EImpsDataTypeInt\r\n" ));
            message.Format( KIntegerValue, iInt );
            aFile.Write( message );
            break;
        case EImpsDataTypeDesc:
            aFile.Write( _L8( "    Impselement type is EImpsDataTypeDesc\r\n" ));
            aFile.Write( KDescStart );
//            message.Copy( iString->Des() );
            messageString = HBufC8::NewL(iString->Length());
            messageString->Des().Copy(iString->Des());
            aFile.Write( _L8( "Value: " )  );
            aFile.Write(messageString->Des());
            aFile.Write( KDescEnd );
            delete messageString;
            break;
        case EImpsDataTypeDesc8:
            aFile.Write( _L8( "    Impselement type is EImpsDataTypeDesc8\r\n" ));
            aFile.Write( KDescStart );
//            message.Copy( iString8->Des() );
            aFile.Write( _L8( "Value: " ) );
//            aFile.Write( message );
            aFile.Write( iString8->Des() );
            aFile.Write( KDescEnd );
            break;
        case EImpsDataTypeBoolean:
            aFile.Write( _L8( "    Impselement type is EImpsDataTypeBoolean\r\n" ));
            if( iBool )
                aFile.Write( _L8( "    iBool = ETrue\r\n" ));
            else
                aFile.Write( _L8( "    iBool = EFalse\r\n" ));
            break;
        case EImpsDataTypeNotSupported:
            aFile.Write( _L8( "    Impselement type is EImpsDataTypeNotSupported\r\n" ));
            break;
        default:
            aFile.Write( _L8( "    Impselement type is unknown\r\n" ));
          break;
        }
  
    iKey->DumpToFileL( aFile );
    
    _LIT8( KImpsElementDumpEnd, "  Element dump end\r\n");
    aFile.Write( KImpsElementDumpEnd );
    }
#endif

// ---------------------------------------------------------
// CImpsElement::CopyL
// ---------------------------------------------------------
//
CImpsElement* CImpsElement::CopyL( ) const
    {
    CImpsElement* elem = CImpsElement::NewL( );
    CleanupStack::PushL( elem );
    elem->iBool = iBool;
    elem->iInt = iInt;
    elem->iValueType = iValueType;
    elem->iIsEmpty = iIsEmpty;
    elem->iKey = iKey->CopyL( );
    if( iString )
        elem->iString = iString->AllocL( );
    if( iString8 )
        elem->iString8 = iString8->AllocL( );
    CleanupStack::Pop( );
    return elem;
    }


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//

CImpsData::CImpsData()
    {
    }

// Symbian OS default constructor can leave.
void CImpsData::ConstructL( )
    {
    iElements = new (ELeave) CImpsElements( KDefaultNumberOfPairs );
    }

// Two-phased constructor.
CImpsData* CImpsData::NewL( )
    {
    CImpsData* self = new (ELeave) CImpsData;
    
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( );

    return self;
    }

// Destructor
CImpsData::~CImpsData( )
    {
    if ( iElements )
        {
        iElements->ResetAndDestroy( );
        delete iElements;
        }

    iElements = NULL;
    }

// ---------------------------------------------------------
// CImpsData::StoreStringL
// Stores string with given key
// ---------------------------------------------------------
//
void CImpsData::StoreStringL(
    const CImpsKey *aKey, 
    const TDesC& aBuff )
    {
    CImpsElement* aElement = CImpsElement::NewL( );
    CleanupStack::PushL( aElement );
    aElement->SetStringL( aKey, aBuff );
    iElements->AppendL( aElement );
    CleanupStack::Pop( );
    }

// ---------------------------------------------------------
// CImpsData::StoreString8L
// Stores string with given key
// ---------------------------------------------------------
//
void CImpsData::StoreString8L(
    const CImpsKey *aKey, 
    const TDesC8& aBuff )
    {
    CImpsElement* aElement = CImpsElement::NewL( );
    CleanupStack::PushL( aElement );
    aElement->SetString8L( aKey,aBuff );
    iElements->AppendL( aElement );
    CleanupStack::Pop( );
    }

// ---------------------------------------------------------
// CImpsData::StoreInt
// Stores interger value with given key
// ---------------------------------------------------------
//
void CImpsData::StoreIntL(
    const CImpsKey *aKey, 
    TInt aInt )
    {
    CImpsElement* aElement = CImpsElement::NewL( );
    CleanupStack::PushL( aElement );
    aElement->SetIntL( aKey,aInt );
    iElements->AppendL( aElement );
    CleanupStack::Pop( );
    }

// ---------------------------------------------------------
// CImpsData::StoreBooleanL
// Stores boolean value with given key
// ---------------------------------------------------------
//
void CImpsData::StoreBooleanL(
    const CImpsKey *aKey, 
    TBool aBool )
    {
    CImpsElement* aElement = CImpsElement::NewL( );
    CleanupStack::PushL( aElement );
    aElement->SetBooleanL( aKey,aBool );
    iElements->AppendL( aElement );
    CleanupStack::Pop( );
    }

// ---------------------------------------------------------
// CImpsData::StoreEmptyL
// Stores empty value with given key
// ---------------------------------------------------------
//
void CImpsData::StoreEmptyL(
    const CImpsKey *aKey )
    {
    CImpsElement* aElement = CImpsElement::NewL( );
    CleanupStack::PushL( aElement );
    aElement->SetEmptyL( aKey );
    iElements->AppendL( aElement );
    CleanupStack::Pop( );
    }

// ---------------------------------------------------------
// CImpsData::RestoreString
// Restores the string with given key
// Return EFalse if not found
// ---------------------------------------------------------
//
TBool CImpsData::RestoreString(
    const CImpsKey *aKey,
    TDesC*& aBuff ) const
    {
    const CImpsElement* aElement = Find( aKey, EImpsDataTypeDesc );
    if( aElement )
        {
        aBuff = aElement->GetString();
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CImpsData::RestoreString8
// Restores the string with given key
// Return EFalse if not found
// ---------------------------------------------------------
//
TBool CImpsData::RestoreString8(
    const CImpsKey *aKey,
    TDesC8*& aBuff )
    {
    const CImpsElement* aElement = Find( aKey, EImpsDataTypeDesc8 );
    if( aElement )
        {
        aBuff = aElement->GetString8();
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CImpsData::RestoreInt
// Restores the integer value with given key
// Return EFalse if not found
// ---------------------------------------------------------
//
TBool CImpsData::RestoreInt(
    const CImpsKey *aKey,
    TInt& aInt )
    {
    const CImpsElement* aElement = Find( aKey, EImpsDataTypeInt );
    if( aElement )
        {
        aInt = aElement->GetInt();
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CImpsData::RestoreBoolean
// Restores the boolean value with given key
// Return EFalse if not found
// ---------------------------------------------------------
//
TBool CImpsData::RestoreBoolean(
    const CImpsKey *aKey,
    TBool& aBoolean )
    {
    const CImpsElement* aElement = Find( aKey, EImpsDataTypeBoolean );
    if( aElement )
        {
        aBoolean = aElement->GetBoolean();
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CImpsData::RestoreEmpty
// Return EFalse if not found
// ---------------------------------------------------------
//
TBool CImpsData::RestoreEmpty(
    const CImpsKey *aKey )
    {
    const CImpsElement* aElement = Find( aKey, EImpsDataTypeNone );
    if( aElement )
        {
        return aElement->IsEmpty();
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CImpsData::CheckKey
// Return NULL if not found
// ---------------------------------------------------------
//
const CImpsKey* CImpsData::CheckKey(
    const CImpsKey *aKey,
    TImpsKeyOptions aOpt ) const
    {
    for( TInt i = 0; i < iElements->Count( ); i++ )
        {
        const CImpsElement* aTmp = (*iElements)[i];
        const CImpsKey* aTmpKey = aTmp->GetKey( );
        if( aTmpKey->CheckKey( aKey,aOpt ) )
            return aTmpKey;
        }
    return NULL;
    }

// ---------------------------------------------------------
// CImpsData::Find
// ---------------------------------------------------------
//
const CImpsElement* CImpsData::Find( const CImpsKey *aKey, TImpsDataType aType) const
    {
    for( TInt i = 0; i < iElements->Count(); i++)
        {
        const CImpsElement* tmp = (*iElements)[i];
        if(tmp->GetType( ) == aType) // Check for type little faster
            {
            if(*tmp->GetKey()==*aKey)
                return tmp;
            }
        }
    return NULL;
    }

// ---------------------------------------------------------
// CImpsData::Size
// ---------------------------------------------------------
//
TInt CImpsData::Size( ) const
    {
    TInt size = sizeof (TInt); // Count value
    for( TInt i = 0; i < iElements->Count(); i++)
        {
        const CImpsElement* tmp = (*iElements)[i];
        size += tmp->Size( );
        }
    return size;
    }

// ---------------------------------------------------------
// CImpsData::Pack
// ---------------------------------------------------------
//
TInt CImpsData::Pack(
    const TUint8*& aPtrStart ) const
    {
    TInt count = Count();
    Mem::Copy( (void*)aPtrStart, &count, sizeof( count ) );
    aPtrStart = aPtrStart + sizeof( count );
  
    TInt ret = KErrNone;
    for ( TInt i = 0; i < count; i++)
        {
        const CImpsElement* tmp = (*iElements)[i];
        ret = tmp->Pack( aPtrStart );
        }
    return ret;
    }

// ---------------------------------------------------------
// CImpsData::UnPack
// ---------------------------------------------------------
//
void CImpsData::UnPackL(
    TUint8*& aPtr )
    {
    TInt elements = 0;   //number of elements in list
    Mem::Copy( &elements, aPtr, sizeof( elements ) );
    aPtr += sizeof( elements );

    for (TInt i = 0; i < elements; i++)
        {
        CImpsElement* aElement = CImpsElement::NewL( );
        CleanupStack::PushL( aElement );
        aElement->UnPackL( aPtr );
        iElements->AppendL( aElement );
        CleanupStack::Pop( );
        }
    }

#ifdef _DEBUG

// ---------------------------------------------------------
// CImpsData::DumpToFileL
// ---------------------------------------------------------
//
void CImpsData::DumpToFileL( RFs& aSession, const TDesC& aFileName ) const
    {
    RFile file;
    User::LeaveIfError( file.Replace( aSession, aFileName, EFileWrite ) );
    _LIT8( KImpsDataDumpBegin, "ImpsData dump begin\r\n");
    file.Write( KImpsDataDumpBegin );

    TInt count = Count();

    _LIT8(KNumberOfElements,"Impsdata number of elements = %d\r\n");
    _LIT8(KElementNumber,"Element number = %d\r\n");
    TBuf8<80> message; 
    message.Format( KNumberOfElements, count );
    file.Write( message );
    
    for ( TInt i = 0; i < count; i++)
        {
        const CImpsElement* tmp = (*iElements)[i];
        message.Format( KElementNumber, i );
        file.Write( message );
        tmp->DumpToFileL( file );
        }
    
    _LIT8( KImpsDataDumpEnd, "ImpsData dump end\r\n");
    file.Write( KImpsDataDumpEnd );
    file.Close( );
    }
#endif

// ---------------------------------------------------------
// CImpsData::CopyL
// ---------------------------------------------------------
//
void CImpsData::CopyL( const CImpsKey* aKey, const CImpsData* aSource )
    {
    TInt count = aSource->Count();
    for ( TInt i = 0; i < count; i++)
        {
        const CImpsElements* aTmp = aSource->iElements;
        const CImpsElement* tmpElem = (*aTmp)[i];
        const CImpsKey* aTmpKey = tmpElem->GetKey( );
        if( !aKey || aTmpKey->CheckKey( aKey,EPartialKey ) )
            {
            CImpsElement* elem = tmpElem->CopyL( );
            iElements->AppendL( elem );
            }
        }
    }

// ---------------------------------------------------------
// CImpsData::SetKeyIndex
// ---------------------------------------------------------
//
void CImpsData::SetKeyIndex( const CImpsKey* aKey, TInt aNewIndex )
    {
    TInt count = Count();
    for ( TInt i = 0; i < count; i++)
        {
        CImpsElement* tmpElem = (*iElements)[i];
        CImpsKey* aTmpKey = tmpElem->GetKey( );
        if( aTmpKey->CheckKey( aKey,EPartialKey ) )
            {
            aTmpKey->SetElementIndex( aKey->Count( ) - 1 , aNewIndex );
            }
        }
    }

//  End of File   

