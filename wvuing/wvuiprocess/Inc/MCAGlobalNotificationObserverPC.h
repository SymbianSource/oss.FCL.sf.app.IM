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
* Description:  Global interface for the UI to get notifications from the engine,
*				 storage etc, when theres a change in contact, list, invite,
*				 settings, conversation, group etc
*
*/




#ifndef MCAGLOBALNOTIFICATIONOBSERVERMANAGERPC_H
#define MCAGLOBALNOTIFICATIONOBSERVERMANAGERPC_H

#include <e32std.h>

class MCAUiGlobalNotificationObserverPC;
// CLASS DECLARATION

/**
 *  Interface for observer
 *
 *  @lib wvuiprocessng.dll
 *  @since 3.2
 */

class MCAGlobalNotificationObserverPC
    {
    public://new functions, called from ccasyncchecker

        /**
        * Has the mcacontactlists sync been done
        */
        virtual TBool IsContactListsSyncDoneL() = 0;

        /*
        * Is the mcacontactlist interface valid. If not logged in, it is invalid
        */
        virtual TBool IsValidListInterfaceL() = 0;

        /*
        * Is the storage synchronised
        */
        virtual TBool StoredContactsIsAllSynchronised( TBool& aFailed ) = 0;

        /*
        * Is the mcastoredcontacts interface valid.If not logged in, it is invalid
        */
        virtual TBool IsValidStoredContactsInterface() = 0;

    public:
        /**
        * Add the UI class as an observer for call backs
        * aGlobalObserver: reference to the UI class which implements
        * the MCAGlobalNotificationObserverPC interface
        */
        virtual void AddGlobalObserverL( MCAUiGlobalNotificationObserverPC* aGlobalObserver ) = 0;

        /**
        * Add the UI class as an observer for call backs
        * aGlobalObserver: reference to the UI class which implements
        * the MCAGlobalNotificationObserverPC interface
        */
        virtual void RemoveGlobalObserver( MCAUiGlobalNotificationObserverPC* aGlobalObserver )	= 0;

        /**
         * Virtual destructor
         */
        virtual ~MCAGlobalNotificationObserverPC() {};
    };

#endif //MCAGLOBALNOTIFICATIONOBSERVERPC_H