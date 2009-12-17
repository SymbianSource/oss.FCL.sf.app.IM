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
* Description:  Connection mode rewaker.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <PEngWVPresenceErrors2.h>

#include "CCnUiConnModeRewaker.h"
#include "MCnUiClientPlugin.h"
#include "CnUiErrors.h"

#include "IMPSCommonUiDebugPrint.h"


//CONSTANTS
const TInt KReWakeUserActivityTimeout = 30000000;    //30 seconds, defined in UI specification



// ================= LOCAL FUNCTIONS =======================
// -----------------------------------------------------------------------------
// ResumeRewakeStatic()
// -----------------------------------------------------------------------------
//
void ResumeRewakeStatic( TAny* aRewaker )
    {
    CCnUiConnModeRewaker* p = static_cast< CCnUiConnModeRewaker* >( aRewaker );
    p->ResumeRewake();
    }



// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiConnModeRewaker* CCnUiConnModeRewaker::NewL()
    {
    CCnUiConnModeRewaker* self = new ( ELeave ) CCnUiConnModeRewaker();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self
    return self;
    }


// Destructor
CCnUiConnModeRewaker::~CCnUiConnModeRewaker()
    {
    Cancel();

    //Rewaker is closing down. Issue possibly pending rewakes now.
    //Raising errors can be only ignored..
    TInt ignore;
    TRAP( ignore, RewakeRegisteredClientsL() );

    iAAClntsToRewake.ResetAndDestroy();
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiConnModeRewaker::CCnUiConnModeRewaker()
        : CTimer( CActive::EPriorityStandard )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiConnModeRewaker::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeRewaker::AddToAAModeRewakeListL()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeRewaker::AddToAAModeRewakeListL( MCnUiClientPlugin& aClientPlugin,
                                                   CPEngNWSessionSlotID2& aNWSessionSlotID )
    {
    if ( aClientPlugin.CurrentConnectionModeSettingL() == ECnUiCMAutomatic )
        {
        const TIMPSConnectionClient client = aClientPlugin.ClientID();
        if ( !ClientAlreadyReqistered( client ) )
            {
            //Make a own copy of plug-in object
            MCnUiClientPlugin* cPlugin = CreateClientPluginL( client, aNWSessionSlotID );
            TInt err = iAAClntsToRewake.Append( cPlugin );
            if ( err != KErrNone )
                {
                delete cPlugin;
                User::Leave( err );
                }

            IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnModeRewaker::AddToAAModeRewakeListL( added client[%d] )" ), client );
            }
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeRewaker::GetAARewakeListL()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeRewaker::GetAARewakeListL( RArray< TIMPSConnectionClient >& aAARewakedClients )
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnModeRewaker::GetAARewakeListL( %d clients on the list)" ), iAAClntsToRewake.Count() );

    aAARewakedClients.Reset();

    const TInt AARewakeCount = iAAClntsToRewake.Count();
    for ( TInt ii = 0; ii < AARewakeCount; ii++ )
        {
        MCnUiClientPlugin* cPlugin = iAAClntsToRewake[ ii ];
        const TIMPSConnectionClient client = cPlugin->ClientID();
        User::LeaveIfError( aAARewakedClients.Append( client ) );
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeRewaker::ClearAARewakeList()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeRewaker::ClearAARewakeList()
    {
    IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnModeRewaker::ClearAARewakeList()" ) );
    iAAClntsToRewake.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeRewaker::RemoveFromAARewakeList()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeRewaker::RemoveFromAARewakeList( TIMPSConnectionClient aClient )
    {
    TInt ii = ( iAAClntsToRewake.Count() - 1 ); //last index is 1 smaller than total count
    for ( ; ii >= 0; ii-- )
        {
        MCnUiClientPlugin* cPlugin = iAAClntsToRewake[ ii ];
        if ( cPlugin->ClientID() == aClient )
            {
            IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnModeRewaker::RemoveFromAARewakeList( removed client [%d] )" ), aClient );
            iAAClntsToRewake.Remove( ii );
            delete cPlugin;
            }
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeRewaker::SuspendLC()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeRewaker::SuspendLC()
    {
    //Cancel the possibly running timer
    Cancel();

    //And push Resume operation to cleanup stack
    CleanupStack::PushL( TCleanupItem( ResumeRewakeStatic, this ) );
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeRewaker::ResumeRewake()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeRewaker::ResumeRewake()
    {
    //Cancel the possibly running timer
    Cancel();

    //And issue new timer event
    if ( iAAClntsToRewake.Count() > 0 )
        {
        IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnModeRewaker::ResumeRewake() - starting timeout timer for %d clients" ), iAAClntsToRewake.Count() );
        After( KReWakeUserActivityTimeout );
        }
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeRewaker::RunL()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeRewaker::RunL()
    {
    RewakeRegisteredClientsL();
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeRewaker::RunError()
// -----------------------------------------------------------------------------
//
TInt CCnUiConnModeRewaker::RunError( TInt aError )
    {
    iAAClntsToRewake.ResetAndDestroy();
    return aError;
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeRewaker::RewakeRegisteredClientsL()
// -----------------------------------------------------------------------------
//
void CCnUiConnModeRewaker::RewakeRegisteredClientsL()
    {
    const TInt clientsToReWakeCount = iAAClntsToRewake.Count();
    for ( TInt ii = 0; ii < clientsToReWakeCount; ii++ )
        {
        MCnUiClientPlugin* cPlugin = iAAClntsToRewake[ ii ];
        IMPSCUI_DP( D_IMPSCUI_LIT( "CCnUiConnModeRewaker::RewakeRegisteredClientsL( waking client [%d] )" ), cPlugin->ClientID() );
        cPlugin->ReWakeAutomaticConnectionModeL();
        }

    iAAClntsToRewake.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CCnUiConnModeRewaker::ClientAlreadyReqistered()
// -----------------------------------------------------------------------------
//
TBool CCnUiConnModeRewaker::ClientAlreadyReqistered( TIMPSConnectionClient aClient )
    {
    const TInt reqisteredClientsCount = iAAClntsToRewake.Count();
    for ( TInt ii = 0; ii < reqisteredClientsCount; ii++ )
        {
        MCnUiClientPlugin* cPlugin = iAAClntsToRewake[ ii ];
        if ( cPlugin->ClientID() == aClient )
            {
            return ETrue;
            }
        }

    return EFalse;
    }


//  End of File
