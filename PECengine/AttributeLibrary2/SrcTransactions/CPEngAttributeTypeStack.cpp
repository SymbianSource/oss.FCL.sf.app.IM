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
* Description:  Attribute type array.
*
*/

// INCLUDE FILES
#include <E32Std.h>
#include "CPEngAttributeTypeStack.h"
#include "MPEngPresenceAttrManager.h"


/**
 * Local macro for return value error handling.
 * If returned value is something else than KErrNone,
 * returns it. Else continues the execution.
 */
#define RETURN_IF_ERROR( op ) \
        { \
        TInt err = op; \
        if( err != KErrNone ) \
            { \
            return err;\
            } \
        } \
     



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::CPEngAttributeTypeStack
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeTypeStack::CPEngAttributeTypeStack()
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributeTypeStack* CPEngAttributeTypeStack::NewLC()
    {
    CPEngAttributeTypeStack* self = new ( ELeave ) CPEngAttributeTypeStack();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributeTypeStack* CPEngAttributeTypeStack::NewL()
    {
    CPEngAttributeTypeStack* self = NewLC();
    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::ConstructL()
// Symbian OS constructor.
// -----------------------------------------------------------------------------
//
void CPEngAttributeTypeStack::ConstructL()
    {
    iAttributes = new ( ELeave ) RArray< TPEngAttributeEntry >;
    iUniqueNs = new ( ELeave ) RArray< TPtrC8 >;
    }


// Destructor
CPEngAttributeTypeStack::~CPEngAttributeTypeStack()
    {
    if ( iAttributes )
        {
        iAttributes->Reset();
        }
    if ( iUniqueNs )
        {
        iUniqueNs->Reset();
        }

    delete iAttributes;
    delete iUniqueNs;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::Reset()
// -----------------------------------------------------------------------------
//
void CPEngAttributeTypeStack::Reset()
    {
    iAttributes->Reset();
    iUniqueNs->Reset();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::PushAttributesL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeTypeStack::PushAttributesL( const TArray<TUint32> aAttributeIds,
                                               MPEngPresenceAttrManager& aAttributeManager )
    {
    TInt attrbStartCount = iAttributes->Count();
    TInt nsStartCount = iUniqueNs->Count();

    const TInt pushCount( aAttributeIds.Count() );
    for ( TInt index( 0 ); index < pushCount; index++ )
        {
        //try push one attribute
        TInt err = DoPushAttribute( aAttributeIds[ index ], aAttributeManager );
        if ( err != KErrNone )
            {
            TInt ii;
            //rollback the attributes
            for ( ii = iAttributes->Count() - 1 ; ii >= attrbStartCount; ii-- )
                {
                iAttributes->Remove( ii );
                }

            //rollback the attribute namespaces
            for ( ii = iUniqueNs->Count() - 1 ; ii >= nsStartCount; ii-- )
                {
                iUniqueNs->Remove( ii );
                }

            //and handle the error
            User::Leave( err );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::PushAttributeL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeTypeStack::PushAttributeL( TUint32 aAttributeId,
                                              MPEngPresenceAttrManager& aAttributeManager )
    {
    User::LeaveIfError( DoPushAttribute( aAttributeId, aAttributeManager ) );
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::UniqueNameSpaceCount()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeTypeStack::UniqueNameSpaceCount() const
    {
    return iUniqueNs->Count();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::UniqueNameSpace()
// -----------------------------------------------------------------------------
//
const TDesC8& CPEngAttributeTypeStack::UniqueNameSpace( TInt aNameSpacesIndex ) const
    {
    return ( *iUniqueNs )[ aNameSpacesIndex ];
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::TypeCount()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeTypeStack::TypeCount() const
    {
    return iAttributes->Count();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::Name()
// -----------------------------------------------------------------------------
//
const TDesC8& CPEngAttributeTypeStack::Name( TInt aTypeCountIndex ) const
    {
    return ( *iAttributes )[ aTypeCountIndex ].iName;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::NameSpace()
// -----------------------------------------------------------------------------
//
const TDesC8& CPEngAttributeTypeStack::NameSpace( TInt aTypeCountIndex ) const
    {
    return ( *iAttributes )[ aTypeCountIndex ].iNS;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::TypeId()
// -----------------------------------------------------------------------------
//
TUint32 CPEngAttributeTypeStack::TypeId( TInt aTypeCountIndex ) const
    {
    return ( *iAttributes )[ aTypeCountIndex ].iTypeId;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::TypeIdArray()
// -----------------------------------------------------------------------------
//
TArray<TUint32> CPEngAttributeTypeStack::TypeIdArray() const
    {
    return TArray<TUint32>( GetTypeIdCount, GetTypeIdPtr, ( const CBase* ) this );
    }




// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::DoPushAttribut()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeTypeStack::DoPushAttribute( TUint32 aAttributeId,
                                               MPEngPresenceAttrManager& aAttributeManager )
    {
    //one attribute type can be here just once
    if ( !IsNewAttribute( aAttributeId ) )
        {
        return KErrAlreadyExists;
        }

    //add the attribute
    TPEngAttributeEntry entry;
    entry.iTypeId = aAttributeId;
    TInt error = aAttributeManager.GetAttributeXmlNameAndNameSpace( entry.iTypeId,
                                                                    entry.iName ,
                                                                    entry.iNS );
    RETURN_IF_ERROR( error );
    RETURN_IF_ERROR( iAttributes->Append( entry ) );

    error = iUniqueNs->Append( entry.iNS );
    if ( error != KErrNone )
        {
        //name space append failed, rollback the attribute entry
        iAttributes->Remove( iAttributes->Count() - 1 );
        return error;
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::IsNewNameSpace()
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeTypeStack::IsNewNameSpace( const TDesC8& aNameSpace ) const
    {
    const TInt nsCount = iUniqueNs->Count();
    for ( TInt index( 0 ); index < nsCount; index++ )
        {
        if ( UniqueNameSpace( index ).Compare( aNameSpace ) == 0 )
            {
            //requested name space is already used
            return EFalse;
            }
        }

    //requested name space isn't yet used
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::IsNewAttribute
// -----------------------------------------------------------------------------
//
TBool CPEngAttributeTypeStack::IsNewAttribute( TUint32 aAttributeId )
    {
    const TInt attrCount = iAttributes->Count();
    for ( TInt index( 0 ); index < attrCount; index++ )
        {
        if ( TypeId( index ) == aAttributeId )
            {
            //requested atribute id is already used
            return EFalse;
            }
        }

    //requested atribute id isn't yet used
    return ETrue;

    }



// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::GetTypeIdCount()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeTypeStack::GetTypeIdCount( const CBase* aPtr )
    {
    const CPEngAttributeTypeStack* self = static_cast<const CPEngAttributeTypeStack*>( aPtr );
    return self->TypeCount();
    }



// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::GetTypeIdPtr()
// -----------------------------------------------------------------------------
//
const TAny* CPEngAttributeTypeStack::GetTypeIdPtr( const CBase* aPtr, TInt aIndex )
    {
    const CPEngAttributeTypeStack* self = static_cast<const CPEngAttributeTypeStack*>( aPtr );
    return &( ( *self->iAttributes )[ aIndex ].iTypeId );
    }





// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CPEngAttributeTypeStack::TPEngAttributeEntry::TPEngAttributeEntry()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeTypeStack::TPEngAttributeEntry::TPEngAttributeEntry()
        :
        iTypeId( 0 ),
        iName( NULL, 0 ),
        iNS( NULL, 0 )
    {
    }




//  End of File
