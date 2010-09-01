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
* Description:  This is view class for chat group view
*
*/



#ifndef CCACHATVIEW_H
#define CCACHATVIEW_H

// INCLUDES
#include "CCAView.h"
#include "MCARecipientObserver.h"
#include "mcagroupobserverpc.h"
#include "PublicEngineDefinitions.h"
#include "MCATabObserver.h"

// FORWARD DECLARATIONS
class CCAChatViewContainer;
class CCAMessageExtensionsHandler;
class CAsyncCallBack;
class CFindItemMenu;
class CItemFinder;
class CEikButtonGroupContainer;
class MCAGroupPC;

// CLASS DECLARATION

/**
 *  This is class for chat group view
 *  @since 3.2
 */
class CCAChatView : public CCAView,
            public MCARecipientObserver,
            public MCAGroupObserverPC,
            public MCATabObserver
    {

    public: // Constructors and destructor

        /**
         * C++ default constructor
         */
        CCAChatView();

        /**
         * Symbian OS default constructor.
         * @param aAvkonViewResourceId Id for resource used
         *        to construct this view
         * @param aViewId View id
         * @param aMessageHandler -reference to Message Extension Handler
         */
        void ConstructL( TInt aAvkonViewResourceId, TUid aViewId,
                         CCAMessageExtensionsHandler& aMessageHandler );

        /**
         * Destructor.
         */
        virtual ~CCAChatView();

    protected: // Functions from base classes

        /**
         * From MEikMenuObserver, Called by framework before
         * constructing menupane
         * @param aResourceId Resource to be constructed
         * @param aMenuPane Pointer to menupane being constructed
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
         * From CAknView
         * @param aCommand Command that was received
         */
        void HandleCommandL( TInt aCommand );

        /**
         * Handles foreground event
         * @param aForeground ETrue if foreground else EFalse
         * @see CAknView
         */
        void HandleForegroundEventL( TBool aForeground );

    private: // From CAknView

        /**
         * From CAknView, Gets called from framework when activating this view
         * @param aPrevViewId Previous view id
         * @param aCustomMessageId Custom message's id
         * @param aCustomMessage Custom message
         */
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
         * From AknView. Deactivates
         */
        void DoDeactivate();

    private:    // functions from MCARecipientObserver

        /**
         * Shows recipient list in chat and I-box view
         * @param aResourceId The resource id where to build list
         * @return result of query 0 if canceled else 1
         */
        TInt ShowRecipientsListL( TInt aResourceId );

        /**
         * Shows customized menu
         */
        void ShowPopUpMenuL();

    private: // From MCAGroupObserverPC


        /**
         * @see MCAGroupObserverPC
         */

        void HandleChatEvent( const TDesC& aWvid = KNullDesC,
                              const TDesC& aContactId = KNullDesC );

        /**
         * Actual handler for chat events
         */
        void DoHandleChatEventL( const TDesC& aWvid = KNullDesC,
                                 const TDesC& aContactId = KNullDesC );



        /**
         * @see  MCAGroupObserverPC
         */
        void HandleMessageError(  TInt aError, const TDesC& aNick = KNullDesC );

        /**
         * Leavable version of error handler
         * @see HandleMessageError
         */
        void DoHandleMessageErrorL( TInt aError, const TDesC& aNick = KNullDesC );


    private: // From MCATabObserver

        /**
        * @see MCATabObserver
        */
        void SwitchTabL( const TDesC& aWvId, const TInt aTabIndex );

    private:    // new methods

        /**
         * Displays list of joined members
         */
        void DisplayJoinedMembersL();

        /**
         * Updates selected item type
         */
        void UpdateItemTypeL();

        /**
         * Callback function for launching the invite query
         * Called by CAsyncCallBack
         * @param aInstance Instance of this class
         * @return Need for recall
         */
        static TInt LaunchInvite( TAny *aInstance );

        /**
         * The actual implementation
         * @param aShowQuery If ETrue, ask the user
         */
        void DoLaunchInviteL( TBool aShowQuery );

    public:    // new methods

        /**
         * Whether menu has been poped up
         */
        TBool GetIfMenuPopup();
    private: // Data

        // Pointer to callback
        CAsyncCallBack* iAsyncCallBack;

        // Previous view's id
        TVwsViewId iPrevViewId;

        // Owns. Container
        CCAChatViewContainer* iContainer;

        // Owns. Array of recipients
        CDesCArray* iRecipients;

        // Owns. Popup-menu for add to friends and reply
        CEikMenuBar* iAddAndReplyMenu;

        // Owns. Popup-menu for find item UI
        CEikMenuBar* iFindContextMenu;

        //Array of joined members. Owns
        CDesCArrayFlat* iJoinedMembers;

        //Group id. Owns
        HBufC* iGroupId;

        //recipient. Owns
        HBufC* iRecipient;

        //Sender. Owns
        HBufC* iSender;

        //Sender without the "wv:" prefix
        TPtrC iCleanSender;

        //Does not own. Extension handler for messages
        CCAMessageExtensionsHandler* iMessageHandler;

        // not owned. Hides CBA buttons when needed
        CEikButtonGroupContainer* iCba;

        //Doesnt Own, Ptr to Group PC interface
        MCAGroupPC* iGroupPC;

        // owns. menu pane for find item functionality
        CFindItemMenu* iFindMenu;

        // owns.
        CItemFinder* iItemFinder;

        //TabId for this group
        TInt iTabId;

        //doesn't own, pointer to grouputils
        CCAGroupUtils* iGroupUtils;

        // A indicator for object process
        TBool iObjectOpenedSaved;

        CEikMenuPane *iMenuPane;
    };

#endif //CCACHATVIEW_H

// End of File
