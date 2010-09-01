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



#ifndef MCACOMMANDOBSERVER_H
#define MCACOMMANDOBSERVER_H

//  INCLUDES
#include <e32std.h>



// CLASS DECLARATION

/**
*  CommandObserver
*
*/
class MCACommandObserver
    {

    public: // New functions

        virtual void CommandCompletedL( TInt aError );

    };

#endif      // MCACOMMANDOBSERVER_H

// End of File
