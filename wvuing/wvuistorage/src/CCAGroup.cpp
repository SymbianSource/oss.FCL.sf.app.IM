/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "CCAGroup.h"
#include "CCAStorage.h"
#include "TCAStoragePanics.h"
#include "TStorageManagerGlobals.h"
#include "MCAStoredGroupsObserver.h"

#include <e32base.h>
#include <e32std.h>
#include <e32def.h>
#include <s32strm.h>

#include "ChatDebugPrint.h"

// CLASS FORWARD declarations

// CONSTANTS
const TInt KCAGroupArrayGranularity = 5; // estimated

//LOCAL FUNCTIONS

// -----------------------------------------------------------------------------
// ExternalizeBufferToStreamL
// -----------------------------------------------------------------------------
//
void ExternalizeBufferToStreamL( const HBufC* aBuffer, RWriteStream& aStream )
    {
    if ( aBuffer )
        {
        if ( aBuffer->Length() != 0 )
            {
            aStream.WriteInt32L( aBuffer->Length() );
            aStream.WriteL( *aBuffer );
            return;
            }
        }
    // no buffer or buffer length is zero, writing length information as 0
    aStream.WriteInt32L( 0 );
    }

// -----------------------------------------------------------------------------
// InternalizeBufferFromStreamL
// -----------------------------------------------------------------------------
//
HBufC* InternalizeBufferFromStreamL( RReadStream& aStream )
    {
    HBufC* buffer = NULL;

    TInt length( aStream.ReadInt32L() );
    if ( length != 0 )
        {
        buffer = HBufC::NewLC( length );
        TPtr ptr( buffer->Des() );
        aStream.ReadL( ptr, length );
        CleanupStack::Pop( buffer ); //buffer
        }
    else
        {
        // creating empty buffer with 1 byte length
        buffer = HBufC::NewL( 1 );
        }

    return buffer;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAGroup::CCAGroup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAGroup::CCAGroup( CCAStorage* aStorage,
                    MCAStoredGroupsObserver* aObserver ) :
        iStorage( aStorage ),
        iIsJoined( EFalse ),
        iIsOwnGroup( EFalse ),
        iIsVisible( ETrue ),
        iIsAdmin( EFalse ),
        iObserver( aObserver )
    {
    iStorageType = TStorageManagerGlobals::EUnknownStorageType;
    }

// -----------------------------------------------------------------------------
// CCAGroup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAGroup::ConstructL()
    {
    iMembers = new ( ELeave ) CDesCArraySeg( KCAGroupArrayGranularity );
    iParticipants = new ( ELeave ) CDesCArraySeg( KCAGroupArrayGranularity );

    // must have something inside, so might as well make it empty and length 1
    iServerName = HBufC::NewL( 1 );
    iGroupId = HBufC::NewL( 1 );
    iGroupName = HBufC::NewL( 1 );
    }

// -----------------------------------------------------------------------------
// CCAGroup::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCAGroup* CCAGroup::NewL( CCAStorage* aStorage,
                                   MCAStoredGroupsObserver* aObserver )
    {
    CCAGroup* self = new( ELeave ) CCAGroup( aStorage, aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
EXPORT_C CCAGroup::~CCAGroup()
    {
    if ( iMembers )
        {
        iMembers->Reset();
        delete iMembers;
        }

    if ( iParticipants )
        {
        iParticipants->Reset();
        delete iParticipants;
        }

    delete iGroupId;
    delete iGroupName;
    delete iServerName;
    }

// -----------------------------------------------------------------------------
// CCAGroup::IsJoined
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCAGroup::IsJoined() const
    {
    return iIsJoined;
    }

// -----------------------------------------------------------------------------
// CCAGroup::SetJoined
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::SetJoined( TBool aJoinStatus )
    {
    iIsJoined = aJoinStatus;

    iIsChanged |= 1;
    }

// -----------------------------------------------------------------------------
// CCAGroup::MembersL
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::GetMembersL( CDesCArray& aMemberList ) const
    {
    aMemberList.Reset();
    const TInt count( iMembers->Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        HBufC* buf = iMembers->MdcaPoint( i ).AllocL();
        CleanupStack::PushL( buf );
        aMemberList.AppendL( *buf ); // copy is made..
        CleanupStack::PopAndDestroy( buf ); // buf
        }
    }

// -----------------------------------------------------------------------------
// CCAGroup::AddMembersLocallyL
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::AddMembersLocallyL( const MDesCArray& aMemberList )
    {
    const TInt count( aMemberList.MdcaCount() );
    for ( TInt i = 0; i < count; i++ )
        {
        HBufC* buf = aMemberList.MdcaPoint( i ).AllocL();
        CleanupStack::PushL( buf );
        iMembers->AppendL( *buf ); // makes a copy..
        CleanupStack::PopAndDestroy( buf ); // buf
        iIsChanged |= 1;
        }
    }

// -----------------------------------------------------------------------------
// CCAGroup::MembersLocallyL
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::RemoveMembersLocally( const MDesCArray& aMemberList )
    {
    const TInt count( aMemberList.MdcaCount() );

    for ( TInt i = 0; i < count; i++ )
        {
        TInt pos( 0 );
        if ( iMembers->Find( aMemberList.MdcaPoint( i ),
                             pos, ECmpCollated ) == 0 )
            {
            // found it
            iMembers->Delete( pos );
            iIsChanged |= 1;
            iMembers->Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAGroup::ResetMembersLocally
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::ResetMembersLocally()
    {
    iMembers->Reset();
    iIsChanged |= 1;
    }

// -----------------------------------------------------------------------------
// CCAGroup::ParticipantsL
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::GetParticipantsL( CDesCArray& aParticipantList ) const
    {
    aParticipantList.Reset();
    const TInt count( iParticipants->Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        HBufC* buf = iParticipants->MdcaPoint( i ).AllocL();
        CleanupStack::PushL( buf );
        aParticipantList.AppendL( *buf ); // copy is made..
        CleanupStack::PopAndDestroy( buf ); // buf
        }
    }

// -----------------------------------------------------------------------------
// CCAGroup::AddParticipantL
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::AddParticipantL( const MDesCArray& aParticipantList )
    {
    const TInt count( aParticipantList.MdcaCount() );
    for ( TInt i = 0; i < count; i++ )
        {
        TPtrC screenName( aParticipantList.MdcaPoint( i ) );
        TInt pos( 0 );
        if ( iParticipants->Find( screenName, pos, ECmpCollated ) != 0 )
            {
            // if the participant wasn't already in group, add it
            HBufC* buf = screenName.AllocLC();
            iParticipants->AppendL( *buf ); // makes a copy..
            CleanupStack::PopAndDestroy( buf ); // buf
            iIsChanged |= 1;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAGroup::RemoveParticipantL
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::RemoveParticipantL( const MDesCArray& aParticipantList )
    {
    const TInt count( aParticipantList.MdcaCount() );

    for ( TInt i = 0; i < count; i++ )
        {
        TInt pos( 0 );
        if ( iParticipants->Find( aParticipantList.MdcaPoint( i ),
                                  pos, ECmpCollated ) == 0 )
            {
            // found it
            iParticipants->Delete( pos );
            iIsChanged |= 1;
            iParticipants->Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAGroup::ResetParticipantsL
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::ResetParticipantsL()
    {
    iParticipants->Reset();
    iIsChanged |= 1;
    }

// -----------------------------------------------------------------------------
// CCAGroup::StorageType
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C TStorageManagerGlobals::TCAStorageType CCAGroup::StorageType() const
    {
    return iStorageType;
    }

// -----------------------------------------------------------------------------
// CCAGroup::SetStorageType
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::SetStorageType(
    TStorageManagerGlobals::TCAStorageType aType )
    {
    iStorageType = aType;
    iIsChanged |= 1;
    }

// -----------------------------------------------------------------------------
// CCAGroup::IsOwnGroup
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCAGroup::IsOwnGroup() const
    {
    return iIsOwnGroup;
    }

// -----------------------------------------------------------------------------
// CCAGroup::SetOwnGroup
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::SetOwnGroup( TBool aOwnGroupStatus )
    {
    iIsOwnGroup = aOwnGroupStatus;
    iIsChanged |= 1;
    }

// -----------------------------------------------------------------------------
// CCAGroup::SetGroupIdL
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::SetGroupIdL( const TDesC& aGroupId )
    {
    HBufC* tempGroupId = aGroupId.AllocL();
    delete iGroupId;
    iGroupId = tempGroupId;
    iIsChanged |= 1;
    }

// -----------------------------------------------------------------------------
// CCAGroup::GroupId
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//

EXPORT_C const TDesC& CCAGroup::GroupId() const
    {
    return *( iGroupId );
    }

// -----------------------------------------------------------------------------
// CCAGroup::SetGroupNameL
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::SetGroupNameL( const TDesC& aGroupName )
    {
    HBufC* tempGroupName = aGroupName.AllocL();
    delete iGroupName;
    iGroupName = tempGroupName;
    iIsChanged |= 1;
    }

// -----------------------------------------------------------------------------
// CCAGroup::GroupName
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCAGroup::GroupName() const
    {
    return *( iGroupName );
    }

// -----------------------------------------------------------------------------
// CCAGroup::IsAdmin
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCAGroup::IsAdmin() const
    {
    return iIsAdmin;
    }

// -----------------------------------------------------------------------------
// CCAGroup::SetAdmin
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::SetAdmin( const TBool aStatus )
    {
    iIsAdmin = aStatus;
    iIsChanged |= 1;
    }

// -----------------------------------------------------------------------------
// CCAGroup::NumParticipants
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCAGroup::NumParticipants() const
    {
    return iParticipants->Count();
    }

// -----------------------------------------------------------------------------
// CCAGroup::IsVisible
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCAGroup::IsVisible() const
    {
    return iIsVisible;
    }

// -----------------------------------------------------------------------------
// CCAGroup::IsVisible
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::SetVisible( const TBool aVisible )
    {
    iIsVisible = aVisible;
    iIsChanged |= 1;

    // UI needs to know about this. it's convenient to use delete event...
    if ( iGroupId->Length() > 0 )
        {
        // there's a valid group id
        if ( !iIsVisible )
            {
            // the group have been set to invisible
            // => signal observers
            iObserver->HandleDelete( *iGroupId );
            }
        }

    // otherwise we can't send observer event. we have no valid group id yet!
    }

// -----------------------------------------------------------------------------
// CCAGroup::Server
// From MCAStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCAGroup::Server() const
    {
    return *( iServerName );
    }

// -----------------------------------------------------------------------------
// CCAGroup::Server
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::SetServerL( const TDesC& aServer )
    {
    HBufC* tempServerName = aServer.AllocL();
    delete iServerName;
    iServerName = tempServerName;
    iIsChanged |= 1;
    }

// -----------------------------------------------------------------------------
// CCAGroup::SaveChangesL
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::SaveChangesL()
    {
    TBool wasChanged( 0 != iIsChanged );
    iStorage->SaveL( this );
    iStorageType = TStorageManagerGlobals::EStoragePersistent;
    iIsChanged = 0;

    if ( wasChanged )
        {
        // Notify observer. The observer shall decide whether to pass
        // the information forward.
        iObserver->HandleChange( *iGroupId );
        }
    }

// -----------------------------------------------------------------------------
// CCAGroup::ExternalizeL
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::ExternalizeL( RWriteStream& aStream ) const
    {
    // integers etc.
    aStream.WriteInt8L( ( TInt8 ) iIsOwnGroup );    // TBool

    // Descriptors are externalized as length-data pairs

    ExternalizeBufferToStreamL( iGroupId, aStream );
    CHAT_DP( D_CHAT_LIT( "CCAGroup::ExternalizeL - Group id: %S" ), iGroupId );

    ExternalizeBufferToStreamL( iGroupName, aStream );
    CHAT_DP( D_CHAT_LIT( "CCAGroup::ExternalizeL - Group name: %S" ),
             iGroupName );

    ExternalizeBufferToStreamL( iServerName, aStream );
    CHAT_DP( D_CHAT_LIT( "CCAGroup::ExternalizeL - Group server: %S" ),
             iServerName );
    }

// -----------------------------------------------------------------------------
// CCAGroup::InternalizeL
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAGroup::InternalizeL( RReadStream& aStream )
    {
    // default values are set in the 2nd phase constructor ConstructL

    // integers etc.
    iIsOwnGroup = aStream.ReadInt8L();   // TBool

    // descriptors
    // group id
    HBufC* tempGroupId = InternalizeBufferFromStreamL( aStream );
    CleanupStack::PushL( tempGroupId );

    HBufC* tempGroupName = InternalizeBufferFromStreamL( aStream );
    CleanupStack::PushL( tempGroupName );

    HBufC* tempServerName = InternalizeBufferFromStreamL( aStream );
    CleanupStack::Pop( tempGroupName );
    CleanupStack::Pop( tempGroupId );

    delete iGroupId;
    iGroupId = tempGroupId;
    CHAT_DP( D_CHAT_LIT( "CCAGroup::InternalizeL - Group id: %S" ), iGroupId );

    // group name
    delete iGroupName;
    iGroupName = tempGroupName;
    CHAT_DP( D_CHAT_LIT( "CCAGroup::InternalizeL - Group name: %S" ),
             iGroupName );

    // server name
    delete iServerName;
    iServerName = tempServerName;
    CHAT_DP( D_CHAT_LIT( "CCAGroup::InternalizeL - Group server: %S" ),
             iServerName );

    // CDesCArrays
    // members
    iMembers->Reset();

    // participants
    iParticipants->Reset();

    // must set saved flag
    SetStorageType( TStorageManagerGlobals::EStoragePersistent );

    // set non-visible flag (set this when you want UI to see this)
    SetVisible( ETrue );

    // we have changed
    iIsChanged |= 1;
    }

// -----------------------------------------------------------------------------
// CCAGroup::MaximalSize
// From MCAExtendedStoredGroup
// When changing Externalize/Internalize, be sure to update this method too.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CCAGroup::MaximalSize() const
    {
    TInt len = 1;   // TBool, 8 bit/1 byte

    len += 4;   // length of buffer (32 bit/4 byte)
    len += iGroupId->Size();

    len += 4;   // length of buffer (32 bit/4 byte))
    len += iGroupName->Size();

    len += 4;   // length of buffer (32 bit/4 byte))
    len += iServerName->Size();

    return len;
    }

// -----------------------------------------------------------------------------
// CCAGroup::Version
// From MCAExtendedStoredGroup
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CCAGroup::Version() const
    {
    return KCAGroupVersion;
    }

// End of File
