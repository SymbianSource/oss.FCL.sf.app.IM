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
* Description:  Client filtered event notifier
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "cimpsconnuiclntfilteredpreseventnotifierng.h"
#include "CIMPSConnUiClntFilteredPresEventNotifierImp.h"
#include "IMPSCommonUiDebugPrint.h"

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
EXPORT_C CIMPSConnUiClntFilteredPresEventNotifier*
CIMPSConnUiClntFilteredPresEventNotifier::NewL( TIMPSConnectionClient aClient )
    {
    CIMPSConnUiClntFilteredPresEventNotifier* self =
        CIMPSConnUiClntFilteredPresEventNotifier::NewLC( aClient );
    CleanupStack::Pop( self );  //self
    return self;
    }


// Two-phased constructor.
EXPORT_C CIMPSConnUiClntFilteredPresEventNotifier*
CIMPSConnUiClntFilteredPresEventNotifier::NewLC( TIMPSConnectionClient aClient )
    {
    CIMPSConnUiClntFilteredPresEventNotifier* self =
        new ( ELeave ) CIMPSConnUiClntFilteredPresEventNotifier;
    CleanupStack::PushL( self );
    self->ConstructL( aClient );
    return self;
    }


// Destructor
EXPORT_C CIMPSConnUiClntFilteredPresEventNotifier::~CIMPSConnUiClntFilteredPresEventNotifier()
    {
    IMPSCUI_DP_TXT( "CIMPSConnUiClntFilteredPresEventNotifier::~CIMPSConnUiClntFilteredPresEventNotifier()" );
    }


// C++ constructor
CIMPSConnUiClntFilteredPresEventNotifier::CIMPSConnUiClntFilteredPresEventNotifier()
    {
    IMPSCUI_DP_TXT( "CIMPSConnUiClntFilteredPresEventNotifier::CIMPSConnUiClntFilteredPresEventNotifier()" );
    }


// Symbian OS constructor
void CIMPSConnUiClntFilteredPresEventNotifier::ConstructL( TIMPSConnectionClient aClient )
    {
    CIMPSConnUiPresEventNotifierImpBase* imp =
        CIMPSConnUiClntFilteredPresEventNotifierImp::NewL( *this, aClient );
    CIMPSConnUiPresEventNotifier::ConstructL( imp );
    }


//  End of File




