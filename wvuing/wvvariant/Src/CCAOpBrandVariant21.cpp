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
* Description:  Concrete implementation of res.file& bitmap changing feature.
*
*/



// INCLUDE FILES
#include    "CCAOpBrandVariant21.h"
#include	"CAlternator.h"
#include    <coemain.h>
#include	<f32file.h>
#include	<eikenv.h>
#include	<CIMPSSAPSettingsStore.h>
#include	<CIMPSSAPSettings.h>

// CONSTANTS
_LIT( KChatAppModuleIdForBranding, "IMApp" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAOpBrandVariant21::CCAOpBrandVariant21
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAOpBrandVariant21::CCAOpBrandVariant21()
    {
    }

// -----------------------------------------------------------------------------
// CCAOpBrandVariant21::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAOpBrandVariant21::ConstructL()
    {


    TBuf< KMaxPath > brandRoot;

    //we try to find branding from C: and Z: only, no MMC!
    _LIT( KCDrive, "C:" );

    brandRoot.Zero();
    brandRoot.Append( KCDrive() );

    TBuf< KMaxPath > privatePath;
    CEikonEnv::Static()->FsSession().PrivatePath( privatePath );

    brandRoot.Append( privatePath );

    iAlternator = CAlternator::InstanceL( brandRoot );
    }

// -----------------------------------------------------------------------------
// CCAOpBrandVariant21::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAOpBrandVariant21* CCAOpBrandVariant21::NewL()
    {
    CCAOpBrandVariant21* self = new( ELeave ) CCAOpBrandVariant21;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCAOpBrandVariant21::~CCAOpBrandVariant21()
    {
    iCoeEnv->DeleteResourceFile( iAppResFileOffset );
    iCoeEnv->DeleteResourceFile( iEngResFileOffset );
    iCoeEnv->DeleteResourceFile( iPresenceResFileOffset );
    iCoeEnv->DeleteResourceFile( iVariationResFileOffset );

    // added 31.7.03 by kikangas to support 2.1 release
    if ( iOrigCommUiResource.Length() > 0 )
        {
        iCoeEnv->DeleteResourceFile( iCommUiResFileOffset );
        }
    }

void CCAOpBrandVariant21::RemoveAppResourceL( CCoeEnv* aCoeEnv,
                                              TInt& aAppResFileOffset )
    {
    aCoeEnv->DeleteResourceFile( aAppResFileOffset );
    aAppResFileOffset = NULL;
    }

void CCAOpBrandVariant21::RetrieveSavedSettingsL( CCoeEnv* aCoeEnv,
                                                  const TDesC& aOrigAppResPath,
                                                  const TDesC& aOrigEngResPath,
                                                  const TDesC& aOrigPresenceResPath,
                                                  const TDesC& aOrigVariationResPath,
                                                  const TDesC& aOrigCommUiResPath,
                                                  TDes& aBitmapPath )
    {
    iCoeEnv = aCoeEnv;

    // store original paths
    iOrigAppResource = aOrigAppResPath;
    iOrigEngResource = aOrigEngResPath;
    iOrigPresenceResource = aOrigPresenceResPath;
    iOrigVariationResource = aOrigVariationResPath;
    iOrigCommUiResource = aOrigCommUiResPath;
    iOrigBitmapPath = aBitmapPath;

    CIMPSSAPSettingsStore* sapStore = CIMPSSAPSettingsStore::NewL();
    CleanupStack::PushL( sapStore );

    // if there are no SAPs then don't try the get default
    if ( sapStore->SAPCountL( EIMPSIMAccessGroup ) > 0 )
        {
        CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();
        sapStore->GetDefaultL( sap, EIMPSIMAccessGroup );
        HBufC* server = sap->SAPName().AllocLC();

        ActivateBrandL( *server, aBitmapPath );
        CleanupStack::PopAndDestroy( 2, sap ); // server, sap
        }
    else
        {
        // loads original resource files
        TBuf<1> empty;
        ActivateBrandL( empty, aBitmapPath );
        }
    CleanupStack::PopAndDestroy( sapStore );
    }

void CCAOpBrandVariant21::ActivateBrandL( const TDesC& aServer, TDes& aBitmapPath )
    {
#ifdef _DEBUG
    RDebug::Print( _L( "iAlternator->ChangeResourceL" ) );
#endif
    // last tried server
    iServerName = aServer;

    // APPLICATION RESOURCE
    TParsePtrC parseApp( iOrigAppResource );
    // if ok, removes the previous resource and adds new one
    TInt err = iAlternator->ChangeResourceL( KChatAppModuleId,
                                             aServer,
                                             parseApp.NameAndExt(),
                                             iAppResFileOffset );
    // if not ok
    if ( err )
        {
        // delete the old one
        CEikonEnv::Static()->DeleteResourceFile( iAppResFileOffset );
        // load original app resource
        // Code scanner warning Id 53 "not using
        // BaflUtils::NearestLanguageFile() when loading a resource file"
        // is not applicable to this case.
        iAppResFileOffset =
            CEikonEnv::Static()->AddResourceFileL(
                iOrigAppResource ); // CSI: 53 # See above
        }


    // ENGINE RESOURCE
    TParsePtrC parseEng( iOrigEngResource );
    // if ok, removes the previous resource and adds new one
    err = iAlternator->ChangeResourceL( KChatAppModuleId,
                                        aServer,
                                        parseEng.NameAndExt(),
                                        iEngResFileOffset );
    // if not ok
    if ( err )
        {
        // delete the old one
        CEikonEnv::Static()->DeleteResourceFile( iEngResFileOffset );
        // load original eng resource
        // Code scanner warning Id 53 "not using
        // BaflUtils::NearestLanguageFile() when loading a resource file"
        // is not applicable to this case.
        iEngResFileOffset =
            CEikonEnv::Static()->AddResourceFileL(
                iOrigEngResource ); // CSI: 53 # See above

        }

    // Presence resource
    TParsePtrC parsePresence( iOrigPresenceResource );
    // if ok, removes the previous resource and adds new one
    err = iAlternator->ChangeResourceL( KChatAppModuleId,
                                        aServer,
                                        parsePresence.NameAndExt(),
                                        iPresenceResFileOffset );
    // if not ok
    if ( err )
        {
        // delete the old one
        CEikonEnv::Static()->DeleteResourceFile( iPresenceResFileOffset );
        // load original presence resource
        // Code scanner warning Id 53 "not using
        // BaflUtils::NearestLanguageFile() when loading a resource file"
        // is not applicable to this case.
        iPresenceResFileOffset =
            CEikonEnv::Static()->AddResourceFileL(
                iOrigPresenceResource ); // CSI: 53 # See above
        }

    // Variation resource
    TParsePtrC parseVariation( iOrigVariationResource );
    // if ok, removes the previous resource and adds new one
    err = iAlternator->ChangeResourceL( KChatAppModuleId,
                                        aServer,
                                        parseVariation.NameAndExt(),
                                        iVariationResFileOffset );
    // if not ok
    if ( err )
        {
        // delete the old one
        CEikonEnv::Static()->DeleteResourceFile( iVariationResFileOffset );
        // load original presence resource
        // Code scanner warning Id 53 "not using
        // BaflUtils::NearestLanguageFile() when loading a resource file"
        // is not applicable to this case.
        iVariationResFileOffset =
            CEikonEnv::Static()->AddResourceFileL(
                iOrigVariationResource ); // CSI: 53 # See above
        }

    // BITMAP
    _LIT( KMif, ".mif" );
    _LIT( KMbm, ".mbm" );
    TParsePtrC parseBitmap( iOrigBitmapPath );
    TFileName myFile;
    myFile.Zero();
    myFile.Append( parseBitmap.Name() );
    myFile.Append( KMif() );

    iBrandedBitmaps = EFalse;

    if ( iAlternator->ChangeBitmapFileL( KChatAppModuleId, aServer, myFile )
         == KErrNone )
        {
        // set branded bitmap
        aBitmapPath.Zero();
        aBitmapPath.Append( iAlternator->CurrentBitmapFileL( KChatAppModuleId,
                                                             aServer,
                                                             myFile ) );
        iBrandedBitmaps = ETrue;
        }
    else
        {
        //take 2 with mbm
        myFile.Zero();
        myFile.Append( parseBitmap.Name() );
        myFile.Append( KMbm() );

        if ( iAlternator->ChangeBitmapFileL( KChatAppModuleId, aServer, myFile )
             == KErrNone )
            {
            // set branded bitmap
            aBitmapPath.Zero();
            aBitmapPath.Append( iAlternator->CurrentBitmapFileL(
                                    KChatAppModuleId, aServer, myFile ) );
            iBrandedBitmaps = ETrue;
            }
        else
            {
            // else set to app defaults!!
            aBitmapPath.Zero();
            aBitmapPath.Append( iOrigBitmapPath );
            }
        }
    }

void CCAOpBrandVariant21::DeleteResource(  CCoeEnv* aCoeEnv )
    {
    aCoeEnv->DeleteResourceFile( iAppResFileOffset );
    }

TBool CCAOpBrandVariant21::BrandBitmapsActivatedL() const
    {
    return iBrandedBitmaps;
    }

void CCAOpBrandVariant21::CurrentResourceFileL( const TDesC& aServer,
                                                const TDesC& aResourceFile,
                                                TDes& aFullPath  )
    {
    iAlternator->GetCurrentResourceFileL( KChatAppModuleIdForBranding,
                                          aServer,
                                          aResourceFile,
                                          aFullPath );
    }

//  End of File
