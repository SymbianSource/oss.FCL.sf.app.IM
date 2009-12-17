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
* Description:  Subscribed user.
*
*/

#ifndef CPENGSUBSCRIBEDUSER_H
#define CPENGSUBSCRIBEDUSER_H

//  INCLUDES
#include <E32Base.h>


// CLASS DECLARATION
/**
 * Subscribed user.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSubscribedUser ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @param aUserID The subscriped users user id.
     */
    static CPEngSubscribedUser* NewLC( const TDesC& aUserID );


    /**
     * Destructor.
     */
    ~CPEngSubscribedUser();

private:

    /**
     * C++ default constructor.
     */
    CPEngSubscribedUser();

    /**
     * Symbian OS constructor.
     */
    void ConstructL( const TDesC& aUserID );



public: // New functions


    /**
     * Gets the subscribed users user id.
     *
     * @since 3.0
     * @return The user id.
     */
    const TDesC& UserID() const;


    /**
     * Increases the subscription count by one.
     * (Adds new subscription.)
     *
     * @since 3.0
     */
    void AddSubscription();


    /**
     * Decreases the subscription count by one.
     * (Removes one subscription.)
     *
     * @since 3.0
     */
    void RemoveSubscription();


    /**
     * Gets the count of subscriptions for this
     * subscribed user.
     *
     * @since 3.0
     * @return The count of subscriptions.
     */
    TInt SubscriptionsCount() const;


private:    // Data


    //OWN: User id
    HBufC*              iUserID;

    //OWN: Subscription count
    TInt                iSubsCount;

    };

#endif      // CPENGSUBSCRIBEDUSER_H

// End of File
