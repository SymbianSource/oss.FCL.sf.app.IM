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
* Description:  Presence attribute constructor container.
*
*/

// INCLUDE FILES
#include "CPEngAttrConstructorCon.h"
#include "CPEngSessionSlotObject.h"
#include "CPEngPresenceAttrConstructor.h"
#include "MPEngPresenceAttrConstructorTypeImp.h"

#include <E32Std.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttrConstructorCon* CPEngAttrConstructorCon::NewL( CPEngSessionSlotObject& aOwner )
    {
    CPEngAttrConstructorCon* self = new( ELeave ) CPEngAttrConstructorCon( aOwner );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::CPEngAttrConstructorCon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttrConstructorCon::CPEngAttrConstructorCon( CPEngSessionSlotObject& aOwner )
        : iOwner( aOwner )
    {
    }


// Destructor
CPEngAttrConstructorCon::~CPEngAttrConstructorCon()
    {
    iConstructors.ResetAndDestroy();
    }



// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::AddConstructorL()
// -----------------------------------------------------------------------------
//
void CPEngAttrConstructorCon::AddConstructorL(
    MPEngPresenceAttrConstructorTypeImp* aTypeConstructor,
    TUint32 aType,
    TPEngAttributeOriginator aOrigin )
    {
    CPEngPresenceAttrConstructor* constructor = NULL;

    TRAPD( err,
           constructor = CPEngPresenceAttrConstructor::NewL( *this,
                                                             *aTypeConstructor,
                                                             aType,
                                                             aOrigin ) );
    if ( err != KErrNone )
        {
        delete aTypeConstructor;
        User::Leave( err );
        }


    CleanupStack::PushL( constructor );
    __AssertConstructorValidL( *constructor );
    iConstructors.AppendL( constructor );

    CleanupStack::Pop( constructor );
    }


// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::FindByType()
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrConstructor* CPEngAttrConstructorCon::FindByType( TUint32 aType )
    {
    const TInt count = iConstructors.Count();
    for ( TInt ii( 0 ); ii < count ; ii++ )
        {
        CPEngPresenceAttrConstructor* constructor = iConstructors[ ii ];
        if ( constructor->iType == aType )
            {
            return constructor;
            }
        }

    return NULL;
    }



// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::FindByTypeL()
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrConstructor& CPEngAttrConstructorCon::FindByTypeL( TUint32 aType )
    {
    CPEngPresenceAttrConstructor* constructor = FindByType( aType );
    if ( !constructor )
        {
        User::Leave( KErrUnknown );
        }

    return *constructor;
    }



// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::TypeArray()
// -----------------------------------------------------------------------------
//
TArray<TUint32> CPEngAttrConstructorCon::TypeArray() const
    {
    return TArray<TUint32>( GetTypeCount, GetTypePtr, ( const CBase* ) this );
    }


// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::OpenRef()
// -----------------------------------------------------------------------------
//
void CPEngAttrConstructorCon::OpenRef()
    {
    //Simply forward the open and close requests to owner

    iOwner.Open();  // CSI: 15,65 #
    }


// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::CloseRef()
// -----------------------------------------------------------------------------
//
void CPEngAttrConstructorCon::CloseRef()
    {
    //Simply forward the open and close requests to owner

    iOwner.Close();
    }



// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::__AssertConstructorValidL()
// -----------------------------------------------------------------------------
//
void CPEngAttrConstructorCon::__AssertConstructorValidL(
    const CPEngPresenceAttrConstructor& aConstructor )
    {
    //Check that each model type ID is unique
    __ASSERT_ALWAYS( !FindByType( aConstructor.iType ),
                     aConstructor.__DbgPanicL( EPEngAttributeTypeIDNotUnique,
                                               KErrAlreadyExists ) );


    //Check attribute XML properties
    TPtrC8 name( NULL, 0 );
    TPtrC8 nameSpace( NULL, 0 );
    TInt err = aConstructor.GetCurrentXmlNames( name, nameSpace );

    if ( aConstructor.Originator() == EPEngLocalAttribute )
        {
        __ASSERT_ALWAYS( ( name.Length() == 0 ) && ( nameSpace.Length() == 0 ),
                         aConstructor.__DbgPanicL( EPEngLocalAttributeTypeProperties,
                                                   KErrGeneral ) );
        __ASSERT_ALWAYS( ( err == KErrArgument ),
                         aConstructor.__DbgPanicL( EPEngLocalAttributeTypeProperties,
                                                   KErrGeneral ) );

        }
    else
        {
        User::LeaveIfError( err );

        __ASSERT_ALWAYS( ( name.Length() > 0 ) && ( nameSpace.Length() > 0 ),
                         aConstructor.__DbgPanicL( EPEngNetworAttributeTypeProperties,
                                                   KErrGeneral ) );
        }
    }



// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::GetTypeCount()
// -----------------------------------------------------------------------------
//
TInt CPEngAttrConstructorCon::GetTypeCount( const CBase* aPtr )
    {
    const CPEngAttrConstructorCon* self = static_cast<const CPEngAttrConstructorCon*>( aPtr );
    return self->iConstructors.Count();
    }



// -----------------------------------------------------------------------------
// CPEngAttrConstructorCon::GetTypePtr()
// -----------------------------------------------------------------------------
//
const TAny* CPEngAttrConstructorCon::GetTypePtr( const CBase* aPtr, TInt aIndex )
    {
    const CPEngAttrConstructorCon* self = static_cast<const CPEngAttrConstructorCon*>( aPtr );
    return &( ( self->iConstructors )[ aIndex ]->iType );
    }



//  End of File
