/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Icon loader
*
*/


#ifndef CCAICONLOADER_H
#define CCAICONLOADER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CColumnListBoxData;
class MCASkinVariant;
class CGulIcon;
class CAknTabGroup;
class CCAAppUi;

// CLASS DECLARATION

/**
*  Class for loading icons.
*
*  @lib chat.app
*  @since 2.1
*/
class CCAIconLoader : public CBase
    {
    public:	// Constructors and destructors

        /**
         * Two-phased constructor
         * @param aMbmFullPath path for bitmaps
         * @param aSkinVariant skinvariant.
         * @since 2.1
         * @return Pointer to created CCAIConLoader object.
         */
        static CCAIconLoader* NewL( const TDesC& aMbmFullPath,
                                    MCASkinVariant& aSkinVariant );

        /**
         * Constructor
         * @param aMbmFullPath path for bitmaps
         * @param aSkinVariant skinvariant.
         * @since 2.1
         */
        CCAIconLoader( const TDesC& aMbmFullPath,
                       MCASkinVariant& aSkinVariant );

    public:		// New functions

        /**
         * Load all needed icons
         * @param aViewIdentifier for view identification.
         * @since 2.1
         * @return Array of icons stacked in one TCleanupItem
         */
        CArrayPtr<CGulIcon>* LoadIconsLC( TUid aViewIdentifier );

        /**
         * Load status pane icons
         * @param aTabGroup Tab group where to add icons.
         * @since 2.1
         */
        void LoadStatusPaneIconsL( CAknTabGroup& aTabGroup );

    private:	// Data

        // Path for bitmaps.
        const TDesC& iMbmFullPath;

        // skin variant
        MCASkinVariant& iSkinVariant;

        // Our AppUI, not owned
        CCAAppUi* iAppUi;
    };

#endif      // CAICONLOADER_H

// End of File
