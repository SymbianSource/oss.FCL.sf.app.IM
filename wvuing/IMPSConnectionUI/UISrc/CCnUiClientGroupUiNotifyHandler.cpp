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
* Description:  Client group remote notify handler.
*
*/


// INCLUDE FILES
#include <E32std.h>
#include "CCnUiClientGroupUiNotifyHandler.h"
#include "MCnUiUiFacade.h"
#include "CnUiCommon.hrh"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCnUiClientGroupUiNotifyHandler* CCnUiClientGroupUiNotifyHandler::NewL(
    TIMPSConnectionClient aClient )
    {
    CCnUiClientGroupUiNotifyHandler* self =
        new ( ELeave ) CCnUiClientGroupUiNotifyHandler( aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CCnUiClientGroupUiNotifyHandler::~CCnUiClientGroupUiNotifyHandler()
    {
    CancelCurrentQuery();
    delete iGroupChannel;
    delete iBrandedResourceFileName;
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiClientGroupUiNotifyHandler::CCnUiClientGroupUiNotifyHandler( TIMPSConnectionClient aClient )
        : iClient( aClient )
    {
    }


// EPOC default constructor can leave.
void CCnUiClientGroupUiNotifyHandler::ConstructL()
    {
    iGroupChannel = CCnUiGroupChannel::NewL( iClient, ECnUiRemoteUiNotificationsChannel );
    iGroupChannel->ListenL( this );
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyHandler::Handled()
// -----------------------------------------------------------------------------
//
TBool CCnUiClientGroupUiNotifyHandler::Handled( TInt aRemoteUiMessage )
    {
    switch ( aRemoteUiMessage )
        {
        case ECGQActiveConnectionScheduledClose:
            {
            return iActiveConnectionScheduledClose;
            }

        case ECGQActiveConnectionSuppressForScheduledConnection:
            {
            return iActiveConnectionSuppressForScheduledConnection;
            }

        default:
            {
            break;
            }
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyHandler::SetHandled()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyHandler::SetHandled( TInt aRemoteUiMessage, TBool aHandled )
    {
    switch ( aRemoteUiMessage )
        {
        case ECGQActiveConnectionScheduledClose:
            {
            iActiveConnectionScheduledClose = aHandled;
            break;
            }

        case ECGQActiveConnectionSuppressForScheduledConnection:
            {
            iActiveConnectionSuppressForScheduledConnection = aHandled;
            break;
            }

        default:
            {
            break;
            }
        }

    if ( aHandled )
        {
        //some service started
        //launch the notification if notify request is already issued
        TInt channelMsg = KErrNotFound;
        if ( iGroupChannel->Read( channelMsg ) == KErrNone )
            {
            HandleChannelMsg( iGroupChannel->GroupID(),
                              iGroupChannel->ChannelId(),
                              channelMsg );
            }
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyHandler::HandlePresenceEventL()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyHandler::HandlePresenceEventL( TIMPSPresenceServiceEvent aEvent )
    {
    if ( ( aEvent == EIMPSPresenceServiceOffline ) ||
         ( aEvent == EIMPSPresenceServiceForceLogOut ) )
        {
        //current connection is lost ==> dismiss possibly shown queries
        //AA side will eventually after timeout figure out the result
        CancelCurrentQuery();
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyHandler::HandleChannelMsg()
// MCnUiGroupChannelListener
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyHandler::HandleChannelMsg( TInt /*aGroupId*/,
                                                        TGCChannelID /*aChannelId*/,
                                                        TInt aChannelMsg )
    {
    TRAPD( err, DoHandleChannelMsgL( aChannelMsg ) );
    MCnUiUiFacade* tempUi = NULL;
    TRAP( err, tempUi = CreateUiFacadeL() );
    if ( err )
        {
        // if we don't get facade we just don't show anything
        return;
        }

    delete iUi;
    iUi = tempUi;

    if ( iBrandedResourceFileName )
        {
        TRAP( err, iUi->SwitchResourceFileL( *iBrandedResourceFileName ) );
        if ( err != KErrNone )
            {
            CActiveScheduler::Current()->Error( err );
            }
        }

    iUi->HandleIfError( err );

    delete iUi;
    iUi = NULL;
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyHandler::HandleQueryResult()
// From MCnUiConnQueryObserver
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyHandler::HandleQueryResult( TBool aQueryAccepted )
    {
    TRAPD( err, ReportQueryResultL( aQueryAccepted ) );

    MCnUiUiFacade* tempUi = NULL;
    TRAP( err, tempUi = CreateUiFacadeL() );
    if ( err )
        {
        // if we don't get facade we just don't show anything
        return;
        }

    delete iUi;
    iUi = tempUi;

    if ( iBrandedResourceFileName )
        {
        TRAP( err, iUi->SwitchResourceFileL( *iBrandedResourceFileName ) );
        if ( err != KErrNone )
            {
            CActiveScheduler::Current()->Error( err );
            }
        }

    iUi->HandleIfError( err );

    delete iUi;
    iUi = NULL;
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyHandler::ReportQueryResultL()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyHandler::ReportQueryResultL( TBool aQueryAccepted )
    {
    if ( aQueryAccepted )
        {
        iGroupChannel->WriteL( ERemoteQueryAccepted );
        }
    else
        {
        iGroupChannel->WriteL( ERemoteQueryDeclined );
        }
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyHandler::CancelCurrentQuery()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyHandler::CancelCurrentQuery()
    {
    delete iCurrentQuery;
    iCurrentQuery = NULL;
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyHandler::DoHandleChannelMsgL()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyHandler::DoHandleChannelMsgL( TInt aChannelMsg )
    {
    //which ever event happens in channel, clear active query
    //==> queries are cleared when new query is issued & some client reports selection
    CancelCurrentQuery();

    //if a channel message is a known & handled query id ==> show the query
    if ( !Handled( aChannelMsg ) )
        {
        return;
        }

    MCnUiUiFacade* tempUi = CreateUiFacadeL();

    delete iUi;
    iUi = tempUi;

    if ( iBrandedResourceFileName )
        {
        iUi->SwitchResourceFileL( *iBrandedResourceFileName );
        }

    //make a proper non waiting query
    switch ( aChannelMsg )
        {
        case ECGQActiveConnectionScheduledClose:
            {
            iCurrentQuery = iUi->QueryL( ECnUiQueryActiveConnectionScheduledClose,
                                         *this );

            break;
            }

        case ECGQActiveConnectionSuppressForScheduledConnection:
            {
            HBufC* serverName = NULL;
            iGroupChannel->ReadL( serverName );
            CleanupStack::PushL( serverName );
            iCurrentQuery = iUi->QueryL( ECnUiQueryActiveConnectionSuppressForScheduledConnection,
                                         *serverName,
                                         *this );
            CleanupStack::PopAndDestroy( serverName ); //serverName
            break;
            }

        default:
            {
            break;
            }
        }

    delete iUi;
    iUi = NULL;
    }


// -----------------------------------------------------------------------------
// CCnUiClientGroupUiNotifyHandler::UseResourceFileL()
// -----------------------------------------------------------------------------
//
void CCnUiClientGroupUiNotifyHandler::UseResourceFileL( const TDesC& aNewResourceFile )
    {
    if ( aNewResourceFile.Length() == 0 )
        {
        //empty resource file name ==> use the default resource
        delete iBrandedResourceFileName;
        iBrandedResourceFileName = NULL;
        }
    else
        {
        // use this resourcefile
        HBufC* tempFile = aNewResourceFile.AllocL();
        delete iBrandedResourceFileName;
        iBrandedResourceFileName = tempFile;
        }
    }



// end of file
