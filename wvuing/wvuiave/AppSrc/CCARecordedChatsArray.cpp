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
* Description:  Array wrapper for recorded chats view
*
*/


// INCLUDE FILES

#include	"CCARecordedChatsArray.h"
#include	"MCARecordedChatsPC.h"
#include    "MCARecordedChatsArrayPC.h"
#include	"ChatDefinitions.h"
#include	"IMDialogUtils.h"
#include    "chatngclient.hrh"
#include    "IMUtils.h"

#include    <avkon.rsg>
#include    <chatNG.rsg>

#include    <StringLoader.h>
#include    <sendui.h>
#include    <s32mem.h> //CBufStore
#include    <s32file.h>
#include    <txtrich.h>
#include    <avkon.hrh>
#include    <pathinfo.h>
// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

// CONSTANTS
const TInt KOneSelected = 1;
const TInt KNoneSelected = 0;


// Format for listbox item.
// 1\tTextLabel\tTimeAndDate
// 2\tTextLabel\tTimeAndDate
// First icon is mandatory and is chosen between group and I-messages (1\t 2\t ).
//_LIT( KRecordedIMessageIcon, "1\t" );


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::CCARecordedChatsArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCARecordedChatsArray::CCARecordedChatsArray(
    MCARecordedChatsPC& aRecordedChatsPC,
    MCARecordedChatsArrayPC& aRecordedChatsArrayPC
)
        : iRecordedChatsPC( aRecordedChatsPC ),
        iRecordedChatsArrayPC( aRecordedChatsArrayPC )
    {
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCARecordedChatsArray* CCARecordedChatsArray::NewL(
    MCARecordedChatsPC& aRecordedChatsPC,
    MCARecordedChatsArrayPC& aRecordedChatsArrayPC )
    {
    CCARecordedChatsArray* self = new ( ELeave ) CCARecordedChatsArray(
        aRecordedChatsPC, aRecordedChatsArrayPC );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::~CCARecordedChatsArray
// Destructor
// -----------------------------------------------------------------------------
//
CCARecordedChatsArray::~CCARecordedChatsArray()
    {

    }

// ---------------------------------------------------------
// CCARecordedChatsArray::MdcaCount
// Returns the number of descriptor elements in a descriptor array.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCARecordedChatsArray::MdcaCount() const
    {
    TInt recChatCount = iRecordedChatsArrayPC.MdcaCount();
    /*if(recChatCount == 0)
    	{
    	recChatCount = 2;
    	}*/
    return recChatCount;
    }

// ---------------------------------------------------------
// CCARecordedChatsArray::MdcaCount
// Return pointer to descriptor data in given index of an array
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC16 CCARecordedChatsArray::MdcaPoint( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );
    __ASSERT_ALWAYS( iRecordedChatsArrayPC.MdcaCount() > aIndex,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );
    TInt recChatCount = iRecordedChatsArrayPC.MdcaCount();

    return iRecordedChatsArrayPC.MdcaPoint( aIndex );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::ConstructL
// Symbian OS default constructor
// -----------------------------------------------------------------------------
//
void CCARecordedChatsArray::ConstructL()
    {
    // If loggerheadersinterface is not provided, we cannot continue.
    // This happens when there is old version of CCAEngine used with
    // this version of UI.
    if ( !( &iRecordedChatsPC ) )
        {
        User::Leave( KErrNotFound );
        }

    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::Name
// Name of history file
// -----------------------------------------------------------------------------
//
const TDesC& CCARecordedChatsArray::Name( TInt aIndex ) const
    {
    // If history data with aIndex does not exists. iLogger.Name returns
    // KNullDesC, so index check is not needed.
    return iRecordedChatsArrayPC.Name( aIndex );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::DeleteHistoryFilesL
// Delete history file. One or many.
// -----------------------------------------------------------------------------
//
TBool CCARecordedChatsArray::DeleteHistoryFilesL( TInt aIndex )
    {
    iRecordedChatsPC.DeleteHistoryFilesL( aIndex, iDeleteCurrentChat );
    if ( !iDeleteCurrentChat ) // Confirmation is already asked
        {
        if ( !ShowConfirmationNoteL( iRecordedChatsArrayPC.GetNextIndex() ) )
            {
            return EFalse;
            }
        }

    //Set current chat deletion false
    iDeleteCurrentChat = EFalse;

    return ETrue;
    }

// -----------------------------------------------------------------------------
//  CCARecordedChatsArray::DeleteHistoryFileL
//  Delete one file and update index.
//	Return ETrue, when there is still work to be done,
//  and EFalse, when work is done.
// -----------------------------------------------------------------------------
//
TBool CCARecordedChatsArray::DeleteHistoryFileL()
    {
    return iRecordedChatsPC.DeleteHistoryFileL();
    }


// -----------------------------------------------------------------------------
// CCARecordedChatsArray::SetCurrentItemIndex
// Set current item.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsArray::SetCurrentItemIndex( TInt aIndex )
    {
    iRecordedChatsArrayPC.SetCurrentItemIndex( aIndex );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::RecordedChatName
// Name of recorded chat item.
// -----------------------------------------------------------------------------
//
const TDesC& CCARecordedChatsArray::RecordedChatName() const
    {
    return Name( iRecordedChatsArrayPC.GetCurrentItemIndex() );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::RecordedEndDate
// End date of recorded chat item.
// -----------------------------------------------------------------------------
//
TTime CCARecordedChatsArray::RecordedEndDate() const
    {
    return iRecordedChatsArrayPC.EndTime( iRecordedChatsArrayPC.GetCurrentItemIndex() );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::SendMTML
// MTM editor launcher.
// Code Scanner warning to be ignored:L-funtion returing error code
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsArray::SendMTML( TInt aIndex, TUid aTargetUid /*= KNullUid*/ )
    {
    return iRecordedChatsPC.SendMTML( aIndex, aTargetUid );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::CheckAndCorrectMTMFilename
// Check and correct proposed filename for MTM sending purposes.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsArray::CheckAndCorrectMtmFilename( TFileName& aFilename,
                                                        MDesCArray& aArray )
    {
    iRecordedChatsPC.CheckAndCorrectMtmFilename( aFilename, aArray );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::DeleteChatL()
// set current chat to be deleted
// -----------------------------------------------------------------------------
//
TBool CCARecordedChatsArray::DeleteChatL()
    {
    //set current chat to be deleted.
    iDeleteCurrentChat = ShowConfirmationNoteL( iRecordedChatsArrayPC.GetCurrentItemIndex() );
    return iDeleteCurrentChat;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::SendChatViaMTML
// Send current chat via MTM
// -----------------------------------------------------------------------------
//
void CCARecordedChatsArray::SendChatViaMTML( TUid aTargetUid /*= KNullUid*/ )
    {
    SendMTML( iRecordedChatsArrayPC.GetCurrentItemIndex(), aTargetUid );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::CurrentChatDeleteStatus
// Is current chat ordered to be deleted.
// -----------------------------------------------------------------------------
//
TBool CCARecordedChatsArray::CurrentChatDeleteStatus()
    {
    return iDeleteCurrentChat;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::ProgressCount
// Deletion progress count.
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsArray::ProgressCount() const
    {
    return iRecordedChatsArrayPC.GetProgressCount();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::CurrentItemIndex
// Return current item index
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsArray::CurrentItemIndex() const
    {
    return iRecordedChatsArrayPC.GetCurrentItemIndex();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::ShowConfirmationNoteL
// Show confirmation note for deletion.
// -----------------------------------------------------------------------------
//
TBool CCARecordedChatsArray::ShowConfirmationNoteL( TInt aIndex )
    {
    HBufC* prompt = NULL;

    TInt markedCount( KNoneSelected );
    if ( iRecordedChatsPC.ValidSelectedItems() )
        {
        markedCount = iRecordedChatsPC.SelectedItemsCount();
        }
    else
        {
        markedCount = KOneSelected; // From recorded chat view
        }

    // First confirmation note.
    if ( markedCount <= KOneSelected )
        {
        prompt = StringLoader::LoadLC( R_QTN_CHAT_REC_DELETE,
                                       iRecordedChatsArrayPC.Name( aIndex ) );
        }
    else
        {
        prompt = StringLoader::LoadLC( R_QTN_CHAT_REC_DELETE_MANY,
                                       markedCount );
        }

    TInt ret( IMDialogUtils::DisplayQueryDialogL(
                  R_GENERIC_YES_NO_CONFIRMATION_QUERY,
                  *prompt ) );

    CleanupStack::PopAndDestroy( prompt );

    // return confirmation ok status. True, if ok or yes. false if not.
    return ( ret == EAknSoftkeyOk || ret == EAknSoftkeyYes );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::PrepareArrayForSendingMtmL
// Prepares MTM launch component, if not yet prepared.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsArray::PrepareArrayForSendingMtmL()
    {
    iRecordedChatsPC.PrepareArrayForSendingMtmL();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::SetSelectedItems
// Initialize or uninitialize selected items. NULL means uninitialization.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsArray::SetSelectedItems( const CArrayFix< TInt >* aArray )
    {
    iRecordedChatsPC.SetSelectedItems( aArray );
    }


// -----------------------------------------------------------------------------
// CCARecordedChatsArray::DynInitSendMenu
// Dim not needed menubars.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsArray::DynInitSendMenuL(
    CEikMenuPane& /* aMenuPane */
)
    {
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::DeleteRecordedChatContainerL
// -----------------------------------------------------------------------------
//
void CCARecordedChatsArray::DeleteRecordedChatContainerL()
    {
    iRecordedChatsPC.DeleteRecordedChatContainerL();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::InitDeleteArrayL
// Init delete array, so we delete items in right order.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsArray::InitDeleteArrayL()
    {
    iRecordedChatsPC.InitDeleteArrayL();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArray::Index
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsArray::Index( const TDesC& aName ) const
    {
    return iRecordedChatsArrayPC.Index( aName );
    }


//  End of File
