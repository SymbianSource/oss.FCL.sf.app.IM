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
* Description:  Logger implementation
*
*/

#ifndef __PENGLOGGER_H__
#define __PENGLOGGER_H__

//  INCLUDES
#include <E32Std.h>



/**
 * Presence Engine logger.
 *
 * @since 3.0
 */
class PEngLogger
    {
    public: //Logging functions


        /**
         * Writes a line to log.
         */
        IMPORT_C static void WriteLog( TRefByValue<const TDesC> aFmt, ... );



    private: //Prohibited construtor / destructor
        PEngLogger();
        ~PEngLogger();
    };


#endif  //__PENGLOGGER_H__

// End of File
