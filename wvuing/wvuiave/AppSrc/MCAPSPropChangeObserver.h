/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Publish and Subscribe property change observer
*
*/



#ifndef MCAPSPROPCHANGEOBSERVER_H
#define MCAPSPROPCHANGEOBSERVER_H

//  INCLUDES
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  Observer for publish and subscribe values
*
*  @lib chat.lib
*  @since Series 60 2.6
*/
class  MCAPSPropChangeObserver
    {
    public:
        /**
        * Handles property changes
        * @since Series 60 2.6
        */
        virtual void HandlePropertyChanged() = 0;
    protected:
        virtual ~MCAPSPropChangeObserver() {};
    };

#endif      // MCAPSPROPCHANGEOBSERVER_H

// End of File
