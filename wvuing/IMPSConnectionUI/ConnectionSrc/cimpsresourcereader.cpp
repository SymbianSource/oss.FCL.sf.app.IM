/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Resource reader
*
*/


#include <bautils.h>
#include <ImpsConnectionUiNg.rsg>

#include "cimpsresourcereader.h"

// Constants
_LIT( KIMPSConnectionUINGResFile, "IMPSConnectionUiNG.rsc" );
_LIT( KIMPSConnectionUiNGResourcePath, "\\Resource\\" );


// -----------------------------------------------------------------------------
// CIMPSResourceReader::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSResourceReader* CIMPSResourceReader::NewL()
    {
    CIMPSResourceReader* self =  CIMPSResourceReader::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIMPSResourceReader::NewLC()
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSResourceReader* CIMPSResourceReader::NewLC()
    {
    CIMPSResourceReader* self = new( ELeave )CIMPSResourceReader;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CIMPSResourceReader::CIMPSResourceReader()
// -----------------------------------------------------------------------------
//
CIMPSResourceReader::CIMPSResourceReader()
    {
    }


// -----------------------------------------------------------------------------
// CIMPSResourceReader::~CIMPSResourceReader()
// -----------------------------------------------------------------------------
//
EXPORT_C CIMPSResourceReader::~CIMPSResourceReader()
    {
    iResFile.Close();
    iFs.Close();
    }


// -----------------------------------------------------------------------------
// CIMPSResourceReader::ConstructL()
// -----------------------------------------------------------------------------
//
void CIMPSResourceReader::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );

    TFileName resourceFileName;
    resourceFileName.Zero();

    TFileName drivePath;
    Dll::FileName( drivePath );
    resourceFileName.Append( TParsePtrC( drivePath ).Drive() );
    resourceFileName.Append( KIMPSConnectionUiNGResourcePath() );
    resourceFileName.Append( KIMPSConnectionUINGResFile );

    if ( resourceFileName.Length() > 0 )
        {
        // when baflutils gets an empty string, it returns "C:",
        // which breaks things
        BaflUtils::NearestLanguageFile( iFs, resourceFileName );
        }

    iResFile.OpenL( iFs, resourceFileName );
    iResFile.ConfirmSignatureL();
    }


// -----------------------------------------------------------------------------
// CIMPSResourceReader::IntResourceValueL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIMPSResourceReader::IsRoamingWarningRequiredL()
    {
    return IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_ROAMING_WARNING );
    }

// -----------------------------------------------------------------------------
// CIMPSResourceReader::IntResourceValueL()
// -----------------------------------------------------------------------------
//
TInt CIMPSResourceReader::IntResourceValueL( TInt aResourceId )
    {
    //Own resource reader for AA plug-in environment (no CONE facilities available)
    TInt plainResourceId = 0x00000fff & aResourceId; // Remove offset from id

    HBufC8* rawDataBuf = iResFile.AllocReadLC( plainResourceId );

    TResourceReader reader;
    reader.SetBuffer( rawDataBuf );

    TInt value = reader.ReadInt32();

    CleanupStack::PopAndDestroy( rawDataBuf );

    return value;
    }

















