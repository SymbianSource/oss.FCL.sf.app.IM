/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Logger manager with read access to history files.
*
*/


// INCLUDE FILES
#include <SysUtil.h>
#include <bautils.h>
#include <s32file.h>

#include "CCALoggerManager.h"
#include "CCALoggerWriter.h"
#include "MCALoggerMessageHeader.h"
#include "MCALoggerMessageFactory.h"
#include "MCALoggerMessage.h"

#ifdef RD_MULTIPLE_DRIVE
#include <centralrepository.h>
#include <IMPSServiceSettingsUINGInternalCRKeys.h>
#include <E32std.h>
#include <EIKAPP.H>
#include <eikappui.h>
#include <eikenv.h>
#include <eikbtgpc.h>
#include <driveinfo.h>
#include <StringLoader.h>            // StringLoader	
#include "IMDialogUtils.h"
#include <CAknMemorySelectionDialogMultiDrive.h>
#include <rsfwmountman.h>
#endif
#include <chatNG.rsg>

//for debug
#include "ChatDebugPrint.h"

//CONSTANTS
const TInt KLengthOfPathEnd = 1;
const TInt KArrayBegin = 0;

//Max length for number is 10
const TInt KMaxNumberLength = 10;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCALoggerManager::~CCALoggerManager
// destructor
// -----------------------------------------------------------------------------
//
CCALoggerManager::~CCALoggerManager()
    {
    CHAT_DP_TXT( "CCALoggerManager::~CCALoggerManager" );

    //Close filesession
    iFileSession.Close();

    //reset toc array
    iFileTocArray.ResetAndDestroy();

    //reset temp toc array
    iFileTempTocArray.ResetAndDestroy();

    //reset write array
    for ( TInt a = 0; a < iWriteFileArray.Count(); ++a )
        {
        //CCALoggerManager owns iFile in TFileRelation struct, but not the key
        delete iWriteFileArray[a].iFile;
        }

    iWriteFileArray.Close();
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::NewL
// Two phase constructor.
// -----------------------------------------------------------------------------
//
CCALoggerManager* CCALoggerManager::NewL()
    {
    CHAT_DP_TXT( "CCALoggerManager::NewL" );

    CCALoggerManager* lm = new ( ELeave ) CCALoggerManager();
    CleanupStack::PushL( lm );
    lm->ConstructL();
    User::LeaveIfError( Dll::SetTls( static_cast< TAny* >( lm ) ) );
    CleanupStack::Pop( lm );
    return lm;
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::CCALoggerManager
// Constructor
// -----------------------------------------------------------------------------
//
CCALoggerManager::CCALoggerManager()
    {
    CHAT_DP_TXT( "CCALoggerManager::CCALoggerManager" );
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::ConstructL
// Initialization of CCALoggerManager
// -----------------------------------------------------------------------------
//
void CCALoggerManager::ConstructL()
    {
    CHAT_DP_TXT( "CCALoggerManager::ConstructL" );

    User::LeaveIfError( iFileSession.Connect() );

    TBuf< KMaxPath > pathTemp;
    iFileSession.PrivatePath( pathTemp );
    pathTemp.Append( KCAHistoryDirectory );

    //Ensure, that there is no file named IMHistory beforehand. Ignore errors
    iFileSession.Delete( pathTemp.Left( pathTemp.Length() - KLengthOfPathEnd ) );

    //Create directories which are needed by IM Logger
    iFileSession.MkDirAll( pathTemp );

    iFileSession.PrivatePath( pathTemp );
    pathTemp.Append( KCAHistoryDataDirectory );

    //Ensure, that there is no file named IMHistory beforehand. Ignore errors
    iFileSession.Delete( pathTemp.Left( pathTemp.Length() - KLengthOfPathEnd ) );

    //Create directories which are needed by IM Logger
    iFileSession.MkDirAll( pathTemp );
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::InstanceL
// Singleton instance of CCALoggerManager
// -----------------------------------------------------------------------------
//
CCALoggerManager* CCALoggerManager::InstanceL(
    MCALoggerMessageFactory* aMessageFactory,
    TBool aCreate , TBool aFirstTime )
    {
    CHAT_DP_TXT( "CCALoggerManager::InstanceL" );

    CCALoggerManager* lm = static_cast< CCALoggerManager* > ( Dll::Tls() );

    //If instance of CCALoggerManager is not created yet. Create it.
    if ( ! lm )
        {
        if ( !aCreate )
            {
            return NULL;
            }
        lm = NewL();
        }

    //If aMessageFactory is passed to instance, change it.

    if ( aMessageFactory )
        {
        lm->SetLoggerMessageFactory( aMessageFactory );
        }

    //if toc file is not yet read, try to read it.
    if ( ( ! lm->iTocFileRead ) )
        {
        // here it's application booting up time..then don't show the error note..
        // check whether this call during application boot up or not....

        lm->ReadTocFileL( aFirstTime ) ;// this first time, we are reading..so we can ignore errors while reading...
        }

    return lm;
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::SuitableLogFileNameL
// Get first possible filename (number)
// -----------------------------------------------------------------------------
//
void CCALoggerManager::SuitableLogFileNameL()
    {
    CHAT_DP_TXT( "CCALoggerManager::SuitableLogFileNameL" );

    //If suitable log filename is already checked just use it.
    if ( iWeHaveLatestFileNameNumber )
        {
        iLatestFileNameNumber++;
        }
    else	//Check next suitable log filename
        {
        //Sort does not help us, because numbers cannot be sorted like letters.
        //10 is before 2 and so on. So we have to go through all files to get
        //right ones.

        // Get all the files in the directory
        // These are named as 1, 2, 5 i.e. numbers
        CDir* directoryFiles;

        //TODOMD: get the correct path name here...

        TBuf< KMaxPath > pathTemp;

#ifndef RD_MULTIPLE_DRIVE
        iFileSession.PrivatePath( pathTemp );
#else
        TBuf< KMaxPath > defaultDrive;
        TBuf< KMaxPath > pathTempWithoutDrive;
        TBuf< KMaxPath > driveAndPath;

        CRepository* cenrep = CRepository::NewL( KWVSettingsCenRepUid );
        TInt err = KErrNone;
        TRAP( err,
              CleanupStack::PushL( cenrep );
              err = cenrep->Get( KIMPSCRIMDefaultMemoryDrive, defaultDrive );
              CleanupStack::PopAndDestroy( cenrep );
            ); // TRAP
        cenrep = NULL;

        driveAndPath.Append( defaultDrive.Left( 2 ) ); 		// append the drive root here
        iFileSession.PrivatePath( pathTempWithoutDrive ); // copy the private path info here..
        driveAndPath.Append( pathTempWithoutDrive ); 	// append the private path to root folder...
        pathTemp.Copy( driveAndPath );	    				// copy this new path into tocfile...

#endif

        pathTemp.Append( KCAHistoryDataDirectory );

        User::LeaveIfError( iFileSession.GetDir(	pathTemp,
                                                 KEntryAttNormal,
                                                 ESortNone,
                                                 directoryFiles ) );

        //If directoryFiles success and is files, last will be noticed and
        //next free number chosen. If there is not files,
        //zero is first one to go.
        CleanupStack::PushL( directoryFiles );
        TLex lexer;
        TInt latestNumber;
        for ( TInt a = 0; a < directoryFiles->Count(); ++a )
            {
            lexer.Assign( ( *directoryFiles )[ a ].iName );
            if ( lexer.Val( latestNumber ) == KErrNone )
                {
                if ( latestNumber >= iLatestFileNameNumber )
                    {
                    iLatestFileNameNumber = latestNumber + 1;
                    }
                }
            }

        CleanupStack::PopAndDestroy( directoryFiles );
        iWeHaveLatestFileNameNumber = ETrue;
        }
    }
#ifdef RD_MULTIPLE_DRIVE

// ---------------------------------------------------------------------------
// CCALoggerManager::DriveStatus
// ---------------------------------------------------------------------------
//

TBool CCALoggerManager::GetDriveStatusL( const TDriveNumber aDriveNumber )
    {


    RFs& fs = CCoeEnv::Static()->FsSession();
    _LIT( KFat, "Fat" );

    // Check if the drive is already mounted
    TFullName fsName;
    TInt error( fs.FileSystemName( fsName, aDriveNumber ) );
    if ( error )
        {
        return EFalse;
        }

    // check if MMC already mounted
    if ( fsName.Length() == 0 )
        {
        // MMC drive isnt mounted at present, so try it now....
        error = fs.MountFileSystem( KFat, aDriveNumber );

        // If it's a locked MMC and the password is already known it'll be
        // unlocked automatically when it's mounted., otherwise the mount will
        // return with KErrLocked.....
        switch ( error )
            {
            case KErrNone:
            case KErrLocked:
                {
                break;
                }
            default:
                {
                return EFalse;
                }
            }
        }
    TDriveInfo driveInfo;
    error = fs.Drive( driveInfo, aDriveNumber );
    if ( error )
        {
        return EFalse;
        }

    // MMC is in slot
    if ( driveInfo.iMediaAtt & KMediaAttLocked )
        {
        return EFalse;
        }

    TVolumeInfo volumeInfo;
    error = fs.Volume( volumeInfo, aDriveNumber );
    if ( error )
        {
        return EFalse;
        }

    // If type is remote drive and aConnectionState is required
    if ( driveInfo.iDriveAtt & KDriveAttRemote )
        {
        TChar driveLetter;
        fs.DriveToChar( aDriveNumber, driveLetter );
        // This statement migth cause leave.. to be solved
        CRsfwMountMan* mountMgr = CRsfwMountMan::NewL( 0, NULL );
        TRsfwMountInfo mountInfo;
        error = mountMgr->GetMountInfo( driveLetter, mountInfo );
        delete mountMgr;

        if ( error )
            {
            return EFalse;
            }
        }
    return ETrue;
    }

#endif
// -----------------------------------------------------------------------------
// CCALoggerManager::ReadTocFile
// Read toc file for headers.
// -----------------------------------------------------------------------------
//
void CCALoggerManager::ReadTocFileL( TBool aNotFirstTime /*ETrue*/ )
    {
    CHAT_DP_TXT( "CCALoggerManager::ReadTocFileL" );

    TBuf< KMaxPath > tocFile;
    TBuf< KMaxPath > tempTocFile;
#ifndef RD_MULTIPLE_DRIVE
    iFileSession.PrivatePath( tocFile );
#else
    TBuf< KMaxPath > defaultDrive;
    TBuf< KMaxPath > pathTempWithoutDrive;
    TBuf< KMaxPath > driveAndPath;

    CRepository* cenrep = NULL;
    TRAPD( err, cenrep = CRepository::NewL( KWVSettingsCenRepUid ) );

    if ( err != KErrNone )
        {
        // creation of cenrep failed -> use default path to save the conversation
        iFileSession.PrivatePath( driveAndPath );
        }
    else
        {

        TRAP( err,
              CleanupStack::PushL( cenrep );
              err = cenrep->Get( KIMPSCRIMDefaultMemoryDrive, defaultDrive );
              CleanupStack::PopAndDestroy( cenrep );
            ); // TRAP
        cenrep = NULL;

        ///////////// end of reading from cenrep

        TInt driveNo = 0;
        err = RFs::CharToDrive( defaultDrive[0], driveNo );
        TDriveNumber driveNumber = TDriveNumber( driveNo );


        // Check drive's media status
        TBool isDriveOk = GetDriveStatusL( driveNumber );
        if ( !isDriveOk )// drive is not ready yet.....
            {
            if ( !aNotFirstTime )
                {
                HBufC* prompt = StringLoader::LoadLC( R_CHAT_SAVED_NOT_AVAILABLE );
                IMDialogUtils::DisplayErrorNoteL( *prompt );
                CleanupStack::PopAndDestroy( prompt ); // prompt
                }
            return; // return from here as there is an
            }

        /////// start atppend this info to complete path ////////////////////////////////

        driveAndPath.Append( defaultDrive.Left( 2 ) ); 		// append the drive root here
        iFileSession.PrivatePath( pathTempWithoutDrive ); // copy the private path info here..
        driveAndPath.Append( pathTempWithoutDrive ); 	// append the private path to root folder...
        tocFile.Copy( driveAndPath );	    				// copy this new path into tocfile...
        }
#endif

    tempTocFile.Copy( tocFile );
    tocFile.Append( KCAHistoryTocFile );
    tempTocFile.Append( KCAHistoryTocTempFile );

    //Cannot create headers without factory
    //This is not error. Just return to caller.
    if ( ! iMessageFactory )
        {
        return;
        }
#ifdef RD_MULTIPLE_DRIVE

    PopulateTocArrayL( iFileTocArray, tocFile, aNotFirstTime );
    PopulateTocArrayL( iFileTempTocArray, tempTocFile, aNotFirstTime ); // we no need to show the error dialog again.

#else
    PopulateTocArrayL( iFileTocArray, tocFile );
    PopulateTocArrayL( iFileTempTocArray, tempTocFile ); // we no need to show the error dialog again.


#endif
    TBool onlyTempTocFlag( EFalse );
    if ( iFileTocArray.Count() + iFileTempTocArray.Count() == 0 )
        {
        iTocFileRead = ETrue;
        return;
        }
    else if ( iFileTocArray.Count() == 0 )
        {
        onlyTempTocFlag = ETrue;
        }

    // Merge temp toc to real toc if needed.
    TInt arrayCount( iFileTempTocArray.Count() );
    for ( TInt b( arrayCount - 1 ); b >= 0; --b )
        {
        // Check if real toc have header temp is offering.
        TInt arrayCount2( iFileTocArray.Count() );
        TBool newOne( ETrue );
        for ( TInt a( 0 ); a < arrayCount2 && newOne; ++a )
            {
            if ( iFileTempTocArray[ b ]->FilenameL().CompareC(
                     iFileTocArray[ a ]->FilenameL() ) == 0 )
                {
                newOne = EFalse;
                }
            }
        // If header is newOne, then add it to real toc.
        if ( newOne )
            {
            User::LeaveIfError( iFileTocArray.Insert( iFileTempTocArray[ b ],
                                                      KArrayBegin ) );
            iFileTempTocArray.Remove( b );
            }
        }

    //Next save the toc file without those headers which are not valid anymore.
    //This is easily done, because only valid headers were appended to array.
    //Just make sure, that old header information does not lose in anycase.
    if ( !onlyTempTocFlag )
        {
        iFileSession.Delete( tempTocFile );
        User::LeaveIfError( iFileSession.Rename( tocFile, tempTocFile ) );
        }

    RFileWriteStream fileWriteStream;
    TInt error( fileWriteStream.Create(	iFileSession,
                                        tocFile,
                                        EFileWrite ) );
    if ( error != KErrNone )
        {
        iFileSession.Delete( tocFile );
        User::LeaveIfError( iFileSession.Rename( tempTocFile, tocFile ) );
        User::Leave( error );
        }
    CleanupClosePushL( fileWriteStream );

    //Write headers and if file write fails trap leave because we want to
    //change old file back.
    // Write the array from end to start order to the file to keep
    // order correct (from oldest to newest).
    TInt maxIndex = iFileTocArray.Count() - 1;
    for ( TInt a = maxIndex; a >= 0; --a )
        {
        if ( CheckMemoryL( iFileTocArray[a]->MessageSizeInBytesL() ) )
            {
            TRAPD( err, iFileTocArray[a]->ExternalizeL( fileWriteStream ) );
            if ( err != KErrNone )
                {
				CleanupStack::PopAndDestroy( 1 ); // fileWriteStream
                iFileSession.Delete( tocFile );
                User::LeaveIfError( iFileSession.Rename( tempTocFile, tocFile ) );
                User::Leave( err );
                }
            }
        else
            {
			CleanupStack::PopAndDestroy( 1 ); // fileWriteStream
            iFileSession.Delete( tocFile );
            User::LeaveIfError( iFileSession.Rename( tempTocFile, tocFile ) );
            User::Leave( KErrDiskFull );
            }
        }

    CHAT_DP( D_CHAT_LIT( " ignore %d" ), aNotFirstTime );

    CleanupStack::PopAndDestroy( 1 ); // fileWriteStream

    //Delete old file.
    iFileSession.Delete( tempTocFile );

    //Now toc file is read.
    iTocFileRead = ETrue;
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::WriteTocFileL
//
// -----------------------------------------------------------------------------
//
void CCALoggerManager::WriteTocFileL( MCALoggerMessageHeader& aHeader,
                                      TBool aTemporary /*= EFalse*/ )
    {
    // incase of multidrive, write the header into the user selected drive itself..
    CHAT_DP_TXT( "CCALoggerManager::WriteTocFileL" );

    TBuf< KMaxPath > tocFile;
    TBuf< KMaxPath > tempTocFile;

#ifndef RD_MULTIPLE_DRIVE
    iFileSession.PrivatePath( tocFile );
#else
    TBuf< KMaxPath > defaultDrive;
    TBuf< KMaxPath > pathTempWithoutDrive;
    TBuf< KMaxPath > driveAndPath;

    CRepository* cenrep = NULL;
    TRAPD( err, cenrep = CRepository::NewL( KWVSettingsCenRepUid ) );

    if ( err != KErrNone )
        {
        // creation of cenrep failed -> use default path to save the conversation
        iFileSession.PrivatePath( driveAndPath );
        }
    else
        {

        TRAP( err,
              CleanupStack::PushL( cenrep );
              err = cenrep->Get( KIMPSCRIMDefaultMemoryDrive, defaultDrive );
              CleanupStack::PopAndDestroy( cenrep );
            ); // TRAP
        cenrep = NULL;

        /////// start atppend this info to complete path ////////////////////////////////
        driveAndPath.Append( defaultDrive.Left( 2 ) ); // append the drive root here
        //check whether this is in mmc card and card was protected with pwd or not
        iFileSession.PrivatePath( pathTempWithoutDrive ); // copy the private path info here..
        driveAndPath.Append( pathTempWithoutDrive ); // append the private path here...
        }
    // copy this new path into tocfile...
    tocFile.Copy( driveAndPath );

#endif
    tempTocFile.Copy( tocFile );
    tocFile.Append( KCAHistoryTocFile );
    tempTocFile.Append( KCAHistoryTocTempFile );

    RFileWriteStream fileStream;
    TInt error = fileStream.Open( iFileSession, !aTemporary ? tocFile :
                                  tempTocFile, EFileWrite );

    //if file does not exist.. create it
    if ( error == KErrNotFound )
        {
        User::LeaveIfError( fileStream.Create( iFileSession, !aTemporary ?
                                               tocFile : tempTocFile, EFileWrite ) );
        }
    else
        {
        User::LeaveIfError( error );
        }

    CleanupClosePushL( fileStream );

    //Seek end of file.
    fileStream.Sink()->SeekL( RFileBuf::EWrite, EStreamEnd );

    if ( CheckMemoryL( aHeader.MessageSizeInBytesL() ) )
        {
        aHeader.ExternalizeL( fileStream );
        }
    else
        {
        User::Leave( KErrDiskFull );
        }

    CleanupStack::PopAndDestroy( 1 ); // fileStream
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::ReadMessageFileL
// Read messages to array
// -----------------------------------------------------------------------------
//
void CCALoggerManager::ReadMessageFileL(
    RPointerArray< MCALoggerMessage >& aMessageArray,
    MCALoggerMessageHeader& aKey )
    {
    CHAT_DP_TXT( "CCALoggerManager::ReadMessageFileL" );

    if ( ! iMessageFactory )
        {
        User::Leave( KErrNotReady );
        }

    RFileReadStream fileStream;
    User::LeaveIfError( fileStream.Open(	iFileSession,
                                         aKey.FilenameL(),
                                         EFileRead ) );
    CleanupClosePushL( fileStream );

    MCALoggerMessage* newMessage;

    //Go through whole stream.
    while ( fileStream.Source()->SizeL() !=
            fileStream.Source()->TellL( RFileBuf::ERead ).Offset() )
        {
        newMessage = iMessageFactory->CreateLoggerMessageL( fileStream );
        CleanupDeletePushL( newMessage );
        newMessage->InternalizeL( fileStream );
        User::LeaveIfError( aMessageArray.Append( newMessage ) );
        CleanupStack::Pop( newMessage );
        }

    CleanupStack::PopAndDestroy( 1 );	// fileStream
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::ReadInstanceL
// Return pointer to read interface
// -----------------------------------------------------------------------------
//
MCALoggerReadInterface* CCALoggerManager::ReadInstanceL(
    MCALoggerMessageFactory* aMessageFactory, TBool aFirstTime )
    {
    CHAT_DP_TXT( "CCALoggerManager::ReadInstanceL" );

    CCALoggerManager* lm = InstanceL( aMessageFactory , ETrue, aFirstTime );

    return lm;
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::WriteInstanceL
// Create write interface and return it.
// -----------------------------------------------------------------------------
//
MCALoggerWriteInterface* CCALoggerManager::WriteInstanceL(
    MCALoggerMessageFactory* aMessageFactory )
    {
    CHAT_DP_TXT( "CCALoggerManager::WriteInstanceL" );

    CCALoggerManager* lm = InstanceL( aMessageFactory );

    return CCALoggerWriter::NewL( *lm, lm->iFileSession );
    }


// -----------------------------------------------------------------------------
// CCALoggerManager::ReleaseInstanceL
// Create write interface and return it.
// -----------------------------------------------------------------------------
//
void CCALoggerManager::ReleaseInstanceL()
    {
    CHAT_DP_TXT( "CCALoggerManager::ReleaseInstanceL" );

    CCALoggerManager* lm = InstanceL( NULL );
    delete lm;
    lm = NULL;
    Dll::SetTls( NULL );
    }


// ================= INHERITED FUNCTIONS =======================

//	FROM MCALoggerWriteObserver

// -----------------------------------------------------------------------------
// CCALoggerManager::AddFileL
// Add new file to write array
// -----------------------------------------------------------------------------
//
void CCALoggerManager::AddFileL( TFileRelation aFile )
    {
    CHAT_DP_TXT( "CCALoggerManager::AddFile" );

    CleanupDeletePushL( aFile.iFile );
    //Removes old file to this writer from iWriteFileArray if exists.
    //This has been done in iWriter, but after this we can be sure of it.
    RemoveFileL( *aFile.iWriter );

    CleanupStack::Pop( aFile.iFile );
    TInt err = iWriteFileArray.Append( aFile );
    if ( err != KErrNone )
        {
        delete aFile.iFile;
        User::Leave( err );
        }

    //Write header information to temporary toc file.
    WriteTocFileL( *aFile.iFile, ETrue );
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::RemoveFileL
// Remove used file from write array and add it to toc.
// -----------------------------------------------------------------------------
//
void CCALoggerManager::RemoveFileL( MCALoggerWriteInterface& aWriter )
    {
    CHAT_DP_TXT( "CCALoggerManager::RemoveFileL" );

    for ( TInt a = 0; a < iWriteFileArray.Count(); ++a )
        {
        if ( iWriteFileArray[ a ].iWriter == &aWriter )
            {
            TFileRelation relation( iWriteFileArray[ a ] );

            //Recorded chat version will add file to toc array.
            User::LeaveIfError( iFileTocArray.Insert( relation.iFile,
                                                      KArrayBegin ) );

            //Both versions will remove file from write array.
            iWriteFileArray.Remove( a );

            //Trigger footer information for history header
            relation.iFile->EndLogging();

            //Write header information to toc file.
            WriteTocFileL( *relation.iFile );
            return;
            }
        }
    }


void CCALoggerManager::LeaveIfDiskFull( TInt aErrorCode )
    {
    if ( aErrorCode == KErrDiskFull )
        {
        User::LeaveIfError( KErrDiskFull );
        }
    }



#ifdef RD_MULTIPLE_DRIVE
// -----------------------------------------------------------------------------
// CCALoggerManager::CreateNewFilenameL
// New actual filename for new history file.
// -----------------------------------------------------------------------------
//
HBufC* CCALoggerManager::CreateNewFilenameMDL()
    {
    CHAT_DP_TXT( "CCALoggerManager::CreateNewFilenameMDL" );

    ///////// read settings from cenrep ///////////////

    TBuf< KMaxPath > defaultDrive;
    TBuf< KMaxPath > pathTempWithoutDrive;
    TBuf< KMaxPath > driveAndPath;

    CRepository* cenrep = NULL;
    TRAPD( err, cenrep = CRepository::NewL( KWVSettingsCenRepUid ) );

    if ( err == KErrNone )
        {
        // creation of cenrep OK
        CleanupStack::PushL( cenrep );
        err = cenrep->Get( KIMPSCRIMDefaultMemoryDrive, defaultDrive );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy( cenrep );
            cenrep = NULL;
            User::Leave( err );
            }///////////// end of reading from cenrep

        // we need to check whether do we have access rights of this root drive or not..
        // 1. Check whether this is write protected or not

        //2. check whether this is in mmc card and card was protected with pwd or not

        TInt driveNo = 0;
        err = RFs::CharToDrive( defaultDrive[0], driveNo );
        TDriveNumber driveNumber = TDriveNumber( driveNo );

        driveAndPath.Append( defaultDrive.Left( 2 ) ); // append the drive root here
        CleanupStack::PopAndDestroy( cenrep );
        cenrep = NULL;

        // added LeaveIfDiskFull() function call
        LeaveIfDiskFull( iFileSession.CreatePrivatePath( driveNumber ) ); // create the private default drive info here...

        iFileSession.PrivatePath( pathTempWithoutDrive ); // copy the private path info here..

        driveAndPath.Append( pathTempWithoutDrive ); // append the private path here...
        driveAndPath.Append( KCAHistoryDirectory );// apend imhistory/data folder here...

        // now check whether this path was created or not.
        // if not create the new path now.....

        // Ensure, that there is no file named IMHistory beforehand.
        iFileSession.Delete( driveAndPath.Left( driveAndPath.Length() - KLengthOfPathEnd ) );

        // Create directories which are needed by IM Logger
        // added LeaveIfDiskFull() function call
        LeaveIfDiskFull( iFileSession.MkDirAll( driveAndPath ) ); // create IMHistory directory here...
        driveAndPath.Append( KCADataDirectory );

        // Ensure, that there is no file named IMHistory beforehand.
        iFileSession.Delete( driveAndPath.Left( driveAndPath.Length() - KLengthOfPathEnd ) );

        // Create directories which are needed by IM Logger
        // added LeaveIfDiskFull() function call
        LeaveIfDiskFull( iFileSession.MkDirAll( driveAndPath ) );// create data directory here...


        }// end of cenrep error



    HBufC* logFile = HBufC::NewL( KMaxNumberLength + driveAndPath.Length() );

    CleanupStack::PushL( logFile );

    SuitableLogFileNameL();

    TPtr ptrPath( logFile->Des() );
    ptrPath.Append( driveAndPath );
    ptrPath.AppendNum( iLatestFileNameNumber );

    CleanupStack::Pop( logFile );

    return logFile;

    }

#endif


// -----------------------------------------------------------------------------
// CCALoggerManager::CreateNewFilenameL
// New actual filename for new history file.
// -----------------------------------------------------------------------------
//
HBufC* CCALoggerManager::CreateNewFilenameL()
    {

#ifndef RD_MULTIPLE_DRIVE

    CHAT_DP_TXT( "CCALoggerManager::CreateNewFilenameL" );

    TBuf< KMaxPath > pathTemp;
    iFileSession.PrivatePath( pathTemp );
    pathTemp.Append( KCAHistoryDataDirectory );

    HBufC* logFile = HBufC::NewL( KMaxNumberLength + pathTemp.Length() );

    CleanupStack::PushL( logFile );

    SuitableLogFileNameL();

    TPtr ptrPath( logFile->Des() );
    ptrPath.Append( pathTemp );
    ptrPath.AppendNum( iLatestFileNameNumber );

    CleanupStack::Pop( logFile );

    return logFile;
#else
    return CreateNewFilenameMDL();
#endif
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::CheckMemoryL
// New actual filename for new history file.
// -----------------------------------------------------------------------------
//
TBool CCALoggerManager::CheckMemoryL( TInt aMemoryRequired )
    {
    CHAT_DP_TXT( "CCALoggerManager::CheckMemory" );

#ifndef RD_MULTIPLE_DRIVE
    if ( ! SysUtil::FFSSpaceBelowCriticalLevelL(	&iFileSession,
                                                 aMemoryRequired ) )
        {
        return ETrue;
        }
    return EFalse;

#else
    TBuf< KMaxPath > defaultDrive;

    CRepository* cenrep = CRepository::NewL( KWVSettingsCenRepUid );

    TInt err = cenrep->Get( KIMPSCRIMDefaultMemoryDrive, defaultDrive );

    delete cenrep;
    cenrep = NULL;
    if ( err != KErrNone )
        {
        User::Leave( err );
        }///////////// end of reading from cenrep
    TInt driveNo = 0;
    err = RFs::CharToDrive( defaultDrive[0], driveNo );
    if ( err != KErrNone )
        {
        User::Leave( err );
        }

    TDriveNumber driveNumber = TDriveNumber( driveNo );

    if ( ! SysUtil::DiskSpaceBelowCriticalLevelL( 	&iFileSession,
                                                   aMemoryRequired, driveNumber ) )
        {
        return ETrue;
        }

    return EFalse;

#endif
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::GetLoggerMessageHeaderArray
// Pass pointer to toc array to client.
// -----------------------------------------------------------------------------
//
void CCALoggerManager::GetLoggerMessageHeaderArray(
    RPointerArray< MCALoggerMessageHeader >*& aHeaderArray )
    {
    CHAT_DP_TXT( "CCALoggerManager::GetLoggerMessageHeaderArray" );

    aHeaderArray = &iFileTocArray;
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::CreateLoggerMessagesL
// Check that file is found from toc array
// -----------------------------------------------------------------------------
//
void CCALoggerManager::CreateLoggerMessagesL(
    RPointerArray< MCALoggerMessage >& aMessageArray,
    MCALoggerMessageHeader& aKey )
    {
    CHAT_DP_TXT( "CCALoggerManager::CreateLoggerMessagesL" );

    for ( TInt a = 0; a < iFileTocArray.Count(); ++a )
        {
        if ( iFileTocArray[a] == &aKey )
            {
            //Read the file
            ReadMessageFileL( aMessageArray, aKey );
            return;
            }
        }

    //if header is missing
    User::Leave( KErrNotFound );
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::DeleteHistoryFileL
// If header is in toc array. Remove and delete it.
// -----------------------------------------------------------------------------
//
void CCALoggerManager::DeleteHistoryFileL( MCALoggerMessageHeader& aKey )
    {
    CHAT_DP_TXT( "CCALoggerManager::DeleteHistoryFileL" );

    for ( TInt a = 0; a < iFileTocArray.Count(); ++a )
        {
        if ( iFileTocArray[a] == &aKey )
            {
            //Remove header from array
            iFileTocArray.Remove( a );

            //delete file from storage space
            iFileSession.Delete( aKey.FilenameL() );


            // Get the toc file path string
            TBuf< KMaxPath > tocFile;
            TBuf< KMaxPath > tempTocFile;

#ifndef RD_MULTIPLE_DRIVE
            iFileSession.PrivatePath( tocFile );
#else
            TBuf< KMaxPath > defaultDrive;
            TBuf< KMaxPath > pathTempWithoutDrive;
            TBuf< KMaxPath > driveAndPath;

            CRepository* cenrep = NULL;
            TRAPD( err, cenrep = CRepository::NewL( KWVSettingsCenRepUid ) );

            if ( err != KErrNone )
                {
                // creation of cenrep failed -> use default path to save the conversation
                iFileSession.PrivatePath( driveAndPath );
                }
            else
                {

                TRAP( err,
                      CleanupStack::PushL( cenrep );
                      err = cenrep->Get( KIMPSCRIMDefaultMemoryDrive, defaultDrive );
                      CleanupStack::PopAndDestroy( cenrep );
                    ); // TRAP
                cenrep = NULL;

                /////// start atppend this info to complete path ////////////////////////////////
                driveAndPath.Append( defaultDrive.Left( 2 ) ); // append the drive root here
                //check whether this is in mmc card and card was protected with pwd or not
                iFileSession.PrivatePath( pathTempWithoutDrive ); // copy the private path info here..
                driveAndPath.Append( pathTempWithoutDrive ); // append the private path here...
                }
            // copy this new path into tocfile...
            tocFile.Copy( driveAndPath );

#endif

            tempTocFile.Copy( tocFile );
            tocFile.Append( KCAHistoryTocFile );
            tempTocFile.Append( KCAHistoryTocTempFile );

            User::LeaveIfError( iFileSession.Rename( tocFile, tempTocFile ) );

            // Filewritestream
            RFileWriteStream fileWriteStream;
            TInt error( fileWriteStream.Create(	iFileSession,
                                                tocFile,
                                                EFileWrite ) );
            if ( error != KErrNone )
                {
                iFileSession.Delete( tocFile );
                User::LeaveIfError( iFileSession.Rename( tempTocFile, tocFile ) );
                User::Leave( error );
                }
            CleanupClosePushL( fileWriteStream );

            //Write headers and if file write fails trap leave because we want to
            //change old file back.
            // Write the array from end to start order to the file to keep
            // order correct (from oldest to newest).
            TInt maxIndex = iFileTocArray.Count() - 1;
            for ( TInt a = maxIndex; a >= 0; --a )
                {
                if ( CheckMemoryL( iFileTocArray[a]->MessageSizeInBytesL() ) )
                    {
                    TRAPD( err, iFileTocArray[a]->ExternalizeL( fileWriteStream ) );
                    if ( err != KErrNone )
                        {
                        iFileSession.Delete( tocFile );
                        User::LeaveIfError( iFileSession.Rename( tempTocFile, tocFile ) );
                        User::Leave( err );
                        }
                    }
                else
                    {
                    iFileSession.Delete( tocFile );
                    User::LeaveIfError( iFileSession.Rename( tempTocFile, tocFile ) );
                    User::Leave( KErrDiskFull );
                    }
                }

            // Delete the temp toc file
            iFileSession.Delete( tempTocFile );

            CleanupStack::PopAndDestroy( 1 ); // fileWriteStream


            //release memory from key.
            delete &aKey;
            return;
            }
        }

    //if header is missing
    User::Leave( KErrNotFound );
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::SetLoggerMessageFactory
// Set message factory
// -----------------------------------------------------------------------------
//
void CCALoggerManager::SetLoggerMessageFactory(
    MCALoggerMessageFactory* aMessageFactory )
    {
    CHAT_DP_TXT( "CCALoggerManager::SetLoggerMessageFactory" );

    iMessageFactory = aMessageFactory;
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::CheckIfHeaderExists
// Checks if header already exists in array.
// -----------------------------------------------------------------------------
//
TBool CCALoggerManager::CheckIfHeaderExistsL( MCALoggerMessageHeader& aHeader )
    {
    for ( TInt a = 0; a < iFileTocArray.Count(); ++a )
        {
        if ( aHeader.FilenameL().CompareC( iFileTocArray[ a ]->FilenameL() )
             == 0 )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCALoggerManager::PopulateTocArray
//
// -----------------------------------------------------------------------------
//
void CCALoggerManager::PopulateTocArrayL( RPointerArray< MCALoggerMessageHeader >& aArray,
                                          const TDesC& aTocFilename
#ifdef RD_MULTIPLE_DRIVE
                                          , TBool aNotFirstTime /*ETrue*/
#endif
                                        )
    {
    RFileReadStream fileReadStream;
    TInt error = fileReadStream.Open(	iFileSession,
                                      aTocFilename,
                                      EFileWrite );
    //if file does not exists, there is no headers. just return from method
#ifndef RD_MULTIPLE_DRIVE
    if ( error == KErrNotFound )
        {
        return;
        }
    else
        {
        User::LeaveIfError( error );
        }
#else
    if ( aNotFirstTime )
        {
        if ( error )
            {
            return;
            }
        }
    else
        {
        if ( error == KErrNotFound || error == KErrPathNotFound )
            {
            return;
            }
        else
            {
            User::LeaveIfError( error );
            }
        }
#endif

    CleanupClosePushL( fileReadStream );

    MCALoggerMessageHeader* newHeader;

    //Go through whole stream.
    while ( fileReadStream.Source()->SizeL() !=
            fileReadStream.Source()->TellL( RFileBuf::ERead ).Offset() )
        {
        newHeader = iMessageFactory->CreateLoggerMessageHeaderL();
        CleanupDeletePushL( newHeader );
        newHeader->InternalizeL( fileReadStream );

        //if file for header exists, append it to iFileTocArray.
        if ( BaflUtils::FileExists( iFileSession, newHeader->FilenameL() ) &&
             !CheckIfHeaderExistsL( *newHeader ) )
            {
            User::LeaveIfError( aArray.Insert( newHeader, KArrayBegin ) );
            CleanupStack::Pop( newHeader );
            }
        else
            {
            CleanupStack::PopAndDestroy( newHeader );
            }
        }

    CleanupStack::PopAndDestroy( 1 ); // fileReadStream
    }

//end of file
