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
* Description:  Used for testing and connection of the Admin sub clients
*
*/


#ifndef __RPENGSTORAGEADMINCLIENT_H__
#define __RPENGSTORAGEADMINCLIENT_H__

//  INCLUDES
#include <e32std.h>

/**
 *  Storage Admin client
 *  It is used for connecting of the admin sub-clients
 *
 *  @lib PEngStoreMan2.dll
 *  @since 3.0
 */

// CLASS DECLARATION
class  RPEngStorageAdminClient: public RSessionBase
    {

    public:		// Constructors and destructors
        /**
         * Destructor.
         */
        IMPORT_C ~RPEngStorageAdminClient();

        /**
         * C++ default constructor.
         */
        IMPORT_C RPEngStorageAdminClient();

    public: 	// new methods

        /**
         * Connects to the presence server
         *
         *	@since 3.0
         *	@return KErrNone if connected
         *			or any of system wide error codes
         */
        IMPORT_C TInt Connect();

        /**
         *	Close connection to the presence server
         *
         *	@since 3.0
         */
        IMPORT_C void Close();

    public: // testing support

        /**
         *  Clean Session slot out of the Storage
         *	intended for testing, since session is wiped with brute force
         *
         *  @since 3.0
         *  @param aSessionId packed session slot id to wipe out
         *  @return KErrNone if operation went fine, or system wide
         *			error code
         */
        IMPORT_C TInt WipeSession( const TDesC8& aSessionId );

    private:	// New functions

        /**
         * Get version
          * @since 3.0
         * @return version of the client
         */
        TVersion Version() const;

    private:	// Data

        /// Flag if client is connected to the Storage Server
        TBool					iConnected ;

    };

#endif      //  __RPENGSTORAGEADMINCLIENT_H__





















