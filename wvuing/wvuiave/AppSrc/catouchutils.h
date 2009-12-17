/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Helper methods for touch support
*
*/


#ifndef CATOUCHUTILS_H
#define CATOUCHUTILS_H


/**
 *  Helper class for touch support
 *
 *  @lib chat.exe
 *  @since S60 v3.1
 */
// This is not a C-class although it begins with a CA
class CATouchUtils  // CSI: 51 #see above
    {
    public:

        /**
         * Variated version of AknLayoutUtils::PenEnabled()
         * Use this to preserver 3.0 compatibility
         * @return ETrue - Pen support on
         *         EFalse - Pen support off
         */
        static TBool PenEnabled();


    private:

        /**
         * C++ constructor
         */
        CATouchUtils();
    };


#endif // CATOUCHUTILS_H
