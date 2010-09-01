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
* Description: Contact DB reader class
*
*/



#ifndef IMAPI_CNTDBREADER_H
#define IMAPI_CNTDBREADER_H

//  INCLUDES
#include <E32BASE.H>
#include <badesca.h>
#include <cntdef.h>

// FORWARD DECLARATIONS
class CContactIdArray;
class CContactDatabase;
class CWVIDCache;

// CLASS DECLARATION

/**
*  Contact DB reader class
*  Reads the contact DB for WV user IDs
*
*  @lib imclient.dll
*  @since 3.0
*/
class CContactDBReader : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CContactDBReader* NewL();

        /**
        * Destructor.
        */
        virtual ~CContactDBReader();

    public: // New functions

        /**
        * Fetches the WV IDs for the given Contact Model IDs
        * @since S60 3.0
        * @param aContactArray List of the Contact Model IDs
        * @param aWVUserIDs List of the corresponding WV IDs
        */
        void ReadWVIdsL(
            const CContactIdArray& aContactArray,
            CDesCArray& aWVUserIds );

        /**
        * Fetches the WV ID for the given Contact Model ID
        * If WV ID not present then leaves with KErrNotFound
        * @since S60 3.0
        * @param aContactArray List of the Contact Model IDs
        * @param aWVUserIDs List of the corresponding WV IDs
        */
        void GetWVIdL(
            const TContactItemId& aConatctId,
            CDesCArray& aWVUserIds );


    private:

        /**
        * C++ default constructor.
        */
        CContactDBReader();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        CContactDatabase*   iContactDB;

    };

#endif      // IMAPI_CNTDBREADER_H   


// End of File
