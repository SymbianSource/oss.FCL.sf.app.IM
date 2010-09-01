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
* Description:  Hashing tool
*
*/


#ifndef __PENGHASHTOOL_H__
#define __PENGHASHTOOL_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KDirDelimLength = 1;
_LIT( KDirDelim, "\\" );


/**
 * Hashing tool
 *
 * @since 3.0
 */
class PEngHashTool
    {
    public:
        /**
         *  Hash given descriptor
         *
         *  @since 3.0
         *  @param  aDesToHash descriptor to hash
         *  @return hashed descriptor
         */
        static HBufC* HashDescriptorL( const TDesC& aDesToHash );

        /**
         *  Hash name of the state from the given
         *	user name and server address
         *
         *  @since 3.0
         *  @param
         *  @return
         */
        static HBufC* HashStateNameL( const TDesC& aServerAddress,
                                      const TDesC& aUserName );

    private:
        /**
         *  Replace forbidden characters in descriptor
         *
         *  @since 3.0
         *  @param descriptor
         *  @param aConvertFrom character to replace
         *  @param aConvertTo charatcter to replace with
         */
        static void ReplaceCharacterInDescriptor( TPtr& aDescriptor,
                                                  TText aConvertFrom,
                                                  TText aConvertTo );
    };

#endif // __PENGHASHTOOL_H__
//  End of File

