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
* Description:  Store API to retrieve presence attribute models.
*
*/

// INCLUDE FILES
#include "CPEngAttributeStore2.h"
#include "CPEngAttributeStore2Imp.h"
#include "PEngPresenceEngineConsts2.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngAttributeStore2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngAttributeStore2* CPEngAttributeStore2::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngAttributeStore2* self = CPEngAttributeStore2::NewLC( aNWSessionSlotID,
                                                              aPriority );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngAttributeStore2* CPEngAttributeStore2::NewLC(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngAttributeStore2* self = new ( ELeave ) CPEngAttributeStore2;
    CleanupStack::PushL( self );
    self->iImp = CPEngAttributeStore2Imp::NewL( aNWSessionSlotID,
                                                aPriority );
    return self;
    }



// Destructor
CPEngAttributeStore2::~CPEngAttributeStore2()
    {
    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngAttributeStore2::CPEngAttributeStore2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeStore2::CPEngAttributeStore2()
    {
    }



// -----------------------------------------------------------------------------
// CPEngAttributeStore2::GetCachedAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeStore2::GetCachedAttribute(
    const TDesC& aPresenceID,
    TUint32 aType,
    MPEngPresenceAttrModel2*& aModel ) const
    {
    return iImp->GetAttribute( aType,
                               aPresenceID,
                               aModel );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2::GetAndLockCachedAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeStore2::GetAndLockCachedAttribute(
    const TDesC& aPresenceID,
    TUint32 aType,
    MPEngPresenceAttrModel2*& aModel )
    {
    return iImp->GetAndLockAttribute( aType, aPresenceID, aModel );
    }



// -----------------------------------------------------------------------------
// CPEngAttributeStore2::StoreCachedAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeStore2::StoreCachedAttribute(
    MPEngPresenceAttrModel2& aModel,
    TBool aReleaseEditLock )
    {
    return iImp->StoreAttribute( aModel, aReleaseEditLock );
    }



// -----------------------------------------------------------------------------
// CPEngAttributeStore2::GetOwnAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeStore2::GetOwnAttribute(
    TUint32 aType,
    MPEngPresenceAttrModel2*& aModel ) const
    {
    return iImp->GetAttribute( aType, KPEngUserOwnPresenceId, aModel );
    }



// -----------------------------------------------------------------------------
// CPEngAttributeStore2::GetAndLockOwnAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeStore2::GetAndLockOwnAttribute(
    TUint32 aType,
    MPEngPresenceAttrModel2*& aModel )
    {
    return iImp->GetAndLockAttribute( aType, KPEngUserOwnPresenceId, aModel );
    }



// -----------------------------------------------------------------------------
// CPEngAttributeStore2::StoreOwnAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeStore2::StoreOwnAttribute(
    MPEngPresenceAttrModel2& aModel,
    TBool aReleaseEditLock )
    {
    return iImp->StoreAttribute( aModel, aReleaseEditLock );
    }



// -----------------------------------------------------------------------------
// CPEngAttributeStore2::UnLockOwnAttribute()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeStore2::UnLockOwnAttribute(
    MPEngPresenceAttrModel2& aModel )
    {
    return iImp->UnLockAttribute( aModel );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2::OwnAttributeAvailable()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeStore2::OwnAttributeAvailable( TUint32 aType ) const
    {
    return iImp->AttributeAvailable( aType, KPEngUserOwnPresenceId );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2::AttributeTypeSupported()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngAttributeStore2::AttributeTypeSupported( TUint32 aType ) const
    {
    return iImp->AttributeTypeSupported( aType );
    }



// End of File


