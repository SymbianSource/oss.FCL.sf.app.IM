/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CAlternator.h.
*
*/



// INCLUDE FILES
#include    "CAlternator.h"
#include    "CAlternatorImp.h"

// ============================ MEMBER FUNCTIONS ===============================

EXPORT_C CAlternator* CAlternator::InstanceL( const TDesC& aBrandRoot )
    {
    return CreateAlternatorL( aBrandRoot );
    }


EXPORT_C void CAlternator::Release()
    {
    TInt ignore;
    TRAP( ignore, ReleaseAlternatorL() );
    }

// -----------------------------------------------------------------------------
// CAlternator::CAlternator
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAlternator::CAlternator()
    {
    }


// Destructor
CAlternator::~CAlternator()
    {

    }


//  End of File
