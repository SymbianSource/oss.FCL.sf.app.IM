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
* Description:  Notifier interface for system events
*
*/

#ifndef __MIMPSSYSTEMNOTIFIER_H
#define __MIMPSSYSTEMNOTIFIER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  System events handler
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.6
*/
NONSHARABLE_CLASS( MIMPSSystemNotifier )
    {
public: // New functions

    /**
    * Subscribes to listen for set of a key, which to notifier is bound
    * @param aKey       Keyword of which changes are to be observed.
    * @return error code
    */
    virtual TInt Subscribe() = 0;

    /**
    * Unsubscribes a request to listen for change of a key, which to notifier is bound
    * @param aKey       Keyword of which changes are to be observed.
    * @return error code
    */
    virtual void UnSubscribe() = 0;

    /**
    * Gets the integer value corresponding the key,  which to notifier is bound
    * @param aKey keyword
    * @param aValue integer value associated with the keyword
    * @return error code
    */
    virtual TInt GetIntKey( TInt& aValue ) = 0;

    /**
    * Destructor
    */
    virtual ~MIMPSSystemNotifier() {};

    };

#endif      // __MIMPSSYSTEMNOTIFIER_H

// End of File
