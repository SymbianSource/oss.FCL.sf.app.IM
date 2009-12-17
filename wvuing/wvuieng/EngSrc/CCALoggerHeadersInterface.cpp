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
* Description:  Offers information regarding a given header to the UI.
*
*/


// INCLUDE FILES
#include "CCALoggerHeadersInterface.h"
#include "CALoggerManagerFactory.h"
#include "MCALoggerReadInterface.h"
#include "CCALoggerMessageHeader.h"
#include "CCAMessageBase.h"
#include "MCAChatInterface.h"
#include "MCASettings.h"
#include "MCAMessagesWriteInterface.h"
#include "privateenginedefinitions.h"


// ================= HELPER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CleanupCArrayPtrCGulIcon
// helper method for ResetAndDestroying the CSeletionItemList array
// -----------------------------------------------------------------------------
//
void CleanupRPointerMCALoggerMessageArray( TAny* aArray )
    {
    RPointerArray<MCALoggerMessage>* items =
        static_cast<RPointerArray<MCALoggerMessage>*>( aArray );
    if ( items )
        {
        items->ResetAndDestroy();
        }
    }

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCALoggerHeadersInterface* CCALoggerHeadersInterface::NewL(
    MCAChatInterface& aChatInterface,
    MCASettings* aSettingsAPI )
    {
    CCALoggerHeadersInterface* self =
        new ( ELeave ) CCALoggerHeadersInterface( aChatInterface,
                                                  aSettingsAPI );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::~CCALoggerHeadersInterface
// Destructor
// -----------------------------------------------------------------------------
//
CCALoggerHeadersInterface::~CCALoggerHeadersInterface()
    {
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::ConstructL
// Construction
// -----------------------------------------------------------------------------
//
void CCALoggerHeadersInterface::ConstructL()
    {
    //Messagefactory must be setted to loggermanager so it does not have to be
    //setted here.
    iLogger = CALoggerManagerFactory::ReadInstanceL();
    iLogger->GetLoggerMessageHeaderArray( iHeaderArray );

    //If header array is not valid -> leave with KErrNotFound.
    if ( !iHeaderArray )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::CCALoggerHeadersInterface
// Default Construction
// -----------------------------------------------------------------------------
//
CCALoggerHeadersInterface::CCALoggerHeadersInterface(
    MCAChatInterface& aChatInterface,
    MCASettings* aSettingsAPI )
        : iChatInterface( aChatInterface ),
        iSettingsAPI( aSettingsAPI )
    {
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::LoggerMessageHeader
// Return header in index.
// -----------------------------------------------------------------------------
//
CCALoggerMessageHeader* CCALoggerHeadersInterface::LoggerMessageHeader(
    TInt aIndex ) const
    {
    __ASSERT_DEBUG( ( aIndex >= 0 && aIndex < iHeaderArray->Count() ),
                    User::Panic( KCAEnginePanicCategory,
                                 EApplicationUserSentThisIndexNotInValidRange ) );

    // WVLogger does not identify header classes in array. It is clients
    // responsibility to know type of header classes used in headerarray.
    // In this case, header array is filled by CCALoggerMessageHeader classes.
    return static_cast< CCALoggerMessageHeader* >( ( *iHeaderArray )[ aIndex ] );
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::ResetAndDestroyMessages
//
// -----------------------------------------------------------------------------
//
void CCALoggerHeadersInterface::ResetAndDestroyMessages( TAny* aObject )
    {
    reinterpret_cast<RPointerArray< MCALoggerMessage >*>( aObject )->ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::Name
//
// -----------------------------------------------------------------------------
//
const TDesC& CCALoggerHeadersInterface::Name( TInt aIndex ) const
    {
    return LoggerMessageHeader( aIndex )->Name();
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::StartTime
//
// -----------------------------------------------------------------------------
//
TTime CCALoggerHeadersInterface::StartTime( TInt aIndex ) const
    {
    return LoggerMessageHeader( aIndex )->StartTime();
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::EndTime
//
// -----------------------------------------------------------------------------
//
TTime CCALoggerHeadersInterface::EndTime( TInt aIndex ) const
    {
    return LoggerMessageHeader( aIndex )->EndTime();
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::Identification
//
// -----------------------------------------------------------------------------
//
const TDesC& CCALoggerHeadersInterface::Identification( TInt aIndex ) const
    {
    return LoggerMessageHeader( aIndex )->Identification();
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::Count
//
// -----------------------------------------------------------------------------
//
TInt CCALoggerHeadersInterface::HeaderCount() const
    {
    return iHeaderArray->Count();
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::ChatType
//
// -----------------------------------------------------------------------------
//
TChatType CCALoggerHeadersInterface::ChatType( TInt aIndex ) const
    {
    return LoggerMessageHeader( aIndex )->ChatType();
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::DeleteHistoryFileL
//
// -----------------------------------------------------------------------------
//
void CCALoggerHeadersInterface::DeleteHistoryFileL( TInt aIndex )
    {
    iLogger->DeleteHistoryFileL( *LoggerMessageHeader( aIndex ) );
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::GetMessageForSendingViaMTML
//
// -----------------------------------------------------------------------------
//
void CCALoggerHeadersInterface::GetMessageForSendingViaMTML( TInt aIndex,
                                                             RWriteStream& aStream )
    {
    RPointerArray< MCALoggerMessage > array;
    TBool showTimestamps = iSettingsAPI->Value( MCASettings::EShowTimeStamps );

    TCleanupItem cleanupItem( CleanupRPointerMCALoggerMessageArray, &array );
    CleanupStack::PushL( cleanupItem );

    iLogger->CreateLoggerMessagesL( array, *LoggerMessageHeader( aIndex ) );

    TInt arrayCount( array.Count() );
    for ( TInt a( 0 ); a < arrayCount; ++a )
        {
        // CALogger does not identify message classes in array. It is clients
        // responsibility to know type of message classes used in messagearray.
        // In this case, message array is filled by CCAMessageBase classes.
        static_cast< CCAMessageBase* >( array[ a ] )->
        ExternalizeForMTML( aStream,
                            showTimestamps );
        }

    CleanupStack::PopAndDestroy( 1 ); //cleanupItem
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::MessagesInterfaceL
//
// -----------------------------------------------------------------------------
//
MCAMessagesReadInterface& CCALoggerHeadersInterface::MessagesInterfaceL(
    TInt aIndex )
    {
    //Clear old one if exists
    RPointerArray< MCALoggerMessage > messages;
    CleanupStack::PushL( TCleanupItem( ResetAndDestroyMessages, &messages ) );

    iChatInterface.DeleteChatL( KNullDesC, KNullDesC, KLoggerBufferId );
    MCAMessagesWriteInterface& writeInterface =
        iChatInterface.MessageWriteInterfaceL( KNullDesC, KNullDesC,
                                               KLoggerBufferId );
    writeInterface.LockBufferMemoryHandling( ETrue );
    iLogger->CreateLoggerMessagesL( messages, *LoggerMessageHeader( aIndex ) );

    while ( messages.Count() )
        {
        CCAMessageBase* baseMessage =
            static_cast< CCAMessageBase* >( messages[ 0 ] );
        MCAMessage* message = baseMessage;
        messages.Remove( 0 );
        writeInterface.AppendL( message );
        }

    CleanupStack::PopAndDestroy(); // messages.ResetAndDestroy()

    return iChatInterface.MessageReadInterfaceL( KNullDesC, KNullDesC,
                                                 KLoggerBufferId );
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::DeleteRecordedChatContainerL
// -----------------------------------------------------------------------------
//
void CCALoggerHeadersInterface::DeleteRecordedChatContainerL()
    {
    iChatInterface.DeleteChatL( KNullDesC, KNullDesC, KLoggerBufferId );
    }

// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::Index
// -----------------------------------------------------------------------------
//
TInt CCALoggerHeadersInterface::Index( const TDesC& aName ) const
    {
    TInt headerCount = iHeaderArray->Count();

    for ( TInt i = 0; i < headerCount; i++ )
        {
        if ( aName.CompareC( Name( i ), KCollationLevel, NULL ) == 0 )
            {
            return i;
            }
        }

    return KErrNotFound;
    }

//fix - refer Ui Spec Approved Version 1.0
//(Instant Messaging NG 001 151006.pdf)
//Section 10.2.10 Pg 131 -
//"In case user has saved the image already or sent it
//by himself, this option(save) is not available."
// -----------------------------------------------------------------------------
// CCALoggerHeadersInterface::ReLoggingL
// -----------------------------------------------------------------------------
//
void CCALoggerHeadersInterface::ReLoggingL( TInt aIndex )
    {
    MCALoggerMessageHeader& msgHeader( *LoggerMessageHeader( aIndex ) );

    MCAMessagesReadInterface& readInterface =
        iChatInterface.MessageReadInterfaceL( KNullDesC, KNullDesC,
                                              KLoggerBufferId );

    readInterface.ReLoggingL( msgHeader );

    if ( readInterface.IsLoggingOn() )
        {
        readInterface.EndReLoggingL();
        }


    }
//  End of File
