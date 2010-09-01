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
* Description:  Control step interface.
*
*/

#ifndef __MCNUICNTRLSTEP_H
#define __MCNUICNTRLSTEP_H

//  INCLUDES
#include <E32Base.h>


//MACROS
/**
 * Macro for return value error handling.
 * If returned value is something else than KErrNone,
 * returns it. Else continues the execution.
 */
#define RETURN_IF_ERROR( op ) \
        { \
        TInt __err = op; \
        if( __err != KErrNone ) \
            { \
            return __err;\
            } \
        } \
     
/**
 * Completion codes for
 * HandleCompleteL()
 * @since 2.1
 */
enum TCnUiHandleCompleteStatus
    {
    ///<No special requirements from step. Teardown can be
    ///<continued to next step.
    ECnUiStepContinueTeardown = 0,

    ///<Step wishes to restart processing from this
    ///<step's RunStepL().
    ECnUiRestartStep = 1
    };



// CLASS DECLARATION
/**
 * Control step interface.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiCntrlStep )
    {
public: // New abstract operation step methods

    /**
     * Run step.
     *
     * Here the concrete step may do what so ever
     * operations the step running needs.
     *
     * @since 2.1
     */
    virtual TInt RunStepL() = 0;


    /**
     * Called to handle failure in step execution.
     *
     * Failed step might have been either this same step
     * or one of the subsequent ones.
     *
     * @since 2.1
     */
    virtual void UndoStepL() = 0;


    /**
     * Called to handle step running completion.
     * This is called when the all steps have been
     * processed successfully.
     *
     * If step wishes, it can restart the step execution
     * from this step forward by returning the ECnUiRestartStep.
     * Else the teardown is coninued to previous step.
     *
     * @since 2.1
     * @return Completion status from TCnUiHandleCompleteStatus.
     */
    virtual TCnUiHandleCompleteStatus HandleCompleteL() = 0;


public:  //Destructor

    /**
     * Inline destructor.
     *
     * Destroy trough this
     * interface is possible.
     */
    virtual ~MCnUiCntrlStep() {};
    };


#endif      //  __MCNUICNTRLSTEP_H

//  End of File

