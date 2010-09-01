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
* Description:  Presence attribute transaction environment.
*
*/

#ifndef CPENGPRESENCEATTRTRANSACTIONENV_H
#define CPENGPRESENCEATTRTRANSACTIONENV_H

//  INCLUDES
#include <E32Base.h>

#include "CPEngSessionSlotObject.h"
#include "MPEngPresenceAttrTransactionEnv.h"


// FORWARD DECLARATIONS
class CPEngOrphanNotificationQue;
class CPEngSubscribedUsers;
class CPEngSubscribedUser;
class CPEngSessionSlotId;
class MPEngPresenceAttrManager;


// CLASS DECLARATION

/**
 * Presence attribute transaction environment.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngPresenceAttrTransactionEnv ) : public CPEngSessionSlotObject,
        public MPEngPresenceAttrTransactionEnv
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngPresenceAttrTransactionEnv* NewL( const CPEngSessionSlotId& aSlotId,
    MPEngPresenceAttrManager& aAttrMan );

    /**
     * Destructor.
     */
    ~CPEngPresenceAttrTransactionEnv();

private:

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL( const CPEngSessionSlotId& aSlotId );

    /**
     * C++ default constructor.
     */
    CPEngPresenceAttrTransactionEnv( MPEngPresenceAttrManager& aAttrMan );




public: // Functions from MPEngPresenceAttrTransactionEnv


    /**
     * From MPEngPresenceAttrTransactionEnv.
     * @since 3.0
     */
    void Close();
    void AddSubscriptionL( const TDesC& aPresenceID );
    void RemoveSubscriptionL( const TDesC& aPresenceID );
    const MDesCArray& SubscribedPresenceIDs() const;


    void SetSessionTracingAttributesL(
        RPointerArray< MPEngPresenceAttrModel2 >& aAttributeModels  );

    void UpdateSessionTracingAttributesL(
        RPointerArray< MPEngPresenceAttrModel2 >& aAttributeModels );

    void HandleNetworkSessionStateChangeL( TBool aSessionOpen );


    void QueOrphanNotificationL( const TDesC8& aPresenceBlock );
    void ProcessOrphanNotificationsL(
        RPointerArray< MPEngPresenceAttrModel2 >& aModels );




private: // Helpers


    /**
     * Initializes user's attributes to presence cache.
     *
     * @since 3.0
     * @param aPresenceID The user which attributes to initialize.
     */
    void DoInitializeAttributesToCacheL( const TDesC& aPresenceID );


    /**
     * Clears user's attributes from cache.
     *
     * @since 3.0
     * @param aPresenceID The user which attributes to clear.
     */
    void DoClearAttributesFromCacheL( const TDesC& aPresenceID );


    /**
     * Processes buffered orphan notifications and
     * stores generated attribute models to presence cache.
     *
     * @since 3.0
     */
    void DoProcessAndStoreOrphanNotificationsL();



private:    // Data

    //OWN: Transaction domain
    HBufC16*    iDomain;

    //OWN: Subscribed users
    CPEngSubscribedUsers*   iSubscribedUsers;


    //OWN: Que for orphan notifications
    CPEngOrphanNotificationQue* iOrphanNotifications;


    //OWN: Attribute Manager
    MPEngPresenceAttrManager& iAttributeManager;


    //OWN: Flag is the above Attribute Manager owned or not
    TBool iAttributeManagerOwned;


    //OWN: Session tracing attributes
    RPointerArray< MPEngPresenceAttrModel2 > iSessTracingAttribs;

    };

#endif      // CPENGPRESENCEATTRTRANSACTIONENV_H

//  End of File

