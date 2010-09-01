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
* Description:  Recorded chats list view
*
*/


#ifndef __CCARECORDEDCHATSLISTVIEW_H__
#define __CCARECORDEDCHATSLISTVIEW_H__

// INCLUDES
#include "CCAView.h"
#include "MCARecipientObserver.h"
// FORWARD DECLARATIONS
class CCARecordedChatsListViewContainer;
class MCARecordedChatProvider;
class CCARecordedChatsArray;

// CLASS DECLARATION

/**
 *  This class is for recorded chats list view
 *
 *  @lib chat.app
 *  @since 2.5
 */
class CCARecordedChatsListView : public CCAView,
            public MCARecipientObserver
    {
    public: // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~CCARecordedChatsListView();

        /**
         * Symbian OS default constructor.
         * @param aAvkonViewResourceId Id for resource used to construct this view
         * @param aViewId View id
         */
        void ConstructL( TInt aAvkonViewResourceId, TUid aViewId );


    private: // Functions from base classes

        /**
         * From CAknView Handles command events. Gets called by framework
         * @param aCommand Command that was received
         */
        void HandleCommandL( TInt aCommand );

        /**
         * From MEikMenuObserver Dynamically initialises a menu pane.
         * The Uikon framework calls this function, if it is implemented in a
         * menu’s observer, immediately before the menu pane is activated.
         * Typically this function should inquire the value of application
         * data, and initialise menu items accordingly. This includes dimming
         * menu items, changing their text, setting the state of checked items
         * and radio buttons and dynamically adding items to a menu.
         * @param aResourceId Resource ID identifying the menu pane to initialise
         * @param aMenuPane The in-memory representation of the menu pane
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
         * From CAknView, Gets called from framework when activating this view
         * @param aPrevViewId Previous view id
         * @param aCustomMessageId Custom message's id
         * @param aCustomMessage Custom message
         */
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
         * From CAknView, Gets called from framework when deactivating this view
         */
        void DoDeactivate();

    public: // New Methods

        /**
         *	Provide recordedchatprovider for other views.
         *	@return Reference to recorded chat provider.
         */
        MCARecordedChatProvider& RecordedChatArrayProvider();

        /**
         *	From MCARecipientObserver Display menu
         */
        void ShowPopUpMenuL();


        /**
         * Shows recipient list (not needed indeed)
         * @param aResourceId Resource id for pop up list
         * @return Positive value if Oked
         */
        TInt ShowRecipientsListL( TInt /*aResourceId*/ );

    private: // Data

        // Owns. Handle to window-owning control container class
        CCARecordedChatsListViewContainer* iContainer;

        // Owns.
        CCARecordedChatsArray* iRecordedChatsArray;

        // Selected index.
        TInt iSelectedIndex;

        // Context sensitive options flag. When on, dim all not needed.
        TBool iContextFlag;

        // Last highlighted chat name.
        TPtrC iSelectedChat;

    };

#endif //__CCARECORDEDCHATSLISTVIEW_H__

// End of File
