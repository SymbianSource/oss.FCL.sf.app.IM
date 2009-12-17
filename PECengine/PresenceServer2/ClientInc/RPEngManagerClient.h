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
* Description:  Presence server main client
*
*/


#ifndef __RPENGMANAGERCLIENT_H__
#define __RPENGMANAGERCLIENT_H__

//  INCLUDES
#include <e32def.h>
#include <e32base.h>
#include "PEngPresenceEngineConsts2.h"

// CLASS DECLARATION
/**
 *  Main client of the  Presence server
 *  This client is used to establise main session to the
 *  Presence server and then passed when new subsession needs to be
 *  connected
 *
 *  @lib PEngClient2
 *  @since 3.0
 */
class RPEngManagerClient : public RSessionBase
    {
    public:		// Constructors and destructors
        /**
         * Destructor.
         */
        IMPORT_C	~RPEngManagerClient();

        /**
         * C++ default constructor.
         */
        IMPORT_C	RPEngManagerClient();

    public:		// Connect close operations

        /**
          *  Openes new handle to the Presence server,
          *	 if server is not running it is started
          *
          *	 @since 3.0
          */
        IMPORT_C TInt Connect();

        /**
         *	Close connection to the Presence server
         *
         *	@since 3.0
         */
        IMPORT_C void Close();

    private:	// New methods

        /**
         *	Get version
         *
         *	@since 3.0
         *  @return version
         */
        TVersion Version() const;

    private:	// Data

        /// OWN: Flag if client is connected to server
        TBool			iConnected;
    };

#endif      //  __RPENGMANAGERCLIENT_H__
