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
* Description: 
*     Data structure defintions for public WV Engine
*     Fundamental API.
*
*/


#ifndef ImpsFundamental_H
#define ImpsFundamental_H

//  INCLUDES
#include <e32base.h>

//Supported search elements 
enum TImpsSearchElement
    {
    EImpsUserAlias,
    EImpsUserOnlineStatus,
    EImpsUserEmailAddress,
    EImpsUserFirstName,
    EImpsUserID,
    EImpsUserLastName,
    EImpsUserMobileNumber,
    EImpsGroupID,
    EImpsGroupName,
    EImpsGroupTopic,
    EImpsGroupUserIDJoined,
    EImpsGroupUserIDOwner
    };

/**
* Search result element
*/
enum TImpsSearchResultType
    {
    EImpsUserResult,
    EImpsGroupResult
    };

// FORWARD DECLARATIONS
class CImpsSearchRequest;

// Remember to call ResetAndDestroy() to clean all the data in the array
typedef CArrayPtrFlat<CImpsSearchRequest> CSearchPairs;

class CImpsSearchRequest: public CBase
{
    public:

        /**
        * Constructor.
        */
        IMPORT_C static CImpsSearchRequest* NewL( );

        /**
        * Set values. 
        * This allocates memory and copies aValue to its internal data storage.
        * @param aElement search key
        * @param aValue search value
        */
        IMPORT_C void SetRequestL( TImpsSearchElement aElement, 
                                   const TDesC& aValue );
        
        /**
        * Reset the object 
        */
        IMPORT_C void ResetL();
        
        /**
        * Returns the value of the object
        * @return TPtrC
        */
        inline TPtrC Value() const;
        
        /**
        * Returns the type of the element
        * @return TInt
        */
        inline TInt Element() const;

        /**
        * Destructor.
        */
        virtual ~CImpsSearchRequest();

    private:
        
        /* 
        * C++ constructor is private by default
        */
        CImpsSearchRequest();


    private:
        TImpsSearchElement iElement;
        HBufC* iValue;
};

#include "ImpsFundFields.inl"

#endif 
            
// End of File
