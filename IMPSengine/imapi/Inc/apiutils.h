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
* Description: Utils for IM api.
*
*/



#ifndef IMAPI_APIUTILS_H
#define IMAPI_APIUTILS_H

//  INCLUDES
#include <E32STD.H>
#include <flogger.h>

// CONSTANTS
_LIT( KTxtImOpenApiPanic, "ImOpenApi" ); // Panic category
_LIT( KImOpenAppID, "OpenApiID" ); // The application id used if not found in setttings



// DATA TYPES
// Panic codes
enum TPanic
    {
    EListCorrupted = 0,
    EListNotEmpty = 1,
    ENullNotifyHandler = 2
    };
//////////////////////////////////////////////////////////////////////////

// CLASS DECLARATION
/**
*  Debug log writer
*
*  @since Series 60 3.0
*/
class CImApiLogger : public CBase
    {
    public: // New functions

        /**
        * Writes log to log file
        * @since S60 3.0
        * @param
        * @return
        */
        static void Log( TRefByValue<const TDesC> aFmt, ... );
        virtual ~CImApiLogger();

    };

// CLASS DECLARATION
/**
*  SAP settings reader class
*  Gets the Client ID from the settings
*
*  @since Series 60 3.0
*/

class SSapSettingsReader
    {
    public:
        // Gets the Client ID which was associated with the given SAP URL
        static const TDesC* ClientIdLC( const TDesC& aSapUrl );
    protected:
    private:
    };

#endif      // IMAPI_APIUTILS_H   

// End of File
