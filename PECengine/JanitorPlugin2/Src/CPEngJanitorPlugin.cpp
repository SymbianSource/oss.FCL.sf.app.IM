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
* Description:  CPEngJanitorPlugin
*
*/

//  Include Files
#include <E32Base.h>
#include "CPEngJanitorPlugin.h"
#include "CPEngContactMapper.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngJanitorPlugin::CPEngJanitorPlugin()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngJanitorPlugin::CPEngJanitorPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CPEngJanitorPlugin::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngJanitorPlugin* CPEngJanitorPlugin::NewL()
    {
    CPEngJanitorPlugin* self = new( ELeave ) CPEngJanitorPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CPEngJanitorPlugin::~CPEngJanitorPlugin()
    {
    delete iContactMapper;
    }


// -----------------------------------------------------------------------------
// CPEngJanitorPlugin::ConstructL()
// Second phase construct
// -----------------------------------------------------------------------------
//
void CPEngJanitorPlugin::ConstructL()
    {
    iContactMapper = CPEngContactMapper::NewL();
    }



// End of file


