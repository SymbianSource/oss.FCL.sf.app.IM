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
* Description:  Waitnote observer.
*
*/

#ifndef __MCNUIWAITNOTEOBSERVER_H
#define __MCNUIWAITNOTEOBSERVER_H


//  INCLUDES
#include <e32base.h>


/**
 * Waitnote event observer for MCnUiUiFacade.
 * Observers waitnote cancel events.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiWaitNoteObserver )
    {
public: // Handle events

    /**
     * Notify method for waitnote cancel events.
     * @since 2.1
     */
    virtual void HandleWaitNoteCancel() = 0;


protected:  //Destructor

    /**
     * Virtual inline destructor.
     * Observer can't be destructed using this interface.
     *
     * @since 2.1
     */
    virtual ~MCnUiWaitNoteObserver() {};

    };

#endif  //  __MCNUIWAITNOTEOBSERVER_H


//  End of File

