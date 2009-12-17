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
* Description:  Takes care of persistent storaging for groups
 *
*/



// INCLUDE FILES

#include <f32file.h>
#include <s32mem.h>

#include "ChatDebugPrint.h"

#include "CCAGroupStorage.h"
#include "CCAStorageDefs.h"
#include "MCAExtendedStoredGroup.h"

// ================= MEMBER FUNCTIONS =======================


// destructor
CCAGroupStorage::~CCAGroupStorage()
    {
    CloseTable();
    delete iColset;
    }

// ----------------------------------------------------------
// CCAGroupStorage::NewL
// ----------------------------------------------------------
//
CCAGroupStorage* CCAGroupStorage::NewL()
    {
    CCAGroupStorage* self = new ( ELeave ) CCAGroupStorage();
    return self;
    }

// ----------------------------------------------------------
// CCAGroupStorage::SetDbL
// ----------------------------------------------------------
//
void CCAGroupStorage::SetDbL( RDbStoreDatabase aDb )
    {
    CDbColSet* tempColSet = aDb.ColSetL( KGroupTable );
    delete iColset;
    iColset = tempColSet;
    iDb = aDb;
    }

// ----------------------------------------------------------
// CCAGroupStorage::ReadFirstL
// ----------------------------------------------------------
//
TInt CCAGroupStorage::ReadFirstL( MCAExtendedStoredGroup* aGroup )
    {
    OpenTableL();
    iTable.BeginningL();
    return ReadNextL( aGroup );
    }

// ----------------------------------------------------------
// CCAGroupStorage::ReadNextL
// ----------------------------------------------------------
//
TInt CCAGroupStorage::ReadNextL( MCAExtendedStoredGroup* aGroup )
    {
    OpenTableL();
    if ( iTable.NextL() )
        {
        TRAPD( err, ReadCurrentL( aGroup ) );

        // For some reason ReadCurrentL leaves with KErrEof,
        // even if the contact was read succesfully.
        if ( err == KErrEof )
            {
            return KErrNone;
            }
        return err;
        }

    return KErrNotFound;
    }

// ----------------------------------------------------------
// CCAGroupStorage::ReadGroupL
// ----------------------------------------------------------
//
void CCAGroupStorage::ReadGroupL( MCAExtendedStoredGroup* aGroup )
    {
    OpenTableL();
    TDbColNo colNo = iColset->ColNo( KGroupId );
    if ( SeekRowL( colNo, aGroup->GroupId() ) )
        {
        TRAPD( err, ReadCurrentL( aGroup ) );

        // For some reason ReadCurrentL leaves with KErrEof,
        // even if the contact was read succesfully.
        if ( ( err == KErrEof ) || ( err == KErrNone ) )
            {
            return;
            }
        User::Leave( err );
        }
    }

// ----------------------------------------------------------
// CCAGroupStorage::WriteGroupL
// ----------------------------------------------------------
//
void CCAGroupStorage::WriteGroupL( MCAExtendedStoredGroup* aGroup )
    {
    OpenTableL();
    WriteToStoreDbL( aGroup );
    }

// ----------------------------------------------------------
// CCAGroupStorage::DeleteGroupL
// ----------------------------------------------------------
//
void CCAGroupStorage::DeleteGroupL( MCAExtendedStoredGroup* aGroup )
    {
    CHAT_DP_FUNC_ENTER( "DeleteGroupL" );

    OpenTableL();
    TDbColNo colNo = iColset->ColNo( KGroupId );

    if ( SeekRowL( colNo, aGroup->GroupId() ) )
        {
        CHAT_DP_FUNC_DP( "DeleteGroupL", "Found it, deleting" );
        iTable.DeleteL();
        }
    else
        {
        CHAT_DP_FUNC_DP( "DeleteGroupL", "Group not found, not deleted" );
        }

    CHAT_DP_FUNC_DONE( "DeleteGroupL" );
    }

// ----------------------------------------------------------
// CCAGroupStorage::OpenTableL
// ----------------------------------------------------------
//
void CCAGroupStorage::OpenTableL()
    {
    if ( iTableOpened )
        {
        return;
        }

    TInt err( iTable.Open( iDb, KGroupTable ) );

    if ( err != KErrNone )
        {
        iTable.Close();
        iTableOpened = EFalse;
        User::Leave( err );
        return;
        }

    iTableOpened = ETrue;
    return;
    }

// ----------------------------------------------------------
// CCAGroupStorage::CloseTable
// ----------------------------------------------------------
//
void CCAGroupStorage::CloseTable()
    {
    if ( iTableOpened )
        {
        iTable.Close();
        }
    iTableOpened = EFalse;
    }

// ----------------------------------------------------------
// CCAGroupStorage::WriteToStoreDbL
// ----------------------------------------------------------
//
void CCAGroupStorage::WriteToStoreDbL( MCAExtendedStoredGroup* aGroup )
    {

    TDbColNo colNo = iColset->ColNo( KGroupId );

    if ( SeekRowL( colNo, aGroup->GroupId() ) )
        {
        iTable.UpdateL();
        }
    else
        {
        iTable.InsertL();
        }

    // Write group id
    iTable.SetColL( colNo, aGroup->GroupId() );

    RDbColWriteStream stream;
    colNo = iColset->ColNo( KGroupData );
    stream.OpenL( iTable, colNo );
    stream.PushL();

    // Externalize contact to buffer
    aGroup->ExternalizeL( stream );
    stream.Close();

    // Write row to table
    TRAPD( err, iTable.PutL() );
    if ( err != KErrNone )
        {
        // recover
        iTable.Cancel();
        iTable.Reset();
        }
    CleanupStack::PopAndDestroy();	// stream
    }

// ----------------------------------------------------------
// CCAGroupStorage::ReadCurrentL
// ----------------------------------------------------------
//
void CCAGroupStorage::ReadCurrentL( MCAExtendedStoredGroup* aGroup )
    {
    iTable.GetL();

    // Get Column number for contact data size
    TDbColNo colNo = iColset->ColNo( KGroupDataSize );

    RDbColReadStream stream;
    colNo = iColset->ColNo( KGroupData );
    stream.OpenL( iTable, colNo );
    stream.PushL();

    // Read stream to contact
    aGroup->InternalizeL( stream );
    stream.Close();

    CleanupStack::PopAndDestroy();	// stream
    }

// ----------------------------------------------------------
// CCAGroupStorage::SeekRowL
// ----------------------------------------------------------
//
TBool CCAGroupStorage::SeekRowL( TDbColNo aColNo, const TDesC& aId )
    {
    iTable.BeginningL();

    while ( iTable.NextL() )
        {
        iTable.GetL();
        if ( !iTable.ColDes( aColNo ).CompareC( aId, KCollationLevel, NULL ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// constructor
CCAGroupStorage::CCAGroupStorage()
    {
    }


//  End of File

