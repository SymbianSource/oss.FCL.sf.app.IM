/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Listen global even request handler
*
*/



#ifndef __CPENGHANDLERLISTENEVENTS_H__
#define __CPENGHANDLERLISTENEVENTS_H__

//  INCLUDES
#include <e32base.h>

#include "CPEngRequestHandler.h"


/**
 * Listen global even request handler
 * Implements global even listen even handling
 *
 *  @since 3.0
 */
class CPEngHandlerListenEvents :
            public CPEngRequestHandler
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CPEngHandlerListenEvents* NewL( TUint32 aSessionId  );

        /**
         * Two-phased constructor.
         */
        static CPEngHandlerListenEvents* NewLC( TUint32 aSessionId );

        /**
         * Destructor.
         */
        virtual ~CPEngHandlerListenEvents();


    public: // Function from CPEngRequestHandler

        /**
         *	Notify client about new event
         *
         *	@since 3.0
         *	@param aNewEvent new event which has occurred
         */
        void NotifyEvent( const TDesC8& aGlobEvent );

        /**
         *  Notify Client about error
         *
         *  @since 3.0
         *  @param aErrorCode error code to be notified
         */
        void NotifyError( TInt aErrorCode );

        /**
         *	Reloads request handler with new RPEngMessage
         *
         *	@since 3.0
         *	@param aNewScout new message of the asynchronous request
         */
        void ReloadScoutWithNewMessageL( const RPEngMessage& aNewScout );


    protected:  // Functions from base classes, to allow further derivation

        /**
         *	Buffer Global event notification
         *
         *  @since 3.0
         *  @param aEvent event to buffer
         */
        inline TInt BufferNotification( const TDesC8& aEvent );

    private:

        /**
         * C++ default constructor.
         */
        CPEngHandlerListenEvents( TUint32 aSessionId );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:    // Data

        /// OWN: Missed events buffer
        CDesC8ArrayFlat						iMissedEvents;

        /// OWN: Last not reported error code
        TInt								iLastResult;

    };

#endif      // __CPENGHANDLERLISTENEVENTS_H__

// End of File
