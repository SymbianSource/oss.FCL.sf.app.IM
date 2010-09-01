/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Plugin holder class
*
*/


//  Include Files
#include <e32base.h>
#include <e32svr.h>
#include <f32file.h>
#include <e32uid.h>

#include "CPEngPluginHolder.h"
#include "PresenceDebugPrint.h"
#include <CPEngPluginInterface2.h>

// CONSTRUCTION
// Static constructor
CPEngPluginHolder* CPEngPluginHolder::NewL()
    {
    CPEngPluginHolder* self = CPEngPluginHolder::NewLC();
    CleanupStack::Pop();

    return self;
    }

// Static constructor, leaves pointer to cleanup-stack
CPEngPluginHolder* CPEngPluginHolder::NewLC()
    {
    CPEngPluginHolder* self = new( ELeave ) CPEngPluginHolder();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
CPEngPluginHolder::~CPEngPluginHolder()
    {
    delete iPlugin;
    }

// Default constructor, protected
CPEngPluginHolder::CPEngPluginHolder()
        : iUid( TUid::Null() )
    {
    }

// Second phase construct
void CPEngPluginHolder::ConstructL()
    {
    }

TUid CPEngPluginHolder::Uid() const
    {
    return iUid;
    }

TBool CPEngPluginHolder::Loaded() const
    {
    return ( iPlugin != NULL );			// CSI: 64 #
    }

void CPEngPluginHolder::LoadPluginL( TUid aPluginUid )
    {
    CPEngPluginInterface2* tmpPlugin = CPEngPluginInterface2::NewL( aPluginUid );
    delete iPlugin;
    iPlugin = tmpPlugin;
    iUid = aPluginUid;
    }

const CPEngPluginInterface2* CPEngPluginHolder::Interface() const
    {
    return iPlugin;
    }

