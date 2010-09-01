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
* Description:  Store API to retrieve contact list models.
*
*/

// INCLUDE FILES
#include "CPEngContactListStore2Imp.h"
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngSessionSlotId.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "PEngListLibraryFactory.h"
#include "MPEngContactListManager.h"
#include "MPEngContactListAdvance.h"


#include <CPEngNWSessionSlotID2.h>






// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngContactListStore2Imp* CPEngContactListStore2Imp::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngContactListStore2Imp* self =
        new ( ELeave ) CPEngContactListStore2Imp( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngContactListStore2Imp::~CPEngContactListStore2Imp()
    {
    if ( iCntLstManager )
        {
        iCntLstManager->Close();
        }

    delete iUsedSlot;
    }



// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::CPEngContactListStore2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngContactListStore2Imp::CPEngContactListStore2Imp( TInt aPriority )
        : iCActivePriority( aPriority ),
        iDummy( KNullDesC )
    {
    }



// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngContactListStore2Imp::ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    iCntLstManager = PEngListLibraryFactory::ContactListManagerL( iUsedSlot->BaseId() );
    }







// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::AllContactListsL()
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngContactListStore2Imp::AllContactListsL() const
    {
    return iCntLstManager->AllContactListsL();
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::SynchronizedContactListsL()
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngContactListStore2Imp::SynchronizedContactListsL() const
    {
    return iCntLstManager->SynchronizedContactLists();
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::ContactListL()
// -----------------------------------------------------------------------------
//
MPEngContactList2& CPEngContactListStore2Imp::ContactListL(
    const TDesC& aContactList )
    {
    return iCntLstManager->ContactListL( aContactList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::CreateContactListL()
// -----------------------------------------------------------------------------
//
MPEngContactListProperties& CPEngContactListStore2Imp::CreateContactListL(
    const TDesC& aContactList,
    TPEngContactListBaseSettings& aBaseSettings )
    {
    return iCntLstManager->CreateContactListL( aContactList,
                                               aBaseSettings );
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::DeleteContactListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListStore2Imp::DeleteContactListL( const TDesC& aContactList )
    {
    iCntLstManager->DeleteContactListL( aContactList );
    }

// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::ChangeContactListTypeL()
// -----------------------------------------------------------------------------
//
void CPEngContactListStore2Imp::ChangeContactListTypeL(
    const TDesC& aContactList,
    TPEngContactListType aContactListType )
    {
    iCntLstManager->ChangeContactListTypeL( aContactList, aContactListType );
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::ContactListPropertiesL()
// -----------------------------------------------------------------------------
//
MPEngContactListProperties& CPEngContactListStore2Imp::ContactListPropertiesL(
    const TDesC& aContactList )
    {
    return iCntLstManager->ContactListSettingsL( aContactList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::SetAsDefaultContactListL()
// -----------------------------------------------------------------------------
//
void CPEngContactListStore2Imp::SetAsDefaultContactListL( const TDesC& aContactList )
    {
    iCntLstManager->SetAsDefaultCntListL( aContactList );
    }


// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::ReleaseContactList
// -----------------------------------------------------------------------------
//
TInt CPEngContactListStore2Imp::ReleaseContactList( const TDesC& aContactList )
    {
    iCntLstManager->ReleaseCntListModel( aContactList );
    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CPEngContactListStore2Imp::DummyList()
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngContactListStore2Imp::DummyList() const
    {
    return iDummy;
    }


// End of File


