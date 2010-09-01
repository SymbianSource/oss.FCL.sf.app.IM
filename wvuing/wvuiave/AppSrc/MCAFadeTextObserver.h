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
* Description:  Interface class for fadetext in navipane
*
*/



#ifndef MCAFADETEXTOBSERVER_H
#define MCAFADETEXTOBSERVER_H

// CLASS DECLARATION

/**
*  Interface class for fadetext in navipane
*
*  @lib chat.app
*  @since 1.2
*/
class MCAFadeTextObserver
    {
    public:

        /**
         * This method notifies when complete
         */
        virtual void NotifyWhenCompleted() = 0;

    protected:

        /**
         * DEstructor
         */
        virtual ~MCAFadeTextObserver() {};

    };

#endif	//MCAFADETEXTOBSERVER_H

//end of file

