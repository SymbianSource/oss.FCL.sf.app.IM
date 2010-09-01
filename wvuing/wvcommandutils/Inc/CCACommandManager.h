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
* Description:  Command Manager for creating the commands
*
*/



#ifndef CCACOMMANDMANAGER_H
#define CCACOMMANDMANAGER_H

//  INCLUDES

#include <e32base.h>
#include "MCACommand.h"
#include "MCASettingsCommand.h"
#include "MCACommandFactory.h"
#include "MCAUiContactEditCmdCB.h"
#include "MCASessionHandlerCmd.h"


//FORWARD DECLARATIONS
class  MCAProcessManager;
class  MCAUiLoginCmdCB;
class  CCASessionHandlerCmd;
class  MCAUiSettingsDialogCmdCB;




// CLASS DECLARATION

/**
*  Command Manager.
*
*/
NONSHARABLE_CLASS( CCACommandManager ) : public CBase,
        public MCACommandFactory

    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCACommandManager* NewL();

    /**
    * Destructor.
    */
    virtual ~CCACommandManager();

private:
    /**
    * Default Constructor.
    */
    CCACommandManager();

public:
    /**
    * Initializes the session handler
    * @param aSessionHandlerCmd : holds the session handler instance
    */
    void Initialize( CCASessionHandlerCmd* aSessionHandlerCmd );


public: // From MCACommandFactory
    /**
    * Creates a new command object corresponding to given command id.
    *
    * @param aCommandId Id of the command to be created
    * @return Pointer to the created object, ownership is transferred.
    */
    MCACommand* NewCommandLC( TInt aCommandId,
                              MCAProcessManager& aProcessManager, MCAUiLoginCmdCB& aUiLoginCmdCB );
    /**
    * Creates a new command object corresponding to given command id.
    *
    * @param aCommandId Id of the command to be created
    * @return Pointer to the created object, ownership is transferred.
    */
    MCASettingsCommand* NewCommandLC( TInt aCommandId,
                                      MCAProcessManager& aProcessManager, MCAUiSettingsDialogCmdCB& aUiCmdCB );

    /**
    * Creates a new command object corresponding to given command id.
    *
    * @param aCommandId Id of the command to be created
    * @return Pointer to the created object, ownership is transferred.
    */
    MCASettingsCommand* NewCommandL( TInt aCommandId,
                                     MCAProcessManager& aProcessManager, MCAUiSettingsDialogCmdCB& aUiCmdCB );

    MCACommand* NewCommandLC( TInt aCommandId, MCAProcessManager& , MCAUiContactEditCmdCB& );
    MCACommand* NewCommandL ( TInt aCommandId, MCAProcessManager& , MCAUiContactEditCmdCB& );

    /**
    * @see MCACommandFactory
    */
    MCASessionHandlerCmd* GetSessionHandlerCmd() ;

private:    // Data

    CCASessionHandlerCmd* iSessionHandlerCmd;

    };

#endif      // CCACOMMANDMANAGER_H

// End of File
