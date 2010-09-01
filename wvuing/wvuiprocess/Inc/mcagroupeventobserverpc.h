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
* Description:  Interface for group event observer
*
*/


#ifndef MCAGROUPEVENTOBSERVERPC_H
#define MCAGROUPEVENTOBSERVERPC_H

//INCLUDES
#include <e32std.h>


// CLASS DECLARATION

/**
 *  Group Event observer interface to the UI.
 *  Signalling goes from the Engine -> UI.
 *
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class MCAGroupEventObserverPC
    {
    public:

        /**
         * Called when new group properties are received from the
         * the network. These are server initiated properties.
         */
        virtual void HandleGroupPropertiesChanged() = 0;

        /**
         * Called when the group has been deleted, or forced kick out of the group.
         * @param aReason The CSP error code
         */
        virtual void HandleGroupLeft( TInt aReason ) = 0;


    protected:
        /**
         * Destructor.
         */
        virtual ~MCAGroupEventObserverPC() {};
    };

#endif      // MCAGROUPEVENTOBSERVERPC_H

// End of File
