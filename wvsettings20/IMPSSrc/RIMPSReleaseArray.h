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
* Description:  Array utility class for cleanup management in central
				 repository transactions.
*
*/


#ifndef __RIMPSRELEASEARRAY_H
#define __RIMPSRELEASEARRAY_H


//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Extension for RPointerArray to include
*  owned object cleanup in close.
*
*  @lib WVSAPSettingsStore.dll
*  @since 3.0
*/
NONSHARABLE_CLASS( RIMPSReleaseArray ): public RPointerArray< HBufC >
    {
public: // New functions

    /**
    * Overwritten function to include owned object cleanup in close.
    * @since 3.0
    */

    void Close();
    };

#endif	// __RIMPSRELEASEARRAY_H

