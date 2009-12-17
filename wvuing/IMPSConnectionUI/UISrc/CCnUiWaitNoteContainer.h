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
* Description:  Wait note container.
*
*/

#ifndef __CCNUIWAITNOTECONTAINER_H
#define __CCNUIWAITNOTECONTAINER_H

//  INCLUDES
#include <E32base.h>
#include "MCnUiUiFacade.h"
#include <AknProgressDialog.h>

// FORWARD DECLARATIONS
class CAknWaitDialog;
class MCnUiWaitNoteObserver;


// CLASS DECLARATION
/**
 * Wait note container.
 *
 * Wraps CAknWaitDialog behaviour.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiWaitNoteContainer ) : public CBase,
        public MCnUiConnWaitNote,
        public MProgressDialogCallback
    {

public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @since 2.1
     * @param aNoteStructureResourceId Resource ID for wait note.
     * @param aLabelText The label text to show in UI.
     * @param aObserver The observer to notify.
     * Can be NULL, if NULL, no observer is notified from
     * note cancels.
     */
    static CCnUiWaitNoteContainer* NewL( TInt aNoteStructureResourceId,
    const TDesC& aLabelText,
    MCnUiWaitNoteObserver* aObserver );

    /**
     * Destructor.
     */
    virtual ~CCnUiWaitNoteContainer();

private:

    /**
     * C++ constructor.
     *
     * @param aObserver The observer to notify.
     */
    CCnUiWaitNoteContainer( MCnUiWaitNoteObserver* aObserver );


    /**
     * Symbian OS constructor.
     * @param aLabelText The label text to show in UI.
     * @param aNoteStructureResourceId Resource ID for wait note.
     */
    void ConstructL( const TDesC& aLabelText, TInt aNoteStructureResourceId );


public: //from MCnUiConnWaitNote
    /**
     * @see MCnUiConnWaitNote
     * @since 2.1
     */
    void PushLC();


public: //from MProgressDialogCallback
    /**
     * @see MProgressDialogCallback
     * @since 2.1
     */
    void DialogDismissedL( TInt aButtonId );


private: // data

    ///<The observer, not owned, can be NULL
    MCnUiWaitNoteObserver*      iObserver;


    ///<The wait note, owned
    CAknWaitDialog*             iWaitNote;
    };

#endif      //  __CCNUIWAITNOTECONTAINER_H

//  End of File

