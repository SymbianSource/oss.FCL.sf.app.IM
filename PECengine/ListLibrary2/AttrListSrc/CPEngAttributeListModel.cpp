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
* Description:  Attribute list model implementation
*
*/

// INCLUDE FILES
#include    "CPEngAttributeListModel.h"
#include    "MPEngPresenceAttrManager.h"
#include    "PEngListLibTools.h"
#include    <e32std.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngAttributeListModel::CPEngAttributeListModel()
// -----------------------------------------------------------------------------
//
CPEngAttributeListModel::CPEngAttributeListModel(
    MPEngPresenceAttrManager* aAttributeManager )
        : iAttributeManager( aAttributeManager )
    {
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListModel::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListModel::ConstructL(
    const RArray<TUint32>& aPresenceAttributes )
    {
    TInt count ( aPresenceAttributes.Count() );
    for ( TInt x ( 0 ) ; x < count ; x++ )
        {
        User::LeaveIfError( iListOfAttributes.InsertInSignedKeyOrder(
                                aPresenceAttributes[x] ) );
        }
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListModel::NewL()
// -----------------------------------------------------------------------------
//
CPEngAttributeListModel* CPEngAttributeListModel::NewL(
    MPEngPresenceAttrManager* aAttributeManager )
    {
    CPEngAttributeListModel* self = NewLC( aAttributeManager );
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListModel::NewL()
// -----------------------------------------------------------------------------
//
CPEngAttributeListModel* CPEngAttributeListModel::NewL(
    MPEngPresenceAttrManager* aAttributeManager,
    const RArray<TUint32>& aPresenceAttributes )
    {
    CPEngAttributeListModel* self = NewLC( aAttributeManager, aPresenceAttributes );
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListModel::NewLC
// -----------------------------------------------------------------------------
//
CPEngAttributeListModel* CPEngAttributeListModel::NewLC(
    MPEngPresenceAttrManager* aAttributeManager )
    {
    CPEngAttributeListModel* self =
        new( ELeave ) CPEngAttributeListModel ( aAttributeManager );

    CleanupStack::PushL( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListModel::NewLC()
// -----------------------------------------------------------------------------
//
CPEngAttributeListModel* CPEngAttributeListModel::NewLC(
    MPEngPresenceAttrManager* aAttributeManager,
    const RArray<TUint32>& aPresenceAttributes )
    {
    CPEngAttributeListModel* self = new( ELeave )
    CPEngAttributeListModel( aAttributeManager );

    CleanupStack::PushL( self );
    self->ConstructL( aPresenceAttributes );

    return self;
    }


//   Destructor
CPEngAttributeListModel::~CPEngAttributeListModel()
    {
    iListOfAttributes.Reset();
    }


// =============================================================================
// =============== From MPEngPresenceAttributeList =============================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngAttributeListModel::AddPresenceAttributeL()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListModel::AddPresenceAttributeL( TUint32 aPresenceAttribute )
    {
    // if answer is KErrNone attribute is known
    TInt err( iAttributeManager->ValidNetworkAttribute( aPresenceAttribute ) );
    User::LeaveIfError( err );

    // attribute is supported, add it to the array
    err = iListOfAttributes.InsertInSignedKeyOrder( aPresenceAttribute );
    NListLibTools::LeaveIfMajorErrorL( err );
    }

// -----------------------------------------------------------------------------
// CPEngAttributeListModel::RemovePresenceAttribute()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListModel::RemovePresenceAttribute( TUint32 aPresenceAttribute )
    {
    TInt index ( iListOfAttributes.FindInSignedKeyOrder( aPresenceAttribute ) );
    if ( index == KErrNotFound )
        {
        // Presence Attribute was not found, nothing to remove
        return KErrNotFound;
        }

    // remove ID at found index
    iListOfAttributes.Remove( index );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPEngAttributeListModel::RemoveAllAttributes()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListModel::RemoveAllAttributes()
    {
    iListOfAttributes.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListModel::ConfirmPresenceAttribute()
// -----------------------------------------------------------------------------
//
TInt CPEngAttributeListModel::ConfirmPresenceAttribute(
    TUint32 aPresenceAttribute ) const
    {
    return iAttributeManager->ValidNetworkAttribute( aPresenceAttribute );
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListModel::ListOfAttributes()
// -----------------------------------------------------------------------------
//
TArray<TUint32> CPEngAttributeListModel::ListOfAttributes() const
    {
    return iListOfAttributes.Array();
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListModel::Close()
// -----------------------------------------------------------------------------
//
void CPEngAttributeListModel::Close()
    {
    delete this;
    }


// -----------------------------------------------------------------------------
// CPEngAttributeListModel::PresenceAttributes()
// -----------------------------------------------------------------------------
//
const RArray<TUint32>& CPEngAttributeListModel::PresenceAttributes() const
    {
    return iListOfAttributes;
    }


//  End of File

