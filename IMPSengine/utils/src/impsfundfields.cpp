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
*     Data structure defintions for public WV Engine
*     Fundamental API.
*
*/


// INCLUDE FILES
#include    "impsfundamental.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor may NOT contain any code, that
// might leave.
//
CImpsSearchRequest::CImpsSearchRequest()
    {
    }

    
// Destructor
CImpsSearchRequest::~CImpsSearchRequest()
    {
    delete iValue;
    }

// ---------------------------------------------------------
// CImpsSearchRequest::NewL
//
// ---------------------------------------------------------
//
EXPORT_C CImpsSearchRequest* CImpsSearchRequest::NewL()
    {
    CImpsSearchRequest* self = new ( ELeave ) CImpsSearchRequest;
    return self;
    }

// ---------------------------------------------------------
// CImpsSearchRequest::SetRequestL
//
// ---------------------------------------------------------
//
EXPORT_C void CImpsSearchRequest::SetRequestL( TImpsSearchElement aElement, 
                                               const TDesC& aValue )
   {
   HBufC* newAttrib = aValue.AllocL();
   delete iValue;
   iValue = newAttrib;
   iElement = aElement;   
   }

// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  

