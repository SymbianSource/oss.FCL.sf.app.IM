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
* Description:  View for recorded chats.
*
*/


#ifndef CCARECORDEDCHATSVIEW_H
#define CCARECORDEDCHATSVIEW_H

// INCLUDES
#include "CCAView.h"
#include "mcarecipientobserver.h"

// FORWARD DECLARATIONS
class CCARecordedChatsContainer;
class MCARecordedChatProvider;
class CFindItemMenu;
class CItemFinder;

// CLASS DECLARATION

/**
 *  This class is for recorded chats list view
 *
 *  @chatng.exe
 *  @since 3.2
 */
class CCARecordedChatsView : public CCAView,
            public MCARecipientObserver
    {
    public: // Constructors and destructor

        /**
         * Symbian OS default constructor.
         * @param aAvkonViewResourceId Id for resource used
         *                             to construct this view
         * @param aViewId View id
         */
        void ConstructL( TInt aAvkonViewResourceId, TUid aViewId );

        /**
         * Destructor.
         */
        virtual ~CCARecordedChatsView();


    public: // Functions from base classes

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
         * @param aResourceId Resource ID identifying the
         *                    menu pane to initialise
         * @param aMenuPane The in-memory representation of the menu pane
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    public: // New public Methods

        /**
         *	Set provider for recorded chats.
         *	@param aChatProvider Interface to access recorded chat provider.
         */
        void SetRecordedChatProvider( MCARecordedChatProvider& aChatProvider );

    private: // New helper methods

        /**
         *	Hides context sensitive popup menu if active.
         */
        void HideContextMenu();

        /**
         * Resolve currently selected item type and set it to CItemFinder
         */
        void UpdateItemTypeL();

    private:

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
        
        /**
        * From CAknView,ProcessCommandL
        */
        void ProcessCommandL(TInt aCommand);

    private: // From MCARecipientObserver

        /**
         * @see MCARecipientObserver
         */
        TInt ShowRecipientsListL( TInt aResourceId );

        /**
         * @see MCARecipientObserver
         */
        void ShowPopUpMenuL();


    private: // Data

        // Owns. Handle to window-owning control container class
        CCARecordedChatsContainer* iContainer;

        //Does not own. Cannot be set when class created.
        MCARecordedChatProvider* iRecChatArrayProvider;

        //owns Format for navibar date and time.
        HBufC* iFormat;

        //Pop-up menu for context sensitive options. Owns
        CEikMenuBar* iContextMenu;

        // owns. menu pane for find item functionality
        CFindItemMenu* iFindMenu;

        // owns.
        CItemFinder* iItemFinder;

        // not owned, from CCoeEnv
        CCAAppUi* iAppUI;

        // Flag for new options menu
        TBool iNewOptionsMenu;

        //Flag to denote whether file has to be recommited or not
        TBool iDirty;

        // Flag to denote whether object present view has been opend
        TBool iOpenObject;
    };

#endif // CCARECORDEDCHATSVIEW_H

// End of File
