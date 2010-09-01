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
#include "CPEngAttributeStore2Imp.h"
#include "CPEngNWSessionSlotID2Imp.h"
#include "CPEngNWSessionSlotStorageProxy.h"
#include "MPEngPresenceAttrManager.h"
#include "PEngAttrLibFactory.h"


#include <MPEngPresenceAttrModel2.h>
#include <CPEngNWSessionSlotID2.h>




// ============================ LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// AddOptionsByOwner()
// Adds the EPEngUserAttribute instance option bit if
// user presence ID is used.
// -----------------------------------------------------------------------------
//
void AddOptionsByOwner( TInt& aInstanceOptions, const TDesC& aPresenceID )
    {
    if ( aPresenceID == KPEngUserOwnPresenceId )
        {
        aInstanceOptions = aInstanceOptions | EPEngUserAttribute;
        }
    else
        {
        aInstanceOptions = aInstanceOptions | EPEngCreationNotAllowed;
        }
    }


// -----------------------------------------------------------------------------
// HandleLoadErrorsByOwnerL()
// For user own attribute, the not found error is ignored
// (default attribute data is returned).
// For other attributes (cached ones), it is handled.
// -----------------------------------------------------------------------------
//
void HandleLoadErrorsByOwnerL( TInt aLoadErr,
                               const TDesC& aPresenceID )
    {
    if ( aPresenceID == KPEngUserOwnPresenceId )
        {
        return;
        }

    User::LeaveIfError( aLoadErr );
    }




// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngAttributeStore2Imp::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngAttributeStore2Imp* CPEngAttributeStore2Imp::NewL(
    const CPEngNWSessionSlotID2& aNWSessionSlotID,
    TInt aPriority )
    {
    CPEngAttributeStore2Imp* self =
        new ( ELeave ) CPEngAttributeStore2Imp( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aNWSessionSlotID );
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngAttributeStore2Imp::~CPEngAttributeStore2Imp()
    {
    if ( iAttrManager )
        {
        iAttrManager->Close();
        }

    delete iUsedSlot;
    }



// -----------------------------------------------------------------------------
// CPEngAttributeStore2Imp::CPEngAttributeStore2Imp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngAttributeStore2Imp::CPEngAttributeStore2Imp( TInt aPriority )
        : iCActivePriority( aPriority )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2Imp::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeStore2Imp::ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    iUsedSlot = CPEngNWSessionSlotStorageProxy::NewL( aNWSessionSlotID );
    iAttrManager = PEngAttrLibFactory::AttributeManagerInstanceL( iUsedSlot->BaseId() );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2Imp::GetAttribute()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeStore2Imp::GetAttribute(
    TUint32 aType,
    const TDesC& aPresenceID,
    MPEngPresenceAttrModel2*& aModel ) const
    {
    aModel = NULL;
    TInt instanceOptions = EPEngStorableModel;
    AddOptionsByOwner( instanceOptions, aPresenceID );

    TRAPD( err,
        {
        MPEngPresenceAttrModel2* model = NULL;

        TInt loadErr = iAttrManager->LoadAttributeLC( model,
                                                      aPresenceID,
                                                      aType,
                                                      instanceOptions );
        HandleLoadErrorsByOwnerL( loadErr,
                                  aPresenceID );

        CleanupStack::Pop(); //model
        aModel = model;
        } );

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2Imp::GetAndLockAttribute()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeStore2Imp::GetAndLockAttribute(
    TUint32 aType,
    const TDesC& aPresenceID,
    MPEngPresenceAttrModel2*& aModel )
    {
    aModel = NULL;
    TInt instanceOptions = EPEngStorableModel |
                           EPEngEditLockedModel;
    AddOptionsByOwner( instanceOptions, aPresenceID );

    TRAPD( err,
        {
        MPEngPresenceAttrModel2* model = NULL;


        TInt loadErr = iAttrManager->LoadAttributeLC( model,
                                                      aPresenceID,
                                                      aType,
                                                      instanceOptions );

        HandleLoadErrorsByOwnerL( loadErr,
                                  aPresenceID );


        CleanupStack::Pop(); //model
        aModel = model;
        } );

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2Imp::StoreAttribute()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeStore2Imp::StoreAttribute(
    MPEngPresenceAttrModel2& aModel,
    TBool aReleaseEditLock )
    {
    TRAPD( err, iAttrManager->StoreEditLockedAttributeL( aModel ) );
    if ( aReleaseEditLock && ( err == KErrNone ) )
        {
        err = UnLockAttribute( aModel );
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2Imp::UnLockAttribute()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeStore2Imp::UnLockAttribute(
    MPEngPresenceAttrModel2& aModel )
    {
    return iAttrManager->StopEditSupport( aModel );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2Imp::AttributeAvailable()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeStore2Imp::AttributeAvailable( TUint32 aType,
                                                  const TDesC& aPresenceID ) const
    {
    TInt instanceOptions = EPEngStorableModel |
                           EPEngEditLockedModel;
    AddOptionsByOwner( instanceOptions, aPresenceID );


    TRAPD( err,
        {
        MPEngPresenceAttrModel2* model = NULL;
        iAttrManager->LoadAttributeLC( model,
                                       aPresenceID,
                                       aType,
                                       instanceOptions );

        CleanupStack::PopAndDestroy(); //model
        } );

    return err;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeStore2Imp::AttributeTypeSupported()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeStore2Imp::AttributeTypeSupported( TUint32 aType ) const
    {
    TArray< TUint32 > attributeTypes = iAttrManager->KnownAttributeTypes();
    const TInt typeCount = attributeTypes.Count();

    for ( TInt ii = 0; ii < typeCount; ii++ )
        {
        if ( attributeTypes[ ii ] == aType )
            {
            return KErrNone;
            }
        }

    return KErrNotSupported;
    }



// End of File


