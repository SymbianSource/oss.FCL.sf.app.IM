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
* Description:  History file writer class
*
*/


// INCLUDE FILES

#include <E32DEF.H>
#include <e32std.h>
#include "CCALoggerWriter.h"
#include "MCALoggerWriteObserver.h"
#include "MCALoggerMessageHeader.h"
#include "MCALoggerMessage.h"
#include "PrivateDefinitions.h"

//for debug
#include "ChatDebugPrint.h"

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

const TInt KErrNotSaved = -999;

#endif
//CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCALoggerWriter::~CCALoggerWriter()
    {
    CHAT_DP_TXT( "CCALoggerWriter::~CCALoggerWriter" );

    if ( iLoggingOn )
        {
        //Must not leave here so trap all leaves no matter what.
        TInt ignore( KErrNone );
        TRAP( ignore, EndLoggingL() );
        }
    }

// -----------------------------------------------------------------------------
// Two phase creation.
// -----------------------------------------------------------------------------
//
CCALoggerWriter* CCALoggerWriter::NewL(		MCALoggerWriteObserver& aObserver,
                                         RFs& aFileSession )
    {
    CHAT_DP_TXT( "CCALoggerWriter::NewL" );
    CCALoggerWriter* self = new ( ELeave ) CCALoggerWriter(	aObserver,
                                                            aFileSession );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CCALoggerWriter::CCALoggerWriter(	MCALoggerWriteObserver& aObserver,
                                  RFs& aFileSession )
        : iManager( aObserver ), iFileSession( aFileSession )
    {
    CHAT_DP_TXT( "CCALoggerWriter::CCALoggerWriterL" );
    }

// ================= INHERITED FUNCTIONS =======================

// From MCALoggerWriteInterface.

// -----------------------------------------------------------------------------
// Logging starts here. Header information should be written.
// -----------------------------------------------------------------------------
//
void CCALoggerWriter::StartLoggingL( MCALoggerMessageHeader* aFile )
    {
    CHAT_DP_TXT( "CCALoggerWriter::StartLoggingL" );

    if ( ! aFile )
        {
        User::Leave( KErrArgument );
        }

    CleanupDeletePushL( aFile );

    //Ownership of aFile is transfered to this component so push it to stack

    if ( iLoggingOn )
        {
        User::Leave( KErrInUse );
        }

    HBufC* logFileName = iManager.CreateNewFilenameL(); // we should get the correct file name only....
#ifdef RD_MULTIPLE_DRIVE

    if ( logFileName == NULL )
        {
        CleanupStack::Pop( aFile );
        User::Leave( KErrNotSaved );
        return;
        }
#endif

    CleanupStack::PushL( logFileName );

    aFile->SetFilenameL( *logFileName );

    CleanupStack::PopAndDestroy( logFileName );

    // Try to create the file
    TInt error = iFileStream.Create( iFileSession, aFile->FilenameL(), EFileWrite );
    if ( error != KErrNone )
        {
        User::LeaveIfError( error );
        }

    //Transfer ownership of aFile to iManager
    TFileRelation relation;

    relation.iFile = aFile;
    relation.iWriter = this;

    CleanupStack::Pop( aFile );

    iManager.AddFileL( relation );

    iLoggingOn = ETrue;
    }

// -----------------------------------------------------------------------------
// Logging ends here. File should be closed after this.
// -----------------------------------------------------------------------------
//
void CCALoggerWriter::EndLoggingL()
    {
    CHAT_DP_TXT( "CCALoggerWriter::EndLoggingL" );

    //set Logginstate off.
    iLoggingOn = EFalse;

    //Close the file
    iFileStream.Close();

    //Inform manager that file is not needed anymore.
    iManager.RemoveFileL( *this );
    }

// -----------------------------------------------------------------------------
// Is logging on. ETrue if on, EFalse if off.
// -----------------------------------------------------------------------------
//
TBool CCALoggerWriter::IsLoggingOn()
    {
    CHAT_DP_TXT( "CCALoggerWriter::IsLoggingL" );
    return iLoggingOn;
    }

// -----------------------------------------------------------------------------
// Logging the message.
// -----------------------------------------------------------------------------
//
void CCALoggerWriter::MakeLogL( MCALoggerMessage& aMessage )
    {
    CHAT_DP_TXT( "CCALoggerWriter::MakeLogL" );

    if ( ! iLoggingOn )
        {
        User::Leave( KErrNotReady );
        }

    if ( iManager.CheckMemoryL( aMessage.MessageSizeInBytesL() ) )
        {
        aMessage.ExternalizeL( iFileStream );
        iFileStream.CommitL();
        }
    else
        {
        User::Leave( KErrDiskFull );
        }
    }

//fix - refer Ui Spec Approved Version 1.0 (Instant Messaging NG 001 151006.pdf)
//Section 10.2.10 Pg 131 -
//"In case user has saved the image already or sent it by himself,
// this option(save) is not available."
// -----------------------------------------------------------------------------
// ReLogging the message.
// -----------------------------------------------------------------------------
//
void CCALoggerWriter::ReLoggingL( MCALoggerMessageHeader& aFile )
    {
    CHAT_DP_TXT( "CCALoggerWriter::StartLoggingL" );

    if ( iLoggingOn )
        {
        User::Leave( KErrInUse );
        }

    // Try to create the file

    User::LeaveIfError( iFileStream.Replace( iFileSession,
                                             aFile.FilenameL(),
                                             EFileWrite ) );

    iLoggingOn = ETrue;

    }

//fix - refer Ui Spec Approved Version 1.0 (Instant Messaging NG 001 151006.pdf)
//Section 10.2.10 Pg 131 -
//"In case user has saved the image already or sent it by himself,
// this option(save) is not available."
// -----------------------------------------------------------------------------
// Logging ends here. File should be closed after this.
// -----------------------------------------------------------------------------
//
void CCALoggerWriter::EndReLogging()
    {
    CHAT_DP_TXT( "CCALoggerWriter::EndLoggingL" );

    //set Logginstate off.
    iLoggingOn = EFalse;

    //Close the file
    iFileStream.Close();

    }

//end of file
