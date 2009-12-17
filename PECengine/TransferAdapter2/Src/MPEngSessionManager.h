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
* Description: This class manage the network session
*
*/

#ifndef __MPENGSESSIONMANAGER_H
#define __MPENGSESSIONMANAGER_H


//  INCLUDES
#include <E32Std.h>

// DATA TYPES



// FORWARD DECLARATIONS


// CLASS DECLARATION
/**
 *  Access handler of one network session
 *
 *  It is used to open/close network session
 *  and register its observers
 *
 *  @since 3.0
 */
class MPEngSessionManager
    {

    public: //

        /**
         *  Close Access handler reference, deleted it
         *
         *  @since 3.0
         *
         *  @param aHandler to be closed
         */
        virtual void CloseAccessHandler() = 0;

        /**
         *  Close Access handler
         *
         *  @since 3.0
         *  @param aHandler handler to be closed
         */
        virtual void ClosePureDataHandler() = 0;


        /**
         * Open reference
         * @since 3.0
         */
        virtual void OpenRef() = 0;

    protected:  //Destructor

        /**
         *  Virtual inline destructor.
         *  Protected destructor to prohibits deletion trough interface.
         */
        virtual ~MPEngSessionManager() {};
    };


#endif      //  __MPEngSessionManager_H

//  End of File



