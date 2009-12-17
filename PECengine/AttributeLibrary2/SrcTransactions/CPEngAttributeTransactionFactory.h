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
* Description:  Presence attribute transaction factory.
*
*/

#ifndef CPENGATTRIBUTETRANSACTIONFACTORY_H__
#define CPENGATTRIBUTETRANSACTIONFACTORY_H__


//  INCLUDES
#include "CPEngSessionSlotObject.h"
#include "MPEngTransactionFactory.h"
#include "PEngWVCspVersion.h"
#include <E32Base.h>



//FORWARD DECLARATION
class MPEngIncomingTransactionHandler;
class MPEngOutgoingTransactionHandler;
class MPEngPresenceAttrTransactionEnv;
class MPEngPresenceAttrManager;



// CLASS DECLARATION

/**
 * Presence attribute transaction factory.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeTransactionFactory ) : public CPEngSessionSlotObject,
        public MPEngTransactionFactory

    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeTransactionFactory* NewL( const CPEngSessionSlotId& aSlotId,
    MPEngPresenceAttrTransactionEnv& aTransEnv,
    MPEngPresenceAttrManager& aAttrManager );

    /**
     * Destructor.
     */
    virtual ~CPEngAttributeTransactionFactory();

private:

    /**
     * C++ default constructor.
     */
    CPEngAttributeTransactionFactory( MPEngPresenceAttrTransactionEnv& aTransEnv,
                                      MPEngPresenceAttrManager& aAttrManager );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const CPEngSessionSlotId& aSlotId );




public: // Functions from base classes

    /**
     * From MPEngTransactionFactory.
     *
     * @see MPEngTransactionFactory
     * @since 3.0
     */
    void Close();

    MPEngIncomingTransactionHandler* IncomingTransactionHandlerL(
        const TDesC8& aIncomingRequest );

    void OutgoingTransactionHandlerL(
        TInt aOperation,
        const TDesC16& aData,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers );



private:    // Data

    //OWN: Attribute transaction environment
    MPEngPresenceAttrTransactionEnv&    iTransEnv;

    //OWN: Attribute manager
    MPEngPresenceAttrManager&   iAttributeManager;

    //OWN: Flag describing are above ref. counted singletons owned or not
    TBool iRefsOwned;

    //OWN: Transaction domain
    HBufC16*    iDomain;

    //OWN: WV CSP version
    TPEngWVCspVersion   iCspVersion;
    };

#endif      //  CPENGATTRIBUTETRANSACTIONFACTORY_H__


//  End of File

