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
* Description:  SAP settings central repository observer
*
*/

#ifndef __MIMPSSAPOBSERVER_H
#define __MIMPSSAPOBSERVER_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION
/**
 * Active central repository observer.
 *
 * @since 3.0
 */
class MIMPSSAPObserver
    {
    public:     //enums
        enum TSAPEvent  ///< cenrep events reported to the observer.
            {
            ESAPSettingChanged = 1   ///< SAP setting has changed.
            };

    public:     // new methods

        /**
         * Method called after cenrep event.
         * Observers are also notified from client itself originated
         * events.
         *
         * @param aEvent Event enumeration.
         *
         * @since 3.0
         */
        virtual void HandleSAPEvent( TSAPEvent aEvent ) = 0;

    protected:  //destructor
        /**
         * Protected destructor.
         * Concrete observers can't be destroyed through
         * observer interface.
         */
        virtual ~MIMPSSAPObserver() {};

    };

#endif      // __MIMPSACTIVEDBOBSERVER_H


//  End of File
