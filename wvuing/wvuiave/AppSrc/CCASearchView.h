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
* Description:  View class for search view
*
*/



#ifndef CCASEARCHVIEW_H
#define CCASEARCHVIEW_H


//  INCLUDES
#include "CCAView.h"
#include "MCARecipientObserver.h"
#include "MCASearchBackgroundObserverPC.h"


// FORWARD DECLARATIONS
class CCASearchViewContainer;
class CEikMenuBar;
class MCASearchInterfacePC;


// CLASS DECLARATION

/**
*  View class for search view
*
*  @lib chatng.app
*  @since 3.2
*/
class CCASearchView : public CCAView,
            public MCARecipientObserver,
            public MCASearchBackgroundObserverPC
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CCASearchView();

        /*
        * destructor
        */
        virtual ~CCASearchView();

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
         * @param aResourceId Resource ID identifying the menu pane to
         *                    initialise
         * @param aMenuPane The in-memory representation of the menu pane
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
         * Shows popup menu instead of application menu
         */
        void ShowPopUpMenuL();

        /**
         * @see MCASearchBackgroundObserverPC
         */
        virtual void HandleBackgroundEventPC(
            TEnumsPC::TCABackgroundTasks aEventSource,
            TEnumsPC::TCABackgroundStatus aStatus,
            TInt aSubTaskNumber,
            TInt aLeaveCode
        );

    public:	// New methods

        /**
         * Shows recipient list (not needed indeed)
         * @param aResourceId Resource id for pop up list
         * @return Positive value if Oked
         */
        TInt ShowRecipientsListL( TInt /*aResourceId*/ );


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

    private:    // Data

        // Owns
        CCASearchViewContainer* iContainer;

        // previous view id
        TVwsViewId iPrevViewId;

        // Owns. Popup-menu
        CEikMenuBar* iPopUpMenu;


        // group synchronization done or not
        TBool iGroupSyncDone;

        //Pointer to SearchInterfacePC,doesnt own
        MCASearchInterfacePC* iSearchInterfacePC;
    };

#endif      // CCASEARCHVIEW_H   

// End of File
