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
* Description:  Access handler of the network session
*
*/

// INCLUDE FILES
#include    "CPEngTransAdapFactory.h"
#include    "CPEngSessionManager.h"
#include    "CPEngSessionSlotId.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngTransAdapFactory::CPEngTransAdapFactory
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPEngTransAdapFactory::CPEngTransAdapFactory()
        : iSessManagers( 1 ) // usually only one session
    {
    }


// -----------------------------------------------------------------------------
// CPEngTransAdapFactory::InstanceLC()
// -----------------------------------------------------------------------------
//
CPEngTransAdapFactory* CPEngTransAdapFactory::InstanceLC()
    {
    CPEngTransAdapFactory* self = static_cast<CPEngTransAdapFactory*>( Dll::Tls() );
    if ( !self )
        {
        // create instance and store it in the Tls
        self = new ( ELeave ) CPEngTransAdapFactory();
        CleanupClosePushL( *self );
        User::LeaveIfError( Dll::SetTls( self ) );
        }
    else
        {
        self->Open(); // CSI: 65 #
        CleanupClosePushL( *self );
        }
    return self;
    }


// Destructor
CPEngTransAdapFactory::~CPEngTransAdapFactory()
    {
    iSessManagers.ResetAndDestroy();
    Dll::SetTls( NULL );


#if _BullseyeCoverage
    cov_write();
#endif
    }


// =============================================================================
// =============== New Functions of the MPEngTransAdapFactory ==================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngTransAdapFactory::OpenSessionCount()
// -----------------------------------------------------------------------------
//
void CPEngTransAdapFactory::OpenSessionCount()
    {
    Open();
    }


// -----------------------------------------------------------------------------
// CPEngTransAdapFactory::CloseSession()
// -----------------------------------------------------------------------------
//
void CPEngTransAdapFactory::CloseSession(
    CPEngSessionManager* aSessManager )
    {
    TInt x ( iSessManagers.Find( aSessManager ) );
    if ( x != KErrNotFound )
        {
        iSessManagers.Remove( x );
        }
    Close();
    }


// =============================================================================
// =============== New Functions of the base class =============================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngTransAdapFactory::AccessHandlerL()
// -----------------------------------------------------------------------------
//
MPEngAccessHandler* CPEngTransAdapFactory::AccessHandlerL(
    const CPEngSessionSlotId& aSessionId )
    {
    MPEngAccessHandler* handler =
        SessionManagerLC( aSessionId ).AccessHandlerL();
    CleanupStack::PopAndDestroy(); // sessManager
    return handler;
    }


// -----------------------------------------------------------------------------
// CPEngTransAdapFactory::PureDataHandlerL()
// -----------------------------------------------------------------------------
//
MPEngPureDataHandler* CPEngTransAdapFactory::PureDataHandlerL(
    const CPEngSessionSlotId& aSessionId )
    {
    MPEngPureDataHandler* handler =
        SessionManagerLC( aSessionId ).PureDataHandlerL();
    CleanupStack::PopAndDestroy(); // sessManager
    return handler;
    }


// =============================================================================
// =============== New Private Functions of the base class =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CPEngTransAdapFactory::SessionManagerLC
// Find Session manager
// if it does not exist it is created
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPEngSessionManager& CPEngTransAdapFactory::SessionManagerLC(
    const CPEngSessionSlotId& aSessionId )
    {
    TInt count ( iSessManagers.Count() );
    CPEngSessionManager* sessManager = NULL;
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        if ( KErrNone == iSessManagers[ x ]->SessionId().Match(
                 aSessionId ) )
            {
            sessManager = iSessManagers[ x ];
            sessManager->Open(); // CSI: 65 #
            CleanupClosePushL( *sessManager );
            break;
            }
        }
    if ( !sessManager )
        {
        // does not exist, create new one
        sessManager = CPEngSessionManager::NewLC( *this, aSessionId );
        iSessManagers.AppendL( sessManager );
        }
    return *sessManager;
    }

// End of File


