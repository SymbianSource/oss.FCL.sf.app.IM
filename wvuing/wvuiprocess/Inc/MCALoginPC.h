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
* Description:  Interface for Login,Logout
*
*/



#ifndef MCALOGINPC_H
#define MCALOGINPC_H


// INCLUDES
#include <e32base.h>
#include "PublicEngineDefinitions.h"
#include "MCABackgroundTaskObserver.h"

// FORWARD DECLARATION
class CPEngNWSessionSlotID2;
class CIMPSSAPSettings;
class TPEngWVCspServicesTree2;
class MCALoginRefreshObserverPC;

// CLASS DECLARATION
/**
* MCALoginPC
* Process Component Interface for Login,Logout
* @lib wvuiprocessng.dll
* @since 3.2
*/

class MCALoginPC
    {
    public:

        /**
        * Does Login to IM Engine
        * @param aState - NetworkState login/logout
        * @param aSettings - pointer to SAP Settings
        * @param aSessionSlotID - Specifies the current session
        * @param aImpsServices - Reference to supported IMPS Services
        * @return true if logged in successfully else false
        */
        virtual TBool LoginL( TNetworkState aState, CIMPSSAPSettings* aSettings,
                              CPEngNWSessionSlotID2* aSessionSlotID, TPEngWVCspServicesTree2 &aImpsServices ) = 0;

        /**
         * Notifies the IM Engine about the Logged out state
         * @param aState - NetworkState login/logout
         * @param aSessionSlotID - Specifies the current session
         * @param aImpsServices - Reference to IMPS Services
         * @return true if notified successfully else false
         */
        virtual TBool NotifyEngineForLogoutL( TNetworkState aState,
                                              CPEngNWSessionSlotID2* aSessionSlotID, TPEngWVCspServicesTree2 &aImpsServices ) = 0;

        /**
         * Logout - Does Logout from IM Engine
         *
         */
        virtual void LogoutL() = 0;

        /**
         * ReadyForShutdown for engine
         * @return true if ready for shutdown
         */
        virtual TBool ReadyForShutdown() = 0;

        /**
         * Cancel All engine requests
         */
        virtual void CancelAllRequests() = 0;


        /**
         * Set the value to show nickname or not
         * @param aShowNickName 'Show nick name' -value
         */
        virtual void SetShowNicknameL( TBool aShowNickName ) = 0;

        /**
         * Set the alias
         * @param aAlias - Alias value to be set
         */
        virtual void SetAliasL( const TDesC& aAlias ) = 0;

        /**
         * Tells if Logged in with a different userid or not
         * @return ETrue if yes else EFalse.
         */
        virtual TBool FreshLoginL( ) = 0;


        /**
         * AddObserver method
         */
        virtual void AddObserver( MCALoginRefreshObserverPC *aObserver ) = 0;

        /**
         * RemoveObserver method
         */
        virtual void RemoveObserver() = 0;



        /**
        * RegisterBackGroundTaskObserver method
        */
        virtual void RegisterBackGroundTaskObserver( MCABackGroundTaskObserver *aObserver ) = 0;

        /**
        * UnRegisterBackGroundTaskObserver method
        */
        virtual void UnRegisterBackGroundTaskObserver( MCABackGroundTaskObserver *aObserver ) = 0;

        /**
         * To know whether Background task has started
         * @return TBool
         */
        virtual TBool IsBackgroundTaskPending()  = 0;


        /**
        * Performs the Network State Changes upon a slot
        *
        */
        virtual void PerformNetworkStateChangeL( TNetworkState aState,
                                                 CPEngNWSessionSlotID2* aSessionSlotID, TPEngWVCspServicesTree2 &aImpsServices ) = 0;



        /**
         * Destructor
         */
        virtual ~MCALoginPC() {};


    };


#endif  // MCALOGINPC_H

// End of File