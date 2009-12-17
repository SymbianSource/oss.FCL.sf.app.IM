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
* Description:  This class handles status, title and contextpane operations
 *
*/



#ifndef CCASTATUSPANEHANDLER_H
#define CCASTATUSPANEHANDLER_H

//  INCLUDES
#include <akntabgrp.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <akncontext.h>
#include <aknindicatorcontainer.h>
#include <akneditstateindicator.h>

#include <akntabobserver.h>

#include "MCAMainViewObserverPC.h"
#include "MCATabObserver.h"


// FORWARD DECLARATIONS
class CCAAppUi;
class MCAOpenChatsPC;

// CLASS DECLARATION

/**
 *  This class handles status, title and contextpane operations
 *
 *  @chatng.exe
 *  @since 3.2
 */
class CCAStatusPaneHandler : public CBase, public MAknTabObserver,
            public MCAMainViewObserverPC
    {
    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CCAStatusPaneHandler();

        /**
         * Two-phased constructor.
         */
        static CCAStatusPaneHandler* NewL();

        /**
         * Destructor.
         */
        virtual ~CCAStatusPaneHandler();

    public: // New functions

        /**
         * Hides navi-pane decorators
         * @since 1.2
         * @param aPicture Bitmap file
         * @param aMask Bitmap mask file
         */
        void SetPicture( const CFbsBitmap* aPicture, const CFbsBitmap* aMask );

        /**
         * Shows navi-pane's tabgroup-decorator according to aResourceId
         * and activates a tab which has TabId=aViewId.
         * @since 1.2
         * @param aViewId ViewId of the view that is visible
         */
        void ShowTabGroupL( const TUid /*aViewId*/ );

        /**
         * Shows navi-pane's tabgroup-decorator according to aResourceId
         * and activates a tab which has aType.
         * @param aType - Type of tab Invitation/conversation
         * @param aWvId - user id
         * @param aInvitationTime - Received Time of the Invitation ( Only applicable to
         *   invitations, for others you can pass any value. suggested is TTime() )
         * @since 3.2
         */
        TInt ShowTabGroupL( const TEnumsPC::TItem aType, const TDesC& aWvId,
                            const TBool noCreate = EFalse );

        /**
         * pops the navi-pane's tabgroup-decorator
         * unregisters from the registered observers
         * Synchronises the process component list
         * @since 3.2
         */
        void SynchroniseAndRemoveTabGroup( );

        /**
         * Hides navi-pane decorators.
         * @param aAllowDuplicates If this is true, duplicate default navi pane
         * contents are allowed. Duplicates need to be removed with PopNaviPane
         * when not needed anymore.
         * @since 1.2
         */
        void ClearNaviPaneL( TBool aAllowDuplicates = EFalse );

        /**
         * Sets title-pane text.
         * @since 1.2
         * @param aTitle Title text
         */
        void SetTitleL( const TDesC& aTitle );

        /**
         * Sets title-pane text. Uses eikon env to load string.
         * @since 2.1
         * @param aEikonEnv Eikon environment to allocate resource
         * @param aResourceId Resource id
         */
        void SetTitleL( CEikonEnv* aEikonEnv, TInt aResourceId );

        /*
         * Sets title-pane text. Uses StringLoader to load string.
         * @since 2.1
         * @param aResourceId Resource id
         * @param aSearchString Search string
         */
        void SetTitleL( TInt aResourceId, const TDesC& aSearchString );

        /**
         * Sets navi-pane label
         * @since 1.2
         * @param aTitle Navi-label text
         */
        void SetNaviLabelL( const TDesC& aText );

        /**
         * Returns pointer to currently active decorator's tabgroup.
         * @since 1.2
         * @return pointer to tabgroup
         */
        CAknTabGroup* TabGroup() const;

        /**
         * Fetches active tab´s id (ViewId)
         * @since 1.2
         * @return UID of an active tab view
         */
        TUid ActiveTabViewId();

        /**
         * Sets active tab by index.
         * @since 1.2
         * @param aIndex Index of a tab to be activated
         */
        void ActivateTab( const TInt aIndex );

        /**
         * This method returns pointer to indicator container
         * @since 1.2
         * @return Handle to indicator
         */
        CAknIndicatorContainer* Indicator() const;

        /**
         * Refresh title pane text, needed in skin change events.
         * @since 1.2
         */
        void RefreshTitlePaneTextL();

        /*
         * Gets title-pane text.
         * Ownership is not transfered.
         * @since 3.0
         */
        const TDesC* Title();

        /**
         * Prohibit tab group drawing for full-screen
         * dialogs. Changes take place during next redraw.
         * Call from dialog's PreLayoutDynInit. Remember to reset
         * by calling RestoreNaviPane in the destructor!
         * @param aProhibit ETrue to prohibit, EFalse to not.
         * @since 3.0
         */
        void ProhibitTabGroup( TBool aProhibit = ETrue );

        /**
         * Restore navi pane decorators to the previous state
         * before ClearNaviPaneL call.
         * Call in dialog destructor.
         * @since 3.0
         */
        void RestoreNaviPane();

        /**
        * Pops the topmost decorator out of the decorator stack.
        * @since 3.0
        */
        void PopNaviPane();



        void SetActiveTabByIndex( const TInt aIndex );
        /**
         * Delete remainings tab other than current
         * @param aType type of tab
         * @param aWvId user id
         *@param aInvitationTime time of message
         * @since 3.0
         */
        TInt ResetAndShowCurrentTabL( const TEnumsPC::TItem aType,
                                      const TDesC& aWvId );
    private:
        /**
         * @see MAknTabObserver
         */
        void TabChangedL( TInt aIndex );


    private:

        /**
         * @see MCAMainViewObserverPC
         */
        void HandleDelete( const TDesC& aContactId , TEnumsPC::TItem aType ) ;

        /**
         * @see MCAMainViewObserverPC
         */
        void HandleAddition( TEnumsPC::TItem aType, TInt aIndex );

        /**
         * @see MCAMainViewObserverPC
         */
        void HandleChange( TEnumsPC::TItem aType, TInt aIndex, TEnumsPC::TChange aChangeType );

        /**
         * @see MCAMainViewObserverPC
         */
        void HandleWatcherEvent( const TDesC& aString , TEnumsPC::TOnlineStatus aOnlineStatus );

        /**
        * @see MCAMainViewObserverPC
        */
        void HandleInvitationEventL( TEnumsPC::TInviteType aInviteType,
                                     const TDesC& aUserId,
                                     const TDesC& aMessage );

        /**
         * @see MCAMainViewObserverPC
         */
        void HandleInvitationResponse( TBool aAcceptance,
                                       const TDesC& aUserId,
                                       const TDesC& aGroupName,
                                       const TDesC& aResponse );

        /**
         * @see MCAMainViewObserverPC
         */
        void HandleSettingsChangeL( TInt aChangedSettingEnum );

    public:

        /**
         * API used to regiter for MCATabObserver Events
         */
        void AddObserver( MCATabObserver* aObserver ) ;

        /**
         * API used to unregiter from MCATabObserver events
         */
        void RemoveObserver( MCATabObserver* aObserver );


        const TPtrC GetTabItemIdentification( const TInt aIndex );
    private:


        /**
         * API to show the tabs
         */
        void ShowTabL();

        /**
         * API to hide the tabs
         */
        void HideTab();

    private:

        /**
         * Loads/updates bitmaps
         * @since 1.2
         */
        void CreateTabsL( const TBool noCreate );

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();



    private:    // Data

        // typedef to ease up the usage of decorator-array
        typedef CArrayPtrFlat<CAknNavigationDecorator> CCANaviPaneStack;

        // Doesn't own. Handle to navi-pane
        CAknNavigationControlContainer*	iNaviPane;

        // Owns. New decorated tab-group
        CAknNavigationDecorator*		iNewDecoratedTabGroup;

        // Owns. Text for navi-pane
        CAknNavigationDecorator*		iNaviPaneText;

        // Doesn't own. Handle to title-pane
        CAknTitlePane*					iTitlePane;

        // Flag to trace decorator-status
        TBool							iNaviTabsActivated;

        // Doesn't own. Handle to context-pane
        CAknContextPane*	            iContextPane;

        // Doesn't own. Handle to appui
        CCAAppUi*                       iAppUi;

        //Owns the OpenChats PC Component
        MCAOpenChatsPC*					iOpenChatsPC;

        // ETrue if tab group drawing is prohibited
        TBool iProhibitTabGroup;

        // Currently active view id
        TUid iActiveViewId;


        //Doesn't own
        RPointerArray< MCATabObserver > iObserverList;

        //previous tabID
        TInt iPrevTabId;

        //Current TabID
        TInt iCurrentTabId;

        //owns the new message icon
        HBufC* iNewMsgIcon;

        //owns the tabtext to be displayed for invitations
        HBufC* iInvitationTabText;

        TInt iOpenChatArrayCount; //record open chat array count

    };

#endif      // CCASTATUSPANEHANDLER_H

// End of File
