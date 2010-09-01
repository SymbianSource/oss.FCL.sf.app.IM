/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Minimal resourcereader for reading resources without CONE

*                facilities

*
*/




#include <bautils.h>

#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR



#include "caminimalresourcereader.h"



// ======== MEMBER FUNCTIONS ========



// ---------------------------------------------------------------------------

// Constructor

// ---------------------------------------------------------------------------

//

CAMinimalResourceReader::CAMinimalResourceReader()

    {

    }



// ---------------------------------------------------------------------------

// CAMinimalResourceReader::ConstructL()

// ---------------------------------------------------------------------------

//

void CAMinimalResourceReader::ConstructL()

    {

    }





// ---------------------------------------------------------------------------

// CAMinimalResourceReader::NewL()

// ---------------------------------------------------------------------------

//

CAMinimalResourceReader* CAMinimalResourceReader::NewL()

    {

    CAMinimalResourceReader* self = CAMinimalResourceReader::NewLC();

    CleanupStack::Pop( self );

    return self;

    }





// ---------------------------------------------------------------------------

// CAMinimalResourceReader::NewLC()

// ---------------------------------------------------------------------------

//

CAMinimalResourceReader* CAMinimalResourceReader::NewLC()

    {

    CAMinimalResourceReader* self = new( ELeave ) CAMinimalResourceReader;

    CleanupStack::PushL( self );

    self->ConstructL();

    return self;

    }





// ---------------------------------------------------------------------------

// Destructor

// ---------------------------------------------------------------------------

//

CAMinimalResourceReader::~CAMinimalResourceReader()

    {

    Release();

    }





// -----------------------------------------------------------------------------

// CAMinimalResourceReader::ReadResourceLC()

// -----------------------------------------------------------------------------

//

HBufC* CAMinimalResourceReader::ReadTextResourceL(

    TInt aTextResourceId )

    {

    // Own resource reader for UIP plug-in environment

    // (no CONE facilities available)



    //resource texts are hold in TBUFs

    //TBUF == "A utility struct that holds one non-zero-terminated string."

    //==> no lead bytes in resource string



    TInt plainResourceId = 0x00000fff & aTextResourceId; // Remove offset from id

    HBufC8* rawDataBuf = iResourceFile.AllocReadLC( plainResourceId );



    //raw data buffer is actually unicode text ==> treat it so

    TPtrC16 rawData( ( TUint16* ) rawDataBuf->Ptr(),

                     rawDataBuf->Length() / 2 );



    HBufC16* resourceData = rawData.AllocL();

    CleanupStack::PopAndDestroy( rawDataBuf ); //rawDataBuf



    return resourceData;

    }



// -----------------------------------------------------------------------------

// CAMinimalResourceReader::OpenResourceFileLC()

// -----------------------------------------------------------------------------

//

void CAMinimalResourceReader::OpenResourceFileL(

    TDesC& aResourceName )

    {

    TFileName resourceFileName;

    iRFs.Close();

    User::LeaveIfError( iRFs.Connect() );

    NearestForCurrentLanguage( resourceFileName, aResourceName  );

    iResourceFile.OpenL( iRFs, resourceFileName );

    iResourceFile.ConfirmSignatureL();

    }



// -----------------------------------------------------------------------------

// CAMinimalResourceReader::Release()

// -----------------------------------------------------------------------------

//

void CAMinimalResourceReader::Release()

    {

    iRFs.Close();

    iResourceFile.Close();

    }



// -----------------------------------------------------------------------------

// CAMinimalResourceReader::NearestForCurrentLanguage()

// -----------------------------------------------------------------------------

//

void CAMinimalResourceReader::NearestForCurrentLanguage(

    TFileName& aName, const TDesC& aResourceFile )

    {

    aName.Zero();

        {

        //artificial variable scope to ease the stack usage

        TFileName drivePath;

        Dll::FileName( drivePath );

        aName.Append( TParsePtrC( drivePath ).Drive() );

        aName.Append( KDC_RESOURCE_FILES_DIR() );

        aName.Append( aResourceFile );

        }



    if ( aName.Length() > 0 )

        {

        // when baflutils gets an empty string, it returns "C:",

        // which breaks things

        BaflUtils::NearestLanguageFile( iRFs, aName );

        }

    }



// End of file

