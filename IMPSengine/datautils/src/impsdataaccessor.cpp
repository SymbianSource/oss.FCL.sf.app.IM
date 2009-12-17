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
* Description: Class for describing imps inner data structure accessor
* 
*/

// INCLUDE FILES
#include    <e32std.h>
#include    "impsfields.h"
#include    "impsconst.h"
#include    "impskey.h"
#include    "impserrors.h"
#include    "impsutils.h"
#include    "impsdatautils.h"
#include    "impstdataaccessor.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CImpsDataAccessor::CImpsDataAccessor()
    {
    }

// Symbian OS default constructor can leave.
void CImpsDataAccessor::ConstructL( CImpsFields* aImpsFields )
    {
    iDataAccessor.SetImpsFields(aImpsFields);
    }

// Two-phased constructor.
EXPORT_C CImpsDataAccessor* CImpsDataAccessor::NewL( CImpsFields* aImpsFields)
    {
    CImpsDataAccessor* self = new ( ELeave ) CImpsDataAccessor;
    CleanupStack::PushL( self );
    self->ConstructL(aImpsFields);
    CleanupStack::Pop();
    return self;
    }

// Two-phased constructor.
EXPORT_C CImpsDataAccessor* CImpsDataAccessor::NewLC( CImpsFields* aImpsFields)
    {
    CImpsDataAccessor* self = new ( ELeave ) CImpsDataAccessor;
    CleanupStack::PushL( self );
    self->ConstructL(aImpsFields);
    return self;
    }
    
// Destructor
EXPORT_C CImpsDataAccessor::~CImpsDataAccessor()
    {
    }

// ---------------------------------------------------------
// CImpsDataAccessor::CopyDataL
// ---------------------------------------------------------
//
void CImpsDataAccessor::CopyDataL( 
            MImpsDataAccessor& aSource,
            MImpsKey* aKey )
    {
    //CImpsDataAccessor& iSource = STATIC_CAST(CImpsDataAccessor&,aSource);
    CImpsDataAccessor& iSource = static_cast<CImpsDataAccessor&>(aSource);
    CImpsKey* iKey = STATIC_CAST(CImpsKey*, aKey);
    // source data
    CImpsData* sp  = iSource.iDataAccessor.GetImpsFields()->Data();

    // destination data
    CImpsData* dp  = iDataAccessor.GetImpsFields()->Data();

    // Copy data
    dp->CopyL( iKey, sp );
    }


//  End of File  
