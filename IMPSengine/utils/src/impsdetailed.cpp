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
*     Data structures used fro detailed error data in 
*     WV engine API.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "impsdetailed.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CImpsDetailedResult::CImpsDetailedResult()
    {
    }

// EPOC default constructor can leave.
void CImpsDetailedResult::ConstructL()
    {
    iCode = KErrNone;          // code
    iDescription.Set( KNullDesC );   // description, just a reference
    iUserIds = new (ELeave) CPtrC16Array(5);       // user-Id
    iGroupIds = new (ELeave) CPtrC16Array(5);
    iScreenNames = new (ELeave) CPtrC16Array(5);
    iGroupNames = new (ELeave) CPtrC16Array(5);
    iMessageIds = new (ELeave) CPtrC16Array(5);
    }

// Two-phased constructor.
EXPORT_C CImpsDetailedResult* CImpsDetailedResult::NewL()
    {
    CImpsDetailedResult* self = new (ELeave) CImpsDetailedResult;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// Destructor
EXPORT_C CImpsDetailedResult::~CImpsDetailedResult()
    {
    if(iUserIds != NULL)
        iUserIds->Reset();
    delete iUserIds;

    if(iGroupIds != NULL)
        iGroupIds->Reset();
    delete iGroupIds;

    if(iScreenNames != NULL)
        iScreenNames->Reset();
    delete iScreenNames;

    if(iGroupNames != NULL)
        iGroupNames->Reset();
    delete iGroupNames;

    if(iMessageIds != NULL)
        iMessageIds->Reset();
    delete iMessageIds;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::Reset()
// ---------------------------------------------------------
//
EXPORT_C void CImpsDetailedResult::Reset() 
    {
    iCode = KErrNone;
    iDescription.Set( KNullDesC );   // description, just a reference
    iUserIds->Reset();
    iGroupIds->Reset();
    iScreenNames->Reset();
    iGroupNames->Reset();
    iMessageIds->Reset();
    }

// ---------------------------------------------------------
// CImpsDetailedResult::Code()
// ---------------------------------------------------------
//
EXPORT_C TInt CImpsDetailedResult::Code() const
    {
    return iCode;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::SetCode()
// ---------------------------------------------------------
//
EXPORT_C void CImpsDetailedResult::SetCode( TInt aInt )
    {
    iCode = aInt;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::Descriptor()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CImpsDetailedResult::Descriptor() const
    {
    return iDescription;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::SetDescriptorL()
// ---------------------------------------------------------
//
EXPORT_C void CImpsDetailedResult::SetDescriptorL( TDesC& aDes )
    {
    iDescription.Set( aDes );
    }

// ---------------------------------------------------------
// CImpsDetailedResult::UserIds()
// ---------------------------------------------------------
//
EXPORT_C CPtrC16Array* CImpsDetailedResult::UserIds() const
    {
    return iUserIds;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::UserIds()
// ---------------------------------------------------------
//
EXPORT_C CPtrC16Array* CImpsDetailedResult::UserIds()
    {
    return iUserIds;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::GroupIds()
// ---------------------------------------------------------
//
EXPORT_C const CPtrC16Array* CImpsDetailedResult::GroupIds() const
    {
    return iGroupIds;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::GroupIds()
// ---------------------------------------------------------
//
EXPORT_C CPtrC16Array* CImpsDetailedResult::GroupIds()
    {
    return iGroupIds;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::MessageIds()
// ---------------------------------------------------------
//
EXPORT_C const CPtrC16Array* CImpsDetailedResult::MessageIds() const
    {
    return iMessageIds;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::MessageIds()
// ---------------------------------------------------------
//
EXPORT_C CPtrC16Array* CImpsDetailedResult::MessageIds()
    {
    return iMessageIds;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::Snames()
// ---------------------------------------------------------
//
EXPORT_C const CPtrC16Array* CImpsDetailedResult::SNames() const
    {
    return iScreenNames;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::Snames()
// ---------------------------------------------------------
//
EXPORT_C CPtrC16Array* CImpsDetailedResult::SNames()
    {
    return iScreenNames;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::SNameGroups()
// ---------------------------------------------------------
//
EXPORT_C const CPtrC16Array* CImpsDetailedResult::SNameGroups() const
    {
    return iGroupNames;
    }

// ---------------------------------------------------------
// CImpsDetailedResult::SNameGroups()
// ---------------------------------------------------------
//
EXPORT_C CPtrC16Array* CImpsDetailedResult::SNameGroups()
    {
    return iGroupNames;
    }


// ---------------------------------------------------------
// CImpsDetailed::CImpsDetailed()
// ---------------------------------------------------------
EXPORT_C CImpsDetailed::CImpsDetailed(TInt aGranularity)
    :CArrayPtrFlat<CImpsDetailedResult>(aGranularity)
    {
    }

// ---------------------------------------------------------
// CImpsDetailed::~CImpsDetailed()
// ---------------------------------------------------------
EXPORT_C CImpsDetailed::~CImpsDetailed()
    {
    ResetAndDestroy();
    }

//  End of File  

