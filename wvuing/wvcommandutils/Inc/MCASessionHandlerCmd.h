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
* Description:  Interface for Command Session Handler for managing observers
*			   : MCAContactFetchObserver,MCAServiceStateObserver and to get some flag values
*			   : for if the fetching is done or not. Note: Interface can be further extended
*
*/


#ifndef MCASESSIONHANDLERCMD_H
#define MCASESSIONHANDLERCMD_H


// FORWARD DECLARATIONS
class MCAContactFetchObserver;
class MCAServiceStateObserver;


// CLASS DECLARATION

class MCASessionHandlerCmd
    {
    public:
        /**
         * Add given observer to ConatctFetch observer list
         * @param aObserver to add
         * @return none
         */
        virtual void AddContactFetchObserverL( MCAContactFetchObserver* aObserver ) = 0;

        /**
         * Remove given observer from observer list
         * @param aObserver observer to remove
         * @return none
         */
        virtual void RemoveContactFetchObserver( MCAContactFetchObserver* aObserver ) = 0;

        /**
         * Add given observer to ServiceState observer list
         * @param aObserver to add
         * @return none
         */
        virtual void AddServiceStateObserversL( MCAServiceStateObserver* aObserver ) = 0;

        /**
         * Remove given observer from observer list
         * @param aObserver observer to remove
         * @return none
         */
        virtual void RemoveServiceStateObservers( MCAServiceStateObserver* aObserver ) = 0;

        /**
         * Check if contact fetching is done
         * @return ETrue if done, EFalse otherwise
         */
        virtual TBool IsFetchDone() = 0;

        /**
         * @return ETrue if the contact list has been refreshed
         *         during this session
         */
        virtual TBool ContactsRefreshed() const = 0;

        /**
         * Gets the IsAlreadyLoggedOut flag value
         * @return ETrue if Already Logged Out
         */
        virtual TBool IsAlreadyLoggedOut() const = 0;

        /**
         * Tells if user is logged in or not
         * @return ETrue if logged in else EFalse
         */
        virtual TBool IsLoggedIn() const = 0;


        /**
        * Sets the contact refreshed flag
        * @param aRefreshed ETrue if the contact list has been refreshed
        */
        virtual void SetContactsRefreshed( TBool aRefreshed ) = 0;


        /**
         * Checks if background login is running.
         * @return ETrue if background login is running.
         */
        virtual TBool IsBackgroundTaskRunning() const = 0;
    };

#endif      // MCASESSIONHANDLERCMD_H

// End of File
