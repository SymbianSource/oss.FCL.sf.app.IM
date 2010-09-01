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
* Description:  View class for private chat lists
*
*/



#ifndef CCAINVITATIONVIEW_H
#define CCAINVITATIONVIEW_H

//  INCLUDES
#include    "CCAView.h"
#include    "chatdefinitions.h"
#include    "MCAInvitationObserverPC.h"
#include	"MCATabObserver.h"
#include	"MCARecipientObserver.h"


// FORWARD DECLARATIONS
class CCAInvitationContainer;
class CCABlockingUI;
class MCAInvitationPC;
class CCAAppUi;
class CEikMenuPane;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION

/**
*  View class for private chat list
*
*  @lib chat.app
*  @since 1.2
*/
class CCAInvitationView : public CCAView,
            public MCAInvitationObserverPC,
            public MCATabObserver,
            public MCARecipientObserver
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CCAInvitationView();

        /**
        * Symbian OS default constructor.
        * @param aAvkonViewResourceId Id for resource used to
        *                             construct this view
        * @param aViewId View id
        */
        void ConstructL( TInt aAvkonViewResourceId, TUid aViewId );

        /**
         * C++ default constructor.
         */
        CCAInvitationView();


    private: // From CAknView

        /**
        * From CAknView Handles command events. Gets called by framework
        * @param aCommand Command that was received
        */
        void HandleCommandL( TInt aCommand );

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
         *Preforms the actual switching between tabs
         * @See MCATabObserver
         */
        void SwitchTabL( const TDesC& aWvId, const TInt aTabIndex );

        /**
         * From MCARecipientObserver
         * Shows customized menu
         */
        void ShowPopUpMenuL();

        /**
         * From MCARecipientObserver
         * Shows recipient list
         * @param aResourceId Resource id for pop up list
         * @return Positive value if Oked
         */
        TInt ShowRecipientsListL( TInt aResourceId );

        /**
         * Dynamically initialises menu pane
         * @see MEikMenuObserver
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private: // From MCAInvitationObserverPC

        /**
         *@see MCAInvitationObserverPC
         */
        virtual void HandleInvitationEventL( TEnumsPC::TInviteType aInviteType, TInt aIndex );

        /**
         * @see MCAInvitationObserverPC
         */
        virtual void HandleInvitationResponse( TBool /*aAcceptance*/,
                                               const TDesC& /*aUserId*/,
                                               const TDesC&  /*aGroupName*/,
                                               const TDesC& /*aResponse*/  );


    private:    // Data

        //Owns
        CCAInvitationContainer* iContainer;

        // Buffer for last known id
        TBuf<KMaxWVIDLength> iLastKnownId;
        TBool iIsLastKnownIdInvitation;

        // Pointer to callback
        CAsyncCallBack* iAsyncCallBack;
        //Pop-up menu for context sensitive options. Owns
        CEikMenuBar* iContextMenu;

        CCABlockingUI* iBlockingUI;

        MCAInvitationPC* iInvitationPC;

        CCAAppUi* iAppUi;

        // Owns.  Sender identification (wvid)
        HBufC* iIdentification;

        //InvitationIndex
        TInt iInvitationIndex;

        //Does not Own
        HBufC* iRejectReason;

        HBufC* iScreenName;

        // Context menu flag
        TBool iContextFlag;

        TBool iChatJoinRejectFlag;

        // Skin background control context
        CAknsBasicBackgroundControlContext* iBgContext;

        TInt iTabId;
        //doesnt own
        CCAStatusPaneHandler* iTitlePane;

    };

#endif      // CCAPRIVATECHATLISTVIEW_H

// End of File
