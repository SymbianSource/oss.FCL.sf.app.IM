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
* Description:  List library singletons factory
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "CPEngListLibFactory.h"
#include    "CPEngAttributeListManager.h"
#include    "CPEngContactListManager.h"
#include    "CPEngAttributeListTransactionManager.h"
#include    "CPEngContactListTransactionManager.h"
#include    "CPEngAuthorizationManager.h"
#include    "CPEngAuthorizationTransFactory.h"

#include    "CPEngSessionSlotId.h"


//Default granurality for manager arrays
const TInt KDefaultGranurality = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngListLibFactory::CPEngListLibFactory()
// -----------------------------------------------------------------------------
//
CPEngListLibFactory::CPEngListLibFactory()
        : iAccessCount( 1 ), // init access count
        iAttrStore( KDefaultGranurality ),
        iCntStore( KDefaultGranurality ),
        iAttrTransManStore( KDefaultGranurality ),
        iCntTransManStore( KDefaultGranurality )
    {
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngListLibFactory::ConstructL()
    {
    iCSPVersion = PEngWVCspVersion::SelectVersionL();
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::NewLC()
// -----------------------------------------------------------------------------
//
CPEngListLibFactory* CPEngListLibFactory::InstanceLC()
    {
    CPEngListLibFactory* listFactory = static_cast<CPEngListLibFactory*> ( Dll::Tls() );
    if ( !listFactory )
        {
        // create notify factory
        listFactory = new( ELeave ) CPEngListLibFactory();
        CleanupClosePushL( *listFactory );
        listFactory->ConstructL();
        User::LeaveIfError( Dll::SetTls( listFactory ) );
        }
    else
        {
        listFactory->Open();                    // CSI: 65 #
        CleanupClosePushL( *listFactory );
        }

    return listFactory;
    }


// Destructor
CPEngListLibFactory::~CPEngListLibFactory()
    {
    iAttrStore.ResetAndDestroy();
    iCntStore.ResetAndDestroy();
    iAttrTransManStore.ResetAndDestroy();
    iCntTransManStore.ResetAndDestroy();
    iAuthManStore.ResetAndDestroy();
    iAuthTransManStore.ResetAndDestroy();

    Dll::SetTls( NULL );

#if _BullseyeCoverage
    cov_write();
#endif
    }


// =============================================================================
// =============== From MPEngStorageFactory ====================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngListLibFactory::RemoveAttrListManager()
// -----------------------------------------------------------------------------
//
void CPEngListLibFactory::RemoveAttrListManager(
    CPEngAttributeListManager* aAttrManager )
    {
    RemoveArrayElement( iAttrStore, aAttrManager );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::RemoveAttrListTransManager()
// -----------------------------------------------------------------------------
//
void CPEngListLibFactory::RemoveAttrListTransManager(
    CPEngAttributeListTransactionManager* aAttrTransManager )
    {
    RemoveArrayElement( iAttrTransManStore, aAttrTransManager );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::RemoveCntListManager()
// -----------------------------------------------------------------------------
//
void CPEngListLibFactory::RemoveCntListManager(
    CPEngContactListManager* aCntManager )
    {
    RemoveArrayElement( iCntStore, aCntManager );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::RemoveCntListTransManager()
// -----------------------------------------------------------------------------
//
void CPEngListLibFactory::RemoveCntListTransManager(
    CPEngContactListTransactionManager* aCntTransManager )
    {
    RemoveArrayElement( iCntTransManStore, aCntTransManager );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::RemoveAuthorizationManager()
// -----------------------------------------------------------------------------
//
void CPEngListLibFactory::RemoveAuthorizationManager(
    CPEngAuthorizationManager * aAuthManager )
    {
    RemoveArrayElement( iAuthManStore, aAuthManager );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::RemoveAuthorizationTransManager()
// -----------------------------------------------------------------------------
//
void CPEngListLibFactory::RemoveAuthorizationTransManager(
    CPEngAuthorizationTransFactory *aAuthTransManager )
    {
    RemoveArrayElement( iAuthTransManStore, aAuthTransManager );
    }


// =============================================================================
// =============== From main class =============================================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngListLibFactory::Open()
// -----------------------------------------------------------------------------
//
void CPEngListLibFactory::Open()
    {
    iAccessCount++;
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::Close()
// -----------------------------------------------------------------------------
//
void CPEngListLibFactory::Close()
    {
    iAccessCount--;
    CheckRefAccessD();
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::AttrListManagerL()
// -----------------------------------------------------------------------------
//
CPEngAttributeListManager* CPEngListLibFactory::AttrListManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    return ManagerTwoParamL( iAttrStore, aSessionSlot );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::AttrListTransManagerL()
// -----------------------------------------------------------------------------
//
CPEngAttributeListTransactionManager* CPEngListLibFactory::AttrListTransManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    return ManagerThreeParamL( iAttrTransManStore, aSessionSlot );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::CntListManagerL()
// -----------------------------------------------------------------------------
//
CPEngContactListManager* CPEngListLibFactory::CntListManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    return ManagerTwoParamL( iCntStore,  aSessionSlot );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::CntListTransManagerL()
// -----------------------------------------------------------------------------
//
CPEngContactListTransactionManager* CPEngListLibFactory::CntListTransManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    return ManagerThreeParamL( iCntTransManStore,  aSessionSlot );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::AuthorizationManagerL()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationManager* CPEngListLibFactory::AuthorizationManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    return ManagerTwoParamL( iAuthManStore,  aSessionSlot );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::AuthorizationTransManagerL()
// -----------------------------------------------------------------------------
//
CPEngAuthorizationTransFactory* CPEngListLibFactory::AuthorizationTransManagerL(
    const CPEngSessionSlotId& aSessionSlot )
    {
    return ManagerThreeParamL( iAuthTransManStore,  aSessionSlot );
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::FindArrayElement()
// -----------------------------------------------------------------------------
//
template< class T>
TInt CPEngListLibFactory::FindArrayElement( const RPointerArray<T>& aArray,
                                            const CPEngSessionSlotId& aSessionSlot )
    {
    TInt count ( aArray.Count() );
    for ( TInt x( 0 ) ; x < count ; x++ )
        {
        if ( KErrNone == aArray[x]->SessionId().Match( aSessionSlot ) )
            {
            return x;
            }
        }

    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::RemoveArrayElement()
// -----------------------------------------------------------------------------
//
template <class T>
void CPEngListLibFactory::RemoveArrayElement( RPointerArray<T>& aArray,
                                              T* aElement )
    {
    TInt index( aArray.Find( aElement ) );
    if ( index != KErrNotFound )
        {
        aArray.Remove( index );
        }

    CheckRefAccessD();
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::ManagerTwoParamL()
// -----------------------------------------------------------------------------
//
template <class T>
T* CPEngListLibFactory::ManagerTwoParamL( RPointerArray<T>& aArray,
                                          const CPEngSessionSlotId& aSessionSlot )
    {
    TInt index( FindArrayElement( aArray, aSessionSlot ) );
    if ( index != KErrNotFound )
        {
        aArray[ index ]->Open();
        return aArray[ index ];
        }

    // manager does not exist yet, create and add to the array
    T* newManager = T::NewLC( *this, aSessionSlot );

    aArray.AppendL( newManager );
    CleanupStack::Pop(); // newManager
    return newManager;
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::ManagerThreeParamL()
// -----------------------------------------------------------------------------
//
template <class T>
T* CPEngListLibFactory::ManagerThreeParamL( RPointerArray<T>& aArray,
                                            const CPEngSessionSlotId& aSessionSlot )
    {
    TInt index( FindArrayElement( aArray, aSessionSlot ) );
    if ( index != KErrNotFound )
        {
        aArray[ index ]->Open();
        return aArray[ index ];
        }

    // manager does not exist yet, create and add to the array
    T* newManager =
        T::NewLC( *this, aSessionSlot, iCSPVersion );

    aArray.AppendL( newManager );
    CleanupStack::Pop(); // newManager
    return newManager;
    }


// -----------------------------------------------------------------------------
// CPEngListLibFactory::CheckRefAccessD()
// -----------------------------------------------------------------------------
//
void CPEngListLibFactory::CheckRefAccessD()
    {
    if ( iAccessCount
         ||
         iAttrStore.Count()
         ||
         iCntStore.Count()
         ||
         iAttrTransManStore.Count()
         ||
         iCntTransManStore.Count()
         ||
         iAuthManStore.Count()
         ||
         iAuthTransManStore.Count() )
        {
        return;
        }

    delete this;
    }


//  End of File
