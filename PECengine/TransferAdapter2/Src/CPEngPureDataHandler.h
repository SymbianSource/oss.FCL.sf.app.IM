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

#ifndef __CPENGPUREDATAHANDLER_H__
#define __CPENGPUREDATAHANDLER_H__

//  INCLUDES
#include <E32Base.h>
#include <impspurecli.h>
#include "MPEngPureDataHandler.h"
#include "MPEngSessionStatusObserver.h"

// FORWARD DECLARATIONS
class CPEngSendDataOperation;
class CPEngSessionSlotId;
class RImpsEng;
class MPEngSessionManager;
class MPEngAccessHandler;

/**
 * CPEngPureDataHandler implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngPureDataHandler ) :
        public CBase,
        public MPEngPureDataHandler,
        public MImpsPureHandler2,
        public MImpsErrorHandler2,
        public MPEngSessionStatusObserver
    {
public: /* Construction */


    /**
     * Instantiates CPEngPureDataHandler object.
     *
     * @return New CPEngPureDataHandler instance.
     */
    static CPEngPureDataHandler* NewL( RImpsEng& aSessClient,
    MPEngSessionManager& aSessManager,
    MPEngAccessHandler& aAccessHandler,
    const CPEngSessionSlotId& aSessionId );


    /**
     * Destructor.
     */
    ~CPEngPureDataHandler();

    //-----------------------------------------------------------------------

public: // new functions from MPEngPureDataHandler

    /**
     *  Close reference to the Pure data handler
     *  @see <MPEngPureDataHandler.h>
     *  @since 3.0
     */
    void Close();

    /**
     *  Get Transfer buffer for out going message
     *  @see <MPEngPureDataHandler.h>
     *  @since 3.0
     */
    TPtr8 TransferBufferL();

    /**
     *  Sends data from the transfer buffer to the network
     *  @see <MPEngPureDataHandler.h>
     *  @since 3.0
     */
    TInt SendDataL( TRequestStatus& aStatus );

    /**
     *  Gets response for the message of given operation id
     *  @see <MPEngPureDataHandler.h>
     *  @since 3.0
     */
    HBufC8* ResponseL( TInt aTransId );

    /**
     *  Cancel sendin operation of given transaction id
     *  @see <MPEngPureDataHandler.h>
     *  @since 3.0
     */
    void CancelSending( TInt aTransId );

    /**
     *  Listen Incoming data
     *  @see <MPEngPureDataHandler.h>
     *  @since 3.0
     */
    TInt ListenIncomingData( TRequestStatus& aStatus );

    /**
     *  Cancel incoming data listening
     *  @see <MPEngPureDataHandler.h>
     *  @since 3.0
     */
    void CancelListening();

public: // new functions from MImpsPureHandler

    /**
     * Observer method for pure message content.
     * @see <impspurecli.h>
     * @seince 3.0
     */
    void HandlePureDataL( TInt aOpId,
                          const TDesC8& aBuffer,
                          TImpsCspIdentifier& aCspId );

public: // new functions from MImpsErrorHandler

    /**
     * Observer method for error messages.
     * @see <impspurecli.h>
     * @seince 3.0
     */
    void HandleErrorL( TInt aStatus,
                       TInt aOpId,
                       const TDesC* aDescription,
                       const CImpsDetailed* aDetailedRes,
                       TImpsCspIdentifier& aCspId );

public: //  new function from the MPEngSessionStatusObserver
    /**
     *  Handle Session status change
     *  @see <MPEngSessionStatusObserver.h>
     *  @since 3.0
     */
    void StatusChangedL( TPEngNWSessionSlotState aNewState,
                         TPEngNWSessionSlotEvent aNewEvent );

public: // New functions

    /**
     *  Open new reference to the object
     *
     *  @since 3.0
     */
    void Open();

private: // new private functions

    /**
     *  Find operation
     *
     *  @since 3.0
     *  @return index in array
     */
    TInt FindOperation( TInt aOpId, TInt ( CPEngSendDataOperation:: *aIdFunc )() const ) const;
    TInt FindOperationL( TInt aOpId, TInt ( CPEngSendDataOperation:: *aIdFunc )() const ) const;

protected: // construction

    /**
     * C++ constructor. protected
     */
    CPEngPureDataHandler( MPEngSessionManager& aSessManager,
                          MPEngAccessHandler& aAccessHandler,
                          const CPEngSessionSlotId& aSessionId,
                          RImpsEng& aSessClient );

    /**
     * Symbian Second phase constructor
     */
    void ConstructL( RImpsEng& aSessClient );

private: //Data

    /// OWN: Access count
    TInt                                    iCount;

    /// OWN: Session manager
    MPEngSessionManager&                    iSessionManager;

    /// OWN: Access handler
    MPEngAccessHandler&						iAccessHandler;

    /// REF: Session Id
    const CPEngSessionSlotId&               iSessionId;

    /// OWN: Max number of active requests
    TInt                                    iMaxActive;

    /// OWN: Transfer buffer, OWNED
    HBufC8*                                 iTrasferBuffer;

    /// OWN: Incoming data queue, OWNED
    RPointerArray<HBufC8>                   iIncomingData;

    // Incoming data Request Status
    TRequestStatus*                         iIncReqStatus;

    /// REF: Main client of the IMPS Engine
    RImpsEng&                               iSessClient;

    /// OWN: Pure client
    RImpsPureClient2                        iPureClient;

    /// OWN: Send data operations]
    ///     Elements OWNed
    RPointerArray<CPEngSendDataOperation>   iOperations;

    /// OWN: Id which is used to do logs more human readable
    TInt                                    iConnectionLogsId;

    /// OWN: Logs Id iteration,
    TInt                                    iOpidIterator;

    /// OWN: Last served op Id
    TInt                                    iLastServerOpId;
    };

#endif  //__CPENGPUREDATAHANDLER_H__

// End of File
