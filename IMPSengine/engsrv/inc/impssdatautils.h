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
* Description: 
* Uitility class for data accessor.* 
*/


#ifndef TImpsSDataUtils_H
#define TImpsSDataUtils_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "impsdataaccessorapi.h"
#include "impsdataaccessor.h"
#include "impsservercommon.h"
#include "impscommonenums.h"
#include "impsdatautils.h"

// CONSTANTS

// FORWARD DECLARATIONS
class MImpsKey;
class TImpsServices;
class TImpsSrvSettings;

// CLASS DECLARATION

/**
*  A set of static methods that get certain data elements from data accessor.
*/
class TImpsSDataUtils : public TImpsDataUtils
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TImpsSDataUtils();


    public: // New functions


        /**
        * Get SubscribeGroupNotice-Response::Value
        * @param aKey, key that will be initialized!
        * @param aAc data accessor
        * @return ETrue if T, else EFalse
        */
        static TBool SubGroupNoticeResponseL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc
        );


        /**
        * Set MessageDelivered::MessageId
        * @param aKey, key that will be initialized!
        * @param aAc data accessor
        * @param aID MessageID
        */
        static void SetMessageDeliveredIdL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            const TDesC& aId );

        /**
        * Create LoginRequest
        * @param aKey, key that will be initialized!
        * @param aAc data accessor
        * @param aUserId UserdID
        * @param aClientId ClientID
        * @param aPwd Password, plain text
        * @param aCookie SessionCookie
        * @param aTime TimeToLive
        */
        static void CreateLoginReqL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            const TDesC& aUserId,
            const TDesC& aClientId,
            const TDesC& aPwd,
            const TDesC& aCookie,
            TInt aTime );
        /**
        * Create CreateLoginReqPhaseOneL
        * @param aKey, key that will be initialized!
        * @param aAc data accessor
        * @param aUserId UserdID
        * @param aClientId ClientID
        * @param aSchema DigestSchema, MD5
        * @param aCookie SessionCookie
        * @param aTime TimeToLive
        */
        static void CreateLoginReqPhaseOneL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            const TDesC& aUserId,
            const TDesC& aClientId,
            const TImpsDigestSchema aSchema,
            const TDesC& aCookie,
            TInt aTime );

        /**
        * Create CreateLoginReqPhaseTwoL
        * @param aSendPwd
        * @param aAc data accessor
        * @param aCSPSessionId
        * @param aTid Transaction ID
        * @param aUserId UserdID
        * @param aClientId ClientID
        * @param aPwd Password
        * 2param aNonce
        * @param aCookie SessionCookie
        * @param aTime TimeToLive
        * @param aKey1 PrivateClientKey1
        * @param aKey2 PrivateClientKey1
        */
        static void CreateLoginReqPhaseTwoL(
            TBool aSendPwd,
            MImpsDataAccessor* aAc,
            const TDesC& aCSPSessionId,
            const TDesC& aTid,
            const TDesC& aUserId,
            const TDesC& aClientId,
            const TDesC& aPwd,
            const TDesC8* aNonce,
            const TDesC& aCookie,
            TInt aTime,
            HBufC* aKey1,
            HBufC* aKey2 );


        /**
        * CreateServiceRequestL
        * @param aAc data accessor
        * @param aServices requested services
        * @param aSID CSP sessionID
        * @param aTID CSP transactionID
        * @param aCID CSP clientID
        * @param aReactive reactive presence auth negotiation
        */
        static void CreateServiceRequestL(
            MImpsDataAccessor* aAc,
            TImpsServices& aServices,
            const TDesC& aSID,
            const TDesC& aTID,
            const TDesC& aCID,
            TBool aReactive );

        /**
        * CreateClientCapabilityReqL
        * @param aAc data accessor
        * @param aSID CSP sessionID
        * @param aTID CSP transactionID
        * @param aCID CSP clientID
        * @param aMultiTrans CSP MultiTrans
        * @param aPollTime CSP PollTime
        * @param aSettings current settings
        */
        static void CreateClientCapabilityReqL(
            MImpsDataAccessor* aAc,
            const TDesC& aSID,
            const TDesC& aTID,
            const TDesC& aCID,
            TInt aMultiTrans,
            TInt aPollTime,
            TImpsSrvSettings& aSettings );


        /**
        * GetKeepAliveL
        * Get KeepAliveTime from KeepAlive-Reaponse or Login-Response
        * @param aAc data accessor
        * @param aTime output param for value
        * @return ETrue if found, else EFalse
        */
        static TBool GetKeepAliveL(
            MImpsDataAccessor* aAc,
            TInt& aTime );

        /**
        * GetLoginSessionIDL
        * @param aAc data accessor
        * @param aID, KNullDesc if not found, SessionID otherwise
        */
        static void GetLoginSessionIDL( MImpsDataAccessor* aAc, TPtrC& aID );

        /**
        * Set ContentDataType
        * @param aAc data accessor
        * @param ContentData, MIME type, OUT
        */
        static void ContentDataTypeL(
            MImpsDataAccessor* aAc,
            TPtrC& aContent );

        /**
        * GetApplicationIDL from NewMessage transaction
        * @param aAc data accessor
        * @param aID, KNullDesc if not found, ApplicationID otherwise
        */
        static void GetApplicationIDL( MImpsDataAccessor* aAc, TPtrC& aID );

        /**
        * GetCIRL
        * This flag exists in WV 1.2 and higher. WV 1.1 message returns always false.
        * @param aAc data accessor
        * @return ETrue if this flag is on.
        */
        static TBool GetCIRL( MImpsDataAccessor* aAc );

        /**
        * GetPureMessageTypeL
        * Get actual message type stored under pureData
        * @param aAc data accessor
        * @return messageType
        */
        static TInt GetPureMessageTypeL( MImpsDataAccessor* aAc );

    private:


    };

#endif

// End of File
