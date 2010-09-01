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
* Description:  Connection closer.
*
*/

#ifndef __CCNUICONNCLOSER_H
#define __CCNUICONNCLOSER_H

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
 */
enum TCnUiConnCloserState
    {
    ECnCloser_Idle = 100,                  ///<Idle state, no operation running
    ECnCloser_ProcessingPlugin = 101,      ///<Waiting client plug-in processing
    ECnCloser_ClosingSAPConnection = 102   ///<Waiting the SAP connection to close
    };



//FORWARD DECLARATION
class CIMPSSAPSettings;
class MCnUiOpContext;
class MCnUiConnectionHandler;
class MCnUiClientPlugin;
class MCnUiOpProgressStateObserver;
class CPEngNWSessionSlotID2;

// CLASS DECLARATION

/**
 * Connection closer.
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiConnCloser ) : protected CActive,
        public MPEngNWSessionOperationObserver2
    {
private: //helper class
    /**
     * Private helper class to manager
     * state information and notify state
     * observer.
     * @since 2.1
     */
    class TCloserStateProxy
        {
        public: //new methods

            /**
             * C++ default constructor.
             * @param aInitial The initial state.
             */
            TCloserStateProxy( TCnUiConnCloserState aInitial );

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
            void Set( TCnUiConnCloserState aNewState, TIMPSConnectionClient aClient );

            /**
             * Gets current state.
             * @since 2.1
             * @return The current state.
             */
            TCnUiConnCloserState operator()();


        private: //data

            ///<Current state, owned
            TCnUiConnCloserState            iState ;

            ///<Observer, not owned, may be NULL
            MCnUiOpProgressStateObserver*   iObs;

        };


public:  // Two-phased constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCnUiConnCloser* NewL();

    /**
    * Destructor.
    */
    virtual ~CCnUiConnCloser();


private:

    /**
     * C++ default constructor.
     */
    CCnUiConnCloser();


protected:  // From CActive

    /**
     * RunL().
     */
    void RunL();


    /**
     * RunError().
     *
     * @since 2.1
     * @param aError The leave code from RunL leave.
     * Stores this as iStatus member.
     * @return Always KErrNone.
     */
    TInt RunError( TInt aError );


    /**
     * DoCancel().
     *
     * @since 2.1
     */
    void DoCancel();


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


public: // New functions

    /**
     * Logout service.
     *
     * @since 2.1
     * @param aClient The client to login.
     * @param aSap The SAP to login.
     * @param aPlugin The client plugin to use in login processing.
     * @param aConnHandler The connection handler to use in login
     * @param aProgressObserver The possible progress observer,
     * may be NULL.
     * @return Operation status. This can be either local
     * error code coming from starting the operation
     * or real network error code.
     */
    TInt MakeLogoutForClient( TIMPSConnectionClient aClient,
                              const CIMPSSAPSettings& aSap,
                              MCnUiClientPlugin& aPlugin,
                              MCnUiConnectionHandler& aConnHandler,
                              MCnUiOpProgressStateObserver* aProgressObserver,
                              CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Cancels the logout.
     *
     * @since 2.1
     */
    void CancelLogout();


    /**
     * Gets wether this connection closer
     * instance has really closed the network
     * connection or not.
     *
     * @since 2.1
     * @return ETrue if connection really closed.
     * Else EFalse.
     */
    TBool ConnectionReallyClosed();


private: // New private helper methods

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
     * Handles the client logout request.
     * Starts asynchronous operation
     * if really needed.
     *
     * @since 2.1
     */
    void HandleClientLoggoutRequestL();


    /**
     * Issues plugin pre loggout processing.
     *
     * @since 2.1
     */
    void IssuePluginPreLoggoutProcessingL();


    /**
     * Issues SAP connection close processing.
     *
     * @since 2.1
     */
    void IssueSapConnectionCloseL();


    /**
     * Resets previoysly cached operation
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
    TCloserStateProxy               iState;

    ///<Active Scheduler Wait, owned
    CActiveSchedulerWait            iWait;

    ///<State flag. Was this operation canceled, owned
    TBool                           iCanceled;

    ///<State flag. Has this operation closed really NW connection, owned
    TBool                           iConnectionReallyClosed;

    ///<Cached operation resource, not owned
    MCnUiConnectionHandler*         iConnHandler;

    ///<Cached operation resource, not owned
    MCnUiClientPlugin*              iPlugin;

    ///<Cached operation resource, not owned
    const CIMPSSAPSettings*           iSap;

    ///<Cached operation resource
    TIMPSConnectionClient           iClient;

    MPEngTransactionStatus2*        iTransactionStatus;

    // owns: does not own
    CPEngNWSessionSlotID2*          iNWSessionSlotID;

    };

#endif      // __CCNUICONNCLOSER_H

// End of File
