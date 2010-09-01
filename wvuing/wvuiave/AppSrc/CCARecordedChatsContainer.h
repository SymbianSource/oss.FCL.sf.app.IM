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
* Description:  Container for recorded chat view
*
*/



#ifndef __CCARECORDEDCHATSCONTAINER_H__
#define __CCARECORDEDCHATSCONTAINER_H__

// INCLUDES
#include "MCALayoutChangeObserver.h"
#include "mcatapeventobserver.h"
#include "MCAMsgAddedToRichTxtCtrlObserver.h"
#include <coecobs.h>

#include <aknview.h>

// FORWARD DECLARATIONS
class CCAChatViewControl;
class MCAViewSwitcher;
class MCARecordedChatProvider;
class MCASettingsPC;
class CGulIcon;
class MCASkinVariant;
class CCAAppUi;
class MCAConversationMessage;
class MCARecipientObserver;
class MCARecordedChatsPC;

// CLASS DECLARATION

/**
 *  container class for chat view
 *
 *  @lib chatng.app
 *  @since 3.2
 */
class CCARecordedChatsContainer : public CCoeControl,
            public MCALayoutChangeObserver,
            public MCATapEventObserver,
            public MCAMsgAddedToRichTxtCtrlObserver,
            public MCoeControlObserver
    {
    private: // Enumerations

        /**
        * Context menubar event source
        */
        enum TContextLaunchEvent
            {
            EKeyActivation,
            ESingleClick,
            EDoubleClick
            };


    public: // Constructors and destructor

        /**
         * Symbian default constructor
         * @param aRect Frame rectangle for container.
         * @param aViewSwitcher Handle to view-switcher
         * @param aRecordedChatArrayProvider: Provides the list of saved conversations
         * This is the interface from which CCARecordedChatsArray  derives.
         * @param aContextMenu Context sensitive menu.
         * @param aObjectProvider Context access mechanism.
         * @param aSettingsPC: reference to the settings process component
         * @param aAppUi: Pointer to the AppUI instance
         * @param aRecordedChatsPC: The reference to recorded chats process component
         */
        static CCARecordedChatsContainer* NewL(
            const TRect& aRect,
            MCAViewSwitcher& aViewSwitcher,
            MCARecordedChatProvider& aRecordedChatArrayProvider,
            MCARecipientObserver& aContextMenu,
            MObjectProvider& aObjectProvider,
            MCASettingsPC& aSettingsPC,
            CCAAppUi* aAppUi,
            MCARecordedChatsPC& aRecordedChatsPC );

        /**
         * Destructor.
         */
        virtual ~CCARecordedChatsContainer();



    public: // New functions

        /**
        * This checks if scroll mode is on or off
        * @since 3.2
        * @return ETrue when scrolling is stopped else return EFalse.
        */
        TBool IsStopped() const;

        /**
         * Returns buffer containing the currently selected text
         * (or empty buffer if nothing selected)
         */
        HBufC* SelectedItemL();

        /**
         * @return Type of selected item
         */
        TInt SelectedItemType();

        /**
         * @return Forward to contact-menu visible (ETrue)
         * @since 3.2
         */
        TBool FwdContactVisible();

        /**
         * @return Forward to group-menu visible (ETrue)
         * @since 3.2
         */
        TBool FwdGroupVisible();

        /**
         * Check if selected item is object
         * @param aIsOpenable method sets this ETrue if selected item
         *                    is openable object else set to EFalse
         * @return ETrue if selected item is object
         * @since 3.2
         */
        TBool IsObjectSelected( TBool& aIsOpenable, TBool& aIsSaved );

        /**
         * @return Selected message or NULL if nothing selected
         * @since 3.2
         */
        MCAConversationMessage* SelectedMessage();

        /**
         * This function forwards the message to group
         * @since 3.2
         */
        void ForwardToGroupL();

        /**
         * This function forwards the message to contact
         * @since 3.2
         */
        void ForwardToContactL();

        /**
         * Sets item highlighting on or off (automatic find)
         * @param aHihghlight ETrue to set finder on
         * @since 3.2
         */
        void SetItemHighlightL( TBool aHighlight );

        /**
         * Free memory so that viewswitch is made possible.
         * In OOM situation the framework cannot create a new container
         * nor change the view if the current container hogs all the memory.
         * This method should however make sure that the current view is still
         * usable after this call, because viewswitch might not succeed.
         */
        void FreeMemoryForViewSwitch();

    private: // Functions from base classes

        /**
         * @see MCALayoutChangeObserver
         */
        void LayoutChangedL( TInt aType );

        /**
         * @see CCoeControl
         */
        void SizeChanged();

        /**
         * From CCoeControl, Gets called when drawing is needed (by framework)
         * @param aRect Rect that needs to be drawed
         */
        void Draw( const TRect& aRect ) const;

        /**
         * @see CCoeControl
         */
        TInt CountComponentControls() const;

        /**
         * @see CCoeControl
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * @see CCoeControl
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent,
                                     TEventCode aType );

        /**
         * @see CCoeControl
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
         * @see MCATapEventObserver
         */
        void HandleTapEventL( TTapEvent aEvent, TUint aControlId );

    private: //Private functions

        /**
         * Sets the default title of this view
         */
        void SetDefaultTitleL();

        /**
         * To load the icons
         */
        void LoadBitmapsL();

        /**
         * Updates CBA.
         * @since S60 v3.2
         */
        void UpdateCbaL();

        /**
         * Opens context sensitive menubar
         * @since S60 v3.2
         */
        void ShowContextMenuL( TContextLaunchEvent aLaunchEvent );

    private: //from MCAMsgAddedToRichTxtCtrlObserver

        /**
         * @see MCAMsgAddedToRichTxtCtrlObserver
         */
        void HandleMessageAddedL( const TInt aMessageCount );

    private: // From MCoeControlObserver

        /**
         * @see MCoeControlObserver
         */
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

    private: //Private constructor

        /**
         * Constructor
         * @param aChatProvider: Provides the list of saved conversations
         * This is the interface from which CCARecordedChatsArray  derives.
         * @param aViewSwitcher Handle to view-switcher
         * @param aContextMenu Context sensitive menu.
         * @param aSettingsPC: reference to the settings process component
         * @param aAppUi: Pointer to the AppUI instance
         * @param aRecordedChatsPC: The reference to recorded chats process component
         */
        CCARecordedChatsContainer(  MCARecordedChatProvider& aChatProvider,
                                    MCAViewSwitcher& aViewSwitcher,
                                    MCARecipientObserver& aContextMenu,
                                    MCASettingsPC& aSettingsPC,
                                    CCAAppUi* aAppUi,
                                    MCARecordedChatsPC& aRecordedChatsPC );

        /**
         * Symbian OS default constructor.
         * @param aRect Frame rectangle for container.
         * @param aObjectProvider Context access mechanism.
         */
        void ConstructL( const TRect& aRect, MObjectProvider& aObjectProvider );


    private: //data

        // Doesn't own. Handle to recorded chat provider
        MCARecordedChatProvider& iRecordedChatArrayProvider;

        //Doesn't own. Handle to recorded chats process component
        MCARecordedChatsPC& iRecordedChatsPC;

        // Owns. Chat view control
        CCAChatViewControl* iChatView;

        // Doesn't own. Handle to view-switcher
        MCAViewSwitcher& iViewSwitcher;

        // Doesn't own. Context sensitive menu options
        MCARecipientObserver& iRecipientObserver;

        //Owns. "From me" icon
        CGulIcon* iFromMe;

        //Owns. "To me" icon
        CGulIcon* iToMe;

        //Owns. "Unsupported" icon
        CGulIcon* iUnsupported;

        //Owns. "Corrupted" icon
        CGulIcon* iCorrupted;

        //Doesn't own. Skin variant interface.
        MCASkinVariant* iSkinVar;

        //Doesn't own. AppUI.
        CCAAppUi* iAppUI;

        /* See IMVariant.hrh.
         * Flag depends on EIMFeatSendkeyInMsgList.
         */
        TBool iSendkeyInMsgList;

        /*
        ** Does not own
        * reference to MCASettingsPC
        */
        MCASettingsPC& iSettingsPC;
    };

#endif // __CCARECORDEDCHATSCONTAINER_H__

// End of File
