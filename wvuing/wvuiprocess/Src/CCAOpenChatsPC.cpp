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

#include "CCAOpenChatsPC.h"
#include "CCAArrayItemPC.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCAOpenChatsPC : Ctor
//
// -----------------------------------------------------------------------------
//
CCAOpenChatsPC::CCAOpenChatsPC( const RPointerArray<MCAArrayItemPC>&
                                aOpenChatsArray )
        : iOpenChatsArray( aOpenChatsArray ),
        iActiveItem( -1 )
    {
    }


// -----------------------------------------------------------------------------
// CCAOpenChatsPC : NewL
//
// -----------------------------------------------------------------------------
//
CCAOpenChatsPC* CCAOpenChatsPC::NewL( const RPointerArray<MCAArrayItemPC>&
                                      aOpenChatsArray )
    {
    CCAOpenChatsPC* self = new ( ELeave ) CCAOpenChatsPC( aOpenChatsArray );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAOpenChatsPC : Destructor
//
// -----------------------------------------------------------------------------
//
CCAOpenChatsPC::~CCAOpenChatsPC()
    {
    delete iActiveItemId;
    }


// -----------------------------------------------------------------------------
// CCAOpenChatsPC: GetIdentification
// -----------------------------------------------------------------------------
//
const TPtrC CCAOpenChatsPC::GetIdentification( TInt aIndex )
    {
    if ( aIndex >= ( iOpenChatsArray.Count() ) || aIndex < 0 )
        {
        return TPtrC ( KNullDesC );
        }
    else
        {

        MCAArrayItemPC *arrayItem = ( MCAArrayItemPC * )iOpenChatsArray[aIndex];
        switch ( arrayItem->Type() )
            {

            case TEnumsPC::EConversationItem:
            case TEnumsPC::EGroupItem:
                {
                return arrayItem->GetItemNameText();
                }

            case TEnumsPC::EInviteItem:
                {
                return *iInvitationIdentification;
                }
            default:
                break;

            }
        }

    return TPtrC ( KNullDesC );
    }


// -----------------------------------------------------------------------------
// CCAOpenChatsPC: GetId
// -----------------------------------------------------------------------------
//
const TPtrC CCAOpenChatsPC::GetId( TInt aIndex )
    {
    if ( aIndex >= ( iOpenChatsArray.Count() ) || aIndex < 0 )
        {
        return TPtrC ( KNullDesC );
        }
    else
        {

        MCAArrayItemPC *arrayItem = ( MCAArrayItemPC * )iOpenChatsArray[aIndex];
        switch ( arrayItem->Type() )
            {

            case TEnumsPC::EConversationItem:
                {
                CCAConversationItemPC* conversationItem =
                    dynamic_cast< CCAConversationItemPC* > ( iOpenChatsArray[aIndex] );

                if ( conversationItem )
                    {
                    return 	conversationItem->UserId();
                    }
                break;

                }
            case TEnumsPC::EGroupItem:
                {
                CCAGroupItemPC* groupItem =
                    dynamic_cast< CCAGroupItemPC* > ( iOpenChatsArray[aIndex] );

                if ( groupItem )
                    {
                    return 	groupItem->GroupId();
                    }
                break;
                }

            case TEnumsPC::EInviteItem:
                {
                CCAInviteItemPC* inviteItem =
                    dynamic_cast< CCAInviteItemPC* > ( iOpenChatsArray[aIndex] );

                if ( inviteItem )
                    {
                    return 	inviteItem->GetInviteID();
                    }
                break;
                }
            default:
                break;

            }

        return TPtrC ( KNullDesC );
        }
    }

// -----------------------------------------------------------------------------
// CCAOpenChatsPC: GetType
// -----------------------------------------------------------------------------
//
TEnumsPC::TItem CCAOpenChatsPC::GetType( TInt aIndex )
    {

    if ( aIndex >= ( iOpenChatsArray.Count() ) || aIndex < 0 )
        {
        return TEnumsPC::EInvalid;
        }
    else
        {
        MCAArrayItemPC *arrayItem = ( MCAArrayItemPC * )iOpenChatsArray[aIndex];
        return arrayItem->Type();
        }

    }



// -----------------------------------------------------------------------------
// CCAOpenChatsPC: Count
// -----------------------------------------------------------------------------
//
TInt CCAOpenChatsPC::Count() const
    {
    return iOpenChatsArray.Count();
    }


// -----------------------------------------------------------------------------
// CCAOpenChatsPC: SetActiveItem
// -----------------------------------------------------------------------------
//
void CCAOpenChatsPC::SetActiveItem( const TInt aIndex )
    {
    delete iActiveItemId;
    iActiveItem = aIndex;

    iActiveItemId = GetId( aIndex ).Alloc();

    iActiveItemType = GetType( aIndex );
    }


// -----------------------------------------------------------------------------
// CCAOpenChatsPC: GetActiveItem
// -----------------------------------------------------------------------------
//
void CCAOpenChatsPC::GetActiveItem( TDesC*& aId, TEnumsPC::TItem& aType )
    {

    aId = NULL;

    if ( iActiveItemId )
        {
        aId = iActiveItemId;
        }


    aType = iActiveItemType;

    }

// -----------------------------------------------------------------------------
// CCAOpenChatsPC: SetInvitationIdentification
// -----------------------------------------------------------------------------
//
void CCAOpenChatsPC::SetInvitationIdentification( TDesC* aIdentification )
    {
    iInvitationIdentification = aIdentification;
    }

// End of File

