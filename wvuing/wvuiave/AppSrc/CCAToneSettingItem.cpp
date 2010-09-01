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
* Description:  IM Alert Tone setting item
*
*/



// INCLUDE FILES
#include    "CCAToneSettingItem.h"
#include    "IMDialogUtils.h"
#include    "imutils.h"
#include    "imprivatecrkeys.h"
#include    "imvariant.hrh"

#include    <CFLDFileListContainer.h>
#include    <stringloader.h>
#include	<chatNG.rsg>
#include    <data_caging_path_literals.hrh>
#include    <centralrepository.h>
#include    <mediafilelist.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"


// LOCAL CONSTANTS AND MACROS
namespace
    {
    // CONSTANTS
    _LIT( KIgnoredExtension, ".RNG" );
    _LIT( KProfileSilentTone,     "Z:No_Sound.wav" );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAToneSettingItem::CCAToneSettingItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAToneSettingItem::CCAToneSettingItem( TInt aIdentifier, TDes& aToneName,
                                        CRepository& aCenRep, TBool aBranding ):
        CAknSettingItem( aIdentifier ),
        iExternalToneFileName( aToneName ),
        iBranding( aBranding ),
        iCenRep( aCenRep )
    {
    }

// Destructor
CCAToneSettingItem::~CCAToneSettingItem()
    {
    delete iNoSoundText;
    delete iNoSoundFileName;
    delete iTitle;
    }

// -----------------------------------------------------------------------------
// CCAToneSettingItem::StoreL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAToneSettingItem::StoreL()
    {
    if ( iInternalToneFileName.Length() == 0 )
        {
        // use the default sound file
        DefaultSoundFile( iInternalToneFileName );
        }
    iExternalToneFileName.Copy( iInternalToneFileName.Left( iExternalToneFileName.MaxLength() ) );
    }

// -----------------------------------------------------------------------------
// CCAToneSettingItem::LoadL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAToneSettingItem::LoadL()
    {
    iInternalToneFileName.Copy( iExternalToneFileName.Left( iInternalToneFileName.MaxLength() ) );
    if ( iInternalToneFileName.Length() == 0 )
        {
        // use the default sound file
        DefaultSoundFile( iInternalToneFileName );
        }
    }

// -----------------------------------------------------------------------------
// CCAToneSettingItem::EditItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAToneSettingItem::EditItemL( TBool /* aCalledFromMenu */ )
    {
    // BrandaleToneList
    CFLDFileListContainer* fl;
    
    // New mediafilelist API
    CMediaFileList* mfl;
    TInt nullItem = KErrNotFound;
    
    // Add "Off" as first item in tone selection list.
    // Launch File List Dialog
    HBufC* currentTone = iInternalToneFileName.AllocLC();
    TBool cancel( EFalse );
    TBool valid( EFalse );
    while ( !cancel && !valid )
        {

#ifdef RD_30_COMPATIBILITY_MODE
        // S60 3.0
        if ( !iBranding )
            {
            fl = CFLDFileListContainer::NewLC();
            fl->InsertNullItemL( *iNoSoundText, *iNoSoundFileName );
            }
        else
            {
            fl = CFLDFileListContainer::NewLC( R_AUDIO_FILELIST_MODEL, ETrue );
            fl->InsertNullItemL( *iNoSoundText, *iNoSoundFileName );
            }

#else // S60 COMMON

        // Branding functionality depends on CR variation (iBrandableToneList)
        if ( iBranding && iBrandableToneList )
            {
            // Use branded tone list
            fl = CFLDFileListContainer::NewLC( R_AUDIO_CLF_LIST_MODEL,
                                               R_AUDIO_FLDIRECTORIES );
            fl->InsertNullItemL( *iNoSoundText, *iNoSoundFileName );
            }
        else
            {
            // Use default tone list when iBrandableToneList is EFalse
            mfl = CMediaFileList::NewL();
            CleanupStack::PushL( mfl );
            mfl->SetAttrL( CMediaFileList::EAttrTitle, iTitle );
            mfl->SetNullItemL(
            		*iNoSoundText, 
            		*iNoSoundFileName,
            		CMediaFileList::EMediaFileTypeAudio,
                    CMediaFileList::ENullItemIconNone
                    );

            // Exclude the video folder
            mfl->SetAttrL( CMediaFileList::EAttrExcludeFolder, CMediaFileList::EMediaFileTypeVideo );
            }
#endif //RD_30_COMPATIBILITY_MODE

        // loop until we get cancel or valid tone file
        if ( iBranding && iBrandableToneList )
        	{
        	// launch the filelist
        	cancel = !fl->LaunchL( iInternalToneFileName, *iTitle );
        	}
        else
        	{
        	// launch the mediafilelist
        	cancel = !mfl->ShowMediaFileListL( &iInternalToneFileName, &nullItem, NULL, NULL );
        	}
        
        if ( !cancel && iInternalToneFileName.Length() > 0 )
            {
            // check validity (== is it DRM protected)
            valid = !IMUtils::FileProtectedL( iInternalToneFileName );

            if ( !valid )
                {
                HBufC* text = CCoeEnv::Static()->AllocReadResourceLC(
                                  R_QTN_CHAT_SETTINGS_TONE_INVALID );
                IMDialogUtils::DisplayErrorConfirmationNoteL( *text );
                CleanupStack::PopAndDestroy( text );
                
                if ( iBranding && iBrandableToneList )
                	{
                if ( fl )
                    {
                    //delete it as it is constructed again.
                    CleanupStack::PopAndDestroy( fl );
                		}
                	}
                else
                	{
                	if ( mfl )
                		{
                		//delete it as it is constructed again.
                		CleanupStack::PopAndDestroy( mfl );
                		}
                	}
                }
            else
                {
                if ( iBranding && iBrandableToneList )
                	{
                	CleanupStack::PopAndDestroy( fl );
                	}
                else
                	{
                	CleanupStack::PopAndDestroy( mfl );
                	}
                }
            }
        else
            {
            // no tone => valid
            valid = ETrue;
            if ( iBranding && iBrandableToneList )
            	{
            	CleanupStack::PopAndDestroy( fl );
            	}
            else
            	{
            	CleanupStack::PopAndDestroy( mfl );
            	}
            }
        }

    if ( cancel )
        {
        // cancelled, reset filename
        iInternalToneFileName.Copy( ( *currentTone ).Left( iInternalToneFileName.MaxLength() ) );
        }

    CleanupStack::PopAndDestroy( currentTone ); // currentTone, fl
    UpdateListBoxTextL();

    // trick for notifying the appsetting dialog of cancel.
    if ( cancel )
        {
        User::Leave( KErrCancel );
        }
    }

// -----------------------------------------------------------------------------
// CCAToneSettingItem::SettingTextL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CCAToneSettingItem::SettingTextL()
    {
    if ( iInternalToneFileName.Length() > 0 )
        {
        // FLD has returned a filename
        if ( iInternalToneFileName.CompareF( *iNoSoundFileName ) == 0 )
            {
            // Filename is "No_Sound", return localized text "None"
            iSettingText.Set( *iNoSoundText );
            }
        else
            {
            // Filename is something else
            TParsePtrC parse( iInternalToneFileName );
            if ( parse.Ext().CompareF( KIgnoredExtension ) == 0 )
                {
                iSettingText.Set( parse.Name() );
                }
            else
                {
                iSettingText.Set( parse.NameAndExt() );
                }
            }
        }
    else
        {
        // Filename is an empty descriptor, return localised text "None"
        iSettingText.Set( *iNoSoundText );
        }
    return iSettingText;
    }

// -----------------------------------------------------------------------------
// CCAToneSettingItem::CompleteConstructionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAToneSettingItem::CompleteConstructionL()
    {
    iNoSoundText =
        StringLoader::LoadL( R_CHAT_AUDIO_TEXT_NONE );

    TFileName fileName;
    TParse* fp = new ( ELeave ) TParse();
    fp->Set( KProfileSilentTone, &KDC_RESOURCE_FILES_DIR, NULL );
    fileName.Copy( fp->FullName().Left( fileName.MaxLength() ) );
    delete fp;
    iNoSoundFileName = fileName.AllocL();
    iTitle = StringLoader::LoadL( R_CHAT_AUDIO_TEXT_SELECT_TONE );

    // Read CR variation flag
    iBrandableToneList = ( IMUtils::CRKeyL( KCRUidIMVariation, KIMVariationKey )
                           & EIMFeatBrandableTones );
    }

// ---------------------------------------------------------
// CCAToneSettingItem::DefaultSoundFile
// Attention: This same function exists also in CCAAudioManager,
// because IMUtils cannot be used there. So if changing here, change also
// there.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAToneSettingItem::DefaultSoundFile( TDes& aSoundFileName )
    {
    TInt err = KErrNone;
    TBool overRide = EFalse;
    TRAP( err, overRide =
              IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_OVERRIDE_DEFAULT_TONE ) )

    if ( overRide && !err )
        {
        // Get overridden default tone from brand package
        HBufC* temp = NULL;
        TRAP( err, temp = StringLoader::LoadL( R_DEFAULT_TONE ) );
        if ( temp )
            {
            aSoundFileName = *temp;
            delete temp;
            }
        }
    else if ( !err )
        {
        // Get default tone from CenRep
        err = iCenRep.Get( KIMAlertTone, aSoundFileName );
        }

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

//  End of File
