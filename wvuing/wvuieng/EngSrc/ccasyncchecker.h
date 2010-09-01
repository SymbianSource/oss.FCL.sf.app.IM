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

#include "mcastoredcontactsobserver.h"
#include "mcacontactlists.h"

#include "e32base.h"

// FORWARD DECLARATIONS

/**
 *  Helper class for checking synch state.
 *
 *  @lib caengine.dll
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CCASyncChecker ) : public CBase,
        public MCAStoredContactsObserver
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
     * Check if sync is done
     * @return ETrue if sync is done
     */
    TBool IsSyncDoneL();

public: // New functions

    /**
     * Check synch
     * If this is called when client is not logged in, it will return EFalse.
     * @return ETrue if success. EFalse if fail
     */
    static TBool CheckSyncStateL();


private: // From MCAStoredContactsObserver

    /**
     * @see MCAStoredContactsObserver
     */
    void HandleContactDelete( const TDesC& aContactId );

    /**
     * @see MCAStoredContactsObserver
     */
    void HandleAddition( MCAContactList& aList, MCAStoredContact& aContact );

    /**
     * @see MCAStoredContactsObserver
     */
    void HandleChange( MCAContactList* aList, MCAStoredContact* aContact,
                       TStorageManagerGlobals::TCAObserverEventType aEventType,
                       TBool aUserIdChanged );


private: // Data

    CActiveSchedulerWait iWait;

    };

#endif // CCASYNCCHECKER_H




