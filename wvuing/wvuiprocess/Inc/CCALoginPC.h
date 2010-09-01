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
* Description:  Process Component for Login,Logout
*
*/



#ifndef CCALOGINPC_H
#define CCALOGINPC_H


//  INCLUDES
#include <e32base.h>
#include "MCALoginPC.h"
#include "MCABackgroundTaskObserver.h"
#include <cimpspresenceconnectionuing.h>
// FORWARD DECLARATIONS
class CCAEngine;
class CPEngNWSessionSlotID2;
class CIMPSSAPSettings;
class TPEngWVCspServicesTree2;
class MCAMainViewArrayPC;
class MCALoginRefreshObserverPC;

// CLASS DECLARATION

/**
* CCALoginPC
* Process Component for Login,Logout
* @lib wvuiprocessng.dll
* @since 3.2
*/
class CCALoginPC : public CBase, public MCALoginPC
    {
    public:
        /**
        * Two-phased constructor.
        * @param aEngine - Reference to chat engine component
        */
        static CCALoginPC* NewL( CCAEngine& aEngine );

        /**
         * Second phased symbian constructor.
         */
        void ConstructL();
        /**
         * Destructor
         */
        virtual ~CCALoginPC();

    private:

        /**
        * Default Constructor
        * @param aEngine - reference to chat engine
        */
        CCALoginPC( CCAEngine& aEngine );

    public:

        /**
         * Does Login to IM Engine
         * @param aState - NetworkState login/logout
         * @param aSettings - pointer to SAP Settings
         * @param aSessionSlotID - Specifies the current session
         * @param aImpsServices - Reference to the supported IMPS Services
         * @return true if logged in successfully else false
         */
        TBool LoginL( TNetworkState aState, CIMPSSAPSettings* aSettings,
                      CPEngNWSessionSlotID2* aSessionSlotID,
                      TPEngWVCspServicesTree2 &aImpsServices );

        /**
         * Notifies the IM Engine about the Logged out state
         * @param aState - NetworkState login/logout
         * @param aSessionSlotID - Specifies the current session
         * @param aImpsServices - Reference to the supported IMPS Services
         * @return true if notified successfully else false
         */
        TBool NotifyEngineForLogoutL( TNetworkState aState,
                                      CPEngNWSessionSlotID2* aSessionSlotID,
                                      TPEngWVCspServicesTree2 &aImpsServices );

        /**
         * Logout - Does Logout from IM Engine
         */
        void LogoutL();

        /**
         * PerformNetworkStateChangeL -for IM Engine
         */
        void PerformNetworkStateChangeL( TNetworkState aState,
                                         CPEngNWSessionSlotID2* aSessionSlotID,
                                         TPEngWVCspServicesTree2 &aImpsServices );

        /**
         * ReadyForShutdown for engine
         * @return true if ready for shutdown
         */
        TBool ReadyForShutdown();

        /**
         * Cancel All engine requests
         */
        void CancelAllRequests();

        /**
         * Used To get MainViewArrayPC
         * @ return Pointer to MCAMainViewArrayPC
         * @ since 3.2
         */
        IMPORT_C MCAMainViewArrayPC* GetArrayInterface() const;

        /**
         * Tells if Logged in with a different userid or not
         * @return ETrue if yes else EFalse.
         */
        TBool FreshLoginL( );


        /**
         * Set the value to show nickname or not
         * @param aShowNickName 'Show nick name' -value
         */
        void SetShowNicknameL( TBool aShowNickName );

        /**
         * Set the alias
         * @param aAlias - Alias value to be set
         */
        void SetAliasL( const TDesC& aAlias );

    public : //from MCALoginPC

        /**
        * AddObserver
        */
        void AddObserver( MCALoginRefreshObserverPC *aObserver );

        /**
        * RemoveObserver
        */
        void RemoveObserver();


        /**
         * Register to receive notifications related to
         * start and completetion of background task
         * @param MCABackGroundTaskObserver- object
         */
        void RegisterBackGroundTaskObserver( MCABackGroundTaskObserver *aObserver );

        /**
         * UnRegister from receiving notifications related to
         * start and completetion of backgroundtask
         */
        void UnRegisterBackGroundTaskObserver( MCABackGroundTaskObserver* aObserver );

        /**
         * To know whether Background task has started
         * @return TBool
         */
        TBool IsBackgroundTaskPending();



    private://Data

        //Ref. to chat engine,doesnt own
        CCAEngine& iEngine;

        //Pointer to MainViewArrayPC,owns
        MCAMainViewArrayPC* iMainViewArrayPC;
        CIMPSSAPSettings* iLoggedInSap;

        //last logged in user id
        HBufC* iLoggedUserId;

        HBufC* iOldLoggedUserId;

        //tells if it is first login or not
        TBool iFirstLogin;


        // owned ,Login refresh for conversation container
        MCALoginRefreshObserverPC* iRefreshObserver;


    };

#endif      // CCALOGINPC_H

// End of File
