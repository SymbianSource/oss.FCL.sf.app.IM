/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for PC Group Interface
*
*/



// INCLUDE FILES
#include "CCAGroupPropertiesPC.h"
#include "MCAStoredGroups.h"
#include "CCAStorageManagerFactory.h"
#include "CCAEngine.h"
#include "MCAStoredGroup.h"
#include "MCAStoredContacts.h"
#include "MCAGroupOperations.h"
#include "MCAGroupManagerInterface.h"
#include "MCAExtendedStoredGroup.h"

#include "CCAServerContactsArrayPC.h"
#include "CAPresenceManager.h"
#include "MCAPresence.h"
//const definitions



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: Constructor
// -----------------------------------------------------------------------------
//
CCAGroupPropertiesPC::CCAGroupPropertiesPC(
    MCAGroupManagerInterface& aGroupMgrInterface )
        : iGroupMgrInterface( aGroupMgrInterface )

    {
    }


// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: NewL
// -----------------------------------------------------------------------------
//
CCAGroupPropertiesPC* CCAGroupPropertiesPC::NewL(
    MCAGroupManagerInterface& aGroupMgrInterface )
    {
    CCAGroupPropertiesPC* self =
        new ( ELeave ) CCAGroupPropertiesPC( aGroupMgrInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: ConstructL
// -----------------------------------------------------------------------------
//
void CCAGroupPropertiesPC::ConstructL(  )
    {
    iChatGroups = CCAStorageManagerFactory::GroupListInterfaceL();
    iStoredContacts = CCAStorageManagerFactory::ContactListInterfaceL();
    }


// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: Destructor
// -----------------------------------------------------------------------------
//
CCAGroupPropertiesPC::~CCAGroupPropertiesPC()
    {
    }



// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: GetPropertiesL
// -----------------------------------------------------------------------------
//
CImpsCommonGroupProps* CCAGroupPropertiesPC::GetEdittedProperties()
    {
    return iGroupPropsForGrpPropDialog;
    }

// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: GetPropertiesL
// Note: CodeScanner is likely to return false positives for this situation,
// because some returned TInt values will not be error codes
// -----------------------------------------------------------------------------
//
TInt CCAGroupPropertiesPC::InitializeForEdittingL( const TDesC& aGroupId )
    {
    TInt err( KErrNone );
    MCAGroupOperations * groupOps = iGroupMgrInterface.GroupOperationsL( aGroupId );
    if ( groupOps )
        {
        err = groupOps->GetPropertiesL( iGroupPropsForGrpPropDialog, iPrivatePropsForGrpPropDialog );
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: IsPrivateAllowed
// -----------------------------------------------------------------------------
//
TEnumsPC::TImpsPropertyBoolPC CCAGroupPropertiesPC::IsPrivateAllowed()
    {
    return ConvertTImpsPropertyBoolToTEnumsPC(
               iGroupPropsForGrpPropDialog->IsPrivateAllowed() );
    }


// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: IsOpen
// -----------------------------------------------------------------------------
//
TEnumsPC::TImpsPropertyBoolPC CCAGroupPropertiesPC::IsOpen()
    {


    TEnumsPC::TImpsPropertyBoolPC impsProperty;
    switch ( iGroupPropsForGrpPropDialog->IsOpen() )
        {
        case EImpsPropUndef:
            {
            impsProperty = TEnumsPC::EImpsPropUndef;
            break;
            }
        case EImpsPropNo:
            {
            impsProperty = TEnumsPC::EImpsPropYes;
            break;
            }
        case EImpsPropYes:
            {
            impsProperty = TEnumsPC::EImpsPropNo;
            break;
            }

        default:
            {
            impsProperty = TEnumsPC::EImpsPropUndef;
            break;
            }
        }
    return impsProperty;

    }


// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC::ConvertTImpsPropertyBoolToTEnumsPC
// -----------------------------------------------------------------------------
//
TEnumsPC::TImpsPropertyBoolPC CCAGroupPropertiesPC::ConvertTImpsPropertyBoolToTEnumsPC
( TImpsPropertyBool aEventSource )
    {
    TEnumsPC::TImpsPropertyBoolPC impsProperty;
    switch ( aEventSource )
        {
        case EImpsPropUndef:
            {
            impsProperty = TEnumsPC::EImpsPropUndef;
            break;
            }
        case EImpsPropNo:
            {
            impsProperty = TEnumsPC::EImpsPropNo;
            break;
            }
        case EImpsPropYes:
            {
            impsProperty = TEnumsPC::EImpsPropYes;
            break;
            }

        default:
            {
            impsProperty = TEnumsPC::EImpsPropUndef;
            break;
            }
        }
    return impsProperty;
    }

// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: NumberOfCreatedGroupsDuringSession
// -----------------------------------------------------------------------------
//
TInt CCAGroupPropertiesPC::NumberOfCreatedGroupsDuringSession()
    {
    return iGroupMgrInterface.NumberOfCreatedGroupsDuringSession();
    }



// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: IsSearchable
// -----------------------------------------------------------------------------
//
TEnumsPC::TImpsPropertyBoolPC CCAGroupPropertiesPC::IsSearchable( )
    {
    return ConvertTImpsPropertyBoolToTEnumsPC( iGroupPropsForGrpPropDialog->IsSearchable() );
    }



// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: GetIdentification
// -----------------------------------------------------------------------------
//
const TPtrC CCAGroupPropertiesPC::GetIdentificationL( const TDesC& aWvId )
    {
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    if ( contacts )
        {
        return contacts->Identification( aWvId ) ;
        }
    else
        {
        return KNullDesC();
        }

    }


// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC::IsFavouriteChatGroup
// -----------------------------------------------------------------------------
//
TBool CCAGroupPropertiesPC::IsFavouriteChatGroup( const TDesC& aGroupId ) const
    {
    TBool isFavourite( EFalse );
    MCAStoredGroup* group = iChatGroups->FindGroup( aGroupId );
    if ( group )
        {
        isFavourite = ( group->StorageType() ==
                        TStorageManagerGlobals::EStoragePersistent );
        }
    return isFavourite;
    }

// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC::FindGroup
// -----------------------------------------------------------------------------
//
TBool CCAGroupPropertiesPC::FindGroup( const TDesC& aGroupId ) const
    {
    MCAStoredGroup* group = iChatGroups->FindGroup( aGroupId );
    if ( group )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC::GetGroupName
// -----------------------------------------------------------------------------
//
const TDesC& CCAGroupPropertiesPC::GetGroupName( const TDesC& aGroupId ) const
    {
    MCAStoredGroup* group = iChatGroups->FindGroup( aGroupId );
    if ( group )
        {
        return group->GroupName();
        }
    else
        {
        return KNullDesC();
        }
    }


// ---------------------------------------------------------
// CCAGroupPropertiesPC::IsJoined
// ---------------------------------------------------------
//
TBool CCAGroupPropertiesPC::IsJoined( const TDesC& aGroupId ) const
    {
    MCAStoredGroup* group = iChatGroups->FindGroup( aGroupId );

    if ( group )
        {
        return group->IsJoined();
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------
// CCAGroupPropertiesPC::IsOwnGroup
// ---------------------------------------------------------
//
TBool CCAGroupPropertiesPC::IsOwnGroup( const TDesC& aGroupId ) const
    {
    MCAStoredGroup* group = iChatGroups->FindGroup( aGroupId );

    if ( group )
        {
        return group->IsOwnGroup();
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------
// CCAGroupPropertiesPC::IsAdmin
// ---------------------------------------------------------
//
TBool CCAGroupPropertiesPC::IsAdmin( const TDesC& aGroupId ) const
    {
    MCAStoredGroup* group = iChatGroups->FindGroup( aGroupId );
    if ( group )
        {
        return group->IsAdmin();
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CCAGroupPropertiesPC::SetOpen
// ---------------------------------------------------------
//
void CCAGroupPropertiesPC::SetOpen( TEnumsPC::TImpsPropertyBoolPC aType )
    {

    TImpsPropertyBool impsProperty;
    switch ( aType )
        {
        case TEnumsPC::EImpsPropUndef:
            {
            impsProperty = EImpsPropUndef;
            break;
            }
        case TEnumsPC::EImpsPropNo:
            {
            impsProperty = EImpsPropYes;
            break;
            }
        case TEnumsPC::EImpsPropYes:
            {
            impsProperty = EImpsPropNo;
            break;
            }

        default:
            {
            impsProperty = EImpsPropUndef;
            break;
            }
        }

    iGroupPropsForGrpPropDialog->SetOpen( impsProperty );
    }

// End of File

