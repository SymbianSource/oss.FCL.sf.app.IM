/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Async to sync.
*
*/

#ifndef __CASYNC2SYNC_H
#define __CASYNC2SYNC_H

// INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 * Synchronizes asynchronous operation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CAsync2Sync ) : public CActive
    {
public:  // Symbian OS constructor and destructor

    /**
     * C++ default constructor.
     */
    static CAsync2Sync* NewLC();


    /**
     * Destructor.
     */
    virtual ~CAsync2Sync();


private:  // Constructor

    /**
     * C++ default constructor.
     */
    CAsync2Sync();


public: //New

    /**
     * Prepares the wait object for the new wait round.
     * Sets the internal request status to KErrNone state.
     *
     * @since 3.0
     */
    void Prepare();


    /**
     * Waits the issued request to complete.
     *
     * If owned request status isn't pending,
     * returns directly. Else returns after
     * request complete.
     *
     * @since 3.0
     */
    void WaitToComplete();


    /**
     * Gets the active object request status.
     *
     * @since 3.0
     * @return Active object request status.
     */
    TRequestStatus&  RequestStatus();



private: //From CActive

    void RunL();
    TInt RunError( TInt aError );
    void DoCancel();


private: //data

    //OWN: Active Scheduler wait
    CActiveSchedulerWait    iWait;
    };

#endif // __CASYNC2SYNC_H


// End of File

