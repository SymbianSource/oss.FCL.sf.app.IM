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
* Description:  Interface class for variation factory.
*
*/



// INCLUDE FILES
#include    "CCAVariantFactory.h"
#include    "CCAVariantFactoryImp.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAVariantFactory::CCAVariantFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAVariantFactory::CCAVariantFactory()
    {
    }

// -----------------------------------------------------------------------------
// CCAVariantFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAVariantFactory* CCAVariantFactory::NewL()
    {
    CCAVariantFactory* self = CCAVariantFactoryImp::NewL();
    return self;
    }


// Destructor
CCAVariantFactory::~CCAVariantFactory()
    {

    }


//  End of File
