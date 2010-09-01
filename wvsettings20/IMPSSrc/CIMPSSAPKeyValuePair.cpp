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
* Description:  Key - value container.
*
*/

// INCLUDE FILES
#include	<utf.h> // for unicode character conversions
#include	<centralrepository.h>
#include    "e32std.h"
#include    "CIMPSSAPKeyValuePair.h"
#include	"IMPSSAPSettingsStoreDefinitions.h"
#include	"IMPSSAPSettingsStorePanics.h"

//MACROS

//Helper macro to check value type in data set
#define REQUIRE_SET_VALUE_TYPE( aType ) \
        { \
        TBool _settable = ( iValueType == EValueTypeNone ) || \
                          ( iValueType == aType ); \
        if( !_settable )\
            {\
            return KErrGeneral;\
            }\
        } \
     
//Helper macro to check value type in data get
#define REQUIRE_GET_VALUE_TYPE( aType ) \
        { \
        if( iValueType == EValueTypeNone ) \
            { \
            return KErrNotFound; \
            } \
        TBool _readable = ( iValueType == aType ); \
        if( !_readable ) \
            { \
            return KErrGeneral; \
            } \
        } \
     

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::New()
// -----------------------------------------------------------------------------
//
CIMPSSAPKeyValuePair* CIMPSSAPKeyValuePair::New( const TDesC16& aKey )
    {
    //Non leaving two phased constructor!!!

    CIMPSSAPKeyValuePair* self = new CIMPSSAPKeyValuePair;	// CSI: 62 #
    if ( self )
        {
        self->iKey = aKey.Alloc();
        if ( !self->iKey )
            {
            delete self;
            self = NULL;
            }
        }

    return self;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::NewLC()
// -----------------------------------------------------------------------------
//
CIMPSSAPKeyValuePair* CIMPSSAPKeyValuePair::NewLC( RReadStream& aStream )
    {
    CIMPSSAPKeyValuePair* self = new ( ELeave ) CIMPSSAPKeyValuePair;
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    return self;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::NewLC()
// -----------------------------------------------------------------------------
//
CIMPSSAPKeyValuePair* CIMPSSAPKeyValuePair::NewLC( const TDesC& aKeyValuePairFlat )
    {
    CIMPSSAPKeyValuePair* self = new ( ELeave ) CIMPSSAPKeyValuePair;
    CleanupStack::PushL( self );
    self->ParseFlatKeyValuePairL( aKeyValuePairFlat );
    return self;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::~CIMPSSAPKeyValuePair()
// Destructor
// -----------------------------------------------------------------------------
//
CIMPSSAPKeyValuePair::~CIMPSSAPKeyValuePair()
    {
    Reset();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::CIMPSSAPKeyValuePair()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSAPKeyValuePair::CIMPSSAPKeyValuePair()
        : iValueType( EValueTypeNone )
    {
    }



// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::ConstructL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPKeyValuePair::ConstructL( RReadStream& aStream )
    {
    //Note! This implementation must be in sync with
    //ExternalizeL()

    //Extension space
    aStream.ReadInt32L();

    //Default data
    TInt keyLength = aStream.ReadInt32L();
    iKey = HBufC::NewL( keyLength );
    TPtr keyPtr( iKey->Des() );
    aStream.ReadL( keyPtr, keyLength );

    iValueType = static_cast< TValueType > ( aStream.ReadInt8L() );
    switch ( iValueType )
        {
        case EValueTypeInt:
            {
            iInt = aStream.ReadInt32L();
            break;
            }

        case EValueTypeDesC8:
            {
            TInt descLength = aStream.ReadInt32L();
            iBuf8 = HBufC8::NewL( descLength );
            TPtr8 descPtr8( iBuf8->Des() );
            aStream.ReadL( descPtr8, descLength );
            break;
            }


        case EValueTypeDesC16:
            {
            TInt descLength = aStream.ReadInt32L();
            iBuf16 = HBufC16::NewL( descLength );
            TPtr16 descPtr16( iBuf16->Des() );
            aStream.ReadL( descPtr16, descLength );
            break;
            }

        //FLOW THROUGH
        case EValueTypeNone:
        default:
            {
            break;
            }
        };
    }



// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::Reset()
// -----------------------------------------------------------------------------
//
void CIMPSSAPKeyValuePair::Reset()
    {
    switch ( iValueType )
        {
        case EValueTypeDesC8:
            {
            delete iBuf8;
            iBuf8 = NULL;
            break;
            }


        case EValueTypeDesC16:
            {
            delete iBuf16;
            iBuf16 = NULL;
            break;
            }

        //FLOW THROUGH
        case EValueTypeInt:
        case EValueTypeNone:
        default:
            {
            break;
            }
        };

    iValueType = EValueTypeNone;

    delete iKey;
    iKey = NULL;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::SetValue()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePair::SetValue( TInt aValue )
    {
    REQUIRE_SET_VALUE_TYPE( EValueTypeInt )

    TInt oldValue( iInt );
    iInt = aValue;
    iValueType = EValueTypeInt;

    TBool dataTooBig( IsDataTooBig() );
    TInt err( KErrNone );
    if ( dataTooBig )
        {
        iInt = oldValue;
        err = KErrTooBig;
        }
    __ASSERT_DEBUG( !err, Panic( EIMPSInvalidKeyValue ) );
    return err;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::Key()
// -----------------------------------------------------------------------------
//
const TDesC& CIMPSSAPKeyValuePair::Key() const
    {
    return *iKey;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::SetValue()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePair::SetValue( const TDesC8& aValue )
    {
    REQUIRE_SET_VALUE_TYPE( EValueTypeDesC8 )

    // Allocate memory for the new value
    HBufC8* tmp = aValue.Alloc();
    if ( !tmp )
        {
        return KErrNoMemory;
        }

    // Swap pointers
    HBufC8* tmp2( NULL );
    tmp2 = iBuf8;
    iBuf8 = tmp;
    iValueType = EValueTypeDesC8;

    TBool dataTooBig( IsDataTooBig() );
    TInt err( KErrNone );

    // Rollback if data too bog
    if ( dataTooBig )
        {
        iBuf8 = tmp2;
        delete tmp;
        err = KErrTooBig;
        }
    // Otherwise delete the old value
    else
        {
        delete tmp2;
        }


    __ASSERT_DEBUG( !err, Panic( EIMPSInvalidKeyValue ) );
    return err;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::SetValue()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePair::SetValue( const TDesC16& aValue )
    {
    REQUIRE_SET_VALUE_TYPE( EValueTypeDesC16 )

    // Allocate memory for the new value
    HBufC16* tmp = aValue.Alloc();
    if ( !tmp )
        {
        return KErrNoMemory;
        }

    // Swap pointers
    HBufC16* tmp2( NULL );
    tmp2 = iBuf16;
    iBuf16 = tmp;
    iValueType = EValueTypeDesC16;

    TBool dataTooBig( IsDataTooBig() );
    TInt err( KErrNone );

    // Rollback if data too bog
    if ( dataTooBig )
        {
        iBuf16 = tmp2;
        delete tmp;
        err = KErrTooBig;
        }
    // Otherwise delete the old value
    else
        {
        delete tmp2;
        }

    __ASSERT_DEBUG( !err, Panic( EIMPSInvalidKeyValue ) );
    return err;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::GetValue()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePair::GetValue( TInt& aValue ) const
    {
    REQUIRE_GET_VALUE_TYPE( EValueTypeInt )

    aValue = iInt;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::GetValue()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePair::GetValue( TPtrC8& aValue ) const
    {
    REQUIRE_GET_VALUE_TYPE( EValueTypeDesC8 )

    aValue.Set( *iBuf8 );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::GetValue()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePair::GetValue( TPtrC16& aValue ) const
    {
    REQUIRE_GET_VALUE_TYPE( EValueTypeDesC16 )

    aValue.Set( *iBuf16 );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPKeyValuePair::ExternalizeL( RWriteStream& aStream ) const
    {
    //Extension space
    aStream.WriteInt32L( 0 );

    //Default data
    aStream.WriteInt32L( iKey->Length() );
    aStream.WriteL( *iKey );
    aStream.WriteInt8L( iValueType );

    switch ( iValueType )
        {
        case EValueTypeInt:
            {
            aStream.WriteInt32L( iInt );
            break;
            }

        case EValueTypeDesC8:
            {
            aStream.WriteInt32L( iBuf8->Length() );
            aStream.WriteL( *iBuf8 );
            break;
            }


        case EValueTypeDesC16:
            {
            aStream.WriteInt32L( iBuf16->Length() );
            aStream.WriteL( *iBuf16 );
            break;
            }

        //FLOW THROUGH
        case EValueTypeNone:
        default:
            {
            break;
            }
        };
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::DataSize()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePair::DataSize() const
    {
    // extension space == 4 bytes
    // keyLength == 4 bytes
    // valueType == 1 bytes
    // ==> 9 bytes
    TInt dataSize = 9;

    dataSize += iKey->Length();

    switch ( iValueType )
        {
        case EValueTypeInt:
            {
            //Integer == 4 bytes
            dataSize += 4;
            break;
            }

        case EValueTypeDesC8:
            {
            //desc8 length == 4 bytes
            dataSize += 4;
            dataSize += iBuf8->Size();
            break;
            }

        case EValueTypeDesC16:
            {
            //desc16 length == 4 bytes
            dataSize += 4;
            dataSize += iBuf16->Size();
            break;
            }

        //FLOW THROUGH
        case EValueTypeNone:
        default:
            {
            break;
            }
        };

    return dataSize;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::DataSizeDec()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePair::DataSizeDec() const
    {

    TInt dataSize( iKey->Length() );

    switch ( iValueType )
        {
        case EValueTypeInt:
            {
            dataSize += KTIntMaxLengthTextual;
            break;
            }

        case EValueTypeDesC8:
            {
            dataSize += iBuf8->Length();
            break;
            }

        case EValueTypeDesC16:
            {
            dataSize += iBuf16->Length();
            break;
            }

        //FLOW THROUGH
        case EValueTypeNone:
        default:
            {
            break;
            }
        };

    return dataSize;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::IsDataTooBig()
// -----------------------------------------------------------------------------
//
TBool CIMPSSAPKeyValuePair::IsDataTooBig() const
    {
    return ( DataSizeDec() > ( NCentralRepositoryConstants::KMaxUnicodeStringLength -
                               2*KKeyValuePairFieldSeparator().Length() - 1 ) );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::KeyValuePairFlatLC()
// -----------------------------------------------------------------------------
//

HBufC* CIMPSSAPKeyValuePair::KeyValuePairFlatLC()
    {

    // reserve memory for 2 field separators
    HBufC* flatBuf = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );

    TPtr flatBufPtr( flatBuf->Des() );

    flatBufPtr.Append( *iKey );
    flatBufPtr.Append( KKeyValuePairFieldSeparator );
    flatBufPtr.AppendNum( iValueType, EDecimal );
    flatBufPtr.Append( KKeyValuePairFieldSeparator );

    switch ( iValueType )
        {
        case EValueTypeInt:
            {
            flatBufPtr.AppendNum( iInt );
            break;
            }

        case EValueTypeDesC8:
            {
            //convert to 16-bit
            TPtrC16 dataPtr16( ( TUint16* ) iBuf8->Ptr(), ( iBuf8->Size() / 2 ) );
            flatBufPtr.Append( dataPtr16 );
            break;
            }

        case EValueTypeDesC16:
            {
            flatBufPtr.Append( *iBuf16 );
            break;
            }

        //FLOW THROUGH
        case EValueTypeNone:
        default:
            {
            break;
            }
        };
    return flatBuf;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePair::ParseFlatKeyValuePairL()
// -----------------------------------------------------------------------------
//

void CIMPSSAPKeyValuePair::ParseFlatKeyValuePairL( const TDesC& aKeyValuePairFlat )
    {
    TInt offset( aKeyValuePairFlat.Find( KKeyValuePairFieldSeparator ) );
    TInt offset2( aKeyValuePairFlat.Mid( offset +
                                         KKeyValuePairFieldSeparator().Length() ).Find(
                      KKeyValuePairFieldSeparator )
                );

    if ( ( offset == KErrNotFound ) || ( offset2 == KErrNotFound ) )
        {
        //Incorrect format, leave
        User::Leave( KErrNotFound );
        }

    TPtrC key( aKeyValuePairFlat.Left( offset ) );

    iKey = key.AllocL();

    TPtrC valueType( aKeyValuePairFlat.Mid( offset +
                                            KKeyValuePairFieldSeparator().Length(), offset2 ) );
    TLex lexer( valueType );
    TInt valueTypeInt( 0 );
    User::LeaveIfError( lexer.Val( valueTypeInt ) );
    TPtrC value( aKeyValuePairFlat.Mid( offset + offset2 +
                                        2*KKeyValuePairFieldSeparator().Length() ) ); // skip 2 separators
    switch ( valueTypeInt )
        {
        case EValueTypeInt:
            {
            TLex lexer( value );
            TInt intValue( 0 );
            User::LeaveIfError( lexer.Val( intValue ) );
            User::LeaveIfError( SetValue( intValue ) );
            break;
            }
        case EValueTypeDesC8:
            {
            //convert to 8-bit
            TPtrC8 dataPtr8( ( TUint8* ) value.Ptr(), value.Size() );
            User::LeaveIfError( SetValue( dataPtr8 ) );
            break;
            }
        case EValueTypeDesC16:
            {
            User::LeaveIfError( SetValue( value ) );
            break;
            }

        //FLOW THROUGH
        case EValueTypeNone:
        default:
            {
            break;
            }
        }
    }










//  End of File

