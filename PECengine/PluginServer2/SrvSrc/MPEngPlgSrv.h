/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  See class description below.
*
*/


#ifndef __MPENGPLGSRV_H__
#define __MPENGPLGSRV_H__

//  INCLUDES
#include <e32def.h>
#include "PEngPlgSrvCommon.h"

// FORWARD DECLARATIONS

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION

    Cache server class. Server class is responsible to create and free
	cached entries. Entries are stored in B-tree to ensure fast read and write
	operations.

-----------------------------------------------------------------------------
*/

class MPEngPlgSrv
    {
    public:		// New methods

        /*
         *	Calls CActiveScheduler::Start()
         */
        virtual void StartServer() = 0;

        /*
         *	Stops the server, actually just calls CActiveScheduler::Stop();
         */
        virtual void StopServer() = 0;

    public: //new methods

        virtual void SetStateL( TPEngPlgOnlineState aNewState ) = 0;

        virtual TInt PluginCount() const = 0;

        virtual TInt Plugin( TInt aIndex ) const = 0;

    public:		// Methods derived from CServer

        /*
         *	Called when new client is connecting, version number of the client is checked here.
         */
        virtual CSession2* NewSessionL( const TVersion& aVersion,
                                        const RMessage2& aMessage ) const = 0;

    protected:  //Destructor

        virtual ~MPEngPlgSrv() {};

    };

#endif      //  __MPENGPLGSRV_H__
