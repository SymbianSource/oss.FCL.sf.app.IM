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
* Description:  Reactive authorization handling utilities
*
*/


#ifndef CCAREACTIVEAUTHORIZATION_H
#define CCAREACTIVEAUTHORIZATION_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include "mcareactiveauthobserverpc.h"

// FORWARD DECLARATIONS
class CCAAppUi;
class CCABlockingUI;
class MCAReactiveAuthPC;




// CLASS DECLARATION

/**
 *  Reactive authorization observers and logic
 *
 *  @lib chatng.exe
 *  @since 3.2
 */
class CCAReactiveAuthorization : public CBase,
            public MCAReactiveAuthObserverPC
    {
    public:  // constructor
        /**
         * Static constructor.
         * @since 3.2
         * @param aAppUi Handle to application UI
         * @param aReactiveAuthPC Handle to Reactive Auth. PC Interface
         * @return Instance of this class
         */
        static CCAReactiveAuthorization* NewLC( CCAAppUi& aAppUi,
                                                MCAReactiveAuthPC* aReactiveAuthPC );

        /**
         * Static constructor.
         * @since 3.2
         * @param aAppUi Handle to application UI
         * @param aReactiveAuthPC Handle to Reactive Auth. PC Interface
         * @return Instance of this class
         */
        static CCAReactiveAuthorization* NewL( CCAAppUi& aAppUi,
                                               MCAReactiveAuthPC* aReactiveAuthPC );

        /**
         * Destructor
         */
        ~CCAReactiveAuthorization();

    public:  //From Base Class MCAReactiveAuthObserverPC

        /**
         * @see MCAReactiveAuthObserverPC
         */
        void HandleRAPendingReqL();

        /**
         * @see MCAReactiveAuthObserverPC
         */
        void HandleRAStatusL();

        /**
         * @see MCAReactiveAuthObserverPC
         */
        void HandleRAError( TInt aError );

    private: // new methods

        /**
         * CIdle callback for RA request and status
         * @param aInstance - pointer that is passed to
         * the callback function when the function is called
         * @returns ETrue if it is intended to be called again
         * means if request is pending else EFalse
         * request consumer
         */
        static TInt ConsumerCallback( TAny* aInstance );

        /**
         * ConsumerCallback calls this function
         * wrapper for DoConsumerL
         * @returns ETrue if request is pending else EFalse
         */
        TInt Consumer();

        /**
         * The actual consumer method
         * Get Pending messages/statuses from IM Presence
         * @returns ETrue if request is pending else EFalse
         */
        TInt DoConsumerL();

        /**
         * This method does the actual RA request handling
         * @param aId The WVID contained within the request
         * @return ETrue if the user accepted this request
         */
        TBool DoHandleRAPendingReqL( const TDesC& aId );

        /**
         * This method does the actual RA status handling
         * @param aId The WVID contained within the status request
         */
        void DoHandleRAStatusL( const TDesC& aId );

        /**
         * This method does the actual RA error handling
         */
        void DoHandleRAError();

    private: // constructors and destructors

        /**
         * Symbian Second Phased Constructor
         */
        void ConstructL();

        /**
         * Default constructor
         * @param aAppUI Handle to application UI
         * @param aReactiveAuthPC Handle to Reactive Auth. PC Interface
         */
        CCAReactiveAuthorization( CCAAppUi& aAppUI,
                                  MCAReactiveAuthPC* aReactiveAuthPC );

    private:    // Data

        // Doesn't own. Handle to appui
        CCAAppUi& iAppUi;


        // Handle to blocking UI. Owned.
        CCABlockingUI* iBlockingUI;

        // owns, used to launch the actual request/status request
        // consumer methods
        CIdle* iIdle;

        // If idle is already running this flag is set to ETrue
        TBool iRequestPending;

        // Handle to Reactive Auth PC. Doesnt Own
        MCAReactiveAuthPC* iReactiveAuthPC;

    };

#endif      // CCAREACTIVEAUTHORIZATION_H

// End of File
