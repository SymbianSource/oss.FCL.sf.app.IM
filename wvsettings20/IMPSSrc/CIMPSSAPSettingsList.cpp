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
* Description:  "Light weight" SAP list.
*
*/

// INCLUDE FILES
#include    <e32std.h>
#include    "cimpssapsettingslist.h"
#include    "cimpssapsettingslistitem.h"
#include    "TIMPSSAPSettingsListItemNameKey.h"

// CONSTANTS
const TInt KSettingsListGranurality = 5; ///< Granularity of the list. Usually there are just quite few servers.


// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettingsList* CIMPSSAPSettingsList::NewL()
    {
    return new ( ELeave ) CIMPSSAPSettingsList();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettingsList* CIMPSSAPSettingsList::NewLC()
    {
    CIMPSSAPSettingsList* self = new ( ELeave ) CIMPSSAPSettingsList();
    CleanupStack::PushL( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::~CIMPSSAPSettingsList()
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettingsList::~CIMPSSAPSettingsList()
    {
    ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::CIMPSSAPSettingsList()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsList::CIMPSSAPSettingsList()
        : CArrayPtrSeg< CIMPSSAPSettingsListItem >( KSettingsListGranurality )
    {
    }




// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::IndexForUid()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettingsList::IndexForUid( TUint32 aUid ) const
    {
    TInt count( Count() );
    TInt index = KErrNotFound;

    for ( TInt ii( 0 ); ii < count; ii++ )
        {
        if ( UidForIndex( ii ) == aUid )
            {
            index = ii;
            break;
            }
        }

    return index;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::UidForIndex()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIMPSSAPSettingsList::UidForIndex( TInt aIndex ) const
    {
    return At( aIndex )->Uid();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::ListItemForUid()
// -----------------------------------------------------------------------------
//
EXPORT_C const CIMPSSAPSettingsListItem* CIMPSSAPSettingsList::ListItemForUid( TUint32 aUid ) const
    {
    TInt index = IndexForUid( aUid );
    CIMPSSAPSettingsListItem* ptr = NULL;

    if ( index != KErrNotFound )
        {
        ptr = At( index );
        }

    return ptr;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::FindNameL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettingsList::FindNameL( const TDesC& aName, TInt& aIndex ) const
    {
    CIMPSSAPSettingsListItem* item = CIMPSSAPSettingsListItem::NewLC( aName, 0, EIMPSIMAccessGroup );
    TIMPSSAPSettingsListItemNameKey key;
    TInt ret( FindIsq( item, key, aIndex ) );
    CleanupStack::PopAndDestroy(); //item
    return ret;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::Delete()
// Overides default delete from CArrayPtrSeg.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettingsList::Delete( TInt aIndex )
    {
    delete At( aIndex );
    CArrayPtrSeg< CIMPSSAPSettingsListItem >::Delete( aIndex );
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::Delete()
// Overides default delete from CArrayPtrSeg.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIMPSSAPSettingsList::Delete( TInt aIndex, TInt aCount )
    {
    for ( TInt ii( 0 ); ii < aCount; ii++ )
        {
        delete At( aIndex + ii );
        CArrayPtrSeg< CIMPSSAPSettingsListItem >::Delete( aIndex + ii );
        }
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::MdcaCount()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIMPSSAPSettingsList::MdcaCount() const
    {
    return Count();
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsList::MdcaPoint()
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CIMPSSAPSettingsList::MdcaPoint( TInt aIndex ) const
    {
    return TPtrC( At( aIndex )->Name() );
    }


//  End of File

