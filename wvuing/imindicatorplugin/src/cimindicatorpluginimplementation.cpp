/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ImIndicatorPlugin plugin implementation
*
*/



// INCLUDE FILES
#include <coemain.h>
#include <coeaui.h>
#include <vwsdef.h>
#include <e32property.h>
#include <imindicatorng.rsg>
#include <stringloader.h>
#include <aknutils.h>

#include "cimindicatorpluginimplementation.h"
#include "chatdebugprint.h"
#include "chatdebugassert.h"
#include "caexternalinterface.h"
#include "caminimalresourcereader.h"
#include "iminternalpskeys.h"

// CONSTANTS
_LIT( KIndicatorResourceFile, "imindicatorng.rsc" );
const TInt KMaxMessageNumberLength = 4; // 9999 = 4 numbers


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CImIndicatorPluginImplementation::CImIndicatorPluginImplementation
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CImIndicatorPluginImplementation::CImIndicatorPluginImplementation()
    {
    }

// ---------------------------------------------------------------------------
// CImIndicatorPluginImplementation::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CImIndicatorPluginImplementation::ConstructL()
    {
    CHAT_DP_FUNC_ENTER( "CImIndicatorPluginImplementation::ConstructL" );

    // Read resource strings
    CAMinimalResourceReader* reader = CAMinimalResourceReader::NewLC();
    TPtrC file( KIndicatorResourceFile );
    reader->OpenResourceFileL( file );

    iPopupHeading =
        reader->ReadTextResourceL( R_QTN_CHAT_STYLUS_POPUP_IM );
    iPopupTextOne =
        reader->ReadTextResourceL( R_QTN_CHAT_STYLUS_POPUP_NEW1 );
    iPopupTextMultiple =
        reader->ReadTextResourceL( R_QTN_CHAT_STYLUS_POPUP_NEW );

    CleanupStack::PopAndDestroy(); // reader

    CHAT_DP_FUNC_DONE( "CImIndicatorPluginImplementation::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CImIndicatorPluginImplementation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImIndicatorPluginImplementation* CImIndicatorPluginImplementation::NewL()
    {
    CImIndicatorPluginImplementation* self =
        new( ELeave ) CImIndicatorPluginImplementation;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CImIndicatorPluginImplementation::~CImIndicatorPluginImplementation()
    {
    CHAT_DP_FUNC_ENTER( "~CImIndicatorPluginImplementation" );
    delete iPopupTextMultiple;
    delete iPopupTextOne;
    delete iPopupHeading;
    CHAT_DP_FUNC_DONE( "~CImIndicatorPluginImplementation" );
    }


// ---------------------------------------------------------------------------
// CImIndicatorPluginImplementation::HandleIndicatorTapL
// ---------------------------------------------------------------------------
//
void CImIndicatorPluginImplementation::HandleIndicatorTapL( const TInt /*aUid*/ )
    {
    CHAT_DP_FUNC_ENTER( "CImIndicatorPluginImplementation::HandleIndicatorTapL" );
    ShowMessageL();
    CHAT_DP_FUNC_DONE( "CImIndicatorPluginImplementation::HandleIndicatorTapL" );
    }



// ---------------------------------------------------------------------------
// CImIndicatorPluginImplementation::CreatePopupContentMessageL()
// ---------------------------------------------------------------------------
//
HBufC* CImIndicatorPluginImplementation::CreatePopupContentMessageL()
    {
    if ( !iPopupTextMultiple || !iPopupTextOne )
        {
        // Load resource strings before calling this method!
        __CHAT_ASSERT_DEBUG( EFalse );
        return KNullDesC().AllocL();
        }

    // Get unread message count
    TInt messageCount = 0;
    RProperty::Get( KPSUidIMUI, KIMUnreadMsgKey, messageCount );

    // Construct proper popup message (singular or plural)
    HBufC* dynStringBuf = NULL;
    if ( messageCount > 1 )
        {
        TInt reqDynLen = iPopupTextMultiple->Length() +
                         KMaxMessageNumberLength;
        dynStringBuf = HBufC::NewL( reqDynLen );
        TPtr dynString = dynStringBuf->Des();
        StringLoader::Format( dynString,
                              iPopupTextMultiple->Des(),
                              -1,             //No index code in source string
                              messageCount );
        }
    else // mesageCount <= 1
        {
        dynStringBuf = iPopupTextOne->AllocL();
        }
    return dynStringBuf;
    }



// ---------------------------------------------------------------------------
// CImIndicatorPluginImplementation::TextL()
// ---------------------------------------------------------------------------
//
HBufC* CImIndicatorPluginImplementation::TextL( const TInt /*aUid*/, TInt& aTextType )
    {
    aTextType = CAknIndicatorPlugin::EAknIndicatorPluginLinkText;
    return CreatePopupContentMessageL();
    }
// ---------------------------------------------------------------------------
// CImIndicatorPluginImplementation::ShowMessageL()
// ---------------------------------------------------------------------------
//
void CImIndicatorPluginImplementation::ShowMessageL()
    {
    // Launch "conversation list view"
    TVwsViewId view( KUidChatClient, KUidFriendsListView );
    CCoeEnv::Static()->AppUi()->CreateActivateViewEventL(
        view, KUidChatPrivateChatListViewSoftNoteActivationId,
        KNullDesC8 );
    }

//  End of File
