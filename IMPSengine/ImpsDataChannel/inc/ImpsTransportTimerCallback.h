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
* Description: Callback function for Transport Timer.
*
*
*/


#ifndef IMPSTRANSPORTTIMERCALLBACK_H
#define IMPSTRANSPORTTIMERCALLBACK_H

#include "ImpsTimingRequester.h"

// CLASS DECLARATION

/**
*  A callback class for the timer class of IMPS Transport layer
*/
class MImpsTransportTimerCallback
    {
    public:

        /**
        * Pure virtual callback function to be implemented in the
        * derived class. This function is called when timer has
        * compeleted.
        * @param aStatus The integer value of iStatus of the timer class
        */
        virtual void HandleTransportTimerEventL( TImpsTimingRequester* iRequester,
                                                 TInt aStatus ) = 0;
    };

#endif      // IMPSTRANSPORTTIMERCALLBACK_H

// End of File
