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
* Description:  Synchronous client group notify mediator.
*
*/



#ifndef __CCNUICLIENTGROUPUINOTIFYMEDIATOR_H
#define __CCNUICLIENTGROUPUINOTIFYMEDIATOR_H

// INCLUDES
#include <E32Base.h>
#include "CCnUiGroupChannel.h"
#include "MCnUiGlobalNotificationUiFacade.h"


// CLASS DECLARATION
/**
 * Synchronous client group notify mediator.
 *
 * Client group notifications and queries return after
 * user remote selection or timeout.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiClientGroupUiNotifyMediator ) : public CTimer,
        public MCnUiGroupChannelListener
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor, constructs the
     * CCnUiClientGroupUiNotifyMediator object.
     *
     * @since 2.1
     * @return is pointer to CCnUiClientGroupUiNotifyMediator object.
     */
    static CCnUiClientGroupUiNotifyMediator* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCnUiClientGroupUiNotifyMediator();


private: //Constructors

    /**
     * C++ default constructor.
     */
    CCnUiClientGroupUiNotifyMediator();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();


public://New methods


    /**
     * Executes active connection for scheduled
     * suppression remote query.
     *
     * @since 2.1
     * @param aServerName The server name to establish the
     * scheduled connection.
     * @param aClientsToShow The clients in which to show
     * the query.
     * @return ETrue if current connection can be closed.
     * Else EFalse.
     */
    TBool CGQActiveConnectionSuppressForScheduledConnectionL(
        const TDesC& aServerName,
        const TArray<TIMPSConnectionClient>& aClientsToShow );


    /**
     * Executes active connection scheduled close query.
     *
     * @since 2.1
     * @param aClientsToShow The clients in which to show
     * the query.
     * @return ETrue if current connection can be closed.
     * Else EFalse.
     */
    TBool CGQActiveConnectionScheduledCloseL(
        const TArray<TIMPSConnectionClient>& aClientsToShow );




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




protected:  // Methods derived from CActive

    /**
     * From CActive.
     * @see CActive
     */
    void RunL();

    /**
     * From CActive.
     * @see CActive
     */
    void DoCancel();

    /**
     * From CActive.
     * @see CActive
     */
    TInt RunError( TInt aError );



private: // New private helper methods

    /**
     * Sends the real query to the client and waits it completion
     * or timeout. Returns user selection.
     *
     * @since 2.1
     * @param aQueryMessageId The query ID. See
     * CnUiCommon.hrh for available remote UI messages.
     * @param aDynamicInsertText The dynamic insert text
     * to use in note.
     * @param aQueryTimeout The query timeout.
     * @param aDefaultResult The default result for timeout cases.
     * @param aClientsToShow The clients in which to show the query.
     * @return ETrue if the query was accepted, else EFalse.
     */
    TBool IssueTheQueryL( TInt aQueryMessageId,
                          const TDesC& aDynamicInsertText,
                          TTimeIntervalMicroSeconds32 aQueryTimeout,
                          TBool aDefaultResult,
                          const TArray<TIMPSConnectionClient>& aClientsToShow );

    /**
     * Waits the started asynchronous steps
     * to complete using the CActiveSchedulerWait.
     *
     * @since 2.1
     */
    void WaitCompletion();


    /**
     * Stops the previously started
     * CActiveSchedulerWait wait loop.
     *
     * @since 2.1
     */
    void Completed();


    /**
     * Issues the message to given groups.
     *
     * @since 2.1
     * @param aRemoteMessage The remote message to send.
     * @param aExtraData The remote message extra data.
     * @param aClientGroups The client groups which to send
     */
    void IssueMessageToGroupsL( TInt aRemoteMessage,
                                const TDesC& aExtraData,
                                const TArray<TIMPSConnectionClient>& aClientGroups );

    /**
     * Cancels the previously issued channel messages.
     *
     * @since 2.1
     */
    void CancelActiveChannels();


    /**
     * Removes identified channel among the currently
     * active ones. If there is found several channels with
     * given id's, removes all.
     *
     * @since 2.1
     */
    void RemoveFromActiveChannels( TInt aGroupId, TGCChannelID aChannelId );


private: //data

    ///< Active Scheduler Wait, owned
    CActiveSchedulerWait                    iWait;

    ///< Active group channels, owned
    RPointerArray<CCnUiGroupChannel>        iGroupChannels;

    ///<Query result, owned
    TBool                                   iQueryResult;
    };

#endif      //__CCNUICLIENTGROUPUINOTIFYMEDIATOR_H

//  End of File

