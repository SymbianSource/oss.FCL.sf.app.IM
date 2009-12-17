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
* Description:  Listen Storage Id changes request handler
*
*/



// INCLUDE FILES
#include <e32std.h>
#include "CPEngHandlerListenSIDs.h"
#include "MPEngStorageFolder.h"
#include "CPEngDataEntry.h"

//  Debug prints
#include "PresenceDebugPrint.h"

// MACROS
#define CHECK_INSERTION( aCode )\
    if ( ( aCode != KErrNone ) && ( aCode != KErrAlreadyExists ) )


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::CPEngHandlerListenSIDs
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CPEngHandlerListenSIDs::CPEngHandlerListenSIDs(
    MPEngStorageFolder& aFolderManager,
    TUint32 aSessionID,
    TInt aSubSessionID )
        : CPEngRequestHandler( EFolderSubSessListenSIDsChanges,
                               aSessionID, aSubSessionID ),
        iFolderManager( aFolderManager ),
        iEntries( 5 ) // usually grows by 5
    {
    }


// ----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::ConstructL
// Symbian 2nd phase constructor can leave.
// Read array of the sids to be observer from the client side and
// construct from them array of the data entries,
// if data entry does no exists, NULL pointer is entered to array
// and store id is stored
// ----------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::ConstructL(
    const RPEngMessage& aMessage )
    {
    // grows by 5 usually
    CPtrCArray* sIds = new( ELeave ) CPtrCArray( 5 );
    CleanupStack::PushL( sIds );
    aMessage.ReadDescriptorArrayLC( KMessageSlot0, *sIds );
    RebuildEntryArrayL( *sIds );
    CleanupStack::PopAndDestroy( 2 ); // aMessage.ReadDescriptorArrayLC, sIds
    }



// ----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CPEngHandlerListenSIDs* CPEngHandlerListenSIDs::NewLC(
    MPEngStorageFolder& aFolderManager,
    const RPEngMessage& aMessage,
    TUint32 aSessionID,
    TInt aSubSessionID )
    {
    CPEngHandlerListenSIDs* self =
        new( ELeave ) CPEngHandlerListenSIDs( aFolderManager,
                                              aSessionID,
                                              aSubSessionID );

    // reference counted
    CleanupClosePushL( *self );
    self->ConstructL( aMessage );

    return self;
    }


// Destructor
CPEngHandlerListenSIDs::~CPEngHandlerListenSIDs()
    {
    iReturnArray.Reset();
    // clean entries
    CleanEntriesArrays();
    // rest of cleaning is done in base class
    }


// =============================================================================
// ========== NEW FUNCTIONS OF THE CPEngHandlerListenSIDs ======================
// =============================================================================


// -----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::NotifyChangedSIDs
// Check if handler is interested in changed Store IDs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::NotifyChangedSIDs(
    const RPointerArray<CPEngDataEntry>& aEntries )
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerListenSIDs: NotifyChangedSIDs: %d" ),
             aEntries.Count() );

    // check if changed entries are observed
    TInt count( aEntries.Count() );
    TInt errCode( KErrNone );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        TInt pos( iEntries.Find( aEntries[ x ] ) );
        if ( KErrNotFound != pos )
            {
            // mark it in the array of indexes
            // append the index of store ID what changed
            errCode = iReturnArray.InsertInOrder( pos );
            // was there some error
            CHECK_INSERTION( errCode )
                {
                break;
                }
            }
        }
    // notify changes
    NotifyClientAboutChanges( errCode );
    }



// ----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::NotifyAllSIdsChanged
// ----------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::NotifyAllSIdsChanged()
    {
    TInt err( KErrGeneral );
    TInt count( iEntries.Count() );
    // reset previous notifications, we will add all again anyway
    iReturnArray.Reset();
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        err = iReturnArray.Append( x );
        CHECK_INSERTION( err )
            {
            break;
            }
        }
    // notify changes
    NotifyClientAboutChanges( err );
    }


// ----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::EntryCreatedL()
// ----------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::EntryCreatedL( CPEngDataEntry& aEntry )
    {
    TInt notLoadedCount( iNotLoaded.Count() );
    // if none entry is missing, carry on
    if ( !notLoadedCount )
        {
        return;
        }
    // check if it is ours
    const TDesC& sid = aEntry.Key();
    for ( TInt x( 0 ) ; x < notLoadedCount ; ++x )
        {
        if ( KErrNone == iNotLoaded[ x ]->CompareF( sid ) )
            {
            // bingo, find and replace
            // since array index starts from 0, init to -1
            TInt pointer( -1 );
            TInt found( -1 );
            do
                {
                pointer++;
                if ( !( iEntries[ pointer ]  ) )
                    {
                    found++;
                    }
                } while ( found != x );
            // pointer points to the entry pointer to be replaced
            iEntries[ pointer ] = &aEntry;
            aEntry.AddObserverL( *this );
            delete iNotLoaded[ x ];
            iNotLoaded.Remove( x );
            return;
            }
        }
    }


// ============================================================================
// ========== NEW FUNCTIONS OF THE CPEngRequestHandler ========================
// ============================================================================

// ----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::ReloadScoutWithNewMessage()
// ----------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::ReloadScoutWithNewMessageL(
    const RPEngMessage& aNewScout )
    {
    // check what kind reload this is
    switch ( aNewScout.Function() )
        {
        case EFolderSubSessReloadAsynchronousScout:
            {
            // first check if handler is loaded
            if ( iRequestMessage.MessageValid() )
                {
                User::Leave( KErrAlreadyExists );
                }
            iRequestMessage = aNewScout;
            // notify changes
            NotifyClientAboutChanges( KErrNone );
            break;
            }

        case EFolderSubSessUpdateListenSIDsScout:
            {
            // update array of SIDs client is interested in
            // first read it to the temp array
            UpdateClientsEngagedSIDsL( aNewScout );
            break;
            }

        case EFolderSubSessFetchChangedSIDsIndexes:
            {
            // fetch indexes of changes SIds
            FetchChangedSidsL( aNewScout );
            break;
            }

        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    }


// ============================================================================
// ========== NEW FUNCTIONS OF THE MPEngHandlerListenSIDs =====================
// ============================================================================

// ----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::EntryUpdated
// ----------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::EntryUpdated(
    const CPEngDataEntry& aEntry )
    {
    PENG_DP( D_PENG_LIT( "CPEngHandlerListenSIDs: EntryUpdated:" ) );

    // check if changed entry was observed
    TInt completeCode( KErrNone );
    TInt pos( iEntries.Find( &aEntry ) );
    if ( pos != KErrNotFound )
        {
        // mark it in the array of indexes
        // append the index of store ID what changed
        TInt err( iReturnArray.InsertInOrder( pos ) );
        // was there some error
        CHECK_INSERTION( err )
            {
            completeCode = err;
            }
        }
    // notify changes
    NotifyClientAboutChanges( completeCode );
    }


// ----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::EntryDeleted
// ----------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::EntryDeleted(
    const CPEngDataEntry& aEntry )
    {
    TInt pos( iEntries.Find( &aEntry ) );
    if ( KErrNotFound == pos )
        {
        // this entry is not in this observer concern
        return;
        }
    // bad, we need to store SID instead
    // count number of NULL pointer before actual pointer
    TInt newPos( 0 );
    for ( TInt x( 0 ) ; x < pos ; ++x )
        {
        newPos += !( iEntries[ x ] );
        }
    iEntries[ pos ] = NULL;
    // watch is, not clean up stack!!
    HBufC* sid = aEntry.Key().Alloc();
    if ( sid )
        {
        if ( KErrNone != iNotLoaded.Insert( sid, newPos ) )
            {
            delete sid;
            }
        }
    }


// ============================================================================
// ========== NEW PRIVATE FUNCTIONS OF THE CPEngHandlerListenSIDs =============
// ============================================================================

// ----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::NotifyClientAboutChanges
// ----------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::NotifyClientAboutChanges(
    TInt aErrCode )
    {
    if ( ( ( iReturnArray.Count() != 0 ) || ( aErrCode != KErrNone ) )
         && iRequestMessage.MessageValid()
       )
        {
        iRequestMessage.Complete( aErrCode );
        }
    }


// ----------------------------------------------------------------------------
// CPEngHandlerListenSIDs::UpdateClientsEngagedSIDsL
// ----------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::UpdateClientsEngagedSIDsL(
    const RPEngMessage& aMessage )
    {
    // grows by 5 usually
    CPtrCArray* sIds = new( ELeave ) CPtrCArray( 5 );
    CleanupStack::PushL( sIds );

    aMessage.ReadDescriptorArrayLC( KMessageSlot1, *sIds );

    // we need to rebuild array if there is array of the IDs which
    // were changed and not yet notified
    TInt count( sIds->MdcaCount() );
    TInt currentCount( iEntries.Count() );
    // pointes out of array for start
    TInt notLdPtr( 0 );
    TInt rtrArrayPtr( 0 );
    TInt indexOffset( 0 );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( x >= currentCount )
            {
            // original array is already out, append on the end
            AppendDataEntryL( sIds->MdcaPoint( x ) );
            notLdPtr++;
            continue;
            }

        // delete all smaller SIds
        TPtrC sid;
        CPEngDataEntry* dataEntry = NULL;
        TInt cmp( 0 );
        do
            {
            dataEntry = DataEntry( x, notLdPtr, sid );
            cmp = sIds->MdcaPoint( x ).CompareF( sid );
            if ( cmp > 0 )
                {
                // sid cannot be futher in sIds array, remove sid
                iEntries.Remove( x );
                if ( dataEntry )
                    {
                    dataEntry->RemoveObserver( *this );
                    }
                else
                    {
                    delete iNotLoaded[ notLdPtr ];
                    iNotLoaded.Remove( notLdPtr );
                    }

                // check if index should be removed from the return array
                if ( ( rtrArrayPtr < iReturnArray.Count() )
                     &&
                     ( x == iReturnArray[ rtrArrayPtr ] + indexOffset )
                   )
                    {
                    iReturnArray.Remove( rtrArrayPtr );
                    }
                indexOffset--;
                // insert compared id
                currentCount--;
                }
            } while ( ( cmp > 0 ) &&  ( x < currentCount ) );

        if ( cmp != KErrNone )
            {
            // add Sid to entry array
            IndertDataEntryL( sIds->MdcaPoint( x ), x, notLdPtr );
            indexOffset++;
            currentCount++;
            }
        else
            {
            // update pointer to not loaded array
            notLdPtr += !dataEntry;
            }

        // update index in return array
        if (	( rtrArrayPtr < iReturnArray.Count() )
             &&
             !indexOffset
             &&
             ( x == ( iReturnArray[ rtrArrayPtr ] + indexOffset ) )
           )
            {
            iReturnArray[ rtrArrayPtr ] = x;
            rtrArrayPtr++;
            }
        }

    // is there something left in the arrays?, clean it
    CleanEntriesArray( count );
    CleanSIdArray( notLdPtr );
    CleanReturnArray( rtrArrayPtr );

    CleanupStack::PopAndDestroy( 2 ); // aMessage.ReadDescriptorArrayLC, sIds
    }


// ---------------------------------------------------------------------------
// CPEngHandlerListenSIDs::FetchChangedSidsL
// ---------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::FetchChangedSidsL(
    const RPEngMessage& aMessage )
    {
    //return index array to client
    aMessage.WriteTIntArrayL( KMessageSlot1, iReturnArray );

    //and reset the contents
    iReturnArray.Reset();
    }


// ---------------------------------------------------------------------------
// CPEngHandlerListenSIDs::CleanEntriesArrays()
// ---------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::CleanEntriesArrays()
    {
    TInt count( iEntries.Count() );
    for ( TInt x( 0 ); x < count ; ++x )
        {
        CPEngDataEntry* dataEntry = iEntries[ x ];
        if ( dataEntry )
            {
            dataEntry->RemoveObserver( *this );
            }
        }
    iEntries.Reset();
    iNotLoaded.ResetAndDestroy();
    }


// ---------------------------------------------------------------------------
// CPEngHandlerListenSIDs::RebuildEntryArrayL()
// ---------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::RebuildEntryArrayL(
    const MDesCArray& aSIds )
    {
    TInt count( aSIds.MdcaCount() );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        AppendDataEntryL( aSIds.MdcaPoint( x ) );
        }
    }

// ---------------------------------------------------------------------------
// CPEngHandlerListenSIDs::DataEntry()
// ---------------------------------------------------------------------------
//
CPEngDataEntry* CPEngHandlerListenSIDs::DataEntry(
    TInt aIndex,
    TInt& aNotLoadedPtr,
    TPtrC& aSId )
    {
    CPEngDataEntry* dataEntry = iEntries[ aIndex ];
    if ( dataEntry )
        {
        aSId.Set( dataEntry->Key() );
        }
    else
        {
        aSId.Set( *( iNotLoaded[ aNotLoadedPtr ] ) );
        }
    return dataEntry;
    }

// ---------------------------------------------------------------------------
// CPEngHandlerListenSIDs::AppendDataEntryL()
// ---------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::AppendDataEntryL(
    const TDesC& aSid )
    {
    CPEngDataEntry* dataEntry =
        iFolderManager.DataEntryL( aSid );

    iEntries.AppendL( dataEntry );
    // if entry does not exist, store store Id
    if ( !dataEntry )
        {
        HBufC* sidBuf = aSid.AllocLC();
        iNotLoaded.AppendL( sidBuf );
        CleanupStack::Pop(); // sidBuf
        }
    else
        {
        dataEntry->AddObserverL( *this );
        }
    }

// ---------------------------------------------------------------------------
// CPEngHandlerListenSIDs::IndertDataEntryL()
// ---------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::IndertDataEntryL(
    const TDesC& aSid,
    TInt aPos,
    TInt& aNotLdPos )
    {
    CPEngDataEntry* dataEntry =
        iFolderManager.DataEntryL( aSid );

    iEntries.InsertL( dataEntry, aPos );
    // if entry does not exist, store store Id
    if ( !dataEntry )
        {
        HBufC* sidBuf = aSid.AllocLC();
        iNotLoaded.InsertL( sidBuf, aNotLdPos );
        CleanupStack::Pop(); // sidBuf
        aNotLdPos++;
        }
    else
        {
        dataEntry->AddObserverL( *this );
        }
    }


// ---------------------------------------------------------------------------
// CPEngHandlerListenSIDs::CleanEntriesArray()
// ---------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::CleanEntriesArray(
    TInt aCount )
    {
    // let is point to last entry
    TInt arrayCount( iEntries.Count() - 1 );
    for ( ; aCount <= arrayCount ; --arrayCount )
        {
        CPEngDataEntry* dataEntry = iEntries[ arrayCount ];
        if ( dataEntry )
            {
            dataEntry->RemoveObserver( *this );
            }
        iEntries.Remove( arrayCount );
        }
    }

// ---------------------------------------------------------------------------
// CPEngHandlerListenSIDs::CleanSIdArray()
// ---------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::CleanSIdArray(
    TInt aCount )
    {
    // let is point to last entry
    TInt arrayCount( iNotLoaded.Count() - 1 );
    for ( ; aCount <= arrayCount ; --arrayCount )
        {
        delete iNotLoaded[ arrayCount ];
        iNotLoaded.Remove( arrayCount );
        }
    }

// ---------------------------------------------------------------------------
// CPEngHandlerListenSIDs::CleanReturnArray()
// ---------------------------------------------------------------------------
//
void CPEngHandlerListenSIDs::CleanReturnArray(
    TInt aCount )
    {
    // let is point to last entry
    TInt arrayCount( iReturnArray.Count() - 1 );
    for ( ; aCount <= arrayCount ; --arrayCount )
        {
        iReturnArray.Remove( arrayCount );
        }
    }
//  End of File
