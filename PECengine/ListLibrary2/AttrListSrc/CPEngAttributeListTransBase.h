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
* Description:  Base class for attribute list transactions
*
*/

#ifndef __CPENGATTRIBUTELISTTRANSBASE_H__
#define __CPENGATTRIBUTELISTTRANSBASE_H__


//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>
#include "MPEngOutgoingTransactionHandler.h"
#include "PEngWVCspVersion.h"


// FORWARD DECLARATIONS
class MPEngAttributeListTransactionManager;
class CPEngContactListTransactionManager;
class CPEngTransactionStatus;
class MPEngXMLSerializer;
class CDesC16Array;

// CLASS DECLARATION

/**
 * Base class for Attribute list transactions
 * Derived classes handle creation and deletion.
 * of the attribute lists from Network server.
 * This class is intended for derivation.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeListTransBase ) :
        public CBase,
        public MPEngOutgoingTransactionHandler
    {
public:

    /**
     * Destructor.
     */
    virtual ~CPEngAttributeListTransBase();


public: // From MPEngOutgoingTransactionHandler

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
    void CancelProcessing();


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
     */
    CPEngTransactionStatus* TransactionResult();


    /**
     * Releases the handler
     * @see <MPEngOutgoingTransactionHandler.h>
     */
    void ReleaseHandler();


public:  // Template functions for derived classes

    /**
     * Consume XML message response from the WV server
     * @since 3.0
     * @param aResponse buffer with the response
     */
    virtual void DoConsumeMessageFromServerL( const TDesC8& aResponse ) = 0;



protected:  // new functions used by the derived classes

    /**
     * Append lists of contact lists to the buffer
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     * @param aContactLists contact list name to append
     */
    void XMLAppendContactListsL( MPEngXMLSerializer& aXmlSerializer,
                                 const MDesCArray& aContactLists );

    /**
     * Append lists of contact Ids to the buffer
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     * @param aContactIds array of wv IDs to be appended
     */
    void XMLAppendContactIdsL( MPEngXMLSerializer& aXmlSerializer,
                               const MDesCArray& aContactIds );

    /**
     * Append settings of the default list, according to passed flag
     *
     * @since 3.0
     * @param aXmlSerializer XML serializer
     * @param aFlag flag if attribute list is default
     */
    void XMLAppendDefaultFlagL( MPEngXMLSerializer& aXmlSerializer,
                                const TBool aFlag );

    /**
     * Copy content of the old descriptor array to the new descriptor array
     *
     * @since 3.0
     * @param aOldDesArray original descriptor array
     * @param aNewArray new descriptor array
     */
    void CopyDesArrayL( const MDesC16Array& aOldDesArray,
                        CDesC16Array& aNewArray );



protected: // constructor

    /**
     * C++ constructor.
     */
    CPEngAttributeListTransBase(
        MPEngAttributeListTransactionManager& aAttrListTransManager,
        CPEngContactListTransactionManager& aCntListTransManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );

    /**
     * Symbian constructor.
     */
    void ConstructL();

protected:    // Data

    // OWN: transaction status class
    CPEngTransactionStatus*                 iTransactionStatus;

    /// OWN: Flag if transaction is completed
    TBool                                   iTransactionCompleted;

    /// REF: Pointer to the attribute list manager
    MPEngAttributeListTransactionManager&   iAttributeListTransManager;

    /// REF: Contact list transaction manager
    CPEngContactListTransactionManager&     iCntListTransManager;

    /// REF: CSP Version
    TPEngWVCspVersion&                      iCSPVersion;

    /// OWN: Operation Id how client called request
    TInt                                    iOperationId;

    };

#endif  //   __CPENGATTRIBUTELISTTRANSBASE_H__

// End of File

