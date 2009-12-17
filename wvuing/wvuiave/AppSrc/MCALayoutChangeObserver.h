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
* Description:  Classes that want events from layout changes implement this interface.
*
*/



#ifndef MCALAYOUTCHANGEOBSERVER_H
#define MCALAYOUTCHANGEOBSERVER_H

#include <e32base.h>

// CLASS DECLARATION

/**
*  Interface class for observing layout changes.
*  @lib chat.app
*  @since 2.0
*/
class MCALayoutChangeObserver
    {

    public:  // New functions

        /*
        * Called when layout changes
        */
        virtual void LayoutChangedL( TInt aType = 0 ) = 0;

    protected:

        virtual ~MCALayoutChangeObserver() {};

    };

#endif      // MCALAYOUTCHANGEOBSERVER_H

// End of File
