/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for Reactive Authorization
*
*/


#ifndef MCAREACTIVEAUTHPC_H
#define MCAREACTIVEAUTHPC_H


// INCLUDES
#include <e32std.h>
#include <badesca.h>



// FORWARD DECLARATIONS
class  MCAReactiveAuthObserverPC;




// CLASS DECLARATION
/**
 *  Reactive Authorization Interface for UI and Engine.
 *
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class MCAReactiveAuthPC
    {

    public:
        /**
         * Destructor.
         */
        virtual ~MCAReactiveAuthPC() {};

    public:

        /**
         * To add the UI side observer for MCAReactiveAuthObserverPC
         * events from the engine.
         * @param aObserver - observer to be added
         */
        virtual void AddReactiveAuthObserver( MCAReactiveAuthObserverPC* aObserver ) = 0;

        /**
         * To remove the UI side observer for MCAReactiveAuthObserverPC
         * events from the engine.
         */
        virtual void RemoveReactiveAuthObserver() = 0;

        /**
         * Find any contact which id is aContactId
         * @param aContactId Contactid to recognize contact.
         * @return ETrue if contact found else EFalse
         */
        virtual TBool FindAnyContactL( const TDesC& aContactId ) = 0;

        /**
         * Get alias for given user.
         * Use this only for one-shot rare operations.
         * @param aUserId The user id for which to fetch alias attribute
         * @return The alias, ownership is transferred.
         * Leaves with KErrNotFound if the alias cannot be found,
         * otherwise some other KErr-code (e.g. KErrNoMemory)
         */
        virtual HBufC* GetAliasL( const TDesC& aUserId  ) = 0;

        /**
        * Get the list of all users who have rejected the auth. request
        * request for processing
        * @param aUserId Will contain the user id
        * @param aUserIdsArray list of all userids who have rejected the auth. request
        */
        virtual void GetListOfRAStatusL( TDes& aUserId, CDesCArray& aUserIdsArray  ) = 0;

        /**
        * Get the information from the next reactive authorization request for
        * processing
        * @param aId Will contain the id for the request
        * @param aUserId Will contain the user id
        * @param aLastPos Position of previous found request in queue
        * @return ETrue if there is another request in the queue
        */
        virtual TBool GetNextPendingRARequestL( TInt& aId, TDes& aUserId,
                                                TInt& aLastPos ) = 0;

        /**
         * Send reactive authorization response
         * @param aId The id number given through observer
         * @param aResponse The response, ETrue if authorizing,
                            EFalse if denying
         */
        virtual void SendReactiveAuthResponseL( TInt aId, TBool aResponse ) = 0;

    };



#endif      // MCAREACTIVEAUTHPC_H

// End of File

