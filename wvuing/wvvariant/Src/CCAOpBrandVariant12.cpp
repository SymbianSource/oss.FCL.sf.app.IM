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
* Description:  Inactive implementation for res.file& bitmap changing feature.
*
*/



// INCLUDE FILES
#include    "CCAOpBrandVariant12.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAOpBrandVariant12::CCAOpBrandVariant12
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAOpBrandVariant12::CCAOpBrandVariant12()
    {
    }

// -----------------------------------------------------------------------------
// CCAOpBrandVariant12::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAOpBrandVariant12::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CCAOpBrandVariant12::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAOpBrandVariant12* CCAOpBrandVariant12::NewL()
    {
    CCAOpBrandVariant12* self = new( ELeave ) CCAOpBrandVariant12;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CCAOpBrandVariant12::~CCAOpBrandVariant12()
    {

    }


// Empty implementations
void CCAOpBrandVariant12::RemoveAppResourceL( CCoeEnv* /*aCoeEnv*/, TInt& /*aAppResFileOffset*/ )
    {
    }

void CCAOpBrandVariant12::RetrieveSavedSettingsL( CCoeEnv* /*aCoeEnv*/, const TDesC& /*aOrigResFilePath*/ )
    {
    }
void CCAOpBrandVariant12::ActivateBrandL( const TDesC& /*aServer*/, const TDesC& /*aAppResFileName*/
                                          , const TDesC& /*aEngResFileName*/
                                          , const TDes& /*aChatAppMbmFileName*/ )
    {
    }

void CCAOpBrandVariant12::DeleteResource( CCoeEnv* /*aCoeEnv*/ )
    {
    }
void CCAOpBrandVariant12::DeleteEngResource( CCoeEnv* /*aCoeEnv*/ )
    {
    }
void CCAOpBrandVariant12::RetrieveSavedEngSettingsL( CCoeEnv* /*aCoeEnv*/, const TDesC& /*aOrigResFilePath*/ )
    {
    }


//  End of File
