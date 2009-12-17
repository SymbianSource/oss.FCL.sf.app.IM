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
* Description: Handler timer
* 
*/


#ifndef IMPSHANDLERTIMER_H
#define IMPSHANDLERTIMER_H

//  INCLUDES
#include "impshandler.h"

// FORWARD DECLARATIONS
class CImpsHandler2;

// CLASS DECLARATION


/**
* CImpsHandlerTimer
* 
*/
class CImpsHandlerTimer : public CActive
    {
    public:
        /**
        * Constructor
        * @param aServer WV engine server
        * @param aPriority Active objects priority
        */
        CImpsHandlerTimer( TInt aPriority, CImpsHandler2&  aHandler );

        /**
        * Destructor
        */
        virtual ~CImpsHandlerTimer();

        /**
        * Start waiting.
        * Use CActive::Cancel() to cancel the request.
        */
        void Start( );

        
    protected:
        // From base class
        void RunL();
        void DoCancel();

    protected:
        RTimer          iTimer;
        CImpsHandler2&  iHandler;    
    };



#endif      // IMPSHANDLERTIMER_H
            
// End of File


