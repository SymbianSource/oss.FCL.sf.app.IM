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
* Description:  Utility class for presence functions
*
*/



// INCLUDE FILES
#include "CAPresenceDefinitions.h"
#include "CAPresenceUtils.h"
#include "ChatDebugPrint.h"
#include "CCAStorageManagerFactory.h"
#include "MCAStoredContact.h"
#include "MCAStoredContacts.h"
#include "MCAWatcherObserver.h"
#include "CCAPresenceError.h"
#include "CCAPresenceErrors.h"
#include "ImpsCSPAllErrors.h"
#include "CAPresenceConst.h"
#include "MCAContactList.h"
#include "TDecodeAttrParams.h"
#include "SServerPrefers.h"
#include "MCAPresenceUpdater.h"
#include "impsbuilddefinitions.h"

#include "WVUIPresenceVariationNG.hrh"
#include <WVUIPresenceVariationNG.rsg>

#include <PEngWVPresenceAttributes2.h>
#include <MPEngPresenceAttrModel2.h>
#include <MPEngContactList2.h>
#include <MPEngTransactionStatus2.h>
#include <PEngWVPresenceErrors2.h>
#include <ImpsErrors.h>
#include <PEngPresenceEngineErrors2.h>
#include <MPEngContactItem.h>
#include <MPEngContactListProperties.h>
#include <barsread.h> // RResourceReader
#include <eikenv.h>

// "test character identity and accents, ignore case"
const TInt KCollationLevel = 1;
#ifndef IMPS_FETCH_CONTACT_BACKGROUND
const TInt KMaxIdAtTheTime = 2;
#else
const TInt KMaxIdAtTheTime = 20;
#endif //IMPS_FETCH_CONTACT_BACKGROUND

_LIT( KPanicAttribute, "WVUIPresenceVariation attribute" );

// -----------------------------------------------------------------------------
// CAPresenceUtils::DecodeOnlineState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAPresence::TPresenceStatus CAPresenceUtils::DecodeOnlineState(
    const RPointerArray<CCAState>& aStates,
    const RPointerArray<MPEngPresenceAttrModel2>& aAttributes )
    {
    MCAPresence::TPresenceStatus status( MCAPresence::EUnknown );

    TInt stateCount( aStates.Count() );
    TBool stateFound( EFalse );
    for ( TInt stateIndex( 0 ); stateIndex < stateCount && !stateFound; ++stateIndex )
        {
        CCAState* state = aStates[stateIndex];
        TInt stateAttrCount( state->iAttributes.Count() );
        if ( stateAttrCount == 0 )
            {
            // no attributes => default state
            status = MCAPresence::TPresenceStatus( state->iStateId );
            }
        else
            {
            // we have a match if:
            // all attributes defined in state can be found from current
            // set of attributes with same values as in state.

            stateFound = ETrue;
            for ( TInt stateAttrIndex( 0 ); stateAttrIndex < stateAttrCount  &&
                  stateFound; ++stateAttrIndex )
                {
                TCAAttribute& stateAttr = state->iAttributes[stateAttrIndex];

                const MPEngPresenceAttrModel2* attr =
                    FindAttr( stateAttr.iAttribute, aAttributes );
                if ( attr )
                    {
                    // attribute found
                    if ( attr->DataInt( stateAttr.iField, stateAttr.iGroup )
                         != stateAttr.iData ||
                         attr->Qualifier()
                         != stateAttr.iQualifier )
                        {
                        // but different value or qualifier => doesn't match
                        stateFound = EFalse;
                        }
                    }
                else
                    {
                    // not found at all => doesn't match
                    stateFound = EFalse;
                    }
                }

            if ( stateFound )
                {
                status = MCAPresence::TPresenceStatus( state->iStateId );
                }
            }
        }

    CHAT_DP_TXT( "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" );
    return status;
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::PopulateAttributes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAPresenceUtils::PopulateAttributesL( const MPEngPresenceAttrModel2& aModel,
                                           RPointerArray<MPEngPresenceAttrModel2>& aArray,
                                           TStorageManagerGlobals::TClientType& aClientType,
                                           TPtrC& aAlias,
                                           TPtrC& aStatusText )
    {
    switch ( aModel.Type() )
        {
        case KUidPrAttrClientInfo:
            {
            aClientType = TStorageManagerGlobals::EUnknownClient;
            if ( aModel.Qualifier() )
                {
                switch ( aModel.DataInt( EPEngCliInfDeviceType ) )
                    {
                    case EPEngCliDevTypeMobilePhone:
                        {
                        aClientType = TStorageManagerGlobals::EMobile;
                        break;
                        }
                    case EPEngCliDevTypeComputer:
                        {
                        aClientType = TStorageManagerGlobals::EPC;
                        break;
                        }
                    default:
                        {
                        // nothing to do here
                        }
                    }
                }
            break;
            }
        case KUidPrAttrAlias:
            {
            if ( aAlias.Ptr() )
                {
                // have valid descriptor to where this points,
                // otherwise it's unused by the IM Open Conversation
                // routines
                aAlias.Set( aModel.DataDesC16( EPEngAlias ) );
                }
            break;
            }
        case KUidPrAttrStatusText:
            {
            if ( ! aStatusText.Ptr() )
                {
                // have valid descriptor to where this points,
                // otherwise it's unused by the IM Open Conversation
                // routines
                aStatusText.Set( aModel.DataDesC16( EPEngStatusText ) );
                }
            break;
            }
        default:
            {
            aArray.AppendL( &aModel );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::GenerateFriendsArrayLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPtrCArray* CAPresenceUtils::GenerateFriendsArrayLC(
    MCAStoredContacts* aContactStorage )
    {
    TInt granularity( aContactStorage->ContactCount() );
    if ( granularity == 0 )
        {
        granularity = 1;
        }

    CPtrCArray* contactsArray =
        new ( ELeave ) CPtrCArray( granularity );
    CleanupStack::PushL( contactsArray );

    TInt listCount( aContactStorage->ListCount() );
    TKeyArrayFix cmpKey( 0, ECmpCollated );
    for ( TInt a( 0 ); a < listCount; ++a )
        {
        MCAContactList& contactList = aContactStorage->ListAt( a );
        TInt contactCount( contactList.Count() );
        for ( TInt b( 0 ); b < contactCount; ++b )
            {
            TPtrC userId( contactList[ b ].UserId() );
            TInt pos( KErrNotFound );
            if ( userId.Length() > 0 )
                {
                // user id exists
                if ( contactsArray->FindIsq( userId, cmpKey, pos ) != 0 )
                    {
                    // user id not found already from the list, so let's add it
                    contactsArray->InsertIsqL( userId, cmpKey );
                    }
                }
            }
        }
    return contactsArray;
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::PushModelArrayL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAPresenceUtils::PushModelArrayL(
    RPointerArray<MPEngPresenceAttrModel2>& aArray, TBool aOwnsItems )
    {
    if ( aOwnsItems )
        {
        CleanupStack::PushL( TCleanupItem( DestroyCloseModelArray, &aArray ) );
        }
    else
        {
        CleanupStack::PushL( TCleanupItem( CloseModelArray, &aArray ) );
        }
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::DecodeAttrModelsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CAPresenceUtils::DecodeAttrModelsL( struct TDecodeAttrParams& aParams )
    {
    CHAT_DP_TXT( "CAPresenceUtils::DecodeAttrModelsL  Enter" );
    // unpack the structure for easier access
    RPointerArray<CCAState>* states = aParams.iPresenceStates;
    RPointerArray< MPEngPresenceAttrModel2 >& array = aParams.iArray;
    MCAStoredContacts* contactStorage = aParams.iContactStorage;
    MCAWatcherObserver* watcherObserver = aParams.iWatcherObserver;
    CDesCArray* stateOnline = aParams.iStateOnline;
    CDesCArray* stateOffline = aParams.iStateOffline;
    CDesCArray* stateUnknown = aParams.iStateUnknown;
    TInt& entryIndex = aParams.iEntryIndex;

    TBool done( EFalse ); // flag for loop termination
    TBool updateUserData( EFalse );

    MPEngPresenceAttrModel2* model = NULL;
    RPointerArray<MPEngPresenceAttrModel2> attributes;
    CAPresenceUtils::PushModelArrayL( attributes, EFalse );

    // alias support preparations
    TPtrC aliasPtr( NULL, 0 );
    HBufC* dummy = NULL;

    if ( aParams.iServerPrefers.iAliasUsed )
        {
        // have alias support
        // alias ptr must point to non-NULL
        dummy = HBufC::NewL( 1 );
        CleanupStack::PushL( dummy );
        aliasPtr.Set( dummy->Des() );
        }
    else
        {
        // no alias support
        aliasPtr.Set( NULL, 0 );
        }
    TPtrC statusText( NULL, 0 );

    TStorageManagerGlobals::TPresenceStatus newPresenceStatus =
        TStorageManagerGlobals::EUnknown;
    TStorageManagerGlobals::TPresenceStatus oldPresenceStatus =
        TStorageManagerGlobals::EUnknown;
    TStorageManagerGlobals::TClientType clientType =
        TStorageManagerGlobals::EUnknownClient;

    TInt attrCount( array.Count() );
    const TDesC* lastID = NULL;

    CHAT_DP( D_CHAT_LIT( "DecodeAttrModelsL for %d models" ), attrCount );

    // Loop through all attributes received
    TInt idCount( 0 );
    do
        {
        updateUserData = EFalse;

        if ( entryIndex < attrCount )
            {
            model = array[entryIndex];

            // Create initial userID or
            // Check if all attributes concerning this user has been processed
            if ( !lastID )
                {
                lastID = &model->PresenceID();
                }
            else if ( model->PresenceID() != *lastID )
                {
                ++idCount;
                updateUserData = ETrue;
                }
            }
        else
            {
            if ( attrCount > 0 )
                {
                updateUserData = ETrue;
                }
            done = ETrue;
            }

        // If all attributes concerning certain user have been collected,
        // then we can update local presence information concerning that user
        if ( updateUserData )
            {
            newPresenceStatus = MapPresenceStateId(
                                    DecodeOnlineState( *states, attributes ) );

            CAPresenceUtils::FillArraysL( *lastID, newPresenceStatus,
                                          stateOnline, stateOffline,
                                          stateUnknown );
            if ( contactStorage )
                {
                MCAStoredContact* updatedContact = contactStorage->UpdatePresenceL(
                                                       *lastID, newPresenceStatus,
                                                       clientType, aliasPtr,
                                                       statusText,
                                                       oldPresenceStatus );

                if ( updatedContact )
                    {
                    if ( updatedContact->IsWatched()
                         && watcherObserver
                         && ( newPresenceStatus != TStorageManagerGlobals::EUnknown )
                         && !( newPresenceStatus == TStorageManagerGlobals::EOffline
                               && oldPresenceStatus == TStorageManagerGlobals::EUnknown )
                       )
                        {
                        watcherObserver->HandleWatcherEvent( updatedContact );
                        }
                    }
                }

            // reset attributes
            attributes.Reset();
            clientType = TStorageManagerGlobals::EUnknownClient;

            if ( aParams.iServerPrefers.iAliasUsed )
                {
                // have alias support
                // alias ptr must point to non-NULL
                aliasPtr.Set( dummy->Des() );
                }
            statusText.Set( NULL, 0 );

            }

        if ( !done && idCount < KMaxIdAtTheTime )
            {
            lastID = &model->PresenceID();
            CAPresenceUtils::PopulateAttributesL( *model,
                                                  attributes,
                                                  clientType,
                                                  aliasPtr,
                                                  statusText );
            }

        ++entryIndex;
        } while ( !done && idCount < KMaxIdAtTheTime );


    --entryIndex;

    if ( aParams.iServerPrefers.iAliasUsed )
        {
        CleanupStack::PopAndDestroy( dummy );
        }

    CleanupStack::PopAndDestroy(); // attributes.Close()
    CHAT_DP( D_CHAT_LIT( "CAPresenceUtils::DecodeAttrModelsL  Done-:%d" ), done );
    return !done;
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::HandleListErrorsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAPresenceUtils::HandleListErrorsL( TInt aError )
    {
    CHAT_DP( D_CHAT_LIT( "CCAPEngListManager::HandleListErrorsL checking \
	                      error %d" ), aError );

    if ( aError != KErrNone && aError != KErrNotFound
         && aError != KErrAlreadyExists )
        {
        CHAT_DP_TXT( "Error can not be ignored, leaving..." );
        User::Leave( aError );
        }
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::CompareAttrModelArray
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAPresenceUtils::CompareAttrModelArray(
    const MPEngPresenceAttrModel2& aFirst,
    const MPEngPresenceAttrModel2& aSecond )
    {
    return aFirst.PresenceID().CompareC( aSecond.PresenceID(), KCollationLevel,
                                         NULL );
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::CloseModelArray
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAPresenceUtils::CloseModelArray( TAny* aObject )
    {
    reinterpret_cast<RPointerArray<MPEngPresenceAttrModel2>*>( aObject )->
    Close();
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::DestroyCloseModelArray
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAPresenceUtils::DestroyCloseModelArray( TAny* aObject )
    {
    reinterpret_cast<RPointerArray<MPEngPresenceAttrModel2>*>( aObject )->
    ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::FillArraysL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAPresenceUtils::FillArraysL(
    const TDesC& aUserId,
    TStorageManagerGlobals::TPresenceStatus aOnlineStatus,
    CDesCArray *aStateOnline,
    CDesCArray *aStateOffline,
    CDesCArray *aStateUnknown )
    {
    if ( aUserId == KNullDesC )
        {
        // if userID == KNullDesC then this is probably our own presence notify
        // no need to do anything here
        return;
        }

    switch ( aOnlineStatus )
        {
        case TStorageManagerGlobals::EOnline:
        case TStorageManagerGlobals::EAway:
        case TStorageManagerGlobals::EBusy: // flow through
            {
            if ( aStateOnline )
                {
                aStateOnline->AppendL( aUserId );
                }
            break;
            }
        case TStorageManagerGlobals::EOffline:
            {
            if ( aStateOffline )
                {
                aStateOffline->AppendL( aUserId );
                }
            break;
            }
        case TStorageManagerGlobals::EUnknown:
            {
            if ( aStateUnknown )
                {
                aStateUnknown->AppendL( aUserId );
                }
            break;
            }
        default:
            {
            // Unknown state, ignore
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::HandleTransactionStatusL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAPresenceUtils::HandleTransactionStatusL( MPEngTransactionStatus2&
                                                aStatus, CCAPresenceErrors& aErrorContainer )
    {
    CHAT_DP_TXT( "CAPresenceUtils::HandleTransactionStatusL" );

    // Reset the container
    aErrorContainer.Reset();
    aErrorContainer.SetError( CAPresenceUtils::MapErrorPECtoCSP(
                                  aStatus.Status() ) );

    // If partially successful, then fill the local information
    if ( aErrorContainer.Error() == ECSPPartiallySuccessful )
        {
        CHAT_DP_TXT( "CAPresenceUtils::HandleTransactionStatusL, received \
                      ECSPPartiallySuccessful" );
        const MPEngDetailedResultEntry2* detailed = NULL;
        TPtrC data;

        TInt count( aStatus.DetailedResultCount() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            detailed = &aStatus.DetailedResult( i );
            detailed->GetDetailedDesc( data, EPEngDTPresenceID );
            CCAPresenceError* error =
                CCAPresenceError::NewLC( CAPresenceUtils::MapErrorPECtoCSP(
                                             detailed->Error() ), data );
            CHAT_DP( D_CHAT_LIT( "Detailed result (%d): %d: %S" ), i,
                     error->ErrorCode(), &data );

            // Ownership is passed to error container
            aErrorContainer.AddDetailedErrorL( error );
            CleanupStack::Pop( error );
            }
        }
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::MapErrorPECtoCSP
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAPresenceUtils::MapErrorPECtoCSP( TInt aError )
    {
    TInt error( aError );

    // Map everything between KPEngErrorWVServerResponseBase and KPEngErrorBase
    // to Imps_ERROR_BASE
    if ( aError < KPEngErrorWVServerResponseBase &&
         aError > KPEngErrorBase )
        {
        error = aError - KPEngErrorWVServerResponseBase + Imps_ERROR_BASE;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::AppendContactsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CAPresenceUtils::AppendContactsL( MPEngContactList2* aContactList,
                                        CDesCArray* aWVIds,
                                        CDesCArray* aNicknames,
                                        MCAStoredContacts* aContactStorage,
                                        TInt& aEntryIndex,
                                        MCAContactList& aCaContactList,
                                        MCAPresenceUpdater& aPresenceUpdater )
    {
    // Fill arrays
    TInt idCount( 0 );
    TInt count( aContactList->Count() );
    if ( aEntryIndex >= count )
        {
        // list has been changed, possibly we ran out of memory
        aEntryIndex = count - 1;
        }

    if ( aWVIds && aNicknames )
        {
        while ( ( idCount < KMaxIdAtTheTime ) && ( aEntryIndex >= 0 ) )
            {
            MPEngContactItem& contact = aContactList->ContactItem( aEntryIndex );
            TPtrC id = contact.Id();
            TPtrC nick( contact.NickName() );
            // append to lists
            aWVIds->AppendL( id );
            aNicknames->AppendL( nick );
            ++idCount;
            --aEntryIndex;
            }
        }
    else
        {
        RPointerArray<MPEngPresenceAttrModel2> attrModels;
        CAPresenceUtils::PushModelArrayL( attrModels, ETrue );
        RPointerArray<MPEngPresenceAttrModel2> onlineStateModels;
        CAPresenceUtils::PushModelArrayL( onlineStateModels, EFalse );

        TPtrC aliasPtr( NULL, 0 );
        TPtrC statusText( NULL, 0 );
        TStorageManagerGlobals::TClientType clientType =
            TStorageManagerGlobals::EUnknownClient;
        const RPointerArray<CCAState>& presenceStates = aPresenceUpdater.PresenceStates();
        TStorageManagerGlobals::TPresenceStatus presenceStatus =
            TStorageManagerGlobals::EUnknown;

        while ( ( idCount < KMaxIdAtTheTime ) && ( aEntryIndex >= 0 ) )
            {
            MPEngContactItem& contact = aContactList->ContactItem( aEntryIndex );
            TPtrC uderId = contact.Id();
            TPtrC nick( contact.NickName() );

            // fetch attribute for this contact and add it all at once
            aPresenceUpdater.ExtractAttributeModelsForUserL( uderId, attrModels );
            // populate attributes
            TInt attrCount( attrModels.Count() );
            for ( TInt x( 0 ) ; x < attrCount ; ++x )
                {
                CAPresenceUtils::PopulateAttributesL( *attrModels[x],
                                                      onlineStateModels,
                                                      clientType,
                                                      aliasPtr,
                                                      statusText );
                }

            presenceStatus = MapPresenceStateId(
                                 DecodeOnlineState( presenceStates, onlineStateModels ) );


            // update directly to storage
            aContactStorage->CreateContactL( aCaContactList,
                                             nick,
                                             uderId,
                                             presenceStatus,
                                             clientType,
                                             aliasPtr,
                                             statusText );

            // reset attributes
            onlineStateModels.Reset();
            attrModels.ResetAndDestroy();

            clientType = TStorageManagerGlobals::EUnknownClient;
            statusText.Set( NULL, 0 );
            aliasPtr.Set( KNullDesC );

            ++idCount;
            --aEntryIndex;
            }
        CleanupStack::PopAndDestroy( 2 ); // attrModels, onlineStateModels
        }

    // if aEntryIndex is -1, then we are done
    return ( aEntryIndex != -1 );
    }


// -----------------------------------------------------------------------------
// CAPresenceUtils::ReadAttributeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCAAttribute CAPresenceUtils::ReadAttributeL( TResourceReader& aReader )
    {
    TInt attribute( aReader.ReadInt16() );
    TInt qualifier( aReader.ReadInt16() );
    TInt value( aReader.ReadInt16() );

    TCAAttribute attr;
    attr.iQualifier = ( qualifier == EIMQualifierTrue );
    attr.iData = KErrNotFound;
    attr.iField = KErrNotFound;
    attr.iGroup = KPEngDefaultAttrValueGroup;

    switch ( attribute )
        {
        case EIMOnlineAttr:
            {
            attr.iAttribute = KUidPrAttrOnlineStatus;
            attr.iField = EPEngOnlineStatus;
            switch ( value )
                {
                case EIMOnlineNoChange:
                    {
                    // by default, value isn't used
                    break;
                    }
                case EIMOnlineTrue:
                    {
                    attr.iData = EPEngOnlineStatusOnline;
                    break;
                    }
                case EIMOnlineFalse:
                    {
                    attr.iData = EPEngOnlineStatusOffline;
                    break;
                    }
                default:
                    {
                    User::Panic( KPanicAttribute, KErrArgument );
                    }
                }
            break;
            }
        case EIMUserAvailabilityAttr:
            {
            attr.iAttribute = KUidPrAttrUserAvailability;
            attr.iField = EPEngUsrAvailability;
            switch ( value )
                {
                case EIMUserAvailabilityNoChange:
                    {
                    // by default, value isn't used
                    break;
                    }
                case EIMUserAvailabilityNotAvailable:
                    {
                    attr.iData = EPEngUsrAvailabilityOffline;
                    break;
                    }
                case EIMUserAvailabilityDiscreet:
                    {
                    attr.iData = EPEngUsrAvailabilityDiscreet;
                    break;
                    }
                case EIMUserAvailabilityAvailable:
                    {
                    attr.iData = EPEngUsrAvailabilityOnline;
                    break;
                    }
                default:
                    {
                    User::Panic( KPanicAttribute, KErrArgument );
                    }
                }
            break;
            }
        case EIMCommCapAttr:
            {
            attr.iAttribute = KUidPrAttrCommCap;
            attr.iField = EPEngCommCapStatus;
            attr.iGroup = EPEngCommCapIMClient;
            switch ( value )
                {
                case EIMCommCapNoChange:
                    {
                    // by default, value isn't used
                    break;
                    }
                case EIMCommCapClosed:
                    {
                    attr.iData = EPEngCommCapStatusClosed;
                    break;
                    }
                case EIMCommCapOpen:
                    {
                    attr.iData = EPEngCommCapStatusOpen;
                    break;
                    }
                default:
                    {
                    User::Panic( KPanicAttribute, KErrArgument );
                    }
                }
            break;
            }
        case EIMStatusTextAttr:
            {
            attr.iAttribute = KUidPrAttrStatusText;
            switch ( value )
                {
                case EIMStatusTextNoChange:
                    {
                    // by default, value isn't used
                    break;
                    }
                default:
                    {
                    User::Panic( KPanicAttribute, KErrArgument );
                    }
                }
            break;
            }
        default:
            {
            User::Panic( KPanicAttribute, KErrArgument );
            }
        }
    return attr;
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::MapStateId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAPresenceUtils::MapStateId( TInt aResourceStateId )
    {
    MCAPresence::TPresenceStatus ret( MCAPresence::EUnknown );
    switch ( aResourceStateId )
        {
        case EIMStateAvailable:
            {
            ret = MCAPresence::EOnline;
            break;
            }
        case EIMStateAway:
            {
            ret = MCAPresence::EAway;
            break;
            }
        case EIMStateBusy:
            {
            ret = MCAPresence::EBusy;
            break;
            }
        case EIMStateInvisible: // flowthrough at the moment
        case EIMStateOffline:
            {
            ret = MCAPresence::EOffline;
            break;
            }
        default:
            {
            User::Panic( KPanicAttribute, KErrArgument );
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::MapPresenceStateId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TStorageManagerGlobals::TPresenceStatus CAPresenceUtils::MapPresenceStateId(
    MCAPresence::TPresenceStatus aPresenceStateId )
    {
    TStorageManagerGlobals::TPresenceStatus ret( TStorageManagerGlobals::EUnknown );
    switch ( aPresenceStateId )
        {
        case MCAPresence::EOnline:
            {
            ret = TStorageManagerGlobals::EOnline;
            break;
            }
        case MCAPresence::EOffline:
            {
            ret = TStorageManagerGlobals::EOffline;
            break;
            }
        case MCAPresence::EAway:
            {
            ret = TStorageManagerGlobals::EAway;
            break;
            }
        case MCAPresence::EBusy:
            {
            ret = TStorageManagerGlobals::EBusy;
            break;
            }
        default:
            {
            // nothing to do
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::ReadStatesFromResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAPresenceUtils::ReadStatesFromResourceL( TInt aResourceId,
                                               RPointerArray<CCAState>& aStates )
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );

    // reset array
    aStates.ResetAndDestroy();

    // read states
    TInt stateCount( reader.ReadInt16() );
    for ( TInt stateIndex( 0 ); stateIndex < stateCount; ++stateIndex )
        {
        // read state id and create state for it
        TInt stateId( MapStateId( reader.ReadInt16() ) );

        CCAState* state = new( ELeave )CCAState( stateId );
        CleanupStack::PushL( state );

        // and attributes
        TInt attributeCount( reader.ReadInt16() );
        for ( TInt i( 0 ); i < attributeCount; ++i )
            {
            state->iAttributes.AppendL( ReadAttributeL( reader ) );
            }

        // finished
        aStates.AppendL( state );
        CleanupStack::Pop( state );
        }

    CleanupStack::PopAndDestroy(); // reader
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::FindState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCAState* CAPresenceUtils::FindStateL( TInt aState, const RPointerArray<CCAState>& aStates )
    {
    CCAState* tmp = new( ELeave )CCAState( aState );
    CleanupStack::PushL( tmp );

    CCAState* ret = NULL;
    TIdentityRelation< CCAState > equals( CAPresenceUtils::EqualStates );

    TInt index( aStates.Find( tmp, equals ) );
    if ( index != KErrNotFound )
        {
        ret = aStates[index];
        }

    CleanupStack::PopAndDestroy( tmp );
    return ret;
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::FindAttrL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const MPEngPresenceAttrModel2* CAPresenceUtils::FindAttr( TInt aType,
                                                          const RPointerArray<MPEngPresenceAttrModel2>& aArray )
    {
    TInt count( aArray.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        const MPEngPresenceAttrModel2* attr = aArray[i];
        if ( attr->Type() == aType )
            {
            return attr;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CAPresenceUtils::EqualStates
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CAPresenceUtils::EqualStates( const CCAState& aState1, const CCAState& aState2 )
    {
    return ( aState1.iStateId == aState2.iStateId );
    }


//  End of File
