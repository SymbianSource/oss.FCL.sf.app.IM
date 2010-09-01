/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ConnectionUi resources.
*
*/

#ifndef __CNUIRESOURCEFILENAME_H
#define __CNUIRESOURCEFILENAME_H

// INCLUDE FILES
#include <E32Std.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

// CONSTANTS

/**
 * Connection UI resource file name.
 */
_LIT( KCnUiResourceFileNameAndPath, "IMPSConnectionUiNG.rsc" );
_LIT( KCnUiVarResourceFileNameAndPath, "IMConnectionUIVariationNG.rsc" );



/**
 * Connection UI resource file name handler.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CnUiResourceFileName )
    {
public: //new methods

    /**
     * Composes nearest language resource file for connection UI:
     *
     * @since 2.1
     * @param aFs The file server session to use.
     * @param aName The file name buffer. On the function return
     * will contain a proper resource file name for
     * Connection UI.
     */
    static inline void NearestForCurrentLanguage( const RFs& aFs, TFileName& aName );

    /**
     * Composes nearest language resource file for connection UI:
     *
     * @since 3.2
     * @param aFs The file server session to use.
     * @param aName The file name buffer. On the function return
     * will contain a proper resource file name for
     * Connection UI.
     */
    static inline void NearestVariationForCurrentLanguage( const RFs& aFs, TFileName& aName );




protected: //prohibited constructor / destructor
    ~CnUiResourceFileName();
    CnUiResourceFileName();
    };

// Inline methods
#include "CnUiResourceFileName.inl"

#endif  //__CNUIRESOURCEFILENAME_H

//  End of File

