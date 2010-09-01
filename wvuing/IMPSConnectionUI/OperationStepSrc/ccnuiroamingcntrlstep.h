/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Control step driver.
*
*/

#ifndef __CCNUIROAMINGCNTRLSTEP_H
#define __CCNUIROAMINGCNTRLSTEP_H


//  INCLUDES
#include <E32Base.h>
#include <barsc.h>

#include "MCnUiCntrlStep.h"

// CLASS DECLARATION
NONSHARABLE_CLASS( CCnUiRoamingCntrlStep ) : public CBase,
        public MCnUiCntrlStep
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiRoamingCntrlStep* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCnUiRoamingCntrlStep();


protected: //protected to allow derivation

    /**
     * C++ constructor.
     *
     */
    CCnUiRoamingCntrlStep();

public: // New functions from MCnUiCntrlStep


    /**
     * Runs connection open / login step.
     *
     * @return Error code from login.
     */
    TInt RunStepL();


    /**
     * Handles complete.
     *
     * @return Always ECnUiStepContinueTeardown.
     */
    TCnUiHandleCompleteStatus HandleCompleteL();


    /**
     * Handles undo.
     *
     */
    void UndoStepL();


private: // New functions
    /**
     * Shows  warning dialog
     * @return Error code.
     */
    TInt ShowRoamingDlgL();
    };

#endif // __CCNUIROAMINGCNTRLSTEP_H









