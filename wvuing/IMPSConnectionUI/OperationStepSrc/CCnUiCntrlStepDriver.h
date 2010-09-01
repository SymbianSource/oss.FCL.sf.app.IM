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
* Description:  Control step driver.
*
*/

#ifndef __CCNUICNTRLSTEPDRIVER_H
#define __CCNUICNTRLSTEPDRIVER_H

//  INCLUDES
#include <E32Base.h>


// FORWARD DECLARATIONS
class MCnUiCntrlStep;



// CLASS DECLARATION
/**
 * Control step driver.
 * Implements step execution behaviour
 * as desribed in MCnUiCntrlStep.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiCntrlStepDriver ) : public CBase
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiCntrlStepDriver* NewL();

    /**
     * Two-phased constructor.
     */
    static CCnUiCntrlStepDriver* NewLC();

    /**
     * Destructor.
     */
    ~CCnUiCntrlStepDriver();

protected: //Protected constructors to allow derivation

    /**
     * C++ constructor.
     */
    CCnUiCntrlStepDriver();



public: // New methods


    /**
     * Appends new step to que of executed steps.
     * New step is appended after existing ones.
     * AppendStepL() takes ownership of
     * given aStep if operation fully succeeds.
     *
     * @since 2.1
     * @param aStep New control step to add
     * Parameter aStep may not be NULL, or
     * EIMPSConn_NULLPtr panic is raised.
     */
    void AppendStepL( MCnUiCntrlStep* aStep );


    /**
     * Executes the added steps.
     * For more information about execution
     * behaviour, see MCnUiCntrlStep.
     *
     * Driver must have at least one
     * step added before it can be executed.
     * In other case, EIMPSConnStepDriverUninitialized
     * panic is raised.
     *
     * @see MCnUiCntrlStep
     * @since 2.1
     * @return Execution status.
     * This is the error value returned
     * from last run RunStepL().
     */
    TInt ExecuteL();



private: //Data

    ///<Steps to execute. Array & steps owned.
    RPointerArray< MCnUiCntrlStep >   iSteps;

    };

#endif      //__CCNUICNTRLSTEPDRIVER_H
//  End of File


