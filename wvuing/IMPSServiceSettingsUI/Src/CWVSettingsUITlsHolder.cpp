/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TLS holder for service settings UI
*
*/

// INCLUDE FILES
#include    "CWVSettingsUITlsHolder.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWVSettingsUITlsHolder::CWVSettingsUITlsHolder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWVSettingsUITlsHolder::CWVSettingsUITlsHolder()
        : iResourceLoaded( EFalse ), iResourceReferenceCount( 0 )
    {
    }


// -----------------------------------------------------------------------------
// CWVSettingsUITlsHolder::InstanceL
// Singleton retrieve method.
// -----------------------------------------------------------------------------
//
CWVSettingsUITlsHolder* CWVSettingsUITlsHolder::InstanceL()
    {
    CWVSettingsUITlsHolder* holder = NULL;
    holder = static_cast<CWVSettingsUITlsHolder*>( Dll::Tls() );
    if ( !holder )
        {
        holder = CWVSettingsUITlsHolder::NewLC();
        User::LeaveIfError( Dll::SetTls( holder ) );
        CleanupStack::Pop( holder ); // holder
        }

    return holder;
    }


// -----------------------------------------------------------------------------
// CWVSettingsUITlsHolder::DestroySingletonsAndHolder()
//
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWVSettingsUITlsHolder::DestroySingletonsAndHolder()
    {
    TInt err = KErrNone;

    CWVSettingsUITlsHolder* holder = NULL;
    holder = static_cast<CWVSettingsUITlsHolder*>( Dll::Tls() );
    if ( holder )
        {
        //holder exist, so try to delete it
        err = Dll::SetTls( NULL );

        if ( err == KErrNone )
            {
            //we could null the TLS - destroy now self
            delete holder;
            }
        else
            {
            //couldn't null the TLS, it's ok
            //destroy just owned singletons
            holder->DoReleaseSingletons();
            }
        }

    return err;
    }




// -----------------------------------------------------------------------------
// CWVSettingsUITlsHolder::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWVSettingsUITlsHolder* CWVSettingsUITlsHolder::NewLC()
    {
    CWVSettingsUITlsHolder* self = new( ELeave ) CWVSettingsUITlsHolder;
    CleanupStack::PushL( self );
    return self;
    }



// Destructor
CWVSettingsUITlsHolder::~CWVSettingsUITlsHolder()
    {
    DoReleaseSingletons();
    }



// -----------------------------------------------------------------------------
// CWVSettingsUITlsHolder::ResourceIsLoaded()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUITlsHolder::ResourceIsLoaded() const
    {
    return iResourceLoaded;
    }


// -----------------------------------------------------------------------------
// CWVSettingsUITlsHolder::ResourceLoaded()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUITlsHolder::ResourceLoaded()
    {
    iResourceLoaded = ETrue;
    iResourceReferenceCount++;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUITlsHolder::ResourceUnLoaded()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUITlsHolder::ResourceUnLoaded()
    {
    iResourceReferenceCount--;
    iResourceLoaded = EFalse;
    }

// -----------------------------------------------------------------------------
// CWVSettingsUITlsHolder::OkToUnloadResource()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CWVSettingsUITlsHolder::OkToUnloadResource() const
    {
    return ( iResourceReferenceCount == 1 );
    }

// -----------------------------------------------------------------------------
// CWVSettingsUITlsHolder::DoReleaseSingletons()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWVSettingsUITlsHolder::DoReleaseSingletons()
    {
    iResourceLoaded = EFalse;
    iResourceReferenceCount = 0;
    }



//  End of File
