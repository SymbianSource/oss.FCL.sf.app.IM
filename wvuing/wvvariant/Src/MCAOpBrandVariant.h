/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface class for resource file& bitmap changing feature.
*
*/



#ifndef MCAOPBRANDVARIANT_H
#define MCAOPBRANDVARIANT_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
_LIT( KChatAppModuleId, "IMApp" );
_LIT( KChatEngModuleId, "IMEng" );

// FORWARD DECLARATIONS
class CCoeEnv;

// CLASS DECLARATION

/**
*  Interface class for resource file& bitmap changing feature.
*  @lib chat.app
*  @since 1.2s
*/
class MCAOpBrandVariant
    {

    public: // New functions

        /**
        * Removes application resource in CreateDocumentL method.
        * @since 1.2s
        * @param aCoeEnv, control environment.
        * @param aAppResFileOffset, resource file offset, kept by the client.
        */
        virtual void RemoveAppResourceL( CCoeEnv* aCoeEnv,
                                         TInt& aAppResFileOffset ) = 0;

        /**
        * Retrieves branded application resource file if there is one.
        * If not, the default
        * application resource file is loaded.
        * @since 1.2s
        * @param aCoeEnv, control environment.
        * @param aOrigAppResPath, original application resource path
        * @param aOrigEngResPath, original engine resource path
        * @param aOrigPresenceResPath, original presence resource path
        * @param aOrigCommUiResPath, original common ui resource path
        * @param aBitmapPath, bitmap path
        */
        virtual void RetrieveSavedSettingsL( CCoeEnv* aCoeEnv,
                                             const TDesC& aOrigAppResPath,
                                             const TDesC& aOrigEngResPath,
                                             const TDesC& aOrigPresenceResPath,
                                             const TDesC& aOrigVariationResPath,
                                             const TDesC& aOrigCommUiResPath,
                                             TDes& aBitmapPath ) = 0;
        /**
        * Activates a brand, changes application's and engine's resource files.
        * @since 1.2s
        * @param aServer server name
        * @param aBitmapPath
        */
        virtual void ActivateBrandL( const TDesC& aServer, TDes& aBitmapPath ) = 0;

        /**
        * Removes application's resource when exiting application.
        * @since 1.2s
        * @param aCoeEnv, control environment.
        */
        virtual void DeleteResource( CCoeEnv* aCoeEnv ) = 0;

        /**
        * Returns true if default graphics aren't used.
        * @since 1.2s
        * @return TBool, false if using default bitmaps.
        */
        virtual TBool BrandBitmapsActivatedL() const = 0;

        /**
        * Returns resource file path for certain resource.
        * @since 1.2s
        * @param aServer, server name
        * @param aResourceFile, resource file name
        * @param aFullPath, empty if not found, otherwise contains full path
        *                   to resource
        */
        virtual void CurrentResourceFileL( const TDesC& aServer, const TDesC& aResourceFile,
                                           TDes& aFullPath ) = 0;

    };

#endif      // MCAOPBRANDVARIANT_H

// End of File
