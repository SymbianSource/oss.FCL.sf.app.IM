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
* Description:  Icon file provider.
 *
*/


// INCLUDE FILES
#include    "ccaiconfileprovider.h"
#include	<aknappui.h>
#include    <akniconutils.h>

_LIT( KMif, ".mif" );
_LIT( KMbm, ".mbm" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAIconFileProvider::CCAIconFileProvider
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAIconFileProvider::CCAIconFileProvider( RFs& aFs )
        :   iFs( aFs )
    {
    }

// -----------------------------------------------------------------------------
// CCAIconFileProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAIconFileProvider::ConstructL( const TDesC& aIconFilename )
    {
    iFilename = aIconFilename.AllocL();
    }

// -----------------------------------------------------------------------------
// CCAIconFileProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAIconFileProvider* CCAIconFileProvider::NewL(
    RFs& aFs, const TDesC& aIconFilename )
    {
    CCAIconFileProvider* self = new( ELeave ) CCAIconFileProvider( aFs );

    CleanupStack::PushL( self );
    self->ConstructL( aIconFilename );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCAIconFileProvider::~CCAIconFileProvider()
    {
    delete iFilename;
    }

// -----------------------------------------------------------------------------
// CCAIconFileProvider::RetrieveIconFileHandleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAIconFileProvider::RetrieveIconFileHandleL(
    RFile& aFile, const TIconFileType aType )
    {
    if ( ! iFilename->Length() )
        {
        User::Leave( KErrNotFound );
        }

    // get the wanted extension
    TPtrC ext( aType == MAknIconFileProvider::EMbmFile ? KMbm : KMif );
    TInt extLen( ext.Length() );

    // and replace it to given filename
    HBufC* name = iFilename->AllocLC();
    TPtr namePtr( name->Des() );
    TInt nameLen( namePtr.Length() );

    if ( nameLen <= extLen )
        {
        // name was invalid
        User::Leave( KErrArgument );
        }

    namePtr.Replace( nameLen - extLen, extLen, ext );

    // load file
    TInt retCode = aFile.Open( iFs, namePtr, EFileRead | EFileShareReadersOnly );
    User::LeaveIfError( retCode );
    CleanupStack::PopAndDestroy( name );
    }

// -----------------------------------------------------------------------------
// CCAIconFileProvider::Finished
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAIconFileProvider::Finished()
    {
    // commit suicide because Avkon Icon Server said so
    delete this;
    }

//  End of File
