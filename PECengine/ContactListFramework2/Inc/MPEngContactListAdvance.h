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
* Description:  Advance list model
*                This Advance Contact List model is used internally
*                by PEC Engine and allows advance maintenance
*                of the contact lists
*
*/


#ifndef __MPENGCONTACTLISTADVANCE_H__
#define __MPENGCONTACTLISTADVANCE_H__

//  INCLUDES
#include "MPEngContactList2.h"

// FORWARD DECLARATIONS
class MPEngContactItemAdvance;

// CLASS DECLARATION
/**
 *  Advance Contact List model
 *
 *  It supports all functionality of the public list model
 *  (MPEngPresenceList2) plus additional capabilities which are intended
 *  to be used only internally by the PEC Engine
 *
 *  @lib PEngListLib2
 *  @since 3.0
 */

class MPEngContactListAdvance : public MPEngContactList2
    {

    public: // Functions from MPEngPresenceList

        /**
         *  Advance Contact item model
         *
         *  Reference to advance contact item model.
         *  It can be used to modify internal states
         *  of the Contact Item which are otherwise available only
         *  for reading.
         *
         *  @since 3.0
         *  @param aIndex index of desired contact Item
         *  @return reference to contact Item
         */
        virtual MPEngContactItemAdvance& ContactItemAdvance(
            TInt aIndex,
            TPEngContactListView aView
            = EPEngCntListLocalView ) = 0;

    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Public destructor to allow deletion trough interface.
         */
        virtual ~MPEngContactListAdvance() {};

    };

#endif      //  __MPENGCONTACTLISTADVANCE_H__

//  End of File

