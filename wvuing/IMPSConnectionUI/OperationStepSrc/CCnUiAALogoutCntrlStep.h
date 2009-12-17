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
* Description:  AA logout control.
*
*/

#ifndef __CCNUIAALOGOUTCNTRLSTEP_H
#define __CCNUIAALOGOUTCNTRLSTEP_H

//  INCLUDES
#include <E32Base.h>
#include "MCnUiCntrlStep.h"



//FORWARD DECLARATION
class MCnUiBaseControlContext;
class CIMPSSAPSettings;
class CCnUiConnCloser;
class CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
 * AA logout control.
 * Performs a AA logout to default SAP
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiAALogoutCntrlStep ) : public CBase,
        public MCnUiCntrlStep
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiAALogoutCntrlStep* NewLC( MCnUiBaseControlContext& aCCntxt,
    CIMPSSAPSettings& aLogoutSap,
    TBool aIsScheduled,
    CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    virtual ~CCnUiAALogoutCntrlStep();


private:

    /**
     * C++ constructor.
     */
    CCnUiAALogoutCntrlStep( MCnUiBaseControlContext& aCCntxt,
                            CIMPSSAPSettings& aLogoutSap,
                            TBool aIsScheduled );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( CPEngNWSessionSlotID2& aNWSessionSlotID );



public: // New functions from MCnUiCntrlStep


    /**
     * Runs connection open / logout step.
     *
     * @since 2.1
     * @return Errorcode from logout.
     */
    TInt RunStepL();


    /**
     * Handles complete.
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



private: // data

    ///<Base control context, not owned
    MCnUiBaseControlContext&               iCCntxt;

    ///<Logout SAP, not owned
    CIMPSSAPSettings&                        iLogoutSap;

    ///<Connection opener, owned
    CCnUiConnCloser*                       iConnCloser;

    // Tells if this is a scheduled logout event
    // if so, then a query must be asked from the user
    TBool									iIsScheduled;

    // owns: session slot id
    CPEngNWSessionSlotID2*                   iNWSessionSlotID;
    };

#endif      //  __CCNUIAALOGOUTCNTRLSTEP_H

//  End of File

