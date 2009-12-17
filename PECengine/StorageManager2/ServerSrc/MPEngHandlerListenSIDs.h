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



#ifndef __MPENGHANDLERLISTENSIDS_H
#define __MPENGHANDLERLISTENSIDS_H

//  INCLUDES

#include <e32def.h>

// FORWARD DECLARATIONS
class CPEngDataEntry;

// CLASS DECLARATION

/**
*  Storage folder abstract interface
*
*  @since 3.0
*/
class MPEngHandlerListenSIDs
    {
    public: // New functions

        /**
         * Notify client about changed one Storage ID
         * @see <CPEngRequestHandler.h>
         *
         * @since 3.0
         * @param aChangedSId changed SID
         */
        virtual void EntryUpdated( const CPEngDataEntry& aEntry ) = 0;

        /**
         *
         */
        virtual void EntryDeleted( const CPEngDataEntry& aEntry ) = 0;

    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * can not be destroyed using this interface.
         */
        virtual ~MPEngHandlerListenSIDs() {};
    };

#endif      // __MPEngHandlerListenSIDs_H

// End of File
