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
* Description:  UI utils for message handling
*
*/



// INCLUDE FILES
#include    "CCAUiMessageUtils.h"
#include    "IMDialogUtils.h"
#include    "ChatDebugPrint.h"
#include    "ChatDefinitions.h"
#include    "CAArrayUtils.h"
#include    "imutils.h"

#include    "MCAConversationMessage.h"
#include    "mcablockchecker.h"


#include    <NewFileServiceClient.h>
#include    <AiwGenericParam.h>
#include    <chatNG.rsg>
#include    <MGFetch.h>
#include    <MMGFetchVerifier.h>
#include    <documenthandler.h>

#include    <eikenv.h>
#include    <imageconversion.h>

#include    "impsbuilddefinitions.h"
#include    "chatdebugassert.h"

#include	"MCAConversationPC.h"
#include	"IMMessageUtilsPC.h"
#include	<aknnavi.h>
#include	<aknappui.h>
// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

#ifdef  RD_IMAGEPREVIEW
#include    <imagepreview.h>


#endif  // RD_IMAGEPREVIEW

// CONSTANTS

_LIT8( KMimeTypeJpeg, "image/jpeg" );
_LIT8( KMimeTypeGif,  "image/gif" );
_LIT8( KMimeTypeEmpty, "" );
// From ICL documentation:
// "all decoder plugins also support thumbnail decodes with ratios of 1:2, 1:4 and 1:8."
const TInt KDecodeScaleRatioMin = 2;
const TInt KDecodeScaleRatioMax = 8;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAUiMessageUtils::CCAUiMessageUtils
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAUiMessageUtils::CCAUiMessageUtils( MCABlockChecker* aBlockChecker )
        : CActive( CActive::EPriorityLow ), iBlockChecker( aBlockChecker )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCAUiMessageUtils::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAUiMessageUtils::ConstructL()
    {
    User::LeaveIfError( iApaSession.Connect() );

    // create dochandler
    iDocHandler = CDocumentHandler::NewL();
    iDocHandler->SetExitObserver( this );
    }

// -----------------------------------------------------------------------------
// CCAUiMessageUtils::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAUiMessageUtils* CCAUiMessageUtils::NewLC( MCABlockChecker* aBlockChecker /*= NULL*/ )
    {
    CCAUiMessageUtils* self = new( ELeave ) CCAUiMessageUtils( aBlockChecker );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// Destructor
CCAUiMessageUtils::~CCAUiMessageUtils()
    {
    Cancel();

    delete iDecoder;
    delete iSelectedNames;

    delete iDocHandler;
    iApaSession.Close();
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

//-----------------------------------------------------------------------------
// CCAUiMessageUtils::RunL
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAUiMessageUtils::RunL()
    {
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

//-----------------------------------------------------------------------------
// CCAUiMessageUtils::DoCancel
// ( Other items commented in header )
//-----------------------------------------------------------------------------
void CCAUiMessageUtils::DoCancel()
    {
    iDecoder->Cancel();
    }

//-----------------------------------------------------------------------------
// CCAUiMessageUtils::RunError
// ( Other items commented in header )
//-----------------------------------------------------------------------------
TInt CCAUiMessageUtils::RunError( TInt aError )
    {
    if ( IsActive() )
        {
        iStatus = aError;
        Cancel();
        }

    if ( aError == KErrNoMemory )
        {
        CActiveScheduler::Current()->Error( KErrNoMemory );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCAUiMessageUtils::SendNewFileL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAUiMessageUtils::SendNewFileL( TNewServiceFileType aFileType,
                                      MCAConversationPC& aMessageRWInterfacePC,
                                      const TDesC& aSender /*= KNullDesC*/,
                                      const MDesCArray* aScreenNames /*= NULL*/,
                                      TBool aIsWhisperAllowed /*= ETrue*/ )
    {
    // show the list of recipients first
    if ( ! SelectRecipientsL( aScreenNames, aIsWhisperAllowed ) )
        {
        // cancelled
        return;
        }

    // only one file will be created
    CDesCArray* fileNames = new( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( fileNames );
    CNewFileServiceClient* newFile = NewFileServiceFactory::NewClientL();
    CleanupStack::PushL( newFile );

    CAiwGenericParamList* params = CAiwGenericParamList::NewLC();

//    CEikonEnv::Static()->SetSystem( ETrue );

    CHAT_DP_TXT( "Starting new file service.." );
    TBool okToSend = EFalse;
    TRAPD( err, okToSend =
               newFile->NewFileL( *fileNames, params, aFileType, EFalse ) );
    if ( err )
        {
//        // restore state
//        CEikonEnv::Static()->SetSystem( EFalse );
        User::LeaveIfError( err );
        }

    if ( okToSend )
        {
        // we got new file, let's send it
        CHAT_DP_TXT( "..got new file, sending it" );

        DoSendFileL( *fileNames, aMessageRWInterfacePC, aSender );

        }

    CleanupStack::PopAndDestroy( 3, fileNames ); // params, newFile, fileNames
    CHAT_DP_TXT( "Send new file done.." );
    }

// -----------------------------------------------------------------------------
// CCAUiMessageUtils::SendImageL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAUiMessageUtils::SendImageL(
    MCAConversationPC& aMessageRWInterfacePC,
    const TDesC& aSender /* = KNullDesC */,
    const MDesCArray* aScreenNames /* = NULL */,
    TBool aIsWhisperAllowed /*= ETrue*/ )
    {
    TBool ok( EFalse );
    // show the list of recipients first
    if ( ! SelectRecipientsL( aScreenNames, aIsWhisperAllowed ) )
        {
        // cancelled
        return;
        }
    while ( !ok )
        {
        // only one image can be selected at the moment
        CDesCArray* files = new( ELeave )CDesCArrayFlat( 1 );
        CleanupStack::PushL( files );

        HBufC* softkey = CCoeEnv::Static()->AllocReadResourceLC(
                             R_TEXT_SOFTKEY_SELECT );

        iBeginVerifySelection = EFalse;

        // This rd flag is for testing purposes only.
#ifndef RD_SEND_NOT_SUPPORTED_CONTENT
        // only one image can be selected (EFalse)
        if ( MGFetch::RunL( *files, EImageFile    , EFalse, *softkey, KNullDesC, this ) )
#else
        if ( MGFetch::RunL( *files, EAnyMediaFile, EFalse, *softkey, KNullDesC, this ) )
#endif //RD_SEND_NOT_SUPPORTED_CONTENT
            {
            // verify the selected image
            iBeginVerifySelection = ETrue;
            TBool verify = VerifySelectionL( files );

            if ( !verify )
                {
                CleanupStack::PopAndDestroy( softkey ); // softkey
                CleanupStack::PopAndDestroy( files );   // files
                continue;
                }

#ifdef RD_IMAGEPREVIEW
            //
            // Open image preview
            //
            if ( files->MdcaCount() == 1 ) // We can only open 1 image
                {
                HBufC* skSelect = CCoeEnv::Static()->AllocReadResourceLC(
                                      R_TEXT_SOFTKEY_SELECT );
                HBufC* skBack = CCoeEnv::Static()->AllocReadResourceLC(
                                    R_TEXT_SOFTKEY_BACK );

                this->SetNavigationPaneDimmed( ETrue );

                TInt res = ImagePreview::ShowImageL( files->MdcaPoint( 0 ),
                                                     *skSelect, *skBack );

                this->SetNavigationPaneDimmed( EFalse );
                CleanupStack::PopAndDestroy( 2 ); // skSelect, skBack
                ( res == EAknSoftkeyCancel ) ? ( ok = EFalse ) : ( ok = ETrue );
                }
            else
                {
                // Invalid number of images
                __CHAT_ASSERT_DEBUG( EFalse );
                }
#endif //RD_IMAGEPREVIEW

            if ( ok )
                {
                // send files
                DoSendFileL( *files, aMessageRWInterfacePC, aSender );

                }
            CleanupStack::PopAndDestroy( 2 ); // files, softkey

            }
        else
            {
            // user canceled
            CleanupStack::PopAndDestroy( 2 ); // files, softkey
            ok = ETrue;
            }
        } //end of while
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::SendFileL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUiMessageUtils::SendFileL(
    const MDesCArray& aFileNames,
    MCAConversationPC& aMessageRWInterfacePC,
    const TDesC& aSender /*= KNullDesC*/,
    const MDesCArray* aScreenNames /*= NULL*/ )
    {
    // show the list of recipients first
    if ( ! SelectRecipientsL( aScreenNames ) )
        {
        // cancelled
        return;
        }

    DoSendFileL( aFileNames, aMessageRWInterfacePC, aSender );

    }


// -----------------------------------------------------------------------------
// CCAUiMessageUtils::SendNewFileL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAUiMessageUtils::SendNewFileL( TNewServiceFileType aFileType,
                                      MCAGroupPC& aMessageRWInterfacePC,
                                      const TDesC& aSender /*= KNullDesC*/,
                                      const MDesCArray* aScreenNames /*= NULL*/,
                                      TBool aIsWhisperAllowed /*= ETrue*/ )
    {
    // show the list of recipients first
    if ( ! SelectRecipientsL( aScreenNames, aIsWhisperAllowed ) )
        {
        // cancelled
        return;
        }

    // only one file will be created
    CDesCArray* fileNames = new( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( fileNames );
    CNewFileServiceClient* newFile = NewFileServiceFactory::NewClientL();
    CleanupStack::PushL( newFile );

    CAiwGenericParamList* params = CAiwGenericParamList::NewLC();

    CHAT_DP_TXT( "Starting new file service.." );
    if ( newFile->NewFileL( *fileNames, params, aFileType, EFalse ) )
        {

        // we got new file, let's send it
        CHAT_DP_TXT( "..got new file, sending it" );

        DoSendFileL( *fileNames, aMessageRWInterfacePC, aSender );

        }

    CleanupStack::PopAndDestroy( 3, fileNames ); // params, newFile, fileNames
    CHAT_DP_TXT( "Send new file done.." );
    }

// -----------------------------------------------------------------------------
// CCAUiMessageUtils::SendImageL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAUiMessageUtils::SendImageL(
    MCAGroupPC& aMessageRWInterfacePC,
    const TDesC& aSender /* = KNullDesC */,
    const MDesCArray* aScreenNames /* = NULL */,
    TBool aIsWhisperAllowed /*= ETrue*/ )
    {
    TBool ok( EFalse );
    // show the list of recipients first
    if ( ! SelectRecipientsL( aScreenNames, aIsWhisperAllowed ) )
        {
        // cancelled
        return;
        }

    while ( !ok )
        {
        // only one image can be selected at the moment
        CDesCArray* files = new( ELeave )CDesCArrayFlat( 1 );
        CleanupStack::PushL( files );

        HBufC* softkey = CCoeEnv::Static()->AllocReadResourceLC(
                             R_TEXT_SOFTKEY_SELECT );

        iBeginVerifySelection = EFalse;

        // This rd flag is for testing purposes only.
#ifndef RD_SEND_NOT_SUPPORTED_CONTENT
        // only one image can be selected (EFalse)
        if ( MGFetch::RunL( *files, EImageFile, EFalse, *softkey, KNullDesC, this ) )
#else
        if ( MGFetch::RunL( *files, EAnyMediaFile, EFalse, *softkey, KNullDesC, this ) )
#endif //RD_SEND_NOT_SUPPORTED_CONTENT
            {
            // verify the selected image
            iBeginVerifySelection = ETrue;
            TBool verify = VerifySelectionL( files );

            if ( !verify )
                {
                CleanupStack::PopAndDestroy( softkey ); // softkey
                CleanupStack::PopAndDestroy( files );   // files
                continue;
                }

#ifdef RD_IMAGEPREVIEW
            //
            // Open image preview
            //
            if ( files->MdcaCount() == 1 ) // We can only open 1 image
                {
                HBufC* skSelect = CCoeEnv::Static()->AllocReadResourceLC(
                                      R_TEXT_SOFTKEY_SELECT );
                HBufC* skBack = CCoeEnv::Static()->AllocReadResourceLC(
                                    R_TEXT_SOFTKEY_BACK );

                this->SetNavigationPaneDimmed( ETrue );

                TInt res = ImagePreview::ShowImageL( files->MdcaPoint( 0 ),
                                                     *skSelect, *skBack );

                this->SetNavigationPaneDimmed( EFalse );

                CleanupStack::PopAndDestroy( 2 ); // skSelect, skBack
                ( res == EAknSoftkeyCancel ) ? ( ok = EFalse ) : ( ok = ETrue );
                }
            else
                {
                // Invalid number of images
                __CHAT_ASSERT_DEBUG( EFalse );
                }
#endif //RD_IMAGEPREVIEW

            if ( ok )
                {
                // send files
                DoSendFileL( *files, aMessageRWInterfacePC, aSender );

                }
            CleanupStack::PopAndDestroy( 2 ); // files, softkey

            }
        else
            {
            // user canceled
            CleanupStack::PopAndDestroy( 2 ); // files, softkey
            ok = ETrue;
            }
        } //end of while
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::SendFileL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUiMessageUtils::SendFileL(
    const MDesCArray& aFileNames,
    MCAGroupPC& aMessageRWInterfacePC,
    const TDesC& aSender /*= KNullDesC*/,
    const MDesCArray* aScreenNames /*= NULL*/ )
    {
    // show the list of recipients first
    if ( ! SelectRecipientsL( aScreenNames ) )
        {
        // cancelled
        return;
        }

    DoSendFileL( aFileNames, aMessageRWInterfacePC, aSender );

    }


// ---------------------------------------------------------
// CCAUiMessageUtils::SelectRecipientsL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAUiMessageUtils::SelectRecipientsL( const MDesCArray* aScreenNames,
                                            TBool aIsWhisperAllowed /*= ETrue*/ )
    {
    delete iSelectedNames;
    iSelectedNames = NULL;

    // check if we're sending a group message and whispering is allowed.
    // in that case, group members are in aScreenNames array

    const MDesCArray* screenNames = aScreenNames;

    if ( screenNames && aIsWhisperAllowed )
        {
        TInt count( screenNames->MdcaCount() );
        if ( count > 0 )
            {
            // insert "All" to top of the list
            HBufC* all = CCoeEnv::Static()->AllocReadResourceLC(
                             R_QTN_CHAT_SEND_FILE_TO_ALL );
            HBufC* title = CCoeEnv::Static()->AllocReadResourceLC(
                               R_QTN_CHAT_SEND_FILE_TO );

            // we have some screennames..
            CDesCArray* lst = CAArrayUtils::CloneArrayLC( *screenNames );
            lst->Sort();
            lst->InsertL( 0, *all );

            // show query
            iSelectedNames =
                IMDialogUtils::MultiselectionListQueryDialogL( *lst, *title );

            CleanupStack::PopAndDestroy( 2, title ); // lst, title
            if ( ! iSelectedNames )
                {
                // cancel
                CleanupStack::PopAndDestroy( all );
                return EFalse; // not accepted
                }

            if ( iSelectedNames->Count() > 0 )
                {
                TInt pos;
                if ( iSelectedNames->Find( *all, pos ) == 0 )
                    {
                    // "All" selected, send message with
                    // empty array of screennames
                    iSelectedNames->Reset();
                    }
                }
            CleanupStack::PopAndDestroy( all );
            }
        else
            {
            // possibly the user is in group by himself
            // => send to all.
            delete iSelectedNames;
            iSelectedNames = NULL;
            iSelectedNames = new( ELeave ) CDesCArrayFlat( 1 );
            iSelectedNames->Reset();
            }
        }
    else if ( !aIsWhisperAllowed )
        {
        // Whispering not allowed
        // Send message with empty array of screennames
        if ( !iSelectedNames )
            {
            iSelectedNames = new( ELeave ) CDesCArrayFlat( 1 );
            }
        iSelectedNames->Reset();
        }

    return ETrue; // was accepted
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::OpenObjectL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUiMessageUtils::OpenObjectL( MCAConversationMessage& aMsg )
    {
    CHAT_DP( D_CHAT_LIT( "*OpenObject: opening.." ) );
    TEnumsPC::TContentProcessState state = aMsg.ContentProcessState();

    switch ( state )
        {
        case TEnumsPC::EThumbnailReady:
            {
            if ( aMsg.MessagerType() != TEnumsPC::EMessageSent )
                {

                if ( aMsg.ContentData().Length() == 0 )
                    {
                    //In case there is only a thumbnail available in the buffer, because the real
                    //image file is deleted because of lack of memory
                    IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_MEM_LOW_IMAGE_OPEN );
                    return;
                    }
                else
                    {
                    if ( aMsg.IsImageSaved() )
                        {
                        //In case there is only a thumbnail available in the buffer, because user
                        //has saved the image
                        IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_MEM_LOW_IMAGE_OPEN_SAVED );
                        return;
                        }
                    }
                }
            else
                {
                //In case there is only a thumbnail available in the buffer, because user
                //sent it by himself
                IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_MEM_LOW_IMAGE_OPEN_SAVED );
                return;
                }

            break;
            }

        case TEnumsPC::EContentCorrupted:
            {
            // => corrupted
            IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_MEDIA_CORRUPTED );
            return;
            }

        case TEnumsPC::EContentNotSupported:
        case TEnumsPC::EContentNotSupportedDrm:
            {
            // => unsupported
            IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_MEDIA_UNSUPPORTED );
            return;
            }

        default:
            {
            break;
            }
        }


    // save to a temporary file
    HBufC* tempFileName = HBufC::NewMaxLC( KMaxPath );
    TPtr tempFileNamePtr( tempFileName->Des() );

    HBufC* fileName = NULL;

    // Check if there is filename defined in message
    const TDesC& origName = aMsg.Text();
    if ( origName.Length() > 0 )
        {
        TParse parser;
        User::LeaveIfError( parser.Set( origName, NULL, NULL ) );
        TPtrC ptr = parser.Name();
        fileName = ptr.AllocLC();
        tempFileNamePtr.SetLength( 0 );
        tempFileNamePtr.Insert( 0, *fileName );
        }
    else
        {
        fileName = CCoeEnv::Static()->AllocReadResourceLC(
                       R_QTN_CHAT_UNNAMED_FILE );
        }

    TPtr fileNamePtr( fileName->Des() );

    TDataType type( aMsg.MimeType() );
    RFile tempFile;
    CleanupClosePushL( tempFile );

    CHAT_DP( D_CHAT_LIT( "*OpenObject: creating temporary file %S" ),
             &fileNamePtr );
    iDocHandler->SaveTempFileL( aMsg.ContentData(), type, fileNamePtr,
                                tempFile );
    iDocHandler->GetPath( tempFileNamePtr );
    CHAT_DP( D_CHAT_LIT( "*OpenObject: created temporary file to %S" ),
             &tempFileNamePtr );

    // check rights
    if ( !IMUtils::FileProtectedL( tempFileNamePtr ) )
        {
        // Create parameters to allow IV do saving
        CAiwGenericParamList* params = CAiwGenericParamList::NewL();
        CleanupStack::PushL( params );
        TAiwGenericParam saveParam( EGenericParamAllowSave );
        params->AppendL( saveParam );
        // start the document
        TRAPD( err, iDocHandler->OpenFileEmbeddedL( tempFile, type, *params ) );
        if ( err == KErrNone )
            {
            // and wait for exit
            CHAT_DP( D_CHAT_LIT( "*OpenObject: launched OpenFileEmbeddedL,\
                        waiting for complete.. " ) );

            if ( ! iWait.IsStarted() )
                {
                iWait.Start();  // CSI: 10 # iWait is not an active object
                }
            }
        else
            {
            CHAT_DP( D_CHAT_LIT( "*OpenObject: OpenFileEmbeddedL left with %d " ),
                     err );
            CActiveScheduler::Current()->Error( err );
            }
        CleanupStack::PopAndDestroy( params );
        }
    else
        {
        // content is protected
        IMDialogUtils::DisplayInformationNoteL(
            R_QTN_DRM_NOT_ALLOWED );
        }

    // delete temporary file
    CleanupStack::PopAndDestroy( 2, fileName ); // tempFile.Close(), fileName
    CHAT_DP( D_CHAT_LIT( "*OpenObject: deleting %S" ), &tempFileNamePtr );
    RFs& fs = CCoeEnv::Static()->FsSession();
    fs.Delete( *tempFileName );

    CleanupStack::PopAndDestroy( tempFileName );
    CHAT_DP( D_CHAT_LIT( "*OpenObject: done" ) );
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::SaveObjectL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAUiMessageUtils::SaveObjectL( MCAConversationMessage& aMsg )
    {
    CHAT_DP( D_CHAT_LIT( "*SaveObject: saving.." ) );
    if ( aMsg.ContentData().Length() == 0 )
        {
        // No content
        return KErrGeneral;
        }

    // Check if there is filename defined in message
    HBufC* fileName = NULL;
    const TDesC& origName = aMsg.Text();

    if ( origName.Length() > 0 )
        {
        TParse parser;
        User::LeaveIfError( parser.Set( origName, NULL, NULL ) );
        TPtrC ptr = parser.Name();
        fileName = parser.Name().AllocLC();
        }
    else
        {
        fileName = HBufC::NewLC( 0 );
        }

    TDataType type( aMsg.MimeType() );
    RFile tempFile;
    CleanupClosePushL( tempFile );

    // First save to temp file
    iDocHandler->SaveTempFileL( aMsg.ContentData(), type, *fileName, tempFile );
    TFileName tempFileName;
    TInt err = KErrNone;
    tempFile.Name( tempFileName );
    iDocHandler->GetPath( tempFileName );

    // Copy temp file, asks user drive for copying and leaves if free space
    // on requested drive is below or will fall below critical level
    if ( origName.Length() > 0 )
        {
        err = iDocHandler->CopyL( tempFile, *fileName, type, KEntryAttNormal );
        }
    else
        {
        err = iDocHandler->CopyL( tempFile, KNullDesC, type, KEntryAttNormal );
        }

    //fix - refer Ui Spec Approved Version 1.0
    //(Instant Messaging NG 001 151006.pdf)
    //Section 10.2.10 Pg 131 -
    //"In case user has saved the image already or sent it
    //by himself, this option(save) is not available."
    if ( err == KErrNone )
        {
        aMsg.SetImageAsSaved( ETrue );
        }
    CleanupStack::PopAndDestroy( 2, fileName );  // tempFile.Close(), fileName

    // Delete temp file
    RFs& fs = CCoeEnv::Static()->FsSession();
    fs.Delete( tempFileName );

    return err;
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::HandleServerAppExit
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUiMessageUtils::HandleServerAppExit( TInt aReason )
    {
    iServerAppExitReason = aReason;
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::VerifySelectionL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAUiMessageUtils::VerifySelectionL( const MDesCArray* aSelectedFiles )
    {
    if ( !iBeginVerifySelection )
        {
        return ETrue;
        }

    TBool ok( EFalse );
    TBool protectedCount( 0 );
    TBool corruptedImage( EFalse );

#ifndef RD_SEND_NOT_SUPPORTED_CONTENT
    if ( aSelectedFiles )
        {
        TInt count( aSelectedFiles->MdcaCount() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            TUid dummyUid( KNullUid );
            TDataType dataType;
            TPtrC filename( aSelectedFiles->MdcaPoint( i ) );
            User::LeaveIfError( iApaSession.AppForDocument(
                                    filename,
                                    dummyUid, dataType ) );

            //Mime type is empty for non image files (e.g book marks)
            // so retrun false
            if ( dataType.Des8().CompareF( KMimeTypeEmpty ) == 0 )
                {
                return EFalse;
                }

            if ( dataType.Des8().CompareF( KMimeTypeJpeg ) == 0 ||
                 dataType.Des8().CompareF( KMimeTypeGif ) == 0 )
                {
                // selected object is gif or jpeg
                if ( !IMUtils::FileProtectedL( filename ) )
                    {
                    // if not protected => image is ok
                    ok = ETrue;
                    }
                else
                    {
                    ++protectedCount;
                    }
                }

            // try to open and decode picture to make sure it's
            // not corrupted.

            delete iDecoder;
            iDecoder = NULL;

            TInt params( CImageDecoder::EOptionNone
#ifndef RD_30_COMPATIBILITY_MODE
                         | CImageDecoder::EPreferFastDecode
#endif
                       );

            TRAPD( err, iDecoder = CImageDecoder::FileNewL(
                                       CEikonEnv::Static()->FsSession(), filename,
                                       static_cast<CImageDecoder::TOptions>( params ) ) );

            if ( err == KErrNone )
                {
                // file seems ok, try to decode image
                CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap();
                CleanupStack::PushL( bitmap );

                TFrameInfo frame =  iDecoder->FrameInfo();
                TSize decodeSize( DecodeSize( frame.iOverallSizeInPixels ) );

                TInt bitmaperr =
                    bitmap->Create( decodeSize, frame.iFrameDisplayMode );

                if ( bitmaperr )
                    {
                    CHAT_DP( D_CHAT_LIT( "CCAUiMessageUtils::VerifySelectionL:\
                            Unable to create bitmap (%d)" ), bitmaperr );
                    CActiveScheduler::Current()->Error( bitmaperr );
                    CleanupStack::PopAndDestroy( bitmap );
                    return EFalse;
                    }
                // start converting
                iDecoder->Convert( &iStatus, *bitmap );
                if ( !IsActive() )
                    {
                    SetActive();
                    }

                // and wait until it's finished
                if ( !iWait.IsStarted() )
                    {
                    iWait.Start();  // CSI: 10 # iWait is not an active object
                    }

                TInt statuscode = iStatus.Int();
                CleanupStack::PopAndDestroy( bitmap );

                if ( statuscode == KErrCorrupt )
                    {
                    CHAT_DP_TXT( "CCAUiMessageUtils::VerifySelectionL\
                            corrupted: decode failed" );
                    corruptedImage = ETrue;
                    }

                delete iDecoder;
                iDecoder = NULL;
                }
            else
                {
                // something wrong with opening file
                // -> assume corrupt
                corruptedImage = ETrue;
                }
            }
        }
    else
        {
        return ok;
        }
#else
    ok = ETrue;
#endif //RD_SEND_NOT_SUPPORTED_CONTENT

    if ( protectedCount > 0 )
        {
        // some of the files were protected
        IMDialogUtils::DisplayQueryDialogL(
            protectedCount == 1 ?
            R_QTN_DRM_INFO_SEND_FORBID_ONE :
            R_QTN_DRM_INFO_SEND_FORBID_SOME );
        }
    else if ( corruptedImage )
        {
        IMDialogUtils::DisplayQueryDialogL( R_QTN_MMS_UPLOAD_INFO_CORRUPTED );
        ok = EFalse; // don't allow selection of this image
        }
    else if ( !ok && aSelectedFiles->MdcaCount() != 0 )
        {
        // not ok, show note
        IMDialogUtils::DisplayQueryDialogL( R_QTN_MMS_INFO_OBJ_NOT_SUPP_SEND );
        }
    return ok;
    }


// ---------------------------------------------------------
// CCAUiMessageUtils::DecodeSize
// (other items were commented in a header).
// ---------------------------------------------------------
//
TSize CCAUiMessageUtils::DecodeSize( const TSize& aSize )
    {
    // 1:1 is always valid ratio for decode scaling
    TInt lastValidRatio( 1 );
    for ( TInt ratio( KDecodeScaleRatioMin ); ratio <= KDecodeScaleRatioMax; ratio <<= 1 )
        {
        if ( aSize.iWidth % ratio + aSize.iHeight % ratio == 0 )
            {
            // this ratio is valid
            lastValidRatio = ratio;
            }
        }

    // return the size scaled with correct ratio
    return TSize( aSize.iWidth / lastValidRatio,
                  aSize.iHeight / lastValidRatio );
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::ForwardContentMessageL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAUiMessageUtils::ForwardContentMessageL(
    const MCAConversationMessage& aMessage,
    const TDesC& aRecipient,
    const MDesCArray* aScreenNames,
    MCAConversationPC& aMessageRWInterfacePC,
    TBool aIsWhisperingAllowed,
    const TDesC& aSender /*= KNullDesC*/ )
    {
    // picture was forwarded
    if ( aMessage.ContentType() == TEnumsPC::EContentPicture )
        {
        // Check image state and DRM.
        // We should not get corrupted or DRM files at this point,
        // but it's better to make sure of it.
        TBool isProtected( IMUtils::ContentProtectedL( aMessage.ContentData() ) );

        TEnumsPC::TContentProcessState state =
            aMessage.ContentProcessState();
        if ( state == TEnumsPC::EContentCorrupted )
            {
            // => corrupted
            IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_MEDIA_CORRUPTED );
            // Message was handled as best as we could
            return ETrue;
            }
        else if ( state == TEnumsPC::EContentNotSupported ||
                  state == TEnumsPC::EContentNotSupportedDrm ||
                  isProtected )
            {
            // => unsupported
            IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_MEDIA_UNSUPPORTED );
            // Message was handled as best as we could
            return ETrue;
            }

        // Image seems to be ok, so let's continue forwarding.

        // show the list of recipients first
        if ( aScreenNames )
            {
            if ( !SelectRecipientsL( aScreenNames, aIsWhisperingAllowed ) )
                {
                // cancelled. Return true because it needs to seem that
                // we handled the message.
                return ETrue;
                }
            }
        else
            {
            // this has to be Null when sending a PtoP message.
            delete iSelectedNames;
            iSelectedNames = NULL;
            }

        IMMessageUtilsPC::SendMessageL(
            aMessage,
            aRecipient,
            aMessageRWInterfacePC,
            iSelectedNames,
            aSender ) ;

        // message handled succesfully
        return ETrue;
        }

    // We handle only pictures here, so message was not handled.
    return EFalse;
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::ForwardContentMessageL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAUiMessageUtils::ForwardContentMessageL(
    const MCAConversationMessage& aMessage,
    const TDesC& aRecipient,
    const MDesCArray* aScreenNames,
    MCAGroupPC& aMessageRWInterfacePC,
    TBool aIsWhisperingAllowed,
    const TDesC& aSender /*= KNullDesC*/ )
    {
    // picture was forwarded
    if ( aMessage.ContentType() == TEnumsPC::EContentPicture )
        {
        // Check image state and DRM.
        // We should not get corrupted or DRM files at this point,
        // but it's better to make sure of it.
        TBool isProtected( IMUtils::ContentProtectedL( aMessage.ContentData() ) );

        TEnumsPC::TContentProcessState state =
            aMessage.ContentProcessState();
        if ( state == TEnumsPC::EContentCorrupted )
            {
            // => corrupted
            IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_MEDIA_CORRUPTED );
            // Message was handled as best as we could
            return ETrue;
            }
        else if ( state == TEnumsPC::EContentNotSupported ||
                  state == TEnumsPC::EContentNotSupportedDrm ||
                  isProtected )
            {
            // => unsupported
            IMDialogUtils::DisplayInformationNoteL( R_QTN_CHAT_MEDIA_UNSUPPORTED );
            // Message was handled as best as we could
            return ETrue;
            }

        // Image seems to be ok, so let's continue forwarding.

        // show the list of recipients first
        if ( aScreenNames )
            {
            if ( !SelectRecipientsL( aScreenNames, aIsWhisperingAllowed ) )
                {
                // cancelled. Return true because it needs to seem that
                // we handled the message.
                return ETrue;
                }
            }
        else
            {
            // this has to be Null when sending a PtoP message.
            delete iSelectedNames;
            iSelectedNames = NULL;
            }

        IMMessageUtilsPC::SendMessageL(
            aMessage,
            aRecipient,
            aMessageRWInterfacePC,
            iSelectedNames,
            aSender ) ;

        // message handled succesfully
        return ETrue;
        }

    // We handle only pictures here, so message was not handled.
    return EFalse;
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::DoSendFileL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUiMessageUtils::DoSendFileL(
    const MDesCArray& aFileNames,
    MCAConversationPC& aMsgRWInterface,
    const TDesC& aSender /*= KNullDesC*/ )
    {

    TDataType dataType;
    TPtrC8 mimeType;
    TBool sendOption;

    // check file before it will be sent.
    if ( CheckFileL( aFileNames, dataType, mimeType, sendOption ) )
        {
        TPtrC fileName( aFileNames.MdcaPoint( 0 ) );
        IMMessageUtilsPC::SendFileL( aMsgRWInterface,
                                     iSelectedNames,
                                     sendOption,
                                     fileName,
                                     mimeType,
                                     KNullDesC8,
                                     aSender );
        }
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::DoSendFileL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUiMessageUtils::DoSendFileL(
    const MDesCArray& aFileNames,
    MCAGroupPC& aMsgRWInterface,
    const TDesC& aSender /*= KNullDesC*/ )
    {
    TDataType dataType;
    TPtrC8 mimeType;
    TBool sendOption;

    // check file before it will be sent.
    if ( CheckFileL( aFileNames, dataType, mimeType, sendOption ) )
        {
        TPtrC fileName( aFileNames.MdcaPoint( 0 ) );
        IMMessageUtilsPC::SendFileL( aMsgRWInterface,
                                     iSelectedNames,
                                     sendOption,
                                     fileName,
                                     mimeType,
                                     KNullDesC8,
                                     aSender );
        }
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::CheckFileL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAUiMessageUtils::CheckFileL( const MDesCArray& aFileNames, TDataType& aDataType, TPtrC8& aMimeType, TBool& aSendOption )
    {
    if ( aFileNames.MdcaCount() == 0 )
        return EFalse;

    // Check if recipient is blocked
    if ( iBlockChecker )
        {
        iBlockChecker->CheckBlockedL();
        }

    TPtrC fileName( aFileNames.MdcaPoint( 0 ) );
    TUid dummyUid( KNullUid );

#ifndef RD_SEND_NOT_SUPPORTED_CONTENT

    if ( IMUtils::FileProtectedL( fileName ) )
        return EFalse;

    // not drm-protected
    User::LeaveIfError( iApaSession.AppForDocument( fileName,
                                                    dummyUid,
                                                    aDataType ) );
    aMimeType.Set( aDataType.Des8() );
    aSendOption = EFalse;

    return ETrue;
#else

    TInt err( iApaSession.AppForDocument( fileName, dummyUid, aDataType ) );
    if ( err == KErrNone )
        aMimeType.Set( aDataType.Des8() );
    else
        aMimeType.Set( KMimeTypeJpeg );

    if ( !IMUtils::FileProtectedL( fileName ) )
        aSendOption = EFalse;
    else
        aSendOption = ETrue;

    return ETrue;
#endif //RD_SEND_NOT_SUPPORTED_CONTENT
    }

// ---------------------------------------------------------
// CCAUiMessageUtils::SetNaviPaneDimmed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAUiMessageUtils::SetNavigationPaneDimmed( TBool aDimmed )
    {
    CEikStatusPane* statusPane = CEikonEnv::Static()->AppUiFactory()->StatusPane();
    CCoeControl* control = statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
    CAknNavigationControlContainer* navipane = static_cast<CAknNavigationControlContainer*>( control );
    statusPane->SetDimmed( aDimmed );
    }

//  End of File
