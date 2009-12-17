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
* Description:  "Light weight" list item for settings list.
*
*/

// INCLUDE FILES
#include    "e32std.h"
#include    "cimpssapsettingslistitem.h"
#include    "IMPSSAPProtectionHandler.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsListItem::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettingsListItem* CIMPSSAPSettingsListItem::NewLC( const TDesC& aSAPName,
                                                                    TUint32 aUid,
                                                                    TIMPSAccessGroup aGroup )
    {
    CIMPSSAPSettingsListItem* self = new ( ELeave ) CIMPSSAPSettingsListItem;
    CleanupStack::PushL( self );
    self->iSAPName = aSAPName.AllocL();
    self->iUid = aUid;
    self->iGroup = aGroup;
    return self;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsListItem::NewLC()
// Two-phased constructor, obsolete
// -----------------------------------------------------------------------------
//

EXPORT_C CIMPSSAPSettingsListItem* CIMPSSAPSettingsListItem::NewLC( const TDesC& aSAPName,
                                                                    TUint32 aUid )
    {
    CIMPSSAPSettingsListItem* self = new ( ELeave ) CIMPSSAPSettingsListItem;
    CleanupStack::PushL( self );
    self->iSAPName = aSAPName.AllocL();
    self->iUid = aUid;
    self->iGroup = EIMPSNoAccessGroup;
    return self;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsListItem::NewLC()
// Two-phased constructor.
//
// Overloaded for setting protection, obsolete
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettingsListItem* CIMPSSAPSettingsListItem::NewLC( const TDesC& aSAPName,
                                                                    TUint32 aUid,
                                                                    TIMPSSAPProtection aProtection )
    {
    IMPSSAPProtectionHandler::ChechkIsProperProtectionLevelL( aProtection );

    CIMPSSAPSettingsListItem* self = NewLC( aSAPName, aUid, EIMPSNoAccessGroup );
    self->iProtection = aProtection;
    return self;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsListItem::NewLC()
// Two-phased constructor.
//
// Overloaded for setting protection
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettingsListItem* CIMPSSAPSettingsListItem::NewLC( const TDesC& aSAPName,
                                                                    TUint32 aUid,
                                                                    TIMPSAccessGroup aGroup,
                                                                    TIMPSSAPProtection aProtection )
    {
    IMPSSAPProtectionHandler::ChechkIsProperProtectionLevelL( aProtection );

    CIMPSSAPSettingsListItem* self = new ( ELeave ) CIMPSSAPSettingsListItem;
    CleanupStack::PushL( self );
    self->iSAPName = aSAPName.AllocL();
    self->iUid = aUid;
    self->iGroup = aGroup;
    self->iProtection = aProtection;
    return self;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsListItem::~CIMPSSAPSettingsListItem()
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSSAPSettingsListItem::~CIMPSSAPSettingsListItem()
    {
    delete iSAPName;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsListItem::CIMPSSAPSettingsListItem()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSAPSettingsListItem::CIMPSSAPSettingsListItem()
        : iUid( 0 ),
        iProtection( ESAPNoProtection )
    {
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsListItem::Name()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIMPSSAPSettingsListItem::Name() const
    {
    return *iSAPName;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsListItem::Uid()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIMPSSAPSettingsListItem::Uid() const
    {
    return iUid;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPSettingsListItem::Protection()
// -----------------------------------------------------------------------------
//
EXPORT_C TIMPSSAPProtection CIMPSSAPSettingsListItem::Protection() const
    {
    return iProtection;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPSettingsListItem::AccessGroup()
// -----------------------------------------------------------------------------
//
EXPORT_C TIMPSAccessGroup CIMPSSAPSettingsListItem::AccessGroup() const
    {
    return iGroup;
    }

//  End of File

