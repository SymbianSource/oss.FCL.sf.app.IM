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
* Description:  Utility class for presence functions
*
*/



#ifndef CAPRESENCEUTILS_H
#define CAPRESENCEUTILS_H

//  INCLUDES
#include "TStorageManagerGlobals.h"
#include "CAPresenceDefinitions.h"
#include "MCAContactLists.h"

#include <e32def.h>
#include <badesca.h>

#include "impsbuilddefinitions.h"

// FORWARD DECLARATIONS
class MCAWatcherObserver;
class MPEngPresenceAttrModel2;
class MCAStoredContacts;
class MPEngContactList2;
class MPEngTransactionStatus2;
class CCAPresenceErrors;
class TPEngNotify;
class MCAStoredContact;
struct TDecodeAttrParams;
class TResourceReader;
class MCAPresenceUpdater;
class MCAContactList;

// helper classes for storing attributes
class TCAAttribute
    {
    public: // data

        TUint32 iAttribute;
        TBool iQualifier;
        TInt iData;
        TInt iField;
        TInt iGroup;
    };

class CCAState : public CBase
    {
    public:
        // constructor
        CCAState( TInt aStateId )
                : iStateId( aStateId )
            {
            };

        // destructor
        virtual ~CCAState()
            {
            iAttributes.Close();
            };

    public: // data

        TInt iStateId;
        RArray<TCAAttribute> iAttributes;

    };


// CLASS DECLARATION

/**
*  Utility class for presence functions
*
*  @lib CAPresence.dll
*  @since 2.1
*/
class CAPresenceUtils // CSI: 51 # This is not a C-class
            // although it begins with CA.
    {
    public: // New functions

        /**
        * Decodes given TPresenceAttrEntity (clientType) to storage TClientType
        * @since 2.1
        * @return Storage TClientType
        */
        static TStorageManagerGlobals::TClientType DecodeClientType(
            TCAAttribute aClientTypeAttr );

        /**
        * Decodes given TPresenceAttrEntities (Online, CommCAP/IM) to presence
        * TPresenceStatus
        * @since 2.1
        * @param aStates State-array
        * @param aOnlineAttr Online-attribute
        * @param aIMStatusAttr CommCAP/IM-attribute
        * @param aUserAvailAttr User availability attribute
        * @return Presence TPresenceStatus
        */
        static MCAPresence::TPresenceStatus DecodeOnlineState(
            const RPointerArray<CCAState>& aStates,
            const RPointerArray<MPEngPresenceAttrModel2>& aAttributes );

        /**
        * Fills given arrays with given data (aUserId,aPresenceStatus).
        * You can give only selected arrays that will be populated.
        * @since 2.1
        * @param aUserId WVId of user
        * @param aPresenceStatus Presence status of given user
        * @param aStateOnline Online-array
        * @param aStateOnline aStateOffline-array
        * @param aStateOnline aStateUnknown-array
        */
        static void FillArraysL( const TDesC& aUserId,
                                 TStorageManagerGlobals::TPresenceStatus aPresenceStatus,
                                 CDesCArray *aStateOnline,
                                 CDesCArray *aStateOffline,
                                 CDesCArray *aStateUnknown );

        /**
        * Populates some of given attributes with data found from given model
        * @since 3.0
        */
        static void PopulateAttributesL(
            const MPEngPresenceAttrModel2& aModel,
            RPointerArray<MPEngPresenceAttrModel2>& aArray,
            TStorageManagerGlobals::TClientType& aClientType,
            TPtrC& aAlias,
            TPtrC& aStatusText );

        /**
        * Generates array of friends.
        * Array ownership is transferred to caller!!!
        * @since 2.1
        * @param aContactStorage Handle to contact storage
        * appending to result array
        * @return Array of friends.
        */
        static CPtrCArray* GenerateFriendsArrayLC(
            MCAStoredContacts* aContactStorage );

        /**
        * Pushes given array to CleanupStack
        * @since 2.1
        * @param aArray Array to be pushed to CleanupStack
        * @param aOwnsItems Does array own it's objects
        */
        static void PushModelArrayL(
            RPointerArray<MPEngPresenceAttrModel2>& aArray,
            TBool aOwnsItems );

        /**
         * Internally used by IM Presence.
         * Decode the attribute models.
         * @param aParams The structure used when decoding the parameters
         * @return ETrue if all done?
         */
        static TBool DecodeAttrModelsL(
            struct TDecodeAttrParams& aParams
        );

        /**
        * Handles given error.
        * This method will leave with given error, except when error is:
        * KErrNone/KErrNotFound/KErrAlreadyExists. These errors can be ignored
        * with presence engine list-handling.
        * @since 2.1
        * @param aError Errorcode
        */
        static void HandleListErrorsL( TInt aError );

        /**
        * Compares two attribute models by presence ids.
        * @since 2.1
        * @param aFirst First model
        * @param aSecond Second model
        * @return Comparison value (@see TDesC::CompareF)
        */
        static TInt CompareAttrModelArray(
            const MPEngPresenceAttrModel2& aFirst,
            const MPEngPresenceAttrModel2& aSecond );

        /**
        * Calls close to given array.
        * @since 2.1
        * @param aObject Array (RPointerArray<MPEngPresenceAttrModel>)
        */
        static void CloseModelArray( TAny* aObject );

        /**
        * Calls ResetAndDestroy to given array
        * @since 2.1
        * @param aObject Array (RPointerArray<MPEngPresenceAttrModel2>)
        */
        static void DestroyCloseModelArray( TAny* aObject );

        /**
        * Creates detailed error description to error container based on
        * information retrieved from given MPEngTransactionStatus
        * @since 2.1
        * @param aStatus Transaction status
        * @param aErrorContainer Error container
        */
        static void HandleTransactionStatusL(
            MPEngTransactionStatus2& aStatus,
            CCAPresenceErrors& aErrorContainer );

        /**
        * Changes given error's base to Imps_ERROR_BASE if given error is
        * in PEC range (KPEngErrorBase-KPEngErrorWVServerResponseBase)
        * @since 2.1
        * @param aError Original errorcode
        * @return Mapped errorcode
        */
        static TInt MapErrorPECtoCSP( TInt aError );

        /**
         * Appends contacts in multiple rounds. Used by CIdle.
         * @param aContactList contact list which is added to local data
         * @param aWVIds if not null then contact wvids are added here
         * @param aNicknames if not null then nicnames are added here
         * @param aContactStorage if aWVIds or aNicknames are null, then
         *        contacts are added to this storage
         * @param aEntryIndex current index were from continue contact
         *                    list handling
         * @return ETrue if method should be called again to continue
         *         list handling
         */
        static TBool AppendContactsL( MPEngContactList2* aContactList,
                                      CDesCArray* aWVIds,
                                      CDesCArray* aNicknames,
                                      MCAStoredContacts* aContactStorage,
                                      TInt& aEntryIndex,
                                      MCAContactList& aCaContactList,
                                      MCAPresenceUpdater& aPresenceUpdater );



        /**
         * Reads one attribute from resource
         * @param aResourceId Resource Id
         * @return Attribute
         */
        static TCAAttribute ReadAttributeL( TResourceReader& aReader );

        /**
         * Map resource state id to presence state id
         */
        static TInt MapStateId( TInt aResourceStateId );

        /**
         * Map presence state id to storage state id
         */
        static TStorageManagerGlobals::TPresenceStatus MapPresenceStateId(
            MCAPresence::TPresenceStatus aPresenceStateId );

        /**
         * Reads presence states and attribute combinations from resource
         * @param aResourceId Resource Id
         * @param aStates Containes states on return
         */
        static void ReadStatesFromResourceL( TInt aResourceId,
                                             RPointerArray<CCAState>& aStates );

        /**
         * @return State matching to aState, or NULL if not found
         */
        static CCAState* FindStateL( TInt aState, const RPointerArray<CCAState>& aStates );

        /**
         * @return Attribute matching to aType, or NULL if not found
         */
        static const MPEngPresenceAttrModel2* FindAttr( TInt aType,
                                                        const RPointerArray<MPEngPresenceAttrModel2>& aArray );

        /**
         * @return ETrue if states match
         */
        static TBool EqualStates( const CCAState& aState1, const CCAState& aState2 );

    };

#endif      // CAPRESENCEUTILS_H

// End of File
