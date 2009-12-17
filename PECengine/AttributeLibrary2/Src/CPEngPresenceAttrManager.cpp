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
* Description:  Concrete MPEngPresenceAttrManager implementation.
*
*/

// INCLUDE FILES

#include "CPEngPresenceAttrManager.h"
#include "CPEngAttrConstructorCon.h"
#include "CPEngPresenceAttrModelStorageProxy.h"
#include "CPEngPresenceAttrConstructor.h"
#include "PEngAttributeStoreIDCodec.h"
#include "PEngAttributeRequestCodec.h"


#include "PEngStorageManager.h"
#include "MPEngStorageManager.h"
#include "PEngWVAttributeConstructorFactory.h"
#include "PresenceDebugPrint.h"


#include <E32Base.h>




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrManager* CPEngPresenceAttrManager::NewLC(
    const CPEngSessionSlotId& aSlotId )
    {
    CPEngPresenceAttrManager* self = new ( ELeave ) CPEngPresenceAttrManager;

    CleanupClosePushL( *self );
    self->ConstructL( aSlotId );

    return self;
    }


// Destructor
CPEngPresenceAttrManager::~CPEngPresenceAttrManager()
    {
    delete iTypeConstructors;
    if ( iStorageManager )
        {
        iStorageManager->Close();
        }
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::CPEngPresenceAttrManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrManager::CPEngPresenceAttrManager()
    {
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrManager::ConstructL( const CPEngSessionSlotId& aSlotId )
    {
    CPEngSessionSlotObject::ConstructL( aSlotId );
    iStorageManager = PEngStorageManager::GetStorageManagerL( aSlotId );

    iTypeConstructors  = CPEngAttrConstructorCon::NewL( *this );
    PEngWVAttributeConstructorFactory::CreateConstructorsL( *iTypeConstructors );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::Close()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrManager::Close()
    {
    CPEngSessionSlotObject::Close();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::InstantiateAttributeLC()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrManager::InstantiateAttributeLC(
    MPEngPresenceAttrModel2*& aModel,
    const TDesC& aPresenceID,
    TUint32 aType,
    TInt aInstanceOptions )

    {
    aModel = NULL;
    TBool userModel = ( aInstanceOptions & EPEngUserAttribute );

    CPEngPresenceAttrConstructor& constructor =
        iTypeConstructors->FindByTypeL( aType );

    //Attribute object holds a reference to attribute manager instance
    //==> method can't be const
    MPEngPresenceAdvancedAttrModel2* model = constructor.NewAttributeInstanceLC(
                                                 userModel,
                                                 aPresenceID );


    if ( ( aInstanceOptions & EPEngStorableModel ) )
        {
        HBufC* storeId = PEngAttributeStoreIDCodec::GenerateStoreIdL( aType, aPresenceID );
        model->Storage().ConnectStorageL( userModel, storeId, *iStorageManager );
        }

    if ( aInstanceOptions & EPEngEditLockedModel )
        {
        if ( aInstanceOptions & EPEngCreationNotAllowed )
            {
            model->Storage().GainEditLockL( EFalse );
            }
        else
            {
            model->Storage().GainEditLockL( ETrue );
            }
        }


    aModel = model;
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::LoadAttributeLC()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrManager::LoadAttributeLC(
    MPEngPresenceAttrModel2*& aModel,
    const TDesC& aPresenceID,
    TUint32 aType,
    TInt aInstanceOptions )

    {
    aModel = NULL;
    MPEngPresenceAttrModel2* model = NULL;
    InstantiateAttributeLC( model, aPresenceID, aType, aInstanceOptions );


    TInt loadStatus = KErrNone;
    if ( aInstanceOptions & EPEngStorableModel )
        {
        loadStatus = model->Advanced()->Storage().RetrieveFromStorageL();
        }


    aModel = model;
    return loadStatus;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::StoreEditLockedAttributeL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrManager::StoreEditLockedAttributeL( const MPEngPresenceAttrModel2& aModel )
    {
    ModelEditLockedFromThisSlotL( aModel );

    const CPEngPresenceAttrModelStorageProxy& storage = aModel.Advanced()->Storage();
    storage.StoreToStorageL();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::StopEditSupport()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrManager::StopEditSupport( MPEngPresenceAttrModel2& aModel ) const
    {
    CPEngPresenceAttrModelStorageProxy& storage = aModel.Advanced()->Storage();
    if ( !storage.IsFromThisStorage( *iStorageManager ) )
        {
        return KErrArgument;
        }

    return storage.StopEditSupport();
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::ModelEditLockedFromThisSlotL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrManager::ModelEditLockedFromThisSlotL(
    const MPEngPresenceAttrModel2& aModel ) const
    {
    const CPEngPresenceAttrModelStorageProxy& storage = aModel.Advanced()->Storage();

    //If the assert below fails, the model in question has been loaded from
    //another NWSessionSlot than where it is now tried to store or publish
    __ASSERT_ALWAYS( storage.IsFromThisStorage( *iStorageManager ),
                     User::Leave( KErrArgument ) );

    //If the assert below fails, the model in question has been
    //loaded without edit lock, and thus it can't be stored or published
    __ASSERT_ALWAYS( storage.IsEditLocked(),
                     User::Leave( KErrArgument ) );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::ValidNetworkAttribute()
// -----------------------------------------------------------------------------
//
TBool CPEngPresenceAttrManager::ValidNetworkAttribute( TUint32 aType ) const
    {
    const CPEngPresenceAttrConstructor* constructor = iTypeConstructors->FindByType( aType );
    if ( constructor )
        {
        TPEngAttributeOriginator attribOriginator = constructor->Originator();

        if ( ( attribOriginator == EPEngClientOriginated ) ||
             ( attribOriginator == EPEngServerOriginated ) ||
             ( attribOriginator == EPEngClientServerOriginated ) )
            {
            //attribute type is a valid network attribute
            return KErrNone;
            }

        else
            {
            //attribute has some other originator
            //==> it is not a valid network attribute
            return KErrNotSupported;
            }
        }

    //Unknown attribute type
    return KErrUnknown;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::KnownAttributeTypes()
// -----------------------------------------------------------------------------
//
TArray<TUint32> CPEngPresenceAttrManager::KnownAttributeTypes() const
    {
    return iTypeConstructors->TypeArray();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::GetAttributeXmlNameAndNameSpace()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrManager::GetAttributeXmlNameAndNameSpace(
    TUint32 aType,
    TPtrC8& aAttributeName,
    TPtrC8& aAttributeNameSpace ) const
    {
    const CPEngPresenceAttrConstructor* constructor =
        iTypeConstructors->FindByType( aType );
    TInt err = KErrUnknown;
    if ( constructor )
        {
        err = constructor->GetCurrentXmlNames( aAttributeName,
                                               aAttributeNameSpace );
        }

    if ( err != KErrNone )
        {
        aAttributeName.Set( NULL, 0 );
        aAttributeNameSpace.Set( NULL, 0 );
        }

    return err;
    }





// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::PackModelArrayL()
// -----------------------------------------------------------------------------
//
HBufC16* CPEngPresenceAttrManager::PackModelArrayL(
    const RPointerArray<MPEngPresenceAttrModel2>& aModels ) const
    {
    return PEngAttributeRequestCodec::PackModelArrayL( aModels );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::UnpackModelArrayL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrManager::UnpackModelArrayL(
    const TDesC16& aModelPkg,
    TInt aInstanceOptions,
    RPointerArray<MPEngPresenceAttrModel2>& aModels  )
    {
    PEngAttributeRequestCodec::UnpackModelArrayL( aModelPkg,
                                                  aInstanceOptions,
                                                  aModels,
                                                  *this );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::PackFetchRequestL()
// -----------------------------------------------------------------------------
//
HBufC16* CPEngPresenceAttrManager::PackFetchRequestL(
    const MDesCArray& aPresenceIDs,
    const TArray<TUint32>& aTypes ) const
    {
    return PEngAttributeRequestCodec::PackFetchRequestL( aPresenceIDs,
                                                         aTypes );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::UnpackFetchRequestL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrManager::UnpackFetchRequestL( const TDesC16& aFetchRequestPkg,
                                                    CDesCArray& aPresenceIDs,
                                                    RArray<TUint32>& aTypes ) const
    {
    PEngAttributeRequestCodec::UnpackFetchRequestL( aFetchRequestPkg,
                                                    aPresenceIDs,
                                                    aTypes );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::GenerateStoreIdL()
// -----------------------------------------------------------------------------
//
HBufC* CPEngPresenceAttrManager::GenerateStoreIdL( TUint32 aAttributeType,
                                                   const TDesC& aPresenceId ) const
    {
    return PEngAttributeStoreIDCodec::GenerateStoreIdL( aAttributeType, aPresenceId );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::ResolveStoreId()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrManager::ResolveStoreId( const TDesC& aStoreId,
                                               TUint32& aAttributeType,
                                               TPtrC& aPresenceId ) const

    {
    return PEngAttributeStoreIDCodec::ResolveStoreId( aStoreId,
                                                      aAttributeType,
                                                      aPresenceId );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::ForceStoreBatchL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrManager::ForceStoreBatchL(
    const RPointerArray<MPEngPresenceAttrModel2> aModels )
    {
    if ( aModels.Count() > 0 )
        {
        User::LeaveIfError( iStorageManager->BufferServerSideNotifications() );
        TRAPD( err,
            {
            const TInt count = aModels.Count();
            for ( TInt ii = count - 1 ; ii >= 0 ; --ii )
                {
                const CPEngPresenceAttrModelStorageProxy& storage =
                    aModels[ ii ]->Advanced()->Storage();
                storage.StoreToStorageL( ETrue );   //Force store
                }
            } );

        User::LeaveIfError( iStorageManager->ReleaseServerSideBuffering() );
        User::LeaveIfError( err );
        }
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrManager::ForceRemoveFromStoreBatchL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrManager::ForceRemoveFromStoreBatchL(
    RPointerArray<MPEngPresenceAttrModel2> aModels )
    {
    User::LeaveIfError( iStorageManager->BufferServerSideNotifications() );
    TRAPD( err,
        {
        const TInt count = aModels.Count();
        for ( TInt ii = count - 1 ; ii >= 0 ; --ii )
            {
            CPEngPresenceAttrModelStorageProxy& storage =
                aModels[ ii ]->Advanced()->Storage();
            storage.RemoveFromStorageL();
            }
        } );

    User::LeaveIfError( iStorageManager->ReleaseServerSideBuffering() );
    User::LeaveIfError( err );
    }


//  End of File


