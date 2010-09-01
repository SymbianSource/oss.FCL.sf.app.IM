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
* Description:  One value adapter for MDesCArray.
*
*/

//  INCLUDES
#include "TPEngMDesCArrayAdapter.h"
#include <E32Std.h>
#include <BaDesCa.h>


//LOCAL constants
namespace
    {
    //Panic
    _LIT( KPEngMDesArrayAdapterPanic, "PEngDesArrAdp" );

    //Panic reasons
    enum TPEngMDesArrayAdapterReasons
        {
        EMdcaPointOutOfBounds
        };

    void PEngMDesAdapPanic( TPEngMDesArrayAdapterReasons aPanicReason )
        {
        User::Panic( KPEngMDesArrayAdapterPanic, aPanicReason );
        }
    }




// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
TPEngMDesCArrayAdapter::TPEngMDesCArrayAdapter( const TDesC& aDesc )
        : iDesc( aDesc )
    {
    }

// -----------------------------------------------------------------------------
// TPEngMDesCArrayAdapter::MdcaCount()
// -----------------------------------------------------------------------------
//
TInt TPEngMDesCArrayAdapter::MdcaCount() const
    {
    //there is just one adapted descriptor
    return 1;
    }

// -----------------------------------------------------------------------------
// TPEngMDesCArrayAdapter::MdcaPoint()
// -----------------------------------------------------------------------------
//
TPtrC TPEngMDesCArrayAdapter::MdcaPoint( TInt aIndex ) const
    {
    //there is just one adapted descriptor
    __ASSERT_ALWAYS( aIndex == 0, PEngMDesAdapPanic( EMdcaPointOutOfBounds ) );
    return iDesc;
    }



//  End of File

