/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contact list model for UI
*
*/



// INCLUDE FILES
#include    "CCAContactListModel.h"
#include    "impsbuilddefinitions.h"

// storage includes
#include    "CCAStorageManagerFactory.h"
#include    "MCAStoredContacts.h"
#include    "MCAStoredContact.h"
#include    "MCAContactList.h"

// engine includes
#include    "MCASettings.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAContactListModel::CCAContactListModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAContactListModel::CCAContactListModel( MCASettings& aSettingsInterface )
        : iSettings( aSettingsInterface )
    {
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAContactListModel::ConstructL()
    {
    iContacts = CCAStorageManagerFactory::ContactListInterfaceL();
    iSettings.AddObserverL( this );
    SetSort();
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAContactListModel* CCAContactListModel::NewL(
    MCASettings& aSettingsInterface )
    {
    CCAContactListModel* self =
        new( ELeave ) CCAContactListModel( aSettingsInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CCAContactListModel::~CCAContactListModel()
    {
    iSettings.RemoveObserver( this );
    }


// -----------------------------------------------------------------------------
// CCAContactListModel::Count
// Returns item count
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAContactListModel::Count() const
    {
    // fetch contact list count from storage
    // and add contact counts from expanded contact lists
    TInt listCount( iContacts->ListCount() );
    TInt itemCount( 0 );
    for ( TInt i( 0 ); i < listCount; ++i )
        {
        MCAContactList& list = iContacts->ListAt( i );

        TInt contactCount( list.FilteredCount( iFilter ) );

        if ( !iSelectionMode )
            {
            // Empty contact lists are not shown in selection-mode
            ++itemCount;
            }

        if ( contactCount > 0 ||
             iFilter & TStorageManagerGlobals::EFilterAll )
            {
            // if we're showing all contacts or
            // there are online contacts in this list,
            // the list-item is shown => increase count
            if ( iSelectionMode )
                {
                ++itemCount;
                }

            if ( !list.Collapsed() || iExpandAll )
                {
                // if list is expanded, add count of count of contacts
                // in this list to item count
                itemCount += contactCount;

                // if list is expanded and the "all" item should
                // be shown, increase counter
                if ( iShowAll )
                    {
                    ++itemCount;
                    }
                }
            }
        }

    //own data item should be also considered when not in selection-mode:
    if ( !iSelectionMode && iContacts->OwnStatus().Identification().Length() )
        {
        ++itemCount;
        }

    return itemCount;
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::Item
// Returns item at given index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAContactListModel::SItem CCAContactListModel::Item( TInt aIndex  ) const
    {
    MCAContactListModel::SItem item;
    item.iType = MCAContactListModel::EInvalid;
    item.iContactList = NULL;
    item.iContact = NULL;

    MCAStoredContact& ownStatus = iContacts->OwnStatus();
    TBool validOwnStatus( ownStatus.Identification().Length() > 0 );

    //own data item should be also considered when not in selection-mode:
    if ( !iSelectionMode && validOwnStatus )
        {
        if ( aIndex == 0 )
            {
            item.iType = MCAContactListModel::EOwnStatus;
            item.iContact = &ownStatus;
            return item;
            }
        aIndex--;
        }

    TInt count( 0 );
    TInt listCount( iContacts->ListCount() );
    for ( TInt i( 0 ); i < listCount; ++i )
        {
        MCAContactList& list = iContacts->ListAt( i );
        item.iContactList = &list;

        if ( !iSelectionMode )
            {
            // Empty contact lists are not shown in selection-mode
            ++count;
            if ( count > aIndex )
                {
                // there's a list in given index
                item.iType = MCAContactListModel::EContactList;
                return item;
                }
            }

        TInt contactCount( list.FilteredCount( iFilter ) );

        if ( contactCount > 0
             || iFilter & TStorageManagerGlobals::EFilterAll )
            {
            // if we're showing all contacts or
            // there are online contacts in this list,
            // the list-item is shown => increase count
            if ( iSelectionMode )
                {
                ++count;
                if ( count > aIndex )
                    {
                    // there's a list in given index
                    item.iType = MCAContactListModel::EContactList;
                    return item;
                    }
                }

            if ( !list.Collapsed() || iExpandAll )
                {
                if ( iShowAll )
                    {
                    // put "all" to beginning of list,
                    // if wanted.
                    ++count;
                    if ( count > aIndex )
                        {
                        item.iType = MCAContactListModel::EAll;
                        return item;
                        }
                    }

                // list is expanded, consider also contacts in this list
                if ( count + contactCount > aIndex )
                    {
                    // there's a contact in given index
                    TInt contactIndex( aIndex - count );
                    item.iType = MCAContactListModel::EContactItem;
                    item.iContact =
                        &list.FilteredContact( contactIndex, iFilter );
                    return item;
                    }
                // add expanded list's contact count
                count += contactCount;
                }
            }
        }
    // not found
    return item;
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::SetSelectionList
// Sets the visibility of "all" item
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactListModel::ShowAll( TBool aShow )
    {
    iShowAll = aShow;
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::HandleSettingsChangeL
// Handles setting value change
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactListModel::HandleSettingsChangeL( TInt aChangedSettingEnum )
    {
    if ( aChangedSettingEnum == MCASettings::EFriendsListOrdering )
        {
        SetSort();
        }
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::MdcaCount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAContactListModel::MdcaCount() const
    {
    return Count();
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::MdcaPoint
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CCAContactListModel::MdcaPoint( TInt aIndex ) const
    {
    MCAContactListModel::SItem item = Item( aIndex );
    switch ( item.iType )
        {
        case MCAContactListModel::EContactList:
            {
            return item.iContactList->DisplayName();
            }

        case MCAContactListModel::EOwnStatus:
        case MCAContactListModel::EContactItem:   // fall through
            {
            return item.iContact->Identification();
            }

        case MCAContactListModel::EAll:
            {
            // These will be filtered out
            return KNullDesC();
            }

        default:
            {
            // Should not be here.
            break;
            }
        }

    // We really should never reach this.
    return KNullDesC();
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::SetSort
// Sets the correct sorting method to storage
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactListModel::SetSort()
    {
    TInt value( iSettings.Value( MCASettings::EFriendsListOrdering ) );

    if ( value == MCASettings::EPresence )
        {
        iContacts->SetSortAlgorithm( MCAStoredContact::ECompareByPresence );
        iContacts->Sort();
        }
    else
        {
        iContacts->SetSortAlgorithm( MCAStoredContact::ECompareAlphabetically );
        iContacts->Sort();
        }
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::SetSelectionMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactListModel::SetSelectionMode( TBool aSelectionMode )
    {
    iSelectionMode = aSelectionMode;
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::SetPrimaryContactList
// -----------------------------------------------------------------------------
//
void CCAContactListModel::SetPrimaryContactLists( TBool aPrimaryInUse )
    {
    if ( aPrimaryInUse )
        {
        iContacts->SetContactListProperty( MCAStoredContacts::EPrimaryCollapseInUse );
        }
    else
        {
        iContacts->SetContactListProperty( MCAStoredContacts::ESecondaryCollapseInUse );
        iContacts->ResetContactLists();
        }
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::ResetDefaultFilter
// -----------------------------------------------------------------------------
//
void CCAContactListModel::ResetDefaultFilter()
    {
    SetFilter( iDefaultFilter );
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::SetDefaultFilter
// -----------------------------------------------------------------------------
//
void CCAContactListModel::SetDefaultFilter(
    TStorageManagerGlobals::TFilterType aFilter )
    {
    iDefaultFilter = aFilter;
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::SetFilter
// -----------------------------------------------------------------------------
//
void CCAContactListModel::SetFilter(
    TStorageManagerGlobals::TFilterType aFilter )
    {
    iFilter = aFilter;
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::Filter
// -----------------------------------------------------------------------------
//
TStorageManagerGlobals::TFilterType CCAContactListModel::Filter() const
    {
    return iFilter;
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::ForceExpanded
// -----------------------------------------------------------------------------
//
void CCAContactListModel::ForceExpanded( TBool aAllExpanded )
    {
    iExpandAll = aAllExpanded;
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::IsForceExpanded
// -----------------------------------------------------------------------------
//
TBool CCAContactListModel::IsForceExpanded() const
    {
    return iExpandAll;
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::MapContactListPositionsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAContactListModel::MapContactListPositions(
    RArray<TInt>& aPositionArray )
    {
    aPositionArray.Reset();
    TInt listcount = iContacts->ListCount();
    if ( listcount == 0 )
        {
        // no lists
        return;
        }

    // Add own status
    TBool validOwnStatus =
        iContacts->OwnStatus().Identification().Length() > 0;
    TInt itemCount = ( validOwnStatus && !iSelectionMode ) ? 1 : 0;

    // first list
    aPositionArray.Append( itemCount );

    // Go through lists
    for ( TInt i = 0; i < listcount - 1; ++i )
        {
        MCAContactList& list = iContacts->ListAt( i );
        TInt contactCount( list.FilteredCount( iFilter ) );

        // Add visible contacts between this list(i) and next list(i+1)
        if ( contactCount > 0 || iFilter & TStorageManagerGlobals::EFilterAll )
            {

            if ( !list.Collapsed() || iExpandAll )
                {
                // if list is expanded, add count of count of contacts
                // in this list to item count
                itemCount += contactCount;

                // if list is expanded and the "all" item should
                // be shown, increase counter
                if ( iShowAll )
                    {
                    ++itemCount;
                    }
                }

            }

        // Append next list's position to array
        aPositionArray.Append( ++itemCount );
        }
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::IndexOfContact
// Gets index of contact in list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAContactListModel::IndexOfContact( MCAStoredContact* aContact ) const
    {
    TInt listCount( iContacts->ListCount() );
    MCAStoredContact& ownStatus = iContacts->OwnStatus();

    // take own status in to account.
    TInt itemCount( ( ownStatus.Identification().Length() > 0 ) ? 1 : 0 );

    if ( aContact == ( &ownStatus ) && itemCount == 1 )
        {
        return 0;
        }

    // in selection mode empty contact lists are not displayed
    TInt emptyList( iSelectionMode ? 0 : -1 );

    for ( TInt i( 0 ); i < listCount; ++i )
        {
        MCAContactList& list = iContacts->ListAt( i );
        TInt contactCount( list.FilteredCount( iFilter ) );

        TInt indexOfContact = list.FindIndexOfContact( aContact, iFilter );

        if ( contactCount > emptyList
             || iFilter & TStorageManagerGlobals::EFilterAll )
            {
            // if we're showing all contacts or
            // there are online contacts in this list,
            // the list-item is shown => increase count
            ++itemCount;

            if ( indexOfContact == KErrNotFound )
                {
                if ( !list.Collapsed() || iExpandAll )
                    {
                    // if list is expanded, add count of count of contacts
                    // in this list to item count
                    itemCount += contactCount;

                    // if list is expanded and the "all" item should
                    // be shown, increase counter
                    if ( iShowAll )
                        {
                        ++itemCount;
                        }
                    }
                }
            else // We found the contact
                {
                if ( list.Collapsed() && !iExpandAll )
                    {
                    // If the list is collapsed -> focus the list instead
                    return itemCount - 1;
                    }
                // add earlier items to index.
                return indexOfContact + itemCount;
                }
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CCAContactListModel::IndexOfList
// Gets index of contact in list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAContactListModel::IndexOfList( MCAContactList* aList,
                                       TBool aIgnoreOwnItem /*= EFalse*/,
                                       TBool aIgnoreEmptyLists /*= ETrue*/ ) const
    {
    TInt listCount( iContacts->ListCount() );
    // take own status in to account.
    TInt itemCount( ( iContacts->OwnStatus().Identification().Length() > 0
                      && !aIgnoreOwnItem ) ? 1 : 0 );

    // in selection mode empty contact lists are not displayed
    TInt emptyList( iSelectionMode ? 0 : -1 );

    for ( TInt i( 0 ); i < listCount; ++i )
        {
        MCAContactList& list = iContacts->ListAt( i );
        if ( &list == aList )
            {
            return itemCount;
            }

        TInt contactCount( list.FilteredCount( iFilter ) );

        if ( contactCount > emptyList ||
             iFilter & TStorageManagerGlobals::EFilterAll ||
             ( contactCount == 0 && !aIgnoreEmptyLists ) ) // show also empty lists.
            {
            // if we're showing all contacts or
            // there are online contacts in this list,
            // the list-item is shown => increase count
            ++itemCount;

            if ( !list.Collapsed() || iExpandAll )
                {
                // if list is expanded, add count of count of contacts
                // in this list to item count
                itemCount += contactCount;

                // if list is expanded and the "all" item should
                // be shown, increase counter
                if ( iShowAll )
                    {
                    ++itemCount;
                    }
                }
            }
        }
    return KErrNotFound;
    }


//  End of File
