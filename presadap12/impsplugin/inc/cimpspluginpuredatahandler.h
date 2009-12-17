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

#ifndef __CIMPSPLUGINPUREDATAHANDLER_H__
#define __CIMPSPLUGINPUREDATAHANDLER_H__

//  INCLUDES
#include <E32Base.h>
#include <impspurecli.h>

#include <F32File.h>
#include "MImpsPluginAccessHandler.h"
#include "MImpsPluginPureDataHandler.h"
#include "MPEngSessionStatusObserver.h"

// FORWARD DECLARATIONS
class CImpsPluginSendDataOperation;
class RImpsEng;

/**
 * CImpsPluginPureDataHandler implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CImpsPluginPureDataHandler ) :
        public CBase,
        public MImpsPureDataHandler,
        public MImpsPureHandler2,
        public MImpsErrorHandler2,
        public MPEngSessionStatusObserver
    {
public: /* Construction */


    /**
     * Instantiates CImpsPluginPureDataHandler object.
     *
     * @return New CImpsPluginPureDataHandler instance.
     */
    static CImpsPluginPureDataHandler* NewL( RImpsEng& aSessClient,
    MImpsPluginAccessHandler& aAccessHandler );


    /**
     * Destructor.
     */
    ~CImpsPluginPureDataHandler();

    //-----------------------------------------------------------------------

public: // new functions from MImpsPureDataHandler

    /**
     *  Get Transfer buffer for out going message
     *  @see <MImpsPureDataHandler.h>
     *  @since 3.0
     */
    TPtr8 TransferBufferL();

    /**
     *  Sends data from the transfer buffer to the network
     *  @see <MImpsPureDataHandler.h>
     *  @since 3.0
     */
    TInt SendDataL( TRequestStatus& aStatus );

    /**
     *  Gets response for the message of given operation id
     *  @see <MImpsPureDataHandler.h>
     *  @since 3.0
     */
    HBufC8* ResponseL( TInt aTransId );

    /**
     *  Cancel sendin operation of given transaction id
     *  @see <MImpsPureDataHandler.h>
     *  @since 3.0
     */
    void CancelSending( TInt aTransId );

    /**
     *  Listen Incoming data
     *  @see <MImpsPureDataHandler.h>
     *  @since 3.0
     */
    TInt ListenIncomingData( TRequestStatus& aStatus );

    /**
     *  Cancel incoming data listening
     *  @see <MImpsPureDataHandler.h>
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
    TInt FindOperation( TInt aOpId, TInt ( CImpsPluginSendDataOperation:: *aIdFunc )() const ) const;
    TInt FindOperationL( TInt aOpId, TInt ( CImpsPluginSendDataOperation:: *aIdFunc )() const ) const;

protected: // construction

    /**
     * C++ constructor. protected
     */
    CImpsPluginPureDataHandler( MImpsPluginAccessHandler& aAccessHandler,
                                RImpsEng& aSessClient );

    /**
     * Symbian Second phase constructor
     */
    void ConstructL( RImpsEng& aSessClient );

private: //Data


    /// OWN: Access handler
    MImpsPluginAccessHandler&				iAccessHandler;

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
    RPointerArray<CImpsPluginSendDataOperation>   iOperations;

    /// OWN: Id which is used to do logs more human readable
    TInt                                    iConnectionLogsId;

    /// OWN: Logs Id iteration,
    TInt                                    iOpidIterator;

    /// OWN: Last served op Id
    TInt                                    iLastServerOpId;

    RFs             iFs;
    };

#endif  //__CIMPSPLUGINPUREDATAHANDLER_H__

// End of File
