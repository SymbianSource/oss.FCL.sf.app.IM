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
* Description: 
*     WV engine Push Handler interface.
*
*/


#ifndef RIMPSWATCHCLI_H
#define RIMPSWATCHCLI_H

// INCLUDES
#include <e32base.h>
#include "impsclient.h"

// CLASS DECLARATION

/**
*  RImpsWatchClient API
*  This class is used internally by the Push Handler 
*  This class is a light weight version of the other classes.
*  This API doesn't start the WV engine if it is not running,
*  the incoming CIR is ingnored in that case.
*  There are no observer methods since we don't need to inform the push handler
*
*/

class RImpsWatchClient : public RImpsEng
    {
    public:

        /**
        * C++ default constructor.
        */      
        IMPORT_C RImpsWatchClient();

    public:

        /**
        * Connects to the WV server
        * @return KErrNone if connections was successfull
        */
        IMPORT_C TInt Register( );

        /**
        * Sends the CIR to the WV server
        * @param aCookie CIR in form of WVCI <version> <sessionid>
        */

        IMPORT_C void SendCIR( const TDesC8& aCookie );

        /**
        * Closes the session with the WV Server
        */
        IMPORT_C void UnRegister( );

        /**
        * Since RImpsWatchClient is inherited from RImpsClient we must implement
        * this pure virtual function.
        * The implementation of this function is empty
        * @return KErrNone 
        */
        TInt DoRegister( CImpsHandler2* /*aHandler*/ );
    };

#endif 
// End of File
