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

#ifndef __MPENGPRESENCEATTRTRANSACTIONENV_H
#define __MPENGPRESENCEATTRTRANSACTIONENV_H


//INCLUDES
#include <E32Std.h>
#include "BamDescA.h"


//FORWARD DECLARATIONS
class MPEngPresenceAttrModel2;



// CLASS DECLARATION


/**
 * Interface for Presence Attribute Transaction Environment.
 *
 * Attribute Transaction Environment manages the list
 * of subscribed presence attributes
 *
 * NOTE!!! Attribute Transaction Environment is reference counted singleton,
 * so client which has retrieved a instance to it, must also free it
 * by calling MPEngPresenceAttrTransactionEnv::Close()
 *
 * @lib 3.0
 */
class MPEngPresenceAttrTransactionEnv
    {

    public: //Reference cleanup

        /**
         * Attribute Transaction Environment is reference counted
         * singleton. and clients must free all obtained instances
         * by calling this to each of them when the Attribute
         * Transaction Environment isn't anymore needed.
         *
         * @since 3.0
         */
        virtual void Close() = 0;



    public: //Subscriptions


        /**
         * Adds given presence id into list of subscribed users.
         *
         * Subscriptions are "reference counted", so one subscribed
         * user can have several subscriptions active.
         * If given presence ID is empty, leaves with KErrArgument.
         *
         * @since 3.0
         * @param aPresenceID The subscribed users presence ID.
         */
        virtual void AddSubscriptionL( const TDesC& aPresenceID ) = 0;


        /**
         * Removes given user from attribute librarys list of
         * subscribed users.

         * Subscriptions are "reference counted", so one subscribed
         * user can have several subscriptions active.
         * ==> Subscribed user must be removed as many times
         * as it has been added.
         *
         * If given presence id is empty, does nothing.
         *
         * @since 3.0
         * @param aPresenceID The subscribed users presence ID.
         */
        virtual void RemoveSubscriptionL( const TDesC& aPresenceID ) = 0;


        /**
         * Gets the list of currently subscribed users.
         *
         * @since 3.0
         * @return The list if currently subscribed users.
         */
        virtual const MDesCArray& SubscribedPresenceIDs() const = 0;


    public: //Session attributes


        /**
         * Sets session tracing attributes.
         * Clears previous attributes and sets the new set in use.
         *
         * @since 3.0
         * @param aAttributeModels Session tracing attributes.
         * In success, takes the ownership of attributes which
         * are removed from parameter array.
         */
        virtual void SetSessionTracingAttributesL(
            RPointerArray< MPEngPresenceAttrModel2 >& aAttributeModels ) = 0;


        /**
         * Updates session tracing attributes. If attribute is found
         * from internal session tracing attributes list and also from
         * parameter array, takes the attribute from parameter array
         * away, and replaces the internal attribute object with
         * parameter one.
         *
         * @since 3.0
         * @param aAttributeModels New session tracing attributes.
         * In success, takes the ownership of attributes which
         * are removed from parameter array.
         */
        virtual void UpdateSessionTracingAttributesL(
            RPointerArray< MPEngPresenceAttrModel2 >& aAttributeModels ) = 0;


        /**
         * Informs held attributes about session state change
         * and then stores those back to storage.
         *
         * @since 3.0
         * @param aSessionOpen If ETrue, network presence session
         * was opened. If EFalse, network presence session was closed.
         */
        virtual void HandleNetworkSessionStateChangeL( TBool aSessionOpen ) = 0;




    public: //Orphan notifications

        /**
         * Adds a presence notification to orphan notification que.
         *
         * @since 3.0
         * @param aPresenceBlock The orphan notification to que.
         */
        virtual void QueOrphanNotificationL( const TDesC8& aPresenceBlock ) = 0;


        /**
         * Processes the qued orphan notifications.
         * Adds the generated attribute models to given array.
         *
         * @since 3.0
         * @param aModels The array to add produced attribute models.
         */
        virtual void ProcessOrphanNotificationsL(
            RPointerArray< MPEngPresenceAttrModel2 >& aModels ) = 0;


    protected:

        /**
         * Protected  destructor.
         *
         * Concrete Attribute Transaction Environment can't
         * be deleted through this interface.
         */
        virtual ~MPEngPresenceAttrTransactionEnv() {};

    };

#endif      //  __MPENGPRESENCEATTRTRANSACTIONENV_H

//  End of File
