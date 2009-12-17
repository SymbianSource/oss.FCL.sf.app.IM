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
* Description:  Application exit logout UI control.
*
*/

#ifndef __CCNUIAPPEXITLOGOUTUICNTRLSTEP_H
#define __CCNUIAPPEXITLOGOUTUICNTRLSTEP_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>
#include "CCnUiLogoutUiCntrlStep.h"


// CLASS DECLARATION
/**
 * Application exit logout UI control.
 *
 * Extends base logout control behaviour to handle
 * correctly application exit requests.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiAppExitLogoutUiCntrlStep ) : public CCnUiLogoutUiCntrlStep
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiAppExitLogoutUiCntrlStep* NewLC(
        MCnUiUiControlContext& aCCntxt,
        CIMPSSAPSettings& aLogoutSap,
        TIMPSExitType aExitType,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    virtual ~CCnUiAppExitLogoutUiCntrlStep();


protected: //protected to allow derivation

    /**
     * C++ constructor.
     */
    CCnUiAppExitLogoutUiCntrlStep( MCnUiUiControlContext& aCCntxt,
                                   CIMPSSAPSettings& aLogoutSap,
                                   TIMPSExitType aExitType );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



protected: // Functions overridden from CCnUiLogoutUiCntrlStep


    /**
     * Handles logout complete.
     *
     * If this is normal mode exit, issues
     * the base class to handle restore for
     * previously dropped network connections
     * if needed.
     *
     * @since 2.1
     * @return Always ECnUiStepContinueTeardown.
     */
    TCnUiHandleCompleteStatus HandleCompleteL();


    /**
     * Handles undo.
     *
     * @since 2.1
     */
    void UndoStepL();


    /**
     * Overridden framework method.
     *
     * @since 2.1
     *
     * @param aClient Init has to initialize the aClient
     * with the client id to log in.
     *
     * @return Error status from init. If init returns something
     * else  than ECnUiLogoutInitContinue login operation RunStepL()
     * is breaked with return value.
     */
    TInt DoInitLogoutStepL( TIMPSConnectionClient& aClient );


private: // private data

    ///< Application exit type, owned
    TIMPSExitType                   iExitType;
    };

#endif      //  __CCNUIAPPEXITLOGOUTUICNTRLSTEP_H

//  End of File

