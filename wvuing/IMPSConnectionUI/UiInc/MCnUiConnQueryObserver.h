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
* Description:  Query observer.
*
*/

#ifndef __MCNUICONNQUERYOBSERVER_H
#define __MCNUICONNQUERYOBSERVER_H


//  INCLUDES
#include <e32base.h>


/**
 * Query observer for MCnUiUiFacade.
 * Observers events from non waiting query.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiConnQueryObserver )
    {
public: // Handle events

    /**
     * Notify method for confirmation query result.
     * @since 2.1
     */
    virtual void HandleQueryResult( TBool aQueryAccepted ) = 0;


protected:  //Destructor

    /**
     * Virtual inline destructor.
     * Observer can't be destructed using this interface.
     *
     * @since 2.1
     */
    virtual ~MCnUiConnQueryObserver() {};

    };

#endif  //  __MCNUICONNQUERYOBSERVER_H


//  End of File

