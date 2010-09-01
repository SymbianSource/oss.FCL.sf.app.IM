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
* Description:  Interface for settings
*
*/


#ifndef MCASETTINGS_H
#define MCASETTINGS_H

// INCLUDES
#include "PublicEngineDefinitions.h"

#include <e32std.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class MCASettingsObserver;
class CIMPSSAPSettings;


// Default nick name
_LIT( KCADefaultNick, "IMDefaultNick" );
// Table of userId specific Aliases
_LIT( KCAOwnAlias, "IMOwnAlias" );
// Authorize IM presence
_LIT( KCAAuthIMPr, "IMAuthIMPr" );
// Receive instant messages
_LIT( KCAReceiveIMessages, "IMReceiveIMessages" );
// Receive invitations
_LIT( KCAReceiveInvitations, "IMReceiveInvitations" );
// Message flow
_LIT( KCAMsgFlow, "IMMsgFlow" );
// Auto update
_LIT( KCAAutoUpd, "IMAutoUpd" );
// Auto update
_LIT( KCAFriendsListOrdering, "IMFriendsListOrdering" );
// Order alphabetically
_LIT( KCAOrderAlpha, "IMOrderAlpha" );

// Show first login done
_LIT( KCAFirstLoginDone, "IMFistLoginDone" );
// Default screen name in use
_LIT( KCADefaultScreenNameInUse, "IMDefaultScreenNameInUse" );
// Automatic presence update
_LIT( KCAAutomaticPresenceUpdate, "IMAutomaticPresenceUpdate" );
// Update selected contacts
_LIT( KCAUpdateSelectedContacts, "IMUpdateSelectedContacts" );
// Show history
_LIT( KCAShowHistory, "IMUpdateSelectedContacts" );
// Show offline friends
_LIT( KCAShowOffline, "IMShowOffline" );
// Show timestamps
_LIT( KCAShowTimeStamps, "IMShowTimeStamps" );

//

// CLASS DECLARATION

/**
*  Interface for internal settings handler.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCASettings
    {
    public: // Setting enums. Don't change the base enums!

        // enumerations for setting items' values
        // (authorize presence, receive imessages, receive invitations)
        enum TCASettingValues
            {
            EAll = 0,
            EFriends,
            ENobody
            };

        // enumerations for friends list ordering setting items' values
        enum TCAFriendsListOrdering
            {
            EAlphabetical = 0,
            EPresence
            };

        // boolean setting items
        enum TCASettingFlags
            {
            EFlagsBase                  = 1,

            EFirstLoginDone,
            EDefaultScreenNameInUse,
            EAutomaticPresenceUpdate,
            EUpdateSelectedContacts,
            EShowHistory,
            EShowOffline,
            EShowTimeStamps
            };

        // integer setting items
        enum TCASettingIntegers
            {
            EIntegersBase               = 1000,

            EAuthorizeIMPresence,
            EReceiveIMessages,
            EReceiveInvitations,
            EMessageFlowSettingLevel,
            EFriendsListOrdering
            };

        // string setting items
        enum TCASettingStrings // For strings add max length to enum value
            {
            EStringsLengthShift = 8,
            EStringsEnumMask = 0xff,
            EStringsLengthMask = 0xffff00,
            EStringsBase = 0x1000000,
            EDefaultScreenNameBase = 0x1000001,
            EOwnWVUserIDBase = 0x1000002,
            EOwnAliasBase = 0x1000003,
            EServiceAddressBase = 0x1000004,
            EStatusMsgOnlineBase = 0x1000005,
            EStatusMsgAwayBase = 0x1000006,
            EStatusMsgBusyBase = 0x1000007,

            EDefaultScreenName = EDefaultScreenNameBase
            + ( KWScreenNameMaxLength << EStringsLengthShift ),

            EOwnWVUserID = EOwnWVUserIDBase
            + ( KWVUserIDMaxLength << EStringsLengthShift ),

            EOwnAlias = EOwnAliasBase
            + ( KWScreenNameMaxLength << EStringsLengthShift ),

            EServiceAddress = EServiceAddressBase +
            ( KWVUserIDMaxLength << EStringsLengthShift ),

            EStatusMsgOnline = EStatusMsgOnlineBase +
            ( RProperty::KMaxPropertySize << EStringsLengthShift ),

            EStatusMsgAway = EStatusMsgAwayBase +
            ( RProperty::KMaxPropertySize << EStringsLengthShift ),
            EStatusMsgBusy = EStatusMsgBusyBase +
            ( RProperty::KMaxPropertySize << EStringsLengthShift )
            };

    public:

        /**
        * Adds settings observer to the observer list.
        * @param aObserver specifies the observer.
        */
        virtual void AddObserverL( const MCASettingsObserver* aObserver ) = 0;

        /**
        * Removes the observer from the list.
        * @param aObserver specifies the observer that gets removed.
        */
        virtual void RemoveObserver(
            const MCASettingsObserver* aObserver ) = 0;

        /**
        * Retrieves value.
        * @param aSetting specifies the wanted setting.
        * @return Boolean value.
        */
        virtual TBool Value( TCASettingFlags aSetting,
                             CIMPSSAPSettings* aSap = NULL ) = 0;

        /**
        * Retrieves value.
        * @param aSetting specifies the wanted setting.
        * @return Integer value.
        */
        virtual TInt Value( TCASettingIntegers aSetting,
                            CIMPSSAPSettings* aSap = NULL ) = 0;

        /**
        * Retrieves value.
        * Ownership is transferred to caller!!!
        * @param aSetting specifies the wanted setting.
        * @return String value.
        */
        virtual HBufC* ValueL( TCASettingStrings aSetting,
                               CIMPSSAPSettings* aSap = NULL ) = 0;

        /**
        * Sets a value.
        * @param aSetting specifies the wanted setting.
        * @param aValue specifies the value we want to store.
        * @param aIgnoreRollback Don't perform rollback operation on these
        *                        errors
        *        NULL if rollback is wanted in every error situation.
        */
        virtual void SetValueL( TCASettingFlags aSetting, TBool aValue,
                                RArray<TInt>* aIgnoreRollback = NULL,
                                CIMPSSAPSettings* aSap = NULL ) = 0;

        /**
        * Sets a value.
        * @param aSetting specifies the wanted setting.
        * @param aValue specifies the value we want to store.
        * @param aIgnoreRollback Don't perform rollback operation on these
        *                        errors
        *        NULL if rollback is wanted in every error situation.
        */
        virtual void SetValueL( TCASettingIntegers aSetting, TInt aValue,
                                RArray<TInt>* aIgnoreRollback = NULL,
                                CIMPSSAPSettings* aSap = NULL ) = 0;

        /**
        * Sets a value.
        * @param aSetting specifies the wanted setting.
        * @param aValue specifies the value we want to store.
        * @param aIgnoreRollback Don't perform rollback operation on
        *                        these errors
        *        NULL if rollback is wanted in every error situation.
        */
        virtual void SetValueL( TCASettingStrings aSetting, const TDesC& aValue,
                                RArray<TInt>* aIgnoreRollback = NULL,
                                CIMPSSAPSettings* aSap = NULL  ) = 0;

        /**
        * Flushes data from memory to persistent storage
        */
        virtual void FlushData() = 0;

        /**
        * Sets the default sap
        */
        virtual void SetDefaultSapL( CIMPSSAPSettings* aSap ) = 0;

        /**
         * Retrieves the own alias value.
         * Ownership is transferred!
         * @return current alias value
         */
        virtual HBufC* OwnAliasL() = 0;

        /**
         * Sets the new alias attribute.
         * @param aAlias new alias attribute.
         */
        virtual void SetOwnAliasL( const TDesC& aAlias ) = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCASettings() {};
    };

#endif      // MCASETTINGS_H

// End of File
