/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Signalling for UI components about group events
*
*/


#ifndef MCAGROUPEVENTOBSERVER_H
#define MCAGROUPEVENTOBSERVER_H

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
 *  MCAGroupEventObserver interface class. Provides signalling for the UI
 *  components.  As the name states mainly changes in the group states are
 *  reported to the UI.
 *  Used to observe events of one group.
 *
 *  @lib CAEngine.lib
 *  @since 2.1
 */
class MCAGroupEventObserver
    {
    public:  // Interface

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
        virtual ~MCAGroupEventObserver() {};
    };

#endif  // MCAGROUPEVENTOBSERVER_H

// End of File
