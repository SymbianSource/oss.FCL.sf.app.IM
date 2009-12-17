/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This interface is used by UI to access network services i.e.
*                sending messages and joining to groups e.g. This class is
*                also used to get different interfaces to the services
*                offered by the CCAEngine.dll.
*
*/


#ifndef CCAENGINE_H
#define CCAENGINE_H

//  INCLUDES
#include    "PublicEngineDefinitions.h"
#include    "MCASettingsObserver.h"
#include    "MCASearchObserverInterface.h"
#include    "MCABackgroundTask.h"
#include    "MCABackgroundObserver.h"
#include    "MCAMessageUtils.h"
#include 	"MCABackgroundTaskObserver.h"

#include    <e32base.h>
#include    <ImpsClient.h>
#include    <ImpsFundamental.h>
#include    <PEngWVServices2.h>

// FORWARD DECLARATIONS
class   CCAGroupManager;
class   MCAPresence;
class   CCASearchManager;
class   CCABlockingManager;
class   CCASettingsManager;
class   CCAInviteManager;
class   CCAPartialSuccessData;
class	CCARequestMapper;
class   MCAError;
class   MCASearchInterface;
class   MCAPresence;
class   MCAInvite;
class   MCABlocking;
class   MCASettings;
class	MCAImpsImClient;
class	MCAImpsGroupClient;
class	MCAImpsFundClient;
class   MCAStoredGroups;
class   MCAGroupManagerInterface;
class	MCABackgroundInterface;
class	CCAStorageObserver;
class	TCALoggerMessageFactory;
class	MCALoggerHeadersInterface;
class   CCAImageLoader;
class   CCAContactListModel;

class   MCAChatInterface;
class   CCAChatContainer;
class   CCAMessageHandler;
class   CCAMessageCreator;

class   CPEngNWSessionSlotID2;
class   MCAAccessInterface;
class   CCAAccessManager;
class   MCAContactListModel;

class 	MCABackgroundTaskObserver;
struct SServerPrefers;

/**
 *  CCAEngine provides one of the interfaces to the engine.
 *  This interface is used by UI to access network services i.e.
 *  sending messages and joining to groups e.g. This class is
 *  also used to get different interfaces to the services
 *  offered by the CCAEngine.dll.
 *
 *  @lib CAEngine.lib
 *  @since 1.2
 */
class CCAEngine : public CBase,
            public MCABackgroundTask,
            public MCABackgroundObserver,
            public MCAMessageUtils,
            private MImpsErrorHandler2,
            private MCASettingsObserver,
            private MCASearchObserverInterface
    {
    public: // Structures and enumerations

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aAppUid is the application's UID.
         */
        IMPORT_C static CCAEngine* NewL( TUid aAppUid );

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CCAEngine();

    public: // Settings related functions

        /**
         * Gets internal settings handling interface.
         * @return Pointer to internal settings handler.
         */
        IMPORT_C MCASettings* GetInternalSettingsInterface() const;

    public: // Search related functions

        /**
         * Gets search handling interface.
         * @return Pointer to the search handler.
         */
        IMPORT_C MCASearchInterface* GetSearchInterface() const;

    public: // Invite related functions

        /**
         * Get invite related functions.
         * @return Pointer to the invite interface.
         */
        IMPORT_C MCAInvite* GetInvitationInterface() const;

    public: // Blocking related functions

        /**
         * Get blocking handling interface.
         * @return Pointer to the blocking interface.
         */
        IMPORT_C MCABlocking* GetBlockingInterface() const;

    public: // Imps fund client accessor

        /**
         * Get Imps fundamental client
         * @return Pointer to the Imps fundamental client
         */
        IMPORT_C MCAImpsFundClient* GetFundClientInterface() const;

    public: // Access interface

        /**
         * Get Imps access interface
         */
        IMPORT_C MCAAccessInterface* GetAccessInterface() const;

    public:

        /**
         * Get interface for contact list model
         */
        IMPORT_C MCAContactListModel& ContactListModel() const;


        /**
         * Get interface for message handling.
         */
        IMPORT_C MCAChatInterface& ChatInterface() const;

        /**
         * Get interface to message utils like messagecreator etc.
         */
        IMPORT_C const MCAMessageUtils& MessageUtils() const;

    public: // Database interfaces

        /**
         * Get group interface.
         * @return Pointer to the group interface.
         */
        IMPORT_C MCAGroupManagerInterface* GetGroupInterface() const;


    public: // Background specific functions

        /**
        * Get Background interface.
        * @return Pointer to the Background interface.
        */
        IMPORT_C MCABackgroundInterface* BackgroundInterface() const;

    public: // From MCABackgroundTask

        /**
         * Handler for the background task event
         * @see MCABackgroundTask
         */
        void HandleBackgroundTaskL( TInt aSubTask );

    public: // From MCABackgroundObserver
        /**
         * Observers background task events
         * @see MCABackgroudObserver::HandleBackgroundEvent
         */
        void HandleBackgroundEvent(
            MCABackgroundInterface::TCABackgroundTasks aEventSource,
            MCABackgroundInterface::TCABackgroundStatus aStatus,
            TInt aSubTaskNumber,
            TInt aLeaveCode
        );

    private: // From MCAMessageUtils

        /**
         * @see MCAMessageUtils
         */
        MCAMessageCreator& MessageCreator() const;

        /**
         * @see MCAMessageUtils
         */
        MCAMessageErrorInformer& MessageErrorInformer() const;

        /**
         * @see MCAMessageUtils
         */
        MCAContentProcessor& ImageScaler() const;

        /**
         * @see MCAMessageUtils
         */
        MCABufferMemoryHandler& MemoryHandler() const;

    public: // New functions

        /**
         * Handles engine logic in login/logout events.
         * @param aLoggedIn If new state is online.
         * @param aNewUSerID specifies the new wv id.
         * @param aServerPrefers. Some preferred states like enable/disable
         *                        grantlist etc.
         */
        IMPORT_C void HandleNetworkStateChangeL(
            TNetworkState  aState,
            TPEngWVCspServicesTree2* aServicesSupported,
            const SServerPrefers& aServerPrefers,
            CPEngNWSessionSlotID2* aSessionSlotID = NULL );

        /**
         * Returns the latest operations partially successful error codes.
         * @return Holds an array of error codes.
         */
        IMPORT_C const RPointerArray<MCAError>* PartialSuccessData() const;

        /**
         * Checks CAEngine if its ready for shutdown.
         * @return ETrue if ready.
         */
        IMPORT_C TBool ReadyForShutdown();

        /**
         * Cancels pending requests in request mapper.
         */
        IMPORT_C void CancelAllRequests();

        /**
         *	Get logger headers interface
         *	@return pointer to logger header access.
         */
        IMPORT_C MCALoggerHeadersInterface* GetLoggerHeadersInterface();

        /**
         * Finalize the engine construction. It's done in two phases
         * for performance reasons. We need to delay the initialization
         * of IMPS Engine until after the login query, to appear fast.
         */
        IMPORT_C void FinalizeEngineConstructionL();

        /**
         * Check if engine is fully finalized
         * This should be checked before accessing the interfaces
         */
        IMPORT_C TBool IsFinalized();

        /**
         * Register to receive notifications related to
         * start and completetion of background task
         * @param MCABackGroundTaskObserver- object
         */
        IMPORT_C void RegisterBackGroundTaskObserver( MCABackGroundTaskObserver* aObserver );

        /**
         * UnRegister from receiving notifications related to
         * start and completetion of backgroundtask
         */
        IMPORT_C void UnRegisterBackGroundTaskObserver( MCABackGroundTaskObserver* aObserver );

        /**
         * To know whether Background task has started
         * @return TBool
         */
        IMPORT_C TBool IsBackgroundTaskPending();



        IMPORT_C void ResetLoggers();


    private: // Functions from MImpsErrorHandler

        /**
         * From MImpsErrorHandler.
         * @see MImpsErrorHandler::HandleErrorL
         */
        void HandleErrorL( TInt aStatus,
                           TInt aOpId,
                           const TDesC* aDescription,
                           const CImpsDetailed* aDetailedRes,
                           TImpsCspIdentifier& aCspId );

    private: // From MCASettingsObserver

        /**
         * Handles setting changes.
         * @param aChangedSettingEnum specifies the changed setting.
         */
        void HandleSettingsChangeL( TInt aChangedSettingEnum );

    private: // from MCASearchObserverInterface

        /**
         * @see MCASearchObserverInterface::HandleSearchError
         */
        void HandleSearchError( TInt aErrorCode );

        /**
         * @see MCASearchObserverInterface::HandleSearchFinished
         */
        void HandleSearchFinished();

    private: // new methods


        /**
         * Notifies the observers of group synchronisation
         * @param aCompleted - ETrue if group synchro is complete else EFalse
         */
        //void NotifyGroupSyncObservers(MCABackgroundTaskObserver::TCAGroupSyncEventType aCompleted)	;
        /**
         * Prepares and starts the synchronizing process if search
         * service available.
         * @param aServicesSupported The supported imps services
         */
        void SynchronizeGroupsL( TPEngWVCspServicesTree2* aServicesSupported );

        /**
         * Handler for finished searches
         */
        void DoHandleSearchFinishedL();

        /**
         * Synchronize the storage with the groups created by the user
         * (in this server). This method is asynchronous, although there
         * is a small delay when calling the initial search.
         */
        void StartSynchronizeGroupsL();

        /**
         * Stop the synchronization of groups created by the user
         * (in this server). Use this to force stop the StartSynchronizeGroupsL.
         */
        void StopSynchronizeGroupsL();

    private:

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

        /**
         * C++ default constructor.
         * @see CCAEngine::NewL.
         */
        CCAEngine( TUid aAppUid );

    private:    // Data

        // Owns (unless otherwise noted)///////////////////////////////////////
        MCAPresence*            iPresenceManager;
        CCASettingsManager*	    iSettingsManager;
        MCAImpsImClient*        iImpsImServer;
        MCAImpsGroupClient*     iImpsGroupServer;
        MCAImpsFundClient*	    iImpsFundAPI;
        CCASearchManager*       iSearchManager;
        CCABlockingManager*	    iBlockingManager;
        CCAInviteManager*       iInviteManager;
        CCARequestMapper*	    iRequestMapper;
        CCAStorageObserver*		iStorageObserver;
        CCAGroupManager*        iGroupManager;
        CCAAccessManager*       iAccessManager;

        CCAChatContainer*       iChatContainer;
        CCAMessageHandler*      iMessageHandler;
        CCAMessageCreator*      iMessageCreator;
        CCAImageLoader*         iImageLoader;
        CCAContactListModel*    iContactListModel;
        // ////////////////////////////////////////////////////////////////////

        // not owned (from Storage Manager)
        MCAStoredGroups*        iStoredGroups;

        // Application UID received from the Application UI
        TUid iAppUid;

        // Owns
        CCAPartialSuccessData* iPartialSuccessData;

        // group synchronizing
        TInt                    iSearchResults;
        HBufC*                  iLoggedUserId;   // owns
        CSearchPairs*           iPairs;   // owns
        CImpsSearchRequest*     iRequest; // owns

        // protection from bogus network state changes, finite state machine
        // logic is inside HandleNetworkStateChangeL
        TNetworkState iState;

        // This is passed to MCALoggerReadInterface when instance called.
        // Pointer to T-class is used so that we do not have to include
        // header of TCALoggerMessageFactory in this header file.
        TCALoggerMessageFactory* iLoggerMessageFactory;

        //Interface to access  logger headers. Owns
        MCALoggerHeadersInterface* iLoggerHeadersInterface;

        // ETrue when the engine has been fully constructed
        TBool iFullyConstructed;

        TBool iGroupSynchronizing;


    };

#endif      // CCAEngine_H

// End of File
