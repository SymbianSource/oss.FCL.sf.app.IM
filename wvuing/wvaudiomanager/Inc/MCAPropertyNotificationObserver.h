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
* Description:  Property notification handler observer interface
*
*/



#ifndef MCAPROPERTYNOTIFICATIONOBSERVER_H
#define MCAPROPERTYNOTIFICATIONOBSERVER_H

#include <e32base.h>

// CLASS DECLARATION

/**
*  Interface for receiving notifications from CCAPropertyObserver when the
*  value of the observed property changes
*
*  @lib
*  @since 3.0
*/
NONSHARABLE_CLASS( MCAPropertyNotificationObserver )
    {
public: // New functions

    /**
    * Called when the value of observed property has changed
    * @since 2.6
    */
    virtual void HandlePropertyNotificationEventL( TUid aCategory, TUint aKey ) = 0;

    };

#endif      // MCAPROPERTYNOTIFICATIONOBSERVER_H

// End of File
