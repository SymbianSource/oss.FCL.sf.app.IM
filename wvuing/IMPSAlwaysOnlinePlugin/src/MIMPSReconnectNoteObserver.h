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
* Description:  This interface is used to inform changes in global reconnection
*                note
*
*/


#ifndef __MIMPSRECONNECTNOTEOBSERVER_H
#define __MIMPSRECONNECTNOTEOBSERVER_H

#include <e32std.h>

class MIMPSReconnectNoteObserver
    {
    public:

        /**
        * This call is made when user has answered to global note
        * @param TInt aAnswer result for answer
        */
        virtual void HandleNoteAnswered( TInt aAnswer ) = 0;

        /**
         * This call Is made when note showing ins canceled for some reason
         */
        virtual void HandleNoteCancelled() = 0;

        /**
        * Virtual destructor
        */
        virtual ~MIMPSReconnectNoteObserver() {};
    };

#endif // __MPENGAOCALLOBSERVER_H
