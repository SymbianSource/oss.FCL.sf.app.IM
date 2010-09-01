/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base class of the contact list management handlers
*
*/

#ifndef __CPENGCONTACTLISTMNGTRANSBASE_H__
#define __CPENGCONTACTLISTMNGTRANSBASE_H__


//  INCLUDES
#include "MPEngOutgoingTransactionHandler.h"
#include "CPEngContactListModBase.h"
#include "PEngWVCspVersion.h"
#include <e32base.h>
#include <badesca.h>



// FORWARD DECLARATIONS
class   CPEngTransactionStatus;
class   CPEngContactListModBase;
class   MPEngXMLSerializer;
class   CPEngSessionSlotId;


// CLASS DECLARATION
/**
 * Base class of the contact list management handlers.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListMngTransBase ):
        public CBase,
        public MPEngOutgoingTransactionHandler
    {
protected:  // enumerations

    // Transaction handler states
    enum TPEngCntListMngTransState
        {
        EPEngListContentUpdate = 1,
        EPEngSubscriptionUpdate,
        EPEngRollBack,
        EPEngActivateAttributes,
        EPEngDeActivateAttributes,
        EPEngTransactionCompleted,
        };

public:

    /**
     * Destructor.
     */
    virtual ~CPEngContactListMngTransBase();



protected: // Constructors

    /**
     * C++ constructor.
     */
    CPEngContactListMngTransBase( CPEngContactListModBase& aContactList,
                                  CPEngSessionSlotId& aSessionSlot,
                                  TPEngWVCspVersion& aCSPVersion,
                                  TInt aOperationId );

    /**
     * Symbian OS constructor
     */
    void ConstructL();



public: // From MPEngOutgoingTransactionHandler

    /**
     * Synchronous method, get Outgoing transaction request
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void RequestL( TDes8& aSendBuffer );


    /**
     * Signals to the transaction handler thst it is last
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void LastRunningTransactionHandler();


    /**
     * Process the response to the request.
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void ProcessResponseL( const TDesC8& aResponse,
                           TRequestStatus& aStatus );

    /**
     * Cancels asynchronous processing of the request
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void CancelProcessing( );


    /**
     * Support for simultaneous transaction handling
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void NewTransactionHandlersL(
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers );

    /**
     * Function to signal completing of the transaction
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    TBool TransactionCompleted( );


    /**
     * Gets transaction status result class
     * @see <MPEngOutgoingTransactionHandler.h>
     * @since 3.0
     */
    CPEngTransactionStatus* TransactionResult( );


    /**
     * Releases the handler
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void ReleaseHandler( );



public: // Template functions for derived classes

    /**
     * Template function to append nick list XML message.
     *
     * @since 3.0
     * @param aXmlSerializer XML message serializer
     */
    virtual void DoAppendListNickUpdateL(
        MPEngXMLSerializer& aXmlSerializer ) = 0;

    /**
     * Template function to append subscription update XML message.
     *
     * @since 3.0
     * @param aXmlSerializer XML message serializer
     */
    virtual void DoGetXMLSubscriptionUpdateL(
        MPEngXMLSerializer& aXmlSerializer ) = 0;

    /**
     * Template function to parse subscription update response.
     *
     * @since 3.0
     * @param aResponse request response
     */
    virtual void DoParseSubscriptionUpdateResponseL(
        const TDesC8& aResponse ) = 0;

    /**
     * Template function to update transaction state
     *
     * Function should update state of the transaction so that
     * it continues for another step of the request
     *
     * @since 3.0
     */
    virtual void DoUpdateTransactionState() = 0;


    /**
     * Template function to fill nick list of the update type.
     *
     * Derived function should fills in the given array with
     * contacts which should be updated by the handler.
     *
     * @since 3.0
     * @param nick list to be filled
     */
    virtual void DoFillNickListLC(
        RPointerArray<CPEngContactListModItemContainer>& aNickList ) = 0;


    /**
     * Template function to complete contact list update.
     *
     * Derived class should do what ever cleaning required
     * to complet request.
     *
     * @since 3.0
     */
    virtual void DoCompleteContactListUpdateL() = 0;



private: ///New functions

    /**
     * Create XML message to update contact list content
     * @since 3.0
     * @param aXmlSerializer XML message serializer
     */
    void GetXMLContactListUpdateL( MPEngXMLSerializer& aXmlSerializer );


    /**
     * Create XML message to roll back not existing WV IDs
     * @since 3.0
     * @param aSendBuffer XML message serializer
     */
    void GetXMLRollBackUpdateL( MPEngXMLSerializer& aXmlSerializer );

    /**
     * Parse XML list update response
     * @since 3.0
     * @param aResponse Response buffer
     */
    void ParseListUpdateResponseL( const TDesC8& aResponse );



protected:  // Data

    /// OWN:Contact list model
    CPEngContactListModBase&            iContactList;

    /// REF: Session slot
    CPEngSessionSlotId&                 iSessionSlotId;

    // OWN: Transaction status
    CPEngTransactionStatus*             iTransactionStatus;

    // OWN: State of the presence contact list update
    TPEngCntListMngTransState           iUpdateState;

    // OWN: Arry for rollback
    CDesCArrayFlat*                     iRollBackIDsArray;

    /// OWN: CSP version
    TPEngWVCspVersion&                  iCSPVersion;

    /// OWN: Operation Id
    TInt                                iOperationId;

    /// OWN: Count of new added contact Ids on the server
    TInt 							    iNewIdsCount;
    };

#endif      //  __CPENGCONTACTLISTMNGTRANSBASE_H__

//  End of File


