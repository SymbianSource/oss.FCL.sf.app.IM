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
* Description:  Store API to retrieve reactive authorization models.
*
*/

#ifndef __CPENGREACTAUTHSTORE_H__
#define __CPENGREACTAUTHSTORE_H__

// INCLUDES
#include <E32Base.h>


//FORWARD DECLARATIONS
class CPEngReactAuthStoreImp;
class CPEngNWSessionSlotID2;
class MPEngAuthorizationRequest;
class MPEngAuthorizationStatus;
class MPEngAuthorizationRespond;



/**
 * Store API to retrieve and manage reactive authorizations.
 *
 * With this API clients can access and manage presence
 * reactive authorizations.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class CPEngReactAuthStore : public CBase
    {
        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates CPEngReactAuthStore object.
         *
         * Instantiates CPEngReactAuthStore object and connects it to
         * identified Presence Engine side NWSessionSlot. NWSessionSlot
         * must be a valid, existing slot.
         *
         * Errors:
         *  - Requested NWSessionSlot not found: KErrNotFound
         *  - Given NWSessionSlotID malformed: KErrArgument
         *
         * @param aNWSessionSlotID The session slot ID to identify the
         * session slot.
         * @param aPriority The priority for Presence Engine client side active
         * objects. These active objects are used when asynchronously delivering
         * events to observers or maintaining Presence Cache state.
         *
         * @return New CPEngReactAuthStore instance.
         */
        IMPORT_C static CPEngReactAuthStore* NewL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );

        IMPORT_C static CPEngReactAuthStore* NewLC(
            const CPEngNWSessionSlotID2& aNWSessionSlotID,
            TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngReactAuthStore();



    private:

        CPEngReactAuthStore();


        //-----------------------------------------------------------------------
    public: /* Reactive authorization requests */


        /**
         * Count of all authorization requests.
         *
         * @since 3.0
         * @return Count of all authorization requests.
         */
        IMPORT_C TInt AuthorizationReqCount() const;


        /**
         * Gets authorization request.
         *
         * @since 3.0
         * @param aIndex Index of the authorization request.
         * @return The authorization request.
         */
        IMPORT_C MPEngAuthorizationRequest& AuthorizationReq( TInt aIndex );


        /**
         * Count of pending authorizations requests.
         * This also includes authorizations which were
         * already responded, but server reissued them again.
         *
         * @since 3.0
         * @return Count of pending authorization requests.
         */
        IMPORT_C TInt PendingAuthorizationReqCount()  const;


        /**
         * Gets pending authorization request.
         *
         * @since 3.0
         * @param aIndex Index of the pending authorization request.
         * @return The pending authorization request.
         */
        IMPORT_C MPEngAuthorizationRequest& PendingAuthorizationReq( TInt aIndex );



        //-----------------------------------------------------------------------
    public: /* Reactive authorization responds */


        /**
         * Count of responded authorizations.
         *
         * @since 3.0
         * @return Count of responded authorizations.
         */
        IMPORT_C TInt RespondedAuthorizationCount() const;


        /**
         * Responded authorizations.
         *
         * @since 3.0
         * @param aIndex Index of the responded authorization.
         * @return The responded authorization.
         */
        IMPORT_C const MPEngAuthorizationStatus& RespondedAuthorization( TInt aIndex ) const;


        /**
         * Count of server denied authorizations.
         *
         * @since 3.0
         * @return Count of server denied authorizations.
         */
        IMPORT_C TInt ServerDeniedAuthorizationCount() const;


        /**
         * Server denied authorization.
         *
         * @since 3.0
         * @param aIndex Index of server denied authorization.
         * @return The served denied authorization.
         */
        IMPORT_C const MPEngAuthorizationStatus& ServerDeniedAuthorization( TInt aIndex ) const;



        //-----------------------------------------------------------------------
    public: /* Reactive authorization responds */

        /**
         * Gets a authorization respond for editing.
         * Ownership of authorization respond is transferred
         * to caller.
         *
         * Authorization respond object buffers individual
         * authorization responses for publishing them at once.
         * Respond object holds also edit lock,
         * so two client cannot edit authorization responses
         * at the same time.
         *
         * Lock and buffered responses are released
         * either by the updating of the reactive authorization
         * responses to network or by deleting the
         * respond object.
         *
         * @param aRespond The authorization respond object.
         *        If operation succeeds, returns respond object
         *        ownership to caller.
         * @return Status code from authorization respond creation:
         *        - KErrNone if creation succeeds.
         *        - KErrLocked if authorization respond is already locked.
         *        - Else one of system wide error codes.
         */
        IMPORT_C TInt GetAndLockAuthorizationRespond(
            MPEngAuthorizationRespond*& aRespond );



        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngReactAuthStoreImp*     iImp;
    };



#endif      //  __CPENGREACTAUTHSTORE_H__

//End of file
