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
* Description: Observer for IMPS property.
*
*
*/


// INCLUDE FILES
#include    <centralrepository.h>
#include    "impspropertyobserver.h"
#include    "impsutils.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CImpsPropertyObserver::NewL
// ---------------------------------------------------------
//
CImpsPropertyObserver* CImpsPropertyObserver::NewL( MImpsPropertyObserver& aObserver,
                                                    TUid aCategory,
                                                    TUint32 aKey )
    {
    CImpsPropertyObserver* self = new ( ELeave ) CImpsPropertyObserver( aObserver,
                                                                        aCategory, aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPropertyObserver Constructor " ) );
#endif
    return self;
    }

// ---------------------------------------------------------
// CImpsPropertyObserver::~CImpsPropertyObserver
// ---------------------------------------------------------
//
CImpsPropertyObserver::~CImpsPropertyObserver()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPropertyObserver Destructor " ) );
#endif
    Cancel();
    delete iRep;
    }

// ---------------------------------------------------------
// CImpsPropertyObserver::CImpsPropertyObserver
// ---------------------------------------------------------
//
CImpsPropertyObserver::CImpsPropertyObserver( MImpsPropertyObserver& aObserver,
                                              TUid aCategory,
                                              TUint32 aKey )
        : CActive( CActive::EPriorityStandard ),
        iObserver( aObserver ),
        iCategory( aCategory ),
        iKey( aKey )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CImpsPropertyObserver::ConstructL
// ---------------------------------------------------------
//
void CImpsPropertyObserver::ConstructL( )
    {
    iRep = CRepository::NewL( iCategory );
    iStatus = KRequestPending;
    iRep->NotifyRequest( iKey, iStatus );
    SetActive( );
    }

// ---------------------------------------------------------
// CImpsPropertyObserver::RunL
// ---------------------------------------------------------
//
void CImpsPropertyObserver::RunL()
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPropertyObserver RunL" ) );
#endif
    TInt newValue( 0 );
    iRep->Get( iKey, newValue );
    iObserver.HandlePropertyChangeL( newValue );
    iStatus = KRequestPending;
    iRep->NotifyRequest( iKey, iStatus );
    SetActive( );
    }

// ---------------------------------------------------------
// CImpsPropertyObserver::RunError
// ---------------------------------------------------------
//
TInt CImpsPropertyObserver::RunError( TInt /*aError*/ )
    {
#ifndef _NO_IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "CImpsPropertyObserver RunError ***" ) );
#endif
    // iObserver.HandlePropertyChangeL cannot actually leave now, so
    // this is not needed:
    /*
    iStatus = KRequestPending;
    iRep->NotifyRequest( iKey, iStatus );
    SetActive( );
    */
    return KErrNone;
    }

// ---------------------------------------------------------
// CImpsPropertyObserver::DoCancel
// ---------------------------------------------------------
//
void CImpsPropertyObserver::DoCancel()
    {
    /*
    Central Repository Design Document:
    It is possible to cancel a notification by supplying the exact
    same arguments used to set up the notification to the relevant
    NotifyCancel API. In this situation the TRequestStatus will no
    be set to KErrCancel but to KInvalidNotificationId.
    */
    iRep->NotifyCancelAll();
    }

//  End of File



