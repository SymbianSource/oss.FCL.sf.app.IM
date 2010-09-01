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
* Description: this class handls search operation
*
*/

#ifndef CPENGSEARCHCONTROL_H
#define CPENGSEARCHCONTROL_H

//  INCLUDES
#include <E32Base.h>
#include <MPEngNWSessionSlotObserver2.h>


// FORWARD DECLARATIONS
class MPEngSearchTransactionObserver2;
class MPEngAdvTransactionStatus2;
class CPEngSearchTransaction2;
class CPEngSearchCriteria2;
class CPEngSearchResultEntry;
class CPEngNWSessionSlotID2;
class CPEngTransactionStatus;
class CPEngNWSessionSlot2;
class CPEngSearchSessionHandler;
class CPEngSearchHandler;
class CPEngSearchTransaction2Imp;



/**
 *
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSearchControl ) : public CActive,
        public MPEngNWSessionSlotObserver2
    {
private: //Enumerations

    /**
     * Search controls state enumerations.
     */
    enum TPEngSearchState
        {
        EPEngSrch_Idle = -1, //Idle state, no operation running
        EPEngSrch_OpeningCspSession = 1,
        EPEngSrch_Searching = 2,
        EPEngSrch_StoppingSearch = 4,
        EPEngSrch_ClosingCspSession = 5
        };


public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngSearchControl* NewL( TInt aPriority,
                                     const CPEngNWSessionSlotID2& aNWSessionSlotID,
                                     const RPointerArray< CPEngSearchCriteria2 >& aCriterias,
                                     TInt aSearchLimit,
                                     MPEngSearchTransactionObserver2& aObserver );

    /**
     * Destructor.
     */
    virtual ~CPEngSearchControl();


private:

    /**
     * C++ default constructor.
     */
    CPEngSearchControl( TInt aPriority,
                        MPEngSearchTransactionObserver2& aObserver );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( TInt aPriority,
                     const CPEngNWSessionSlotID2& aNWSessionSlotID,
                     const RPointerArray< CPEngSearchCriteria2 >& aCriterias,
                     TInt aSearchLimit );




public: //New services


    /**
     *
     *
     * @since 3.0
     */
    void SetOwner( CPEngSearchTransaction2Imp& aOwner );


    /**
     *
     *
     * @since 3.0
     */
    void Cancel();


    /**
     *
     *
     * @since 3.0
     */
    void IssueSearch();


    /**
     *
     *
     * @since 3.0
     */
    TInt IssueContinueSearchFromNetwork( TInt aContinueIndex );


    /**
     *
     *
     * @since 3.0
     */
    TInt IssueStopSearchFromNetwork();






private: //From CActive

    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );

private: //From MPEngNWSessionSlotObserver2

    /**
     * NWSessionSlot change handler.
     *
     * Called by CPEngNWSessionSlotNotifier2 to report NWSessionSlot
     * changed events. If this method leaves, error code is
     * reported back to HandleNWSessionSlotError().
     *
     * @since 3.0
     * @param aNotifier The notifier source where notification
     *        is coming from. No ownership transferred.
     * @param aEvent The event container.  No ownership transferred.
     */
    void HandleNWSessionSlotChangeL(
        CPEngNWSessionSlotNotifier2& aNotifier,
        CPEngNWSessionSlotEvent2& aEvent );


    /**
     * Notification failure handler.
     *
     * @since 3.0
     * @param aError The error resulting from Presence Engine internal
     *        change event handling (e.g. out of memory) or
     *        leave error from HandleNWSessionSlotChangeL().
     * @param aNotifier The notifier of which event handling failed.
     *        No ownership transferred.
     */
    void HandleNWSessionSlotError(
        TInt aError,
        CPEngNWSessionSlotNotifier2& aNotifier );



private: //Helpers

    /**
     *
     *
     * @since 3.0
     */
    void AsyncTerminateSearch();


    /**
     *
     *
     * @since 3.0
     */
    void ValidateSearchCriteriasL(
        const RPointerArray< CPEngSearchCriteria2 >& aCriterias );


    /**
     *
     *
     * @since 3.0
     */
    void DoNotifyObserver( TInt aErr );

    /**
     *
     *
     * @since 3.0
     */
    void HandleStepIssued( TPEngSearchState aState, TInt aErr );



private: //Data


    //REF: The observer
    MPEngSearchTransactionObserver2&     iObserver;

    //REF: The owning transaction
    CPEngSearchTransaction2Imp*     iOwner;

    //OWN: Result data, transaction status
    CPEngTransactionStatus*     iTransactionStatus;

    //OWN: Result data, search result
    CPEngSearchResultEntry*     iSearchResult;

    //OWN: Result data, current operation
    TInt    iTransactionOp;

    //OWN: Search session handler
    CPEngSearchSessionHandler*  iSearchSessionHandler;

    //OWN: Search session handler
    CPEngSearchHandler*     iSearchHandler;

    //OWN: Session notifier
    CPEngNWSessionSlotNotifier2*    iSlotNotifier;

    //OWN: Cancel ongoing flag
    TBool   iCancelling;

    //OWN: Current state
    TPEngSearchState    iSearchState;

    //OWN: Delayed error
    TInt    iDelayedErr;

    };

#endif      //CPENGSEARCHCONTROL_H




