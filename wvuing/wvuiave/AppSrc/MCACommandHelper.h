/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  command helper for single-list view
*
*/


#ifndef MCACOMMANDHELPER_H
#define MCACOMMANDHELPER_H

/**
 * Helper class to invoke commands from container.
 */
class MCACommandHelper
    {
    public:

        /**
         * Invoke command.
         * @param aCommand Command to invoke.
         */
        virtual void InvokeCommandL( TInt aCommand ) = 0;
    };
#endif //MCACOMMANDHELPER_H
