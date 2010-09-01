/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Utils for im launcher log.
*
*/



#ifndef IMAPI_APIUTILS_H
#define IMAPI_APIUTILS_H

//  INCLUDES
#include <E32STD.H>
#include <flogger.h>


/**
*  Debug log writer
*
*  @lib
*  @since S60 2.6
*/
class CImApiLogger : public CBase
    {
    public: // New functions

        /**
        * Writes log to log file
        * @since S60 2.6
        * @param
        * @return
        */
        static void Log( TRefByValue<const TDesC> aFmt, ... );
        virtual ~CImApiLogger();

    };

#endif      // IMAPI_APIUTILS_H   

// End of File
