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
* Description:  Transfer adamapter manager
*
*/


#ifndef __PENGTRANSFERADAPTER_H
#define __PENGTRANSFERADAPTER_H

//  INCLUDES
#include    <e32base.h>


// FORWARD DECLARATIONS
class MPEngAccessHandler;
class MPEngPureDataHandler;
class CPEngSessionSlotId;


/**
 *  Transfer manager adapter
 *  It is used to retrieve access handler and
 *  pure data handler for the defined session
 *
 *  @lib
 *  @since 3.0
 */
class PEngTransferAdapter
    {

    public: // New functions

        /**
         *
         *
         *  @since 3.0
         *  @param
         *  @return
         */
        IMPORT_C static MPEngAccessHandler* AccessHandlerL(
            const CPEngSessionSlotId& aSessionSlot );

        /**
         *
         *
         *  @since 3.0
         *  @param
         *  @return
         */
        IMPORT_C static MPEngPureDataHandler* PureDataHandlerL(
            const CPEngSessionSlotId& aSessionSlot );

    protected: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        PEngTransferAdapter();

        /**
        * Destructor.
        */
        virtual ~PEngTransferAdapter();

        // Prohibit copy constructor if not deriving from CBase.
        PEngTransferAdapter( const PEngTransferAdapter& );

        // Prohibit assignments operator if not deriving from CBase.
        PEngTransferAdapter& operator=( const PEngTransferAdapter& );
    };


#endif      // __PEngTransferAdapter_H

// End of File
