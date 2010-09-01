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
#include "CPEngNWSessionSlotEventEntry.h"
#include "CPEngSessionSlotEvent.h"
#include "CPEngNWSessionSlotID2Imp.h"
#include <CPEngNWSessionSlotID2.h>
#include <E32Std.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEventEntry::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotEventEntry* CPEngNWSessionSlotEventEntry::NewL()
    {
    CPEngNWSessionSlotEventEntry* self = new ( ELeave ) CPEngNWSessionSlotEventEntry;
    CleanupStack::PushL( self );
    self->iNWSessionSlotID = CPEngNWSessionSlotID2::NewL();
    CleanupStack::Pop( self );
    return self;
    }




// Destructor
CPEngNWSessionSlotEventEntry::~CPEngNWSessionSlotEventEntry()
    {
    delete iNWSessionSlotID;
    delete iEventDescBuf;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEventEntry::CPEngNWSessionSlotEventEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotEventEntry::CPEngNWSessionSlotEventEntry()
    {
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEventEntry::CopyL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotEventEntry::CopyL( const CPEngNWSessionSlotEventEntry& aSource )
    {
    CPEngNWSessionSlotID2* newId = aSource.iNWSessionSlotID->CloneL();
    delete iNWSessionSlotID;
    iNWSessionSlotID = newId;

    iEvent = aSource.iEvent;
    iNWSessionSlotState = aSource.iNWSessionSlotState;
    iCSPSessionState = aSource.iCSPSessionState;
    iEventInt = aSource.iEventInt;

    //Carefully with the additional buffer.
    //Source may not always have the buffer, but
    //the TPtrC points always to valid data...
    HBufC* newBuf = aSource.iEventDesc.AllocL();
    delete iEventDescBuf;
    iEventDescBuf = newBuf;
    iEventDesc.Set( *iEventDescBuf );
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEventEntry::SetFullEventReferenceL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotEventEntry::SetFullEventReferenceL( const CPEngSessionSlotEvent& aInternalEvent )
    {
    iNWSessionSlotID->Implementation()->SetAllL( aInternalEvent.SessionSlotIndentification(),
                                                 aInternalEvent.ApplicationId() );

    iEvent = aInternalEvent.Event();
    iNWSessionSlotState = aInternalEvent.AppSessSltState();
    iCSPSessionState = aInternalEvent.GlobSessSltState();
    iEventInt = aInternalEvent.EventInt();

    iEventDesc.Set( aInternalEvent.EventDesc() );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEventEntry::SetPseudoEventReferenceL()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotEventEntry::SetPseudoEventReferenceL(
    const CPEngSessionSlotId& aBasePart,
    const TDesC& aAppId,
    TPEngNWSessionSlotState aAppState,
    TPEngNWSessionSlotState aCSPState )
    {
    //valid parts
    iNWSessionSlotID->Implementation()->SetAllL( aBasePart, aAppId );
    iNWSessionSlotState = aAppState;
    iCSPSessionState = aCSPState;

    //Pseudo data
    iEvent = EPEngEventNone;
    iEventInt = KErrNotFound;
    iEventDesc.Set( NULL, 0 );
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotEventEntry::ResetReferences()
// -----------------------------------------------------------------------------
//
void CPEngNWSessionSlotEventEntry::ResetReferences()
    {
    iNWSessionSlotID->Implementation()->Reset();
    iEventDesc.Set( NULL, 0 );
    delete iEventDescBuf;
    iEventDescBuf = NULL;
    }



//  End of File

