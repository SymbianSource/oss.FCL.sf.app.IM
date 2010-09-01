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
* Description:  Abstract factory for creating command objects.
*
*/



#ifndef MCACOMMANDFACTORY_H
#define MCACOMMANDFACTORY_H

//  INCLUDES
#include <e32std.h>
#include "MCAUiContactEditCmdCB.h"

// FORWARD DECLARATIONS
class MCAProcessManager;
class MCAUiLoginCmdCB;
class MCAUiSettingsDialogCmdCB;
class MCASettingsCommand;
class MCASessionHandlerCmd;

// CLASS DECLARATION

/**
*  Abstract factory for creating command objects.
*/
class MCACommandFactory
    {
    public:
        // Destructor
        virtual ~MCACommandFactory() {}

    public: // New functions

        /**
        * Creates a new command object corresponding to given command id.
        *
        * @param aCommandId Id of the command to be created
        * @return Pointer to the created object, ownership is transferred.
        */
        virtual MCACommand* NewCommandLC( TInt aCommandId,
                                          MCAProcessManager& aProcessManager,
                                          MCAUiLoginCmdCB& aUiLoginCmdCB ) = 0;
        /**
        * Creates a new command object corresponding to given command id.
        *
        * @param aCommandId Id of the command to be created
        * @return Pointer to the created object, ownership is transferred.
        */
        virtual MCASettingsCommand* NewCommandLC( TInt aCommandId,
                                                  MCAProcessManager& aProcessManager,
                                                  MCAUiSettingsDialogCmdCB& aUiCmdCB ) = 0;

        /**
        * Creates a new command object corresponding to given command id.
        *
        * @param aCommandId Id of the command to be created
        * @return Pointer to the created object, ownership is transferred.
        */
        virtual MCASettingsCommand* NewCommandL( TInt aCommandId,
                                                 MCAProcessManager& aProcessManager,
                                                 MCAUiSettingsDialogCmdCB& aUiCmdCB ) = 0;

        virtual MCACommand* NewCommandLC( TInt aCommandId,
                                          MCAProcessManager& ,
                                          MCAUiContactEditCmdCB& ) = 0;
        virtual MCACommand* NewCommandL ( TInt aCommandId,
                                          MCAProcessManager& ,
                                          MCAUiContactEditCmdCB& ) = 0;

        /**
         * Get SessHandlerObserversManager interface.
         * @return Pointer to the SessHandlerObserversManager interface.
         */

        virtual MCASessionHandlerCmd* GetSessionHandlerCmd()  = 0;

    };

#endif      // MCACOMMANDFACTORY_H

// End of File
