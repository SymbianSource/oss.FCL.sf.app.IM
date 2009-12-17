/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef __IMPSPLUGINACCESSHANDLER_H__
#define __IMPSPLUGINACCESSHANDLER_H__

//  INCLUDES
#include    <E32Base.h>
#include    <ImpsAccessCli.h>
#include    "PEngPresenceEngineConsts2.h"
#include    "PEngWVServices2.h"
#include    "MImpsPluginAccessHandler.h"

// FORWARD DECLARATIONS
class MPEngSessionStatusObserver;


/**
 * CImpsPluginAccessHandler
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CImpsPluginAccessHandler ) :
        public CBase,
        public MImpsPluginAccessHandler,
        public MImpsAccessHandler2,
        public MImpsErrorHandler2,
        public MImpsStatusHandler2
    {
public: /* Construction */


    /**
     * Instantiates CImpsPluginAccessHandler object.
     *
     * @return New CImpsPluginAccessHandler instance.
     */
    static CImpsPluginAccessHandler* NewL( RImpsEng& aSessClient );


    /**
     * Destructor.
     */
    ~CImpsPluginAccessHandler();

    //-----------------------------------------------------------------------
public: // from MImpsPluginAccessHandler


    /**
     *  Log in to the presence server
     *  @see <MImpsPluginAccessHandler>
     */
    void LogInL( TRequestStatus& aStatus,
                 TDesC &aPassword,
                 TDesC &aClientId,
                 TDesC &aServiceAddress,
                 TInt32 aIapId );

    /**
     *  Log out from the presence server
     *  @see <MImpsPluginAccessHandler>
     */
    void LogOutL( TRequestStatus& aStatus );


    /**
     *  Cancels active asynchronous operation
     *  @see <MImpsPluginAccessHandler>
     */
    void Cancel( TInt aReason );

    /**
     *  Register session status observer
     *  @see <MImpsPluginAccessHandler>
     */
    void RegisterSessionObserverL(
        MPEngSessionStatusObserver& aObserver );

    /**
     *  UnRegister session status observer
     *  @see <MImpsPluginAccessHandler>
     */
    void UnregisterSessionObserver(
        MPEngSessionStatusObserver& aObserver );

    /**
     *  Get session service capabilities
     *  @see <MImpsPluginAccessHandler>
     */
    TPEngWVCspServicesTree2& ServiceCapabilities();


public: // MImpsAccessHandler2

    /**
     * Observer method for Login and Open event.
     * @see <impspurecli.h>
     */
    void HandleLoginL( TInt aId,
                       TImpsCspIdentifier& aCspId );

    /**
     * Observer method for Logout event.
     * @see <impspurecli.h>
     */
    void HandleLogoutL( TInt aId,
                        TImpsCspIdentifier& aCspId );

    /**
     * Observer method for CancelLogin event.
     * @see <impspurecli.h>
     */
    void HandleLoginCancelL( TInt aCancelledOpId,
                             TImpsCspIdentifier& aCspId );

    /**
     * Observer for NumberOfSessions and NumberOfSubSessions response
     * @see <impspurecli.h>
     */
    void HandleNbrSessionsL( TInt aId,
                             TInt aNbr,
                             TImpsCspIdentifier& aCspId );

public: // MImpsErrorHandler

    /**
     * Observer method for error messages.
     * @see <impspurecli.h>
     */
    void HandleErrorL( TInt aStatus,
                       TInt aOpId,
                       const TDesC* aDescription,
                       const CImpsDetailed* aDetailedRes,
                       TImpsCspIdentifier& aCspId );

public: // MImpsStatusHandler
    /**
     * Observer method for changes in service status.
     * @see <MImpsStatusHandler.h>
     */
    void HandleStatusChangeL( TImpsServiceStatus aStatus,
                              TImpsCspIdentifier& aCspId );

private: // new private functions

    /**
     *  Copy service tree
     *
     *  @since 3.0
     *  @param WV Engine service tree
     */
    void CopyServiceTree( TImpsServices& aFrom );

    /**
     *  Notify if new state
     *
     *  @since 3.0
     *  @param aNewState
     *  @param aEvent
     */
    void NotifyIfNewStateL( TPEngNWSessionSlotState aNewState,
                            TPEngNWSessionSlotEvent aEvent );


    /**
     *  Handle completion of the log in request
     *
     *  @since 3.0
     */
    void HandleLogInCompletionL();



protected: // construction

    /**
     * C++ constructor. protected
     */
    CImpsPluginAccessHandler();

    /**
     * Symbian Second phase constructor
     */
    void ConstructL( RImpsEng& aSessClient );

private: //Data
    enum TPEngOperation
        {
        EIdle           = 0,
        EDoingLogIn     = 1,
        EDoingLogOut    = 2,
        ECanceling      = 3
        };

    /// OWN: State of the session
    TPEngNWSessionSlotState     iSessStatus;

    /// OWN: Service tree of the session
    TPEngWVCspServicesTree2     iServices;

    /// OWN: TBool service tree fetched
    TBool                       iServiceFetched;

    /// REF: Session state observer
    RPointerArray<MPEngSessionStatusObserver> iObservers;

    /// OWN: Access client of WV Engine
    RImpsAccessClient2          iAccessClient;

    /// OWN: Operation Id from the WV Server
    TInt                        iOpId;

    /// REF: Request Status of the active request
    TRequestStatus*             iStatus;

    /// OWN: Active operation
    TPEngOperation              iOperation;

    };

#endif  //__IMPSPLUGINACCESSHANDLER_H__

// End of File
