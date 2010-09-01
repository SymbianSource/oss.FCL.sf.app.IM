/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Generic thread safe server starter.
*
*/

#ifndef __PENGSERVERSTARTER_H__
#define __PENGSERVERSTARTER_H__

//  INCLUDES
#include <E32Std.h>



// CLASS DECLARATION
/**
* Generic thread safe server starter used to start server
* process from given exe/dll file.
*
* WINS environment requirements:
* - target must be normal polymorphic dll
* - 1st exported function must be castable to TThreadFunction
* - Thread function parameter is TPEngServerParams*.
*
*
* THUMB environment requirements:
* - target must be normal executable, epocexe
* - if started using this launcher, RProcess command line
*   is TPEngServerParams contents. If exe is started by other
*   means, e.g. with file browser, then no TPEngServerParams contents
*   available in RProcess command line.
*
*  @since 3.0
*/
class PEngServerStarter
    {
    public:     // New methods

        /**
         * Launch server process
         *
         * @since 3.0
         * @param aServerExeBaseName Plain server executable base name.
         *        Server path and drive are determined on the runtime
         *        from client dll location & compile time platsec
         *        definitions.
         * @param aServerName The name for the server.
         * @param aParams is optional parameter to supply to server process.
         * @return KErrNone if the server succesfully started or
         * if its already running. Else one of system wide error codes.
         */
        IMPORT_C static TInt LaunchServer( const TDesC& aServerExeBaseName,
                                           const TDesC& aServerName,
                                           TInt aParam1 = KErrNone,
                                           TInt aParam2 = KErrNone );


        /**
         * Connects to server process and starts new server if needed.
         * (Starting done with PEngServerStarter::LaunchServer() )
         *
         *
         * @since 3.0
         * @param aSession Session to open.
         * @param aServerName The server to connect.
         *                    Given to RSessionBase::CreateSession().
         * @param aVersion The required server version.
         *                    Given to RSessionBase::CreateSession().
         * @param aAsyncMessageSlots Amount of needed async slots.
         *                    Given to RSessionBase::CreateSession().
         * @param aServerExeBaseName The server exe to start if not yest running,
         * as in PEngServerStarter::LaunchServer()
         * @param aParam1 & 2 The extra parameters to provide to launched server
         * process as in PEngServerStarter::LaunchServer()
         *
         * @return Error code from RSessionBase::CreateSession() or
         * PEngServerStarter::LaunchServer(). If given session is already
         * opened (it's handle number is something else than KNullHandle)
         *  returns KErrInUse.
         */
        IMPORT_C static TInt ConnectServer( RSessionBase& aSession,
                                            const TDesC& aServerName,
                                            const TVersion& aVersion,
                                            TInt aAsyncMessageSlots,
                                            const TDesC& aServerExeBaseName,
                                            TInt aParam1 = KErrNone,
                                            TInt aParam2 = KErrNone  );


    private:    // Methods not implemented

        PEngServerStarter( const PEngServerStarter& );
        PEngServerStarter& operator=( const PEngServerStarter& );
    };

#endif      //  __PENGSERVERSTARTER_H__


//End of file