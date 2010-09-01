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
* Description:  Network connection close UI control.
*
*/

#ifndef __CCNUINWDISCONNECTUICNTRLSTEP_H
#define __CCNUINWDISCONNECTUICNTRLSTEP_H

//  INCLUDES
#include <E32Std.h>
#include <impspresenceconnectionuiconstsng.h>
#include "CCnUiLogoutUiCntrlStep.h"


//FORWARD DECLARATION
class CIMPSSAPSettings;
class CPEngNWSessionSlotID2;


// CLASS DECLARATION
/**
 * Network connection close UI control.
 * Closes network connection by issuing logout
 * one by one for each logged in client.
 *
 * Extends CCnUiLogoutUiCntrlStep behaviour
 * by looping back from HandleCompleteL()
 * after each client logout.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiNWDisconnectUiCntrlStep ) : public CCnUiLogoutUiCntrlStep
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     * @param aCCntxt UI control context.
     * @param aLogoutSap The SAP that was logouted.
     * @param aDisconnectedClients During client logout,
     * logouted client id's are appended to this array.
     * So after whole control execution, this array will
     * contain the clients which were logged out.
     * @param aMainLevelOperation ETrue if this operation is carried
     * on main level, else EFalse (Operation is a sub operation for login).
     */
    static CCnUiNWDisconnectUiCntrlStep* NewLC(
        MCnUiUiControlContext& aCCntxt,
        CIMPSSAPSettings& aLogoutSap,
        RArray< TIMPSConnectionClient >& aDisconnectedClients,
        TBool aMainLevelOperation,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );

    /**
     * Destructor.
     */
    virtual ~CCnUiNWDisconnectUiCntrlStep();


protected: //protected to allow derivation

    /**
     * C++ constructor.
     */
    CCnUiNWDisconnectUiCntrlStep( MCnUiUiControlContext& aCCntxt,
                                  CIMPSSAPSettings& aLogoutSap,
                                  RArray< TIMPSConnectionClient >& aDisconnectedClients,
                                  TBool aMainLevelOperation );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: // Overridden functions from MCnUiCntrlStep


    /**
     * Overridden HandleCompleteL().
     * Appends the logged out client to
     * parameter aDisconnectedClients array.
     *
     * If there is still more clients to logout,
     * issues a new round.
     *
     * @since 2.1
     * @return ECnUiRestartSte if there is another clients to disconnect.
     * Else ECnUiStepContinueTeardown.
     */
    TCnUiHandleCompleteStatus HandleCompleteL();


    /**
     * Undo step.
     *
     * @since 2.1
     */
    void UndoStepL();


protected: // Over written framework methods

    /**
     * Over written framework method.
     *
     * Setups the disconnect step as needed.
     *
     * @since 2.1
     *
     * @param aClient Init has to initialize the aClient
     * with the client id to log out.
     *
     * @return Error status from init. If init returns something
     * else  than ECnUiLogoutInitContinue login operation RunStepL()
     * is breaked with return value.
     */
    TInt DoInitLogoutStepL( TIMPSConnectionClient& aClient );


    /**
     * Over written framework method.
     *
     * Shows a proper note for
     * successful logout.
     *
     * @since 2.1
     * @param aClient The accessed client.
     */
    void DoShowLogoutOkNoteL( TIMPSConnectionClient aClient );


protected: // New helper functions

    /**
     * Initializes the disconnect.
     * Gets the SAP o disconnect and
     * populates the clients to logout.
     *
     * @since 2.1
     */
    void InitNWDisconnectL();



private: // private data

    ///<Cached reference to aDisconnectedClients parameter, not owned
    RArray< TIMPSConnectionClient >& iRequestedDisconnClients;

    ///<Is this mainlevel operation or not,oned
    TBool                            iMainlevelOperation;

    ///<Has the disconnect initialization done, owned
    TBool                            iInitDone;

    ///<Clients to disconnect, owned
    RArray< TIMPSConnectionClient >  iClientsToDisconn;

    };

#endif      //  __CCNUINWDISCONNECTUICNTRLSTEP_H

//  End of File

