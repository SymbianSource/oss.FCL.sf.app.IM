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
#include "CCAContact.h"
#include "CCAStorage.h"
#include "TCAStoragePanics.h"
#include "MCAStoredContactsObserver.h"
#include "MCAStorageInfo.h"
#include "TStorageManagerGlobals.h"
#include "ChatDebugPrint.h"
#include "CCAStorageDefs.h"
#include "CAUtils.h"

#include <e32base.h>
#include <e32std.h>
#include <e32def.h>
#include <s32strm.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAContact::CCAContact
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAContact::CCAContact( MCAStoredContactsObserver* aObserver,
                        MCAStorageInfo* aVariation ) :
        iObserver( aObserver ), iVariation( aVariation )
    {
    iPresenceStatus = TStorageManagerGlobals::EUnknown;
    iClientType = TStorageManagerGlobals::EUnknownClient;
    }

// -----------------------------------------------------------------------------
// CCAContact::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAContact::ConstructL()
    {
    // creating empty user id to making sure that we always have some
    // initial "value", even though it is empty.
    // User id is necesssary information
    // for contact.
    iUserId = HBufC::NewL( 1 );

    // same for nickname
    iNickname = HBufC::NewL( 1 );

    // same for alias
    iAlias = HBufC::NewL( 1 );

    UpdateIdentification();
    }

// -----------------------------------------------------------------------------
// CCAContact::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAContact* CCAContact::NewL( MCAStoredContactsObserver* aObserver,
                              MCAStorageInfo* aVariation )
    {
    CCAContact* self = new( ELeave ) CCAContact( aObserver, aVariation );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCAContact::~CCAContact()
    {
    delete iUserId;
    delete iNickname;
    delete iAlias;
    delete iStatusText;
    }

// -----------------------------------------------------------------------------
// CCAContact::UserId
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
const TDesC& CCAContact::UserId() const
    {
    __ASSERT_ALWAYS( iUserId, Panic( ENoSuitableUserId ) );
    return *( iUserId );
    }

// -----------------------------------------------------------------------------
// CCAContact::Nickname
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
const TDesC& CCAContact::Nickname() const
    {
    if ( iNickname )
        {
        return *iNickname;
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CCAContact::Alias
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
const TDesC& CCAContact::Alias() const
    {
    if ( iAlias )
        {
        return *iAlias;
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CCAContact::Identification
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
const TDesC& CCAContact::Identification() const
    {
    return iIdentification;
    }

// -----------------------------------------------------------------------------
// CCAContact::OnlineStatus
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
TStorageManagerGlobals::TPresenceStatus CCAContact::OnlineStatus() const
    {
    return iPresenceStatus;
    }

// -----------------------------------------------------------------------------
// CCAContact::IsBlocked
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
TBool CCAContact::IsBlocked() const
    {
    return iIsBlocked;
    }

// -----------------------------------------------------------------------------
// CCAContact::IsWatched
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
TBool CCAContact::IsWatched() const
    {
    return iWatcherActive;
    }

// -----------------------------------------------------------------------------
// CCAContact::ClientType
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
TStorageManagerGlobals::TClientType CCAContact::ClientType() const
    {
    return iClientType;
    }

// -----------------------------------------------------------------------------
// CCAContact::SetClientType
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetClientType( TStorageManagerGlobals::TClientType aType )
    {
    if ( iClientType != aType )
        {
        // state different, has changed
        iIsChanged |= 1;
        }

    iClientType = aType;
    }

// -----------------------------------------------------------------------------
// CCAContact::SetOnlineStatus
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetOnlineStatus(
    TStorageManagerGlobals::TPresenceStatus aOnlineStatus )
    {
    if ( iPresenceStatus != aOnlineStatus )
        {
        // state different, has changed
        iIsChanged |= 1;
        }

    iPresenceStatus = aOnlineStatus;
    }

// -----------------------------------------------------------------------------
// CCAContact::SetBlocked
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetBlocked( TBool aBlocked )
    {
    if ( iIsBlocked != aBlocked )
        {
        // state different, has changed
        iIsChanged |= 1;
        }

    iIsBlocked = aBlocked;
    }

// -----------------------------------------------------------------------------
// CCAContact::SetWatched
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetWatched( TBool aWatched )
    {
    if ( iWatcherActive != aWatched )
        {
        // state different, has changed
        iIsChanged |= 1;
        }

    iWatcherActive = aWatched;
    }

// -----------------------------------------------------------------------------
// CCAContact::SetPendingMessages
// From MCAExtendedStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetPendingMessages( TInt aAmount )
    {
    if ( iPendingMessages != aAmount )
        {
        // state different, has changed
        iIsChanged |= 1;
        }

    iPendingMessages = aAmount;
    }


// -----------------------------------------------------------------------------
// CCAContact::SignalChanges
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SignalChanges()
    {
    TBool wasChanged( 0 != iIsChanged );
    iIsChanged = 0;

    if ( wasChanged )
        {
        // The observer shall decide whether to pass the information onward.
        iObserver->HandleChange( NULL, this,
                                 TStorageManagerGlobals::EStorageEventPostChange, iUserIdChanged );
        }

    iUserIdChanged = EFalse;
    }

// -----------------------------------------------------------------------------
// CCAContact::Selected
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
TBool CCAContact::Selected() const
    {
    return iSelected;
    }

// -----------------------------------------------------------------------------
// CCAContact::SetSelected
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetSelected( TBool aSelected )
    {
    iSelected = aSelected;
    }

// -----------------------------------------------------------------------------
// CCAContact::SetSelected
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
TInt CCAContact::PendingMessages() const
    {
    return iPendingMessages;
    }

// -----------------------------------------------------------------------------
// CCAContact::StatusText
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
const TDesC& CCAContact::StatusText() const
    {
    if ( iStatusText )
        {
        return *iStatusText;
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CCAContact::SetStatusTextL
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetStatusTextL( const TDesC& aStatusText )
    {
    HBufC* tmp = aStatusText.AllocL();
    delete iStatusText;
    iStatusText = tmp;
    }

// -----------------------------------------------------------------------------
// CCAContact::GetPropertyL
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
HBufC* CCAContact::PropertyL(
    TStorageManagerGlobals::TCAStorageProperty aProperty )
    {
    switch ( aProperty )
        {
        case TStorageManagerGlobals::EStoragePropNickname :
            {
            return iNickname->AllocL();
            // break;  // RVCT
            }
        case TStorageManagerGlobals::EStoragePropAlias :
            {
            return iAlias->AllocL();
            // break;  // RVCT
            }
        default :
            {
            return NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAContact::GetPropertyL
// From MCAStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetPropertyL(
    TStorageManagerGlobals::TCAStorageProperty aProperty,
    const TDesC& aData )
    {
    switch ( aProperty )
        {
        case TStorageManagerGlobals::EStoragePropNickname :
            {
            // set nickname
            HBufC* tmp = aData.AllocL();
            delete iNickname;
            iNickname = tmp;

            UpdateIdentification();
            break;
            }
        case TStorageManagerGlobals::EStoragePropAlias :
            {
            HBufC* tmp = aData.AllocL();
            delete iAlias;
            iAlias = tmp;

            UpdateIdentification();
            break;
            }
        case TStorageManagerGlobals::EStoragePropUserId:
            {
            SetUserIdL( aData );
            break;
            }
        default :
            {
            // no such property
            User::Leave( KErrArgument );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAContact::UpdateIdentification
// Update identification of contact
// -----------------------------------------------------------------------------
//
void CCAContact::UpdateIdentification()
    {
    iIsChanged |= 1;

    if ( iVariation && iVariation->ShowNickname()
         && ( iNickname->Length() > 0 ) )
        {
        // use nickname as identification
        iIdentification.Set( *iNickname );
        return;
        }

    // no nickname
    if ( iAlias->Length() > 0 )
        {
        // use alias as identification from now on
        iIdentification.Set( *iAlias );
        return;
        }

    // otherwise always fallback to userid
    iIdentification.Set( CAUtils::DisplayId( *iUserId ) );
    }

// -----------------------------------------------------------------------------
// CCAContact::SetUserIdL
// From MCAExtendedStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetUserIdL( const TDesC& aUserId )
    {
    HBufC* tempUserId = aUserId.AllocL();
    delete iUserId;
    iUserId = tempUserId;
    iUserIdChanged = ETrue;

    UpdateIdentification();
    }

// -----------------------------------------------------------------------------
// CCAContact::SetNickname
// From MCAExtendedStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetNicknameL( const TDesC& aNickname )
    {
    HBufC* tempNickname = aNickname.AllocL();
    delete iNickname;
    iNickname = tempNickname;

    UpdateIdentification();
    }

// -----------------------------------------------------------------------------
// CCAContact::SetAliasL
// From MCAExtendedStoredContact
// -----------------------------------------------------------------------------
//
void CCAContact::SetAliasL( const TDesC& aAlias )
    {
    HBufC* tempAlias = aAlias.AllocL();
    delete iAlias;
    iAlias = tempAlias;

    UpdateIdentification();
    }


//  End of File
