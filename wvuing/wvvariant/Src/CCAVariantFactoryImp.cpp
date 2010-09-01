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
* Description:  Factory class that creates variant instances for 2.1.
*
*/



// INCLUDE FILES
#include    "CCAVariantFactoryImp.h"
#include	<bldvariant.hrh>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAVariantFactoryImp::CCAVariantFactoryImp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAVariantFactoryImp::CCAVariantFactoryImp()
    {
    }

// -----------------------------------------------------------------------------
// CCAVariantFactoryImp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAVariantFactoryImp* CCAVariantFactoryImp::NewL()
    {
    CCAVariantFactoryImp* self = new( ELeave ) CCAVariantFactoryImp;
    return self;
    }


// -----------------------------------------------------------------------------
// CCAVariantFactoryImp::~CCAVariantFactoryImp
// Destructor
// -----------------------------------------------------------------------------

CCAVariantFactoryImp::~CCAVariantFactoryImp()
    {
    delete iSkinVariant;
    delete iOpBrandVariant;
    delete iAHVariant;
    }

// -----------------------------------------------------------------------------
// CCAVariantFactoryImp::SkinVariantL
// -----------------------------------------------------------------------------
//
MCASkinVariant*	CCAVariantFactoryImp::SkinVariantL()
    {
    if ( !iSkinVariant )
        {
        iSkinVariant = CCASkinVariant20::NewL( OpBrandVariantL() );
        }

    return iSkinVariant;
    }

// -----------------------------------------------------------------------------
// CCAVariantFactoryImp::OpBrandVariantL
// -----------------------------------------------------------------------------
//
MCAOpBrandVariant* CCAVariantFactoryImp::OpBrandVariantL()
    {
    if ( !iOpBrandVariant )
        {
        iOpBrandVariant = CCAOpBrandVariant21::NewL();
        }

    return iOpBrandVariant;
    }

// -----------------------------------------------------------------------------
// CCAVariantFactoryImp::AHVariantL
// -----------------------------------------------------------------------------
//
MCAAHVariant* CCAVariantFactoryImp::AHVariantL()
    {
    if ( !iAHVariant )
        {
        iAHVariant = CCAAHVariant20::NewL();
        }
    return iAHVariant;
    }

//  End of File
