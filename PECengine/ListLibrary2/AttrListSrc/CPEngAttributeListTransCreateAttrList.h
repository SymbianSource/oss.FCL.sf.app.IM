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
* Description:  Create attribute list transaction handler
*
*/

#ifndef __CPENGATTRIBUTELISTTRANSCREATEATTRLIST_H__
#define __CPENGATTRIBUTELISTTRANSCREATEATTRLIST_H__

//  INCLUDES
#include <e32base.h>
#include "CPEngAttributeListTransBase.h"


// FORWARD DECLARATIONS
class CPEngAttributeListItem;
class MPEngPresenceAttrManager;


// CLASS DECLARATION

/**
 * Create attribute list transaction handler
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeListTransCreateAttrList ) :
        public CPEngAttributeListTransBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeListTransCreateAttrList* NewLC(
        const CPEngAttributeListItem& aAttributeListItem,
        MPEngAttributeListTransactionManager& aAttrListTransManager,
        CPEngContactListTransactionManager& aCntListTransManager,
        MPEngPresenceAttrManager& aPresenceAttributeManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId,
        TBool aSync = EFalse );

    /**
     * Destructor.
     */
    virtual ~CPEngAttributeListTransCreateAttrList();


public:  // From MPEngOutgoingTransactionHandler

    /**
     * Synchronous method, get Outgoing transaction request
     * @see <MPEngOutgoingTransactionHandler.h>
     * @since 3.0
     */
    void RequestL( TDes8& aSendBuffer );


public:  // From CPEngAttributeListTransBase

    /**
     * Consume XML message response from the WV server
     * @see <CPEngAttributeListTransBase.h>
     * @since 3.0
     */
    void DoConsumeMessageFromServerL( const TDesC8& aResponse );


private: // constructor

    /**
     * C++ constructor.
     */
    CPEngAttributeListTransCreateAttrList(
        MPEngAttributeListTransactionManager& aAttrListTransManager,
        CPEngContactListTransactionManager& aCntListTransManager,
        MPEngPresenceAttrManager& aPresenceAttributeManager,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId,
        TBool aSync );

    /**
     * Symbian constructor .
     */
    void ConstructL( const CPEngAttributeListItem& aAttributeListItem );


private:    // Data

    /// OWN: Attribute list item
    CPEngAttributeListItem*                 iAttributeListItem;

    /// REF: Presence attribute manager
    MPEngPresenceAttrManager&               iPresenceAttributeManager;

    /// OWN: is this synchronization transaction
    TBool                                   iSynchronization;

    };

#endif  //   __CPENGATTRIBUTELISTTRANSCREATEATTRLIST_H__

// End of File

