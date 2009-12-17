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
* Description:  Implementation for Reactive Authorization PC Interface
*
*/


#ifndef CCAREACTIVEAUTHPC_H
#define CCAREACTIVEAUTHPC_H


// INCLUDES
#include	"mcareactiveauthpc.h"
#include    "MCAReactiveAuthObserver.h"





// FORWARD DECLARATIONS
class MCAReactiveAuthObserverPC;
class MCAPresence;




//class declaration
/**
 *  Implementation class for Reactive Authorization PC Interface
 *
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class CCAReactiveAuthPC: public CBase,
            public MCAReactiveAuthPC,
            public MCAReactiveAuthObserver

    {

    public:

        /**
         * Symbian Two Phase Constructor
         */
        static CCAReactiveAuthPC* NewL();

        /**
         * virtual destructor
         */
        virtual ~CCAReactiveAuthPC();

    private:
        /**
         * default constructor
         */

        CCAReactiveAuthPC( );


        /**
         * Second phased symbian constructor.
         */
        void ConstructL();

    public:

        /**
         * Find any contact which id is aContactId
         * @param aContactId Contactid to recognize contact.
         * @return ETrue if contact found else EFalse
         */
        TBool FindAnyContactL( const TDesC& aContactId );


        /**
         * Get alias for given user.
         * Use this only for one-shot rare operations.
         * @param aUserId The user id for which to fetch alias attribute
         * @return The alias, ownership is transferred.
         * Leaves with KErrNotFound if the alias cannot be found,
         * otherwise some other KErr-code (e.g. KErrNoMemory)
         */
        HBufC* GetAliasL( const TDesC& aUserId  );

        /**
         * Get the list of all users who have rejected the auth. request
         * request for processing
         * @param aUserId Will contain the user id
         * @param aUserIdsArray list of all userids who have rejected the auth. request
         */
        void GetListOfRAStatusL( TDes& aUserId,
                                 CDesCArray& aUserIdsArray  );

        /**
         * Get the information from the next reactive authorization request for
         * processing
         * @param aId Will contain the id for the request
         * @param aUserId Will contain the user id
         * @param aLastPos Position of previous found request in queue
         * @return ETrue if there is another request in the queue
         */
        TBool GetNextPendingRARequestL( TInt& aId,
                                        TDes& aUserId,
                                        TInt& aLastPos );

        /**
         * Send reactive authorization response
         * @param aId The id number given through observer
         * @param aResponse The response, ETrue if authorizing,
                             EFalse if denying
         */
        void SendReactiveAuthResponseL( TInt aId,
                                        TBool aResponse );


    public://From MCAReactiveAuthPC

        /**
         * To add the UI side observer for MCAReactiveAuthObserverPC
         * events from the engine.
         * @param aObserver - observer to be added
         */
        void AddReactiveAuthObserver( MCAReactiveAuthObserverPC* aObserver );

        /**
         * To remove the UI side observer for MCAReactiveAuthObserverPC
         * events from the engine.
         */
        void RemoveReactiveAuthObserver();


    public:// From MCAReactiveAuthObserver

        /**
         * Incoming pending reactive authorization request.
         */
        void HandleRAPendingReqL() ;

        /**
         * Incoming reactive authorization status message
         */
        void HandleRAStatusL() ;

        /**
         * Error handler, if any of the observer methods leave.
         * @param aError The error code
         */
        void HandleRAError( TInt aError ) ;

    private:

        /**
         * Does not own, do not delete
         * iReactiveAuthObserverPC: a reference to the observer
         * MCAReactiveAuthObserverPC for events
         * from MCAReactiveAuthPC to UI. MCAReactiveAuthPC inturn receives
         * the actual engine events from MCAReactiveAuthObserver.
         */
        MCAReactiveAuthObserverPC* iReactiveAuthObserverPC;

        //Doesnt Own, Handle to chat presence
        MCAPresence* iPresence;


    };

#endif      // CCAREACTIVEAUTHPC_H

// End of File
