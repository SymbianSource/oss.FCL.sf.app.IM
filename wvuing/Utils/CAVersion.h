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
* Description:  Version provider for IM.
*
*/


#ifndef __CAVERSION_H__
#define __CAVERSION_H__

#include <e32base.h>

//	FORWARD CLASS DECLERATIONS

// CLASS DECLARATION
class	RFs;

/**
 *  Utils for IM modules.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class CAVersion // CSI: 51 # This is not a C-class although it begins with CA
    {
    public: // new methods

        /**
         * Copies the content of aOwnVersionFile to aTergetFilem
         * if they are the same
         * @param aFs
         * @param aOwnVersionFile, give only file name. Path will be
         *                         retrieved from aFs and PathInfo.
         * @param aTergetFile, give file name with path without
         *                     drive letter. Drive will be retrieved
         *                     from PathInfo.
         * @return KErrNone on success or system wide error code
         * @since 2.8
         */
        static TInt UpdateVersion( RFs& aFs
                                   , const TDesC& aOwnVersionFile
                                   , const TDesC& aTergetFile );

    };

#endif      // __CAVERSION_H__

// End of File
