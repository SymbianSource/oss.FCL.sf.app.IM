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
* Description:  Helper class for waiting fetch process.
*
*/


#ifndef CCAFETCHWAITER_H
#define CCAFETCHWAITER_H

#include "ccawaitnote.h"
#include "mcacontactfetchobserver.h"

#include <e32base.h>

// FORWARD DECLARATIONS
class CCAUISessionManager;

/**
 *  Helper class for checking synch state.
 *
 *  @lib chat.exe
 *  @since S60 v3.1
 */
class CCAFetchWaiter : public CBase,
            public MCAWaitNoteObserver,
            public MCAContactFetchObserver
    {

    private: // construction

        /**
         * Create synch checker
         * @return ETrue if success. EFalse if fail
         */
        static CCAFetchWaiter* NewLC();

        /**
         * Destruction
         */
        ~CCAFetchWaiter();

    private: // Helper methods

        /**
         * Wait for fetch to complete.
         * @return success.
         */
        TBool WaitForFetchCompleteL();

        /**
         * Check if Fetch is already done
         * @return ETrue if sync is done
         */
        TBool IsFetchDone();

        /**
         * Check online state
         * @return ETrue if online
         */
        TBool LoggedIn();

        /**
         * Access to ui session manager
         * @return Ref to ui session manager
         */
        CCAUISessionManager& UISessionManager();

    public: // New functions

        /**
         * Check fetch state. If the fetch process is not done this method will
         * wait until it is.
         * If this is called when client is not logged in, it will return EFalse.
         * @return ETrue if success. EFalse if not logged in or canceled
         */
        static TBool CheckFetchStateL();

    public: // From MCAContactFetchObserver

        /**
         * @see MCAContactFetchObserver
         */
        void HandleFetchCompleteL( TInt aError );

    private: // from MCAWaitNoteObserver

        /**
         * @see MCAWaitNoteObserver
         */
        void NoteCanceled( TInt aButtonId );

    private: // Data

        // Is wait note canceled by user.
        TBool iWaitNoteCanceled;

        // Owned. Wait note.
        CCAWaitNote* iWaitNote;

        // Scheduler wait
        CActiveSchedulerWait iWait;
    };

#endif // CCAFETCHWAITER_H




