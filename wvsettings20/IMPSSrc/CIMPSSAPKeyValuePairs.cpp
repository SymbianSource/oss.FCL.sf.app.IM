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
* Description:  Container for individual key-value pairs
*
*/

// INCLUDE FILES
#include    "CIMPSSAPKeyValuePairs.h"
#include    "CIMPSSAPKeyValuePair.h"
#include    "CIMPSSAPLookupKeyValuePair.h"
#include    "IMPSSAPSettingsStorePanics.h"
#include    <e32std.h>

//CONSTANTS
const TInt KKeyValuePairGranurality = 5;


// LOCAL CONSTANTS AND MACROS
#define KEYWORD_ORDER TLinearOrder< CIMPSSAPKeyValuePair > ( CompareKeys )


// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CompareKeys()
// Comparison function for TLinearOrder
//
// @param aLhs The left hand side in comparison.
// @param aRhs The right hand side in comparison.
// @return Result from compare
// -----------------------------------------------------------------------------
//
TInt CompareKeys( const CIMPSSAPKeyValuePair& aLhs, const CIMPSSAPKeyValuePair& aRhs )
    {
    return aLhs.Key().Compare( aRhs.Key() );
    }



// -----------------------------------------------------------------------------
// SetValueToPairs()
// Templated algorithm to set or update new value to list of key values.
//
// @param aPairs The list of key-value pairs.
// @param aLookupKey The lookup key to use in searching.
// @param aKey The key for which to assign the value.
// @param aValue Templated data type to assign to key.
// @return Error code.
// -----------------------------------------------------------------------------
//
template< typename T >
TInt SetValueToPairs( RPointerArray< CIMPSSAPKeyValuePair >& aPairs,
                      CIMPSSAPLookupKeyValuePair& aLookupKey,
                      const TDesC& aKey,
                      const T& aValue )
    {
    aLookupKey.SetLookupKey( aKey );


    TInt err = KErrNone;
    TInt index = aPairs.FindInOrder( &aLookupKey, KEYWORD_ORDER );

    if ( index != KErrNotFound )
        {
        //Update existing
        err = aPairs[ index ]->SetValue( aValue );
        }

    else
        {
        //create new
        CIMPSSAPKeyValuePair* newPair = CIMPSSAPKeyValuePair::New( aKey );
        if ( !newPair )
            {
            err = KErrNoMemory;
            }
        else
            {
            //Insert new

            err = newPair->SetValue( aValue );
            if ( err == KErrNone )
                {
                err = aPairs.InsertInOrderAllowRepeats( newPair, KEYWORD_ORDER );
                }

            if ( err != KErrNone )
                {
                delete newPair;
                }

            }
        }
    aLookupKey.ResetLookupKey();
    return err;
    }




// -----------------------------------------------------------------------------
// GetValueFromPairs()
//
// Templated algorithm to get value from list of key values.
//
// @param aPairs The list of key-value pairs.
// @param aLookupKey The lookup key to use in searching.
// @param aKey The key for which to get the value.
// @param aValue Templated data type to get.
// @return Error code.
// -----------------------------------------------------------------------------
//
template< typename T >
TInt GetValueFromPairs( const RPointerArray< CIMPSSAPKeyValuePair >& aPairs,
                        CIMPSSAPLookupKeyValuePair& aLookupKey,
                        const TDesC& aKey,
                        T& aValue )
    {
    aLookupKey.SetLookupKey( aKey );


    TInt index = aPairs.FindInOrder( &aLookupKey, KEYWORD_ORDER );
    if ( index != KErrNotFound )
        {
        index = aPairs[ index ]->GetValue( aValue );
        }


    aLookupKey.ResetLookupKey();
    return index;
    }




// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSSAPKeyValuePairs* CIMPSSAPKeyValuePairs::NewL()
    {
    CIMPSSAPKeyValuePairs* self = new ( ELeave ) CIMPSSAPKeyValuePairs;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }



// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::~CIMPSSAPKeyValuePairs()
// Destructor
// -----------------------------------------------------------------------------
//
CIMPSSAPKeyValuePairs::~CIMPSSAPKeyValuePairs()
    {
    iPairs.ResetAndDestroy();
    delete iLookupKey;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::CIMPSSAPKeyValuePairs()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSAPKeyValuePairs::CIMPSSAPKeyValuePairs()
        : iPairs( KKeyValuePairGranurality )
    {
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::ConstructL)
// -----------------------------------------------------------------------------
//
void CIMPSSAPKeyValuePairs::ConstructL()
    {
    iLookupKey = CIMPSSAPLookupKeyValuePair::NewL();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::ObjectID()
// -----------------------------------------------------------------------------
//
TIMPSSapObjectID CIMPSSAPKeyValuePairs::ObjectID() const
    {
    return EIMPSSapObjKeyValuePairs;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::ObjectVersion()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePairs::ObjectVersion() const
    {
    return KIMPSSapObjVersionInitial;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::ExternalizeVersionDataL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPKeyValuePairs::ExternalizeVersionDataL( TInt aVersion,
                                                     RWriteStream& aStream ) const
    {
    switch ( aVersion )
        {
        case  KIMPSSapObjVersionInitial:
            {
            //Phase 1 key value objects

            const TInt count = iPairs.Count();
            aStream.WriteInt32L( count );
            for ( TInt ii = 0; ii < count; ii++ )
                {
                iPairs[ ii ]->ExternalizeL( aStream );
                }
            break;
            }

        default:
            {
            //Unknown version ID in externalization.
            Panic( EIMPSSetStoreUnknownObjectVersionExternalize );
            break;
            }
        };
    }

// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::InternalizeVersionDataL()
// -----------------------------------------------------------------------------
//
void CIMPSSAPKeyValuePairs::InternalizeVersionDataL( TInt aVersion,
                                                     RReadStream& aStream )
    {
    switch ( aVersion )
        {
        case  KIMPSSapObjVersionInitial:
            {
            //Phase 1 key value objects
            const TInt count = aStream.ReadInt32L();
            for ( TInt ii = 0; ii < count; ii++ )
                {
                CIMPSSAPKeyValuePair* pair = CIMPSSAPKeyValuePair::NewLC( aStream );
                User::LeaveIfError( iPairs.Append( pair ) );
                CleanupStack::Pop( pair );
                }

            break;
            }

        default:
            {
            //Unknown version ID in internalization.
            Panic( EIMPSSetStoreUnknownObjectVersionInternalize );
            break;
            }
        };
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::DataSize()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePairs::DataSize() const
    {
    //pairs count == 4 bytes
    TInt dataSize = 4;
    const TInt count = iPairs.Count();
    for ( TInt ii = 0; ii < count; ii++ )
        {
        dataSize += iPairs[ ii ]->DataSize();
        }

    return dataSize;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::Reset()
// -----------------------------------------------------------------------------
//
void CIMPSSAPKeyValuePairs::Reset()
    {
    iPairs.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::SetValueInt()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePairs::SetValueInt( const TDesC& aKey, TInt aValue )
    {
    return SetValueToPairs( iPairs, *iLookupKey, aKey, aValue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::SetValueDesC8()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePairs::SetValueDesC8( const TDesC& aKey, const TDesC8& aValue )
    {
    return SetValueToPairs( iPairs, *iLookupKey, aKey, aValue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::SetValueDesC16()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePairs::SetValueDesC16( const TDesC& aKey, const TDesC16& aValue )
    {
    return SetValueToPairs( iPairs, *iLookupKey, aKey, aValue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::GetValueInt()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePairs::GetValueInt( const TDesC& aKey, TInt& aValue )
    {
    return GetValueFromPairs( iPairs, *iLookupKey, aKey, aValue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::GetValueDesC8()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePairs::GetValueDesC8( const TDesC& aKey, TPtrC8& aValue )
    {
    return GetValueFromPairs( iPairs, *iLookupKey, aKey, aValue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::GetValueDesC16()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePairs::GetValueDesC16( const TDesC& aKey, TPtrC16& aValue )
    {
    return GetValueFromPairs( iPairs, *iLookupKey, aKey, aValue );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::DeletePair()
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPKeyValuePairs::DeletePair( const TDesC& aKey )
    {

    iLookupKey->SetLookupKey( aKey );

    TInt index = iPairs.FindInOrder( iLookupKey, KEYWORD_ORDER );
    if ( index != KErrNotFound )
        {
        delete iPairs[ index ];
        iPairs.Remove( index );
        index = KErrNone;
        }

    iLookupKey->ResetLookupKey();

    return index;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::Pairs()
// -----------------------------------------------------------------------------
//
const RPointerArray< CIMPSSAPKeyValuePair >& CIMPSSAPKeyValuePairs::Pairs() const
    {
    return iPairs;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPKeyValuePairs::Pairs()
// -----------------------------------------------------------------------------
//
RPointerArray< CIMPSSAPKeyValuePair >& CIMPSSAPKeyValuePairs::Pairs()
    {
    return iPairs;
    }

//  End of File

