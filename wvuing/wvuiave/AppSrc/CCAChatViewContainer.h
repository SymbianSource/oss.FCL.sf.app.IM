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
* Description:  Container class for chat group view
 *
*/



#ifndef CCACHATVIEWCONTAINER_H
#define CCACHATVIEWCONTAINER_H

// INCLUDES
#include "mcagroupeventobserverpc.h"
#include "MCALayoutChangeObserver.h"
#include "ChatDefinitions.h"            // KNickNameLength
#include "mcatapeventobserver.h"
#include "MCAMsgAddedToRichTxtCtrlObserver.h"
#include "CCAMessageEditor.h"   // MCAMessageEditorObserver
#include <NewFileService.hrh> // TNewServiceFileType
#include <eikedwob.h>
#include <aknutils.h>

// FORWARD DECLARATIONS
class MCAViewSwitcher;
class MCARecipientObserver;
class CCAMessageEditor;
class CCAAppUi;
class MCASkinVariant;
class CCAMessageExtensionsHandler;
class CCAChatViewControl;
class MObjectProvider;
class MCAConversationMessage;
class MCAGroupPC;
class CCAEditIndicator;
class MCAGroupUtilsPC;
class CCAChatView;



// CLASS DECLARATION

/**
 *  This is container class for chat view
 *  @since 3.2
 */
class CCAChatViewContainer : public CCoeControl,
            public MEikEdwinSizeObserver,
            public MCAGroupEventObserverPC,
            public MCALayoutChangeObserver,
            public MCATapEventObserver,
            public MCoeControlObserver,
            public MCAMessageEditorObserver,
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

    public: // Constructors and destructor

        /**
         * Symbian OS default constructor.
         * @param aRect Frame rectangle for container.
         * @param aViewSwitcher Handle to view switcher.
         * @param aGroupPC Handle to group pc
         * @param aRecipientObserver Recipient observer
         * @param aParent Context access mechanism.
         * @param aExtensionHandler Ref. to Msg. Extension Handler
         * @param aCba Pointer to Button Group Container class
         * @param aActivateFromTabSwitch Flag used while switching
         * in tabs, to do focus in text box
         * @param aTabId Ref. to tab id for this group
         */
        static CCAChatViewContainer* NewL( const TRect& aRect,
                                           MCAViewSwitcher& aViewSwitcher,
                                           MCAGroupPC* aGroupPC,
                                           MCARecipientObserver& aRecipientObserver,
                                           MObjectProvider* aParent,
                                           CCAMessageExtensionsHandler& aExtensionHandler,
                                           CEikButtonGroupContainer* aCba,
                                           const TBool aActivateFromTabSwitch,
                                           TInt& aTabId,
                                           CCAChatView * aView );

        /**
         * Destructor.
         */
        virtual ~CCAChatViewContainer();

    private:    // constructors

        /**
         * Default constructor
         * @param aViewSwitcher Handle to view switcher.
         * @param aGroupPC Handle to group pc
         * @param aRecipientObserver Recipient observer
         * @param aParent Context access mechanism.
         * @param aExtensionHandler Ref. to Msg. Extension Handler
         * @param aCba Pointer to Button Group Container class
         * @param aTabId Ref. to tab id for this group
         */
        CCAChatViewContainer(
            MCAViewSwitcher& aViewSwitcher,
            MCAGroupPC* aGroupPC,
            MCARecipientObserver& aRecipientObserver,
            CCAMessageExtensionsHandler& aMessageHandler,
            CEikButtonGroupContainer* aCba, TInt& aTabId, CCAChatView * aView );

        /**
         * Symbian OS default constructor.
         * @param aRect Frame rectangle for container.
         * @param aParent Context access mechanism.
         * @param aActivateFromTabSwitch Flag used while switching
         * in tabs, to do focus in text box
         */
        void ConstructL( const TRect& aRect, MObjectProvider* aParent,
                         const TBool aActivateFromTabSwitch );


    public:

        /**
         * Switches the tab
         */
        void SwitchTabL();

        /**
         * Check if selected item is object
         * @param aIsOpenable method sets this ETrue if selected item
         *                    is openable object else set to EFalse
         * @param aSavePossible method sets this ETrue if selected item
         *                    is savable object else set to EFalse
         * @return ETrue if selected item is object
         */
        TBool IsObjectSelected( TBool& aIsOpenable , TBool& aSavePossible );

        /**
         * @return Selected message or NULL if nothing selected
         */
        MCAConversationMessage* SelectedMessage();

        /**
         * @return Selected item
         */
        HBufC* SelectedItemL();

        /**
         * @return Type of selected item
         */
        TInt SelectedItemType();

        /**
         * Sets item highlighting on or off (automatic find)
         * @param aHihghlight ETrue to set finder on
         */
        void SetItemHighlight( TBool aHighlight );

        /**
         * Should we fetch new messages or not
         * @param aFetch ETrue if this chat is in foreground and should receive messages,
         *               EFalse otherwise.
         */
        void FetchMessages( TBool aFetch );

        /**
         * Checks if current message is private or not
         * @return Is message private or not ETrue if private message
         */
        TBool CheckIfPrivateMessageL();

        /**
         * Does really group properties change
         * @param aGroupd Group id
         */
        void DoHandleGroupPropertiesChangedL();

        /**
         * Handles the leaving of the group and
         * switching of view etc. logic
         * @param aReason Reason for leaving the group
         */
        void DoHandleGroupLeftL( TInt aReason );

        /**
         * Checks if user can invite friends to this group
         * @since 3.2
         * @return Is invitation allowed or not
         */
        TBool IsInvitationAllowed() const;

        /**
         * Start invite editor for current group
         * @since 3.2
         */
        void InviteFriendsToGroupL();

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
         * Updates the softkeys dynamically
         */
        void UpdateCbaL();

        /**
         * Sets item emphasization on or off
         * @since 3.2
         * @param aIsEmphasized On/Off (TBool)
         */
        void SetItemEmphasize( const TBool aIsEmphasized );

        /**
         * This method checks if emphasization is on or off
         * @since 3.2
         * @return Is emphasization on or off (TBool)
         */
        TBool IsEmphasized() const;

        /**
         * This method returns pointer to msg editor
         * @since 3.2
         * @return Handle to editor
         */
        CCAMessageEditor& Editor() const;

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
         * This method handles message sending
         * @since 3.2
         */
        void SendMessageL();

        /**
         * This method handles object sending
         * @param aNewFile If ETrue, a new file will be created before sending
         * @param aFileType Type of file to be created when creating new file
         * @since 3.2
         */
        void SendObjectL( TBool aNewFile,
                          TNewServiceFileType aFileType = ENewFileServiceImage );

        /**
         * This method initialises whisper message
         * @since 3.2
         * @param aNickName A nick name whom to send whisper message
         * @param aGroupMsg ETrue if multiple recipients (to group)
         */
        void InitPrivateMessageL( const TDesC& aNickName, const TBool aGroupMsg );

        /**
         * This function returns handle to array of recipients
         * @since 3.2
         * @return Handle to recipient array
         */
        CDesCArrayFlat& Recipients() const;

        /**
         * This function returns handle to array of SentRecipients
         * @since 3.2
         * @return Handle to sent recipient array
         */
        CDesCArrayFlat* SentRecipients() const;

        /**
         * This function allows reply to focused contact
         * @since 3.2
         */
        void ReplyL();

        /**
         * This function displays chat properties dialog
         * @since 3.2
         */
        void EditChatGroupPropertiesL();

        /**
         * This function allows leaving from group
         * @since 3.2
         */
        void LeaveGroupL();

        /**
         * This method displays chat info
         * @since 3.2
         */
        void DisplayChatInfoL();

        /**
         * Checks if application user has sent a message
         * @since 3.2
         * @return ETrue if did
         */
        TBool ApplicationUserSentThis() const;

        /**
         * @since 3.2
         * @return ETrue if this is received message
         */
        TBool Received() const;


        /**
         * Sets the Logging/Saving on
         * @since 3.2
         */
        void SaveChatL();

        /**
         * Sets the logging off
         * @since 3.2
         */
        void SetLoggingOffL();

        /**
         * Checks if whispering is allowed
         * @since 3.2
         * @return ETrue if allowed
         */
        TBool IsWhisperAllowedL() const;

        /**
         * Checks if user is admin in this group
         * @since 3.2
         * @return ETrue if admin else EFalse
         */
        TBool IsUserAdmin() const;

        /**
         * Checks if group is open
         * @since 3.2
         * @return ETrue if open else EFalse
         */
        TBool IsGroupOpen() const;

        /**
         * Tests if extension (in chars) fits to editor
         * @return ETrue if fits
         * @param aLength Extensions length in chars
         * @since 3.2
         */
        TBool ExtensionFits( TInt aLength );

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
         * This function does basic clearing after the message is sent
         * aActivateFromTabSwitch - Flag used in Tab switching
         */
        void ResetAfterSendL( const TBool aActivateFromTabSwitch = EFalse );


        /**
         * Sets the size of all icons; smileys etc.
         * @param aParentRect Size of the parent object (view)
         */
        void ResizeIcons( TRect aParentRect );

        /**
         * Method to force refresh in the rich text editor,
         * called from view when adding a smiley.
         * @since 3.2
         */
        void RefreshEditorL();

        /**
         * Is item highlight on or off.
         * @since 3.2
         * @return ETrue if find activated, EFalse otherwise
         */
        TBool IsItemHighlight() const;

        /**
         * Returns TRect for reading pane text line element.
         * Used to determine icon sizes.
         * @since 3.2
         * @return TRect for reading pane text line element.
         */
        const TRect TextRect() const;

#ifdef RD_MULTIPLE_DRIVE

        /**
         *	show memorey selection dialog and updatesthe same to the cenrep.
         *	Leaves if HBufC::NewL fails.
		 *	@return ETrue if ok 
         *	@since 2.1
         */
	    TBool ShowMemorySelectionDialogL();

#endif


    private:    // From MCAMessageEditorObserver

        /**
         * Handle editor event.
         */
        void HandleMessageEditorEventL( TEditorEvent aEvent );

    private: // Functions from MCALayoutChangeObserver

        /**
        * From MCALayoutChangeObserver, for observing layout changes
        */
        void LayoutChangedL( TInt aType );

    private: // From MCAGroupEventObserverPC

        /**
         * Called when the server has indicated that the user is not a member
         * anymore.
         * @param aReason specifies why.
         */
        void HandleGroupLeft( TInt aReason );

        /**
         * Called when new group properties are received from the
         * the network. These are server initiated properties.
         */
        void HandleGroupPropertiesChanged();

    private: // Functions from MEikEdwinSizeObserver

        /**
         * From MEikEdwinSizeObserver, This method handles edwin's size change
         * @param aEdwin The edwin for which the size event is being handled.
         * @param aEventType The event type.
         * @param aDesirableEdwinSize The desired size for the edwin identified by aEdwin.
         * @return The return value depends on your implementation.
         * Return ETrue if you wish to redraw the edwin. EFalse otherwise.
         */
        TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin,
                                     TEdwinSizeEvent aEventType, TSize aDesirableEdwinSize );

    private: //from MCAMsgAddedToRichTxtCtrlObserver
        //when user navigate among tab.

        /**
         * @see MCAMsgAddedToRichTxtCtrlObserver
         */
        void HandleMessageAddedL( const TInt aMessageCount );

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

    private: // Functions CCoeControl

        /**
         * From CoeControl, Handles "size changed"-events..
         */
        void SizeChanged();

        /**
         * From CoeControl, Returns the number of control contained by this class.
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
         * @param aEvent Event that occured
         * @param aType Type of key-event (EEventKey, EEventKeyUp or EEventKeyDown)
         * @return Containers response to event (EKeyWasNotConsumed/ EKeyWasConsumed)
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

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

    private: //New functions

        /**
         * This function displays information notes
         * @param aResourceId Resource ID of note
         */
        void DisplayInfoNoteL( const TInt aResourceId );


        /**
         * This function clears selected text from editor
         * @param aSelectionLength Length of selected string
         */
        void ClearSelectionL( const TInt aSelectionLength );

        /**
         * Creates private message header
         * @param aIconHeader Icon header
         * @param aGroupMsg If ETrue then group icon else single icon
         * @param aNick Descriptor that contains nick name
         * @param aMsg Descriptor that contains message
         */
        void CreatePrivMsgHeaderL( TPictureHeader& aIconHeader,
                                   const TBool& aGroupMsg,
                                   TDesC& aNick,
                                   HBufC* aMsg );

        /**
         * Creates private message header for A/H variant
         * @param aIconHeader Icon header
         * @param aGroupMsg If ETrue then group icon else single icon
         * @param aNick Descriptor that contains nick name
         * @param aMsg Descriptor that contains message
         */
        void CreatePrivMsgAHHeaderL( TPictureHeader& aIconHeader,
                                     const TBool& aGroupMsg,
                                     TDesC& aNick,
                                     HBufC* aMsg );

        /**
         * Loads bitmaps that are needed in this container
         */
        void LoadBitmapsL();


    private: //data

        // Owns. Chat view control
        CCAChatViewControl* iChatView;

        //Doesn't own. Handle to recipient observer
        MCARecipientObserver& iRecipientObserver;

        // Doesn't own. Handle to view switcher
        MCAViewSwitcher& iViewSwitcher;

        // Doesn't own. Handle to group process component
        MCAGroupPC* iGroupPC;


        // Owns. Editor used for chat input
        CCAMessageEditor* iEditor;

        // Owns. Graphics for private icon
        CGulIcon* iPrivIcon;
        CGulIcon* iPrivToMany;
        CGulIcon* iToMe;

        //Owns. "Unsupported" icon
        CGulIcon* iUnsupported;

        //Owns. "Corrupted" icon
        CGulIcon* iCorrupted;

        TBool iIsEmphasis;
        TBool iIsPrivateMsgActive;

        TBool iIsLoggedIn;
        TBool iIsEditorStateChanged;
        // Is whispering allowed in group
        TBool iWhisperingAllowed;
        // Is group open
        TBool iIsOpen;

        TInt iNickLength;
        TInt iCurrentTextLenght;

        TAknLayoutRect iOutLine;
        TAknLayoutRect iHoriline;
        TAknLayoutRect iVertLine;

        // Owns. Array of screennames
        CDesCArrayFlat* iRecipients;
        CDesCArrayFlat* iSentRecipients;

        //Owns. Group id
        HBufC* iGroupId;

        //doesn't own
        CCAAppUi* iAppUi;

        MCASkinVariant* iSkinVar;

        // these are set by InitPrivateMessage
        // needed for layout changing
        TBuf<KNicknameMaxLength> iCurrentNickInPrivMsg;
        TBool					 iIsGroupMsgInPrivMsg;

        //Extension handler
        CCAMessageExtensionsHandler& iMessageExtensionsHandler;

        TBool iItemHighlight;

        // Does not own. Pointer to cba of view.
        CEikButtonGroupContainer* iCba;

        // See IMVariant.hrh.
        // Flag depends on EIMFeatSendkeyInMsgList.
        TBool iSendkeyInMsgList;

        //owns
        CCAEditIndicator* iEditIndicator;

        //Tab id
        TInt& iTabId;

        //Doesnt own,pointer to Group Utils PC
        MCAGroupUtilsPC* iGroupUtilsPC;
        // Used for switching tabs
        TBool ifocusFlag;

        CCAChatView *iView;
    };

#endif //CCACHATVIEWCONTAINER_H

// End of File
