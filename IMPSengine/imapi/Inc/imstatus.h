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
* Description:  Interface for Instant Messaging services
*
*/


#ifndef OPENAPI_IMSTATUS_H
#define OPENAPI_IMSTATUS_H

// INCLUDES
#include <e32base.h>
#include <ecom/ECom.h>
#include <badesca.h>
#include <cntdef.h>

// DATA TYPES
// IM Status attribute
enum TIMAttribute
    {
    EIMOnline = 0,
    EIMOffline,
    EIMAvailable,
    EIMNotAvailable,
    EIMDiscreet
    };


// FORWARD DECLARATIONS
class MImStatusObserver;
class MImStatusList;

// CLASS DECLARATION
class MImStatusFetcher
    {

    public:

        virtual ~MImStatusFetcher() {}

        /**
        * Method for registering the IM Status Fetcher to the WV Engine.
        * This method is asynchronous method and it will signal its
        * completion using the HandleRegisterL function.
        * @since S60 2.6
        * @param aObserver observer object used for notifying the user
        *        software
        * @return Status codes (to be defined)
        */
        virtual TInt RegisterObserverL(
            MImStatusObserver* aObserver ) = 0;

        /**
        * Method for unregistering the IM StatusFetcher from the WV Engine.
        * This method is synchronous method.
        * @since S60 2.6
        * @return Status codes (to be defined)
        */
        virtual TInt UnregisterObserverL( ) = 0;

        /**
        * Method for getting the users' online status using Contact Model IDs.
        * This method is asynchronous and the results are obtained
        * in HandleGetOnlineStatusL() function.
        * @since S60 2.6
        * @param aContactIds array of contact model IDs
        * @return positive number is operation code. Negative numbers are
        *                   status codes (to be defined)
        */
        virtual TInt GetOnlineStatusL(
            const CContactIdArray& aContactIds ) = 0;

        /**
        * Method for getting the users' online status using User IDs.
        * This method is asynchronous and the results are obtained
        * in HandleGetOnlineStatusL() function.
        * @since S60 2.6
        * @param aUserIds array of user IDs
        * @return positive number is operation code. Negative numbers are
        *                   status codes (to be defined)
        */
        virtual TInt GetOnlineStatusL(
            const CDesCArray& aUserIds ) = 0;


    };


// CLASS DECLARATION
/**
* MImStatusObserver
*
* Abstract interface for handling the notify events from the API.
* User derives his class from this and implements the observer methods below.
*/
class MImStatusObserver
    {
    public:

        /**
        * Method for getting the users' online status.
        * @since S60 2.6
        * @param aOpCode operatin code (see also the GetOnlineStatusL() method)
        * @param aErrorCode error codes (to be defined)
        * @param aStatusData the Online statuses of the requested users.
        *                    The user takes the ownership of the object!
        */
        virtual void HandleGetOnlineStatusL(
            const TInt aOpCode,
            const TInt aErrorCode,
            MImStatusList* aStatusList ) = 0;
    };



// CLASS DECLARATION
/**
* MImStatusList
*
* Abstract interface for reading out the results of the satus fetching
* This interface is returned in the HandleGetOnlineStatusL() method.
*/
class MImStatusList
    {
    public:
        /**
        * Method for getting the number of the returned statuses
        * @since S60 2.6
        * @return the number of the statuses
        */
        virtual TInt Count() = 0;

//        virtual TContactItemId& OwnerContactIdL( TInt aIndex ) = 0;

        /**
        * Method for getting the status owner user ID
        * @since S60 2.6
        * @param aIndex index of the IM status
        * @return the owner user ID
        */
        virtual TPtrC OwnerUserIdL( TInt aIndex ) = 0;

        /**
        * Method for getting the status value
        * @since S60 2.6
        * @param aIndex index of the IM status
        * @return the value of the IM status
        */
        virtual TIMAttribute IMStatusL( TInt aIndex ) = 0;
    };


#endif
// End of File
