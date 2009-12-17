/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Inline methods for CnUiResourceFileName
*
*/



// ================= HELPER CLASS =======================
/**
 * Helper inline class
 *
 * @since 2.8
 */
inline void CnUiResourceFileName::NearestForCurrentLanguage( const RFs& aFs, TFileName& aName )
	{
	aName.Zero();
		{
		//artificial variable scope to ease the stack usage
		TFileName drivePath;
		Dll::FileName( drivePath );
		aName.Append( TParsePtrC( drivePath ).Drive() );
		aName.Append( KDC_RESOURCE_FILES_DIR() );
		aName.Append( KCnUiResourceFileNameAndPath );
		}

    if ( aName.Length() > 0 )
        {
        // when baflutils gets an empty string, it returns "C:",
        // which breaks things
        BaflUtils::NearestLanguageFile( aFs, aName );
        }
    }

/**
 * Helper inline class
 *
 * @since 3.2
 */

inline void CnUiResourceFileName::NearestVariationForCurrentLanguage( const RFs& aFs, TFileName& aName )
	{
	aName.Zero();
		{
		//artificial variable scope to ease the stack usage
		TFileName drivePath;
		Dll::FileName( drivePath );
		aName.Append( TParsePtrC( drivePath ).Drive() );
		aName.Append( KDC_RESOURCE_FILES_DIR() );
		aName.Append( KCnUiVarResourceFileNameAndPath );
		}

    if ( aName.Length() > 0 )
        {
        // when baflutils gets an empty string, it returns "C:",
        // which breaks things
        BaflUtils::NearestLanguageFile( aFs, aName );
        }
    }



//  End of File

