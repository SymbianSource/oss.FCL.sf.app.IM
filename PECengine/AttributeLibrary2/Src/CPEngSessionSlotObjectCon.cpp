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
* Description:  NWSessionSlot object container.
*
*/

// INCLUDE FILES
#include "CPEngSessionSlotObjectCon.h"
#include "CPEngSessionSlotObject.h"
#include "CPEngSessionSlotId.h"
#include <E32Std.h>



//LOCAL constants
namespace
    {
    _LIT( KPEngSlotObjectConPanic, "AttrSlotObjCon" );
    enum TPEngSlotObjectConPanicReasons
        {
        ESlotObjNotEmptyOnDestruction,
        ESlotObjConObjectAlreadyAdded,
        ESlotObjConObjectNULL,
        };

    void PanicSlotObjCon( TPEngSlotObjectConPanicReasons aPanicReason )
        {
        User::Panic( KPEngSlotObjectConPanic, aPanicReason );
        }
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSessionSlotObjectCon::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngSessionSlotObjectCon* CPEngSessionSlotObjectCon::NewL()
    {
    CPEngSessionSlotObjectCon* self = new( ELeave ) CPEngSessionSlotObjectCon;
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObjectCon::CPEngSessionSlotObjectCon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSessionSlotObjectCon::CPEngSessionSlotObjectCon()
    {
    }


// Destructor
CPEngSessionSlotObjectCon::~CPEngSessionSlotObjectCon()
    {
    __ASSERT_DEBUG( iObjects.Count() == 0,
                    PanicSlotObjCon( ESlotObjNotEmptyOnDestruction ) );
    iObjects.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObjectCon::FindById()
// -----------------------------------------------------------------------------
//
CPEngSessionSlotObject* CPEngSessionSlotObjectCon::FindById(
    const CPEngSessionSlotId& aSlotId )
    {
    return DoFindById( aSlotId );
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObjectCon::FindById()
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotObject* CPEngSessionSlotObjectCon::FindById(
    const CPEngSessionSlotId& aSlotId ) const
    {
    CPEngSessionSlotObjectCon* self = const_cast< CPEngSessionSlotObjectCon* > ( this );
    CPEngSessionSlotObject* object = self->DoFindById( aSlotId );
    return object;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObjectCon::AddL()
// -----------------------------------------------------------------------------
//
void CPEngSessionSlotObjectCon::AddL( CPEngSessionSlotObject* aObject )
    {
    __ASSERT_ALWAYS( aObject, PanicSlotObjCon( ESlotObjConObjectNULL ) );
    __ASSERT_ALWAYS( !aObject->iContainer, PanicSlotObjCon( ESlotObjConObjectAlreadyAdded ) );


    CPEngSessionSlotObject* object = FindById( aObject->SlotId() );
    __ASSERT_ALWAYS( !object, User::Leave( KErrAlreadyExists ) );

    iObjects.AppendL( aObject );
    aObject->iContainer = this;
    }



// -----------------------------------------------------------------------------
// CPEngSessionSlotObjectCon::Remove()
// -----------------------------------------------------------------------------
//
void CPEngSessionSlotObjectCon::Remove( CPEngSessionSlotObject* aObject )
    {
    const TInt objectCount = iObjects.Count();
    for ( TInt ii = 0; ii < objectCount; ii++ )
        {
        const CPEngSessionSlotObject* objectTrie = iObjects[ ii ];
        if ( objectTrie == aObject )
            {
            iObjects.Remove( ii );
            aObject->iContainer = NULL;
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObjectCon::Count()
// -----------------------------------------------------------------------------
//
TInt CPEngSessionSlotObjectCon::Count() const
    {
    return iObjects.Count();
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObjectCon::At()
// -----------------------------------------------------------------------------
//
CPEngSessionSlotObject& CPEngSessionSlotObjectCon::At( TInt aIndex )
    {
    return *iObjects[ aIndex ];
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObjectCon::DoFindById()
// -----------------------------------------------------------------------------
//
CPEngSessionSlotObject* CPEngSessionSlotObjectCon::DoFindById(
    const CPEngSessionSlotId& aSlotId )
    {
    CPEngSessionSlotObject* object = NULL;

    const TInt objectCount = iObjects.Count();
    for ( TInt ii = 0; ii < objectCount; ii++ )
        {
        CPEngSessionSlotObject* objectTrie = iObjects[ ii ];
        if ( objectTrie->SlotId().Match( aSlotId ) == KErrNone )
            {
            object = objectTrie;
            break;
            }
        }

    return object;
    }



//  End of File
