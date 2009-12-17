/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Login Command class
*
*/



#ifndef CCALOGINCMD_H
#define CCALOGINCMD_H


//  INCLUDES
#include <e32base.h>
#include "MCACommand.h"



// FORWARD DECLARATIONS
class MCAProcessManager;
class CCASessionHandlerCmd;
class MCAUiLoginCmdCB;



// CLASS DECLARATION
/**
*  Login Command class.
*
*/
NONSHARABLE_CLASS( CCALoginCmd ) : public CBase, public MCACommand
    {
public:
    // Constructors and destructor

    /**
    * Two Phase Constructor
    * @param aProcessManager Ref. to Appln's Process Manager
    * @param aUiLoginCmdCB Ref. to UI Login Call Back Interface
    * @param aSessionHandlerCmd Ref. to Command Session Handler
    */
    static CCALoginCmd* NewL( MCAProcessManager& aProcessManager,
    MCAUiLoginCmdCB& aUiLoginCmdCB, CCASessionHandlerCmd& aSessionHandlerCmd  );

    /**
    * Two Phase Constructor
    * Created Object would be pushed to cleanupstack
    * @param aProcessManager Ref. to Appln's Process Manager
    * @param aUiLoginCmdCB Ref. to UI Login Call Back Interface
    * @param aSessionHandlerCmd Ref. to Command Session Handler
    */
    static CCALoginCmd* NewLC( MCAProcessManager& aProcessManager,
                               MCAUiLoginCmdCB& aUiLoginCmdCB, CCASessionHandlerCmd& aSessionHandlerCmd  );

    /**
    * Destructor.
    */
    virtual ~CCALoginCmd();

private:

    /**
    * C++ default constructor.
    * @param aProcessManager Ref. to Appln's Process Manager
    * @param aUiLoginCmdCB Ref. to UI Login Call Back Interface
    * @param aSessionHandlerCmd Ref. to Command Session Handler
    */

    CCALoginCmd( MCAProcessManager& aProcessManager,
                 CCASessionHandlerCmd& aSessionHandlerCmd,
                 MCAUiLoginCmdCB& aUiLoginCmdCB );

public:

    /**
    * Set command observer.
    *
    * @param aObserver Command observer
    */
    void SetObserver( MCACommandObserver& aObserver );

    /**
    * Cancel command.
    *
    */
    void CancelCommand();
    /**
    * Executes the command.
    *
    */
    void ExecuteCommandL();

    /**
    *
    * @return ETrue if the command is asynchronous, otherwise EFalse.
    */
    TBool IsAsynchronous();

protected:

    /**
    * Complete command.
    *
    * @param aError Commend completed with this error code
    */
    void CompletedL( TInt aError );

private:
    // command observer
    MCACommandObserver* iObserver;
    // Ref. to Process Manager
    MCAProcessManager& iProcessManager;
    // Ref. to Command Session Handler
    CCASessionHandlerCmd& iSessionHandlerCmd;
    //Ref. to UI Login Call Back Interface
    MCAUiLoginCmdCB& iUiLoginCmdCB;

    };

#endif      // CCALoginCmd_H

// End of File
