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
* Description:  Interface for the Recorded chats UI to interact with the engine
*
*/


#include    "MCALoggerHeadersInterface.h"
#include    "CCARecordedChatsPC.h"
#include    "MCARecordedChatsArrayPC.h"
#include	"ChatDefinitions.h"
#include	"IMDialogUtils.h"
#include    "IMUtils.h"
#include    "CCAStorageManagerFactory.h"
#include 	"MCAStoredContacts.h"
#include    "MCAMessagesWriteInterface.h"
#include    "MCAStoredGroups.h"
#include    "MCAStoredGroup.h"
#include    "CCAServerContactsArrayPC.h"

#include	"MCALoggerMessageHeader.h"
#include	"MCAMessagesReadInterface.h"

#include    <bamdesca.h> // MDesCArray

#include    <avkon.rsg>
#include    <chatNG.rsg>

#include    <e32base.h>
#include    <StringLoader.h>
#include    <sendui.h>
#include    <s32mem.h>   //CBufStore
#include    <s32file.h>
#include    <txtrich.h>
#include    <avkon.hrh>
#include    <eikmenup.h>
#include	<aknutils.h>

#include    <senduiconsts.h>
#include    <CMessageData.h>
#include    <pathinfo.h>


#ifdef RD_MULTIPLE_DRIVE

#include "CCAProcessManagerFactory.h"
#include "MCAProcessManager.h"
#include "CCAEngine.h"

#endif
_LIT( KCAHistoryTempDirectory, ":\\system\\temp\\IMTemp\\" );

_LIT( KLineFeedFormat, "%c" );

/* list of forbidden characters which which are removed when
 * file name is created
 */
_LIT( KForbiddenCharacters, "\\/:*?\"<>|" );

/*
 * This is for log names which are not good for filename
 */
_LIT( KGeneralSendFilename, "Badlogname" );

_LIT( KNotepadFileExtension, ".txt" );

_LIT( KFileFooterFormat, " ( %d )" );


//CONSTANTS
const TInt KOneSelected = 1;
const TInt KFileFooterLength = 6;
const TInt KLengthOfPathEnd = 1;

const TInt KFileCount = 2;

//recommended minimimun amount of granularity for segmented buffer is 16
const TInt KSegmentGranularity = 16;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::CCARecordedChatsPC
// -----------------------------------------------------------------------------
//
CCARecordedChatsPC::CCARecordedChatsPC( MCALoggerHeadersInterface& aLoggerHeadersInterface,
                                        MCARecordedChatsArrayPC& aRecordedChatsArrayPC )
        : iLoggerHeadersInterface( aLoggerHeadersInterface ),
        iRecordedChatsArrayPC( aRecordedChatsArrayPC )
    {
    // since we can not appui fromave to pc
    // hence codescanner warning can be ignored
    iEikonEn = CEikonEnv::Static();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::~CCARecordedChatsPC
// -----------------------------------------------------------------------------
//
CCARecordedChatsPC::~CCARecordedChatsPC()
    {
    delete iSendAppUi;
    iItemsForDeletion.Close();
    delete iHistoryTempDir;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::NewL
// -----------------------------------------------------------------------------
//
CCARecordedChatsPC* CCARecordedChatsPC::NewL( MCALoggerHeadersInterface& aLoggerHeadersInterface,
                                              MCARecordedChatsArrayPC& aRecordedChatsArrayPC )
    {
    CCARecordedChatsPC* self = new ( ELeave ) CCARecordedChatsPC( aLoggerHeadersInterface,
                                                                  aRecordedChatsArrayPC );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
// -----------------------------------------------------------------------------
// CCARecordedChatsPC::ConstructL
// -----------------------------------------------------------------------------
//
void CCARecordedChatsPC::ConstructL()
    {

    //To store the history on a temp file in D: drive
    RFs &fileSession = iEikonEn->FsSession();
    TChar dDrive;
    User::LeaveIfError( fileSession.DriveToChar( EDriveD, dDrive ) );
    // +1 for dirve letter
    iHistoryTempDir = HBufC::NewL( 1 + KCAHistoryTempDirectory().Length() );
    TPtr ptr( iHistoryTempDir->Des() );
    ptr.Append( dDrive );
    ptr.Append( KCAHistoryTempDirectory );

    TFileName historyTempDir;
    historyTempDir.Copy( ( *iHistoryTempDir ).Left( historyTempDir.MaxLength() ) );

    //Ensure, that there is no file named IMHistory beforehand. Ignore errors
    fileSession.Delete( historyTempDir.Left(
                            historyTempDir.Length() - KLengthOfPathEnd ) );

    TInt createDirError( fileSession.MkDirAll( historyTempDir ) );
    if ( createDirError != KErrNone && createDirError != KErrAlreadyExists )
        {
        User::LeaveIfError( createDirError );
        }
    }



// -----------------------------------------------------------------------------
// CCARecordedChatsPC::DeleteRecordedChatContainerL
// -----------------------------------------------------------------------------
//
void CCARecordedChatsPC::DeleteRecordedChatContainerL()
    {
    iLoggerHeadersInterface.DeleteRecordedChatContainerL();
    }


// -----------------------------------------------------------------------------
// CCARecordedChatsPC::DeleteHistoryFilesL
// Delete history file. One or many.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsPC::DeleteHistoryFilesL( TInt aIndex, TBool aDeleteCurrentChat )
    {
    // This method is called from container, and container should initialize
    // iSelectedItems. If not initialized, leave with KErrArgument
    if ( !iSelectedItems )
        {
        User::Leave( KErrArgument );
        }

    TInt markedCount( iSelectedItems->Count() );
    iRecordedChatsArrayPC.SetProgressCount( 0 );

    InitDeleteArrayL();

    if ( markedCount == 0 ) //one item
        {
        // If flag is on for current chat deletion. Change index.
        // This can occur only when markedCount is 0, so check is not
        // needed in else branch
        if ( aDeleteCurrentChat )
            {
            aIndex = iRecordedChatsArrayPC.GetCurrentItemIndex();
            }
        else    //This is for keeping track of current item.
            {
            iRecordedChatsArrayPC.SetCurrentItemIndex( aIndex );
            }

        __ASSERT_ALWAYS( aIndex >= 0,
                         User::Panic( KPanicText, EAccessArrayOutOfBounds ) );

        __ASSERT_ALWAYS( aIndex <= iRecordedChatsArrayPC.MdcaCount(),
                         User::Panic( KPanicText, EAccessArrayOutOfBounds ) );

        iRecordedChatsArrayPC.SetNextIndex( aIndex );
        }
    else
        {
        TInt progressCount = iRecordedChatsArrayPC.GetProgressCount();
        if ( progressCount >= 0 && progressCount < iItemsForDeletion.Count() )
            {
            iRecordedChatsArrayPC.SetNextIndex( iItemsForDeletion[ progressCount ] );
            }
        }
    }

// -----------------------------------------------------------------------------
//  CCARecordedChatsPC::DeleteHistoryFileL
//  Delete one file and update index.
// -----------------------------------------------------------------------------
//
TBool CCARecordedChatsPC::DeleteHistoryFileL( )
    {
    // This method is called from container, and container should initialize
    // iSelectedItems. If not initialized, leave with KErrArgument
    if ( !iSelectedItems )
        {
        User::Leave( KErrArgument );
        }

    TInt curItemIndex = iRecordedChatsArrayPC.GetCurrentItemIndex();
    TInt nextIndex = iRecordedChatsArrayPC.GetNextIndex();
    //Keep track of current item index
    if ( (  curItemIndex  >  nextIndex ||
            curItemIndex == nextIndex &&
            curItemIndex == iLoggerHeadersInterface.HeaderCount() - 1 ) &&
         curItemIndex != 0 )
        {
        iRecordedChatsArrayPC.SetCurrentItemIndex( curItemIndex - 1 );
        }

    iLoggerHeadersInterface.DeleteHistoryFileL( iRecordedChatsArrayPC.GetNextIndex() );
    TInt progressCount = iRecordedChatsArrayPC.GetProgressCount();
    iRecordedChatsArrayPC.SetProgressCount( progressCount + 1 );
    progressCount = iRecordedChatsArrayPC.GetProgressCount();
    if ( iItemsForDeletion.Count() <= progressCount )
        {
        return EFalse;
        }

    iRecordedChatsArrayPC.SetNextIndex( iItemsForDeletion[ progressCount ] );
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::CheckAndCorrectMTMFilename
// Check and correct proposed filename for MTM sending purposes.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsPC::CheckAndCorrectMtmFilename( TFileName& aFilename,
                                                     MDesCArray& aArray )
    {
    TBool successFlag;

    // remove forbidden characters
    TInt len( aFilename.Length() );

    // Start from end of data
    for ( TInt i( len - 1 ); i >= 0; --i )
        {
        // now all the specials get removed
        TPtrC c = aFilename.Mid( i, 1 );  // next character
        TInt position = ( KForbiddenCharacters() ).FindC( c );

        if ( position != KErrNotFound )
            {
            // the current character is a FORBIDDEN CHARACTER,
            // remove it
            aFilename.Delete( i, 1 );
            }
        }

    // Sanity check. We removed characters, but still checking if there
    // is anything in the string
    if ( aFilename.Length() < 1 )
        {
        aFilename.Append( KGeneralSendFilename );
        }

    // Insert path in begin of file
    RFs &fileSession = iEikonEn->FsSession();

    TFileName tempFileName;
    tempFileName.Copy( ( *iHistoryTempDir ).Left( tempFileName.MaxLength() ) );
    tempFileName.Append( aFilename );

    aFilename = tempFileName;

    //start from two, because there is one with same name
    //filename -> filename ( 2 )
    TInt fileIndex( KFileCount );
    TInt arrayCount( aArray.MdcaCount() );
    do
        {
        successFlag = ETrue;
        for ( TInt b( 0 ); b < arrayCount && successFlag; ++b )
            {
            if ( tempFileName.CompareC( aArray.MdcaPoint( b ) ) == 0 )
                {
                TBuf< KFileFooterLength > filenameFooter;
                filenameFooter.Format( KFileFooterFormat, fileIndex++ );
                tempFileName = aFilename;
                tempFileName.Append( filenameFooter );
                successFlag = EFalse;
                }
            }
        }
    while ( !successFlag );

    aFilename = tempFileName;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::SelectedItemsCount
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsPC::SelectedItemsCount() const
    {
    return iSelectedItems->Count();
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::ValidSelectedItems
// -----------------------------------------------------------------------------
//
TBool CCARecordedChatsPC::ValidSelectedItems() const
    {
    if ( iSelectedItems )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::PrepareArrayForSendingMtmL
// Prepares MTM launch component, if not yet prepared.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsPC::PrepareArrayForSendingMtmL()
    {
    if ( !iSendAppUi )
        {
        //no meaning for command id for us in here so zero passed because
        //command id is mandatory parameter for CSendUi::NewL method.
        iSendAppUi = CSendUi::NewL();
        }
    }
// -----------------------------------------------------------------------------
// CCARecordedChatsPC::IsMarked
// Returns items marked status
// -----------------------------------------------------------------------------
//
TBool CCARecordedChatsPC::IsMarked( TInt aIndex )
    {
    __ASSERT_ALWAYS( aIndex >= 0,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );
    __ASSERT_ALWAYS( iRecordedChatsArrayPC.MdcaCount() > aIndex,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );

    if ( !iSelectedItems )
        {
        return EFalse;
        }

    TInt markedCount( iSelectedItems->Count() );
    for ( TInt a( 0 ); a < markedCount; ++a )
        {
        if ( aIndex == iSelectedItems->At( a ) )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::SetSelectedItems
// Initialize or uninitialize selected items. NULL means uninitialization.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsPC::SetSelectedItems( const CArrayFix< TInt >* aArray )
    {
    iSelectedItems = aArray;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::InitDeleteArrayL
// Init delete array, so we delete items in right order.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsPC::InitDeleteArrayL()
    {
    iItemsForDeletion.Reset();

    RArray< TInt > tempArray;
    CleanupClosePushL( tempArray );

    TInt itemCount = iSelectedItems->Count();
    TInt index;
    for ( index = 0; index < itemCount; ++index )
        {
        User::LeaveIfError( tempArray.Append( iSelectedItems->At( index ) ) );
        }
    tempArray.Sort();

    for ( index = itemCount - 1; index >= 0; --index )
        {
        if ( index >= 0 && index < tempArray.Count() ) //Bound checking
            {
            User::LeaveIfError( iItemsForDeletion.Append( tempArray[ index ] ) );
            }
        }
    //Codescanner warning ignored
    CleanupStack::PopAndDestroy(); // tempArray.Close()
    }


// -----------------------------------------------------------------------------
// CCARecordedChatsPC::SendMTML
// MTM editor launcher.
// Code Scanner warning to be ignored
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsPC::SendMTML( TInt aIndex, TUid aTargetUid /*= KNullUid*/ )
    {
    __ASSERT_ALWAYS( aIndex >= 0,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );
    __ASSERT_ALWAYS( iRecordedChatsArrayPC.MdcaCount() > aIndex,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );

    if ( !iSendAppUi )
        {
        // Leave after prepare. Prepare should have been called earlier.
        // For some reason iSendAppUi is not ready for sending MTM messages
        // if we try to use it right after creation. This is why leave is needed.
        PrepareArrayForSendingMtmL();
        User::Leave( KErrNotReady );
        }

    if ( aTargetUid == KNullUid )
        {
        // No target uid defined, show query

        // only one item in the array
        CArrayFix<TUid>* servicesToDim = new( ELeave )CArrayFixFlat<TUid>( 1 );
        CleanupStack::PushL( servicesToDim );
        servicesToDim->AppendL( KSenduiMtmAudioMessageUid );

        TRAPD( cancel, aTargetUid = iSendAppUi->ShowSendQueryL( NULL,
                                                                KCapabilitiesForAllServices, servicesToDim ) );
        CleanupStack::PopAndDestroy( servicesToDim );
        if ( cancel == KErrCancel || aTargetUid == KNullUid )
            {
            // user cancelled
            return KErrCancel;
            }
        User::LeaveIfError( cancel );
        }

    CDesCArray* attachments = NULL;
    TBool fileSendFlag( EFalse );

    TStreamId theId;

    //recommended minimimun amount of granularity for segmented buffer is 16
    CStreamStore* bufStore = CBufStore::NewLC( KSegmentGranularity );
    TInt maxMsgLength( IMUtils::MaxMsgLength() );

    RStoreWriteStream outStream;
    RStoreReadStream inStream;

    // this is done to get the Id for replacement in the for-loop
    theId = outStream.CreateLC( *bufStore );
    //Codescanner warning ignored
    CleanupStack::PopAndDestroy(); //outStream

    CRichText* text =
        CRichText::NewL( iEikonEn->SystemParaFormatLayerL(),
                         iEikonEn->SystemCharFormatLayerL() );
    CleanupStack::PushL( text );

    TInt markedCount;
    if ( iSelectedItems )
        {
        markedCount = iSelectedItems->Count();
        }
    else
        {
        markedCount = KOneSelected; // Delete from recorded chat view.
        }

    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );

#ifdef RD_UNIFIED_EDITOR
    // check sending capabilities
    TSendingCapabilities capa;
    iSendAppUi->ServiceCapabilitiesL( aTargetUid, capa );
    if ( capa.iFlags & TSendingCapabilities::ESupportsAttachments )
        {
        // send as attachments
        fileSendFlag = ETrue;
        attachments = new( ELeave ) CDesCArraySeg( 1 );
        CleanupStack::PushL( attachments );
        }
    else if ( ! ( capa.iFlags & TSendingCapabilities::ESupportsBodyText ) )
        {
        // doesn't support attachments nor body text, can't do anything
        User::Leave( KErrNotSupported );
        }

#else // no RD_UNIFIED_EDITOR

    //History data is send as attachments for bt, ir and mms, and as message to
    //sms
    TUid smsUid = { KSenduiMtmSmsUidValue };
    if ( aTargetUid != smsUid  )
        {
        fileSendFlag = ETrue;
        attachments = new( ELeave ) CDesCArraySeg( 1 );
        CleanupStack::PushL( attachments );
        }
#endif // RD_UNIFIED_EDITOR

    TInt arrayCount( Max( markedCount, KOneSelected ) );
    // Process all marked messages
    for ( TInt a( 0 ); a < arrayCount; ++a )
        {
        TInt index;
        if ( iSelectedItems && markedCount != 0 )
            {
            index = iSelectedItems->At( a );
            }
        else
            {
            index = aIndex; // Delete from recorded chat view
            }

        outStream.ReplaceLC( *bufStore, theId );
        GetMessageForSendingViaMTML( index, outStream );
        outStream.CommitL();

        inStream.OpenLC( *bufStore, theId );

        // First parameter is document position at which to insert the text.
        // Must be a valid position, or a panic occurs.
        // inStream is stream from which to read the text.
        // Third parameter are import parameters, including the foreign
        // encoding to convert from, whether to guess the foreign encoding
        // and the maximum number of characters to import.
        // EOrganiseByLine means when importing text from a text file,
        // a single line feed or a line feed followed by a carriage return is
        // converted into a space character. A line feed which is followed by
        // another line feed is converted into a
        // CEditableText::EParagraphDelimiter.
        text->ImportTextL( 0, inStream, CPlainText::EOrganiseByLine );

        if ( fileSendFlag )
            {

            TFileName filename;
            filename.Append( iLoggerHeadersInterface.Name( aIndex ) );
            filename.Append( KNotepadFileExtension );

            TPtr ptrDate( iRecordedChatsArrayPC.Time() );

            // This hardly fails. And if it for some reason fails,
            // it is not critical, because only thing what user notifies is
            // missing end time of chat
            iLoggerHeadersInterface.EndTime( aIndex ).FormatL( ptrDate,
                                                               iRecordedChatsArrayPC.GetDateFormat() );

            CheckAndCorrectMtmFilename( filename, *attachments );

            attachments->AppendL( filename );
            messageData->AppendAttachmentL( filename );

            TInt deleteError( iEikonEn->FsSession().Delete( filename ) );
            if ( deleteError != KErrNotFound )
                {
                User::LeaveIfError( deleteError );
                }

            text->ExportAsTextL( filename,
                                 CPlainText::EOrganiseByLine, maxMsgLength );
            }
        else
            {
            //launch MTM editor.
            messageData->SetBodyTextL( text );
            iSendAppUi->CreateAndSendMessageL( aTargetUid, messageData );
            }

        text->Reset();

        // outstream is destroyed with close, so it cannot be used
        // as parameter for popanddestroy. Codescanner warning ignored
        CleanupStack::PopAndDestroy( 2 ); //instream, outstream
        }

    if ( fileSendFlag )
        {
        // Send files as attachments.
        iSendAppUi->CreateAndSendMessageL( aTargetUid, messageData );
        arrayCount = attachments->Count();
        for ( TInt a ( 0 ); a < arrayCount; ++a )
            {
            User::LeaveIfError( iEikonEn->FsSession().Delete(
                                    attachments->MdcaPoint( a ) ) );
            }
        attachments->Reset();
        CleanupStack::PopAndDestroy( attachments );
        }

    //inStream, text, outstream, bufStore
    //Codescanner warning ignored.
    CleanupStack::PopAndDestroy( 3, bufStore ); //messagedata, text, bufStore

    //Return KErrNone as we are not interested
    //in any other error code except KErrCancel
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::StoredContactsCount
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsPC::StoredContactsCount() const
    {
    // contacts
    MCAStoredContacts* contacts = NULL;
    TRAPD( err, contacts = CCAStorageManagerFactory::ContactListInterfaceL() );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        return EFalse;
        }
    if ( !contacts )
        {
        return EFalse;
        }

    TInt numContacts( contacts->ContactCount() );

    return numContacts;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::ReadInterface
// -----------------------------------------------------------------------------
//
MCAMessagesReadInterface& CCARecordedChatsPC::ReadInterfaceL() const
    {
    return iLoggerHeadersInterface.MessagesInterfaceL( iRecordedChatsArrayPC.GetCurrentItemIndex() );
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::WriteInterface
// -----------------------------------------------------------------------------
//
MCAMessagesWriteInterface& CCARecordedChatsPC::WriteInterfaceL() const
    {
    MCAMessagesWriteInterface* tmpMessagesWriteInterface = NULL;
    return *tmpMessagesWriteInterface;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::GroupCountL
// Note: CodeScanner is likely to return false positives for this situation,
// because some returned TInt values will not be error codes.
// -----------------------------------------------------------------------------
//
TInt CCARecordedChatsPC::GroupCountL( TBool aGroupsSupported ) const
    {
    TInt numGroups( 0 );
    numGroups = CCAStorageManagerFactory::
                GroupListInterfaceL()->GroupCount( aGroupsSupported );
    return numGroups;
    }

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::GetPairArrayForServerContactsArrayLC
// -----------------------------------------------------------------------------
//
MCAServerContactsArrayPC* CCARecordedChatsPC::GetPairArrayForServerContactsArrayLC()
    {

    MCAStoredGroups* groups = CCAStorageManagerFactory::GroupListInterfaceL();
    RPointerArray<MCAStoredGroup> groupList;

    //Array of current group names
    CDesCArray* groupNames = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( groupNames );

    //Array of current group ids
    CDesCArray* groupIds = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( groupIds );

    CleanupClosePushL( groupList );
    groups->PopulateGroupsListL( groupList );


    TInt count( groupList.Count() );
    //For each item in the group list get the groupname and group id
    for ( TInt index( 0 ); index < count; index++ )
        {
        groupNames->AppendL( groupList[ index ]->GroupName() );
        groupIds->AppendL( groupList[ index ]->GroupId() );
        }
    //Codescanner warning ignored
    CleanupStack::PopAndDestroy(); // groupList.Close()

    /*We are only popping these two arrays from the clean up stack.
      But their ownership has been transferred to the CCAServerContactsArrayPC and
      it will be deleted in that class. Codescanner warning ignored*/
    CleanupStack::Pop( 2, groupNames ); //groupNames, groupids

    CCAServerContactsArrayPC* pairsArray =
        CCAServerContactsArrayPC::NewL( groupNames, groupIds, KNullDesC );



    CleanupStack::PushL( pairsArray ); //pairsArray

    //Not owned. Transfer ownership of pairsArray to the caller.
    //since we return as MCAServerContactsArrayPC object sliciing will happen
    //so its better to push the original data onto stack
    return pairsArray;
    }


// -----------------------------------------------------------------------------
// CCARecordedChatsPC::GetMessageForSendingViaMTML
// Get recorded chat messages for sending via MTM.
// -----------------------------------------------------------------------------
//
void CCARecordedChatsPC::GetMessageForSendingViaMTML( TInt aIndex,
                                                      RWriteStream& aStream )
    {
    TBuf<1> lineFeed;
    lineFeed.Format( KLineFeedFormat, CEditableText::ELineBreak );

    HBufC* identification = NULL;
    if ( iLoggerHeadersInterface.ChatType( aIndex ) == EPTOPChat )
        {
        identification =
            StringLoader::LoadLC( R_CHATCLIENT_HISTORY_IMESSAGES,
                                  iLoggerHeadersInterface.Identification( aIndex ) );
        }
    else
        {
        identification =
            StringLoader::LoadLC( R_CHATCLIENT_HISTORY_GROUP_NAME,
                                  iLoggerHeadersInterface.Identification( aIndex ) );
        }

    aStream.WriteL( *identification );
    CleanupStack::PopAndDestroy( identification );

    aStream.WriteL( lineFeed );

    TPtr ptrDate( iRecordedChatsArrayPC.Time() );

    // This hardly fails. And if it for some reason fails, it is not critical,
    // because only thing what user notifies is missing end time of chat
    iLoggerHeadersInterface.StartTime( aIndex ).FormatL( ptrDate,
                                                         iRecordedChatsArrayPC.GetStartDateFormat() );
    AknTextUtils::LanguageSpecificNumberConversion( ptrDate );


    // Write start information to the file
    aStream.WriteL( ptrDate );
    aStream.WriteL( lineFeed );

    iLoggerHeadersInterface.GetMessageForSendingViaMTML( aIndex, aStream );

    // Write end information to the file.
    iLoggerHeadersInterface.EndTime( aIndex ).FormatL( ptrDate,
                                                       iRecordedChatsArrayPC.GetEndDateFormat() );
    AknTextUtils::LanguageSpecificNumberConversion( ptrDate );

    aStream.WriteL( ptrDate );
    aStream.WriteL( lineFeed );
    }

//fix - refer Ui Spec Approved Version 1.0 (Instant Messaging NG 001 151006.pdf)
//Section 10.2.10 Pg 131 -
//"In case user has saved the image already or sent it by himself,
// this option(save) is not available."

// -----------------------------------------------------------------------------
// CCARecordedChatsPC::ReLoggingL
// -----------------------------------------------------------------------------
//
void CCARecordedChatsPC::ReLoggingL()
    {
    iLoggerHeadersInterface.ReLoggingL( iRecordedChatsArrayPC.GetCurrentItemIndex() );
    }


// -----------------------------------------------------------------------------
// CCARecordedChatsPC::ResetEngineLoggerL
// -----------------------------------------------------------------------------
//

void CCARecordedChatsPC::ResetEngineLoggerL()
    {
#ifdef RD_MULTIPLE_DRIVE
    MCAProcessManager* processManager =  CCAProcessManagerFactory::InstanceL()->GetProcessManager();
    CCAEngine&	engine ( *processManager->GetEngine() );
    engine.ResetLoggers();
#endif
    }



//  End of File


