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
* Description:  IM application AppUi class
 *
*/


#ifndef CCAAPPUI_H
#define CCAAPPUI_H

// INCLUDES

#include "MCAViewSwitcher.h"	// MCAViewSwitcher
#include "ChatDefinitions.h"
#include "MCAAppUi.h"
#include "MCAUiLoginCmdCB.h"
#include "TEnumsPC.h"
#include "MCAUiGlobalNotificationObserverPC.h"

#include <aknviewappui.h>		// CAknViewAppUi
#include <aknwaitdialog.h>
#include <akntabobserver.h>
#include <MWVSettingsCASettingsng.h>
#include <MWVSettingsObserverng.h>
#include "CAExternalInterface.h"
#include <AknInfoPopupNoteController.h> // for CAKnInfoPopupNote
#include <MIMPSSAPObserver.h>
#include "MCABackGroundTaskObserver.h"
#include "MCASplashScreenObserver.h"
#include "MCASkinVariant.h"

#include <cimpspresenceconnectionuing.h> // for enum TIMPSLoginType

// FORWARD DECLARATIONS
class CCAEngine;
class CIMPSConnectionUiAgent;
class CIMPSSAPSettingsStore;
class CIMPSSAPSettings;

class CCASplashScreen;
class CCAStatusPaneHandler;
class CCAAudioManager;
class CCAInputAbsorber;
class CIdle;
class MCALayoutChangeObserver;
class CCAGroupUtils;
class CCAUISessionManager;
class CCAIconLoader;
class CCAMessageExtensionsHandler;
class MCAOpBrandVariant;
class MCAServerChangeNotify;
class MCANotifierInterface;
class MCALayoutInfo;
class CCALayoutInfo;

class CWVSettingsUIDialog;
class MCAProcessManager;

class TAknLayoutScalableParameterLimits;
class TAknWindowComponentLayout;

class MCAConversationMessage;
class MCAApplicationNGPC;
class MCAGroupManagerInterface;

// CLASS DECLARATION

/**
 *  Eikon application architectures AppUi class
 *
 *  @lib chatng.app
 *  @since 3.2
 */
class CCAAppUi : public CAknViewAppUi,
            public MCAAppUi,
            public MCAViewSwitcher,
            public MProgressDialogCallback,
            public MAknTabObserver,
            public MCAUiGlobalNotificationObserverPC,
            public MWVSettingsCASettings,
            public MWVSettingsObserver,
            public MAknInfoPopupNoteObserver,
            public MIMPSSAPObserver,
            public MCABackGroundTaskObserver,
            public MCASplashScreenObserver

    {
    public: // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aEngine Reference to application model (engine)
         */
        CCAAppUi( CCAEngine& aEngine, MCAProcessManager& aProcessManager );

        /**
         * Destructor.
         */
        virtual ~CCAAppUi();

    public: // New functions
    	
        class CKludgeScheduler : public CActiveScheduler
            {
            public:
                 TInt PublicLevel() const;
            };
        //Get the level of current active scheduler
        TInt LevelOfActiveScheduler();
        
        /**
        * Handle to IM Connection UI
        * @since 3.2
        * @return Handle to im connection ui
        */
        CIMPSPresenceConnectionUi& ConnectionUI();

        /**
        * Handle to UI session manager
        * @since 2.1
        * @return Handle to ui session manager
        */
        CCAUISessionManager& UISessionManager();

        /**
         * Fetches a handle to status pane handler
         * @return handle to status pane handler (CCAStatusPaneHandler*)
         */
        CCAStatusPaneHandler* CAStatusPane() const;

        /**
         * Fetches a handle to application's process manager
         * @see MCAProcessManager
         * @Returns a reference to process manager
         */

        MCAProcessManager& GetProcessManager() const;

        /**
         * Gets a pointer to screenname buffer containing
         * KScreenNameMaxLength of editable space
         * @return Descriptor that contains screen name
         */
        HBufC* ScreenNamePtr();

        /**
         * Clears Universal Indication Pane's IM-icon
         */
        void IMUIPIConForcedClear();

        /**
         * Sets new counter value to IM's soft notification
         * @param aCount new count of unread messages
         */
        void SetSoftNotificationCounterL( TInt aCount ) const;

        /**
         * Plays default beep
         */
        void PlayBeep();

        /**
         * Capture events if needed
         */
        void CaptureEventsL();

        /**
         * Releases events capturing
         */
        void ReleaseCapturingL();

        /**
         * Returns if events are captured
         * @return ETrue if captured
         */
        TBool AreEventsCaptured() const;

        /**
         * Handles the actual invitation response
         * @param aAcceptance ETrue if accepted
         * @param aUserId Descriptor that contains user id
         * @param aGroupName Descriptor that contains group name
         * @param aResponse Descriptor that contains response message
         */
        void DoHandleInvitationResponseL( TBool aAcceptance,
                                          const TDesC& aUserId,
                                          const TDesC& aGroupName,
                                          const TDesC& aResponse );

        /**
         * Returns the number of received messages (invitations + p2pmessages)
         * since last call to ResetMsgCount
         * @return received messages count
         */
        TInt ReceivedInvites() const;

        /**
         * Returns the instance of CCAGroupUtils
         * @return pointer to group utility class's instance
         */
        CCAGroupUtils* GroupUtils();

        /**
         * Sets the forwarded message. This method will not take the
         * ownership of the passed message, but will create a clone
         * of it. So it doesn't matter if the actual message gets deleted.
         * @param aFwdMessage forwarded message
         */
        void SetForwardMessageL( const MCAConversationMessage* aFwdMessage );

        /**
         * Gets the forwarded message or NULL
         * @return forwarded message
         */
        const MCAConversationMessage* ForwardMessage();

        /**
         * Shows wait dialog. Remember to call DismissWaitDialogL after this!
         * !! Remember to TRAP everything between ShowWaitDialogL and
         * DismissWaitDialogL and give the error code as parameter to
         * DismissWaitDialogL !!
         * @param aTextResource Resource ID of text to be displayed
         * @param aShowImmediately Should the dialog be shown immediately
         */
        void ShowWaitDialogL( TInt aTextResource, TBool aShowImmediately = EFalse );

        /**
         * Shows wait dialog. Remember to call CleanupStack::Pop and DismissWaitDialogL
         * after this! Call CleanupStack::Pop first. Leaves one item in cleanup stack.
         * @param aTextResource Resource ID of text to be displayed
         * @param aShowImmediately Should the dialog be shown immediately
         */
        void ShowWaitDialogLC( TInt aTextResource, TBool aShowImmediately = EFalse );

        /**
         * Overridden version that takes text descriptor as parameter
         * @param aText Prompt text
         * @param aShowImmediately Should the dialog be shown immediately
         */
        void ShowWaitDialogL( const TDesC& aText, TBool aShowImmediately = EFalse );

        /**
         * Overridden version that takes text descriptor as parameter and places one item
         * in cleanup stack. Remember to call CleanupStack::Pop and DismissWaitDialogL
         * after this! Call CleanupStack::Pop first.
         * @param aText Prompt text
         * @param aShowImmediately Should the dialog be shown immediately
         */
        void ShowWaitDialogLC( const TDesC& aText, TBool aShowImmediately = EFalse );

        /**
         * Dismisses the wait dialog. Shows also error note on imps errors.
         * @param aErrorCode Error code. Leaves on system error codes
         *        (greater than Imps_ERROR_BASE) after deleting the dialog.
         * @param aWaitUntilDismissed Should we wait until the dialog is gone
         * @param aAdditionalData Additional data for the dialog
         */
        void DismissWaitDialogL( TInt aErrorCode, TBool aWaitUntilDismissed = EFalse,
                                 const TDesC& aAdditionalData = KNullDesC );

        /**
         * Returns icon loader
         * @since 2.1
         * @return Reference to icon loader
         */
        CCAIconLoader& IconLoader() const;

        /**
         * Returns message handler
         * @since 2.1
         * @return Reference to message handler
         */
        CCAMessageExtensionsHandler& MessageHandler() const;

        /**
         * Modify ID for display. From userid, groupid "wv:" is hidden.
         * From list id "wv:userid" is hidden.
         * @param aId The id to modify
         * @param aListHiding Modify id for list hiding
         * @return The id with some parts hidden
         */
        static TPtrC DisplayId( const TDesC& aId, TBool aListHiding = EFalse );

        /**
         * Read the branded resources and set appropriate flags.
         * Done during construction and SAP change event.
         * Reads:
         *   - the wv: hiding setting.
         * @since 2.1
         */
        void ReadBrandedResourcesL();

        /**
         * Sets the refresh flag
         */
        void SetRefreshed();

        /**
         * @return ETrue if the "refresh list"-command has been used
         */
        TBool ListRefreshed();

        /**
         * Changes the resource of connection UI agent.
         * @param aNewResource the resource to be used
         */
        void ChangeConnUiAgentResourceL( const TDesC& aNewResource );

        /**
         * Returns view which should be called after refreshView.
         * @param aPrevious This is used normally. Override if needed.
         */
        TUid AfterRefreshViewId( const TUid& aPrevious );

        /**
         * Set mode of after refresh.
         * @param aMode True, use default, EFalse switch to MainView after refrehs.
         */
        void SetAfterRefreshMode( TBool aMode );

        /**
         * @return a MCALayoutInfo pointer
         */
        MCALayoutInfo* LayoutInfo();

        /**
         * To get information of refrheswaitflag.
         * @return reference to refresh wait flag.
         */
        TInt& RefreshWaitFlag();


        /**
        * Returns a flag which informs about the state of appui.
        * Either it's fully functional or being destroyed.
        * @return ETrue if appui is being destroyed
        */
        TBool IsUnderDestruction();

        /**
         * Initialize observers to the engine.
         * @since 3.0
         */
        void InitializeEngineObserversL();

        /**
         * Finalize construction of chat
         * @since 3.0
         */
        void FinalizeChatConstructionL();

        /**
         * Sets new message beep not to be played next time
         * when playing is requested.
         * @since 3.1
         * @param aPlayOnce ETrue if beep allready played, EFalse otherwise.
         */
        void SetPlayOnce( TBool aPlayOnce );

        /**
         * Launches server settings dialog
         * @since 3.1u
         */
        void LaunchServerSettingsDialogL();

        /**
         * Notify resource observers.
         * This should be called whenever resources are changed.
         * @since 3.0
         */
        void NotifyResourceChangeL();

#ifdef RD_CHAT_GROUP_MESSAGE_INDICATION_NEW
        /**
         * Returns construction state.
         * @since S60 v3.2
         * @return ETrue if construction done, EFalse otherwise.
         */
        TBool IsFullyConstructed() const;
#endif  // RD_CHAT_GROUP_MESSAGE_INDICATION_NEW

#ifdef RD_SETTINGS_FACELIFT
        /**
         * Is settings dialog active or not.
         * @since S60 v3.2
         * @return ETrue if iServSettingsDialog != NULL
         */
        TBool IsSettingsDialogActive() const;
#endif  // RD_SETTINGS_FACELIFT

        /**
         * Call this when asynchronously launched dialog has returned
         * @see ReportPendingDialog
         * @since 3.1
         */
        void ReportPendingDialogClosed();

        /**
         * When launching a dialog asynchronously, this method should be
         * called, so that AppUI knows to give dialog time to return.
         * If you call this, you MUST call ReportPendingDialogClosed when
         * the dialog is closed. Otherwise the AppUI hangs on exit!
         * @since 3.1
         */
        void ReportPendingDialog();
        /**
         * Sets if forward once.
         * ReSets if forward is more than once..
         */
        void SetResetForwardTo( TEnumsPC::TForwardStatus aForward );

        /**
         *  Return iForward.
         */
        TEnumsPC::TForwardStatus RetForwardTo( );

        /**
         * Unregisters the previous view registered
         * before forwarding.
         */
        void UnRegisterPreviousview();

#ifdef RD_MULTIPLE_DRIVE
        /**
         * Gets the status of the drive.
         *	@since 2.1
         *  @param aDriveNumber: the drive to be checked.
         *	@return ETrue if accessable else EFalse.
         */
        TBool GetDriveStatusL( const TDriveNumber aDriveNumber );

#endif
        // Get the value of iStopUpdateCba
        TBool GetStopUpdateCba();

        // Get the value of iIsFSWExit
        TBool  GetIsFSWExitFlag();
        // Set the value of iIsFSWExit
        void SetFSWExitFlag( TBool aValue );

    private: // From MWVSettingsCASettings
        /**
        * @see MWVSettingsCASettings
        */
        TInt LaunchSettingsL ( CIMPSSAPSettings& aServer );

    private: // From MWVSettingsObserver

        /**
         * @see MWVSettingsObserver
         */
        void ServerChangedL ( const TDesC& aNewServerName,
                              TBool aDefaultServerChanged  = EFalse );

        /**
         * @see MWVSettingsUICaller
         */
        HBufC* CurrentServerNameLC();

    public: // From MCAAppUi

        /**
         * @see MCAAppUi
         */
        TDes& MbmFullPath();

        /**
         * @see MCAAppUi
         */
        void AddLayoutChangeObserver( MCALayoutChangeObserver* aObserver );

        /**
         * @see MCAAppUi
         */
        void RemoveLayoutChangeObserver( MCALayoutChangeObserver* aObserver );

        /**
         * @see MCAAppUi
         */
        void AddResourceChangeObserver( MCAResourceChangeObserver* aObserver );

        /**
         * @see MCAAppUi
         */
        void RemoveResourceChangeObserver( MCAResourceChangeObserver* aObserver );


    private: // From MProgressDialogCallback

        /**
         * From MProgressDialogCallback Get's called when a dialog is dismissed
         * @param aButtonId Button that was used to dismiss the dialog
         */
        void DialogDismissedL( TInt aButtonId );

    private:
        //popup pane related functionss.
        void AddToPreviewListL( const TDesC& aContactId, const TDesC& aMsg );

        void CalculatePreviewPaneWidth();

        TInt SelectWindowVariety( const TInt aNumberOfLines,
                                  const TAknLayoutScalableParameterLimits& aLimits ) const;

        TRect RectFromLayout( const TRect& aParent,
                              const TAknWindowComponentLayout& aComponentLayout ) const;


    public: // Functions from base classes

        /**
         * From CEikAppUi, takes care of command handling.
         * @param aCommand command to be handled
         */
        void HandleCommandL( TInt aCommand );

        /**
         * From MCAViewSwitcher, handles view-switch requests
         * @param aViewId View to be switched
         */
        void SwitchViewL( TUid aViewId );

        /**
         * From MCAViewSwitcher, handles view-switch requests
         * @param aViewId View to be switched
         * @param aCustomMessageId Passed messages id
         * @param aCustomMessage Message to be passed
         */
        void SwitchViewL( TUid aViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
         * From MCAViewSwitcher, handles view-switch requests
         */
        void SwitchViewBackL( TUid aViewId, TUid aCustomMessageId = KUidMainView,
                              const TDesC8& aCustomMessage = KNullDesC8 );

        /**
         * From MCAViewSwitcher, handles view-switch requests
         */
        void RegisterSwitchBack( TUid aViewId,
                                 TUid aCustomMessageId,
                                 HBufC8* aCustomMessage,
                                 TUid aNextView );

        /**
         * From MCAViewSwitcher, deregisters switch back info
         */
        void DeRegisterSwitchBack( TUid aViewId,
                                   const TDesC& aTarget );

        /**
         * From MCAViewSwitcher, checks if view exists
         * @see MCAViewSwitcher
         * @since 2.1
         */
        TBool ViewExists( TUid aViewId );

        /**
         * From MCAViewSwitcher
         * @return Active view's id
         */
        TUid ActiveViewId();

        /**
         * From CEikAppui, Handles a change to the application's resources
         * which are shared across the environment
         * @param aType The type of resources that have changed
         */
        void HandleResourceChangeL( TInt aType );

        /**
         * From CAknAppUi, Handles window server events.
         * aEvent Event to be handled
         * aDestination Destination of event.
         */
        void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );

        /**
         * Show info popup note
         */
        void ShowInfoPopupL();

    private:
        /**
         * Handles events reported from info popup note.
         * @see MAknInfoPopupNoteObserver
         */
        void HandleInfoPopupNoteEvent(
            CAknInfoPopupNoteController* aController,
            TAknInfoPopupNoteEvent aEvent );

    private: // From MAknTabObserver

        /**
         * @see MAknTabObserver
         */
        void TabChangedL( TInt aIndex );

    private: // From CAknViewAppUi

        /**
         * From CAknViewAppUi
         * @see CAknViewAppUi
         */
        void HandleForegroundEventL( TBool aForeground );

    private:  // From MCAUiGlobalNotificationObserverPC
        /**
        * @see MCAUiGlobalNotificationObserverPC
        */
        void HandleDelete( const TDesC& aContactId , TEnumsPC::TItem type ) ;

        /**
        * @see MCAUiGlobalNotificationObserverPC
        */
        void HandleAddition( TEnumsPC::TItem type ) ;

        /**
        * @see MCAUiGlobalNotificationObserverPC
        */
        void HandleChange( TEnumsPC::TItem type, TEnumsPC::TChange changeType ) ;

        /**
        * @see MCAUiGlobalNotificationObserverPC
        */
        void HandleInvitationEventL( TEnumsPC::TInviteType aInviteType,
                                     const TDesC& aUserID,
                                     const TDesC& aMessage ) ;

        /**
        * @see MCAUiGlobalNotificationObserverPC
        */
        void HandleInvitationResponse( TBool aAcceptance,
                                       const TDesC& aUserId,
                                       const TDesC& aGroupName,
                                       const TDesC& aResponse ) ;
        /**
        * Handles setting changes.
        * @see MCAUiGlobalNotificationObserverPC
        * @param aChangedSettingEnum specifies the changed setting.
        */
        void HandleSettingsChangeL( TInt aChangedSettingEnum );

        /**
        * From MCAUiGlobalNotificationObserverPC
        */
        void HandleChatEvent( const TEnumsPC::TChatEventType aEvent,
                              const TDesC& aWvid,
                              const TDesC& aContactId,
                              const TDesC& aMessage,
                              const TEnumsPC::TMessagerType aMsgrType,
                              const TEnumsPC::TMessageType aMsgType );

        /**
        * From MCAUiGlobalNotificationObserverPC
        */
        void HandleMessageError( TInt aError, const TDesC& aInfo );




    private: // New functions

        /**
         * Symbian OS default constructor.
         */
        void ConstructL();

        /**
         * Solves resource file paths.
         * @param aPathWithoutDrive, a resource file path without drive letter.
         * @return HBufC*, allocated + added on cleanup stack.
         */
        HBufC* SolveResPathLC( const TDesC& aPathWithoutDrive ) const;

        /**
         * Real implementation of waiting for engine to shutdown
         * @return Need for recall
         */
        TInt DoExitCallback();

        /**
         * CIdle-kind of function for waiting for engine to shutdown
         * @param aInstance Instance of this class
         * @return Need for recall
         */
        static TInt ExitCallback( TAny* aInstance );

        /**
         * This method closes wait dialog. This happens when wait dialog is running
         * and leave occurred.
         * Pushed into CleanupStack. Used with TCleanupItem.
         * This handles leaves with wait dialog by closing wait dialog if a leave occurs.
         * Wait dialog in CleanupStack, example usage:
         * ShowWaitDialogL( _L("Test"), ETrue );
         * CleanupStack::PushL( TCleanupItem( CCAAppUi::CloseDialog, this) ); // this = CCAAppUi pointer
         * User::Leave( KErrDiskFull );
         * @param aInstance Instance of this class
         */
        static void CloseDialog( TAny* aInstance );

        /**
        * Retrieves branding settings and changed UI brand
        * @since 2.1
        * @param aBrand Brand variant
        */
        void RetrieveBrandL( MCAOpBrandVariant& aBrand );

        /**
         * Initializes UIP icon handling
         * @since 2.1
         */
        void InitializeUIPHandlingL();

        /**
         * Update icon if it is needed. Disable it if not.
         */
        void UpdateIMUIPIcon();

        /**
         * Update soft note if it is needed. Disable it if not.
         *
         */
        void UpdateSoftNote( TInt aUnreadCount );

        /**
         * Start process for changing status
         */
        void ChangeStatusProcessL();

        /**
         * Changes current status message
         * @param aStatus Status which message is needed.
         * @param aUpdateToNetwork ETrue update, EFalse do not update.
         * @return KErrCancel if canceled.
         */
        TInt AskStatusMessageL( TEnumsPC::TOnlineStatus aStatus,
                                TBool aUpdateToNetwork );

        /**
         * Get status old status messages from file.
         * @param aStatus Status which message is needed.
         * @param aArray. Array of messages. Don't clear before filling.
         */
        void ReadStatusMessagesL( TEnumsPC::TOnlineStatus aStatus,
                                  CDesCArray& aArray );

        /**
         * Write status messages to file.
         * @param aStatus Status which messages are written
         * @param aArray. Array of messages to write. Newest in top.
         *                Do not write duplicates.
         */
        void WriteStatusMessagesL( TEnumsPC::TOnlineStatus aStatus,
                                   MDesCArray& aArray );

        /**
         * Maps TEnumsPC::TOnlineStatus to TEnumsPC::TCASettingStrings,
         * which stores the status messages.
         * @param aStatus Online state which is mapped to correct setting
         *                string
         * @return Setting string value of the given online status.
         */
        TEnumsPC::TCASettingStrings ConvertToTEnumsStatusMsgValue(
            TEnumsPC::TOnlineStatus aStatus );
        /**
         * Shows information note about low memory situation.
         * @since S60 v3.2
         */
        void ShowMemLowNoteL();

        /**
         * Get default SAP Uid
         * @return SAP Uid
         */
        TUint32 DefaultSAPUid();
        /**
        * Shows Splash Screen.
        * @since S60 v3.2
        */
        void ShowSplashScreenL( MCASkinVariant& aSkinVariant );



    private: // Functions from base classes

        /**
         * From CEikAppUi, handles key events.
         * @param aKeyEvent Event to handled.
         * @param aType Type of the key event.
         * @return Reponse code (EKeyWasConsumed, EKeyWasNotConsumed).
         */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent, TEventCode aType );

    public:

        /**
         * Tab keys are fwded to this function
         * @param aKeyEvent Event to handled.
         * @param aType Type of the key event.
         * @return Reponse code (EKeyWasConsumed, EKeyWasNotConsumed).
         */
        TKeyResponse OfferTabEventL( const TKeyEvent&
                                     aKeyEvent, TEventCode aType );

    public:
        /**
        * The observer is added to get the server change callback
        * .
        */
        void AddServerChangeObsL( MCAServerChangeNotify* aObserver );

        /**
         * The observer is removed by using this method
         * .
         */
        void RemoveServerChangeObs( MCAServerChangeNotify* aObserver );
        /**
        * From MIMPSSAPObserver
        * @see MIMPSSAPObserver for more information
        */
        void HandleSAPEvent( TSAPEvent aEvent );
        /**
         * Sets th focus Flag : Used for Tab switching
         */
        void SetFocusFlag( TBool aFlag );
        /**
         * returns value of focus flag : Used for tab switching
         */
        TBool FocusFlag();

        // from MCABackGroundTaskObserver
        void HandleBackGroundTaskCompleteL( TBool aCompleted );

        // from MCASplashScreenObserver
        void SplashScreenShowed();

        /**
         * From IsQueryAccepted
         * @return ETrue if exit query accepted or EFalse
         */
        TBool IsQueryAccepted();

    private :

        /**
         * To know whether Background task has started
         * @return TBool
         */
        TBool IsBackgroundTaskPendingL();

        /**
         * WaitToCompleteBackgroundTaskL
         * Pop the wait note processing
         * @return ETrue
         */
        TBool WaitToCompleteBackgroundTaskL();


    private: //Data

        // Owns. Shared data is used for Universal Indicator Pane changes
        MCANotifierInterface* iNotifierAPI;

        // Owns. Handle to status pane handler
        CCAStatusPaneHandler* iStatusPaneHandler;

        // Doesn't own. Handle to application engine
        CCAEngine& iEngine;

        //reference to the app ui process component
        MCAApplicationNGPC& iApplicationNGPC;


        // invitation active or not
        TBool iInviteActive;

        // we are foreground or not
        TBool iIsAppForeground;

        //Owns. Audiomanager for playing sounds
        CCAAudioManager* iAudioManager;

        // Owns. holds the current screenname
        HBufC* iScreenName;

        //Owns. Capture events when active
        CCAInputAbsorber* iInputAbsorber;

        //Stores active view id
        TUid iActiveViewId;

        // Owns
        CIdle* iExitIdle;

        // helper arrays to count if UIP icon should shown or not
        CDesCArrayFlat* iUIPInvitations;   // owns
        CDesCArrayFlat* iUIPConversations; // owns

        TFileName		iChatMbmFullPath;
        TInt			iResourceFileOffset;
        RPointerArray<MCALayoutChangeObserver> iLayoutObservers;

        // Doesn't own objects. Array of resource observers
        RPointerArray<MCAResourceChangeObserver> iResourceObservers;

        // Owned. Clone of the forwarded message.
        const MCAConversationMessage* iForwardMessage;

        // Owns.
        CCAGroupUtils* iGroupUtils;

        // Doesn't own (destroys itself)
        CAknWaitDialog* iWaitDialog;
        CActiveSchedulerWait iWait;
        TBool iWaitDialogDismissed;

        // Owns. UI session manager
        CCAUISessionManager* iUISessionManager;

        // Owns. Icon loader
        CCAIconLoader* iIconLoader;

        // Owns. Message handler
        CCAMessageExtensionsHandler* iMessageHandler;

        // Owns. Domain part of current server
        HBufC* iServerDomain;

        // view switcher custom messages
        TUid iCustomMessageId;
        HBufC8* iCustomMessage;
        TBool iOkToSwitch;

        // Back-support
        TUid iLastView;
        RArray<TUid> iSwitchView;
        RArray<TUid> iSwitchMessage;
        RPointerArray<HBufC8> iSwitchMessageContent;

        // Owns. Connection UI agent
        CIMPSConnectionUiAgent* iConnUIAgent;

        // The splash screen showed at the startup
        CCASplashScreen* iSplashScreen;


        TBool iRefreshed;

        // Override mode can be set with this for view switch after refreshView.
        TBool iDefaultRefreshInUse;

        ///
        CCALayoutInfo* iLayoutInfo;

        // Owns. Status message ptr to original message
        HBufC* iStatusMessage;

        /// Flag to inform if refresh is happening or not.
        TBool iRefreshWaitFlag;

        // Flag to inform that appui is being destroyed
        TBool iDestructingFlag;

        // ETrue when engine has been properly initialized
        TBool iEngineInitialized;

        // ETrue when chat has been fully constructed
        TBool iChatFullyConstructed;

        // ETrue if beep allready played
        TBool iPlayOnce;

        // SAP settings store
        CIMPSSAPSettingsStore* iSAPSettingsStore;

        // Doesn't own (destroys itself).
        CWVSettingsUIDialog* iServSettingsDialog;

        TBool iNeedRefresh;

        // How many times has the message been forwarded
        TInt iForwardCount;

        // If ETrue, last forward was to contact
        // if EFalse, to group
        TBool iForwardedToContact;

        // Not owned, destroyed in RunLD of dialog
        CEikDialog* iMemLowNote;

        // Flag is read from CR, ETrue in releases >= 3.2
        // EFalse in releases < 3.2
        TBool iShowMemLowNote;

        // Flag is read from CR in construction
        // @see IMVariant.hrh
        // ETrue if feature EIMFeatBrandableTones is
        // variated on, EFalse otherwise.
        TBool iBrandableToneList;

        // How many async dialogs are open.
        TInt iPendingDialogCounter;

        //Reference to appln's process manager
        MCAProcessManager& iProcessManager;

        /// Owns - Popup info for showing info popupnote.
        CAknInfoPopupNoteController* iInfoPopup;

        //ETrue - denotes that the popup pane is active
        TBool iInfoNoteShowing;

        //owns - holds the array of messages to be shown in the preview pane
        CDesCArray* iPreviewMessageList;

        //holds the length of the message in the preview pane
        TInt iTotalPreviewMsgLength;

        //holds the preview pane's Max no. of characters tat can be visible
        TRect iPreviewPaneRect;

        //holds the current font - doesnt own
        CFont* iCurrentFont;

        RPointerArray<MCAServerChangeNotify> iServerChangeNotify;

        // used for tab swithching
        TBool iFocusFlag;

        // background task complete or not
        TBool iBackgroundTaskPending;

        // processing note is visible
        TBool iWaitVisible;
        // owned queryaccepted for exit
        TBool iQueryAccepted;

        //Forward once(ETrue) else EFlase;
        TEnumsPC::TForwardStatus iForward;

        // Owns. Connection UI
        CIMPSPresenceConnectionUi* iConnUI;

        //Maintaining count to Unread messages.
        TInt iUnreadMsgCount;

        //Maintain record if exit is required after back ground processing completed.
        //because of Fast swaping window exit.
        TBool iIsFSDExitNeeded;

        // An indicator to decide whether to update current cba
        TBool iStopUpdateCba;

        TBool iLaunchSettingsDialogWait;

        TBool iIsFSWExit;
        MCAGroupManagerInterface* iGroupManagerInterface;
        TBool iStatusFlag;
    };

#endif // CCAAPPUI_H

// End of File
