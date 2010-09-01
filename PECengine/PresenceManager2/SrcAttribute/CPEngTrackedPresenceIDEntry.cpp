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
* Description:  Tracked Presence ID entry.
*
*/

// INCLUDE FILES
#include "CPEngTrackedPresenceIDEntry.h"

#include <CPEngTrackedPresenceID2.h>
#include <MPEngPresenceAttrModel2.h>
#include <E32Std.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceIDEntry* CPEngTrackedPresenceIDEntry::NewLC(
    const TDesC& aPresenceID,
    const TArray<TUint32>& aTypes,
    TInt aFilterFlag )
    {
    CPEngTrackedPresenceIDEntry* self = new ( ELeave ) CPEngTrackedPresenceIDEntry;
    CleanupStack::PushL( self );
    self->ConstructL( aPresenceID, aTypes, aFilterFlag );
    return self;
    }


// Destructor
CPEngTrackedPresenceIDEntry::~CPEngTrackedPresenceIDEntry()
    {
    delete iInterface;
    delete iPresenceID;
    iTrackedAttributes.Close();
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::CPEngTrackedPresenceIDEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceIDEntry::CPEngTrackedPresenceIDEntry()
        : iAttributesChanged( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::ConstructL()
// Symbian OS constructor.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDEntry::ConstructL( const TDesC& aPresenceID,
                                              const TArray<TUint32>& aTypes,
                                              TInt aFilterFlag )
    {
    iFilterFlag =  aFilterFlag;
    iInterface = new ( ELeave ) CPEngTrackedPresenceID2( *this );
    iPresenceID = aPresenceID.AllocL();

    AddTrackedAttributesL( aTypes );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::Interface()
// -----------------------------------------------------------------------------
//
CPEngTrackedPresenceID2& CPEngTrackedPresenceIDEntry::Interface()
    {
    return *iInterface;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::PresenceID()
// Tracked presence ID.
// -----------------------------------------------------------------------------
//
TPtrC CPEngTrackedPresenceIDEntry::PresenceID() const
    {
    return *iPresenceID;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::Compare()
// Tracked presence ID.
// -----------------------------------------------------------------------------
//
TInt CPEngTrackedPresenceIDEntry::Compare( const CPEngTrackedPresenceIDEntry& aLhs,
                                           const CPEngTrackedPresenceIDEntry& aRhs )
    {
    return aLhs.PresenceID().Compare( aRhs.PresenceID() );
    }



// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::IsOnlyTrackedAttribute()
// Tracked attributes.
// -----------------------------------------------------------------------------
//
TBool CPEngTrackedPresenceIDEntry::IsOnlyTrackedAttribute( TUint32 aType ) const
    {
    TBool result = EFalse;

    //Given attribute is only tracked attribute if
    // - there is just 1 tracked attribute
    // - and is is same as given
    if ( ( iTrackedAttributes.Count() == 1 ) &&
         ( iTrackedAttributes[ 0 ] == aType ) )
        {
        result = ETrue;
        }

    return result;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::TrackedAttributes()
// Tracked attributes.
// -----------------------------------------------------------------------------
//
TBool CPEngTrackedPresenceIDEntry::AttributesTracked() const
    {
    return ( iTrackedAttributes.Count() != 0 );
    }



// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::AddTrackedAttributesL()
// Tracked attributes.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDEntry::AddTrackedAttributesL( const TArray<TUint32>& aTypes )
    {
    const TInt typeCount( aTypes.Count() );
    for ( TInt typeIx = 0; typeIx < typeCount; typeIx++ )
        {
        TUint32 attributeLookup( aTypes[ typeIx ] );

        TInt index = KErrNotFound;
        TInt result = iTrackedAttributes.FindInUnsignedKeyOrder( attributeLookup,
                                                                 index );
        if ( result == KErrNotFound )
            {
            User::LeaveIfError( iTrackedAttributes.Insert( attributeLookup, index ) );
            }
        }
    }



// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::RemoveTrackedAttributes()
// Tracked attributes.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDEntry::RemoveTrackedAttributes( const TArray<TUint32>& aTypes )
    {
    const TInt typeCount( aTypes.Count() );
    for ( TInt typeIx = 0; typeIx < typeCount; typeIx++ )
        {
        TUint32 attributeLookup( aTypes[ typeIx ] );

        TInt index = iTrackedAttributes.FindInUnsignedKeyOrder( attributeLookup );
        if ( index != KErrNotFound )
            {
            iTrackedAttributes.Remove( index );
            }
        }


    if ( iTrackedAttributes.Count() == 0 )
        {
        iAttributesChanged = EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::TrackedAttributes()
// Tracked attributes.
// -----------------------------------------------------------------------------
//
TArray<TUint32> CPEngTrackedPresenceIDEntry::TrackedAttributes() const
    {
    return TArray< TUint32 >( TrackedAttributesCount,
                              TrackedAttributeAt,
                              this );
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::NextTrackedAttribute()
// Tracked attributes.
// -----------------------------------------------------------------------------
//
TUint32 CPEngTrackedPresenceIDEntry::NextTrackedAttribute( TInt& aIterIx ) const
    {
    TUint32 attribute = KPEngNullAttributeType;

    if ( iTrackedAttributes.Count() > aIterIx )
        {
        attribute = iTrackedAttributes[ aIterIx ];
        aIterIx++;
        }

    return attribute;
    }



// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::SetFilterFlag()
// Filter flags.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDEntry::SetFilterFlag( TInt aFlag )
    {
    iFilterFlag = aFlag;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::FilterFlag()
// Filter flags.
// -----------------------------------------------------------------------------
//
TInt CPEngTrackedPresenceIDEntry::FilterFlag() const
    {
    return iFilterFlag;
    }


void CPEngTrackedPresenceIDEntry::MarkAllAttributesChanged()
    {
    iAttributesChanged = ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::ResetChangeMarks()
// Attribute change marks.
// -----------------------------------------------------------------------------
//
void CPEngTrackedPresenceIDEntry::ResetChangeMarks()
    {
    iAttributesChanged = EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::AttributesChanged()
// Attribute change marks.
// -----------------------------------------------------------------------------
//
TBool CPEngTrackedPresenceIDEntry::AttributesChanged() const
    {
    return iAttributesChanged;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::NextChangedAttribute()
// Attribute change marks.
// -----------------------------------------------------------------------------
//
TUint32 CPEngTrackedPresenceIDEntry::NextChangedAttribute( TInt& aIterIx ) const
    {
    if ( iAttributesChanged )
        {
        return NextTrackedAttribute( aIterIx );
        }


    return KPEngNullAttributeType;
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::TrackedAttributesCount()
// -----------------------------------------------------------------------------
//
TInt CPEngTrackedPresenceIDEntry::TrackedAttributesCount( const CBase* aThis )
    {
    const CPEngTrackedPresenceIDEntry* self =
        static_cast< const CPEngTrackedPresenceIDEntry* > ( aThis );

    return self->iTrackedAttributes.Count();
    }


// -----------------------------------------------------------------------------
// CPEngTrackedPresenceIDEntry::TrackedAttributeAt()
// -----------------------------------------------------------------------------
//
const TAny* CPEngTrackedPresenceIDEntry::TrackedAttributeAt( const CBase* aThis,
                                                             TInt aIndex )
    {
    const CPEngTrackedPresenceIDEntry* self =
        static_cast< const CPEngTrackedPresenceIDEntry* > ( aThis );

    return &self->iTrackedAttributes[ aIndex ];
    }


//  End of File

