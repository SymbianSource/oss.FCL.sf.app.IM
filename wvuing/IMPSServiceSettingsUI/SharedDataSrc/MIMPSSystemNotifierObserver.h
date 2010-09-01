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
* Description:  System key notification handler observer interface
*
*/



#ifndef MIMPSSYSTEMNOTIFIEROBSERVER_H
#define MIMPSSYSTEMNOTIFIEROBSERVER_H

#include "MIMPSSystemNotifier.h"

// CLASS DECLARATION

/**
*  Interface for receiving notifications from CIMPSSystemObserver when the
*  value of the observed key changes
*
*  @lib
*  @since 2.6
*/
NONSHARABLE_CLASS( MIMPSSystemNotifierObserver )
    {
public: // New functions

    /**
    * Called when the value of observed key has set
    * @since 2.6
    */
    virtual void HandleSystemNotifierEventL( TUid aKey, TInt aValue ) = 0;
    };

#endif      // MIMPSSYSTEMNOTIFICATIONOBSERVER_H

// End of File
