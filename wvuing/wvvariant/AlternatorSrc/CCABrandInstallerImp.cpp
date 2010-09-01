/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Concrete implementation for installing branding data
*
*/



// INCLUDE FILES
#include "CCABrandInstallerImp.h"
#include "ChatDebugPrint.h"
#include <bautils.h>
#include <eikenv.h>
#include <f32file.h>

// CONSTANTS
namespace
    {
    _LIT( KBrandingFolder, "Branding\\IMApp" );
    _LIT( KImportFolder, "import" );
    _LIT( KDirSeparator, "\\" );
    _LIT( KBranding, "Branding" );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCABrandInstallerImp::CCABrandInstallerImp()
    {
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCABrandInstallerImp::ConstructL()
    {
    iEikEnv = CEikonEnv::Static();
    iFileMan = CFileMan::NewL( iEikEnv->FsSession() );
    iPrivatePath = HBufC::NewL( KMaxPath );
    TPtr privatePath( iPrivatePath->Des() );
    iEikEnv->FsSession().PrivatePath( privatePath );
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCABrandInstallerImp* CCABrandInstallerImp::NewL()
    {
    CCABrandInstallerImp* self = CCABrandInstallerImp::NewLC();
    CleanupStack::Pop();
    return self;
    }

CCABrandInstallerImp* CCABrandInstallerImp::NewLC()
    {
    CCABrandInstallerImp* self = new( ELeave ) CCABrandInstallerImp;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CCABrandInstallerImp::~CCABrandInstallerImp()
    {
    delete iFileMan;
    delete iPrivatePath;
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::BrandAlreadyExistsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCABrandInstallerImp::BrandAlreadyExistsL( const TDesC& aBrandName )
    {
    HBufC* folderPath = HBufC::NewLC( KMaxPath );
    TPtr folderPathPtr( folderPath->Des() );

    //Append private folder to folder path
    folderPathPtr.Append( *iPrivatePath );
    //Append branding path to private path
    folderPathPtr.Append( KBrandingFolder );

    folderPathPtr.Append( KDirSeparator );
    //Append server name to path
    folderPathPtr.Append( aBrandName );
    //Check if folder exists, if exists brand is once installed
    TBool returnValue = FolderExists( *folderPath );
    CleanupStack::PopAndDestroy( folderPath );
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::CreateImportFolderL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABrandInstallerImp::CreateImportFolderL()
    {
    HBufC* folderPath = HBufC::NewLC( KMaxPath );
    TPtr folderPathPtr( folderPath->Des() );
    folderPathPtr.Append( *iPrivatePath );
    //Append import folder to private path
    folderPathPtr.Append( KImportFolder );

    folderPathPtr.Append( KDirSeparator );

    //create import folder
    User::LeaveIfError( CreateFolder( *folderPath ) );
    CleanupStack::PopAndDestroy( folderPath );
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::ImportFolderExistsL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCABrandInstallerImp::ImportFolderExistsL()
    {
    HBufC* privatePath = HBufC::NewLC( KMaxPath );
    TPtr privatePathPtr( privatePath->Des() );

    privatePathPtr.Append( *iPrivatePath );
    //Append import folder to private path
    privatePathPtr.Append( KImportFolder );

    privatePathPtr.Append( KDirSeparator );
    //Check if import folder exists
    TBool returnValue = FolderExists( *privatePath );
    CleanupStack::PopAndDestroy( privatePath );
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::ListOfInstalledBrandsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABrandInstallerImp::ListOfInstalledBrandsL( CDesCArray& aListOfBrands )
    {
    RFs& fileSys = iEikEnv->FsSession();
    HBufC* privatePath = HBufC::NewLC( KMaxPath );
    TPtr privatePathPtr( privatePath->Des() );

    privatePathPtr.Append( *iPrivatePath );
    //Append branding folder to path
    privatePathPtr.Append( KBrandingFolder );

    privatePathPtr.Append( KDirSeparator );

    CDir* dirList;
    //Get directories
    User::LeaveIfError( fileSys.GetDir( *privatePath, KEntryAttMaskSupported, ESortByName, dirList ) );
    for ( TInt i = 0; i < dirList->Count(); i++ )
        {
        aListOfBrands.AppendL( ( *dirList )[ i ].iName );
        }
    delete dirList;
    CleanupStack::PopAndDestroy( privatePath );
    }
// -----------------------------------------------------------------------------
// CCABrandInstallerImp::ListOfUninstalledBrandsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABrandInstallerImp::ListOfUninstalledBrandsL( CDesCArray& aListOfBrands )
    {
    RFs& fileSys = iEikEnv->FsSession();
    HBufC* privatePath = HBufC::NewLC( KMaxPath );
    TPtr privatePathPtr( privatePath->Des() );

    privatePathPtr.Append( *iPrivatePath );
    //Append import folder to path
    privatePathPtr.Append( KImportFolder );

    privatePathPtr.Append( KDirSeparator );
    //Append branding folder to path
    privatePathPtr.Append( KBrandingFolder );
    privatePathPtr.Append( KDirSeparator );

    CDir* dirList;
    //Get directories
    CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::ListOfUninstalledBrandsL: Get dirs from folder:%S" ), &privatePathPtr );

    User::LeaveIfError( fileSys.GetDir( *privatePath, KEntryAttMaskSupported, ESortByName, dirList ) );
    HBufC* path = HBufC::NewLC( KMaxPath );
    TPtr pathPtr( path->Des() );
    for ( TInt i = 0; i < dirList->Count(); i++ )
        {
        pathPtr.Append( ( *dirList )[ i ].iName );
        CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::ListOfUninstalledBrandsL: Brand folder:%S" ), &pathPtr );

        aListOfBrands.AppendL( *path );
        pathPtr.Zero();
        }
    delete dirList;
    CleanupStack::PopAndDestroy( 2 );
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::IsNewBrandAvailableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCABrandInstallerImp::IsNewBrandAvailableL()
    {
    HBufC* privatePath = HBufC::NewLC( KMaxPath );
    TPtr privatePathPtr( privatePath->Des() );

    privatePathPtr.Append( *iPrivatePath );
    //Append import folder to path
    privatePathPtr.Append( KImportFolder );

    privatePathPtr.Append( KDirSeparator );
    //Append branding folder to path
    privatePathPtr.Append( KBrandingFolder );

    privatePathPtr.Append( KDirSeparator );
    CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::IsNewBrandAvailableL(): Searching from...%S" ), &privatePathPtr );
    TBool returnValue = FolderExists( *privatePath );
    CleanupStack::PopAndDestroy( privatePath );
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::InstallNewBrandL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCABrandInstallerImp::InstallNewBrandL()
    {

    CDesCArray* brands = new ( ELeave ) CDesCArrayFlat( 10 );
    CleanupStack::PushL( brands );

    ListOfUninstalledBrandsL( *brands );

    HBufC* installPath = HBufC::NewLC( KMaxPath );
    TPtr installPathPtr( installPath->Des() );

    HBufC* sourcePath = HBufC::NewLC( KMaxPath );
    TPtr sourcePathPtr( sourcePath->Des() );

    for ( TInt i = 0; i < brands->MdcaCount(); i++ )
        {
        installPathPtr.Append( *iPrivatePath );
        installPathPtr.Append( KBrandingFolder );
        installPathPtr.Append( KDirSeparator );
        installPathPtr.Append( brands->MdcaPoint( i ) );

        CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::InstallNewBrandL(): Install path: %S" ), &installPathPtr );

        if ( !( FolderExists( *installPath ) ) )
            {
            CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::InstallNewBrandL(): Create unexisting folder: %S" ), &installPathPtr );
            TInt createErr = CreateFolder( *installPath );
            CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::InstallNewBrandL(): CreateErr: %d" ), createErr );
            }
        else
            {
            //If install folder exists, then there is coming update to brand.
            //Old files must be removed before install.
            CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::InstallNewBrandL(): Remove existin folder: %S" ), &installPathPtr );
            TInt remErr = RemoveFolder( *installPath );
            CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::InstallNewBrandL(): RemoveErr: %d" ), remErr );
            CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::InstallNewBrandL(): Create Removed existin folder: %S" ), &installPathPtr );
            TInt createErr2 = CreateFolder( *installPath );
            CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::InstallNewBrandL(): CreateErr2: %d" ), createErr2 );
            }

        sourcePathPtr.Append( *iPrivatePath );
        sourcePathPtr.Append( KImportFolder );
        sourcePathPtr.Append( KDirSeparator );
        sourcePathPtr.Append( KBrandingFolder );
        sourcePathPtr.Append( KDirSeparator );
        sourcePathPtr.Append( brands->MdcaPoint( i ) );
        CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::InstallNewBrandL(): Source path: %S" ), &sourcePathPtr );


        TInt err = MoveFolder( *sourcePath, *installPath );
        sourcePathPtr.Zero();
        installPathPtr.Zero();
        CHAT_DP( D_CHAT_LIT( "CCABrandInstallerImp::InstallNewBrandL(): MoveFolder err: %d" ), err );
        User::LeaveIfError( err );
        }

    //Clean import folder
    HBufC* removedPath = HBufC::NewLC( KMaxPath );
    TPtr removedPathPtr( removedPath->Des() );
    removedPathPtr.Append( *iPrivatePath );
    removedPathPtr.Append( KImportFolder );
    removedPathPtr.Append( KDirSeparator );
    removedPathPtr.Append( KBranding );
    removedPathPtr.Append( KDirSeparator );

    RemoveFolder( *removedPath );
    CleanupStack::PopAndDestroy( 4 );
    }
// -----------------------------------------------------------------------------
// CCABrandInstallerImp::FolderExists
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCABrandInstallerImp::FolderExists( const TDesC& aFolderPath )
    {
    return BaflUtils::FolderExists( iEikEnv->FsSession(), aFolderPath );
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::CreateFolder
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCABrandInstallerImp::CreateFolder( const TDesC& aFolderPath )
    {
    RFs& fileSys = iEikEnv->FsSession();
    return fileSys.MkDirAll( aFolderPath );
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::MoveFolder
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCABrandInstallerImp::MoveFolder( const TDesC& aSourcePath, const TDesC& aDestFolder )
    {
    RFs& fileSys = iEikEnv->FsSession();
    return fileSys.Rename( aSourcePath, aDestFolder );
    }

// -----------------------------------------------------------------------------
// CCABrandInstallerImp::RemoveFolder
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCABrandInstallerImp::RemoveFolder( const TDesC& aFolderPath )
    {
    return iFileMan->RmDir( aFolderPath );
    }

//  End of File
