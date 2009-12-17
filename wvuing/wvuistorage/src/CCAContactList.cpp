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
* Description:  Implementation of contact data container
*
*/



// INCLUDE FILES
#include "CCAContactList.h"
#include "CAUtils.h"
#include "CCAContact.h"
#include "CCAContactSorter.h"
#include "MCAStorageInfo.h"
#include "MCAStoredContactsObserver.h"
#include "TCAStoragePanics.h"

#include "ChatDebugPrint.h"
#include "ChatDebugAssert.h"

// ============================ MEMBER FUNCTIONS ===============================

// Compares two contacts
TInt CompareUserIdAlphabetically(
    const MCAStoredContact& aFirst,
    const MCAStoredContact& aSecond )
    {
    return CAUtils::NeutralCompare( aFirst.UserId(), aSecond.UserId() );
    }

// -----------------------------------------------------------------------------
// CCAContactList::CCAContact
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAContactList::CCAContactList( MCAStoredContactsObserver& aObserver,
                                CCAContactSorter& aSorter,
                                MCAStorageInfo& aInfo )
        : iSorter( aSorter ),
        iInfo( aInfo ),
        iPrimaryCollapsed( ETrue ),
        iSecondaryCollapsed( ETrue ),
        iCurrentCollapsed( &iPrimaryCollapsed ),
        iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CCAContactList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAContactList::ConstructL( const TDesC& aContactListId,
                                 const TDesC& aDisplayName )
    {
    iListId = aContactListId.AllocL();
    iDisplayName = aDisplayName.AllocL();
    }

// -----------------------------------------------------------------------------
// CCAContactList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAContactList* CCAContactList::NewL( MCAStoredContactsObserver& aObserver,
                                      CCAContactSorter& aSorter,
                                      MCAStorageInfo& aInfo,
                                      const TDesC& aContactListId,
                                      const TDesC& aDisplayName )
    {
    CCAContactList* self = NewLC( aObserver,
                                  aSorter,
                                  aInfo,
                                  aContactListId,
                                  aDisplayName );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAContactList::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAContactList* CCAContactList::NewLC( MCAStoredContactsObserver& aObserver,
                                       CCAContactSorter& aSorter,
                                       MCAStorageInfo& aInfo,
                                       const TDesC& aContactListId,
                                       const TDesC& aDisplayName )
    {
    CCAContactList* self = new( ELeave ) CCAContactList( aObserver, aSorter, aInfo );

    CleanupStack::PushL( self );
    self->ConstructL( aContactListId, aDisplayName );
    return self;
    }


// Destructor
CCAContactList::~CCAContactList()
    {
    iContacts.ResetAndDestroy();
    iOrderedContacts.Reset();
    delete iListId;
    delete iDisplayName;
    }


// -----------------------------------------------------------------------------
// CCAContactList::ListId
// -----------------------------------------------------------------------------
//
const TDesC& CCAContactList::ListId() const
    {
    if ( iListId )
        {
        return *iListId;
        }
    return KNullDesC;
    }


// -----------------------------------------------------------------------------
// CCAContactList::SetListIdL
// -----------------------------------------------------------------------------
//
void CCAContactList::SetListIdL( const TDesC& aListId )
    {
    HBufC* newId = aListId.AllocL();
    delete iListId;
    iListId = newId;
    }

// -----------------------------------------------------------------------------
// CCAContactList::DisplayName
// -----------------------------------------------------------------------------
//
TPtrC CCAContactList::DisplayName() const
    {
    if ( iDisplayName )
        {
        if ( iDisplayName->Length() )
            {
            return *iDisplayName;
            }
        }
    return CAUtils::DisplayId( *iListId, ETrue );
    }

// -----------------------------------------------------------------------------
// CCAContactList::SetDisplayName
// -----------------------------------------------------------------------------
//
void CCAContactList::SetDisplayNameL( const TDesC& aDisplayName )
    {
    // Check capitalizing is enabled or not
    // extra  checking added here to keep the new Functionality(capitalizing)
    // added and the original code in seperate parts.
    // UI CR ID: 101-39727
    HBufC* tempName = NULL;
    if ( CAUtils::CapitalizingEnabled() )
        {
        if ( aDisplayName.Length() )
            {
            tempName = CAUtils::CapitalizeListNameL( aDisplayName );// Newly added capitalization Functionality
            }
        else
            {
            // when server doesnot send any display name we have to show the listid
            // the list id to be shown  is capitalized.(list id is used as it recieve
            // from the server.)
            tempName = CAUtils::CapitalizeListIdL( *iListId );
            }
        }

    else
        {
        tempName =  aDisplayName.AllocL(); // Original Functionality
        }
    delete iDisplayName;
    iDisplayName = tempName;
    iObserver.HandleChange( this,
                            NULL,
                            TStorageManagerGlobals::EStorageEventListChanged,
                            EFalse );
    }

// -----------------------------------------------------------------------------
// CCAContactList::FindContact
// -----------------------------------------------------------------------------
//
MCAStoredContact* CCAContactList::FindContact( const TDesC& aContactId )
    {
    TInt orderedIndex( 0 );
    TInt contactIndex = FindContactIndex( aContactId, orderedIndex );
    return ( contactIndex >= 0 ? iContacts[ contactIndex ] : NULL );
    }

//
MCAStoredContact* CCAContactList::FindContact( const MCAStoredContact* aContact )
    {
    TInt indexOfContact( FindIndexOfContact( aContact ) );
    if ( indexOfContact == KErrNotFound )
        {
        return NULL;
        }
    return iContacts[ indexOfContact ];
    }

// -----------------------------------------------------------------------------
// CCAContactList::FindContactByNick
// -----------------------------------------------------------------------------
//
MCAStoredContact* CCAContactList::FindContactByNick( const TDesC& aNick )
    {
    TInt contactIndex = FindContactIndexByNick( aNick );
    return ( contactIndex >= 0 ? iContacts[ contactIndex ] : NULL );
    }

// -----------------------------------------------------------------------------
// CCAContactList::RemoveContact
// -----------------------------------------------------------------------------
//
TInt CCAContactList::RemoveContact( const TDesC& aContactId )
    {
    TInt orderedIndex( 0 );
    TInt contactIndex = FindContactIndex( aContactId, orderedIndex );
    if ( KErrNotFound != contactIndex )
        {
        delete iContacts[ contactIndex ];
        iContacts.Remove( contactIndex );
        iOrderedContacts.Remove( orderedIndex );
        return KErrNone;
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CCAContactList::FindContact
// -----------------------------------------------------------------------------
//
MCAStoredContact* CCAContactList::AddContactL( MCAStoredContact* aContact )
    {
    // optimize insert for inserting of reverse alphabetical order
    TInt idIndex( 0 );
    TInt count( iOrderedContacts.Count() );
    if ( count &&
         0 <= CAUtils::NeutralCompare( aContact->UserId(),
                                       iOrderedContacts[ 0 ]->UserId() ) )
        {
        // there are items and the contact should not be inserted to beginning
        // => find the correct place
        TLinearOrder< MCAStoredContact > userIdOrder( *CompareUserIdAlphabetically );
        if ( KErrNone == iOrderedContacts.FindInOrder( aContact, idIndex, userIdOrder ) )
            {
            // this contact already exists, return it
            return iOrderedContacts[ idIndex ];
            }
        }

    // the position is now correct, insert the contact
    iOrderedContacts.InsertL( aContact, idIndex );

    // insert also to list sorted by contact "identification"
    TInt err( KErrNone );
    count = iContacts.Count();
    TLinearOrder< MCAStoredContact >& order = iSorter.InsertOrder();
    if ( count &&  0 > ( *order )( aContact, iContacts[ 0 ] ) )
        {
        // the item should be inserted in the beginning of the array
        err = iContacts.Insert( aContact, 0 );
        }
    else
        {
        // insert in correct position
        err = iContacts.InsertInOrderAllowRepeats( aContact, order );
        }

    if ( err != KErrNone )
        {
        // appending to second array did not succeed, so remove from first and leave
        iOrderedContacts.Remove( idIndex );
        User::Leave( err );
        }

    return aContact;
    }

// -----------------------------------------------------------------------------
// CCAContactList::FindContact
// -----------------------------------------------------------------------------
//
TInt CCAContactList::ContactCount( TBool aSkipOfflineContacts,
                                   TBool aSkipBlocekedContacts ) const
    {
    if ( !aSkipOfflineContacts && !aSkipBlocekedContacts )
        {
        return iContacts.Count();
        }

    TInt countOfContacts( 0 );
    TInt count( iContacts.Count() );
    for ( TInt a( 0 ); a < count; ++a )
        {
        TStorageManagerGlobals::TPresenceStatus status(
            iContacts[ a ]->OnlineStatus() );
        if ( status == TStorageManagerGlobals::EOnline ||
             status == TStorageManagerGlobals::EAway ||
             status == TStorageManagerGlobals::EBusy )
            {
            if ( aSkipBlocekedContacts )
                {
                if ( !iContacts[ a ]->IsBlocked() )
                    {
                    ++countOfContacts;
                    }
                }
            else
                {
                ++countOfContacts;
                }
            }
        else
            {
            if ( aSkipBlocekedContacts )
                {
                if ( !iContacts[ a ]->IsBlocked() )
                    {
                    ++countOfContacts;
                    }
                }
            }
        }
    return countOfContacts;
    }

// -----------------------------------------------------------------------------
// CCAContactList::SetCollapsed
// -----------------------------------------------------------------------------
//
void CCAContactList::SetCollapsed( TBool aCollapsed )
    {
    if ( !iInfo.CollapseLocked() )
        {
        *iCurrentCollapsed = aCollapsed;
        }
    }

// -----------------------------------------------------------------------------
// CCAContactList::Collapsed
// -----------------------------------------------------------------------------
//
TBool CCAContactList::Collapsed() const
    {
    return *iCurrentCollapsed;
    }

// -----------------------------------------------------------------------------
// CCAContactList::FindContactIndex
// -----------------------------------------------------------------------------
//
TInt CCAContactList::FindContactIndex( const TDesC& aContactId,
                                       TInt& aOrderedIndex ) const
    {
    return FindContactByUserId( aContactId, aOrderedIndex );
    }

// -----------------------------------------------------------------------------
// CCAContactList::FindContactIndexByNick
// -----------------------------------------------------------------------------
//
TInt CCAContactList::FindContactIndexByNick( const TDesC& aNick ) const
    {
    return FindContactInArray( aNick, ContactFindByNick );
    }

// -----------------------------------------------------------------------------
// CCAContactList::FindContactInArray
// -----------------------------------------------------------------------------
//
TInt CCAContactList::FindContactInArray(
    const TDesC& aId,
    TBool ( *aCompare )( const TDesC&, const MCAStoredContact& ) ) const
    {
    TInt count( iContacts.Count() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( ( *aCompare )( aId, *( iContacts[ x ] ) ) )
            {
            return x;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CCAContactList::FindContactEntry
// -----------------------------------------------------------------------------
//
TInt CCAContactList::FindContactEntry( const MCAStoredContact* aContact ) const
    {
    TInt index( iContacts.Find( aContact ) );
    __ASSERT_DEBUG( index != KErrNotFound , Panic( EContactsArrayOutOfSync ) );
    return index;
    }

// -----------------------------------------------------------------------------
// CCAContactList::FindContactByUserId
// -----------------------------------------------------------------------------
//
TInt CCAContactList::FindContactByUserId( const TDesC& aUserId,
                                          TInt& aIndexOrderedArray ) const
    {
    TInt low( 0 );
    TInt high( iOrderedContacts.Count() );
    while ( high > low )
        {
        TInt m( ( low + high ) / 2 );
        TInt compare( CAUtils::NeutralCompare( aUserId, iOrderedContacts[ m ]->UserId() ) );
        if ( compare == KErrNone )
            {
            aIndexOrderedArray = m;
            return FindContactEntry( iOrderedContacts[ m ] );
            }
        else if ( compare > 0 )
            {
            low = m + 1;
            }
        else
            {
            high = m;
            }
        }
    aIndexOrderedArray = KErrNotFound;
    return KErrNotFound;
    }

// TIdentityRelation
TBool CCAContactList::ContactFindById( const TDesC& aId,
                                       const MCAStoredContact& aContact )
    {
    return ( CAUtils::NeutralCompare( aId,
                                      aContact.UserId() ) == KErrNone );
    }

// TIdentityRelation
TBool CCAContactList::ContactFindByNick( const TDesC& aId,
                                         const MCAStoredContact& aContact )
    {
    // Use folding compare to ignore the case sensitivity, but to retain
    // the comparision of symbols.
    return( KErrNone == aId.CompareF( aContact.Nickname() ) );
    }

TInt CCAContactList::PendingMessages() const
    {
    TInt amount( 0 );
    for ( TInt a( 0 ); a < iContacts.Count(); ++a )
        {
        amount += iContacts[ a ]->PendingMessages();
        }
    return amount;
    }

void CCAContactList::Sort()
    {
    iSorter.Sort( iContacts );
    }

void CCAContactList::ResortContact( MCAStoredContact* aContact )
    {
    // let it panic, if index is not found, which would be bad
    TInt index( iContacts.Find( aContact ) );
    __ASSERT_DEBUG( index != KErrNotFound , Panic( ESortingCorupted ) );
    // Check if resort is even needed for the contact
    TLinearOrder< MCAStoredContact >& order = iSorter.InsertOrder();
    if (
        (
            ( index == 0 )
            ||
            ( 0 >= ( *order )( iContacts[ index - 1], aContact ) )
        )
        &&
        (
            ( index == ( iContacts.Count() - 1 ) )
            ||
            ( 0 <= ( *order )( iContacts[ index + 1], aContact ) )
        )
    )
        {
        // the item is in correct position
        // => no need to resort
        return;
        }

    iContacts.Remove( index );
    // this should alway succeed, since we did not actually add anything to array
    // no need to add contact to the ordered array, should be there already
    TInt err( iContacts.InsertInOrderAllowRepeats( aContact, order ) );
    __ASSERT_DEBUG( err == KErrNone , Panic( ESortingCorupted ) );
    }

void CCAContactList::ResortUnKnownContact( MCAStoredContact* aContact )
    {
    TInt index( iContacts.Find( aContact ) );
    // Check if resort is even needed for the contact
    TLinearOrder< MCAStoredContact >& order = iSorter.InsertOrder();
    if (
        ( KErrNotFound == index )
        ||
        (
            ( index == 0 )
            ||
            ( 0 > ( *order )( iContacts[ index - 1], aContact ) )
        )
        &&
        (
            ( index == ( iContacts.Count() - 1 ) )
            ||
            ( 0 < ( *order )( iContacts[ index + 1], aContact ) )
        )
    )
        {
        // the item is in correct position
        // no need to resort
        return;
        }
    iContacts.Remove( index );
    // this should alway succeed, since we did not actually add anything to array
    // no need to add contact to the ordered array, should be there already
    TInt err( iContacts.InsertInOrderAllowRepeats( aContact, order ) );
    __ASSERT_DEBUG( err == KErrNone , Panic( ESortingCorupted ) );
    }
// -----------------------------------------------------------------------------
// CCAContactListList::Selected
// From MCAContactList
// -----------------------------------------------------------------------------
//
TBool CCAContactList::Selected() const
    {
    return iSelected;
    }

// -----------------------------------------------------------------------------
// CCAContactList::SetSelected
// From MCAContactList
// -----------------------------------------------------------------------------
//
void CCAContactList::SetSelected( TBool aSelected )
    {
    iSelected = aSelected;
    }

// -----------------------------------------------------------------------------
// CCAContactList::FindIndexOfContact
// From MCAContactList
// -----------------------------------------------------------------------------
//
TInt CCAContactList::FindIndexOfContact( const MCAStoredContact* aContact,
                                         TStorageManagerGlobals::TFilterType aFilter  ) const
    {
    TInt count( iContacts.Count() );

    if ( aFilter == TStorageManagerGlobals::EFilterAll )
        {
        for ( TInt a( 0 ); a < count; ++a )
            {
            const MCAStoredContact* contact = iContacts[ a ];
            if ( aContact == contact )
                {
                return a;
                }
            }
        }
    else
        {
        TInt filteredIndex( -1 );

        for ( TInt i( 0 ); i < count; ++i )
            {
            MCAStoredContact* contact ( iContacts[i] );

            if ( FilterAllowsContact( contact, aFilter ) )
                {
                // contact was in the correct state
                filteredIndex++;

                if ( contact == aContact )
                    {
                    // Contact was found, return the index
                    return filteredIndex;
                    }
                }
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CCAContactList::Count
// From MCAContactList
// -----------------------------------------------------------------------------
//
TInt CCAContactList::Count() const
    {
    return ContactCount( EFalse, EFalse );
    }

// -----------------------------------------------------------------------------
// CCAContactList::OnlineCount
// From MCAContactList
// -----------------------------------------------------------------------------
//
TInt CCAContactList::OnlineCount() const
    {
    return FilteredCount( TStorageManagerGlobals::EFilterNonOffline );
    }

// -----------------------------------------------------------------------------
// CCAContactList::NonBlockedCount
// From MCAContactList
// -----------------------------------------------------------------------------
//
TInt CCAContactList::NonBlockedCount() const
    {
    return FilteredCount( TStorageManagerGlobals::EFilterNonBlocked );
    }

// -----------------------------------------------------------------------------
// CCAContactList::operator[]
// From MCAContactList
// -----------------------------------------------------------------------------
//
MCAStoredContact& CCAContactList::operator[]( TInt aIndex ) const
    {
    __CHAT_ASSERT_DEBUG( aIndex < iContacts.Count() )
    return *iContacts[ aIndex ];
    }

// -----------------------------------------------------------------------------
// CCAContactList::OnlineContact
// From MCAContactList
// -----------------------------------------------------------------------------
//
MCAStoredContact& CCAContactList::OnlineContact( TInt aIndex ) const
    {
    return FilteredContact(
               aIndex, TStorageManagerGlobals::EFilterNonOffline );
    }

// -----------------------------------------------------------------------------
// CCAContactList::NonBlockedContact
// From MCAContactList
// -----------------------------------------------------------------------------
//
MCAStoredContact& CCAContactList::NonBlockedContact( TInt aIndex ) const
    {
    return FilteredContact(
               aIndex, TStorageManagerGlobals::EFilterNonBlocked );
    }


// -----------------------------------------------------------------------------
// CCAContactList::SetPrimaryInuse
// From MCAContactList
// -----------------------------------------------------------------------------
//
void CCAContactList::SetPrimaryInuse( TBool aPrimaryInUse )
    {
    if ( aPrimaryInUse )
        {
        iCurrentCollapsed = &iPrimaryCollapsed;
        }
    else
        {
        iCurrentCollapsed = &iSecondaryCollapsed;
        }
    }

// -----------------------------------------------------------------------------
// CCAContactList::FilteredContact
// From MCAContactList
// -----------------------------------------------------------------------------
//
MCAStoredContact& CCAContactList::FilteredContact(
    TInt aIndex, TStorageManagerGlobals::TFilterType aFilter ) const
    {
    if ( aFilter == TStorageManagerGlobals::EFilterAll )
        {
        return ( *this )[aIndex];
        }
    else
        {
        TInt filteredIndex( -1 );
        TInt count( iContacts.Count() );

        for ( TInt i( 0 ); i < count; ++i )
            {
            MCAStoredContact* contact = iContacts[i];

            if ( FilterAllowsContact( contact, aFilter ) )
                {
                // Contact was in correct state
                filteredIndex++;

                if ( aIndex == filteredIndex )
                    {
                    // index was also correct
                    return *iContacts[i];
                    }
                }
            }

        __CHAT_ASSERT_DEBUG( EFalse );
        // prevent compiler warning
        return ( MCAStoredContact& )KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CCAContactList::FilteredCount
// From MCAContactList
// -----------------------------------------------------------------------------
//
TInt CCAContactList::FilteredCount(
    TStorageManagerGlobals::TFilterType aFilter ) const
    {
    if ( aFilter == TStorageManagerGlobals::EFilterAll )
        {
        return Count();
        }
    else
        {
        TInt filtered( 0 );
        TInt count( iContacts.Count() );

        for ( TInt i( 0 ); i < count; ++i )
            {
            if ( FilterAllowsContact( iContacts[i], aFilter ) )
                {
                filtered++;
                }
            }

        return filtered;
        }
    }

// -----------------------------------------------------------------------------
// CCAContactList::FilterAllowsContact
// -----------------------------------------------------------------------------
//
TBool CCAContactList::FilterAllowsContact( const MCAStoredContact* aContact,
                                           TStorageManagerGlobals::TFilterType aFilter ) const
    {
    TBool showContact( EFalse );
    TInt myStatus( KErrNone );
    TStorageManagerGlobals::TPresenceStatus status(
        aContact->OnlineStatus() );

    if ( aFilter & TStorageManagerGlobals::EFilterAndOperation )
        {
        myStatus = myStatus | TStorageManagerGlobals::EFilterAndOperation;
        }

    if ( status == TStorageManagerGlobals::EOnline )
        {
        myStatus = myStatus | TStorageManagerGlobals::EFilterOnline;
        }

    if ( status == TStorageManagerGlobals::EOnline ||
         status == TStorageManagerGlobals::EAway ||
         status == TStorageManagerGlobals::EBusy )
        {
        myStatus = myStatus | TStorageManagerGlobals::EFilterNonOffline;
        }

    if ( status == TStorageManagerGlobals::EAway )
        {
        myStatus |= TStorageManagerGlobals::EFilterAway;
        }

    if ( !aContact->IsBlocked() )
        {
        myStatus |= TStorageManagerGlobals::EFilterNonBlocked;
        }

    if ( aFilter & TStorageManagerGlobals::EFilterAndOperation )
        {
        if ( ( myStatus & aFilter ) == aFilter )
            {
            showContact = ETrue;
            }
        }
    else
        {
        if ( myStatus & aFilter )
            {
            showContact = ETrue;
            }
        }

    return showContact;
    }


// -----------------------------------------------------------------------------
// CCAContactList::SetSynchronised
// -----------------------------------------------------------------------------
//
void CCAContactList::SetSynchronised(
    MCAContactList::TSynchroniseState aSynchroniseState /*= ESynchroniseDone*/ )
    {
    iSyncState = aSynchroniseState;
    iObserver.HandleChange( this,
                            NULL,
                            TStorageManagerGlobals::EStorageEventListChanged,
                            EFalse );
    }

// -----------------------------------------------------------------------------
// CCAContactList::Synchronised
// -----------------------------------------------------------------------------
//
MCAContactList::TSynchroniseState CCAContactList::Synchronised() const
    {
    return iSyncState;
    }


//  End of File
