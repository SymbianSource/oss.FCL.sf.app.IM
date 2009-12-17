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
* Description:  Implementation of alternator.
*
*/



// INCLUDE FILES
#include "CAlternatorImp.h"
#include "ChatDebugPrint.h"
#include "CCABrandInstallerImp.h"
#include <eikenv.h>
#include <bautils.h>
#include <s32strm.h>	// load/save 
#include <s32file.h>
#include <eikapp.h>
#include <bautils.h>
#include <bldvariant.hrh>
#include <e32std.h>
#include <badesca.h>


// CONSTANTS
_LIT( KRsc, ".RSC" );
_LIT( KRscWild, ".R*" );
_LIT( KDirSeparator, "\\" );
_LIT( KRelativeBrandPath, "branding\\" );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CAlternatorImp::CAlternatorImp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAlternatorImp::CAlternatorImp()
//	:iResFiles( 1 )	// usually just one,
    {
    }

// -----------------------------------------------------------------------------
// CAlternatorImp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAlternatorImp::ConstructL( const TDesC& aBrandRoot )
    {
    CHAT_DP( D_CHAT_LIT( "CAlternatorImp::ConstructL brand root is:%S" ) ,
             &aBrandRoot );
    iBrandRoot = HBufC::NewL( KMaxPath );
    TPtr myPtr = iBrandRoot->Des();
    myPtr = aBrandRoot;

    CHAT_DP( D_CHAT_LIT( "CAlternatorImp::ConstructL ourbrand root is:%S" ),
             &myPtr );

    iEikEnv = CEikonEnv::Static();

    //Start procedure to install sis installed brands
    iBrandInstaller = CCABrandInstallerImp::NewL();
    if ( !( iBrandInstaller->ImportFolderExistsL() ) )
        {
        iBrandInstaller->CreateImportFolderL();
        }
    if ( iBrandInstaller->IsNewBrandAvailableL() )
        {
        iBrandInstaller->InstallNewBrandL();
        }
    }

// -----------------------------------------------------------------------------
// CAlternatorImp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CAlternatorImp* CAlternatorImp::NewL( const TDesC& aBrandRoot )
    {
    CAlternatorImp* self = new( ELeave ) CAlternatorImp();

    CleanupStack::PushL( self );
    self->ConstructL(  aBrandRoot );
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CAlternatorImp::~CAlternatorImp()
    {
    delete iBrandRoot;
    delete iBrandInstaller;
    }


// constructs current bitmap file full path
const TDesC& CAlternatorImp::CurrentBitmapFileL( const TDesC& aModuleId,
                                                 const TDesC& aAlternative,
                                                 const TDesC& aBitmapFileName )
    {
    // if the path is not ok, set bitmap path as empty
    TInt ret( ConstructCompletePathAndCheck( iCurrentBitmap, aModuleId,
                                             aAlternative,
                                             aBitmapFileName ) );
    if ( ret != KErrNone )
        {
        if ( ret == KErrNoMemory )
            {
            User::Leave( ret );
            }
        iCurrentBitmap.Zero();
        }
    CHAT_DP( D_CHAT_LIT( "CAlternatorImp::CurrentBitmapFileL :%S" ),
             &iCurrentBitmap );

    return iCurrentBitmap;
    }


// changes resource
TInt CAlternatorImp::ChangeResourceL( const TDesC& aModuleId,
                                      const TDesC& aAlternative,
                                      const TDesC& aResFileName,
                                      TInt& aResOffset  )
    {
    TInt err = LoadResourceFileL( aModuleId, aAlternative, aResFileName,
                                  aResOffset );
    return err;
    }

// changes bitmap file
TInt CAlternatorImp::ChangeBitmapFileL( const TDesC& aModuleId,
                                        const TDesC& aAlternative,
                                        const TDesC& aBitmapFileName )
    {
    // iCurrentBitmap contains the path if ok
    iCurrentBitmap.Zero();
    TInt ret( ConstructCompletePathAndCheck( iCurrentBitmap, aModuleId,
                                             aAlternative, aBitmapFileName ) );
    if ( ret == KErrNoMemory )
        {
        User::Leave( ret );
        }
    if ( ret != KErrNone )
        {
        return KErrNotFound;
        }
    // the new bitmap path is in iCurrentBitmap
    return KErrNone;
    }

void  CAlternatorImp::GetCurrentResourceFileL( const TDesC& aModuleId,
                                               const TDesC& aAlternative,
                                               const TDesC& aResFileName,
                                               TDes& aFullPath )
    {
    TInt ret( ConstructCompletePathAndCheck( aFullPath, aModuleId,
                                             aAlternative, aResFileName ) );
    if ( ret == KErrNoMemory )
        {
        User::Leave( ret );
        }
    if ( ret != KErrNone )
        {
        aFullPath.Zero();
        }

    CHAT_DP( D_CHAT_LIT( "CAlternatorImp::GetCurrentResourceFileL :%S" ),
             &aFullPath );

    // otherwise the current resource file path is in aFullPath
    }

void CAlternatorImp::SetBrandRoot( const TDesC& aBrandRoot )
    {
    CHAT_DP( D_CHAT_LIT( "CAlternatorImp::SetBrandRoot %S" ), &aBrandRoot );

    TPtr myPtr = iBrandRoot->Des();
    myPtr = aBrandRoot;
    }

// loads resource file
TInt CAlternatorImp::LoadResourceFileL( const TDesC& aModuleId,
                                        const TDesC& aAlternative,
                                        const TDesC& aResFileName,
                                        TInt& aResOffSet )
    {

    TFileName resFilePath;
    TInt ret( ConstructCompletePathAndCheck( resFilePath, aModuleId,
                                             aAlternative, aResFileName ) );
    if ( ret == KErrNoMemory )
        {
        User::Leave( ret );
        }
    if ( ret != KErrNone )
        {
#ifdef _DEBUG
        RDebug::Print( _L( "path not found" ) );
#endif
        return KErrNotFound;
        }

    TEntry entry;
    User::LeaveIfError( iEikEnv->FsSession().Entry( resFilePath, entry ) );
    TInt fileSize( entry.iSize );
    // test if there's enough heap available, this one leaves if not and
    // resources won't get changed
    HBufC8* allocTest = HBufC8::NewL( fileSize );
    delete allocTest;


    // if path ok, then make the change, first delete the previous one
    iEikEnv->DeleteResourceFile( aResOffSet );

    RFs& fs = iEikEnv->FsSession();

    if ( resFilePath.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( fs, resFilePath );
        }

    TRAPD( err, aResOffSet =  iEikEnv->AddResourceFileL( resFilePath )  );
    if ( err == KErrNoMemory )
        {
        User::Leave( KErrNoMemory );
        }


#ifdef _DEBUG
    RDebug::Print( _L( "ADDED resource file:" ) );
    RDebug::Print( resFilePath );
#endif

    return err;
    }


TBool CAlternatorImp::IsPathOk( const TDesC& aCheckThis ) const
    {
    CHAT_DP( D_CHAT_LIT( "CAlternatorImp::IsPathOk %S" ), &aCheckThis );

    return BaflUtils::FileExists( iEikEnv->FsSession(), aCheckThis );
    }

TInt CAlternatorImp::ConstructCompletePathAndCheck( TDes& aResult,
                                                    const TDesC& aModuleId,
                                                    const TDesC& aAlternative,
                                                    const TDesC& aFileName )
    {
    aResult.Zero();
    aResult.Append( *iBrandRoot );

    aResult.Append( KRelativeBrandPath() );
    aResult.Append( aModuleId );
    aResult.Append( KDirSeparator );
    aResult.Append( aAlternative );
    aResult.Append( KDirSeparator );

    TBool resourceFile( EFalse );
    TFileName file( aFileName );

    TInt rscPos( aFileName.FindC( KRsc ) );
    if ( rscPos != KErrNotFound )
        {
        // replace the ".RSC" with ".R*"
        file.Copy( aFileName.Left( rscPos ) );
        file.Append( KRscWild );
        resourceFile = ETrue;
        }
    else
        {
        file.Copy( aFileName );
        }

    CHAT_DP( D_CHAT_LIT( "CAlternatorImp::ConstructCompletePathAndCheck \
                          trying to find %S%S" )
             , &aResult, &file );

    // Find the actual file
    CDir* files = NULL;
    TFindFile fileFinder( iEikEnv->FsSession() );
    TInt err( fileFinder.FindWildByDir( file, aResult, files ) );
    delete files;

    // If file was found, then take the first appearance of it
    // (order is correct aready)
    if ( err == KErrNone )
        {
        aResult.Zero();
        if ( resourceFile )
            {
            // change the ".R*" back to ".RSC"
            TParsePtrC parse( fileFinder.File() );
            file.Copy( parse.DriveAndPath() );
            file.Append( parse.Name() );
            file.Append( KRsc );

            CHAT_DP( D_CHAT_LIT( "CAlternatorImp::ConstructCompletePathAndCheck \
    		                      finding nearest language file for %S" ), &file );

            if ( file.Length() != 0 )
                {
                BaflUtils::NearestLanguageFile( iEikEnv->FsSession(), file );
                }

            aResult.Copy( file );
            }
        else
            {
            aResult.Append( fileFinder.File() );
            }
        }

    CHAT_DP( D_CHAT_LIT( "CAlternatorImp::ConstructCompletePathAndCheck \
                          TFindFile returned %d for %S" ),
             err, &aResult );

    // return was the file found or not
    return err;
    }

//  End of File
