/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Holds information of one network request
*
*/


#ifndef CCAREQUEST_H
#define CCAREQUEST_H

// INCLUDES
#include <ImpsDetailed.h>

#include "PublicEngineDefinitions.h"

// FORWARD DECLARATIONS
class CCARequestMapper;

// CLASS DECLARATION

class CCARequest : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aOpId Operation Id
        * @param aDestroyAfterComplete Should the request be destroyed
        * after it's complete
        * @param aCallBack Callback function to be executed after complete
        */
        static CCARequest* NewL( TInt aOpId,
                                 TBool aDestroyAfterComplete,
                                 TCallBack aCallBack );

        /**
        * Destructor.
        */
        virtual ~CCARequest();

    public:

        /**
        * Executes callback function if one is defined
        * @return Return value of callback function
        */
        TInt ExecuteCallBackFunction() const;

        /**
        * Is the request marked to be destroyed after complition
        * @return Gets destroyed after complete or not
        */
        TBool DestroyAfterComplete() const;

        /**
        * Checks if given id matches this request's id
        * @since 1.2
        * @param aOpId Operation id
        * @return Does it match or not
        */
        TBool IdMatches( TInt aOpId ) const;

        /**
        * Starts waiting for HandleRequest-call
        * @since 1.2
        */
        void StartWait();

        /**
        * Stops waiting if one is in active state
        * @since 1.2
        */
        void StopWaitIfNeeded();

        /**
        * Sets the error status for this request
        * @since 1.2
        * @param aErrorCode Error code
        */
        void SetErrorCode( TInt aErrorCode );

        /**
        * Returns the error status of this request
        * @since 1.2
        * @return Error status
        */
        TInt ErrorCode() const;

        /**
        * Sets the request type for this request
        * @since 1.2
        * @param aRequestType Type of request
        */
        void SetRequestType( TInt aRequestType );

        /**
        * Return the type of request that was set to this request
        * @since 1.2
        * @return Request type
        */
        TInt RequestType() const;

        /**
        * Sets an array of additional data for this request
        * @since 1.2
        * @param aAdditionalArray Array
        */
        void SetAdditionalData( const MDesCArray& aAdditionalArray );

        /**
        * Returns a pointer to additional data array.
        * Does not transfer ownership!
        * @since 1.2
        * @return Additional data array
        */
        const MDesCArray* AdditionalData() const;

        /**
         * Sets the mapper for this request
         * @param aRequestMapper Request mapper
         */
        void SetRequestMapper( CCARequestMapper* aRequestMapper );

    private:    // constructors

        /**
        * C++ default constructor.
        * @see CCARequest::NewL for parameters
        */
        CCARequest( const TInt aOpId, TBool aDestroyAfterComplete, TCallBack aCallBack );

    private:    // data

        TInt iOpId; // current operation id
        TInt iErrorCode; // error code
        TInt iRequestType; // request/operation type

        // Doesn't own! Used to hold additional data for this request
        const MDesCArray* iAdditionalArray;
        CActiveSchedulerWait iWait;
        TCallBack iCallBack;
        TBool iDestroyAfterComplete;

        // Doesn't own. Mapper for this request
        CCARequestMapper* iRequestMapper;
    };

#endif      // CCAREQUEST_H

// End of File
