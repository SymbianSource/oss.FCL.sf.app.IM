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
* Description:  Search criteria.
*
*/

#include "CPEngSearchCriteria2.h"
#include <PEngPresenceEngineConsts2.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSearchCriteria2* CPEngSearchCriteria2::NewL()
    {
    CPEngSearchCriteria2* self = new( ELeave )CPEngSearchCriteria2;
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlot2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngSearchCriteria2* CPEngSearchCriteria2::NewLC()
    {
    CPEngSearchCriteria2* self = new( ELeave )CPEngSearchCriteria2;
    CleanupStack::PushL( self );
    return self;
    }



// Destructor
CPEngSearchCriteria2::~CPEngSearchCriteria2()
    {
    delete iSearchValue;
    }



// -----------------------------------------------------------------------------
// CPEngSearchCriteria2::CPEngSearchCriteria2()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngSearchCriteria2::CPEngSearchCriteria2()
        : iElement( KPEngNullSearchElement )
    {
    }


// -----------------------------------------------------------------------------
// CPEngSearchCriteria2::SetSearchL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngSearchCriteria2::SetSearchL( TInt aElement,
                                                const TDesC& aSearchValue )
    {
    HBufC* tmp = aSearchValue.AllocL();

    delete iSearchValue;
    iSearchValue = tmp;
    iElement = aElement;
    }


// -----------------------------------------------------------------------------
// CPEngSearchCriteria2::Element()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngSearchCriteria2::Element() const
    {
    return iElement;
    }


// -----------------------------------------------------------------------------
// CPEngSearchCriteria2::SearchValue()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPEngSearchCriteria2::SearchValue() const
    {
    if ( iSearchValue )
        {
        return *iSearchValue;
        }

    return KNullDesC;
    }


//End of file


