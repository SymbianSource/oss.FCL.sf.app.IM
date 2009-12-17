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
* Description: Header file for imps util class
* 
*/


#ifndef IMPSUTILS_H
#define IMPSUTILS_H

#include "impsconst.h"
#include "impsservercommon.h"
#include <f32file.h>
#include <s32file.h>

// FORWARD DECLARATION
class CImpsVariant;

// Logging function
// This should not be here but no time to create a new file just now.

// Client logger
class CImpsClientLogger : public CBase
    {
    public:
        IMPORT_C static void Log( TRefByValue<const TDesC> aFmt, ... );
        virtual ~CImpsClientLogger();
    };


//**********************************
// CImpsSettingFile
//**********************************
// Read key-value pairs from ASCII file
class CImpsSettingFile: public CBase
    {
    public:
        /**
        * Constructor.
        */
        IMPORT_C static CImpsSettingFile* NewL(
            RFs& aFs );

        IMPORT_C ~CImpsSettingFile();

        /**
        * Opens a file. It is closed in destructor!
        */
        IMPORT_C void OpenL( TDesC& aResFile );

        /**
        * Get the key value. Leave with KErrNotFound if not found.
        */
        IMPORT_C TPtrC8 KeyValueL( const TDesC8& aKey );

    private:
        CImpsSettingFile( RFs& aFs );

        void ReadRowL( TPtrC8& aKey, TPtrC8& aValue );
    private:
        // Data
        RFs& iFs;
        RFileReadStream iReader;
        TBuf8<128> iRowBuffer;
        TFileName iFileName;
        TBool     iOpen;

    };


//**********************************
// message service type
//**********************************
IMPORT_C GLDEF_C TImpsEventType impsService( const CImpsVariant* aVariant,
                                             TImpsMessageType aMessage );

#endif      // ?INCLUDE_H   

// End of File
