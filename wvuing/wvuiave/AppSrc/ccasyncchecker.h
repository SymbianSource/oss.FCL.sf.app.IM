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
* Description:  Helper class for checking synch state.
*
*/


#ifndef CCASYNCCHECKER_H
#define CCASYNCCHECKER_H

#include "ccawaitnote.h"
#include "MCAUiGlobalNotificationObserverPC.h"
#include "TEnumsPC.h"
#include "e32base.h"



// FORWARD DECLARATIONS
class CCAAppUi;
class MCAGlobalNotificationObserverPC;

//CLASS DECLARATION
/**
 *  Helper class for checking synch state.
 *
 *  @lib chatng.exe
 *  @since S60 v3.1
 */
class CCASyncChecker : public CBase,
            MCAWaitNoteObserver,
            MCAUiGlobalNotificationObserverPC
    {

    private: // construction

        /**
         * Create synch checker
         * @return ETrue if success. EFalse if fail
         */
        static CCASyncChecker* NewLC();

        /**
         * Destruction
         */
        ~CCASyncChecker();

    private: // Helper methods

        /**
         * Wait for sync to complete.
         * @return success.
         */
        TBool WaitForSyncCompleteL();

        /**
         * Check online state
         * @return ETrue if online
         */
        TBool LoggedIn();

    public: // New functions

        /**
         * Check synch
         * If this is called when client is not logged in, it will return EFalse.
         * @return ETrue if success. EFalse if fail
         */
        static TBool CheckSyncStateL();

    public: //From MCAUiGlobalNotificationObserverPC

        /**
         * @see MCAUiGlobalNotificationObserverPC
         */
        virtual void HandleDelete( const TDesC& aContactId , TEnumsPC::TItem type );

        /**
         * @see MCAUiGlobalNotificationObserverPC
         */
        virtual void HandleAddition( TEnumsPC::TItem type );

        /**
         * @see MCAUiGlobalNotificationObserverPC
         */
        virtual void HandleChange( TEnumsPC::TItem aType, TEnumsPC::TChange aChangeType );


        /**
        * @see MCAUiGlobalNotificationObserverPC
        */
        virtual void HandleInvitationEventL( TEnumsPC::TInviteType /*aInviteType*/,
                                             const TDesC& /*aUserId*/, const TDesC& /*aMessage*/ )
            {
            //dummy implementation
            };

        /**
         * @see MCAUiGlobalNotificationObserverPC
         */
        virtual void HandleInvitationResponse( TBool /*aAcceptance*/, const TDesC& /*aUserId*/,
                                               const TDesC& /*aGroupName*/, const TDesC& /*aResponse*/ )
            {
            //dummy implementation
            };

        /**
         * @see MCAUiGlobalNotificationObserverPC
         */
        virtual void HandleSettingsChangeL( TInt /*aChangedSettingEnum*/ )
            {
            //dummy implementation
            };

        /**
         * @see MCAUiGlobalNotificationObserverPC
         */
        virtual void HandleChatEvent( const TEnumsPC::TChatEventType /*aEvent*/,
                                      const TDesC& /*aWvid*/, const TDesC& /*aContactId*/,
                                      const TDesC& /*aMessage*/,
                                      const TEnumsPC::TMessagerType /*aMsgrType*/,
                                      const TEnumsPC::TMessageType /*aMsgType*/ )
            {
            //dummy implementation
            };

        /**
        * To handle error condition in message related event
        * aError: error id
        * aInfo: error detail
        */
        virtual void HandleMessageError( TInt /*aError*/, const TDesC& /*aInfo*/ )
            {
            //dummy implementation
            };

    private: // from MCAWaitNoteObserver

        /**
         * @see MCAWaitNoteObserver
         */
        void NoteCanceled( TInt aButtonId );

    private:
        /*
        * Default constructor
        */
        CCASyncChecker();

    private: // Data

        // Is wait note canceled by user.
        TBool iWaitNoteCanceled;

        // Owned. Wait note.
        CCAWaitNote* iWaitNote;

        //active scheduler wait
        CActiveSchedulerWait iWait;

        //Pointer to app ui. Doesnt own.
        CCAAppUi* iAppUi;

        //Pointer to the global notification observer
        MCAGlobalNotificationObserverPC* iGlobalNotificationPC;

    };

#endif // CCASYNCCHECKER_H




