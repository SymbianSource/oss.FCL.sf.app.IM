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
* Description:  Provides the log handling interface for the UI
*
*/


#ifndef MCASDCRVARIATOR_H
#define MCASDCRVARIATOR_H

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION
/**
 *  Variation class for Shared Data or Central Repository (persistent
 *  preference store).
 *  @lib CAEngine.lib
 *  @since 3.0
 */
class MCASDCRVariator
    {
    public:

        /**
         * Get integer from persistent preference store.
         * @param aSetting The setting key
         * @param aValue The value to which to get the integer
         * @return KErrNone if successful
         * @since 3.0
         */
        virtual TInt GetInt( TInt aSetting, TInt& aValue ) = 0;

        /**
         * Set integer to persistent preference store.
         * @param aSetting The setting key
         * @param aValue The value to which to set
         * @return KErrNone if successful
         * @since 3.0
         */
        virtual TInt SetInt( TInt aSetting, TInt aValue ) = 0;

        /**
         * Get string from persistent preference store.
         * @param aSetting The setting key
         * @param aValue The value to which to get the integer
         * @return KErrNone if successful
         * @since 3.0
         */
        virtual TInt GetString( TInt aSetting, TDes& aValue ) = 0;

        /**
         * Set string to persistent preference store.
         * @param aSetting The setting key
         * @param aValue The value to which to set
         * @return KErrNone if successful
         * @since 3.0
         */
        virtual TInt SetString( TInt aSetting, const TDesC& aValue ) = 0;

        /**
         * Flush the data to the persistent preference store
         * @return KErrNone if successful
         */
        virtual TInt FlushData() = 0;

        /**
         * Destructor.
         */
        virtual ~MCASDCRVariator() {};
    };

#endif      // MCASDCRVARIATOR_H

// End of File
