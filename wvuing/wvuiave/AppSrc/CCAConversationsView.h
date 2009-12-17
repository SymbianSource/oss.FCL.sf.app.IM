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
* Description:  View class for conversation view
*
*/



#ifndef CCACONVERSATIONSVIEW_H
#define CCACONVERSATIONSVIEW_H

// INCLUDES
#include "CCAView.h"
#include "MCARecipientObserver.h"
#include "PublicEngineDefinitions.h"
#include "MCAConversationObserverPC.h"
#include "CCAConversationsContainer.h" // MCATitleUpdater
#include "mcablockchecker.h"
#include "MCATabObserver.h"
#include "chatdefinitions.h"
#include <akntabgrp.h>
#include <AknInfoPopupNoteController.h> // for CAKnInfoPopupNote
#include "TEnumsPC.h"

// FORWARD DECLARATIONS
class MCAIndicatorTerminator;
class CFindItemMenu;
class CItemFinder;
class CEikButtonGroupContainer;
class CCAAppUi;
class MCAConversationPC;

// CLASS DECLARATION

/**
 *  This is view class for conversation view
 *
 *  @lib chatng.exe
 *  @since 3.2
 */
class CCAConversationsView :
            public CCAView,
            public MCARecipientObserver,
            public MCAConversationObserverPC,
            public MCATitleUpdater, // For updating title
            public MCAEditorContentStorage, // For storing editor content
            public MCABlockChecker,
            public MCATabObserver
    {

    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CCAConversationsView();

        /**
         * Symbian 2nd phase constructor.
         * @param aAvkonViewResourceId Resource id for view
         * @param aViewId Id of the view
         */
        void ConstructL( TInt aAvkonViewResourceId, TUid aViewId,
                         CCAMessageExtensionsHandler& aMessageHandler );

        /**
         * Destructor.
         */
        virtual ~CCAConversationsView();

    public: // New methods

        /**
         * Handles changes in CBA states
         * @since 3.2
         * @param aResourceId Resource that changes
         */
        void HandleCbaChangeL( TInt aResourceId );

        /**
         * Sets the index of last unread message
         * @since 3.2
         * @param aLastUnreadIndex
         */
        void SetLastUnreadIndex( const TInt aLastUnreadIndex = -1 );


    private: //New methods

        /**
         * Closes conversation
         */
        void CloseConversationL();

        /**
         * This function forwards the message to contact
         * @since 3.2
         */
        void ForwardToContactL();

        /**
         * This function forwards the message to group
         * @since 3.2
         */
        void ForwardToGroupL();



    private: // Functions from base classes

        /**
         * From MEikMenuObserver, Called by framework before
         * constructing menupane
         * @param aResourceId Resource to be constructed
         * @param aMenuPane Pointer to menupane being constructed
         * @see MEikMenuObserver
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );


    private: // From MCAConversationObserverPC

        /**
         * @see MCAConversationObserverPC
         */

        void HandleChatEvent( const TDesC& aWvid,
                              const TDesC& aContactId  ) ;

        /**
         * @see MCAConversationObserverPC
         */
        void HandleContactChange( const TDesC& aContactId );


        /**
         * @see MCAConversationObserverPC
         */
        void HandleMessageError( TInt aError,
                                 const TDesC& aInfo,
                                 TEnumsPC::TContentType aMsgContentType ) ;


    private:

        void DoHandleChatEventL( const TDesC& aWvid,
                                 const TDesC& aContactId );

        void DoHandleMessageErrorL( TInt aError,
                                    const TDesC& aInfo,
                                    TEnumsPC::TContentType aMsgContentType );


    private:

        /**
         * From CAknView, Gets called from framework when activating this view
         * @param aPrevViewId Previous view id
         * @param aCustomMessageId Custom message's id
         * @param aCustomMessage Custom message
         * @see CAknView
         */
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
         * From AknView, deactivates view
         * @see CAknView
         */
        void DoDeactivate();

        /**
         * From MCARecipientObserver
         * Shows recipient list
         * @param aResourceId Resource id for pop up list
         * @return Positive value if Oked
         * @see CAknView
         */
        TInt ShowRecipientsListL( TInt aResourceId );

        /**
         * From MCARecipientObserver
         * Shows customized menu
         * @see CAknView
         */
        void ShowPopUpMenuL( );

        /**
         * From CAknView
         * @param aCommand Command that was received
         * @see CAknView
         */
        void HandleCommandL( TInt aCommand );

        /**
         * Handles foreground event
         * @param aForeground ETrue if foreground else EFalse
         * @see CAknView
         */
        void HandleForegroundEventL( TBool aForeground );

        /**
         * Shows Templates menu
         */
        void ShowTemplatesL();

    public:
        /**
        * @see MCATabObserver
        */
        void SwitchTabL( const TDesC& aWvId, const TInt aTabIndex ) ;

    private:

        /**
         * Resolve currently selected item type and set it to CItemFinder
         */
        void UpdateItemTypeL();

    private: // From MCATitleUpdater

        /**
         * Update title
         */
        void UpdateTitleL();

    private: // From MCAEditorContentStorage

        /**
         * @see MCAEditorContentStorage
         */
        void StoreContent( const HBufC* aContent );

    private: // From MCABlockChecker

        /**
         * @see MCABlockChecker
         */
        void CheckBlockedL();


    private: // Data

        // Previous view's id
        TVwsViewId iPrevViewId;

        // Owns. Container
        CCAConversationsContainer* iContainer;

        // Owns. Popup-menu for add to friends and reply
        CEikMenuBar* iAddAndReplyMenu;

        // Owns. Popup-menu for find item UI
        CEikMenuBar* iFindContextMenu;

        //Owns. Contact id
        HBufC* iContactId;
        //Owns. ListId
        HBufC* iListId;
        //Owns. Title
        HBufC* iTitle;

        //Owns. Sender
        HBufC* iSender;

        // displayid'd sender
        TPtrC iCleanSender;

        TInt iLastUnreadIndex;
        TBool iForeground;

        //Doesn't own. Message handler
        CCAMessageExtensionsHandler* iMessageHandler;

        // owns. menu pane for find item functionality
        CFindItemMenu* iFindMenu;

        // owns.
        CItemFinder* iItemFinder;

        // not owned, used to hide the CBA
        CEikButtonGroupContainer* iCba;

        // not owned, from CCoeEnv
        CCAAppUi* iAppUI;

        // Owns.
        const HBufC* iEditorContent;

        // doesnt own. Ptr to the process component for conversations
        MCAConversationPC* iConversationPC;

        //current tab id
        TInt iTabId;
        //doesnt own
        CCAStatusPaneHandler* iTitlePane;

        // A indicator for object process
        TBool iObjectOpenedSaved;
    };

#endif // CCACONVERSATIONSVIEW_H
