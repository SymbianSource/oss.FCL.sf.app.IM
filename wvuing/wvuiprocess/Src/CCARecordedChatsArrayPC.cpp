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
* Description:  Array Interface for recorded chats array to interact with
*				 the engine
*
*/


#include "MCALoggerHeadersInterface.h"
#include "CCARecordedChatsArrayPC.h"
#include "MCARecordedChatsPC.h"
#include "chatdefinitions.h"
#include "AknUtils.h"
#include "StringLoader.h"
#include <chatNG.rsg>


// Format for listbox item.
// 1\tTextLabel\tTimeAndDate
// 2\tTextLabel\tTimeAndDate
// First icon is mandatory and is chosen between group and I-messages (1\t 2\t ).
_LIT( KRecordedIMessageIcon, "1\t" );
_LIT( KRecordedGroupChatIcon, "2\t" );

const TInt KGranularity = 2;

// ---------------------------------------------------------
// CCARecordedChatsArrayPC::CCARecordedChatsArrayPC
// ---------------------------------------------------------
//
CCARecordedChatsArrayPC::CCARecordedChatsArrayPC( MCALoggerHeadersInterface&
                                                  aLoggerHeadersInterface )
        : iLoggerHeadersInterface( aLoggerHeadersInterface )
    {

    }

// ---------------------------------------------------------
// CCARecordedChatsArrayPC::NewL
// ---------------------------------------------------------
//
CCARecordedChatsArrayPC* CCARecordedChatsArrayPC::NewL(
    MCALoggerHeadersInterface& aLoggerHeadersInterface )
    {
    CCARecordedChatsArrayPC* self = new ( ELeave ) CCARecordedChatsArrayPC( aLoggerHeadersInterface );
    return self;
    }


// ------------------------------------------------------------------
// CCARecordedChatsArrayPC::FinalizeRecChatsArrayPCConstructionL
// To be called from the app ui once all the construction is over
// since this PC class uses the string resources
// ------------------------------------------------------------------
//
void CCARecordedChatsArrayPC::FinalizeRecChatsArrayPCConstructionL()
    {
    // maximum length of icons appended to identification
    // KRecordedIMessageIcon and KRecordedGroupChatIcon have same length
    TInt maxLengthOfIcons = KRecordedIMessageIcon().Length();

    // Reserve enough memory to handle maximum size item
    iData = HBufC::NewL( KFriendsListMaxIdentificationLength + maxLengthOfIcons );

    // get and format the time/date string
    HBufC* tFormat = CCoeEnv::Static()->AllocReadResourceLC(
                         R_QTN_TIME_USUAL_WITH_ZERO );
    HBufC* dFormat = CCoeEnv::Static()->AllocReadResourceLC(
                         R_QTN_DATE_SHORT_WITH_ZERO );

    CDesCArray* array = new( ELeave )CDesCArrayFlat( KGranularity ); // two items
    CleanupStack::PushL( array );
    array->AppendL( *tFormat );
    array->AppendL( *dFormat );
    iDateFormat = StringLoader::LoadL( R_QTN_CHAT_RECORD_END, *array );
    iStartDateFormat = StringLoader::LoadL(
                           R_CHATCLIENT_HISTORY_START_DATE, *array );
    iEndDateFormat = StringLoader::LoadL( R_CHATCLIENT_HISTORY_END_TIME, *array );

    // Same iTime is used for every format.
    // Take max length of them for buffer size.
    iTime = HBufC::NewL( Max( iEndDateFormat->Length(), Max( iDateFormat->Length(),
                                                             iStartDateFormat->Length() ) ) );

    //Codescanner warning ignored
    CleanupStack::PopAndDestroy( 3, tFormat ); //array, dFormat, tFormat

    }

// ------------------------------------------------------------------
// CCARecordedChatsArrayPC::~CCARecordedChatsArrayPC
// ------------------------------------------------------------------
//
CCARecordedChatsArrayPC::~CCARecordedChatsArrayPC()
    {
    delete iDateFormat;
    delete iStartDateFormat;
    delete iEndDateFormat;
    delete iTime;
    delete iData;
    }

// ------------------------------------------------------------------
// CCARecordedChatsArrayPC::GetDateFormat
// ------------------------------------------------------------------
//
TPtr CCARecordedChatsArrayPC::GetDateFormat()
    {
    return iDateFormat->Des();
    }

// ------------------------------------------------------------------
// CCARecordedChatsArrayPC::GetStartFormat
// ------------------------------------------------------------------
//
TPtr CCARecordedChatsArrayPC::GetStartDateFormat()
    {
    return iStartDateFormat->Des();
    }

// ------------------------------------------------------------------
// CCARecordedChatsArrayPC::GetEndFormat
// ------------------------------------------------------------------
//
TPtr CCARecordedChatsArrayPC::GetEndDateFormat()
    {
    return iEndDateFormat->Des();
    }

// ---------------------------------------------------------
// CCARecordedChatsArrayPC::MdcaCount
// Returns the number of descriptor elements in a descriptor array.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCARecordedChatsArrayPC::MdcaCount() const
    {
    return iLoggerHeadersInterface.HeaderCount();
    }

// ---------------------------------------------------------
// CCARecordedChatsArrayPC::MdcaPoint
// Return pointer to descriptor data in given index of an array
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC16 CCARecordedChatsArrayPC::MdcaPoint( TInt aIndex ) const
    {

    if ( aIndex < 0 || aIndex >= iLoggerHeadersInterface.HeaderCount() )
        {
        return KNullDesC();
        }

    // Copy available identification (nickname/screenname/wvid) to buffer
    // Altough this is const method, next line does affect it's member data,
    // because of performance -> no need to create new buffer every time...
    // Copy max KFriendsListMaxIdentificationLength from identification


    TPtr dataPtr( iData->Des() );
    dataPtr.Copy( Name( aIndex ).Left( KFriendsListMaxIdentificationLength ) );

    //Show chat type icon in front of chat name.
    if ( ChatType( aIndex ) == EPTOPChat )
        {
        dataPtr.Insert( 0, KRecordedIMessageIcon );
        }
    else
        {
        dataPtr.Insert( 0, KRecordedGroupChatIcon );
        }

    //After chat name and before time
    dataPtr.Append( KTabulator );

    TPtr ptrDate( Time() );

    // This hardly fails. And if it for some reason fails, it is not critical,
    // because only thing what user notifies is missing end time of chat
    TRAPD( err, EndTime( aIndex ).FormatL( ptrDate, *iDateFormat ) );
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }

    AknTextUtils::LanguageSpecificNumberConversion( ptrDate );

    //Append time
    dataPtr.Append( Time() );

    return iData->Des();
    }

// ---------------------------------------------------------
// CCARecordedChatsArrayPC::Time
// Return reference to time
// ---------------------------------------------------------
//
TPtr CCARecordedChatsArrayPC::Time() const
    {
    return iTime->Des();
    }

// ---------------------------------------------------------
// CCARecordedChatsArrayPC::Name
// Return reference to descriptor data in given index of an array
// ---------------------------------------------------------
//
const TDesC& CCARecordedChatsArrayPC::Name( TInt aIndex ) const
    {
    return iLoggerHeadersInterface.Name( aIndex );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::ChatType
// -----------------------------------------------------------------------------
//
TEnumsPC::TChatType CCARecordedChatsArrayPC::ChatType( TInt aIndex ) const
    {
    TEnumsPC::TChatType chatType = TEnumsPC::EUnknownChatType;

    switch ( iLoggerHeadersInterface.ChatType( aIndex ) )
        {
        case EPTOPChat:
            {
            chatType = TEnumsPC::EPTOPChat;
            break;
            }
        case EGroupChat:
            {
            chatType = TEnumsPC::EGroupChat;
            break;
            }
        default:
            {
            chatType = TEnumsPC::EUnknownChatType;
            }
        }
    return chatType;
    }
// -----------------------------------------------------------------------------
// CCARecordedChatsArrayPC::StartTime
// -----------------------------------------------------------------------------
//
TTime CCARecordedChatsArrayPC::StartTime( TInt aIndex ) const
    {
    return iLoggerHeadersInterface.StartTime( aIndex );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArrayPC::EndTime
// -----------------------------------------------------------------------------
//
TTime CCARecordedChatsArrayPC::EndTime( TInt aIndex ) const
    {
    return iLoggerHeadersInterface.EndTime( aIndex );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArrayPC::Identification
// -----------------------------------------------------------------------------
//
const TDesC& CCARecordedChatsArrayPC::Identification( TInt aIndex ) const
    {
    return iLoggerHeadersInterface.Identification( aIndex );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsArrayPC::Index
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsArrayPC::Index( const TDesC& aName ) const
    {
    return iLoggerHeadersInterface.Index( aName );
    }
//End of File

