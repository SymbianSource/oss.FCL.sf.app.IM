/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Key for sorting CIMPSSAPSettingsListItem's.
*
*/

// INCLUDE FILES
#include "TIMPSSAPSettingsListItemNameKey.h"
#include "cimpssapsettingslistitem.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// IMPSSAPProtectionHandler::TIMPSSAPSettingsListItemNameKey()
// -----------------------------------------------------------------------------
//
TIMPSSAPSettingsListItemNameKey::TIMPSSAPSettingsListItemNameKey( TBool aAscending )
        : TKeyArrayFix( 0, ECmpCollated, sizeof( CIMPSSAPSettingsListItem* ) ),
        iAscending( aAscending )
    {
    }

// -----------------------------------------------------------------------------
// IMPSSAPProtectionHandler::Compare()
// -----------------------------------------------------------------------------
//
TInt TIMPSSAPSettingsListItemNameKey::Compare( TInt aLeft, TInt aRight ) const
    {
    //The list item array holds actually pointers to list items.
    //At() method returns TAny pointer, pointing to list item pointer in the array
    //==> TAny pointer needs to be casted to CIMPSSAPSettingsListItem pointer pointer.
    CIMPSSAPSettingsListItem* leftItem = * ( reinterpret_cast< CIMPSSAPSettingsListItem** > ( At( aLeft ) ) );
    CIMPSSAPSettingsListItem* rightItem = * ( reinterpret_cast< CIMPSSAPSettingsListItem** > ( At( aRight ) ) );

    TPtrC leftName( leftItem->Name() );
    TPtrC rightName( rightItem->Name() );

    TInt status = leftName.CompareC( rightName );

    if ( !iAscending )
        {
        status *= -1; //flip order, if ascending
        }

    return status;
    }


//  End of File
