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
* Description:  Handler to synchronize contact list
*
*/

#ifndef __CPENGCONTACTLISTTRANSSYNC_H__
#define __CPENGCONTACTLISTTRANSSYNC_H__

//  INCLUDES
#include "CPEngContactListTransBase.h"


// CLASS DECLARATION
class MPEngXMLSerializer;
class CPEngContactListModItemContainer;


/**
 * Handler to synchronize contact list
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListTransSync ):
        public CPEngContactListTransBase
    {
public:

    /**
     * Two-phased constructor.
     */
    static CPEngContactListTransSync* NewLC(
        CPEngContactListModBase& aContactList,
        CPEngSessionSlotId& aSessionSlotId,
        MPEngContactListTransactionManager& aManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );

    /**
     * Destructor.
     */
    virtual ~CPEngContactListTransSync();


public: // From MPEngOutgoingTransactionHandler

    /**
     *  Support for simultaneous transaction handling
     *  @see <MPEngOutgoingTransactionHandler.h>
     */
    void NewTransactionHandlersL(
        RPointerArray<MPEngOutgoingTransactionHandler>& aTransactionsArray );


protected: // From CPEngContactListTransBase

    /**
     * Create XML message to create contact list
     * @see <CPEngContactListTransBase.h>
     */
    void DoGetXMLCreateCntListL( MPEngXMLSerializer& aXmlSerializer );


    /**
     * Parse contact list create XML response
     * @see <CPEngContactListTransBase.h>
     */
    void DoParseCreateCntListResponseL( const TDesC8& aResponse,
                                        MPEngXMLParser& aXMLparser );

    /**
     * Create XML message to fetch contact list
     * @see <CPEngContactListTransBase.h>
     */
    void DoGetXMLFetchCntListL( MPEngXMLSerializer& aXmlSerializer );


    /**
     * Parse contact list fetch XML response
     * @see <CPEngContactListTransBase.h>
     */
    void DoParseFetchCntListResponseL( const TDesC8& aResponse,
                                       MPEngXMLParser& aXMLparser );


private:

    /**
     * Parses contact list properties
     *
     * @since 3.0
     * @param aResponse buffer with request response
     * @param aXMLparser1 XML parser
     * @param aXMLparser1 XML parser
     */
    void ParsePropertiesL( const TDesC8& aResponse,
                           MPEngXMLParser& aXMLparser1,
                           MPEngXMLParser& aXMLparser2 );

    /**
     * Create Master contact list update handlers
     *
     * @since 3.0
     */
    void CreateMasterCntListUpdateHandlersL();



private: // Constructors

    /**
     * C++ constructor.
     */
    CPEngContactListTransSync( CPEngContactListModBase& aContactList,
                               CPEngSessionSlotId& aSessionSlotId,
                               MPEngContactListTransactionManager& aManager,
                               TPEngWVCspVersion& aCSPVersion,
                               TInt aOperationId );

    /**
     * Symbian constructor.
     */
    void ConstructL();


private:    // Data

    /// OWN: Additional handlers
    RPointerArray<MPEngOutgoingTransactionHandler>  iAdditionalHandlers;
    };

#endif      //  __CPENGCONTACTLISTTRANSSYNC_H__

//  End of File

