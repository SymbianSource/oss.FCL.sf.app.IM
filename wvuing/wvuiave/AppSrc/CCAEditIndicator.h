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
* Description: Editor indicator controller
*
*/


#ifndef CCAEDITINDICATOR_H
#define CCAEDITINDICATOR_H

//  INCLUDES
#include <e32def.h>
#include <e32std.h>
#include <e32base.h>


class CCAMessageEditor;


class CCAEditIndicator : public CActive
    {
    public:
        /**
         * @see CActive
         */
        void RunL();

        /**
         * @see CActive
         */
        void DoCancel();

    public:

        static CCAEditIndicator* NewL( CCAMessageEditor& aEditor );

        /**
         * @return ETrue - if the timer is already active
         */
        TBool IsIndicatorActive();

        /**
         * StartTimer
         * Initiates and spans the timer
         */
        void StartTimer();

        /**
         * Destructor
         *
         */
        ~CCAEditIndicator();

        /**
         * Constructor
         * @param - reference to the editor
         */
        CCAEditIndicator( CCAMessageEditor& aEditor );

    private :
        //doesnt own
        RTimer iTimer;

        //doesnt own
        CCAMessageEditor& iEditor;
    };

#endif      // CCAEDITINDICATOR_H

// End of File
