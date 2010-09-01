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
* Description:  View class for single list
*
*/


#ifndef CCASINGLELISTVIEW_H
#define CCASINGLELISTVIEW_H

// INCLUDES
#include    "TEnumsPC.h"
#include    "CCAView.h"
#include    "PublicEngineDefinitions.h"
#include    "MCAProcessManager.h"
#include    "MCACommandHelper.h"
#include    "MCAMainViewArrayPC.h"
#include    "MCASingleListViewCB.h"
#include    "MCASyncObserverPC.h"
#include    "MCAListInitObserver.h"
// FORWARD DECLARATIONS
class CCASingleListContainer;
class CAsyncCallBack;

// CLASS DECLARATION
/**
*  View class for friends list
*
*  @lib chatng.app
*  @ since 3.2
*/
class CCASingleListView :	public CCAView,
            public MCACommandHelper,
            public MCASingleListViewCB,
            public MCASyncObserverPC,
            public MCAListInitObserver

    {
    public: // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~CCASingleListView();

        /**
         * Symbian OS constructor
         * @param aAvkonViewResourceId resourceId for view.
         * @param aViewId Id for view.
         */
        void ConstructL( TInt aAvkonViewResourceId, TUid aViewId );

    public: // Functions from base classes

        /**
        * To add "Create new" menu to the options menu when own data is focussed
        * @param aMenuPane: Reference to the menu pane
        */
        void AddCreateNewSubmenuL( CEikMenuPane* aMenuPane );
        /**
        * To add "Own options" menu to the options menu when own data is focussed
        * @param aMenuPane: Reference to the menu pane
        */
        void AddOwnOptionsSubMenuL( CEikMenuPane* aMenuPane ) ;

        /**
        * To add "New Chat" menu to the options menu when open chat is focussed
        * @param aMenuPane: Reference to the menu pane
        */
        void AddNewChatSubmenuL( CEikMenuPane* aMenuPane );

        /**
        * To add "Edit options" submenu to the options menu when
        * contact item is focussed
        * @param aMenuPane: Reference to the menu pane
        * @param aIsContact: True, if the focused item is a contact
        */
        void AddContactEditOptionsSubMenuL( CEikMenuPane* aMenuPane, TBool aIsContact );


        /**
        * To add dynamically create the options menu for the list box items
        * @param aMenuPane: Reference to the menu pane
        */
        void DynCreateMenuPaneL( CEikMenuPane* aMenuPane );

        /**
         * From MEikMenuObserver,
         * Called by framework before constructing menupane
         * @see MEikMenuObserver
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
         * From CAknView Handles command events. Gets called by framework
         * @see CAknView
         */
        void HandleCommandL( TInt aCommand );

    public:
        /**
         * @ see MCASingleListViewCB.h
         */
        TInt GetCurrentItemIndex( );

        /**
         * @ see MCASingleListViewCB.h
         */
        void SetCurrentItemIndexL( TBool aFreshLogin = EFalse );

        /**
         * @ see mcasyncobserverpc.h
         */
        virtual void NotifySyncCompleted();

        /**
         * Callback HandleListInitChange from presence list manager
         * after the list init complete
         */
        void HandleListInitChange( TBool aCompleted );

        /**
         * performs the action related to list initialization
         * Show wait note or removes the same
         * @return ETrue
         */
        TBool InitializeListL();

    private: // From MCACommandHelper

        /**
         * @see MCACommandHelper
         */
        void InvokeCommandL( TInt aCommand );

    private:

        /**
         * From CAknView, Gets called from framework when activating this view
         * @see CAknView
         */
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
         * From CAknView, Gets called from framework when deactivating this view
         * @see CAknView
         */
        void DoDeactivate();

        /**
        *Instantiate iContainer object
        * @param aCustomMessageId  Specifies the message type.
        * @param aCustomMessage  The activation message.
        */
        void CreateContainerL( const TDesC8& aCustomMessage, TUid aCustomMessageId );

    private: // new methods

        /**
         * Constructor for context sensitive menu
         */
        void ConstructContextMenuBarL();

        /**
         * Handles errors
         * @param aError Error id
         */
        void HandleError( TInt aError );

        /**
         * Callback function for uninitializing the view
         * Called by CAsyncCallBack
         * @param aInstance Instance of this class
         * @return Need for recall
         */
        static TInt Uninitialize( TAny *aInstance );

        /**
         * Real implementation of uninitialize
         * Called from Uninitialize
         * @return Need for recall
         */
        TInt DoUninitialize();


        /**
         * updates the title according to current default contact list
         */
        void UpdateTitleL();

        /**
         * Check to see whether list sync is happening in the background
         * complete the pending task and the search
         * @return TBool - ETrue if list sync is still not complete
         */
        TBool CompletePendingTaskForSearchL();

        /**
         * ShowManualUpdateNoteL
         */
        void ShowManualUpdateNoteL();

    private: // Data
        // Owns. Window-owning container for simple controls
        TInt iGlobalSoftNoteId;

        //Container index id used only once
        TInt iContainerIndexId;

        //Current listbox index
        TInt iCurrentIndex;

        // Owns. Context sensitive menu bar
        CEikMenuBar* iContextMenuBar;

        //Owns the reference to the container
        //Do not delete
        CCASingleListContainer* iContainer;

        //Does not own. Do not delete this reference to the CCAAppUi instance
        CCAAppUi* iAppUi;

        //Does not own. Do not delete this reference to the MCAMainViewArrayPC
        MCAMainViewArrayPC* iMainViewArrayPC;

        TCAItemDetails iItemDetails;

        TBool iConversationExits;
        TBool iCreateGrpFromContactList;

        //whether processing wait note is visible/not
        TBool iWaitVisible;

        TBool iFirstManualRefresh;

        // Restrict handling exit event only for one time even if mutil-tapping RSK (Exit) key.
        TBool iAlreadyTrytoExit;

        // Whether we are trying to join a group
        TBool iAlreadyTrytoJoinGroup;
    };

#endif //CCASINGLELISTVIEW_H

// End of File
