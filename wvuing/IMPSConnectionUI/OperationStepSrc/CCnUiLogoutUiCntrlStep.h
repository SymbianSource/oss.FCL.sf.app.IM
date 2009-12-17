/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Logout UI control.
*
*/

#ifndef __CCNUILOGOUTUICNTRLSTEP_H
#define __CCNUILOGOUTUICNTRLSTEP_H

//  INCLUDES
#include <E32Base.h>
#include "MCnUiCntrlStep.h"
#include "MCnUiOpProgressStateObserver.h"



//FORWARD DECLARATION
class MCnUiUiControlContext;
class MCnUiConnWaitNote;
class CCnUiConnCloser;
class CPEngNWSessionSlotID2;


// CLASS DECLARATION
/**
 * Logout UI control.
 * Performs logout using the CCnUiConnCloser.
 *
 * This class behaviour can be extended by deriving.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiLogoutUiCntrlStep ) : public CBase,
        public MCnUiCntrlStep,
        public MCnUiOpProgressStateObserver
    {
protected: //own enumerations
    /**
     * Return status for succesful logout init.
     */
    enum TCnUiLogoutInitStatus
        {
        ECnUiLogoutInitContinue = 1   //must be positive to distinguish from KErrXXX error codes
        };

public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiLogoutUiCntrlStep* NewLC( MCnUiUiControlContext& aCCntxt,
                                          CIMPSSAPSettings& aLogoutSap,
                                          const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    virtual ~CCnUiLogoutUiCntrlStep();


protected: //protected to allow derivation

    /**
     * C++ constructor.
     */
    CCnUiLogoutUiCntrlStep( MCnUiUiControlContext& aCCntxt,
                            CIMPSSAPSettings& aLogoutSap );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: // New functions from MCnUiCntrlStep


    /**
     * Runs logout operation.
     * Uses virtual DoInitLogoutStepL()
     * to setup logout data and virtual
     * DoHandleLogoutStatusL() to handle logout
     * status code.
     *
     * @since 2.1
     * @return Errorcode from logout.
     */
    TInt RunStepL();


    /**
     * Handles logout complete.
     *
     * If there isn't anymore active network
     * connection, issues connection restore
     * for previously dropped network connections
     * if needed.
     *
     * @since 2.1
     * @return Always ECnUiStepContinueTeardown.
     */
    TCnUiHandleCompleteStatus HandleCompleteL();


    /**
     * Undo step.
     *
     * @since 2.1
     */
    void UndoStepL();



public: // New functions from MCnUiOpProgressStateObserver

    /**
     * Notification method to get notifications from
     * MCnUiOpProgressStateObserver events.
     *
     * Sets up proper wait note by using the
     * DoShowLogoutStateSpecificWaitnoteL();
     *
     * @param aStateId The state id from CCnUiConnCloser.
     * @param aClient The accessed client.
     * @since 2.1
     */
    void HandleProgressStateEnter( TInt aStateId,
                                   TIMPSConnectionClient aClient );



protected: // New helper & template functions for derived class use


    /**
     * Shows error note from simultaneous
     * login operation.
     *
     * @since 2.1
     */
    void ShowSimultaneousLoginOperationNoteL();


    /**
     * Framework method.
     * Virtual method to initializes the logout operation.
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
    virtual TInt DoInitLogoutStepL( TIMPSConnectionClient& aClient );


    /**
     * Framework method.
     *
     * Virtual method to show proper wait note for
     * logout state.
     *
     * @since 2.1
     * @param aStateId State id from CCnUiConnCloser.
     * @param aClient The accessed client.
     */
    virtual void DoShowLogoutStateSpecificWaitnoteL( TInt aStateId,
                                                     TIMPSConnectionClient aClient );



    /**
     * Framework method.
     *
     * Virtual method to show proper note for
     * successful logout.
     *
     * @since 2.1
     * @param aClient The accessed client.
     */
    virtual void DoShowLogoutOkNoteL( TIMPSConnectionClient aClient );


    /**
     * Gets the current client information from TLS
     *
     * @since 3.0
     */
    virtual TIMPSConnectionClient ConnectionClient();

protected: // protected data shared with derived classes

    ///<UI operation context, not owned
    MCnUiUiControlContext&              iCCntxt;

    ///<Used logout SAP, not owned
    CIMPSSAPSettings&                     iLogoutSap;

    ///<Wait note, owned. Initialized and destroyed during the operation.
    MCnUiConnWaitNote*                  iWaitNote;

    // owns: network session slot ID
    CPEngNWSessionSlotID2*              iNWSessionSlotID;

private: // private data

    ///<Connection closer, owned
    CCnUiConnCloser*                    iConnCloser;

    };

#endif      //  __CCNUILOGOUTUICNTRLSTEP_H

//  End of File

