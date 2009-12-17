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
* Description:  this class handls key operation
*
*/

#ifndef __TPENGSTORAGESERVERBTREEKEYS_H__
#define __TPENGSTORAGESERVERBTREEKEYS_H__

// INCLUDES
#include	<s32btree.h>
#include	"CPEngDataEntry.h"
#include	"PEngStorageServerCommon.h"


// CLASS DECLARATION
class TPEngBaseKey : public MBtreeKey
    {
    public:
        /*
         *	Return key which is between aLeft and aRight. Can be also equal to left.
         */
        void Between( const TAny* aLeft, const TAny* aRight, TBtreePivot& aPivot ) const
            {
            __ASSERT_ALWAYS( aLeft && aRight, User::Panic( KStorageServerName, EErrorInBTree ) );
            //Return key equal to left.
            aPivot.Copy( reinterpret_cast<const TUint8*>( aLeft ), 4 );
            }

        //Compare two keys.
        TInt Compare( const TAny* aLeft, const TAny* aRight ) const
            {
            __ASSERT_ALWAYS( aLeft && aRight, User::Panic( KStorageServerName, EErrorInBTree ) );
            HBufC* left = *reinterpret_cast<HBufC**>( const_cast<TAny*>( aLeft ) );
            HBufC* right = *reinterpret_cast<HBufC**>( const_cast<TAny*>( aRight ) );
            TInt ret = left->CompareF( *right );
            return ret;
            }
    };

class TPEngDataEntryKey : public TPEngBaseKey
    {
    public:

        //Get pointer to entry key.(HBufC**)
        const TAny* Key( const TAny* anEntry ) const
            {
            __ASSERT_ALWAYS( anEntry, User::Panic( KStorageServerName, EErrorInBTree ) );
            CPEngDataEntry* entry = *reinterpret_cast<CPEngDataEntry**>( const_cast<TAny*>( anEntry ) );
            return entry->KeyBuffer();
            }
    };

#endif      //  __TPENGSTORAGESERVERBTREEKEYS_H__

//  End of File
