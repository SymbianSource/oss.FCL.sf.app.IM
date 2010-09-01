/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  holds the partial success information
*
*/


#ifndef CCAPARTIALSUCCESSDATA_H
#define CCAPARTIALSUCCESSDATA_H

// INCLUDES
#include    <e32base.h>

// FORWARD DECLARATIONS
class       CCAErrorData;
class       MCAError;
class       CImpsDetailed;

// CLASS DECLARATION

/**
*  CCAPartialSuccessData holds the message information.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class CCAPartialSuccessData : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAPartialSuccessData* NewL();

        /**
        * Destructor.
        */
        virtual ~CCAPartialSuccessData();

    public: // New functions

        /**
        * Set the partial success data. There can be only one of these
        * structures at a single moment. So when calling this function
        * it overrides any existing data.
        * @param aDetailedResults holds the information from the
        * partial success.
        */
        void SetPartialSuccessDataL(
            const CImpsDetailed& aDetailedResults );

        /**
        * Returns the partial success data array.
        * @return Partial success array (non-modifiable).
        */
        const RPointerArray<MCAError>* PartialSuccessData() const;

    private:

        /**
        * C++ default constructor.
        * @see CCAPartialSuccessData::ConstructL.
        */
        CCAPartialSuccessData();

    private:    // Data

        // Error data holder
        RPointerArray<MCAError> iErrorData;
    };

#endif      // CCAPARTIALSUCCESSDATA_H

// End of File
