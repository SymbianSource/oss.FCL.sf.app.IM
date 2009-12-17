/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Process Manager to get process interfaces e.g. login,settings
*
*/


#ifndef CCAPROCESSMANAGER_H
#define CCAPROCESSMANAGER_H


//INCLUDES
#include <e32base.h>

#include "MCAChatObserver.h"
#include "MCAStoredContactsObserver.h"
#include "mcamessageerrorobserver.h"

#include <MWVSettingsCASettingsng.h>
#include <MWVSettingsObserverng.h>

#include "MCAInviteObserver.h"
#include "MCAChatObserver.h"
#include "MCAProcessManager.h"

// FORWARD DECLARATIONS
class CCALoginPC;
class CCAEngine;
class MCASettingsPC;
class MCALoginPC;
class CCASettingsPC;
class MCAProcessManager;
class MCAMainViewArrayPC;
class MCAConversationPC;
class CCAConversationPC;
class CCAContactEditPC;
class MCABlockingPC;
class CCASearchDataPC;
class CCASearchInterfacePC;
class CCAInvitationPC;
class MCASelectionPC;
class MCAConversationPC;
class MCASearchInterfacePC;
class MCASearchDataPC;
class MCAMessageFlowHandlerPC;
class CCARecordedChatsPC;
class MCARecordedChatsPC;
class MCARecordedChatsArrayPC;
class CCARecordedChatsArrayPC;
class MCAReactiveAuthPC;
class MCAGroupPC;
class MCAGroupUtilsPC;
class MCAInviteEditorPC;
class MCAOpenChatsPC;
class MCAGroupPropertiesPC;
class CCAGroupPropertiesPC;

class CCAIMUtilsPC;
class MCAGlobalNotificationObserverPC;
class CCAGlobalNotificationObserverPC;

class MCAApplicationNGPC;
class CCAApplicationNGPC;

class CCAProcessManager: public CBase,
            public MCAStoredContactsObserver,
            public MCAMessageErrorObserver,
            public MCAChatObserver,
            public MCAInviteObserver,
            public MCAProcessManager

    {
    public:

        /**
        * Two-phased constructor.
        */

        IMPORT_C static CCAProcessManager* NewL();

        /**
        * Destructor.
        */

        IMPORT_C virtual ~CCAProcessManager();

        /**
        * Default Constructor
        */

        CCAProcessManager();

    private:
        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    public:  // From MCAProcessManager


        /**
        * @see MCAProcessManager
        */
        IMPORT_C CCAEngine* GetEngine() const;

        /**
        * @see MCAProcessManager
        */
        IMPORT_C MCAOpenChatsPC* CreateOpenChatsPCL(
            const RPointerArray<MCAArrayItemPC>& aOpenChatsArray );

        /**
        * @see MCAProcessManager
        */

        IMPORT_C MCALoginPC* GetLogInInterface() const;

        /**
        * @see MCAProcessManager
        */

        IMPORT_C MCASettingsPC* GetSettingsInterface() const;

        /**
         * @see MCAProcessManager
         */
        EXPORT_C MCAInvitationPC* GetInvitationsInterface( ) const;
        /**
        * @see MCAProcessManager
        */

        IMPORT_C void FinalizeEngineConstructionL();

        /**
        * @see MCAProcessManager
        */

        IMPORT_C virtual MCAMainViewArrayPC* GetArrayInterface() const;

        /**
        * @see MCAProcessManager
        */

        IMPORT_C MCAConversationPC* GetConversationInterface() const;

        /**
         * @see MCAProcessManager
         */
        IMPORT_C MCAContactEditPC* GetContactEditInterface() const;


        /**
         * Get the MCABlockingPC interface.
         * return the Pointer to the BlockingInterface;
         */
        IMPORT_C MCABlockingPC* GetBlockingInterface() const;

        /**
         * @see MCAProcessManager
         */
        IMPORT_C MCASearchDataPC* GetSearchDataInterface() const;

        /**
        * @see MCAProcessManager
        */
        IMPORT_C MCASearchInterfacePC* GetSearchInterface() const;

        /**
        * @see MCAProcessManager
        */
        IMPORT_C MCAGroupPC* GetGroupInterface( ) const;
        /**
        * @see MCAProcessManager
        */

        IMPORT_C MCAGroupUtilsPC* GetGroupUtilsInterface( ) const;

        /**
        * @see MCAProcessManager
        */
        IMPORT_C MCAReactiveAuthPC* GetReactiveAuthInterface() const;

        /**
          * @see MCAProcessManager
          */
        EXPORT_C MCARecordedChatsPC* GetRecordedChatsPC() const;

        /**
        * @see MCAProcessManager
        */

        EXPORT_C MCARecordedChatsArrayPC* GetRecordedChatsArrayPC() const;

        /**
        * @see MCAProcessManager
        */
        EXPORT_C MCAInviteEditorPC* CreateInviteEditorPCL();

        /**
        * @see MCAProcessManager
        */
        EXPORT_C MCAGlobalNotificationObserverPC* GetGlobalNotificationInterface() const;

        /**
        * @see MCAProcessManager
        */
        EXPORT_C MCAGroupPropertiesPC* GetGroupPropertiesPC() const;

        /**
        * see MCAProcessManager
        */
        EXPORT_C MCAApplicationNGPC* GetApplicationNGPC() const;

    private:  // From MCAChatObserver

        /**
         * @see MCAChatObserver
         */

        void HandleChatEvent( TChatEventType aEvent, MCAMessage* aMessage );

    private: // From MCAStoredContactsObserver

        /**
         * @see MCAStoredContactsObserver
         */
        void HandleContactDelete( const TDesC& aContactId );

        /**
         * @see MCAStoredContactsObserver
         */
        void HandleAddition( MCAContactList& aList, MCAStoredContact& aContact );

        /**
         * @see MCAStoredContactsObserver
         */
        void HandleChange(
            MCAContactList* aList,
            MCAStoredContact* aContact,
            TStorageManagerGlobals::TCAObserverEventType aEventType,
            TBool aUserIdChanged );


        void InitializeEngineObserversL();

    private: // From MCAInviteObserver
        /**
         * @see MCAInviteObserver
         */
        void HandleInvitationEventL( TInviteEventType aEvent, const MCAInvitation* aInvitation );

        /**
         * From MCAInviteObserver, Gets called when we received response to our invitation
         *
         * @param aAcceptance Accepted or rejected
         * @param aUserId User Id of the response sender
         * @param aGroupName group name
         * @param aResponse response text
         */
        void HandleInvitationResponse( TBool aAcceptance,
                                       const TDesC& aUserId,
                                       const TDesC& aGroupName,
                                       const TDesC& aResponse );

    private: // From MCAMessageErrorObserver

        /**
         * @see MCAMessageErrorObserver
         */
        void HandleMessageError( TInt aError, MCAMessage* aMessage );


    private: //data
        /**
         * Owns the instance of the CCALoginPC for use by the UI
         */
        CCALoginPC*      iCCALogIn;

        /**
        * Owns the instance of the CCAEngine for use by the other process components
        */
        CCAEngine*       iEngine;

        /**
        * Owns the instance of the CCASettingsPC for use by the UI
        */
        CCASettingsPC*   iSettingsPC;

        /**
        * Owns the instance of the MCAMainViewArrayPC for use by the UI
        */
        MCAMainViewArrayPC* iMainViewItemArray;


        /**
        * Owns the instance of the CCAConversationPC for use by the UI
        */
        CCAConversationPC*   iConversationPC;

        /**
        * Owns the instance of the CCAContactEditPC for use by the UI
        */
        CCAContactEditPC *iCCAContactEditPC;

        /**
        * Owns the instance of the MCABlockingPC for use by the UI
        */
        MCABlockingPC* iCCABlockingPC;

        /**
        * Owns the instance of the CCASearchDataPC for use by the UI
        */
        CCASearchDataPC* iDataInterfacePC;

        /**
        * Owns the instance of the CCASearchInterfacePC for use by the UI
        */
        CCASearchInterfacePC* iSearchInterfacePC;

        /**
        * Owns the instance of the CCARecordedChatsPC for use by the UI
        */
        CCARecordedChatsPC* iRecordedChatsPC;

        /**
        * Owns the instance of the CCARecordedChatsArrayPC for use by the UI
        */
        CCARecordedChatsArrayPC* iRecordedChatsArrayPC;

        /**
        * Owns the instance of the MCAReactiveAuthPC for use by the UI
        */
        MCAReactiveAuthPC* iReactiveAuthPC;


        /**
        * Owns the instance of the CCAInvitationPC for use by the UI
        */

        CCAInvitationPC* iInvitationsPC;

        /**
        * Owns the instance of the MCAGroupPC for use by the UI
        */
        MCAGroupPC* iGroupPC;


        /**
        * Owns the instance of the MCAGroupUtilsPC for use by the UI
        */
        MCAGroupUtilsPC* iGroupUtilsPC;

        /**
        * Owns the instance of the MCAGlobalNotificationObserverPC for use by the UI
        */
        CCAGlobalNotificationObserverPC* iGlobalNotificationObserverPC;


        /**
        * Owns the instance of the MCAOpenChatsPC for use by the UI
        */
        MCAOpenChatsPC* iOpenChatsPC;

        CCAGroupPropertiesPC* iGroupPropertiesPC;

        CCAApplicationNGPC* iApplicationNGPC;
    };

#endif //CCAPROCESSMANAGER_H   

//    End of File