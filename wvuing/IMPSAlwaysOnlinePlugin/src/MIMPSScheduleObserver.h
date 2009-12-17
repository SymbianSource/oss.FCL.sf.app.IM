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
* Description:  This interface is used to inform about the changes in sheduled
*				 login/logout status.
*
*/


#ifndef __MIMPSSCHEDULEOBSERVER_H
#define __MIMPSSCHEDULEOBSERVER_H

#include <e32std.h>

class MIMPSScheduleObserver
    {
    public:

        /**
        * This call is made when the scheduled login status is checked
        * @param aIsScheduledLoginOk tells if client should be logged in
        *
        */
        virtual void HandleScheduleEvent( TBool aIsScheduledLoginOk ) = 0;

        /**
        * Virtual destructor
        */
        virtual ~MIMPSScheduleObserver() {};
    };

#endif // __MIMPSSCHEDULEOBSERVER_H
