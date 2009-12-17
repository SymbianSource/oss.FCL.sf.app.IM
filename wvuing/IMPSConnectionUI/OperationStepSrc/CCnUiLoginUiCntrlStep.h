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
* Description:  Login UI control.
*
*/

#ifndef __CCNUILOGINUICNTRLSTEP_H
#define __CCNUILOGINUICNTRLSTEP_H

//  INCLUDES
#include <E32Base.h>
#include <f32file.h>
#include <impspresenceconnectionuiconstsng.h>
#include "MCnUiCntrlStep.h"
#include "MCnUiWaitNoteObserver.h"
#include "MCnUiOpProgressStateObserver.h"


//FORWARD DECLARATION
class MCnUiUiControlContext;
class MCnUiConnWaitNote;
class CCnUiConnOpener;
class CPEngNWSessionSlotID2;
class RResourceFile;
class MIMPSSharedData;

// CLASS DECLARATION
/**
 * Login SAP select UI control.
 * Performs real connection open
 * using the CCnUiConnOpener.
 *
 * This class behaviour can be extented by deriving.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiLoginUiCntrlStep ) : public CBase,
        public MCnUiCntrlStep,
        public MCnUiWaitNoteObserver,
        public MCnUiOpProgressStateObserver
    {
protected: //own enumerations
    /**
     * Return status for succesful login init.
     */
    enum TCnUiLoginInitStatus
        {
        ECnUiLoginInitContinue = 1   //must be positive to distinguish from KErrXXX error codes
        };


public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     * @param aCCntxt UI control context.
     */
    static CCnUiLoginUiCntrlStep* NewLC( MCnUiUiControlContext& aCCntxt,
                                         CIMPSSAPSettings& aLoginSap,
                                         TIMPSLoginType aLoginType,
                                         CPEngNWSessionSlotID2& aNWSessionSlotID,
                                         TIMPSConnectionClient aClient );


    /**
     * Destructor.
     */
    virtual ~CCnUiLoginUiCntrlStep();


protected: //protected to allow derivation

    /**
     * C++ constructor.
     *
     * By default the UI level login type is
     * EIMPSManualLogin ==> Login is reported
     * as manual login, not AA one.
     */
    CCnUiLoginUiCntrlStep( MCnUiUiControlContext& aCCntxt,
                           CIMPSSAPSettings& aLoginSap,
                           CPEngNWSessionSlotID2& aNWSessionSlotID,
                           TIMPSConnectionClient aClient,
                           TIMPSLoginType aLoginType = EIMPSManualLogin );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: // New functions from MCnUiCntrlStep


    /**
     * Runs connection open / login step.
     *
     * @since 2.1
     * @return Error code from login.
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



public: // New functions from MCnUiWaitNoteObserver

    /**
     * Notify method for wait note cancel events.
     * @since 2.1
     */
    void HandleWaitNoteCancel();



public: // New functions from MCnUiOpProgressStateObserver

    /**
     * Handles progress state enter.
     * Setups proper wait note using the DoShowLoginStateSpecificWaitnoteL()
     *
     * @since 2.1
     * @param aStateId The state id from CCnUiConnOpener.
     * @param aClient The accessed client.
     */
    void HandleProgressStateEnter( TInt aStateId,
                                   TIMPSConnectionClient aClient );




protected: // New template functions for derived class use


    /**
     * Framework method.
     * Virtual method to initializes the login operation.
     *
     * @since 2.1
     *
     * @param aClient Init has to initialize the aClient
     * with the client id to log in.
     *
     * @return Error status from init. If init returns something
     * else  than ECnUiLoginInitContinue login operaion RunStepL()
     * is breaked with return value.
     */
    virtual TInt DoInitLoginStepL( TIMPSConnectionClient& aClient );


    /**
     * Framework method.
     *
     * Virtual method to show proper wait note for
     * login state.
     *
     * @since 2.1
     * @param aStateId State id from CCnUiConnOpener.
     * @param aClient The accessed client.
     */
    virtual void DoShowLoginStateSpecificWaitnoteL( TInt aStateId,
                                                    TIMPSConnectionClient aClient );


private: // New functions


    /**
     * Handles the login data failure error.
     *
     * @since 2.1
     * @param aTrieError The original trie error
     * @return KErrNone if login is allowed to try again.
     * Else error to return from login try.
     */
    TInt HandleLoginDataFailureL( TInt aTrieError );


    /**
     * Handles the conflicting SAP connection error.
     *
     * @since 2.1
     * @param aTrieError The original trie error
     * @param aTriedLoginClient Client which were tried to login.
     * @return KErrNone if login is allowed to try again.
     * Else error to return from login try.
     */
    TInt HandleConflictingSapConnectionL( TInt aTrieError,
                                          TIMPSConnectionClient aTriedLoginClient );



    /**
     * Tries disconnect other currently active
     * connections.
     *
     * @since 2.1
     * @return ETrue if succesfully disconnected
     * other connections. Else EFalse.
     */
    TBool TryDisconnectOtherConnectionsL();

    /**
     * Opens correct resource file.
     * !!! Two items left to cleanup stack. !!!
     * @param aResourceFile Contains opened file on return
     * @since 3.1
     */
    void OpenResourceFileLC( RResourceFile& aResourceFile );

    /**
     * Resource reader for non-Cone situations
     * @param aResourceId The integer-valued resource id to read
     * @return The value of the integer
     * @since 3.1
     */
    TInt ReadResourceIntValueL( TInt aResourceId );

protected: // protected data

    ///<Used UI control context, not owned
    MCnUiUiControlContext&              iCCntxt;

    ///<Used login SAP, not owned
    CIMPSSAPSettings&                     iLoginSap;

    ///<Wait note, owned. Initialized and destroyed during the operation.
    MCnUiConnWaitNote*                  iWaitNote;



private: // private data

    ///<Connection opener, owned
    CCnUiConnOpener*                    iConnOpener;

    ///<Login type setting, owned
    const TIMPSLoginType                iLoginType;

    // Does not own Network session slot ID
    CPEngNWSessionSlotID2&              iSessionSlotID;

    // The client application ID
    TIMPSConnectionClient               iClient;

    // file handle used by resource file reading routine
    RFs   iRFs;

    // Owned.
    MIMPSSharedData*	iSharedData;
    };

#endif      //  __CCNUILOGINUICNTRLSTEP_H

//  End of File

