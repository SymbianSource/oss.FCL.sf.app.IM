/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  this class is a contact mapper abstract interface
*
*/

#ifndef __MPENGCONTACTMAPPER_H__
#define __MPENGCONTACTMAPPER_H__

//  INCLUDES
#include <e32std.h>
#include <badesca.h>



// FORWARD DECLARATIONS
class CContactIdArray;


//  CLASS DEFINITION
/**
 * Contact mapper abstract interface
 *
 *  @since 3.0
 */
class MPEngContactMapper
    {
    public: //Attribute Data

        /**
         *  Map wv id to the contact db id
         *
         *  @since 3.0
         *  @param aWvId wv id to map
         *  @return array of mapped contact ids
         */
        virtual CContactIdArray* MapWvIdToContactIdLC(
            const TDesC& aWVId ) = 0;

    protected: // destructor

        /**
         *  Protected Inline virtual destructor. Concrete class cannot
         *  be deleted trough this interface.
         */
        virtual ~MPEngContactMapper() {};

    };

#endif      //  __MPENGCONTACTMAPPER_H__
//  End of File

