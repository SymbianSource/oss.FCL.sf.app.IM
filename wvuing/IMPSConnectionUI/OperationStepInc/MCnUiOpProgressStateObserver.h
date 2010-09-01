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
* Description:  Operation progress state observer.
*
*/

#ifndef __MCNUIOPPROGRESSSTATEOBSERVER_H
#define __MCNUIOPPROGRESSSTATEOBSERVER_H

//  INCLUDES
#include <E32Std.h>
#include <impspresenceconnectionuiconstsng.h>



//DATA TYPES
/**
 * Operation progress state observer.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiOpProgressStateObserver )
    {
public: // New event handling methods

    /**
     * Handling method called to handle
     * operaion progress state enter events.
     *
     * @since 2.1
     * @param aStateId State id from concrete
     * observed operation.
     * @param aClient The client for which the state enter happened.
     */
    virtual void HandleProgressStateEnter( TInt aStateId,
                                           TIMPSConnectionClient aClient ) = 0;



protected:  //Destructor

    /**
     * Inline destructor.
     *
     * Destruction using this
     * interface isn't possible.
     */
    virtual ~MCnUiOpProgressStateObserver() {};
    };


#endif      //  __MCNUIOPPROGRESSSTATEOBSERVER_H

//  End of File

