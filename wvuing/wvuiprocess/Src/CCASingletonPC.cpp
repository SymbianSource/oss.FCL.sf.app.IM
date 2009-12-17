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
* Description:  Singleton base class
*
*/



// INCLUDE FILES
#include    "CCASingletonPC.h"
#include    <e32std.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCASingletonPC::CCASingletonPC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCASingletonPC::CCASingletonPC()
    {
    }


// -----------------------------------------------------------------------------
// CCASingletonPC::~CCASingletonPC
// Destructor
// -----------------------------------------------------------------------------
//
CCASingletonPC::~CCASingletonPC()
    {
    Dll::SetTls( NULL );
    }


// -----------------------------------------------------------------------------
// CCASingletonPC::BaseInstanceL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CCASingletonPC::BaseInstanceL()
    {
    TAny* instance = Dll::Tls();
    if ( !instance )
        {
        instance = ConstructSingletonInstanceL();
        TInt err( Dll::SetTls( instance ) );
        if ( err == KErrNone )
            {
            return instance;
            }
        else
            {
            delete instance;
            User::Leave( err );
            return NULL;    // this line just keeps the compiler happy
            }
        }
    else
        {
        return instance;
        }
    }

//  End of File
