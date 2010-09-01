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


#ifndef __CPENGPLGSRV_H__
#define __CPENGPLGSRV_H__

//  INCLUDES
#include "PEngPlgSrvCommon.h"
#include <e32def.h>
#include <s32std.h>
#include <s32btree.h>
#include <e32svr.h>
#include <f32file.h>
#include <badesca.h>
#include "MPEngPlgSrv.h"

// FORWARD DECLARATIONS
class TPEngServerParams;
class CPEngPluginHolder;

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION

    Cache server class. Server class is responsible to create and free
	cached entries. Entries are stored in B-tree to ensure fast read and write
	operations.

-----------------------------------------------------------------------------
*/


class CPEngPlgSrv : public CPolicyServer,
            public MPEngPlgSrv
    {

    public:		// Constructors and destructors

        static CPEngPlgSrv* NewLC( const TDesC& aServerName, TInt aPriority );	// Static constructor
        virtual ~CPEngPlgSrv();				// Destructor (virtual)

    protected:	// Constructors and destructors

        CPEngPlgSrv( TInt aPriority );						// Default constructor, protected to allow derivation
        void ConstructL( const TDesC& aServerName );		// Second phase construct

    public:		// New methods

        /*
         * Starts the server, called from clients Connect() method or from WinsMain()
         */
        static TInt ExecuteServerL( TPEngServerParams& aParams );
        /*
         *	Stops the server, actually just calls CActiveScheduler::Stop();
         */
        void StopServer();

        /*
         * Starts the server
         */
        static void RunServerL( TPEngServerParams& aParams );

    public: //new methods

        void SetStateL( TPEngPlgOnlineState aNewState );

        void LoadByTypeL( const TDesC8& aPluginType );
        void UnloadOnlinePluginsL();
        void UnloadAllL();

        TInt PluginCount() const;
        TInt Plugin( TInt aIndex ) const;

    protected: //new methods

        /*
         *	Calls CActiveScheduler::Start()
         */
        void StartServer();

    public:		// Methods derived from CPolicyServer

        /*
         *	Called when new client is connecting, version number of the client is checked here.
         */
        CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;

    private:	// Methods not implemented

        CPEngPlgSrv( const CPEngPlgSrv& );		// Copy constructor
        CPEngPlgSrv& operator=( const CPEngPlgSrv& );	// Assigment operator

    private:	// Data

        // array of plugins, OWNED
        RPointerArray<CPEngPluginHolder> iPlugins;

        // server name holder
        TFullName iServerName;

        // online state of the server
        TPEngPlgOnlineState iOnlineState;
    };

#endif      //  __CPENGPLGSRV_H__

