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
* Description:  PC Implementation for the app ui
*
*/



// INCLUDE FILES
#include "CCAApplicationNGPC.h"
#include "MCAStoredGroups.h"
#include "CCAStorageManagerFactory.h"
#include "CCAEngine.h"
#include "MCAStoredGroup.h"
#include "MCAStoredContacts.h"
#include "MCAChatInterface.h"
//const definitions



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCAApplicationNGPC: NewL
// -----------------------------------------------------------------------------
//
CCAApplicationNGPC* CCAApplicationNGPC::NewL( CCAEngine& aEngine )
    {
    CCAApplicationNGPC* self =
        new ( ELeave ) CCAApplicationNGPC( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CCAApplicationNGPC: Constructor
// -----------------------------------------------------------------------------
//
CCAApplicationNGPC::CCAApplicationNGPC( CCAEngine& aEngine )
        : iEngine( aEngine )
    {

    }

// -----------------------------------------------------------------------------
// CCAApplicationNGPC: ~CCAApplicationNGPC
// -----------------------------------------------------------------------------
//
CCAApplicationNGPC::~CCAApplicationNGPC()
    {

    }

// -----------------------------------------------------------------------------
// CCAApplicationNGPC: ConstructL
// -----------------------------------------------------------------------------
//
void CCAApplicationNGPC::ConstructL( )
    {
    iStoredContacts = CCAStorageManagerFactory::ContactListInterfaceL();
    }

// -----------------------------------------------------------------------------
// CCAApplicationNGPC: SetWVHiding
// -----------------------------------------------------------------------------
//
void CCAApplicationNGPC::SetWVHiding( TInt aHide )
    {
    iStoredContacts->SetWVHiding( aHide ? ETrue : EFalse );
    }
// -----------------------------------------------------------------------------
// CCAApplicationNGPC: SetWVHidingPrefixOnly
// -----------------------------------------------------------------------------
//
void CCAApplicationNGPC::SetWVHidingPrefixOnly( TInt aHide )
    {
    iStoredContacts->SetWVHidingPrefixOnly( aHide ? ETrue : EFalse );
    }
// -----------------------------------------------------------------------------
// CCAApplicationNGPC: SetWVHidingPrefixOnly
// -----------------------------------------------------------------------------
//
void CCAApplicationNGPC::SetCapitalizingEnabled( TInt aCapital )
    {
    iStoredContacts->SetCapitalizingEnabled( aCapital ? ETrue : EFalse );
    }
// -----------------------------------------------------------------------------
// CCAApplicationNGPC: Identification
// -----------------------------------------------------------------------------
//
const TPtrC CCAApplicationNGPC::Identification( const TDesC& aUserId )
    {
    return iStoredContacts->Identification( aUserId );
    }

// -----------------------------------------------------------------------------
// CCAApplicationNGPC: MessagesPendingCount
// -----------------------------------------------------------------------------
//

TInt CCAApplicationNGPC::MessagesPendingCount( TInt& aCountOfChats,
                                               TEnumsPC::TUnreadFilter aUnreadFilter )
    {
    MCAMessagesReadInterface::TUnreadFilter unreadFilter =
        ConvertToEngineUnreadFilter( aUnreadFilter );
    return iEngine.ChatInterface().MessagesPendingCount( aCountOfChats, unreadFilter );
    }

// -----------------------------------------------------------------------------
// CCAApplicationNGPC: ChatGroupMessagesPendingCount
// -----------------------------------------------------------------------------
//
TInt CCAApplicationNGPC::ChatGroupMessagesPendingCount( TInt &aCountOfChats ) const
    {
    return iEngine.ChatInterface().ChatGroupMessagesPendingCount( aCountOfChats );
    }


// -----------------------------------------------------------------------------
// CCAApplicationNGPC: ConvertToEngineUnreadFilter
// -----------------------------------------------------------------------------
//
MCAMessagesReadInterface::TUnreadFilter CCAApplicationNGPC::
ConvertToEngineUnreadFilter( TEnumsPC::TUnreadFilter aUnreadFilter )
    {
    MCAMessagesReadInterface::TUnreadFilter
    unreadFilter( MCAMessagesReadInterface::EUnreadReceived );
    switch ( aUnreadFilter )
        {
        case TEnumsPC::EUnreadAll:
            {
            unreadFilter =  MCAMessagesReadInterface::EUnreadAll;
            break;
            }
        case TEnumsPC::EUnreadReceived:
            {
            unreadFilter = MCAMessagesReadInterface::EUnreadReceived;
            break;
            }
        default:
            {
            //nothing to do
            }
        }
    return unreadFilter;
    }

//end of file

