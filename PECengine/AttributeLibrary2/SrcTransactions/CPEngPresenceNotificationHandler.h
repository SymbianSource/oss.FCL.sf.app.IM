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
* Description:  Presence notification handler.
*
*/

#ifndef CPENGPRESENCENOTIFICATIONHANDLER_H__
#define CPENGPRESENCENOTIFICATIONHANDLER_H__


//  INCLUDES
#include "MPEngIncomingTransactionHandler.h"
#include "PEngWVCspVersion.h"
#include <E32Base.h>
#include <BaDescA.h>


//FORWARD DECLARATION
class MPEngPresenceAttrManager;
class MPEngPresenceAttrTransactionEnv;
class MPEngXMLParser;
class MPEngPresenceAttrModel2;



// CLASS DECLARATION

/**
 * Presence notification handler.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngPresenceNotificationHandler ) : public CBase,
        public MPEngIncomingTransactionHandler
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngPresenceNotificationHandler* NewL( const TDesC& aDomain,
    TPEngWVCspVersion aCspVersion,
    MPEngPresenceAttrManager& aAttrManager,
    MPEngPresenceAttrTransactionEnv& aTransEnv );

    /**
     * Destructor.
     */
    virtual ~CPEngPresenceNotificationHandler();



private:

    /**
     * C++ default constructor.
     */
    CPEngPresenceNotificationHandler( MPEngPresenceAttrManager& aAttrManager,
                                      MPEngPresenceAttrTransactionEnv& aTransEnv,
                                      TPEngWVCspVersion aCspVersion );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const TDesC& aDomain );



public: // Methods from MPEngIncomingTransactionHandler

    /**
     * From MPEngIncomingTransactionHandler
     *
     * @since 3.0
     */
    void ProcessRequestL( const TDesC8& aRequest,
                          TRequestStatus& aStatus );

    void CancelProcessing();


public: // New methods


    /**
     * Checks is the incoming request
     * a presence update notification.
     *
     * @since 3.0
     * @param aIncomingRequest The incoming request to check.
     * @return ETrue if the request is presence update
     * request. Else EFalse.
     */
    static TBool CanHandleL( const TDesC8& aIncomingRequest );



    /**
     * Processes one presence notification block.
     *
     * @since 3.0
     * @param aPresenceBlock The presence block to process.
     * @param aDomain The domain for presence notification.
     * @param aSubscribedPresences Subscribed presences IDs.
     *        Used for matching presence notifications to
     *        subscribed presence IDs.
     * @param aAttributeManager The attribute manager to instantiate attribute models.
     * @param aParser XML parser for processing the notification.
     * @param aModels Model array to append generated attribute models.
     * @return EFalse if couldn't match the notification presence ID to any subscriped ID.
     *         Else ETrue.
     */
    static TBool ProcessNotifyBlockL( const TDesC8& aPresenceBlock,
                                      const TDesC& aDomain,
                                      const MDesCArray& aSubscribedPresences,
                                      MPEngPresenceAttrManager& aAttributeManager,
                                      MPEngXMLParser& aParser,
                                      RPointerArray< MPEngPresenceAttrModel2 >& aModels );


private:    // Data

    //REF: The attribute manger
    MPEngPresenceAttrManager&   iAttributeManager;

    //REF: The attribute transaction environment
    MPEngPresenceAttrTransactionEnv&  iTransEnv;

    //OWN: WV CSP version
    TPEngWVCspVersion   iCspVersion;

    //OWN: The XML parser
    MPEngXMLParser* iXmlParser;

    //OWN: Presence data blocks
    CDesC8ArraySeg  iPresenceDataBlocks;

    //OWN: Parsed attribute models
    RPointerArray< MPEngPresenceAttrModel2 > iModels;

    //OWN: Transaction domain.
    HBufC16* iDomain;

    };

#endif      //  CPENGPRESENCENOTIFICATIONHANDLER_H__

//  End of File




