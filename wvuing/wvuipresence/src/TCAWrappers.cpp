/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Presence-list handling interface for clients
*
*/



// INCLUDE FILES

#include "TCAWrappers.h"
#include "e32base.h"
#include "MPEngContactList2.h"
#include "MPEngContactItem.h"
#include "CPEngContactListStore2.h"
#include "CPEngTrackedPresenceIDs2.h"
#include "CPEngTrackedPresenceID2.h"

EXPORT_C TMultiPecListWrapper::TMultiPecListWrapper()
        : iCount( KErrNotFound )
    {
    }

EXPORT_C void TMultiPecListWrapper::InitializeLC( MDesCArray& aLists,
                                                  CPEngContactListStore2& aListStore )
    {
    // build the pointer array
    CleanupClosePushL( *this );
    TInt count( aLists.MdcaCount() );
    iCount = 0;
    TInt listCount( 0 );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        MPEngContactList2& list = aListStore.ContactListL( aLists.MdcaPoint( x ) );
        listCount = list.Count();
        iLists.AppendL( &list );
        iListCounts.AppendL( listCount );
        iCount += listCount;
        }
    }

EXPORT_C void TMultiPecListWrapper::Close()
    {
    iLists.Reset();
    iListCounts.Reset();
    }

EXPORT_C TInt TMultiPecListWrapper::MdcaCount() const
    {
    return iCount;
    }

EXPORT_C TPtrC16 TMultiPecListWrapper::MdcaPoint( TInt aIndex ) const
    {
    // ASSERT for count can be added here
    TInt loop( 0 );
    while ( aIndex >= iListCounts[ loop ] )
        {
        aIndex -= iListCounts[ loop ];
        loop++;
        }
    return iLists[ loop ]->ContactItem( aIndex ).Id();
    }

// ============ One Contact List wrapper ===========================================

EXPORT_C TOneContactListWrapper::TOneContactListWrapper(
    const MPEngContactList2& aCntList )
        : iList( aCntList )
    {
    }

EXPORT_C TInt TOneContactListWrapper::MdcaCount() const
    {
    return iList.Count();
    }

EXPORT_C TPtrC16 TOneContactListWrapper::MdcaPoint(
    TInt aIndex ) const
    {
    return iList.ContactItem( aIndex ).Id();
    }


// ======================= Contact Lists names Wrapper ==============================

EXPORT_C TContactListsWrapper::TContactListsWrapper()
        : iWatcherIndex( KErrNotFound )
    {
    }

EXPORT_C TContactListsWrapper::TContactListsWrapper(
    const MDesCArray& aCntLists )
        : iCntLists( &aCntLists ),
        iWatcherIndex( KErrNotFound )
    {
    TInt index( 0 );
    TInt count( aCntLists.MdcaCount() );
    while (
        ( index < count )
        &&
        ( KErrNone != aCntLists.MdcaPoint( index ).CompareF( KPEngWatcherList )  )
    )
        {
        index++;
        }

    if ( index < count )
        {
        iWatcherIndex = index;
        }
    }

EXPORT_C TInt TContactListsWrapper::MdcaCount() const
    {
    // there is always watcher list
    return ( iWatcherIndex != KErrNotFound ) ? iCntLists->MdcaCount() - 1 : iCntLists->MdcaCount() ;
    }

EXPORT_C TPtrC16 TContactListsWrapper::MdcaPoint(
    TInt aIndex ) const
    {
    return ( ( aIndex < iWatcherIndex ) ?
             iCntLists->MdcaPoint( aIndex )
             : iCntLists->MdcaPoint( aIndex + 1 ) );
    }

EXPORT_C TOneDesWrapper::TOneDesWrapper( const TDesC& aDes )
        : iDes( aDes )
    {
    }

EXPORT_C TInt TOneDesWrapper::MdcaCount() const
    {
    return 1;
    }

EXPORT_C TPtrC16 TOneDesWrapper::MdcaPoint(
    TInt /*aIndex*/ ) const
    {
    // panic if going out of zero index
    return iDes;
    }


EXPORT_C TTrackedPresenceIds::TTrackedPresenceIds()
    {
    }

EXPORT_C void TTrackedPresenceIds::InitializeLC(
    CPEngTrackedPresenceIDs2& aTrackedIds )
    {
    CleanupClosePushL( *this );

    TInt count( aTrackedIds.TrackedPresenceIDsCount() );
    aTrackedIds.ResetTrackedIterator();
    for ( TInt x ( 0 ) ; x < count ; ++x )
        {
        iTrackedIds.AppendL( aTrackedIds.NextTrackedPresenceID() );
        }
    }

EXPORT_C void TTrackedPresenceIds::Close()
    {
    iTrackedIds.Reset();
    }

EXPORT_C TInt TTrackedPresenceIds::MdcaCount() const
    {
    return iTrackedIds.Count();
    }

EXPORT_C TPtrC16 TTrackedPresenceIds::MdcaPoint(
    TInt aIndex ) const
    {
    return iTrackedIds[ aIndex ]->PresenceID();
    }

// End of File

