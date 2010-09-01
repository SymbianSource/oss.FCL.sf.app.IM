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
* Description: Requester for Imps timer.
*
*
*/


#ifndef IMPSTIMINGREQUESTER_H
#define IMPSTIMINGREQUESTER_H

// CLASS DECLARATION

/**
* This class only exists in order to be able to
* "label" the classes, which make use of the
* CImpsTransportTimer services. Classes, which
* instantiate the timer class, must derive from
* this abstraction so that, in case the timer expires,
* the callback interface (MImpsTransportTimerCallback)
* is able to find out who issued the activation of the
* timer in the first place. See the class CHttpTransportAdapter
* and the function "HandleTransportTimerEventL" for an example
* of how this is supposed to work.
*/
class TImpsTimingRequester
    {
    public:  //None

    private:  //None

    protected:  //None
    };

#endif      // IMPSTRANSPORTTIMERCALLBACK_H

// End of File
