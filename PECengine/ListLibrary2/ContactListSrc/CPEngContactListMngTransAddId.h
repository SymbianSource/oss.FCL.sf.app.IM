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
* Description:  Contact list add contact handler.
*
*/

#ifndef __CPENGCONTACTLISTMNGTRANSADDID_H__
#define __CPENGCONTACTLISTMNGTRANSADDID_H__

//  INCLUDES
#include "CPEngContactListMngTransBase.h"


/**
 * Contact list add contact handler.
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListMngTransAddId ) :
        public CPEngContactListMngTransBase
    {
public:

    /**
     * Two-phased constructor.
     */
    static CPEngContactListMngTransAddId* NewLC(
        CPEngContactListModBase& aContactList,
        CPEngSessionSlotId& aSessionSlotId,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );

    /**
     * Destructor.
     */
    virtual ~CPEngContactListMngTransAddId();


public: // From CPEngContactListMngTransBase

    /**
     * Appends add nick list XML message
     * @see <CPEngContactListMngTransBase.h>
     */
    void DoAppendListNickUpdateL( MPEngXMLSerializer& aXmlSerializer );


    /**
     * Append subscription update XML message
     * @see <CPEngContactListMngTransBase.h>
     */
    void DoGetXMLSubscriptionUpdateL( MPEngXMLSerializer& aXmlSerializer );


    /**
     * Parse subscription update response
     * @see <CPEngContactListMngTransBase.h>
     */
    void DoParseSubscriptionUpdateResponseL( const TDesC8& aResponse );


    /**
     * Update Transaction State
     * @see <CPEngContactListMngTransBase.h>
     */
    void DoUpdateTransactionState();


    /**
     * Fill nick list of the update
     * @see <CPEngContactListMngTransBase.h>
     */
    void DoFillNickListLC(
        RPointerArray<CPEngContactListModItemContainer>& aNickList );

    /**
     * Complete contact list update
     * @see <CPEngContactListMngTransBase.h>
     */
    void DoCompleteContactListUpdateL();


private: // Constructors

    /**
     * C++ constructor.
     */
    CPEngContactListMngTransAddId( CPEngContactListModBase& aContactList,
                                   CPEngSessionSlotId& aSessionSlot,
                                   TPEngWVCspVersion& aCSPVersion,
                                   TInt aOperationId );

    /**
     * Symbian constructor.
     */
    void ConstructL();

    };

#endif      //  __CPENGCONTACTLISTMNGTRANSADDID_H__


//  End of File


