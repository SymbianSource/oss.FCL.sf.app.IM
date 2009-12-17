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
* Description:  Connection opener.
*
*/

#ifndef __CCNUICONNOPENER_H
#define __CCNUICONNOPENER_H

//  INCLUDES
#include <E32base.h>
#include <impspresenceconnectionuiconstsng.h>
#include <MPEngNWSessionOperationObserver2.h>


//DATA TYPES

/**
 * Connection closer state enumerations.
 * These state id's are reported to
 * given MCnUiOpProgressStateObserver
 * operation progress observer
 *
 * NOTE!! The states ECnOpener_ClosingOwnedSAPConnection &
 * ECnOpenerClosingInterferingSAPConnection
 * perform real network operations, but canceling
 * those isn't supported.
 */
enum TCnUiConnOpenerState
    {
    ECnOpenerIdle = 0,                  ///<Idle state, no operation running
    ECnOpenerOpeningSAPConnection = 1,  ///<Waiting the SAP connection to open
    ECnOpenerProcessingPlugin = 2,      ///<Waiting client plug-in processing
    ECnOpenerClosingOwnedSAPConnection = 3,   ///<Waiting the SAP connection to close
    ECnOpenerClosingInterferingSAPConnection = 4   ///<Waiting the SAP connection to close
    };



//FORWARD DECLARATION
class CIMPSSAPSettings;
class MCnUiConnectionHandler;
class MCnUiClientPlugin;
class MCnUiOpProgressStateObserver;
class CPEngNWSessionSlotID2;
class MPEngTransactionStatus2;

// CLASS DECLARATION

/**
 * Connection opener.
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiConnOpener ) : protected CActive,
        public MPEngNWSessionOperationObserver2
    {
private: //helper class

    /**
     * Private helper class to manager
     * state information and notify state
     * observer.
     * @since 2.1
     */
    class TOpenerStateProxy
        {
        public: //new methods
            /**
             * C++ default constructor.
             * @param aInitial The initial state.
             */
            TOpenerStateProxy( TCnUiConnOpenerState aInitial );

            /**
             * Sets new state observer. Observer may be NULL.
             * @since 2.1
             * @param aObs The observer to set and notify further
             * state changes.
             */
            void SetProgressObserver( MCnUiOpProgressStateObserver* aObs );

            /**
             * Sets new state and notifies registered observer if any..
             * @since 2.1
             * @param aNewState The new state.
             * @param aClient The client for which the state was entered
             */
            void Set( TCnUiConnOpenerState aNewState, TIMPSConnectionClient aClient );

            /**
             * Gets current state.
             * @since 2.1
             * @return The current state.
             */
            TCnUiConnOpenerState operator()();


        private: //data

            ///<Current state, owned
            TCnUiConnOpenerState            iState ;

            ///<Observer, not owned, may be NULL
            MCnUiOpProgressStateObserver*   iObs;
        };


public:  // Two-phased constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCnUiConnOpener* NewL();

    /**
    * Destructor.
    */
    virtual ~CCnUiConnOpener();


private:

    /**
     * C++ default constructor.
     */
    CCnUiConnOpener();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();


public: // New functions

    /**
     * Login service.
     *
     * @since 2.1
     * @param aClient The client to login.
     * @param aSap The SAP to login.
     * @param aPlugin The client plug-in to use in login processing.
     * @param aConnHandler The connection handler to use in login
     * @param aProgressObserver The possible progress observer,
     * may be NULL.
     * @return Operation status. This can be either local
     * error code coming from starting the operation
     * or real network error code.
     */
    TInt MakeLoginForClient( TIMPSConnectionClient aClient,
                             const CIMPSSAPSettings& aSap,
                             MCnUiClientPlugin& aPlugin,
                             MCnUiConnectionHandler& aConnHandler,
                             MCnUiOpProgressStateObserver* aProgressObserver,
                             CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Cancels the login.
     *
     * @since 2.1
     */
    void CancelLogin();

public: // Methods from MPEngNWSessionOperationObserver2

    /**
     * From MPEngNWSessionOperationObserver2.
     * See it for details.
     *
     * @since 3.0
     */
    void HandleNWSessionOperationCompleteL(
        MPEngTransactionStatus2& aStatus,
        CPEngNWSessionSlot2& aNWSessionSlot,
        TInt aNWSessionSlotOperation );

    /**
     * From MPEngNWSessionOperationObserver2.
     * See it for details.
     *
     * @since 3.0
     */
    void HandleNWSessionOperationNotifyError(
        TInt aError,
        CPEngNWSessionSlot2& aNWSessionSlot,
        TInt aNWSessionSlotOperation );

public: // methods from CActive
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

    /**
     * From CActive.
     * @since S60 v3.0
     * @see CActive
     */
    void Cancel();

    /**
     * From CActive.
     * @since S60 v3.0
     * @see CActive
     */
    TBool IsActive() const;

    /**
    * Verifies SAP compulsory contents.
    *
    * @since 2.1
    * @param aSap The SAP to check
    * @return KErrNone if has required compulsory settings,
    * else KConnUiErrorSapMissingCompulsoryFields.
    */
    static TInt CheckConnOpenCompulsorySettingsL( const CIMPSSAPSettings& aSap );

private: // New private helper methods

    /**
     * Waits the started asynchronous steps
     * to complete using the CActiveSchedulerWait.
     * @param aWaiter which CActiveSchedulerWait to start
     *
     * @since 2.1
     */
    void WaitCompletion( CActiveSchedulerWait& aWaiter );


    /**
     * Stops the previously started
     * CActiveSchedulerWait wait loop.
     * @param aWaiter which CActiveSchedulerWait to stop
     *
     * @since 2.1
     */
    void Completed( CActiveSchedulerWait& aWaiter );


    /**
     * Starts the SAP login if needed.
     *
     * @since 2.1
     * @param aCloseOfInterferingConnectionAllowed If ETrue then
     * possibly existing interfering SAP connection is tried to
     * close. If EFalse, then it isn't.
     */
    void HandleSapConnectionOpenRequestL( TBool aCloseOfInterferingConnectionAllowed );


    /**
     * Issues the real asynhronous SAP
     * connection open.
     *
     * @since 2.1
     */
    void IssueConnectionOpenL();


    /**
     * Handles SAP connection succesfully
     * opened case. Checks has the SAP enough
     * capabilites for current client etc.
     *
     * @since 2.1
     */
    void HandleSapConnectionOpenedL();


    /**
     * Issues the client post login processing.
     *
     * @since 2.1
     */
    void IssuePluginPostLoginProcessingL();


    /**
     * Closes the owned NW connection and sets client logged out
     * @since 2.1
     */
    void UndoConnectionStateChanges();


    /**
     * Issues the connection close.
     *
     * Connection close is needed when the
     * SAP doesn't have enough capabilities
     * for client or the login is canceled
     * after the network connection is
     * succesfully opened.
     *
     * @since 2.1
     */
    void IssueSapConnectionCloseIfNeededL();


    /**
     * Issues the close of interfering SAP connection.
     *
     * Interfering SAP connection is silently closed
     * when there is noticed to be a open SAP
     * connection but no known client is
     * logged in.
     *
     * @since 2.1
     */
    void IssueCloseOfInterferingSapConnectionL();


    /**
     * Resets previously cached operation
     * parameters.
     * @since 2.1
     */
    void ResetCachedParameters();


    /**
     * Resets internal state flags.
     *
     * @since 2.1
     */
    void ResetStateFlags();

    /**
     * Handles operation completions
     *
     * @since 3.0
     */
    void HandleOperationCompletionsL();

    /**
     * Creates a network session slot copy
     *
     * @since 3.0
     */
    CPEngNWSessionSlotID2* CopyNWSessionSlotL( CPEngNWSessionSlotID2& aSessionSlot );


private:    // Data

    ///< The internal state proxy, owned
    TOpenerStateProxy               iState;

    ///<Active Scheduler Wait, owned
    CActiveSchedulerWait            iOpenWait;

    ///<Active Scheduler Wait, for PEC waiting, owned
    CActiveSchedulerWait            iCancelWaiter;

    ///<State flag. Did this operation open the SAP connection, owned
    TBool                           iOwnsSapConnection;

    ///<State flag. Was this operation canceled, owned
    TBool                           iCanceled;

    ///<Cached operation resource, not owned
    MCnUiConnectionHandler*         iConnHandler;

    ///<Cached operation resource, not owned
    MCnUiClientPlugin*              iPlugin;

    ///<Cached operation resource, not owned
    const CIMPSSAPSettings*           iSap;

    ///<Cached operation resource, owned
    TIMPSConnectionClient           iClient;

    MPEngTransactionStatus2*        iTransactionStatus;
    TInt                            iStatusInt;

    // owns: network session slot id
    CPEngNWSessionSlotID2*          iNWSessionSlotID;

    };

#endif      // __CCNUICONNOPENER_H

// End of File
