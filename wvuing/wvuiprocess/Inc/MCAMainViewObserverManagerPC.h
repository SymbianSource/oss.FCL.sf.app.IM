/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Main View Observer Manager Interface
*
*/
#ifndef MCAMAINVIEWOBSERVERMANAGERPC_H
#define MCAMAINVIEWOBSERVERMANAGERPC_H

class MCAMainViewObserverPC;
class MCASyncObserverPC;

class MCAMainViewObserverManagerPC
    {

    public:

        //events or callbacks to UI..

        /**
        * Adds observer, handles errors by leaving.
        * Adds observer, observer must point to valid
        * object. If aObserver is NULL, leaves with
        * @param aObserver The observer to add.
        */
        virtual void AddObserverL( MCAMainViewObserverPC* aObserver ) = 0;

        /**
         * RemoveObserver, remove from call back
         */
        virtual void RemoveObserver( MCAMainViewObserverPC* aObserver ) = 0;

        /**
         * Adds observer, observer must point to valid
         * @param aObserver The observer to add.
         */
        virtual void AddObserver( MCASyncObserverPC* aObserver ) = 0;

        /**
         * RemoveObserver, remove from call back
         */
        virtual void RemoveObserver( ) = 0;

        /**
         * ~MCAMainViewObserverManagerPC
         */
        virtual ~MCAMainViewObserverManagerPC() {};
    };

#endif //MCAMAINVIEWOBSERVERMANAGERPC_H