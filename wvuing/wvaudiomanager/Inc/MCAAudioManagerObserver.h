/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface class for observe audiomanager playing
*
*/



#ifndef MCAAUDIOMANAGEROBSERVER_H
#define MCAAUDIOMANAGEROBSERVER_H

// CLASS DECLARATION

/**
*  Interface class for fadetext in navipane
*
*  @lib chat.app
*  @since 2.6
*/
class MCAAudioManagerObserver
    {
    public:

        /**
         * This method notifies when playing is completed
         */
        virtual void PlayCompleted() = 0;

    protected:

        /**
         * Destructor
         */
        virtual ~MCAAudioManagerObserver() {};

    };

#endif	// MCAAUDIOMANAGEROBSERVER_H

//end of file

