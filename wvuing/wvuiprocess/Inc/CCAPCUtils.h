/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utils for IM modules.
*
*/


#ifndef __CCAPCUTILS_H__
#define __CCAPCUTILS_H__

#include    <e32base.h>
#include    <s32strm.h>
#include    <badesca.h>

//	FORWARD CLASS DECLERATIONS
class MCAStoredContact;

// CLASS DECLARATION

/**
 *  Utils for IM modules.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class CCAPCUtils   // CSI: 51 # This is not a C-class although it begins with CA
    {
    public: // new methods

        /**
         * Process userid/groupid/listid so that the "wv:" part is hidden,
         * depending on the branded setting coming from the UI.
         * @param aId The id to process
         * @return The id with the "wv:" (userid, groupid) or "wv:user" (listid)
         * part hidden
         */
        IMPORT_C static TPtrC DisplayId( const TDesC& aId, TBool aListHiding = EFalse );

        /**
        * Combines the descriptors in given array to one descriptor.
        * Ownership is transferred to caller!
        * The items are separated with KSpace.
        * @since
        * @param aArray Array of descriptors to be combined
        * @param aDisplayId Set this to ETrue if the user ids are displayed on UI.
        *                   Strips off the "WV:" part from ids.
        * @return Buffer containing the combined descriptors
        *         separated with KSpace
        */
        IMPORT_C static HBufC* CreateBufLC( const CDesCArray& aArray, TBool aDisplayId = EFalse );

        /**
        * Handles partial successful errors
        * @return count of partial success errors
        * @since 3.2
        */

        IMPORT_C static TInt PartialSucessCountL();

        /**
         * Handles partial successful errors
         * @return correct error code
         * @since 3.2
         */

        IMPORT_C static TInt GetErrorCodeL( TInt aIndex );

        /**
         * Modifies aArray so that it does not contain duplicate user ids.
         * @since series 60 v3.2
         * @param aArray Array of MCAStoredContacts to be modified.
         */
        IMPORT_C static void RemoveDuplicateWVIds( CDesCArray& aArray );

    private:    // Data

        // Error data holder
        RPointerArray<TInt> iErrorData;

    };

#endif      // __CCAPCUTILS_H__

// End of File
