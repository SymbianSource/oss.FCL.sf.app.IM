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
* Description:  Active object based global note container.
*
*/



#ifndef __CCNUIGLOBALNOTECONTAINER_H
#define __CCNUIGLOBALNOTECONTAINER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

class CAknGlobalNote;
// CLASS DECLARATION
/**
 * Active object based global note container.
 *
 * Shows desired global note in synchronous manner.
 */
NONSHARABLE_CLASS( CCnUiGlobalNoteContainer ) : public CActive
    {
private: //private enumerations

    /**
     * Global note container states.
     *
     * @since 2.1
     */
    enum TCnUiGNCState
        {
        ECnUiGNCIdle = 0,                       ///< Idle state, no operation running
        ECnUiGNCShowingDetailedNoteMain = 1,    ///< Container showing detailed main note
        ECnUiGNCShowingDetailedNoteSecond = 2,  ///< Container showing detailed second note
        ECnUiGNCShowingWaitingNote = 3          ///< Container showing simple waiting note
        };



public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor, constructs the
     * CCnUiGlobalNoteContainer object.
     *
     * @since 2.1
     * @return is pointer to CCnUiGlobalNoteContainer object.
     */
    static CCnUiGlobalNoteContainer* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCnUiGlobalNoteContainer();


private: //Constructors

    /**
     * C++ default constructor.
     */
    CCnUiGlobalNoteContainer();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();


public://New methods

    /**
     * Shows two phased "detailed" global note.
     *
     * @since 2.1
     *
     * @param aPromptText The Prompt text for the main note.
     * @param aDetailedPromptText The Prompt text for the detailed note.
     */
    void ShowWaitigDetailedNoteL( const TDesC& aPromptText,
                                  const TDesC& aDetailedPromptText );


    /**
     * Shows two phased "detailed" global note.
     *
     * @since 2.1
     *
     * @param aPromptText The Prompt text for the main note.
     */
    void ShowWaitingNoteL( const TDesC& aPromptText );



protected:  // Methods derived from CActive

    /**
     * From CActive.
     * @see CActive
     */
    void RunL();

    /**
     * From CActive.
     * @see CActive
     */
    void DoCancel();

    /**
     * From CActive.
     * @see CActive
     */
    TInt RunError( TInt aError );



private: // New private helper methods

    /**
     * Waits the started asynchronous steps
     * to complete using the CActiveSchedulerWait.
     *
     * @since 2.1
     */
    void WaitCompletion();


    /**
     * Stops the previously started
     * CActiveSchedulerWait wait loop.
     *
     * @since 2.1
     */
    void Completed();

    /**
     * Resets the container internal state
     *
     * @since 2.1
     */
    void ResetState();


    /**
     * Issues detailed main note.
     *
     * @since 2.1
     */
    void IssueDetailedMainNoteL();


    /**
     * Issues detailed second note.
     *
     * @since 2.1
     */
    void IssueDetailedSecondNoteL();


    /**
     * Issues normal waiting note.
     *
     * @since 2.1
     */
    void IssueWaitingNoteL();



private: //data

    ///<Active Scheduler Wait, owned
    CActiveSchedulerWait            iWait;

    ///<Global note handle, owned
    CAknGlobalNote*                 iGlobalNote;

    ///<Prompt text for main note, owned
    HBufC*                          iPromptText;

    ///<Prompt text for second note, owned
    HBufC*                          iDetailedPromptText;

    ///<Note id for current note, owned
    TInt                            iNoteId;

    ///<Note container internal state, owned
    TCnUiGNCState                   iState;
    };

#endif      //__CCNUIGLOBALNOTECONTAINER_H

//  End of File

