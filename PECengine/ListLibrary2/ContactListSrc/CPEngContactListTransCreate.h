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
* Description:  Handler to create contact list
*
*/

#ifndef __CPENGCONTACTLISTTRANSCREATE_H__
#define __CPENGCONTACTLISTTRANSCREATE_H__

//  INCLUDES
#include "CPEngContactListTransBase.h"


// CLASS DECLARATION
class   MPEngXMLSerializer;


/**
 * Handler to create contact list
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListTransCreate ):
        public CPEngContactListTransBase
    {
public:

    /**
     * Two-phased constructor.
     */
    static CPEngContactListTransCreate* NewLC(
        CPEngContactListModBase& aContactList,
        CPEngSessionSlotId& aSessionSlotId,
        MPEngContactListTransactionManager& aManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );

    /**
     * Destructor.
     */
    virtual ~CPEngContactListTransCreate();


protected: //From CPEngContactListTransBase

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



protected: // Constructors

    /**
     *  C++ constructor.
     *  contact list model reference owned
     */
    CPEngContactListTransCreate( CPEngContactListModBase& aContactList,
                                 CPEngSessionSlotId& aSessionSlotId,
                                 MPEngContactListTransactionManager& aManager,
                                 TPEngWVCspVersion& aCSPVersion,
                                 TInt aOperationId );


    /**
     * Symbian constructor
     */
    void ConstructL();
    };

#endif      //  __CPENGCONTACTLISTTRANSCREATE_H__

//  End of File

