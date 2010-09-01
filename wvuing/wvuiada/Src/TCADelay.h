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
* Description: Utility class which used in logging adapter for counting time delay when calling functions
*
*/



#ifndef TCADELAY_H
#define TCADELAY_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Utility class.
*  Used in logging adapter for counting time delay when calling functions.
*  Start time is registered in construction.
*
*  @lib CAAdapter.dll
*  @since 1.2
*/
class TCADelay
    {
    public:  // Constructors and destructor

        TCADelay();

    public: // New functions

        /**
        * Calculates difference between construction and a call to this function.
        * @since 1.2
        * @return TInt difference in microseconds
        */
        TInt StopAndReturn( );

    private:    // Data

        // Start time
        TTime iStart;

        // Stop time
        TTime iStop;

    };

#endif      // TCADELAY_H

// End of File
