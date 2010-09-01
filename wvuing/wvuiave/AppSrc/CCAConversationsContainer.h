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
* Description:  This is container class for conversation view
*
*/



#ifndef CCACONVERSATIONSCONTAINER_H
#define CCACONVERSATIONSCONTAINER_H

//  INCLUDES
#include "MCALayoutChangeObserver.h"
#include "mcatapeventobserver.h"
#include "CCAMessageEditor.h" // MCAMessageEditorObserver

#include <aknview.h>
#include <eikedwob.h>
#include <bamdesca.h> //For the MDesCArray
#include <badesca.h> //For the CDesCArray
#include <eikbtgpc.h>
#include <eikrted.h>
#include <aknutils.h>
#include <aknEditStateIndicator.h>
#include <AknIndicatorContainer.h>
#include "ChatDefinitions.h"
#include "MCALoginRefreshObserverPC.h"
#include "MCAMsgAddedToRichTxtCtrlObserver.h"

// FORWARD DECLARATIONS
class MCAViewSwitcher;
class MCARecipientObserver;
class MCASkinVariant;
class CCAChatListBox;
class CCAMessageEditor;
class CCAEngine;
class CCABlockingUI;
class CCAAppUi;
class CCAMessageExtensionsHandler;
class CCAChatViewControl;
class MCAMessagesReadInterface;
class MCAMessagesWriteInterface;
class CGulIcon;
class MCAConversationMessage;
class MCAConversationPC;
class CCAEditIndicator;

/**
 * Helper class for updating title when needed.
 */
class MCATitleUpdater
    {
    public:

        /**
         * Update title.
         */
        virtual void UpdateTitleL() = 0;

    protected:

        /**
         * Destructor for protection.
         */
        virtual ~MCATitleUpdater() {};
    };

/**
 * Helper class for stroring editor content.
 */
class MCAEditorContentStorage
    {
    public:
        /**
         * Stores editor content.
         * @param aContent Pointer to content data,
         *                 takes ownership.
         */
        virtual void StoreContent( const HBufC* aContent ) = 0;

    protected:

        /**
         * Destructor for protection.
         */
        virtual ~MCAEditorContentStorage() {};
    };

// CLASS DECLARATION

/**
*  This is container class for conversation view
*
*  @lib chat.app
*  @since 3.2s
*/
class CCAConversationsContainer : public CCoeControl,
            public MEikEdwinSizeObserver,
            public MCALayoutChangeObserver,
            public MCATapEventObserver,
            public MCoeControlObserver,
            public MCAMessageEditorObserver,
            public MCALoginRefreshObserverPC,
            public MCAMsgAddedToRichTxtCtrlObserver
    {
    public: // Enumerations

        /**
         * Observed controls (MCATapEventObserver)
         */
        enum TTouchableControls
            {
            EMessageList = 1,
            EMessageEditor
            };

    public:  // Constructors and destructor

        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        * @param aViewSwitcher Handle to view switcher.
        * @param aRecipientObserver Recipient observer
        * @param aParent Context access mechanism.
        * @param aMessageHandler Handling messages
        * @param aTitleUpdater for updating title of chat.
        * @param aTabId
        */
        static CCAConversationsContainer* NewL( const TRect& aRect,
                                                MCAViewSwitcher& aViewSwitcher,
                                                MCAConversationPC* aCCAConversationManagerPC,
                                                MCARecipientObserver& aRecipientObserver,
                                                MObjectProvider* aParent,
                                                CCAMessageExtensionsHandler& aMessageHandler,
                                                MCATitleUpdater& aTitleUpdater,
                                                MCAEditorContentStorage& aEditorContentStorage,
                                                CEikButtonGroupContainer* aCba,
                                                const TBool aActivateFromTabSwitch , TInt& aTabId );

        /*
        * Constructor
        */
        CCAConversationsContainer(
            MCAViewSwitcher& aViewSwitcher,
            MCAConversationPC* aCCAConversationManagerPC,
            MCARecipientObserver& aRecipientObserver,
            CCAMessageExtensionsHandler& aMessageHandler,
            MCATitleUpdater& aTitleUpdater,
            MCAEditorContentStorage& aEditorContentStorage,
            CEikButtonGroupContainer* aCba, TInt& aTabId );

        /**
        * Destructor.
        */
        virtual ~CCAConversationsContainer();


    private:

        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        * @param aParent Context access mechanism.
        */
        void ConstructL( const TRect& aRect, MObjectProvider* aParent,
                         const TBool aActivateFromTabSwitch );


    public: // New functions

        /**
         * Check if selected item is object
         * @param aIsOpenable method sets this ETrue if selected item
         *                    is openable object else set to EFalse
         * @return ETrue if selected item is object
         */
        TBool IsObjectSelected( TBool& aIsOpenable , TBool& aIsSaved );

        /**
         * @return Selected message or NULL if nothing selected
         */
        MCAConversationMessage* SelectedMessage();

        /**
         * Sets item highlighting on or off (automatic find)
         * @param aHihghlight ETrue to set finder on
         */
        void SetItemHighlightL( TBool aHighlight );

        /**
         * Should we fetch new messages or not
         * @param aFetch ETrue if this chat is in foreground and should receive
         *               messages, EFalse otherwise.
         */
        void FetchMessages( TBool aFetch );

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
        * This checks if scroll mode is on or off
        * @since 3.2
        * @return Returns if scrolling is stopped or not (TBool)
        */
        TBool IsStopped() const;

        /**
        * This method starts scrolling
        * @since 3.2
        */
        void StartScrollingL( const TBool aActivateFromTabSwitch = EFalse );

        /**
         * This method stops scrolling
         * @since 3.2
         */
        void StopScrollingL();

        /**
        * This method returns pointer to msg editor
        * @since 3.2
        * @return Handle to editor
        */
        CCAMessageEditor& Editor() const;

        /**
        * This method handles message sending
        * @since 3.2
        */
        void SendMessageL();

        /**
         * This function allows friend adding to friends list
         * @since 3.2
         */
        void AddToFriendsL();


        /**
        * Blocks user
        * @since 3.2
        */
        void BlockUserL();

        /**
         * Checks if user is blocked
         * @since 3.2
         * @return ETrue if blocked
         */
        TBool IsBlocked() const;


        /**
        *Block user by wv id
        * @since 3.2
        */
        void BlockUserWVIdL();

        /**
        *Unblock users
        * @since 3.2
        */
        void UnBlockL();

        /**
        * Show blocked contacts
        * @since 3.2
        */
        void DisplayBlockedListL();

        /**
         * Checks if application user has sent a message
         * @since 3.2
         * @return ETrue if did
         */
        TBool ApplicationUserSentThis() const;

        /**
         * Sets the Logging/recording on
         * @since 3.2
         */
        void RecordChatL();
#ifdef RD_MULTIPLE_DRIVE

        /**
         *	show memorey selection dialog and updatesthe same to the cenrep.
         *	Leaves if HBufC::NewL fails.
		 *	@return ETrue if ok 
         *	@since 2.1
         */
	    TBool ShowMemorySelectionDialogL();

#endif

        /**
         * Tests if extension (in chars) fits to editor
         * @return ETrue if fits
         * @param aLength Extensions length in chars
         * @since 3.2
         */
        TBool ExtensionFits( TInt aLength );

        // some oracles for the view class

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
         * CheckReplyL. Check if there is reply situation
         */
        void CheckReplyL();

        /**
         * Sets the size of all icons; smileys etc.
         * @param aParentRect Size of the parent object (view)
         */
        void ResizeIcons( TRect aParentRect );

        /**
         * Method to force refresh in the rich text editor,
         * called from view when adding a smiley.
         * @since 3.0
         */
        void RefreshEditorL();

        /**
         * Is item highlight on or off.
         * @since S60 v3.2
         * @return ETrue if find activated, EFalse otherwise
         */
        TBool IsItemHighlight() const;

        /**
         * Returns TRect for reading pane text line element.
         * Used to determine icon sizes.
         * @since S60 v3.
         * @return TRect for reading pane text line element.
         */
        const TRect TextRect() const;

        /**
         * ETrue if message sending is ongoing
         */
        TBool Sending() const;

        /**
         * Resets the sending state
         */
        void ResetSending();

        /**
         * Checks if conversation partner is blocked
         * and unblocks if partner is blocked. Shows needed
         * notes. Used in message sending.
         * @since S60 v3.2
         */
        void CheckBlockedL();


    public:
        //added for multitab support
        /**
         * SwitchViewL
         * Switches between the conversation
         * @since S60 v3.2
         */
        void SwitchViewL( );

        /**
         * GetEditorFlag
         * get the current editor flag
         * @since S60 v3.2
         */
        TInt GetEditorFlag( );

        /**
         * Handle editor event.
         * from  MCALoginRefreshObserverPC
         */
        void HandleLoginRefreshCompleteL();

    private:
        /**
         * This function does basic clearing after the message is sent
         */
        void ResetAfterSendL( const TBool aActivateFromTabSwitch = EFalse );

        /**
         * Update the CBA labels
         */
        void UpdateCbaL();

        /**
         * This function displays information notes
         * @param aResourceId Resource ID of note
         */
        void DisplayInfoNoteL( const TInt aResourceId );

        /**
         * Display warning note before recording
         */
        void ShowRecordSavingWarningL() const;


    private: // Functions from base classes

        /**
         * From MCALayoutChangeObserver, for layout change observing
         */
        void LayoutChangedL( TInt aType );

        /**
         * From MEikEdwinSizeObserver, This method handles edwin's size change
         * @param aEdwin The edwin for which the size event is being handled
         * @param aEventType The event type
         * @param aDesirableEdwinSize The desired size for the edwin identified
         *                            by aEdwin
         * @return The return value depends on your implementation.
         *         Return ETrue if you wish to redraw the edwin.
         *         EFalse otherwise
         */
        TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin,
                                     TEdwinSizeEvent aEventType, TSize aDesirableEdwinSize );

    private: // From MCATapEventObserver

        /**
         * @see MCATapEventObserver
         */
        void HandleTapEventL( TTapEvent aEvent, TUint aControlId );

    private: // From MCoeControlObserver

        /**
         * @see MCoeControlObserver
         */
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

    private: //from MCAMsgAddedToRichTxtCtrlObserver

        /**
         * @see MCAMsgAddedToRichTxtCtrlObserver
         */
        void HandleMessageAddedL( const TInt aMessageCount );

    private: // Functions CCoeControl

        /**
        * From CoeControl, Handles "size changed"-events..
        */
        void SizeChanged();

        /**
        * From CoeControl,
        * Returns the number of control contained by this class.
        * @return Number of controls contained
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl, Returns handle to control pointed by aIndex
        * @param aIndex Wanted control's index [0..n]
        * @return Handle to wanted control
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * From CCoeControl, Gets called when drawing is needed (by framework)
         * @param aRect Rect that needs to be drawed
         */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl, Handles key-events
        * @param aKeyEvent Event that occured
        * @param aType Type of key-event
        *              (EEventKey, EEventKeyUp or EEventKeyDown)
        * @return Containers response to event
        *                    (EKeyWasNotConsumed/ EKeyWasConsumed)
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /*
        * From CCoeControl.
        * @param aContext Help context
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
        /*
         * From CCoeControl.
         * @param aDrawNow Draw param
         */
        void FocusChanged(TDrawNow aDrawNow);

    private:    // From MCAMessageEditorObserver

        /**
         * Handle editor event.
         */
        void HandleMessageEditorEventL( TEditorEvent aEvent );

    private:    // new functions

        /**
         * Loads bitmaps
         */
        void LoadBitmapsL();


    private:    // Data

        // Owns. Chat view control
        CCAChatViewControl* iChatView;

        // Doesn't own. Handle to view switcher
        MCAViewSwitcher& iViewSwitcher;

        //Doesn't own. Handle to recipient observer
        MCARecipientObserver& iRecipientObserver;

        // Owns. Editor used for chat input
        CCAMessageEditor* iEditor;

        //Owns. UI side blocker
        CCABlockingUI* iBlockingUI;

        //Owns. "From me" icon
        CGulIcon* iFromMe;

        //Owns. "To me" icon
        CGulIcon* iToMe;

        //Owns. "Unsupported" icon
        CGulIcon* iUnsupported;

        //Owns. "Corrupted" icon
        CGulIcon* iCorrupted;

        TBool iIsChangeToOnlineAskedOnce;
        TBool iSending;

        TInt iCurrentItemIndex;

        TAknLayoutRect iOutLine;
        TAknLayoutRect iHoriline;
        TAknLayoutRect iVertLine;


        TPtrC iCurrentRecipient;

        MCASkinVariant* iSkinVar;
        CCAAppUi*		iAppUi;

        CCAMessageExtensionsHandler& iMessageExtensionsHandler;

        // If add contact is asked. ETrue if not EFalse.
        TBool iIsAddToContactAskedOnce;

        /// For updating title when needed.
        MCATitleUpdater& iTitleUpdater;

        TBool iItemHighlight;

        // For storing editor content. Not owned.
        MCAEditorContentStorage& iEditorContentStorage;

        // Does not own. Pointer to cba of view.
        CEikButtonGroupContainer* iCba;

        // See IMVariant.hrh.
        // Flag depends on EIMFeatSendkeyInMsgList.
        TBool iSendkeyInMsgList;

        //ptr to the conversation's process component
        MCAConversationPC* iConversationPC;

        //owns
        CCAEditIndicator* iEditIndicator;

        TInt& iTabId;

        TBuf<KMaxWVIDLength> iWvId;

        // Used in switching tabs
        TBool ifocusFlag;

        // Flag to indicate whether login is running
        TBool iLogingFlag;
    };

#endif      // CCACONVERSATIONSCONTAINER_H

// End of File
