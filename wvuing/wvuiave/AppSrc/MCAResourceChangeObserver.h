/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Classes that want events from resource changes implement this interface.
*
*/



#ifndef MCARESOURCECHANGEOBSERVER_H
#define MCARESOURCECHANGEOBSERVER_H



// CLASS DECLARATION

/**
*  Interface class for observing resource changes.
*  @lib chat.app
*  @since S60 3.0
*/
class MCAResourceChangeObserver
    {

    public:  // New functions

        /*
        * Called when resource changes
        */
        virtual void ResourceChangedL() = 0;

    protected:

        virtual ~MCAResourceChangeObserver() {};

    };

#endif      // MCARESOURCECHANGEOBSERVER_H

// End of File
