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
* Description:  Declares view for chat list
 *
*/



#ifndef CCACHATLISTVIEW_H
#define CCACHATLISTVIEW_H

// INCLUDES
#include "CCAView.h"

// FORWARD DECLARATIONS
class CCAChatListViewContainer;
class MCAChatListInterface;
class MCAGroupManagerInterface;

// CLASS DECLARATION

/**
 *  This class is for chat list view
 *
 *  @lib chat.app
 *  @since 1.2s
 */
class CCAChatListView : public CCAView
    {
    public: // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~CCAChatListView();

        /**
         * Symbian OS default constructor.
         * @param aAvkonViewResourceId Id for resource
         *                             used to construct this view
         * @param aViewId View id
         */
        void ConstructL( TInt aAvkonViewResourceId, TUid aViewId );


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
         * @param aResourceId Resource ID identifying the menu pane
         *                    to initialise
         * @param aMenuPane The in-memory representation of the menu pane
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

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

    private: // Data
        // Owns. Handle to window-owning control container class
        CCAChatListViewContainer* iContainer;

        //Group interface
        MCAGroupManagerInterface* iGroupInterface;

        // owns. WVID of currently selected group
        HBufC* iSelectedGroup;

        // owns. WVID of group chosen for deletion
        HBufC* iDeleteGroupId;

        // flip flop for skipping the group id storaging in
        // certain places
        TBool iSkipGIDStorage;
    };

#endif //CCACHATLISTVIEW_H

// End of File
