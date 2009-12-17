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
* Description:  Variation flag getters of the storage
*
*/



#ifndef MCASTORAGEINFO_H
#define MCASTORAGEINFO_H


//#include <e32std.h>

// CLASS DECLARATION

/**
 *  Contains info about the state of the storage module
 *
 *  @lib CAStorage.dll
 *  @since 3.0
 */
class MCAStorageInfo
    {
    public: // Interface

        /**
         * Tells if we should show the nickname of the contact.
         * (It can be brand variated so that the nickname usage
         * in whole is not supported.)
         * @return ETrue if nickname should be shown, EFalse otherwise
         */
        virtual TBool ShowNickname() = 0;

        /**
         * Information of collapse/expande state for contact lists. If locked
         * contact list cannot be collapsed/expanded. If not then contact list
         * can be collapsed/expanded.
         * @return ETrue if collapse/expande is locked, EFalse otherwise.
         */
        virtual TBool CollapseLocked() = 0;
    };

#endif      // MCASTORAGEINFO_H

// End of File
