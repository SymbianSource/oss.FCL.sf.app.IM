/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for search observer
*
*/


#ifndef MCASEARCHOBSERVERINTERFACEPC_H
#define MCASEARCHOBSERVERINTERFACEPC_H

//INCLUDES
#include <e32std.h>


// CLASS DECLARATION

/**
 *  Search observer interface to the UI.
 *  Signalling goes from the Engine -> UI.
 *
 *  @lib wvuiprocessng.lib
 *  @since 2.1
 */
class MCASearchObserverInterfacePC
    {
    public:

        /**
         * Error handling function for the UI side.
         * @param aErrorCode defines the error
         */
        virtual void HandleSearchError( TInt aErrorCode ) = 0;

        /**
         * The pending search has been finished.
         */
        virtual void HandleSearchFinished() = 0;

    protected:
        /**
         * Destructor.
         */
        virtual ~MCASearchObserverInterfacePC() {};
    };

#endif      // MCASEARCHOBSERVERINTERFACEPC_H

// End of File
