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
* Description:  Presence attribute publisher.
*
*/

#ifndef CPENGATTRIBUTEPUBLISHHANDLER_H__
#define CPENGATTRIBUTEPUBLISHHANDLER_H__


//  INCLUDES
#include "MPEngOutgoingTransactionHandler.h"
#include "PEngWVCspVersion.h"
#include <E32Base.h>


//FORWARD DECLARATION
class CPEngTransactionStatus;
class MPEngPresenceAttrManager;
class MPEngPresenceAttrTransactionEnv;
class MPEngPresenceAttrModel2;
class MPEngXMLParser;



// CLASS DECLARATION

/**
 * Presence attribute publisher.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributePublishHandler ) : public CBase,
        public MPEngOutgoingTransactionHandler
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CPEngAttributePublishHandler* NewLC( MPEngPresenceAttrManager& aAttrManager,
    MPEngPresenceAttrTransactionEnv& aTransEnv,
    const TDesC16& aData,
    TInt aTransactionOperation,
    TPEngWVCspVersion aCspVersion );

    /**
    * Destructor.
    */
    virtual ~CPEngAttributePublishHandler();



private:

    /**
     * C++ default constructor.
     */
    CPEngAttributePublishHandler( MPEngPresenceAttrManager& aAttrManager,
                                  MPEngPresenceAttrTransactionEnv& aTransEnv,
                                  TInt aTransactionOperation,
                                  TPEngWVCspVersion aCspVersion );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const TDesC16& aData );





public: // Functions from MPEngOutgoingTransactionHandler

    /**
     * From MPEngOutgoingTransactionHandler
     *
     * @since 3.0
     */
    void RequestL( TDes8& aSendBuffer );

    void LastRunningTransactionHandler();

    void ProcessResponseL( const TDesC8& aResponse,
                           TRequestStatus& aStatus );


    void CancelProcessing();
    void NewTransactionHandlersL(
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers );


    TBool TransactionCompleted();
    CPEngTransactionStatus* TransactionResult();

    void ReleaseHandler();







private:    // Data

    //REF: Attribute Manager
    MPEngPresenceAttrManager&   iAttributeManager;

    //REF: Attribute Transaction Env
    MPEngPresenceAttrTransactionEnv&    iTransEnv;

    //OWN : Transaction operation
    const TInt iTransactionOperation;

    //OWN: WV CSP version
    const TPEngWVCspVersion  iCspVersion;

    //OWN : Transaction status
    CPEngTransactionStatus*     iTransactionStatus;

    //OWN: Published attribute models
    RPointerArray< MPEngPresenceAttrModel2 >  iAttributeModels;

    //OWN: XML Parser
    MPEngXMLParser*     iXmlParser;

    /// OWN: Online status was published flag
    TBool               iOnlineStPublished;

    };

#endif      //  CPENGATTRIBUTEPUBLISHHANDLER_H__

//  End of File

