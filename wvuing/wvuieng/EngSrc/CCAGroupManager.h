/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Network operations for chat group handling.
*
*/


#ifndef CCAGROUPMANAGER_H
#define CCAGROUPMANAGER_H

// INCLUDES
#include "MCAImpsGroupClient.h"
#include "MCAGroupManagerInterface.h"
#include "PublicEngineDefinitions.h"
#include <ImpsGroupCli.h>
#include <ImpsClient.h>

#include <e32base.h>

// FORWARD DECLARATIONS
class MCAStoredGroups;
class MCAStoredContact;
class MCAStoredContacts;
class MCASettings;
class CCARequestMapper;
class MCASearchInterface;
class MCAGroupObserver;
class CCAGroupWrapper;
class MCAImpsFactory;
class MCAChatInterface;
class MCAMessageUtils;
class MCAMessagesWriteInterface;

// CLASS DECLARATION

/**
 *  Network operations for chat group handling.
 *
 *  @lib CAEngine.lib
 *  @since 2.1
 */
class CCAGroupManager : public CBase,
            public MImpsErrorHandler2,
            public MImpsGroupHandler2,
            public MCAGroupManagerInterface
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aSearchAPI is the API to the search manager.
         * @param aSettingsAPI is the API to the settings side.
         * @param aRequestMapper is the API to the request mapper.
         *        Used to provide synchronous waiting.
         * @param aIMPSFactory The factory which can create IMPS interfaces
         * when demanded
         * @param aChatInterface is the API to the chat containers.
         */
        static CCAGroupManager* NewL(
            MCASearchInterface& aSearchAPI,
            MCASettings& aSettingsAPI,
            CCARequestMapper& aRequestMapper,
            MCAImpsFactory* aIMPSFactory,
            MCAChatInterface& aChatInterface,
            MCAMessageUtils& aMessageUtils );

        /**
         * Destructor.
         */
        virtual ~CCAGroupManager();

    public: // from MCAGroupManagerInterface

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::NumberOfCreatedGroupsDuringSession
         */
        TInt NumberOfCreatedGroupsDuringSession() const;

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::PopulateCreatedGroupsL
         */
        TInt PopulateCreatedGroupsL( CDesCArray& aGroupList ) const;

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::JoinedGroups
         */
        TInt JoinedGroups();

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::GroupOperationsL
         */
        MCAGroupOperations* GroupOperationsL( const TDesC& aId );

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::SetGroupEventObserverL
         */
        void SetGroupEventObserverL( const TDesC& aId, MCAGroupEventObserver* aObserver );

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::CreateGroupL
         */
        HBufC* CreateGroupL(
            CImpsCommonGroupProps* aProperties,
            CImpsPrivateGroupProps* aPrivProperties,
            const TDesC& aScreenName,
            TBool aJoinGroup,
            TBool aIsWhisperingEnabled,
            TInt& aErrorCode
        );

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::JoinGroupL
         */
        TInt JoinGroupL( const TDesC& aGroupId, const TDesC& aScreenName,
                         TBool aUsers, TBool aIsFavourite, TBool aIsWhisperingEnabled );

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::LeaveJoinedGroupsL
         */
        void LeaveJoinedGroupsL();

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::LeaveGroupL
         */
        TInt LeaveGroupL( const TDesC& aGroupId );

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::GenerateGroupIdLC
         */
        HBufC* GenerateGroupIdLC( const TDesC& aResourcePart );

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::DeleteGroupL
         */
        TInt DeleteGroupL( const TDesC& aGroupId, TBool aDeleteFromNetwork );

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::IsAllowedAccessL
         */
        TBool IsAllowedAccessL( const TDesC& aGroupId,
                                const CDesCArray& aCheckUsers,
                                CDesCArray& aNotInList );

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::IsGroupOpenL
         */
        TBool IsGroupOpenL( const TDesC& aGroupId );

        /**
         * From MCAGroupManagerInterface
         * @see MCAGroupManagerInterface::GroupNameL
         */
        HBufC* GroupNameL( const TDesC& aGroupId );

    public: // new methods

        /**
         * Handles network state changes
         * @param aState network state
         */
        void HandleNetworkStateChangeL( TNetworkState  aState );

        /**
         * Propagates IMPS error code.
         * @param aError last IMPS error.
         */
        void LastImpsError( TInt aError );
	    void CancelGroupOperationL(const TDesC& aGroupId);

    protected: // From MImpsErrorHandler

        /**
         *	From MImpsErrorHandler
         *  @see MImpsErrorHandler::HandleErrorL
         */
        void HandleErrorL(
            TInt aStatus,
            TInt aOpId,
            const TDesC* aDescription,
            const CImpsDetailed* aDetailedRes,
            TImpsCspIdentifier& aCspId );

    protected: // From MImpsGroupHandler

        /**
         * From MImpsGroupHandler, Handles a response of CreateGroupL, DeleteGroupL,
         * AddMembersL,
         * RemoveMembersL, ModifyMembersAccessL, SubscribeL, UnsubscribeL.
         * @see MImpsGroupHandler::HandleCompleteL
         */
        void HandleCompleteL(
            TInt aOpId,
            TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler, Handles JoinGroupL response.
         * @see MImpsGroupHandler::HandleJoinL
         */
        void HandleJoinL(
            TInt          aOpId,
            const MDesCArray&   aUserList,
            const MDesCArray&   aScreenNames,
            const TDesC&        aWelcomeText,
            TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler, Handles GroupMembersL response.
         * @see MImpsGroupHandler::HandleGroupMembersL
         */
        void HandleGroupMembersL(
            TInt        aOpId,
            const MDesCArray& aUserList,
            const MDesCArray& aScreenNames,
            const MDesCArray& aModers,
            const MDesCArray& aAdmins,
            TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler, Handles GroupPropertiesL response and subscribed notification.
         * @see MImpsGroupHandler::HandleGroupPropertiesL
         */
        void HandleGroupPropertiesL(
            TInt                      aOpId,
            const TDesC&                    aGroupId,
            const CImpsCommonGroupProps&    aGroupProps,
            const CImpsPrivateGroupProps&   aOwnProps,
            TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler, Handles SetRejectListL response.
         * @see MImpsGroupHandler::HandleRejectListL
         */
        void HandleRejectListL(
            TInt        aOpId,
            const MDesCArray& aUserList,
            TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler, Handles CheckSubscriptionL response.
         * MImpsGroupHandler::HandleSubscriptionL
         */
        void HandleSubscriptionL(
            TInt aOpId,
            TBool aIsSubscribed,
            TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler, Handles group change notification about new users.
         * @see MImpsGroupHandler::HandleNewUsersL
         */
        void HandleNewUsersL(
            const TDesC&        aGroupId,
            const MDesCArray&   aUserList,
            const MDesCArray&   aScreenNames,
            TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler, Handles group change notification about users left.
         * @see MImpsGroupHandler::HandleLeftUsersL
         */
        void HandleLeftUsersL(
            const TDesC&        aGroupId,
            const MDesCArray&   aUserList,
            const MDesCArray&   aScreenNames,
            TImpsCspIdentifier& aCspId );

        /**
         * From MImpsGroupHandler, Handles both LeaveGroupL response and a service
         * initiated group leave situation.
         * @see MImpsGroupHandler::HandleLeaveL
         */
        void HandleLeaveL(
            TInt   aOpId,
            const TDesC& aGroupId,
            const TDesC& aDescription,
            TImpsCspIdentifier& aCspId );

    private: // new methods

        /**
         * Find the group wrapper class based on the operation id.
         * @param aOpId The operation id
         * @return The group wrapper if found, NULL otherwise
         */
        CCAGroupWrapper* FindGroupWrapper( TInt aOpId );

        /**
         * Overloaded version.
         * @param aGroupId The group id
         * @see FindGroupWrapper
         */
        CCAGroupWrapper* FindGroupWrapper( const TDesC& aGroupId );

        /**
         * Get the properties for the given group.
         * Replaces the given parameters with new properties.
         * Used internally.
         * @param aGroupId The group id
         * @param aCommonProps The group common properties
         * @param aPrivProps The group private properties
         */
        void GetPropertiesL( const TDesC& aGroupId,
                             CImpsCommonGroupProps*& aCommonProps,
                             CImpsPrivateGroupProps*& aPrivProps );

        /**
         * Delete a group wrapper based on the group id.
         * Doesn't do anything if no such group id could be found.
         * @param aGroupId The group id
         */
        void DeleteGroupWrapper( const TDesC& aGroupId );

        /**
         * Create a new group wrapper and add it to the internal list.
         * Leaves if the group could not be found.
         * @param aGroupId The group id
         * @return The created wrapper
         */
        CCAGroupWrapper* CreateWrapperL( const TDesC& aGroupId );

        /**
         * Cleanup things after leaving group
         * @param aWrapper The group wrapper
         */
        void CleanupLeaveGroupL( const TDesC& aGroupId );

        /**
         * Force the setting of private allowed flag
         * @param aWrapper Group wrapper
         * @param aPrivProps Private properties
         */
        TInt SetPrivateAllowedL( CCAGroupWrapper& aWrapper,
                                 CImpsPrivateGroupProps& aPrivProps );

        /**
         * Remove the chat data for the given group id.
         * If the chat data does not exist, it is not removed,
         * and there will be no error.
         * @param aGroupId The group id
         */
        void RemoveChatDataL( const TDesC& aGroupId );


        /**
        * Writes welcome message to messageinterface according to parameters.
        * If custom welcome message don't exist, the default one with
        * screen name is shown. Topic string is shown only if it is defined.
        * @param aWelcomeMessage  Custom welcome message
        * @param aGroup           Group name or id.
        * @param aTopic           Topic
        * @param aMessages        Iterface for writing constructed messages
        * @return None
        */
        void WriteWelcomeMessageL( const TDesC& aWelcomeMessage,
                                   const TDesC& aGroup,
                                   const TDesC& aTopic,
                                   MCAMessagesWriteInterface& aMessages );

    private:

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

        /**
         * C++ default constructor.
         * @see CCAGroupManager::NewL.
         */
        CCAGroupManager(
            MCASearchInterface& aSearchAPI,
            MCASettings& aSettingsAPI,
            CCARequestMapper& aRequestMapper,
            MCAImpsFactory* aIMPSFactory,
            MCAChatInterface& aChatInterface,
            MCAMessageUtils& aMessageUtils );

    private: // Data

        CCARequestMapper& iRequestMapper;   // not owned
        MCASettings& iSettingsAPI;          // not owned
        MCASearchInterface& iSearchAPI;     // not owned

        MCAStoredGroups* iStoredGroups;     // not owned
        MCAStoredContacts* iStoredContacts; // not owned

        // not owned
        MCAImpsFactory* iImpsFactory;

        MCAChatInterface& iChatInterface; // not owned
        MCAMessageUtils& iMessageUtils;     // not owned

        RPointerArray< CCAGroupWrapper > iGroupWrappers; // owned

        // These are used temporarily to store group properties
        // during CreateGroupL method before passing ownership
        // to group wrapper. Set at the start of CreateGroupL
        // method (after deleting previous values just in case that
        // previous CreateGroupL has failed) and set to NULL after
        //  owner ship of properties is transferred to group wrapper.
        //If something fails (Leaves ) between these two phases, then variables
        // are not NULL and should be deleted in destructor or in next
        // time calling CreateGroupL.
        CImpsCommonGroupProps* iProperties;
        CImpsPrivateGroupProps* iPrivProperties;

        TInt iNumCreatedGroups;
        TInt iGroupIdOrdinal;
        TInt iNumJoinedGroups;

        // last IMPS error from CCAEngine
        TInt iLastImpsError;

        // Operation id of group props request when joining
        // to group
        TInt iPropsOpIdWhileJoining;
    };

#endif      // CCAGROUPMANAGER_H

// End of File
