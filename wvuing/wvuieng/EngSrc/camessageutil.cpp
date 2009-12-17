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
* Description:  General helper methods for message handling
*
*/


#include <e32std.h>
#include <stringloader.h>

#include "chatdebugprint.h"
#include "camessageutil.h"
#include "mcamessage.h"
#include "mcamessagecreator.h"
#include "mcamessageswriteinterface.h"

#include <caengineNG.rsg> // Resource
#include <avkon.rsg>

// CONSTANTS
const TInt KMaxDateStringLength = 25; // max length of date message

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAMessageUtil::AppendDateStampL
// ---------------------------------------------------------------------------
//
void CAMessageUtil::AppendDateStampL(
    TTime aNewTime,
    MCAMessagesWriteInterface& aWriteInterface,
    MCAMessageCreator& aMessageCreator )
    {
    // DateStamp is NOT appended if:
    // 1. There are no previous messages
    // 2. Previous message is from the same day as aNewTime

    CHAT_DP_FUNC_ENTER( "CAMessageUtil::AppendDateStampL" )
    TTime lastTime = aWriteInterface.Time();

    if ( lastTime == Time::NullTTime() )
        {
        // no previous messages
        return;
        }

    // Compare timestamps
    TDateTime lastDate = lastTime.DateTime();
    TDateTime newDate = aNewTime.DateTime();

    if ( lastDate.Day() != newDate.Day() ||
         lastDate.Month() != newDate.Month() ||
         lastDate.Year() != newDate.Year() )
        {
        // Day is not the same
        // Create a system message about new date

        // Format Date eg. "11.12.2006"
        HBufC* dateFormat = NULL;
        dateFormat = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO );
        HBufC* dateString = HBufC::NewLC( KMaxDateStringLength );
        TPtr dateStringPtr( dateString->Des() );
        aNewTime.FormatL( dateStringPtr, *dateFormat );

        CHAT_DP(
            D_CHAT_LIT( "Adding datestamp to container: %U" ), dateStringPtr );

        // Load date message
        HBufC* dateMessage =
            StringLoader::LoadLC( R_SYSTEM_MESSAGE_DATE_CHANGE,
                                  dateStringPtr );

        // Cleanupstack not needed because ownership
        // is transferred to aWriteInterface
        MCAMessage* message = aMessageCreator.CreateSystemMessageL( // CSI: 35 # See comment above
                                  MCAMessage::ESystemMessageDateChange, *dateMessage );

        // Append message to container
        // ownership is transferred
        aWriteInterface.AppendL( message );

        // dateformat, datestring, dateMessage
        CleanupStack::PopAndDestroy( 3, dateFormat );
        }

    CHAT_DP_FUNC_DONE( "CAMessageUtil::AppendDateStampL" )
    }


// ---------------------------------------------------------------------------
// CAMessageUtil::AppendMessageWithDateStampL
// ---------------------------------------------------------------------------
//
void CAMessageUtil::AppendMessageWithDateStampL(
    MCAMessage& aNewMessage,
    MCAMessagesWriteInterface& aWriteInterface,
    MCAMessageCreator& aMessageCreator,
    TBool aSharedOwnership )
    {
    // Make sure this does not leave before the new message is
    // appended to interface,
    TRAP_IGNORE( AppendDateStampL( aNewMessage.TimeStamp(),
                                   aWriteInterface,
                                   aMessageCreator ) );
    aWriteInterface.AppendL( &aNewMessage, aSharedOwnership );
    }

// End of file
