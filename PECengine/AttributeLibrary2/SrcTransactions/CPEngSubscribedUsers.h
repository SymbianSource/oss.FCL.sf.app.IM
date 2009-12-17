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
* Description:  List of subscribed users.
*
*/

#ifndef CPENGSUBSCRIBEDUSERS_H
#define CPENGSUBSCRIBEDUSERS_H

//  INCLUDES
#include <E32Base.h>
#include <BaMDesCA.H>



// FORWARD DECLARATIONS
class CPEngSubscribedUser;


// CLASS DECLARATION
/**
 * List of subscribed users.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSubscribedUsers ) : public CBase,
        public MDesCArray
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngSubscribedUsers* NewL();


    /**
     * Destructor.
     */
    ~CPEngSubscribedUsers();


private:

    /**
     * C++ default constructor.
     */
    CPEngSubscribedUsers();

    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: //  From MDesCArray


    /**
     * Gets the count of users in subscribed users array.
     * @since 3.0
     */
    TInt MdcaCount() const;


    /**
     * Indexes the subscribed users array.
     *
     * @since 3.0
     * @param aIndex The position of the user id to get from the
     * subscribed users array.
     * @return User ID.
     */
    TPtrC16 MdcaPoint( TInt aIndex ) const;



public: //  New functions


    /**
     * Adds a new subscribed user.
     *
     * @since 3.0
     * @param aUser The new user to add.
     * Takes the ownership in success.
     */
    void AddSubscribedUserL( CPEngSubscribedUser* aUser );


    /**
     * Locates a user by user id.
     *
     * @since 3.0
     * @param aUserID
     * @return User entrys index or KErrNotFound.
     */
    TInt FindSubscribedUser( const TDesC& aUserID ) const;


    /**
     * Gets the count of subscribed users.
     *
     * @since 3.0
     * @return The subscribed users count.
     */
    TInt SubscribedUserCount() const;


    /**
     * Gets the subscribed user by index.
     *
     * @since 3.0
     * @param aIndex The index to get.
     * @return  The user entry. No ownership returned.
     */
    CPEngSubscribedUser& SubscribedUser( TInt aIndex );


    /**
     * Deletes a subscribed user by index.
     *
     * @since 3.0
     * @param aIndex The index to delete.
     */
    void DeleteSubscribeUser( TInt aIndex );


private:    // Data


    //OWN: The subscribed users.
    RPointerArray< CPEngSubscribedUser > iUsers;

    };

#endif      // CPENGSUBSCRIBEDUSERS_H

// End of File
