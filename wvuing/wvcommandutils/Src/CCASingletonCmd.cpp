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
#include    <e32std.h>
#include    "CCASingletonCmd.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCASingletonCmd::CCASingletonCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCASingletonCmd::CCASingletonCmd()
    {

    }

// -----------------------------------------------------------------------------
// CCASingletonCmd::~CCASingletonCmd
// Destructor
// -----------------------------------------------------------------------------
//
CCASingletonCmd::~CCASingletonCmd()
    {
    Dll::SetTls( NULL );
    }


// -----------------------------------------------------------------------------
// CCASingletonCmd::BaseInstanceL()
// -----------------------------------------------------------------------------
//
TAny* CCASingletonCmd::BaseInstanceL()
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
