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
* Description:  Connection Manager have different states
*
*/

#ifndef __MConnectionManagerStateHandlerBase_H__
#define __MConnectionManagerStateHandlerBase_H__


#include "impsconnectionmanagercontrolif.h"


/**
 * ?one_line_short_description
 *
 * ?more_complete_description
 *
 * @lib ?library
 * @since S60 v4.0
 */
class MConnectionManagerStateHandlerBase
    {

    public:

        /**
         * ?description
         *
         * @since S60 3.2
         * @return TBool Return value tells if there's still substates to advance
         *               to in this state handler.
         */
        virtual TBool AdvanceState() = 0;


        /**
         * ?description
         *
         * @since S60 3.2
         * @return TBool Return value tells if SetActive needs to be called for this
         *               state.
         */
        virtual void ActivateStateL() = 0;


        virtual void DeleteStateD() = 0;


        virtual TInt StateIndex() = 0;


    protected:

        /**
         * Protected destructor.
         * Instancies can't be destroyed via this interface.
         */
        virtual ~MConnectionManagerStateHandlerBase() {}

    };


#endif // MConnectionManagerStateHandlerBase


