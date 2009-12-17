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
* Description:  Interface for Process Manager
*
*/


#ifndef MCAPROCESSMANAGER_H
#define MCAPROCESSMANAGER_H

//INCLUDES
#include <e32base.h>

//FORWARD DECLARATION
class MCAMainViewArrayPC;
class MCALoginPC;
class MCASettingsPC;
class CCAEngine;
class MCAConversationPC;
class MCAContactEditPC;
class MCABlockingPC;
class MCASearchDataPC;
class MCASearchInterfacePC;
class MCAContactSelectionPC;
class MCAReactiveAuthPC;
class MCARecordedChatsPC;
class MCARecordedChatsArrayPC;
class MCAMessageFlowHandlerPC;
class MCAInvitationPC;
class MCAGroupPC;
class MCAGroupUtilsPC;
class MCAInviteEditorPC;
class MCAGlobalNotificationObserverPC;
class MCAOpenChatsPC;
class MCAGroupPropertiesPC;
class MCAApplicationNGPC;
class MCAArrayItemPC;
//CLASS DECLARATION

class MCAProcessManager
    {

    public:

        /**
         * Get Array interface.
         * @return Pointer to the Array interface.
         */
        virtual MCAMainViewArrayPC* GetArrayInterface() const = 0;

        /**
         * Get Login interface.
         * @return Pointer to the Login interface.
         */

        virtual MCALoginPC* GetLogInInterface() const = 0;

        /**
         * Get Settings interface.
         * @return Pointer to the Settings interface.
         */

        virtual MCASettingsPC* GetSettingsInterface() const = 0;

        /**
         * Get Blockings interface.
         * @return Pointer to the Blockings interface.
         */

        virtual MCABlockingPC* GetBlockingInterface( ) const = 0;

        /**
         * Get Invitations interface.
         * @return Reference to the Invitations interface.
         */
        virtual MCAInvitationPC* GetInvitationsInterface( ) const = 0;

        /**
         * Get Array interface.
         * @return Pointer to the Array interface.
         */

        virtual void FinalizeEngineConstructionL() = 0;

        /**
         * @return Pointer to Engine
         */

        virtual CCAEngine* GetEngine() const = 0;


        /**
         * Get Open Chats Interface.
         * @return Pointer to the MCAOpenChatsPC
         */
        virtual MCAOpenChatsPC* CreateOpenChatsPCL(
            const RPointerArray<MCAArrayItemPC>& aOpenChatsArray ) = 0;


        /**
         * Get Conversation interface.
         * @return Pointer to the Conversation interface.
         */

        virtual MCAConversationPC* GetConversationInterface() const = 0 ;

        /**
         * Get ContactEdit interface.
         * @return Pointer to the ContactEdit interface.
         */

        virtual MCAContactEditPC* GetContactEditInterface() const = 0;


        /**
         * Get the SearchData interface.
         * return the Pointer to the SearchData interface.
         */

        virtual MCASearchDataPC* GetSearchDataInterface() const = 0;

        /**
         * Get the SearchInterface.
         * return the Pointer to the SearchInterface.
         */

        virtual MCASearchInterfacePC* GetSearchInterface() const = 0;

        /**
         * Get the Reactive Auth. Interface
         * return the Pointer to the Reactive Auth. Interface
         */

        virtual MCAReactiveAuthPC* GetReactiveAuthInterface() const = 0;
        /**
         * Get the MCARecordedChatsPC from the PC
         * Return the ponter to the MCARecordedChatsPC
         */
        virtual MCARecordedChatsPC* GetRecordedChatsPC() const = 0;

        /**
         * Get the MCARecordedChatsArrayPC from the PC
         * Return the ponter to the MCARecordedChatsArrayPC
         */
        virtual MCARecordedChatsArrayPC* GetRecordedChatsArrayPC() const = 0;

        /**
         * Get the GroupInterface.
         * return the Pointer to the GroupInterface.
         */

        virtual MCAGroupPC* GetGroupInterface() const = 0;

        virtual MCAGroupUtilsPC* GetGroupUtilsInterface( ) const = 0;

        /**
         * Get the InviteEditorInterface
         * Ownership is passed to the caller.
         * return the Pointer to the InviteEditorInterface.
         */

        virtual MCAInviteEditorPC* CreateInviteEditorPCL() = 0;

        /**
        * Pointer to the global notification process component
        * This registers for all the global events on contacts.contact lists,
        * invites, groups etc. Currently used by app ui
        */
        virtual MCAGlobalNotificationObserverPC* GetGlobalNotificationInterface() const = 0;

        /**
        * Pointer to the group properties process component
        */
        virtual MCAGroupPropertiesPC* GetGroupPropertiesPC() const = 0;

        /**
        * Pointer to the ApplicationNG process component used by appui
        */
        virtual MCAApplicationNGPC* GetApplicationNGPC() const = 0;

        /**
        * Destructor
        */
        virtual ~MCAProcessManager() {};

    };

#endif    //MCAPROCESSMANAGER_H

//End of File