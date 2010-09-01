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
* Description: this is a factory class
*
*/

#ifndef __MPENGTRANSADAPFACTORY_H
#define __MPENGTRANSADAPFACTORY_H


//  INCLUDES
#include <E32Std.h>

// DATA TYPES



// FORWARD DECLARATIONS
class CPEngSessionManager;

// CLASS DECLARATION
/**
 *  Access handler of one network session
 *
 *  It is used to open/close network session
 *  and register its observers
 *
 *  @since 3.0
 */
class MPEngTransAdapFactory
    {

    public: //

        /**
         *  Add session reference
         *
         *  @since 3.0
         */
        virtual void OpenSessionCount() = 0;

        /**
         *  Close session manager
         *  If Transaction adapter factory is not needed it is deleted
         *
         *  @since 3.0
         *  @param aSessManager session manager
         */
        virtual void CloseSession( CPEngSessionManager* aSessManager ) = 0;

    protected:  //Destructor

        /**
         *  Virtual inline destructor.
         *  Protected destructor to prohibits deletion trough interface.
         */
        virtual ~MPEngTransAdapFactory() {};
    };


#endif      //  __MPEngTransAdapFactory_H

//  End of File



