/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  logger implementation
*
*/

#ifndef __IMPSLOGGER_H__
#define __IMPSLOGGER_H__

//  INCLUDES
#include <E32Std.h>



/**
 * Imps Protocol logger.
 *
 * @since 3.2
 */
class IMPSLogger
    {
    public: //Logging functions


        /**
         * Writes a line to log.
         */
        //IMPORT_C static void WriteLog( TRefByValue<const TDesC> aFmt,... );
        static void WriteLog( TRefByValue<const TDesC> aFmt, ... );



    private: //Prohibited construtor / destructor
        IMPSLogger();
        ~IMPSLogger();
    };


#endif  //__IMPSLOGGER_H__

// End of File
