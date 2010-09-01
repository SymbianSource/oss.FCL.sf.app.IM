/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Network operations for one chat group handling.
*
*/


#ifndef CCAGGROUPWRAPPER_H
#define CCAGGROUPWRAPPER_H

// INCLUDES
#include "MCAGroupOperations.h"
#include "PublicEngineDefinitions.h"
#include "PrivateEngineDefinitions.h"
#include "ImpsGroupCli.h"

#include <e32base.h>

// FORWARD DECLARATIONS

class MCAExtendedStoredGroup;
class MCAImpsGroupClient;
class MCASettings;
class MCAGroupEventObserver;
class MDesCArray;

class CCARequest;
class CCARequestMapper;
class CCoeEnv;

// CLASS DECLARATION

/**
 *  Network operations for one chat group handling.
 *
 *  @lib CAEngine.lib
 *  @since 2.1
 */
class CCAGroupWrapper : public CBase,
            public MCAGroupOperations,
            public MImpsGroupHandler2
    {

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aSettingsAPI is the API to the settings side.
         * @param aGroup is group that network operations wrapper handles
         * @param aRequestMapper is the API to the request mapper.
         *        Used to provide synchronous waiting.
         * @param aClient is IMPS engine interface for network operations
         */
        static CCAGroupWrapper* NewL(
            MCASettings& aSettingsAPI,
            MCAExtendedStoredGroup& aGroup,
            CCARequestMapper& aRequestMapper,
            MCAImpsGroupClient* aClient );

        /**
        * Destructor.
        */
        virtual ~CCAGroupWrapper();

    public:	// New Methods

        /**
         * Returns handled group
         * @return Handled group
         */
        MCAStoredGroup& Group();

        /**
         * Adds event observer for group events
         * @param aObserver is group events observer
         */
        void RegisterEventObserver( MCAGroupEventObserver* aObserver );

        /**
         * Gets group properties
         * @param aCommonProps is group common properties
         * @param aPrivateProps is group private properties
         */
        void LocalProperties( CImpsCommonGroupProps*& aCommonProps,
                              CImpsPrivateGroupProps*& aPrivateProps );

        /**
         * Sets group properties
         * @param aCommonProps is group common properties
         * @param aPrivateProps is group private properties
         */
        void SetLocalProperties( CImpsCommonGroupProps* aCommonProps,
                                 CImpsPrivateGroupProps* aPrivateProps );

        /**
         * Gets network operations as request
         * @return current request
         * @see CCARequest
         */
        CCARequest* Request();

        /**
         * Propagates IMPS error code.
         * @param aError last IMPS error.
         */
        void LastImpsError( TInt aError );

        /**
         * Set is new topic shown or not.
         * @since S60 v3.1
         * @param aIsShown Is topic shown or not.
         */
        void SetTopicShown( TBool aIsShown );

        /**
         * Is new topic shown or not.
         * @since S60 v3.1
         * @return ETrue if topic already shown, EFalse if not.
         */
        TBool IsTopicShown() const;

    public: // From MCAGroupOperations

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::OperationId
         */
        TInt	OperationId();

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::JoinL
         */
        TInt JoinL( const TDesC& aScreenName, TBool aUsers,
                    TBool aIsFavourite );

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::CreateGroupL
         */
        TInt CreateGroupL( const TDesC& aScreenName, TBool aJoinGroup );

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::LeaveL
         */
        TInt LeaveL();

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::DeleteFromNetworkL
         */
        TInt DeleteFromNetworkL();

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::UpdatePropertiesL
         */
        TInt UpdatePropertiesL( CImpsCommonGroupProps* aCommonProps,
                                CImpsPrivateGroupProps* aPrivateProps );

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::AddMembersL
         */
        TInt AddMembersL( const CDesCArray& aUserList,
                          const CDesCArray& aScreenNameList );

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::RemoveMembersL
         */
        TInt RemoveMembersL( const CDesCArray& aUserList );

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::ModifyMembersL
         */
        TInt ModifyMembersL( const CDesCArray* aAdminList,
                             const CDesCArray* aModerList,
                             const CDesCArray* aOrdinaryList );

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::GetPropertiesL
         */
        TInt GetPropertiesL( CImpsCommonGroupProps*& aCommonProps,
                             CImpsPrivateGroupProps*& aPrivateProps );

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::GetMembersL
         */
        TInt GetMembersL( CDesCArray& aUserList,
                          CDesCArray& aScreenNames,
                          CDesCArray& aModers,
                          CDesCArray& aAdmins );

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::GetRejectListL
         */
        TInt GetRejectListL( CDesCArray& aRejectList );

        /**
         * From MCAGroupOperations
         * @see MCAGroupOperations::SetRejectListL
         */
        TInt SetRejectListL( const CDesCArray& aRejectedList,
                             const CDesCArray& aEnabledList );

    public:	// From MImpsGroupHandler

        /**
         * From MImpsGroupHandler
         */
        void HandleCompleteL( TInt aOpId,
                              TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler
         */
        void HandleJoinL( TInt aOpId,
                          const MDesCArray& aUserList,
                          const MDesCArray& aScreenNames,
                          const TDesC& aWelcomeText,
                          TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler
         */
        void HandleGroupMembersL( TInt aOpId,
                                  const MDesCArray& aUserList,
                                  const MDesCArray& aScreenNames,
                                  const MDesCArray& aModers,
                                  const MDesCArray& aAdmins,
                                  TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler
         */
        void HandleGroupPropertiesL( TInt aOpId,
                                     const TDesC& aGroupId,
                                     const CImpsCommonGroupProps& aGroupProps,
                                     const CImpsPrivateGroupProps& aOwnProps,
                                     TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler
         */
        void HandleRejectListL( TInt aOpId,
                                const MDesCArray& aUserList,
                                TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler
         */
        void HandleSubscriptionL( TInt aOpId,
                                  TBool aIsSubscribed,
                                  TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler
         */
        void HandleNewUsersL(
            const TDesC& aGroupId,
            const MDesCArray& aUserList,
            const MDesCArray& aScreenNames,
            TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler
         */
        void HandleLeftUsersL(
            const TDesC& aGroupId,
            const MDesCArray& aUserList,
            const MDesCArray& aScreenNames,
            TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler
         */
        void HandleLeaveL(
            TInt aOpId,
            const TDesC& aGroupId,
            const TDesC& aDescription,
            TImpsCspIdentifier& aCspId );

    private:

        /**
         * Copies array to another array
         * @param aSource is source array
         * @param aDest is destination array
         */
        void CopyArrayL( const MDesCArray& aSource, CDesCArray* aDest );

        /**
        * C++ default constructor.
        * @see CCAGroupWrapper::NewL.
        */
        CCAGroupWrapper( MCASettings& aSettingsAPI,
                         MCAExtendedStoredGroup& aGroup,
                         CCARequestMapper& aRequestMapper,
                         MCAImpsGroupClient* aClient );

        /**
         * Helper method that fills CCARequest and executes request to server
         * and after waits network operation return.
         * Error code of network operation returned.
         * @param aOperationId operation which ImpsGroupClient has given
         *        for operation
         * @param aRequestType executed operation type
         * @param aAdditionalData that is added to CCARequest
         * @return Error code of server response
         */
        TInt ExecuteRequestL( TInt aOperationId,
                              TOperationRequests aRequestType,
                              MDesCArray* aAdditionalData = NULL );

        /**
         * Copy the properties from one set of CImps*GroupProps
         * to another.
         * @param aFromCommonProps Source common properties
         * @param aToCommonProps Target common properties
         * @param aFromPrivateProps Source private properties
         * @param aToPrivateProps Target private properties
         */
        void CopyPropertiesL(
            const CImpsCommonGroupProps* aFromCommonProps,
            CImpsCommonGroupProps* aToCommonProps,
            const CImpsPrivateGroupProps* aFromPrivateProps,
            CImpsPrivateGroupProps* aToPrivateProps );

    private: // Data

        // Pointer to observer of this group
        MCAGroupEventObserver* iObserver;

        // group common properties
        CImpsCommonGroupProps* iCommonProperties; // owned

        // group private properties
        CImpsPrivateGroupProps* iPrivateProperties; // owned

        // API to the settings side
        MCASettings& iSettingsAPI;

        // handled group
        MCAExtendedStoredGroup&	iGroup; // not owned

        CCoeEnv* iConeEnv;  // not owned

        // is the API to the request mapper
        CCARequestMapper& iRequestMapper;   // not owned

        // IMPS engine interface for network operations
        MCAImpsGroupClient* iImpsGroupClient;   // not owned

        // group id
        TInt iGroupIdOrdinal;

        // operation error
        TInt iOperationError;

        // current operation id
        TInt iOperationId;

        // doesn't own. filled upon request completion
        CCARequest* iRequest;
        CDesCArray* iMembers;
        CDesCArray* iScreenNames;
        CDesCArray* iModers;
        CDesCArray* iAdmins;
        CDesCArray* iRejectedList;

        // last IMPS error from CCAEngine
        TInt iLastImpsError;

        // Is new topic shown or not
        TBool iTopicShown;

        // last topic
        HBufC* iOldTopic;
    };

#endif      // CCAGGROUPWRAPPER_H

// End of File
