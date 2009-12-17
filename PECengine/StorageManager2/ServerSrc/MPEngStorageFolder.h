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
* Description:  Storage folder abstract interface
*
*/



#ifndef __MPENGSTORAGEFOLDER_H
#define __MPENGSTORAGEFOLDER_H

//  INCLUDES

#include <e32def.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class CPEngDataEntry;

// CLASS DECLARATION
/**
*  Storage folder abstract interface
*
*  @since 3.0
*/
class MPEngStorageFolder
    {
    public: // New functions

        /**
         *
         *
         */
        virtual CPEngDataEntry* DataEntryL( const TDesC& aSId  ) = 0;


    protected:  //Destructor
        /**
         * Virtual inline destructor.
         * can not be destroyed using this interface.
         */
        virtual ~MPEngStorageFolder() {};
    };

#endif      // __MPENGSTORAGEFOLDER_H

// End of File
