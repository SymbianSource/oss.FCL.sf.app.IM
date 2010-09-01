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
* Description:  Interface for CCAcontactSelectionPC
*
*
*/


#ifndef MCACONTACTSELECTIONPC_H
#define MCACONTACTSELECTIONPC_H

#include <e32base.h>
#include <BADESCA.h>

#include "TEnumsPC.h"


//Forward Declarations


// INTERFACE DECLARATION
/*
* MCAMainViewArrayPC: This is main interface of arraywrapper on
* Process component side.UI Side array decorator has to use this
* array and get the required details.
*
* @lib wvuiprocessng.lib
*/
class MCAContactSelectionPC
    {

    public: //destructor
        virtual ~MCAContactSelectionPC() {};


    public:

        /**
         * Clears all selected-flags
         */
        virtual void ClearSelectedL( ) = 0;


    };

#endif