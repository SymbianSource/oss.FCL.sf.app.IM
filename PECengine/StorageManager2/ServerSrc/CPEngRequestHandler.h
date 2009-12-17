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
* Description:  Template of the Server request handler implementation
*
*/



#ifndef __CPENGREQUESTHANDLER_H__
#define __CPENGREQUESTHANDLER_H__

//  INCLUDES
#include <e32base.h>

#include "TPEngStorageServerMessages.h"
//	Own RMessage class wraper
#include "RPEngMessage.h"

// CLASS DECLARATION
/**
 *  Template of the Server request handler implementation
 *  It implements skeleton of one request handler
 *
 *  @since 3.0
 */
class CPEngRequestHandler : public CObject
    {
    public:  // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~CPEngRequestHandler();

    public: // New functions

    public: // Function from MPEngRequestHandler

        /**
         *	Return Session ID of the session from which request was issued
         *
         *	@since 3.0
         *	@return session ID
         */
        TUint32 SessionId() const ;

        /**
         *	Return Sub Session ID of the session from which request was issued
         *
         *	@since 3.0
         *	@return session ID
         */
        TInt SubSessionId() const ;

        /**
         *	Return Request function
         *
         *	@since 3.0
         *	@return request function
         */
        TPEngStorageServerMessages RequestFunction() const;

        /**
         *	Cancel Request
         *
         *	@since 3.0
         */
        void CancelRequestD();

        /**
         *	Reloads request handler with new RPEngMessage, this is used when
         *
         *	@since 3.0
         *	@param aNewScout new message ofthe asynchronous request
         */
        virtual void ReloadScoutWithNewMessageL( const RPEngMessage& aNewScout ) = 0;


    protected:	// Protected constructor to allow derivation

        /**
         * C++ default constructor.
         */
        CPEngRequestHandler(	TPEngStorageServerMessages aRequestFunction,
                             TUint32 aSessionID,
                             TInt aSubSessionID );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();


    protected:  // Data protected to allow derivation

        /// OWN: Request RPEngMessage
        RPEngMessage						iRequestMessage;

        /// OWN: Session ID from where request was issued
        TUint32								iSessionID;

        /// OWN: Sub Session ID, from where request came
        TInt								iSubSessionID;

        /// OWN: Request Function
        TPEngStorageServerMessages			iRequestFunction;

    private:    // Data
    };

#endif      // __CPENGREQUESTHANDLER_H__

// End of File
