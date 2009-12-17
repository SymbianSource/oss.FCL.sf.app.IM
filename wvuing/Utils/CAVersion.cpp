/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Version provider for IM.
*
*/


#include "CAVersion.h"
#include "ChatDebugPrint.h"

#include <f32file.h>
#include <pathinfo.h>


// CONSTANTS
const TInt KChatVersionBuffer = 1024;
const TInt KDriveAndColon = 2;

// ---------------------------------------------------------
// CAVersion::UpdateVersion
// ---------------------------------------------------------
//
TInt CAVersion::UpdateVersion( RFs& aFs
                               , const TDesC& aOwnVersionFile
                               , const TDesC& aTargetFile )
    {
    CHAT_DP( D_CHAT_LIT( "CAVersion::UpdateVersion %S %S" ), &aOwnVersionFile,
             &aTargetFile );

    // Get ROM and C drives
    TBuf<KDriveAndColon> romDrive;
    TBuf<KDriveAndColon> cDrive;
    romDrive = PathInfo::RomRootPath().Left( KDriveAndColon );
    cDrive = PathInfo::PhoneMemoryRootPath().Left( KDriveAndColon );

    // Construct full path for private version file
    TFileName ownVersionFile;
    aFs.PrivatePath( ownVersionFile );
    ownVersionFile.Insert( 0, romDrive );
    ownVersionFile.Append( aOwnVersionFile );

    // Append drive to published version file
    TFileName targetFile;
    targetFile.Append( cDrive );
    targetFile.Append( aTargetFile );

    if ( ownVersionFile.Compare( targetFile ) == KErrNone )
        {
        //nothing to do
        return KErrNone;
        }

    TBuf8< KChatVersionBuffer > myBuffer;
    myBuffer.Zero();

    TInt err( KErrNone );

    RFile myInputFile;
    RFile myOutputFile;

    err = myInputFile.Open( aFs, ownVersionFile, EFileRead );

    if ( !err )
        {
        err = myOutputFile.Replace( aFs, targetFile, EFileWrite );
        }
#ifdef _DEBUG
    else
        {
        CHAT_DP( D_CHAT_LIT( "CAVersion::UpdateVersion Open %d" ), err );
        }
#endif

    if ( !err )
        {
        TBool more( ETrue );
        while ( !err && more )
            {
            err = myInputFile.Read( myBuffer, KChatVersionBuffer );
            if ( !myBuffer.Length() )
                {
                more = EFalse;
                }
            if ( !err )
                {
                err = myOutputFile.Write( myBuffer );
                }
#ifdef _DEBUG
            else
                {
                CHAT_DP( D_CHAT_LIT( "CAVersion::UpdateVersion Read %d" ),
                         err );
                }

            if ( err )
                {
                CHAT_DP( D_CHAT_LIT( "CAVersion::UpdateVersion Write %d" ),
                         err );
                }
#endif
            }
        }
#ifdef _DEBUG
    else
        {
        CHAT_DP( D_CHAT_LIT( "CAVersion::UpdateVersion Replace %d" ), err );
        }
#endif

    myInputFile.Close();
    myOutputFile.Close();

    return err;
    }

// End of file
