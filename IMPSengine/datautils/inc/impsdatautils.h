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
* util classes for imps data accessor.
*
*/


#ifndef TImpsDataUtils_H
#define TImpsDataUtils_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "impsdataaccessorapi.h"
#include "impsdataaccessor.h"
#include "impsservercommon.h"
#include "impscommonenums.h"

// CONSTANTS

const TImpsContent KTransContentElements[]  =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent
    };

const TImpsContent KNewMessageElements[]  =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyNewMessage
    };

const TImpsContent KSendMessageElements[]  =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeySendMessage_Request
    };

//Client capability request
const TImpsContent KClientCapabilityReqElements[]  =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyClientCapability_Request
    };

const TImpsContent KClientServiceReqElements[]  =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyService_Request
    };

const TImpsContent KClientCapabilityResElements[]  =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyClientCapability_Response
    };

const TImpsContent KClientServiceResElements[]  =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyService_Response
    };

const TImpsContent KTransModeElements[]  =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionDescriptor,
      EImpsKeyTransactionMode
    };

const TImpsContent KKeepAliveReq[] =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyKeepAlive_Request
    };

const TImpsContent KGet12PollElements[] =
    { EImpsKeySession,
      EImpsKeyPoll
    };

const TImpsContent KGetPollElements[] =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionDescriptor,
      EImpsKeyPoll
    };

const TImpsContent KSetResultCodeElements[] =
    { EImpsKeySession,
      EImpsKeyTransaction,
      EImpsKeyTransactionContent,
      EImpsKeyStatus,
      EImpsKeyResult,
      EImpsKeyCode
    };

const TImpsContent KGetCIRElements[] =
    { EImpsKeySession,
      EImpsKeyCIR
    };

// FORWARD DECLARATIONS
class MImpsKey;
class TImpsDataUtils;


// CLASS DECLARATION

/**
*  A set of static methods that get certain data elements from data accessor.
*/
class TImpsDataUtils
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C TImpsDataUtils();


    public: // New functions

        /**
        * Add multiple key elements at once
        */
        IMPORT_C static void AddValuesFromArrayL(
            MImpsKey* aImpsKey,
            const TImpsContent* aElementArray,
            TInt aSize );


        /**
        * Get sender from internal data (C-S)
        * @param aPredecessor, element array,
        *        parent node of particular Recipient element
        * @param aAc data accessor
        * @param aSender UserID or SName output,
        *        Sender(User(UserID) or ScreenName if a GroupID
        * @param aGroup GroupID output,
        *        Sender(ScreenName(SName, GroupID))
        * @param aUsers UserIDs output
        */
        IMPORT_C static void GetSenderL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            TPtrC& aSender,
            TPtrC& aGroup );

        /**
        * Set recipients data (C-S)
        * @param aPredecessor key element path so far
        * @param aAc message data
        * @param @aSender UserID or SName
        * @param aGroupId ScreenName( GroupdID)
        */
        IMPORT_C static void SetSenderL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            const TDesC* aSender,
            const TDesC* aGroupId );


        /**
        * Set ContentData (C-S)
        * @param aKey, key that will be initialized!
        * @param aAc data accessor
        * @param ContentData,
        *        notice: converted to UTF-8 in Message Interpreter in this rel.
        */
        IMPORT_C static void SetContentDataL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            const TDesC& aContent
        );

        /**
        * Get ContentData (C-S)
        * @param aKey, key that will be initialized!
        * @param aAc data accessor
        * @param ContentData,
        *        notice: converted to UTF-8 in Message Interpreter in this rel.
        */
        IMPORT_C static void GetContentDataL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            TPtrC& aContent
        );

        /**
        * Set Result code value
        * @param aKey, key that will be initialized!
        * @param aAc data accessor
        * @param aCode result code to set
        */
        IMPORT_C static void SetResultStatusL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            TInt aCode );

        /**
        * Get Result code value (C-S)
        * @param aKey, key that will be initialized!
        * @param aAc data accessor
        * @param aCode result code back
        * @return ETrue if found, else EFalse
        */
        IMPORT_C static TBool GetResultStatusL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            TInt& aCode );

        /**
        * Get NewMessage::MessageInfo:MessageId (C-S)
        * @param aKey, key that will be initialized!
        * @param aAc data accessor
        * @param aID MessageID
        * @return ETrue if found, else EFalse
        */
        IMPORT_C static TBool GetNewMessageIdL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            TPtrC& aId );

        /**
        * CreateClientIdL (I)
        * @param aKey, key to the ClientID.
        *        ClientID is inserted,
        *        The Key is in the original state after this function
        * @param aAc data accessor
        * @param aClientId ClientID
        */
        IMPORT_C static void CreateClientIdL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            const TDesC& aClientId );

        /**
        * GetMessageTypeL (C-S)
        * @param aAc data accessor
        * @return messagetype integer
        */
        IMPORT_C static TInt GetMessageTypeL( MImpsDataAccessor* aAc );

        /**
        * GetTransactionIDL (S-I)
        * @param aAc data accessor
        * @param aID, KNullDesc if not found, TransactionID otherwise
        */
        IMPORT_C static void GetTransactionIDL( MImpsDataAccessor* aAc,
                                                TPtrC& aID );

        /**
        * SetTransactionIDL (I)
        * @param aAc data accessor
        * @param aID
        */
        IMPORT_C static void SetTransactionIDL( MImpsDataAccessor* aAc,
                                                const TDesC& aID );

        /**
        * GetSessionIDL (I)
        * @param aAc data accessor
        * @param aID, KNullDesc if not found, SessionID otherwise
        */
        IMPORT_C static void GetSessionIDL( MImpsDataAccessor* aAc,
                                            TPtrC& aID );

        /**
        * SetSessionIDL (S-I)
        * @param aAc data accessor
        * @param aID
        */
        IMPORT_C static void SetSessionIDL( MImpsDataAccessor* aAc,
                                            const TDesC& aID );

        /**
        * GetSessionTypeL (I)
        * @param aAc data accessor
        * @param aType, EImpsUndefSes if not found, type otherwise
        */
        IMPORT_C static void GetSessionTypeL(
            MImpsDataAccessor* aAc,
            TImpsSessionType& aType );

        /**
        * SetSessionTypeL (S-I)
        * @param aAc data accessor
        * @param aType session type
        */
        IMPORT_C static void SetSessionTypeL(
            MImpsDataAccessor* aAc, const TImpsSessionType aType );

        /**
        * GetPollL
        * @param aKey, empty in the beginning!
        * @param aAc data accessor
        * @param TBool, ETrue if poll is on, EFalse if off
        * @return ETrue if found, else EFalse
        */
        IMPORT_C static TBool GetPollL( MImpsKey* aKey,
                                        MImpsDataAccessor* aAc,
                                        TBool& aBool );

        /**
        * Copy specific part of the message to the current message
        * @param aSource where to copy data elements
        * @param aDestination
        */
        IMPORT_C static void CopyNewUsersL( MImpsDataAccessor& aSource,
                                            MImpsDataAccessor& aDestination );

        /**
        * Copy specific part of the message to the current message
        * @param aSource where to copy data elements
        * @param aDestination
        */
        IMPORT_C static void CopyLeftUsersL( MImpsDataAccessor& aSource,
                                             MImpsDataAccessor& aDestination );

        /**
        * Copy specific part of the message to the current message
        * @param aSource where to copy data elements
        * @param aDestination
        */
        IMPORT_C static void CopyGroupPropertiesL( MImpsDataAccessor& aSource,
                                                   MImpsDataAccessor& aDestination );

        /**
        * Copy specific part of the message to the current message
        * @param aSource where to copy data elements
        * @param aDestination
        */
        IMPORT_C static void CopyOwnPropertiesL( MImpsDataAccessor& aSource,
                                                 MImpsDataAccessor& aDestination );

        /**
        * Get value from central  repository
        * Leaves, if the repository or the id not found
        * @param aRepositoryUid uid of the repository
        * @param aId id of the wanted key
        * @return value of the wanted key
        */
        IMPORT_C static TInt GetCenRepIntValueL( TUid aRepositoryUid,
                                                 TUint32 aId );

        /**
        * Get value from central  repository
        * Leaves, if the repository or the id not found
        * @param aRepositoryUid uid of the repository
        * @param aId id of the wanted key
        * @return value of the wanted key
        */
        IMPORT_C static TReal GetCenRepRealValueL( TUid aRepositoryUid,
                                                   TUint32 aId );


        /**
        * Set ContentData (C-S)
        * @param aKey, key that will be initialized!
        * @param aAc data accessor
        * @param ContentData
        */
        IMPORT_C static void SetBinaryContentDataL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            const TDesC8& aContent
        );

        /**
        * Get ContentType from NewMessage(C-S)
        * @param aAc data accessor
        * @param ContentType
        */

        IMPORT_C static void GetContentTypeL(
            MImpsDataAccessor* aAc,
            TPtrC& aContentType );

        /**
        * Get ContentData datatype
        * @param aAc data accessor
        * @param aIndex index of the transaction
        * @return data type of ContentData
        */

        IMPORT_C static TImpsDataType GetContentDataTypeL(
            MImpsDataAccessor* aAc, TInt aIndex );


    private:


    };

#endif

// End of File
