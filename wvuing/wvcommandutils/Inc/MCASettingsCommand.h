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
* Description: Abstract factory for creating command objects
*
*/



#ifndef MCASETTINGSCOMMAND_H
#define MCASETTINGSCOMMAND_H

//  INCLUDES
#include <e32std.h>

#include "MCACommandObserver.h"
#include "MCACommand.h"



// FORWARD DECLARATIONS
class MCASettingSapExt;

// CLASS DECLARATION

/**
*  Abstract factory for creating command objects.

*/
class MCASettingsCommand : public MCACommand
    {

    public: // New functions
        /**
        * Executes the command.
        *
        */
        virtual void ExecuteCommandL() = 0;

        /**
        * Cancels the command execution, only for asynchronous commands.

        */
        virtual void CancelCommand() = 0;

        /**
        * Sets command observer.

        * @param aObserver Reference to the command observer.
        */
        virtual void SetObserver( MCACommandObserver& aObserver ) = 0;

        /**
        *
        * @return ETrue if the command is asynchronous, otherwise EFalse.
        */
        virtual TBool IsAsynchronous() = 0;

        /**
        * Gets an object of MCASettingSapExt
        * @Returns MCASettingSapExt object
        */
        virtual MCASettingSapExt*  GetAppSettingsSAPExt() = 0;

        /*
        * Destructor
        */

        virtual ~MCASettingsCommand() {}

    };

#endif      // MCAUICOMMAND_H

// End of File
