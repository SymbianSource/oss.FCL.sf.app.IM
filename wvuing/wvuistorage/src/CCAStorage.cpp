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
* Description:  Takes care of persistent storaging
*
*/


// INCLUDE FILES

#include <f32file.h>
#include <sysutil.h>

#include "ChatDebugPrint.h"

#include "CCAStorage.h"
#include "MCAExtendedStoredGroup.h"

#include "CCAStorageDefs.h"

// Database overhead
const TInt KDbOverhead = 32;
const TInt KEmptyDbSize = 1024;

// ================= MEMBER FUNCTIONS =======================

// constructor
CCAStorage::CCAStorage()
    {
    }

// destructor
CCAStorage::~CCAStorage()
    {
    delete iGroupStorage;
    CloseDb();
    iFs.Close();
    }

// -----------------------------------------------------------
// CCAStorage::NewL
// -----------------------------------------------------------
//
CCAStorage* CCAStorage::NewL()
    {
    CCAStorage* self = new ( ELeave ) CCAStorage();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------
// CCAStorage::LoadL
// -----------------------------------------------------------
//
void CCAStorage::LoadL( MCAExtendedStoredGroup* aGroup )
    {
    iGroupStorage->ReadGroupL( aGroup );
    }

// -----------------------------------------------------------
// CCAStorage::SaveL
// -----------------------------------------------------------
//
void CCAStorage::SaveL( MCAExtendedStoredGroup* aGroup )
    {
    CheckFreespaceLevelL( aGroup );
    iGroupStorage->WriteGroupL( aGroup );
    iDb.Compact();
    }

// -----------------------------------------------------------
// CCAStorage::DeleteL
// -----------------------------------------------------------
//
void CCAStorage::DeleteL( MCAExtendedStoredGroup* aGroup )
    {
    CHAT_DP_FUNC_ENTER( "DeleteL" );

    if ( ! aGroup )
        {
        CHAT_DP_FUNC_DP( "DeleteL", "Group is NULL!" );
        return; // can't do anything!
        }

    if ( ! iGroupStorage )
        {
        CHAT_DP_FUNC_DP( "DeleteL", "Group storage is NULL!" );
        return; // can't do anything!
        }

    iGroupStorage->DeleteGroupL( aGroup );
    iDb.Compact();

    CHAT_DP_FUNC_DONE( "DeleteL" );
    }

// -----------------------------------------------------------
// CCAStorage::LoadFirstL
// -----------------------------------------------------------
//
TInt CCAStorage::LoadFirstL( MCAExtendedStoredGroup* aGroup )
    {
    return iGroupStorage->ReadFirstL( aGroup );
    }

// -----------------------------------------------------------
// CCAStorage::LoadNextL
// -----------------------------------------------------------
//
TInt CCAStorage::LoadNextL( MCAExtendedStoredGroup* aGroup )
    {
    return iGroupStorage->ReadNextL( aGroup );
    }

// -----------------------------------------------------------
// CCAStorage::DbExists
// -----------------------------------------------------------
//
TBool CCAStorage::DbExists()
    {
    RFile temp;

    TBuf< KMaxPath > storagePath;
    iFs.PrivatePath( storagePath );
    storagePath.Append( KStorageName );

    TInt err( temp.Open( iFs, *( &storagePath ), EFileRead ) );

    TInt size( 0 );
    if ( err == KErrNone )
        {
        temp.Size( size );
        }

    temp.Close();

    if ( size == 0 )
        {
        return EFalse;
        }

    return ( err == KErrNone );
    }

// -----------------------------------------------------------
// CCAStorage::CreateDbL
// -----------------------------------------------------------
//
void CCAStorage::CreateDbL()
    {
    TBuf< KMaxPath > storagePath;
    iFs.PrivatePath( storagePath );
    iFs.MkDirAll( storagePath );    // make sure the directory exists

    storagePath.Append( KStorageName );
    iFileStore = CPermanentFileStore::ReplaceL( iFs, storagePath, EFileWrite );

    iFileStore->SetTypeL( iFileStore->Layout() );

    TStreamId streamId( 0 );
    TRAPD( err, streamId = iDb.CreateL( iFileStore ) );

    if ( err )
        {
        delete iFileStore;
        iFileStore = NULL;

        // remember to keep this updated
        iFs.Delete( storagePath );
        User::Leave( err );
        }

    iFileStore->SetRootL( streamId );
    TInt err2( iFileStore->Commit() );
    User::LeaveIfError( err2 );

    TRAPD( err3, CreateTablesL() );
    if ( err3 )
        {
        delete iFileStore;
        iFileStore = NULL;
        iDb.Close();
        iFs.Delete( storagePath );
        User::Leave( err3 );
        }
    }

// -----------------------------------------------------------
// CCAStorage::OpenDbL
// -----------------------------------------------------------
//
void CCAStorage::OpenDbL()
    {
    TBuf< KMaxPath > storagePath;
    iFs.PrivatePath( storagePath );
    storagePath.Append( KStorageName );

    iFileStore = CPermanentFileStore::OpenL( iFs, storagePath, EFileWrite );
    iFileStore->SetTypeL( iFileStore->Layout() );
    iDb.OpenL( iFileStore, iFileStore->Root() );
    }

// -----------------------------------------------------------
// CCAStorage::CloseDb
// -----------------------------------------------------------
//
void CCAStorage::CloseDb()
    {
    delete iFileStore;
    iFileStore = NULL;
    iDb.Close();
    }

// -----------------------------------------------------------
// CCAStorage::CreateTablesL
// -----------------------------------------------------------
//
void CCAStorage::CreateTablesL()
    {
    CDbColSet* groups = CDbColSet::NewLC();

    AddColumnL( KGroupId, EDbColText, groups, KDbUndefinedLength );
    AddColumnL( KGroupData, EDbColLongBinary, groups );
    AddColumnL( KGroupDataSize, EDbColInt32, groups );

    TInt err = iDb.CreateTable( KGroupTable, *groups );
    User::LeaveIfError( err );

    CleanupStack::PopAndDestroy( groups );	// groups
    }

// -----------------------------------------------------------
// CCAStorage::AddColumnL
// -----------------------------------------------------------
//
void CCAStorage::AddColumnL( const TDesC& aName, TDbColType aType,
                             CDbColSet* aColset, TInt aMaxLength /* =0 */ )
    {
    TDbCol column( aName, aType );
    if ( aMaxLength != 0 )
        {
        column.iMaxLength = aMaxLength;
        }
    aColset->AddL( column );
    }

// -----------------------------------------------------------
// CCAStorage::CheckFreespaceLeveL
// -----------------------------------------------------------
//
void CCAStorage::CheckFreespaceLevelL( MCAExtendedStoredGroup* aGroup )
    {
    DoFreespaceLevelCheckL( aGroup->MaximalSize() + KDbOverhead );
    }

// -----------------------------------------------------------
// CCAStorage::DoFreespaceLevelCheckL
// -----------------------------------------------------------
//
void CCAStorage::DoFreespaceLevelCheckL( TInt aSize )
    {
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFs, aSize ) )
        {
        User::Leave( KErrDiskFull );
        }
    }

// -----------------------------------------------------------
// CCAStorage::ConstructL
// -----------------------------------------------------------
//
void CCAStorage::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );

    iGroupStorage = CCAGroupStorage::NewL();

    // Open existing or create new database.
    if ( DbExists() )
        {
        OpenDbL();
        }
    else
        {
        DoFreespaceLevelCheckL( KEmptyDbSize );
        CreateDbL();
        }
    iGroupStorage->SetDbL( iDb );
    }

//  End of File
