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
* Description:  Tracked Presence ID.
*
*/

// INCLUDE FILES
#include <CPEngNWSessionSlotEvent2.h>
#include <E32Std.h>
#include "CPEngNWSessionSlotEventEntry.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEvent2::CPEngNWSessionSlotEvent2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotEvent2::CPEngNWSessionSlotEvent2()
    {
    }


// Destructor
CPEngNWSessionSlotEvent2::~CPEngNWSessionSlotEvent2()
    {
    delete iImp;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEvent2::NWSessionSlotID()
// -----------------------------------------------------------------------------
//
EXPORT_C const CPEngNWSessionSlotID2& CPEngNWSessionSlotEvent2::NWSessionSlotID() const
    {
    return *iImp->iNWSessionSlotID;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEvent2::Event()
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngNWSessionSlotEvent CPEngNWSessionSlotEvent2::Event() const
    {
    return iImp->iEvent;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEvent2::NWSessionSlotState()
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngNWSessionSlotState CPEngNWSessionSlotEvent2::NWSessionSlotState() const
    {
    return iImp->iNWSessionSlotState;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEvent2::CSPSessionState()
// -----------------------------------------------------------------------------
//
EXPORT_C TPEngNWSessionSlotState CPEngNWSessionSlotEvent2::CSPSessionState() const
    {
    return iImp->iCSPSessionState;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEvent2::EventInt()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotEvent2::EventInt() const
    {
    return iImp->iEventInt;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEvent2::EventDesc16()
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CPEngNWSessionSlotEvent2::EventDesc16() const
    {
    return iImp->iEventDesc;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEvent2::CloneLC()
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlotEvent2Copy* CPEngNWSessionSlotEvent2::CloneLC() const
    {
    CPEngNWSessionSlotEvent2Copy* clone = CPEngNWSessionSlotEvent2Copy::NewL();
    CleanupStack::PushL( clone );
    clone->Implementation()->CopyL( *iImp );
    return clone;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::Implementation()
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotEventEntry* CPEngNWSessionSlotEvent2::Implementation()
    {
    return iImp;
    }

const CPEngNWSessionSlotEventEntry* CPEngNWSessionSlotEvent2::Implementation() const
    {
    return iImp;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::NewL()
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotEvent2Copy* CPEngNWSessionSlotEvent2Copy::NewL()
    {
    CPEngNWSessionSlotEvent2Copy* self = new ( ELeave ) CPEngNWSessionSlotEvent2Copy;

    CleanupStack::PushL( self );
    self->iImp = CPEngNWSessionSlotEventEntry::NewL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEvent2Copy::CPEngNWSessionSlotEvent2Copy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotEvent2Copy::CPEngNWSessionSlotEvent2Copy()
    {
    }


// Destructor
CPEngNWSessionSlotEvent2Copy::~CPEngNWSessionSlotEvent2Copy()
    {
    }



//  End of File


