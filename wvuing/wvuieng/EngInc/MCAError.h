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
* Description:  Error interface for retrieving partial success data
*
*/


#ifndef MCAERROR_H
#define MCAERROR_H

#include <badesca.h>
#include <e32std.h>

/**
*  MCAError interface class provides functionality to retrieve partial
*  success data (201 "error").
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCAError
    {
    public:  // Interface

        /**
        * Returns the error code.
        * @return Error code.
        */
        virtual TInt Code() const = 0;

        /**
        * Textual description from the error.
        * @return Plain text explanation.
        */
        virtual const TDesC& Description() const = 0;

        /**
        * Return user IDs.
        * @return Array that holds the user IDs.
        */
        virtual const CDesCArray& UserIDs() const = 0;

        /**
        * Return group IDs.
        * @return Array that holds the group IDs.
        */
        virtual const CDesCArray& GroupIDs() const = 0;

        /**
        * Return message IDs.
        * @return Array that holds the message IDs.
        */
        virtual const CDesCArray& MessageIDs() const = 0;

        /**
        * Return screen names. The corresponding group is found from
        * corresponding array index in the ScreenNameGroup().
        * @return Array that holds the screen names.
        */
        virtual const CDesCArray& ScreenNames() const = 0;

        /**
        * Return group IDs.
        * @return Array that holds the group IDs.
        */
        virtual const CDesCArray& ScreenNameGroup() const = 0;

        /**
        * Destructor.
        */
        virtual ~MCAError() {};

    };

#endif      // MCAERROR_H

// End of File
