/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Concrete implementation of res.file& bitmap changing feature.
*
*/



// INCLUDE FILES
#include    "CCAOpBrandVariant20.h"
#include	"CAlternator.h"
#include    <coemain.h>
#include	<f32file.h>
#include	<eikenv.h>
#include	<CWVSAPSettingsStore.h>
#include	<CWVSAPSettings.h>


// CONSTANTS
_LIT( KChatAppModuleIdForBranding, "IMApp" );
_LIT( KChatEngModuleIdForBranding, "IMEng" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAOpBrandVariant20::CCAOpBrandVariant20
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAOpBrandVariant20::CCAOpBrandVariant20()
    {
    }

// -----------------------------------------------------------------------------
// CCAOpBrandVariant20::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAOpBrandVariant20::ConstructL()
    {
    iAlternator = CAlternator::InstanceL();
    }

// -----------------------------------------------------------------------------
// CCAOpBrandVariant20::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAOpBrandVariant20* CCAOpBrandVariant20::NewL()
    {
    CCAOpBrandVariant20* self = new( ELeave ) CCAOpBrandVariant20;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CCAOpBrandVariant20::~CCAOpBrandVariant20()
    {
    iCoeEnv->DeleteResourceFile( iAppResFileOffset );
    iCoeEnv->DeleteResourceFile( iEngResFileOffset );

    if ( iOrigCommUiResource.Length() > 0 ) // added 31.7.03 by kikangas to support 2.1 release
        {
        iCoeEnv->DeleteResourceFile( iCommUiResFileOffset );
        }

    delete iServerName;
    }

void CCAOpBrandVariant20::RemoveAppResourceL( CCoeEnv* aCoeEnv, TInt& aAppResFileOffset )
    {
    aCoeEnv->DeleteResourceFile( aAppResFileOffset );
    aAppResFileOffset = NULL;
    }

void CCAOpBrandVariant20::RetrieveSavedSettingsL( CCoeEnv* aCoeEnv, const TDesC& aOrigAppResPath,
                                                  const TDesC& aOrigEngResPath,
                                                  const TDesC& aOrigCommUiResPath,
                                                  TDes& aBitmapPath )
    {
    iCoeEnv = aCoeEnv;

    // store original paths
    iOrigAppResource = aOrigAppResPath;
    iOrigEngResource = aOrigEngResPath;
    iOrigCommUiResource = aOrigCommUiResPath;
    iOrigBitmapPath = aBitmapPath;

    CWVSAPSettingsStore* sapStore = CWVSAPSettingsStore::NewL();
    CleanupStack::PushL( sapStore );

    // if there are no SAPs then don't try the get default
    if ( sapStore->SAPCountL() > 0 )
        {
        CWVSAPSettings* sap = CWVSAPSettings::NewLC();
        sapStore->GetDefaultL( *sap );
        HBufC* server = sap->SAPName().AllocLC();

        ActivateBrandL( *server, aBitmapPath );
        CleanupStack::PopAndDestroy( 2 ); // server, sap
        }
    else
        {
        // loads original resource files
        TBuf<1> empty;
        ActivateBrandL( empty, aBitmapPath );
        }
    CleanupStack::PopAndDestroy(); // sapStore
    }

void CCAOpBrandVariant20::ActivateBrandL( const TDesC& aServer, TDes& aBitmapPath )
    {
#ifdef _DEBUG
    RDebug::Print( _L( "iAlternator->ChangeResourceL" ) );
#endif

    delete iServerName;
    iServerName = NULL;
    iServerName = aServer.AllocL();

    // APPLICATION RESOURCE
    TParsePtrC parseApp( iOrigAppResource );

    // if ok, removes the previous resource and adds new one
    TInt err = iAlternator->ChangeResourceL( KChatAppModuleId, aServer, parseApp.NameAndExt(), iAppResFileOffset );
    // if not ok
    if ( err )
        {
        // delete the old one
        CEikonEnv::Static()->DeleteResourceFile( iAppResFileOffset );
        // load original app resource
        // Code scanner warning Id 53 "not using
        // BaflUtils::NearestLanguageFile() when loading a resource file"
        // is not applicable to this case.
        iAppResFileOffset = CEikonEnv::Static()->AddResourceFileL(
                                iOrigAppResource ); // CSI: 53 # See above
        }


    // ENGINE RESOURCE

    TParsePtrC parseEng( iOrigEngResource );

    // if ok, removes the previous resource and adds new one
    err = iAlternator->ChangeResourceL( KChatAppModuleId, aServer, parseEng.NameAndExt(), iEngResFileOffset );
    // if not ok
    if ( err )
        {
        // delete the old one
        CEikonEnv::Static()->DeleteResourceFile( iEngResFileOffset );
        // load original eng resource
        // Code scanner warning Id 53 "not using
        // BaflUtils::NearestLanguageFile() when loading a resource file"
        // is not applicable to this case.
        iEngResFileOffset = CEikonEnv::Static()->AddResourceFileL(
                                iOrigEngResource ); // CSI: 53 # See above
        }

    // COMMON UI  RESOURCE
    if ( iOrigCommUiResource.Length() > 0 )
        {
        TParsePtrC parseCUI( iOrigCommUiResource );

        // if ok, removes the previous resource and adds new one
        err = iAlternator->ChangeResourceL( KChatAppModuleId, aServer, parseCUI.NameAndExt(), iCommUiResFileOffset );
        // if not ok
        if ( err )
            {
            // delete the old one
            CEikonEnv::Static()->DeleteResourceFile( iCommUiResFileOffset );
            // load original eng resource
            // Code scanner warning Id 53 "not using
            // BaflUtils::NearestLanguageFile() when loading a resource file"
            // is not applicable to this case.
            iCommUiResFileOffset = CEikonEnv::Static()->AddResourceFileL(
                                       iOrigCommUiResource ); // CSI: 53 # See above
            }
        }


    // BITMAP
    TParsePtrC parseBitmap( iOrigBitmapPath );
    if ( iAlternator->ChangeBitmapFileL( KChatAppModuleId, aServer, parseBitmap.NameAndExt() ) == KErrNone )
        {
        // set branded bitmap
        aBitmapPath.Zero();
        aBitmapPath.Append( iAlternator->CurrentBitmapFile( KChatAppModuleId, aServer, parseBitmap.NameAndExt() ) );
        }
    else
        {
        // else set to app defaults!!
        aBitmapPath.Zero();
        aBitmapPath.Append( iOrigBitmapPath );

        }
    }

void CCAOpBrandVariant20::DeleteResource(  CCoeEnv* aCoeEnv )
    {
    aCoeEnv->DeleteResourceFile( iAppResFileOffset );
    }

TBool CCAOpBrandVariant20::BrandBitmapsActivated() const
    {

    TParsePtrC parseBitmap( iOrigBitmapPath );
    TPtrC origFileName( parseBitmap.NameAndExt() );

    // returns zero length path if brand bitmap path is not set
    if ( iAlternator->CurrentBitmapFile( KChatAppModuleId, *iServerName, origFileName ).Length() == 0 )
        {
        return EFalse;
        }

    return ETrue;
    }
void CCAOpBrandVariant20::CurrentResourceFileL( const TDesC& /*aServer*/, const TDesC& /*aResourceFile*/, TDes& /*aFullPath*/ )
    {
    }

//  End of File
