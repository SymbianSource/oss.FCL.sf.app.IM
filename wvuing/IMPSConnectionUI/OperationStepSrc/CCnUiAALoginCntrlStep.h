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
* Description:  AA login control.
*
*/

#ifndef __CCNUIAALOGINCNTRLSTEP_H
#define __CCNUIAALOGINCNTRLSTEP_H

//  INCLUDES
#include <E32Base.h>
#include "MCnUiCntrlStep.h"
#include "MCnUiClientPlugin.h"
#include "impspresenceconnectionuiconstsng.h"



//FORWARD DECLARATION
class MCnUiBaseControlContext;
class CIMPSSAPSettings;
class CCnUiConnOpener;
class CCnUiConnCloser;
class CPEngNWSessionSlotID;

// CLASS DECLARATION
/**
 * AA login control.
 * Performs a AA login to default SAP
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiAALoginCntrlStep ) : public CBase,
        public MCnUiCntrlStep
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiAALoginCntrlStep* NewLC( MCnUiBaseControlContext& aCCntxt,
    CIMPSSAPSettings& aLoginSap,
    TBool aShowDetailedError,
    CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    virtual ~CCnUiAALoginCntrlStep();


private:

    /**
     * C++ constructor.
     */
    CCnUiAALoginCntrlStep( MCnUiBaseControlContext& aCCntxt,
                           CIMPSSAPSettings& aLoginSap,
                           TBool aShowDetailedError,
                           CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: // New functions from MCnUiCntrlStep


    /**
     * Runs connection open / login step.
     *
     * @since 2.1
     * @return Errorcode from login.
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



public: // New functions, left public to allow testing

    /**
     * Runs connection open / login step.
     *
     * @since 2.1
     * @return Error code from login.
     */
    TInt DoRunStepL();


    /**
     * Loads default SAP from SAP Settings
     * store as login SAP.
     *
     * @since 2.1
     * @param aSap
     * @return KErrNone / KCnUiErrorNoProperDefaultSap.
     */
    TInt LoadDefaultSapL( CIMPSSAPSettings& aSap );

private: // new helper
    /**
      * Gets the current client information from TLS
      *
      * @since 3.0
      */
    TIMPSConnectionClient ConnectionClient();

private: // data

    ///<Base control context, not owned
    MCnUiBaseControlContext&               iCCntxt;

    ///<Login SAP, not owned
    CIMPSSAPSettings&                      iLoginSap;

    ///<Connection opener, owned
    CCnUiConnOpener*                       iConnOpener;

    ///<Connection closer, owned
    CCnUiConnCloser*						iConnCloser;

    // Tells wether we should show a detailed error
    // note in case an error occurs during login
    TBool									iShowDetailedError;

    // does not own
    CPEngNWSessionSlotID2&                   iNWSessionSlotID;

    MCnUiClientPlugin*                      iLogoutPlugin;
    };

#endif      //  __CCNUIAALOGINCNTRLSTEP_H

//  End of File

