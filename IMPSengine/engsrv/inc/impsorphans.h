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
* Description: Orphan message and message queue
* 
*/


#ifndef Orphans_H
#define Orphans_H

//  INCLUDES
#include <e32base.h>
#include "impsservercommon.h"
#include "impsapplauncherproxy.h"
#include <impsservices.h>
#include "impssrvutils.h"


// FORWARD DECLARATIONS
class COrphan;
class CImpsFields;
class MImpsDataAccessor;



// CLASS DECLARATION
/**
*  Orphan message
*/

class COrphan : public CBase
    {
    public:
        static COrphan* NewL(
            const TDesC& aCID,
            const TDesC& aSessionId,
            TImpsEventType aServiceType,
            const TDesC&    aSap,
            const TDesC&    aUserId,
            TTime aValidity,
            CImpsFields* aFields );

        ~COrphan();
        void Destroy();
        TDesC& ApplicationId();
        CImpsFields* Message();
        TTime Expiry();
        void RemoveOwnership();

        TBuf<KImpsMaxClientId> iCID;        //lint !e1925  // CSP client-id
        TBuf<KImpsMaxSID>   iSessionId;     //lint !e1925  // CSP session id
        TImpsEventType      iServiceType;    //lint !e1925  // event type
        HBufC*              iSap;
        HBufC*              iUserId;
        CImpsFields*        iFields;        //lint !e1925
        TTime               iExpiry;        // Validity time
        TDblQueLink         iLink;          //lint !e1925


    private:
        COrphan(
            const TDesC& aCID,
            const TDesC& aSessionId,
            TImpsEventType aServiceType,
            TTime aValidity,
            CImpsFields* aFields );

        void ConstructL(
            const TDesC&    aSap,
            const TDesC&    aUserId );


    };

// CLASS DECLARATION
/**
*  Double linked fixed size queue for orphan messages. When the queue at max
*  capacity the oldest item gets deleted.
*/

class COrphanQueue : public CBase
    {
    public:

        /**
        * Constructs a new COrpanQueue with given capacity
        * @param aCapacity the capacity of the queue
        */
        COrphanQueue( TInt aCapacity );

        // Destructor
        ~COrphanQueue();

        /**
        * Sets the capacity of the queue
        * @param aNewCapacity the new capacity of the queue
        */
        void SetLength( TInt aNewCapacity );

        /**
        * Adds a new orphan message to the end of the queue
        * @param aOrphan an orphan message to be added
        */
        void Add( COrphan& aOrphan );

        /**
        * Destroys orphan messages in the queue with the given ClientID
        * @param aApplicationId an application ID to be destroyed
        */
        void DestroyOrphans( TDesC& aApplicationId );

        /**
        * Returns an orphan message with the given clientID and removes
        * it from the queue. If not found returns NULL.
        * @param aApplicationId an application ID to be returned
        * @param aServiceType the type of the client which requests the message
        */
        CImpsFields* OrphanMessageLC(
            const TDesC& aApplicationId,
            TImpsEventType aServiceType,
            TImpsSessIdent aCspId );


        /**
        * Deletes all expired orphan messages.
        * @param aExpiry expiry time
        */
        void DiscardExpired( TTime aExpiry );

    private:
        TDblQue<COrphan>        iOrphanList;
        TInt                    iCapacity;
        TInt                    iSize;
    };

/**
* CImpsOrphans
*/
class CImpsOrphans : public CBase,
            public MImpsAppLaunchHandler
    {
    public:

        /**
        * Destructor
        */
        virtual ~CImpsOrphans();

        /**
        * create new instance
        */
        static CImpsOrphans* NewL();

        /**
        * Store message and launch application
        * @param aMsg orphan message
        */
        virtual void NewOrphanL( CImpsFields* aMsg, TImpsSessIdent aCspId );

        /**
        * Get next orphan
        * @param aApplicationId
        * @param aServiceType the type of the client which requests the message
        * @return new message, NULL if no match. Ownership of the msg is
        * passed to the caller of this method.
        */
        virtual CImpsFields* NextOrphanLC(
            const TDesC& aApplicationId,
            TImpsEventType aServiceType,
            TImpsSessIdent aCspId );

        /**
        * Check expiration of orphans
        */
        virtual void CheckExpiryL( );

        /**
        * Stop
        */
        virtual void Stop( );

    public:
        void HandleAppLaunch( const TDesC& aApplicationId, TInt aStatus );

    protected:

        void ConstructL();

        /**
        * Adds the orphan message to the queue.
        * @param aFields the received message in internal data format
        * @param aServiceType the type of the client which requests the message
        * @returns pointer to the orphaned message
        */
        COrphan* AddOrphanL(
            CImpsFields *aFields,
            TImpsEventType aServiceType,
            TImpsSessIdent aCspId );

        /**
        * Gets the IM validity from the internal data. If not present it returns
        * the default validity.
        * @param aAc data accessor
        * @param aValidity the validity
        */
        void GetValidityFromMessageL(
            MImpsDataAccessor& aAc,
            TTimeIntervalSeconds& aValidity );

        /**
        * Gets the corresponding service type for a given message
        * @param aMessageType type of the message
        * @returns the service type
        */
        TImpsEventType ServiceType( TImpsMessageType aMessageType );

    private:
        /**
        * Constructor
        */
        CImpsOrphans( );

    protected:
        // Orphan message queue
        COrphanQueue*           iOrphanQueue;
        CImpsAppLauncherProxy*  iLauncherProxy;
    };

#endif
