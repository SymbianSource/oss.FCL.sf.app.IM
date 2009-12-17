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



#ifndef __CCNUICLIENTGROUPUINOTIFYHANDLER_H
#define __CCNUICLIENTGROUPUINOTIFYHANDLER_H

// INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>
#include "MCnUiConnQueryObserver.h"
#include "CCnUiGroupChannel.h"


//FORWARD DECLARATION
class MCnUiUiFacade;
class MCnUiConnQuery;



// CLASS DECLARATION
/**
 * Client group remote notify handler.
 *
 * Handles client group remote UI notifications.
 * (Used as client side pair to CCnUiClientGroupUiNotifyMediator)
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiClientGroupUiNotifyHandler ) : public CBase,
        public MCnUiGroupChannelListener,
        public MCnUiConnQueryObserver
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor, constructs the
     * CCnUiClientGroupUiNotifyHandler object.
     *
     * @since 2.1
     * @param aClient The client.
     * @param aUi The UI to use.
     * @return is pointer to CCnUiClientGroupUiNotifyHandler object.
     */
    static CCnUiClientGroupUiNotifyHandler* NewL( TIMPSConnectionClient aClient );

    /**
     * Destructor.
     */
    virtual ~CCnUiClientGroupUiNotifyHandler();


private: //Constructors

    /**
     * C++ default constructor.
     */
    CCnUiClientGroupUiNotifyHandler( TIMPSConnectionClient aClient );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();


public: //New methods

    /**
     * Checks is certain remote UI message handled.
     *
     * @since 2.1
     * @param Remote message to check. (See CnUiCommon.hrh
     * for remote messages.)
     * @param ETrue if remote message is handled. Else EFalse.
     */
    TBool Handled( TInt aRemoteUiMessage );


    /**
     * Sets certain remote UI message handling status.
     *
     *
     * @since 2.1
     * @param Remote message to set. (See CnUiCommon.hrh
     * for remote messages.)
     * @param aHandled ETrue if message should be handled.
     * EFalse if message handling should be stopped.
     * By default all known message handlings are stopped.
     * If not supported message type is tried to enable / disable,
     * does nothing.
     */
    void SetHandled( TInt aRemoteUiMessage, TBool aHandled );


    /**
     * Notifies from presence event.
     *
     * @since 2.1
     * @param aEvent The presence event.
     */
    void HandlePresenceEventL( TIMPSPresenceServiceEvent aEvent );

    /**
     * Sets the new resource file to use in Connection UI.
     *
     * @param aNewResourceFile the new resourcefile to be used
     * @since 2.6
     */
    void UseResourceFileL( const TDesC& aNewResourceFile );

public: //from MCnUiGroupChannelListener

    /**
     * Callback method for channel events.
     *
     * @since 2.1
     * @param aGroupId    The originating channel group.
     * @param aChannelId  The originating channel.
     * @param aChannelMsg The channel message.
     */
    void HandleChannelMsg( TInt aGroupId,
                           TGCChannelID aChannelId,
                           TInt aChannelMsg );

public: //from MCnUiConnQueryObserver

    /**
     * Notify method for confirmation query result.
     * @since 2.1
     */
    void HandleQueryResult( TBool aQueryAccepted );


private: //helpers
    void ReportQueryResultL( TBool aQueryAccepted );
    void CancelCurrentQuery();
    void DoHandleChannelMsgL( TInt aChannelMsg );



private: //data

    ///<Client for which behalf working. Owned.
    const TIMPSConnectionClient             iClient;

    ///<The UI to use. Owned.
    MCnUiUiFacade*                          iUi;

    ///<Client group channel to get remote UI messages. Owned.
    CCnUiGroupChannel*                      iGroupChannel;

    ///<Pointer to currently active query. Owned.
    MCnUiConnQuery*                         iCurrentQuery;


    ///< Flags describing the handled events, owned
    ///< Flag contents are either EFalse or ETrue
    TBool       iActiveConnectionScheduledClose;
    TBool       iActiveConnectionSuppressForScheduledConnection;

    ///<Resource file name, owned
    HBufC*									iBrandedResourceFileName;
    };

#endif      //__CCNUICLIENTGROUPUINOTIFYHANDLER_H

//  End of File

