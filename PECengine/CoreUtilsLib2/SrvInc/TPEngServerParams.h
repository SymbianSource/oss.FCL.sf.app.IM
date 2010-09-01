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
* Description:  Server startup parameters.
*
*/

#ifndef __TPENGSERVERPARAMS_H__
#define __TPENGSERVERPARAMS_H__


//  INCLUDES
#include <E32Std.h>


// CLASS DECLARATION
/**
 * Server startup parameters.
 *
 * Container to pass startup params to launched server process.
 * This class is also used to signal client when the server
 * is fully started.
 *
 * @since 3.0
 */
class TPEngServerParams
    {
    public:

        /**
         * C++ constructor (for server side).
         *
         * @since 3.0
         */
        IMPORT_C TPEngServerParams();


        /**
         * C++ constructor (for client side).
         *
         * @since 3.0
         * @param aServerName The name for started server.
         * @param aParam1 External parameter 1.
         * @param aParam2 External parameter 2.
         */
        IMPORT_C TPEngServerParams( const TDesC& aServerName,
                                    TInt aParam1,
                                    TInt aParam2 );


    public: //New methods for client side


        /**
         * Gets a descriptor presenting TPEngServerParams.
         *
         * @since 3.0
         * @return Descriptor presentation of TPEngServerParams.
         */
        IMPORT_C TPtrC AsCommandLine() const;



    public: //New methods for server side

        /**
         * Initializes TPEngServerParams contents from
         * process command line.
         *
         * This should be called in server E32Main() or
         * similar place to get retrieve parameter contents
         * from command line.
         *
         * @since 3.0
         * @return KErrNone if process command line
         * contained TPEngServerParams. Else KErrNotFound.
         */
        IMPORT_C TInt InitFromCmdLine();



        /**
         * Changes server thread name to given one.
         * (In __WINS__ build the renaming isn't done, as
         * it would break the ServerStarter.)
         *
         * Server should call this method as early as possible
         * during its construction.
         *
         * @since 3.0
         * @param aThreadName The new thread name to use.
         */
        IMPORT_C void RenameMainThread( const TDesC& aThreadName );


        /**
         * Signals the client that server is successfully
         * started.
         *
         * Server should call this method when server it is
         * fully started.
         *
         * @since 3.0
         */
        IMPORT_C void Signal();


        /**
         * Gets the parameters as given from the client side.
         *
         * @since 3.0
         * @return Param value as given from client side.
         */
        IMPORT_C TInt Param1() const;
        IMPORT_C TInt Param2() const;


        /**
         * Gets the server name as given from the client side.
         *
         * @since 3.0
         * @return Server name as given from client side.
         */
        IMPORT_C const TDesC& ServerName() const;


    private:    // Methods not implemented

        TPEngServerParams( const TPEngServerParams& );
        TPEngServerParams& operator=( const TPEngServerParams& );


    private: //data

        //OWN: Server name to identify the started server
        TName           iServerName;

        //OWN: Extra parameters
        TInt            iParam1;
        TInt            iParam2;

    };


#endif      //  __TPENGSERVERPARAMS_H__

