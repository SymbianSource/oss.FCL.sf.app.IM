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
* Description:  Delete attribute lists Transaction Handler
*
*/



#ifndef __CPENGATTRIBUTELISTTRANSDELETEATTRLIST_H__
#define __CPENGATTRIBUTELISTTRANSDELETEATTRLIST_H__

//  INCLUDES
#include <e32base.h>
#include "CPEngAttributeListTransBase.h"


// FORWARD DECLARATIONS
class MPEngAttributeListTransactionManager;
class MDesCArray;


// CLASS DECLARATION

/**
 * Transaction handler to delete attribute list
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeListTransDeleteAttrList ) :
        public CPEngAttributeListTransBase
    {
public:

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeListTransDeleteAttrList* NewLC(
        MPEngAttributeListTransactionManager& aAttrListTransManager,
        CPEngContactListTransactionManager& aCntListTransManager,
        MDesCArray& aDeleteContactList,
        MDesCArray& aDeleteContactIds,
        TBool aDeleteDefault,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );
    /**
     * Destructor.
     */
    virtual ~CPEngAttributeListTransDeleteAttrList();


public: // Functions from MPEngOutgoingTransactionHandler

    /**
     *  Synchronous method, get Outgoing transaction request
     *  @see <MPEngOutgoingTransactionHandler.h>
     *  @since 3.0
     */
    void RequestL( TDes8& aSendBuffer );


public:  // Functions of the CPEngAttributeListTransBase class

    /**
     *  Consume XML message response from the WV server
     *  @see <CPEngAttributeListTransBase.h>
     *  @since 3.0
     */
    void DoConsumeMessageFromServerL( const TDesC8& aResponse );


private: // constructor

    /**
     * C++ default constructor.
     */
    CPEngAttributeListTransDeleteAttrList(
        MPEngAttributeListTransactionManager& aAttrListTransManager,
        CPEngContactListTransactionManager& aCntListTransManager,
        MDesCArray& aDeleteContactList,
        MDesCArray& aDeleteContactIds,
        TBool aDeleteDefault,
        TPEngWVCspVersion& aCSPVersion,
        TInt aOperationId );

    /**
     * By default Symbian constructor is private.
     */
    void ConstructL();

private:    // Data

    /// REF: Array of contact list, to delete attr list from
    MDesCArray&                 iArrayDeleteContactList;

    /// REF: Aarray of contact ids, to delete attr list from
    MDesCArray&                 iArrayDeleteContactIds;

    /// OWN: flag if default attributelist should be deleted
    TBool                       iDeleteDefaultAttrList;
    };


#endif  //   __CPENGATTRIBUTELISTTRANSDELETEATTRLIST_H__

// End of File

