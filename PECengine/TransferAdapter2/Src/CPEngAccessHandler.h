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

#ifndef __CPENGACCESSHANDLER_H__
#define __CPENGACCESSHANDLER_H__

//  INCLUDES
#include    <E32Base.h>
#include    <ImpsAccessCli.h>
#include    "PEngPresenceEngineConsts2.h"
#include    "PEngWVServices2.h"

#include    "MPEngAccessHandler.h"

// FORWARD DECLARATIONS
class CPEngSessionSlotId;
class MPEngSessionStatusObserver;
class MPEngSessionManager;

/**
 * NWSessionSlotID implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAccessHandler ) :
        public CBase,
        public MPEngAccessHandler,
        public MImpsAccessHandler2,
        public MImpsErrorHandler2,
        public MImpsStatusHandler2
    {
public: /* Construction */


    /**
     * Instantiates CPEngAccessHandler object.
     *
     * @return New CPEngAccessHandler instance.
     */
    static CPEngAccessHandler* NewL( RImpsEng& aSessClient,
    MPEngSessionManager& aSessManager,
    const CPEngSessionSlotId& aSessionId );


    /**
     * Destructor.
     */
    ~CPEngAccessHandler();

    //-----------------------------------------------------------------------
public: // from MPEngAccessHandler
    /**
     *  Close reference to the Access handler
     *  @see <MPEngAccessHandler.h>
     */
    void Close();

    /**
     *  Open new reference to the object
     *  @see <MPEngAccessHandler.h>
     */
    void Open();

    /**
     *  Log in to the presence server
     *  @see <MPEngAccessHandler.h>
     */
    void LogInL( TRequestStatus& aStatus, HBufC8* aLoginBuffer );

    /**
     *  Log out from the presence server
     *  @see <MPEngAccessHandler.h>
     */
    void LogOutL( TRequestStatus& aStatus );

    /**
     *  Returns data which was used for log in.
     *  @see <MPEngAccessHandler.h>
     */
    const TDesC8& LogInData( ) const;

    /**
     *  Cancels active asynchronous operation
     *  @see <MPEngAccessHandler.h>
     */
    void Cancel();

    /**
     *  Register session status observer
     *  @see <MPEngAccessHandler.h>
     */
    void RegisterSessionObserverL(
        MPEngSessionStatusObserver& aObserver );

    /**
     *  UnRegister session status observer
     *  @see <MPEngAccessHandler.h>
     */
    void UnregisterSessionObserver(
        MPEngSessionStatusObserver& aObserver );

    /**
     *  Get session service capabilities
     *  @see <MPEngAccessHandler.h>
     */
    TPEngWVCspServicesTree2& ServiceCapabilities();

    /**
     *  Assert log in data of opened session with the passed data
     *  @see <MPEngAccessHandler.h>
     */
    void AssertLoginDataL( TDesC8& aLogInData );


public: // MImpsAccessHandler

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
    void HandleLogInCompletionL( );



protected: // construction

    /**
     * C++ constructor. protected
     */
    CPEngAccessHandler( MPEngSessionManager& aSessManager,
                        const CPEngSessionSlotId& aSessionId );

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

    /// OWN: Access count
    TInt                        iCount;

    /// REF: Session manager
    MPEngSessionManager&        iSessionManager;

    /// REF: Session Slot Id
    const CPEngSessionSlotId&   iSessionId;

    /// OWN: State of the session
    TPEngNWSessionSlotState     iSessStatus;

    /// OWN: Buffer with log in Data
    HBufC8*                     iLogInData;

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

#endif  //__CPENGACCESSHANDLER_H__

// End of File
