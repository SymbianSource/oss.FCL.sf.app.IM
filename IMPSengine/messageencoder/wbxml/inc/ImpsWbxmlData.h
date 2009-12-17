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
* Description:  Provides methods for CSP WBXML data handling
*
*/


#ifndef CImpsWbXmlData_H
#define CImpsWbXmlData_H

//  INCLUDES
#include <e32base.h>
#include "ImpsDataAccessorApi.h"
#include "nw_wbxml_reader.h"
#include "impscommonenums.h"

// CONSTANTS
const TInt KImpsWbXmlMaxLength50   = 50;
const TInt KImpsWbXmlMaxLength100  = 100;
const TInt KImpsWbXmlMaxLength200  = 200;
const TInt KImpsWbXmlMaxLength400  = 400;
const TInt KImpsWbXmlNoMaxLength   = 0;

// FORWARD DECLARATIONS
class CImpsKey;

// CLASS DECLARATION

class CImpsWbXmlData : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CImpsWbXmlData* NewL();

        /**
        * Destructor.
        */
        virtual ~CImpsWbXmlData();

    public: // New functions


        /**
        * access method for integer attribute token value
        * @param aToken WBXML token
        * @return attribute token value
        */
        TUint GetWbXmlAttributeTokenValue( TInt aToken, TImpsCspVersion aCspVersion );

        /**
        * access method for string attribute token value
        * @param aToken WBXML token
        * @return attribute token value
        */
        TPtrC8 GetWbXmlAttributeStringValue( TInt aToken, TImpsCspVersion aCspVersion );

        /**
        * method returns wbxml token value corresponding to
        * @param aToken WBXML token
        * @param aValue WBXML token value
        * @param aMaxLength possible max string length
        * @return wbxml token
        */
        TUint32 GetWbXmlElementValue( TInt aToken, TUint32 aValue, TInt& aMaxLength );

        /**
        * method returns wbxml token value corresponding to
        * given imps engine value and possible max string length
        * @param aToken WBXML token
        * @param aValue WBXML token value
        * @return wbxml token
        */
        TUint32 GetWbXmlElementValue( TInt aToken, TDesC*& aValue );

        /**
        * method returns wbxml string value corresponding to
        * given imps engine value
        * @param aToken WBXML token
        * @param aValue WBXML token value
        * @return wbxml value
        */
        TPtrC8 GetWbXmlStringValue( TInt aToken, TInt aValue );

        /**
        * method returns imps engine value coresponding to given
        * wbxml element value if exists
        * @param aToken WBXML token
        * @param aValue WBXML token value
        * @return imps engine value
        */
        TInt GetImpsEngineValue( TInt aToken, TInt aValue );

        /**
        * method returns imps engine value coresponding to given
        * string if exists
        * @param aToken WBXML token
        * @param aValue string value
        * @return imps engine value
        */
        TUint32 GetImpsEngineValue( TInt aToken, TPtrC8 aValue );

        /**
        * method returns possible string corresponding to given token
        * @param aToken WBXML token
        * @param aValue WBXML token value
        * @return imps engine value
        */
        TPtrC GetImpsEngineDescValue( TInt aToken, TInt aValue );

        /**
        * method returns possible string corresponding to given token
        * @param aToken WBXML token
        * @param aValue WBXML token value
        * @return imps engine value
        */
        TPtrC8 GetImpsEngineDesc8Value( TInt aToken, TInt aValue );


//        const TDesC8& GetAttributeStringByToken(TInt aToken);

    private:

        /**
        * C++ default constructor.
        */
        CImpsWbXmlData();

        /**
        * By default constructor is private.
        */
        void ConstructL();

    };

#endif      // ?INCLUDE_H   

// End of File
