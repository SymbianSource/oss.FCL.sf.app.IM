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
* Description:  Property manager and value container
*
*/

//  INCLUDES
#include    "CPEngContactListProperty.h"
#include    "PEngStorageGlobals.h"
#include    "MPEngContactListProperties.h"
#include    "MPEngContactItem.h"
#include    <e32base.h>
#include    <s32strm.h>



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// PEngPropertyManager::GetProperty()
// -----------------------------------------------------------------------------
//
TInt PEngPropertyManager::GetProperty(
    const RPointerArray<CPEngContactListProperty>& aProperties,
    TUint aName,
    TUint aLevel,
    TInt& aValue )
    {
    TInt x( FindProperty( aProperties, aName, aLevel, ETInt ) );
    if ( x == KErrNotFound )
        {
        return KErrNotFound;
        }

    aValue = aProperties[ x ]->ValueInt();
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// PEngPropertyManager::GetProperty()
// -----------------------------------------------------------------------------
//
TInt PEngPropertyManager::GetProperty(
    const RPointerArray<CPEngContactListProperty>& aProperties,
    TUint aName,
    TUint aLevel,
    TPtrC8& aValue )
    {
    TInt x( FindProperty( aProperties, aName, aLevel, EDes8 ) );
    if ( x == KErrNotFound )
        {
        return KErrNotFound;
        }

    aValue.Set( aProperties[ x ]->ValueDes8() );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// PEngPropertyManager::GetProperty()
// -----------------------------------------------------------------------------
//
TInt PEngPropertyManager::GetProperty(
    const RPointerArray<CPEngContactListProperty>& aProperties,
    TUint aName,
    TUint aLevel,
    TPtrC16& aValue )
    {
    TInt x( FindProperty( aProperties, aName, aLevel, EDes16 ) );
    if ( x == KErrNotFound )
        {
        return KErrNotFound;
        }

    aValue.Set( aProperties[ x ]->ValueDes16() );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// PEngPropertyManager::PropertyOrZero()
// -----------------------------------------------------------------------------
//
TInt PEngPropertyManager::PropertyOrZero(
    const RPointerArray<CPEngContactListProperty>& aProperties,
    TUint aName,
    TUint aLevel )
    {
    TInt x( FindProperty( aProperties, aName, aLevel ) );
    if ( x == KErrNotFound )
        {
        return 0; // zero as default value
        }

    return aProperties[ x ]->ValueInt();
    }


// -----------------------------------------------------------------------------
// PEngPropertyManager::SetPropertyL()
// -----------------------------------------------------------------------------
//
void PEngPropertyManager::SetPropertyL(
    RPointerArray<CPEngContactListProperty>& aProperties,
    TUint aName,
    TUint aLevel,
    TInt aValue,
    TInt& aSize )
    {
    TInt x( FindProperty( aProperties, aName, aLevel ) );
    if ( x == KErrNotFound )
        {
        // does not exists yet, create it
        CPEngContactListProperty* prop = CPEngContactListProperty::NewLC( aName,
                                                                          aLevel );
        prop->SetValue( aValue );
        aProperties.AppendL( prop );
        CleanupStack::Pop( prop );
        aSize += prop->EntrySize();
        }

    else
        {
        aProperties[ x ]->SetValue( aValue );
        }
    }


// -----------------------------------------------------------------------------
// PEngPropertyManager::SetPropertyL()
// -----------------------------------------------------------------------------
//
void PEngPropertyManager::SetPropertyL(
    RPointerArray<CPEngContactListProperty>& aProperties,
    TUint aName,
    TUint aLevel,
    const TDesC8& aValue,
    TInt& aSize )
    {
    TInt x( FindProperty( aProperties, aName, aLevel ) );
    if ( x == KErrNotFound )
        {
        // does not exists yet, create it
        CPEngContactListProperty* prop = CPEngContactListProperty::NewLC( aName,
                                                                          aLevel );
        prop->SetValueL( aValue );
        aProperties.AppendL( prop );
        CleanupStack::Pop( prop );
        aSize += prop->EntrySize();
        }
    else
        {
        TInt dif( aProperties[ x ]->EntrySize() );
        aProperties[ x ]->SetValueL( aValue );
        dif -= aProperties[ x ]->EntrySize();
        aSize += dif;
        }
    }


// -----------------------------------------------------------------------------
// PEngPropertyManager::SetPropertyL()
// -----------------------------------------------------------------------------
//
void PEngPropertyManager::SetPropertyL(
    RPointerArray<CPEngContactListProperty>& aProperties,
    TUint aName,
    TUint aLevel,
    const TDesC16& aValue,
    TInt& aSize )
    {
    TInt x( FindProperty( aProperties, aName, aLevel ) );
    if ( x == KErrNotFound )
        {
        // does not exists yet, create it
        CPEngContactListProperty* prop = CPEngContactListProperty::NewLC( aName,
                                                                          aLevel );
        prop->SetValueL( aValue );
        aProperties.AppendL( prop );
        CleanupStack::Pop( prop );
        aSize += prop->EntrySize();
        }
    else
        {
        TInt dif( aProperties[ x ]->EntrySize() );
        aProperties[ x ]->SetValueL( aValue );
        dif -= aProperties[ x ]->EntrySize();
        aSize += dif;
        }
    }


// -----------------------------------------------------------------------------
// PEngPropertyManager::DeletePropertyL()
// -----------------------------------------------------------------------------
//
TBool PEngPropertyManager::DeletePropertyL(
    RPointerArray<CPEngContactListProperty>& aProperties,
    TUint aName,
    TUint aLevel,
    TInt& aSize )
    {
    TInt x( FindProperty( aProperties, aName, aLevel ) );
    if ( x != KErrNotFound )
        {
        aSize -= aProperties[ x ]->EntrySize();
        delete aProperties[ x ];
        aProperties.Remove( x );
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// PEngPropertyManager::FindProperty()
// -----------------------------------------------------------------------------
//
TInt PEngPropertyManager::FindProperty(
    const RPointerArray<CPEngContactListProperty>& aProperties,
    TUint aName,
    TUint aLevel )
    {
    TInt count( aProperties.Count() );
    for ( TInt x( 0 ) ; x < count  ; ++x )
        {
        if ( ( aProperties[ x ]->Name() == aName ) &&
             ( aProperties[ x ]->Level() == aLevel ) )
            {
            return x;
            }
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// PEngPropertyManager::FindProperty()
// -----------------------------------------------------------------------------
//
TInt PEngPropertyManager::FindProperty(
    const RPointerArray<CPEngContactListProperty>& aProperties,
    TUint aName,
    TUint aLevel,
    TPEngPropertyType aType )
    {
    TInt index( FindProperty( aProperties, aName, aLevel ) );
    if ( index != KErrNotFound )
        {
        if ( aProperties[ index ]->ValidateType( aType ) )
            {
            return index;
            }

        return KErrNotFound;
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// PEngPropertyManager::ExternalizePropertiesL()
// -----------------------------------------------------------------------------
//
void PEngPropertyManager::ExternalizePropertiesL(
    const RPointerArray<CPEngContactListProperty>& aProperties,
    RWriteStream& aStream,
    TInt aStoreType )
    {
    TInt count( aProperties.Count() );
    for ( TInt i( 0 ) ; i < count  ; ++i )
        {
        aProperties[ i ]->ExternalizeL( aStream, aStoreType );
        }

    // mark
    aStream.WriteInt32L( KErrNotFound );
    }


// -----------------------------------------------------------------------------
// PEngPropertyManager::InternalizePropertiesL()
// -----------------------------------------------------------------------------
//
void PEngPropertyManager::InternalizePropertiesL(
    RPointerArray<CPEngContactListProperty>& aProperties,
    RReadStream& aStream,
    TInt& aSize )
    {
    // size of one number, marker for each property
    aSize += 4;

    while ( KErrNotFound != aStream.ReadInt32L() )
        {
        CPEngContactListProperty* prop =
            CPEngContactListProperty::NewLC( aStream );
        aProperties.AppendL( prop );
        CleanupStack::Pop(); // prop
        // before each property is validation number, size 4
        aSize += ( prop->EntrySize() + 4 );
        }
    }



// ===============CPEngContactListProperty MEMBER FUNCTIONS ====================

// -----------------------------------------------------------------------------
// CPEngContactListProperty::CPEngContactListManager()
// -----------------------------------------------------------------------------
//
CPEngContactListProperty::CPEngContactListProperty()
    {
    }

// -----------------------------------------------------------------------------
// CPEngContactListProperty::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListProperty* CPEngContactListProperty::NewLC( TUint aName,
                                                           TUint aLevel )
    {
    CPEngContactListProperty* self = new( ELeave ) CPEngContactListProperty();
    CleanupStack::PushL( self );
    self->iName = aName;
    self->iLevel = aLevel;

    //Validate the given level
    StorageTypeForLevelL( aLevel );

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::NewLC()
// -----------------------------------------------------------------------------
//
CPEngContactListProperty* CPEngContactListProperty::NewLC( RReadStream& aStream )
    {
    CPEngContactListProperty* self = new( ELeave ) CPEngContactListProperty;
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;
    }


// Destructor
CPEngContactListProperty::~CPEngContactListProperty()
    {
    delete iTDes16Value;
    delete iTDes8Value;
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::Name()
// -----------------------------------------------------------------------------
//
TUint CPEngContactListProperty::Name() const
    {
    return iName;
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::Level()
// -----------------------------------------------------------------------------
//
TUint CPEngContactListProperty::Level() const
    {
    return iLevel;
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::ValueInt()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListProperty::ValueInt() const
    {
    return iTIntValue;
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::ValueDes16()
// -----------------------------------------------------------------------------
//
const TDesC16& CPEngContactListProperty::ValueDes16() const
    {
    return *iTDes16Value;
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::ValueDes8()
// -----------------------------------------------------------------------------
//
const TDesC8& CPEngContactListProperty::ValueDes8() const
    {
    return *iTDes8Value;
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::SetValueL()
// -----------------------------------------------------------------------------
//
void CPEngContactListProperty::SetValue( TUint aValue )
    {
    iTIntValue = aValue;
    iTIntValueValid = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::SetValueL()
// -----------------------------------------------------------------------------
//
void CPEngContactListProperty::SetValueL( const TDesC16& aValue )
    {
    HBufC16*    temp = aValue.AllocL();
    delete iTDes16Value;
    iTDes16Value = temp;
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::SetValueL()
// -----------------------------------------------------------------------------
//
void CPEngContactListProperty::SetValueL( const TDesC8& aValue )
    {
    HBufC8* temp = aValue.AllocL();
    delete iTDes8Value;
    iTDes8Value = temp;
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::ValidateType()
// -----------------------------------------------------------------------------
//
TBool CPEngContactListProperty::ValidateType( TPEngPropertyType aType ) const
    {
    switch ( aType )
        {
        case ETInt:
            {
            return iTIntValueValid;
            }


        case EDes8:
            {
            return reinterpret_cast< TInt > ( iTDes8Value );
            }


        case EDes16:
            {
            return reinterpret_cast< TInt > ( iTDes16Value );
            }

        default:
            {
            return EFalse;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListProperty::ExternalizeL( RWriteStream& aStream,
                                             TInt aType ) const
    {
    if ( StorageTypeForLevelL( iLevel ) != aType )
        {
        // nothing to store, type and level do not match
        return;
        }


    //Mark start
    aStream.WriteInt32L( KErrNone );
    aStream.WriteUint32L( iName );
    aStream.WriteUint32L( iLevel );


    //Int value
    aStream.WriteInt32L( iTIntValue );
    aStream.WriteInt32L( iTIntValueValid );


    //16 bit desc
    if ( iTDes16Value )
        {
        aStream.WriteInt32L( iTDes16Value->Length() );
        aStream << *iTDes16Value;
        }
    else
        {
        aStream.WriteInt32L( 0 ); // zero length of the buffer
        }


    //8 bit desc
    if ( iTDes8Value )
        {
        aStream.WriteInt32L( iTDes8Value->Length() );
        aStream << *iTDes8Value;
        }
    else
        {
        aStream.WriteInt32L( 0 ); // zero length of the buffer
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::EntrySize()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListProperty::EntrySize()
    {
    // 24 for storing 6 32bits numbers
    return 24 + ( iTDes16Value ? iTDes16Value->Length() : 0 )
           + ( iTDes8Value ? iTDes8Value->Length() : 0 );
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::InternalizeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListProperty::InternalizeL( RReadStream& aStream )
    {
    // mark read outside
    iName = aStream.ReadUint32L();
    iLevel = aStream.ReadUint32L();
    iTIntValue = aStream.ReadInt32L();
    iTIntValueValid = aStream.ReadInt32L();

    // read buffers
    delete iTDes16Value;
    iTDes16Value = NULL;
    TInt size( aStream.ReadInt32L() );
    if ( size != 0 )
        {
        iTDes16Value = HBufC16::NewL( aStream, size );
        }

    delete iTDes8Value;
    iTDes8Value = NULL;
    size = aStream.ReadInt32L();
    if ( size != 0 )
        {
        iTDes8Value = HBufC8::NewL( aStream, size );
        }
    }


// -----------------------------------------------------------------------------
// CPEngContactListProperty::StorageTypeForLevelL()
// -----------------------------------------------------------------------------
//
TInt CPEngContactListProperty::StorageTypeForLevelL( TUint aLevel )
    {
    if ( ( aLevel == KPEngCntLstPropertyNativePermanent ) ||
         ( aLevel == KPEngCntLstPropertyExternalPermanent ) ||
         ( aLevel == KPEngCntItemPropertyNativePermanent ) ||
         ( aLevel == KPEngCntItemPropertyExternalPermanent ) )
        {
        return EPEngStorageBasicPermanent;
        }

    if ( ( aLevel == KPEngCntLstPropertyNativeCached ) ||
         ( aLevel == KPEngCntLstPropertyExternalCached ) ||
         ( aLevel == KPEngCntItemPropertyNativeCached ) ||
         ( aLevel == KPEngCntItemPropertyExternalCached ) )
        {
        return EPEngStorageBasicCached;
        }

    User::Leave( KErrUnknown );
    return EPEngStorageBasicTemporary;
    }


// end of file

