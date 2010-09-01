/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This Advance Contact Item model is used internally
*               by PEC Engine and allows advance maintenance
*               of the contact item
*
*/



#ifndef __MPENGCONTACTITEMADVANCE_H__
#define __MPENGCONTACTITEMADVANCE_H__


//  INCLUDES
#include <e32base.h>
#include "MPEngContactItem.h"


// CLASS DECLARATION
/**
 *  Advance version of the Contact Item Model
 *
 *  It supports all functionalities which are intended to be used
 *  by the PEC engine internally.
 *
 *  @lib PEngListLib2
 *  @since 3.0
 */
class MPEngContactItemAdvance : public MPEngContactItem
    {

    public: // New functions


    protected:  //Destructor

        /**
         *  Virtual inline destructor.
         *  Protected destructor to prohibited deletion trough interface.
         */
        virtual ~MPEngContactItemAdvance() {};

    };
#endif      //  __MPENGCONTACTITEMADVANCE_H__

//  End of File

