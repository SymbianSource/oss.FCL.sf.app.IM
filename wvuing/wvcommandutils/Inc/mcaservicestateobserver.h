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
* Description:  Observer interface to get events from service state changes.
*
*/


#ifndef MCASERVICESTATEOBSERVER_H__
#define MCASERVICESTATEOBSERVER_H__


// CLASS DECLARATION

/**
 *  Observer interface to get events from service state changes.
 *
 *  @lib Chat.exe
 *  @since S60 v3.1
 */
class MCAServiceStateObserver
    {
    public:

        // Possible service states
        enum TServiceState
            {
            EUnknown = 0,
            EOffline,
            EOnline
            };

        /**
         * Handle service state event
         * @param aState Current service state
         */
        virtual void HandleServiceStateChangeL( TServiceState aState ) = 0;

    protected: // For protection.

        /**
         * Destructor for protection
         */
        virtual ~MCAServiceStateObserver() {}
    };

#endif      // MCASERVICESTATEOBSERVER_H__

// End of File
