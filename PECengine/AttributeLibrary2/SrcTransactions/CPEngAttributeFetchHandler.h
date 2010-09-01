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
* Description:  Attribute fetch handler.
*
*/

#ifndef CPENGATTRIBUTEFETCHHANDLER_H__
#define CPENGATTRIBUTEFETCHHANDLER_H__


//  INCLUDES
#include <E32base.h>
#include "MPEngOutgoingTransactionHandler.h"
#include "PEngWVCspVersion.h"



//FORWARD DECLARATION
class CPEngTransactionStatus;
class MPEngPresenceAttrManager;
class MPEngPresenceAttrModel2;
class MPEngAttrFetchProcessor;




// CLASS DECLARATION

/**
 * Attribute fetcher.
 *
 * Handles presence attribute fetch XML request
 * generation and response decoding.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeFetchHandler ) : public CActive,
        public MPEngOutgoingTransactionHandler
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeFetchHandler* NewLC( MPEngPresenceAttrManager& aAttrManager,
    const TDesC& aDomain,
    const TDesC16& aData,
    TInt aTransactionOperation,
    TPEngWVCspVersion aCspVersion );


    /**
     * Destructor.
     */
    virtual ~CPEngAttributeFetchHandler();




private: //Constructors

    /**
     * C++ default constructor.
     */
    CPEngAttributeFetchHandler( MPEngPresenceAttrManager& aAttrManager,
                                TInt aTransactionOperation );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const TDesC& aDomain,
                     TPEngWVCspVersion aCspVersion,
                     const TDesC16& aData );



public: // Functions from MPEngOutgoingTransactionHandler

    /**
     * From MPEngOutgoingTransactionHandler
     *
     * @since 3.0
     */
    void RequestL( TDes8& aSendBuffer );

    void LastRunningTransactionHandler();

    void ProcessResponseL( const TDesC8& aResponse, TRequestStatus& aStatus );
    void CancelProcessing();

    void NewTransactionHandlersL( RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers );

    TBool TransactionCompleted();
    CPEngTransactionStatus* TransactionResult();

    void ReleaseHandler();



protected: //from CActive

    void RunL();
    TInt RunError( TInt aError );
    void DoCancel();



private: //Request helpers


    /**
     * Issues new active object run round.
     *
     * @since 3.0
     */
    void IssueNewRound();


private:    // Data


    //REF: Attribute Manager
    MPEngPresenceAttrManager&   iAttributeManager;

    //OWN: Operation id
    const TInt  iTransactionOperation;

    //OWN: Fetch processor
    MPEngAttrFetchProcessor*    iFetchedProcessor;

    //OWN: Decoded attribute models
    RPointerArray< MPEngPresenceAttrModel2 >    iResultModels;

    //OWN: Fetcher transaction status
    CPEngTransactionStatus*     iResultStatus;

    //REF: Status to complete
    TRequestStatus*     iRequest;

    };

#endif      //  CPENGATTRIBUTEFETCHHANDLER_H__

//  End of File

