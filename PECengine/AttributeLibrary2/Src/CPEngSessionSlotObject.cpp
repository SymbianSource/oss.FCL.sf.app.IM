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
* Description:  Base class for session slot, reference counted objects.
*
*/

// INCLUDE FILES
#include <E32Std.h>
#include "CPEngSessionSlotObject.h"
#include "CPEngSessionSlotObjectCon.h"
#include "CPEngSessionSlotId.h"


//LOCAL constants
namespace
    {
    _LIT( KPEngSlotObjectPanic, "AttrSlotObj" );
    enum TPEngSlotObjectPanicReasons
        {
        ESlotObjAccessCountNegative,
        ESlotObjHasAlreadyOwner,
        };

    void PanicSlotObj( TPEngSlotObjectPanicReasons aPanicReason )
        {
        User::Panic( KPEngSlotObjectPanic, aPanicReason );
        }
    }




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngSessionSlotObject::CPEngSessionSlotObject
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSessionSlotObject::CPEngSessionSlotObject()
        : iAccessCount( 1 )
    {
    }


// Destructor
CPEngSessionSlotObject::~CPEngSessionSlotObject()
    {
    delete iSlotId;

    if ( iContainer )
        {
        iContainer->Remove( this );
        }

    if ( iOwner )
        {
        iOwner->Close();
        }
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObject::Open()
// -----------------------------------------------------------------------------
//
void CPEngSessionSlotObject::Open()
    {
    iAccessCount++;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObject::Close()
// -----------------------------------------------------------------------------
//
void CPEngSessionSlotObject::Close()
    {
    __ASSERT_DEBUG( ( iAccessCount >= 1 ), PanicSlotObj( ESlotObjAccessCountNegative ) );

    iAccessCount--;

    if ( iAccessCount == 0 )
        {
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObject::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngSessionSlotObject::ConstructL( const CPEngSessionSlotId& aSlotId )
    {
    iSlotId = aSlotId.CloneL();
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObject::SlotId()
// -----------------------------------------------------------------------------
//
const CPEngSessionSlotId& CPEngSessionSlotObject::SlotId() const
    {
    return *iSlotId;
    }


// -----------------------------------------------------------------------------
// CPEngSessionSlotObject::SetOwner()
// -----------------------------------------------------------------------------
//
void CPEngSessionSlotObject::SetOwner( CObject& aOwner )
    {
    __ASSERT_ALWAYS( !iOwner, PanicSlotObj( ESlotObjHasAlreadyOwner ) );

    iOwner = &aOwner;
    TInt val = iOwner->Open();     // CSI: 65 #
    }



// -----------------------------------------------------------------------------
// CPEngSessionSlotObject::RefCount()
// -----------------------------------------------------------------------------
//
TInt CPEngSessionSlotObject::RefCount() const
    {
    return iAccessCount;
    }




//  End of File
