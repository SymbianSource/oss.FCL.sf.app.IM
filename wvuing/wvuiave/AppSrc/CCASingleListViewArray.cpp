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
* Description:  Single list-array decorator
*
*/


// INCLUDE FILES

#include	"MCAMainViewArrayPC.h"
#include 	"MCAProcessManager.h"
#include	"CCASingleListViewArray.h"

#include    "MCASkinVariant.h"
#include    "ChatDebugAssert.h"
#include    "ChatDebugPrint.h"
#include    "imutils.h"
#include    "ccacontactlistboxmodel.h"
#include	"impsbuilddefinitions.h"
#include	"CCAContactListBox.h"
#include	"chatdefinitions.h"

// platform includes
#include    <chatNG.rsg>
#include    <chatNG.mbg>
#include    <avkon.mbg>
#include    <aknutils.h>
#include    <stringloader.h>
#include    <AknIconUtils.h>
#include    <AknIconArray.h>
#include    <eikclbd.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
// CONSTANTS

_LIT( KTemporaryAll, "All" );
// constants for column numbers (A=0, B=1, C=2, D=3 ) and font indices
const TInt KCColumn = 2;
const TInt KPrimaryFont = 0;
const TInt KSecondaryFont = 1;

// We need to have same icons twice if we're going to use them in both A and
// B columns in the same time. Otherwise the list starts to flicker when
// marquee is acticve. List has to resize the icon all the time to
// refresh the view.

// A-column icons
_LIT( KEmptyIconA,              "4\t" );
_LIT( KListExpandedIcon,        "5\t" );
_LIT( KListCollapsedIcon,       "6\t" );
_LIT( KChatListExpandedIcon,    "21\t" );
_LIT( KChatListCollapsedIcon,   "20\t" );
_LIT( KContactOnlineIconA,      "12\t" );
_LIT( KContactAwayIconA,        "13\t" );
_LIT( KContactOfflineIconA,     "14\t" );

_LIT( KContactSelectedIcon,     "09\t" );
_LIT( KContactNotSelectedIcon,  "10\t" );
_LIT( KContactOnDeviceTypeIconA, "16\t" );
_LIT( KContactAppearOfflineIconA, "19\t" );
// A-column Invitation Icon
_LIT( KInvitationIconA, "18\t" );
// B-column icons
_LIT( KContactOnlineIconB,      "0\t" );
_LIT( KContactOfflineIconB,     "1\t" );
_LIT( KContactBlockedIcon,      "7\t" );
_LIT( KContactAwayIconB,        "8\t" );
_LIT( KContactUnknownIcon,      "11\t" );
_LIT( KEmptyIconB,              "11\t" );
_LIT( KContactBusyIconB,        "15\t" );
_LIT( KContactOnDeviceTypeIconB, "17\t" );

// D-column icons
_LIT( KContactWatcherIcon,      "\t2" );
#ifdef RD_MESSAGE_INDICATION_NEW
_LIT( KContactMsgPendingIcon,   "3\t" );
#else
_LIT( KContactMsgPendingIcon,   "\t3" );
#endif

const TInt KIconIndexMaxLenght = 4;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CCASingleListViewArray::~CCASingleListViewArray()
    {
    delete iData;
    delete iAllText;
    iFonts.Close();
    }


// ---------------------------------------------------------
// CCAChatListView::NewL()
// ---------------------------------------------------------
//
CCASingleListViewArray* CCASingleListViewArray::NewL(
    MCAMainViewArrayPC& aMainViewArray,
    TEnumsPC::TListSelectionType aSelectionList,
    CColumnListBoxData* aListboxData /*= NULL*/,
    TBool aShowIndicators /*= ETrue*/,
    TBool aAutomaticRefresh /*=ETrue*/,
    CCAContactListBox& aListBox )
    {
    CCASingleListViewArray* self = new ( ELeave )
    CCASingleListViewArray(
        aMainViewArray,
        aSelectionList,
        aListboxData,
        aShowIndicators,
        aAutomaticRefresh,
        aListBox );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }



// ---------------------------------------------------------
// Constructor
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CCASingleListViewArray::CCASingleListViewArray(
    MCAMainViewArrayPC& aMainViewArray,
    TEnumsPC::TListSelectionType aSelectionList,
    CColumnListBoxData* aListboxData,
    TBool aShowIndicators,
    TBool aAutomaticRefresh,
    CCAContactListBox& aListBox )
        : iMainViewArray( aMainViewArray ),
        iSelectionList( aSelectionList ),
        iListboxData( aListboxData ),
        iShowIndicators( aShowIndicators ),
        iAutomaticRefresh( aAutomaticRefresh ),
        iListBox ( aListBox )
    {
    iFilter = iMainViewArray.Filter();
    }

// ---------------------------------------------------------
// CCAChatListView::ConstructL()
// Symbian OS default constructor can leave.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::ConstructL()
    {
    // maximum length of icons appended to identification
    // There can be four icons (A+B and two on D-column)
    iMaxLengthOfIcons =
        KEmptyIconB().Length() + // longest B-column icon
        KContactOnlineIconA().Length() + // longest A-Column icon
        KContactWatcherIcon().Length() +
        KContactMsgPendingIcon().Length();

    // Reserve enough memory to handle maximum size item
    iData = HBufC::NewL( KFriendsListMaxIdentificationLength +
                         iMaxLengthOfIcons );


    // set this length to wrapper array...
    //
    iAllText = KTemporaryAll().AllocL();

    // fetch the font pointers
    iFonts.AppendL( AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont ) );
    iFonts.AppendL(
        AknLayoutUtils::FontFromId( EAknLogicalFontSecondaryFont ) );
    }


// ---------------------------------------------------------
// CCASingleListViewArray::LoadIconsLC
// Loads necessary icons
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAknIconArray* CCASingleListViewArray::LoadIconsLC( const TDesC& aMbmFile,
                                                    MCASkinVariant& aSkinVariant )
    {
    // array granularity is 15, because 15 icons are added to array
    CAknIconArray* icons = new( ELeave )CAknIconArray( 15 );
    CleanupStack::PushL( icons );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 0
                        EMbmChatngQgn_prop_im_friend_on,
                        EMbmChatngQgn_prop_im_friend_on_mask,
                        aMbmFile ) );
    icons->AppendL( aSkinVariant.LoadBitmapL( // 1
                        EMbmChatngQgn_prop_im_friend_off,
                        EMbmChatngQgn_prop_im_friend_off_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 2
                        EMbmChatngQgn_indi_im_watch_add,
                        EMbmChatngQgn_indi_im_watch_add_mask,
                        aMbmFile ) );
    icons->AppendL( aSkinVariant.LoadBitmapL( // 3
                        EMbmChatngQgn_prop_im_imsg_new,
                        EMbmChatngQgn_prop_im_imsg_new_mask,
                        aMbmFile ) );

    TPtrC avkonBmpFile( AknIconUtils::AvkonIconFileName() );
    icons->AppendL( aSkinVariant.LoadBitmapL( // 4
                        EMbmAvkonQgn_prop_empty,
                        EMbmAvkonQgn_prop_empty_mask,
                        avkonBmpFile ) );

    /*icons->AppendL( aSkinVariant.LoadBitmapL( // 5
    	EMbmChatngQgn_prop_group_expanded,
    	EMbmChatngQgn_prop_group_expanded_mask,
    	aMbmFile ) );
      icons->AppendL( aSkinVariant.LoadBitmapL( // 6
    	EMbmChatngQgn_prop_group_collapsed,
    	EMbmChatngQgn_prop_group_collapsed_mask,
    	aMbmFile ) );
    */
    icons->AppendL( aSkinVariant.LoadBitmapL( // 5
                        EMbmChatngQgn_prop_im_friends_expanded,
                        EMbmChatngQgn_prop_im_friends_expanded_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 6
                        EMbmChatngQgn_prop_im_friends_collapsed,
                        EMbmChatngQgn_prop_im_friends_collapsed_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 7
                        EMbmChatngQgn_prop_im_user_blocked,
                        EMbmChatngQgn_prop_im_user_blocked_mask,
                        aMbmFile ) );
    icons->AppendL( aSkinVariant.LoadBitmapL( // 8
                        EMbmChatngQgn_prop_im_friend_away,
                        EMbmChatngQgn_prop_im_friend_away_mask,
                        aMbmFile ) );

    //Change the check box icon to correct type as specified by UI Specification
    icons->AppendL( aSkinVariant.LoadBitmapL( // 9
                        EMbmAvkonQgn_prop_checkbox_on,
                        EMbmAvkonQgn_prop_checkbox_on_mask,
                        avkonBmpFile ) );
    icons->AppendL( aSkinVariant.LoadBitmapL( // 10
                        EMbmAvkonQgn_prop_checkbox_off,
                        EMbmAvkonQgn_prop_checkbox_off_mask,
                        avkonBmpFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 11
                        EMbmAvkonQgn_prop_empty,
                        EMbmAvkonQgn_prop_empty_mask,
                        avkonBmpFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 12
                        EMbmChatngQgn_prop_im_friend_on,
                        EMbmChatngQgn_prop_im_friend_on_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 13
                        EMbmChatngQgn_prop_im_friend_away,
                        EMbmChatngQgn_prop_im_friend_away_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 14
                        EMbmChatngQgn_prop_im_friend_off,
                        EMbmChatngQgn_prop_im_friend_off_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 15
                        EMbmChatngQgn_prop_im_friend_busy,
                        EMbmChatngQgn_prop_im_friend_busy_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 16
                        EMbmChatngQgn_prop_im_on_mobile_title,
                        EMbmChatngQgn_prop_im_on_mobile_title_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 17
                        EMbmChatngQgn_prop_im_on_mobile_title,
                        EMbmChatngQgn_prop_im_on_mobile_title_mask,
                        aMbmFile ) );

    //for invitations
    icons->AppendL( aSkinVariant.LoadBitmapL( // 18
                        EMbmChatngQgn_prop_im_new_invit,
                        EMbmChatngQgn_prop_im_new_invit_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 19
                        EMbmChatngQgn_prop_im_friend_invisible,
                        EMbmChatngQgn_prop_im_friend_invisible_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 20
                        EMbmChatngQgn_prop_im_chats_collapsed,
                        EMbmChatngQgn_prop_im_chats_collapsed_mask,
                        aMbmFile ) );

    icons->AppendL( aSkinVariant.LoadBitmapL( // 21
                        EMbmChatngQgn_prop_im_chats_expanded,
                        EMbmChatngQgn_prop_im_chats_expanded_mask,
                        aMbmFile ) );

    return icons;
    }

// ---------------------------------------------------------
// CCASingleListViewArray::SetFilterType( TFilterType aFilter )
// ---------------------------------------------------------
//
void CCASingleListViewArray::SetFilterType(
    TEnumsPC::TFilterType aFilter )
    {
    iFilter = aFilter;
    iMainViewArray.SetSingleListArrayFilter( iFilter );
    }

// ---------------------------------------------------------
// CCASingleListViewArray::MdcaCount
// Returns the number of descriptor elements in a descriptor array.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASingleListViewArray::MdcaCount() const
    {
    return iMainViewArray.Count();
    }

// ---------------------------------------------------------
// CCASingleListViewArray::MdcaCount
// Return pointer to descriptor data in given index of an array
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC16 CCASingleListViewArray::MdcaPoint( TInt aIndex ) const
    {
    // Although this is a const method, we do change the member data.
    // This is done because of performance
    // -> no need to create new buffer every time.
    TPtr dataPtr( iData->Des() );
    dataPtr.Zero();

    TInt fontIndex( KSecondaryFont );

    // fetch the item and process correct type
    TEnumsPC::TItem ItemType = iMainViewArray.GetType( aIndex );

    if ( aIndex == iListBox.TopItemIndex() && iSelectionList == TEnumsPC::ESingleListViewSelect )
        {
        ItemType = TEnumsPC::EOwnStatusItem;
        aIndex = 0;
        }
    else
        {
        ItemType = iMainViewArray.GetType( aIndex );
        }


    switch ( ItemType )
        {
        case TEnumsPC::EOwnStatusItem:
            {
            fontIndex = KPrimaryFont;
            // Own data......
            AppendOwnData( dataPtr, aIndex );
            break;
            }
        case TEnumsPC::EOpenChatsListItem:
            {
            fontIndex = KPrimaryFont;
            // Openchats folder data......
            AppendOpenChatsList( dataPtr, aIndex );
            break;
            }
        case TEnumsPC::EConversationItem:
            {
            // ongoing conversatiosn data......
            AppendConversationItem( dataPtr, aIndex );
            break;
            }

        case TEnumsPC::EContactListItem:
            {
            // contact list
            fontIndex = KPrimaryFont;
            TRAPD( err, AppendContactListL( dataPtr, aIndex ) );

            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                }
            break;
            }
        case TEnumsPC::EContactItem:
            {
            // contact item
            AppendContact( dataPtr, aIndex );
            break;
            }
        case TEnumsPC::EAll:
            {
            // "All"
            AppendAll( dataPtr, aIndex );
            break;
            }

        case TEnumsPC::EInviteItem:
            {
            // InvitationItem data......
            AppendInvitationItem( dataPtr, aIndex );
            break;
            }
        case TEnumsPC::EGroupItem:
            {
            // InvitationItem data......
            AppendGroupItem( dataPtr, aIndex );
            break;
            }
        default:
            {
            // shouldn't be here
            __CHAT_ASSERT_DEBUG( EFalse );
            }
        } //end outer switch

    // set font if we have listbox data
    if ( iListboxData )
        {
        // text is in C-column
        if ( iContactListBoxModel )
            {
            TInt index = iContactListBoxModel->VisibleItemIndex( aIndex );

            ItemType = iMainViewArray.GetType( index );
            TInt err = KErrNone;

            if ( ItemType == TEnumsPC::EOwnStatusItem )
                {
                TInt topIndex = iListBox.TopItemIndex();
                fontIndex = KPrimaryFont;
                TRAP( err, iListboxData->SetColumnFontForRowL( topIndex,
                                                               KCColumn, iFonts[ fontIndex ] ) );

                }
            else
                {
                TRAP( err, iListboxData->SetColumnFontForRowL( index,
                                                               KCColumn, iFonts[ fontIndex ] ) );

                }
            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                }
            }
        }
    return dataPtr;
    }

// ---------------------------------------------------------
// CCASingleListViewArray::AppendOpenChatsList
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::AppendOpenChatsList( TPtr& aBuffer, TInt aIndex )  const
    {
    // append the data to A-column of the list box
    aBuffer.Append( iMainViewArray.IsCollapsed( aIndex )
                    ? KChatListCollapsedIcon : KChatListExpandedIcon );

    // append the data to B-column of the list box
    aBuffer.Append( KTab );

    // append the data to C-column of the list box
    aBuffer.Append( iMainViewArray.GetItemNameText( aIndex ) );


    // D - Column,is not needed

    }

// ---------------------------------------------------------
// CCASingleListViewArray::AppendConversationItem
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::AppendConversationItem( TPtr& aBuffer, TInt aIndex ) const
    {
    // A Column
    aBuffer.Append( KEmptyIconA );

    // B-column
    if ( iMainViewArray.IsMsgPending( aIndex ) )
        {
        aBuffer.Append( KContactMsgPendingIcon );
        }
    else
        {
        aBuffer.Append( KEmptyIconB );
        }


    // C-Column
    aBuffer.Append( iMainViewArray.GetItemNameText( aIndex ) );


    }
// ---------------------------------------------------------
// CCASingleListViewArray::AppendGroupItem
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::AppendGroupItem( TPtr& aBuffer, TInt aIndex ) const
    {
    // A Column
    aBuffer.Append( KEmptyIconA );


    // B-column
    if ( iMainViewArray.IsMsgPending( aIndex ) )
        {
        aBuffer.Append( KContactMsgPendingIcon );
        }
    else
        {
        aBuffer.Append( KEmptyIconB );
        }

    // C-Column
    aBuffer.Append( iMainViewArray.GetItemNameText( aIndex ) );


    }


// ---------------------------------------------------------
// CCASingleListViewArray::AppendInvitationItem
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::AppendInvitationItem( TPtr& aBuffer, TInt aIndex ) const
    {
    // A Column
    aBuffer.Append( KEmptyIconA );

    // B-column
    if ( iMainViewArray.GetType( aIndex ) == TEnumsPC::EInviteItem )
        {
        aBuffer.Append( KInvitationIconA );
        }
    else
        {
        aBuffer.Append( KEmptyIconB );
        }

    // C-Column
    aBuffer.Append( iMainViewArray.GetItemNameText( aIndex ) );
    }

// ---------------------------------------------------------
// CCASingleListViewArray::AppendContact
// Append formatted contact identification to buffer
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::AppendContact( TPtr& aBuffer, TInt aIndex ) const
    {
    // A-column
    if ( iSelectionList == TEnumsPC::EMultiSelect ||
         iSelectionList == TEnumsPC::ESingleListMultiSelect ) //For checkbox to be displayed
        {
        aBuffer.Append( iMainViewArray.Selected( aIndex ) ?
                        KContactSelectedIcon : KContactNotSelectedIcon );
        }
    /*
    * only in single list view we show msg pending icon  and not for SINGLE SELECT
    * or MULTI SELECT mode
    */
    else if ( iSelectionList == TEnumsPC::ESingleListViewSelect )
        {
#ifdef RD_MESSAGE_INDICATION_NEW
        // A-column
        if ( iMainViewArray.IsMsgPending( aIndex ) && iShowIndicators )
            {
            // Add icon if there are unreaded p2p messages
            aBuffer.Append( KContactMsgPendingIcon );
            }
        else
            {
            aBuffer.Append( KEmptyIconA );
            }
#else
        aBuffer.Append( KEmptyIconA );
#endif
        }
    else
        {
        aBuffer.Append( KEmptyIconA );
        }

    // B-column
    if ( iMainViewArray.IsBlocked( aIndex ) )
        {
        // contact is blocked
        aBuffer.Append( KContactBlockedIcon );
        }
    else
        {
        // contact isn't blocked, check the status
        switch ( iMainViewArray.GetOnlineStatus( aIndex ) )
            {
            case TEnumsPC::EOnline:
                {
                TBuf<KIconIndexMaxLenght> icon( KContactOnlineIconB );
                TInt value( 0 );
                TInt ignore( 0 );
                TRAP( ignore, value = IMUtils::IntResourceValueL(
                                          RSC_CHAT_VARIATION_DEVICE_TYPE_TO_ONLINE_STATUS ) );
                if ( value )
                    {
                    if ( iMainViewArray.ClientType( aIndex ) == TEnumsPC::EMobile )
                        {
                        icon = KContactOnDeviceTypeIconB;
                        }
                    }
                aBuffer.Append( icon );
                break;
                }
            case TEnumsPC::EOffline:
                {
                aBuffer.Append( KContactOfflineIconB );
                break;
                }
            case TEnumsPC::EAway:
                {
                aBuffer.Append( KContactAwayIconB );
                break;
                }
            case TEnumsPC::EBusy:
                {
                aBuffer.Append( KContactBusyIconB );
                break;
                }
            case TEnumsPC::EUnknown:  // flowthrough
            default:
                {
                aBuffer.Append( KContactUnknownIcon );
                break;
                }
            }
        }

    // C-column
    TPtrC identification( iMainViewArray.GetItemNameText( aIndex ) );
    aBuffer.Append( identification.Left( aBuffer.MaxLength() - iMaxLengthOfIcons ) );

#ifndef RD_MESSAGE_INDICATION_NEW
    // D-column
    if ( iMainViewArray.IsMsgPending( aIndex ) && iShowIndicators )
        {
        // Add icon if there are unreaded p2p messages
        aBuffer.Append( KContactMsgPendingIcon );
        }
#endif

    if ( iMainViewArray.IsWatched( aIndex ) && iShowIndicators && iAutomaticRefresh )
        {
        // Add icon if contact's presence is watched
        aBuffer.Append( KContactWatcherIcon );
        }
    }//end AppendContact(..)


#ifdef IMPS_CONTACT_FETCH_BACKGROUND
// ---------------------------------------------------------
// CCASingleListViewArray::AppendContactListL
// Append formatted contact list identification to buffer
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::AppendContactListL( TPtr& aBuffer, TInt aIndex ) const
    {
    TEnumsPC::TSyncStatus syncState = iMainViewArray.Synchronised( aIndex );
    // A-column
    if ( syncState == TEnumsPC::ESyncSuccess )
        {
        aBuffer.Append( iMainViewArray.IsCollapsed( aIndex )
                        && !iMainViewArray.IsForceExpanded()
                        ? KListCollapsedIcon : KListExpandedIcon );
        }
    else
        {
        aBuffer.Append( KEmptyIconA );
        }

    // B-column
    aBuffer.Append( KTab );

    // C-column
    // append "number of online contacts / total number
    // of contacts" after identification

    if ( syncState == TEnumsPC::ESyncSuccess  )
        {

        // have only one icon in D-column

        TPtrC identification( iMainViewArray.GetItemNameText( aIndex ) );
        aBuffer.Append( identification.Left( aBuffer.MaxLength() - iMaxLengthOfIcons ) );

        }
    else if ( syncState == TEnumsPC::ESyncFailed )
        {
        HBufC* text = StringLoader::LoadL( R_QTN_CHAT_CONTACTLIST_FETCHING_FAILED,
                                           iMainViewArray.GetItemNameText( aIndex ) );
        aBuffer.Append( text->Des() );
        delete text;
        }
    else
        {
        HBufC* text = StringLoader::LoadL( R_QTN_CHAT_CONTACTLIST_FETCHING );
        aBuffer.Append( text->Des() );
        delete text;
        }
    /*
    #ifndef RD_MESSAGE_INDICATION_NEW
    // D-column
    if( iMainViewArray.IsMsgPending( aIndex ) && iShowIndicators &&
     		syncState == TEnumsPC::ESyncSuccess )
     {
     // Add icon if there are unreaded p2p messages
     aBuffer.Append( KContactMsgPendingIcon );
     }
    #endif
    */
    }

#else
// ---------------------------------------------------------
// CCASingleListViewArray::AppendContactListL
// Append formatted contact list identification to buffer
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::AppendContactListL( TPtr& aBuffer,
                                                 TInt aIndex ) const
    {
    // A-column
    aBuffer.Append( iMainViewArray.IsCollapsed( aIndex )
                    && !iMainViewArray.IsForceExpanded()
                    ? KListCollapsedIcon : KListExpandedIcon );

    // B-column
    aBuffer.Append( KTab );

    // C-column
    // append "number of online contacts / total number
    // of contacts" after identification
    // have only one icon in D-column
    TPtrC identification( iMainViewArray.GetItemNameText( aIndex ) );
    aBuffer.Append( identification.Left(
                        aBuffer.MaxLength() - iMaxLengthOfIcons ) );
    /*
		#ifndef RD_MESSAGE_INDICATION_NEW

    // D-column
    if( iMainViewArray.IsMsgPending(aIndex) > 0 && iShowIndicators )
    	{
    	// Add icon if there are unreaded p2p messages
    	aBuffer.Append( KContactMsgPendingIcon );
    	}
		#endif
    */
    }

#endif //IMPS_CONTACT_FETCH_BACKGROUND



// ---------------------------------------------------------
// CCASingleListViewArray::AppendAll
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCASingleListViewArray::AppendAll( TPtr& aBuffer, TInt aIndex ) const
    {
    // A-column
    if ( iSelectionList == TEnumsPC::EMultiSelect ||
         iSelectionList == TEnumsPC::ESingleListMultiSelect ) //check box for multi select
        {
        aBuffer.Append( iMainViewArray.Selected( aIndex )
                        ? KContactSelectedIcon : KContactNotSelectedIcon );

        }
    else if ( iSelectionList != TEnumsPC::EUnknownSelect )
        {
        aBuffer.Append( KEmptyIconA );
        }

    // B-column
    aBuffer.Append( KTab );

    // C-column
    aBuffer.Append( *iAllText );
    }

// ---------------------------------------------------------
// CCASingleListViewArray::AppendOwnData
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::AppendOwnData( TPtr& aBuffer, TInt aIndex ) const
    {

    // append each string to each column in the list box...

    // append to A-Column
    switch ( iMainViewArray.GetOnlineStatus( aIndex ) )
        {
        case TEnumsPC::EOnline:
            {
            TBuf<KIconIndexMaxLenght> icon( KContactOnlineIconA );
            TInt value( 0 );
            TInt ignore( 0 );
            TRAP( ignore, value = IMUtils::IntResourceValueL(
                                      RSC_CHAT_VARIATION_DEVICE_TYPE_TO_ONLINE_STATUS ) );
            if ( value )
                {
                if ( iMainViewArray.ClientType( aIndex )
                     == TEnumsPC::EMobile )
                    {
                    icon = KContactOnDeviceTypeIconA;
                    }
                }
            aBuffer.Append( icon );
            break;
            }
        case TEnumsPC::EInvisible:
            {
            aBuffer.Append( KContactAppearOfflineIconA );
            break;
            }
        case TEnumsPC::EAway:
            {
            aBuffer.Append( KContactAwayIconA );
            break;
            }
        case TEnumsPC::EOffline: //Flowthrough
        default:
            {
            aBuffer.Append( KContactOfflineIconA );
            break;
            }
        }//end switch of TEnumsPC::EOwnStatusItem
    // append to B-Column
    // B-Column
    aBuffer.Append( KTab );
    aBuffer.Append( iMainViewArray.GetItemNameText( aIndex ) );
    }

// ---------------------------------------------------------
// CCASingleListViewArray::SetContactListModel
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::SetContactListBoxModel( CCAContactListBoxModel* aModel )
    {
    iContactListBoxModel = aModel;
    }

// ---------------------------------------------------------
// CCASingleListViewArray::SetAutomaticRefresh
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASingleListViewArray::SetAutomaticRefresh( TBool aAutomaticRefresh )
    {
    iAutomaticRefresh = aAutomaticRefresh;
    }

//  End of File
