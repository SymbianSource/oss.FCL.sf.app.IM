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
* Description: Callback function for Tcp CirWatcher.
*
*
*/


#ifndef __TCPCIRWATCHERTIMERCALLBACK_H__
#define __TCPCIRWATCHERTIMERCALLBACK_H__

// CLASS DECLARATION

/**
*  A callback class for the timer class of IMPS Transport layer
*/
class MTcpCirWatcherTimerCallback
    {
    public:

        /**
        * Pure virtual callback function to be implemented in the
        * derived class. This function is called when timer has compeleted.
        * @param aRequester The object that starts the timer
        * @param aStatus The integer value of iStatus of the timer class
        */
        virtual void HandleTimerEventL( const TUint aExpiredRequest ) = 0;
    };

#endif      // __IMPSCIRWATCHERTIMERCALLBACK_H__

// End of File
