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
* Description:  Represents the search data for UI
*
*/



// INCLUDES
#include    "CCASearchDataPC.h"
#include    "CCAStorageManagerFactory.h"
#include    "MCAStoredContacts.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCASearchDataPC: Constructor
// -----------------------------------------------------------------------------
//
CCASearchDataPC::CCASearchDataPC( MCASearchData* aSearchData )
        : iDataInterface( aSearchData )
    {

    }

// -----------------------------------------------------------------------------
// CCASearchDataPC: NewL
// -----------------------------------------------------------------------------
//
CCASearchDataPC* CCASearchDataPC::NewL( MCASearchData* aSearchData )
    {
    CCASearchDataPC* self = new ( ELeave ) CCASearchDataPC( aSearchData );
    return self;
    }

// -----------------------------------------------------------------------------
// CCASearchDataPC: Destructor
// -----------------------------------------------------------------------------
//
CCASearchDataPC::~CCASearchDataPC()
    {

    }

// -----------------------------------------------------------------------------
// CCASearchDataPC: SearchData
// -----------------------------------------------------------------------------
//
TPtrC CCASearchDataPC::SearchData( TInt aIndex ) const
    {
    if ( iDataInterface )
        {
        return iDataInterface->SearchData( aIndex );
        }
    else
        {
        return KNullDesC();
        }
    }

// -----------------------------------------------------------------------------
// CCASearchDataPC: SearchDataCount
// -----------------------------------------------------------------------------
//
TInt CCASearchDataPC::SearchDataCount() const
    {
    if ( iDataInterface )
        {
        return iDataInterface->SearchDataCount();
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCASearchDataPC: HowManyResultsAvailable
// -----------------------------------------------------------------------------
//
TInt CCASearchDataPC::HowManyResultsAvailable() const
    {
    if ( iDataInterface )
        {
        return iDataInterface->HowManyResultsAvailable();
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCASearchDataPC: IdentificationL
// -----------------------------------------------------------------------------
//
const TPtrC CCASearchDataPC::IdentificationL( const TDesC& aContactId )
    {
    MCAStoredContacts* storedContacts = CCAStorageManagerFactory::ContactListInterfaceL();
    return storedContacts->Identification( aContactId );
    }

// End of File
