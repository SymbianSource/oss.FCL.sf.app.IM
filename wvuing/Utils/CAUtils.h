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


#ifndef __CAUTILS_H__
#define __CAUTILS_H__

#include    <e32base.h>
#include    <s32strm.h>
#include    <badesca.h>

//	FORWARD CLASS DECLERATIONS

// CLASS DECLARATION

/**
 *  Utils for IM modules.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class CAUtils   // CSI: 51 # This is not a C-class although it begins with CA
    {
    public: // new methods

        /**
         * Case insensitive comparison of WVIDs, using protocol ("wv:") neutral
         * comparison.  The "wv:" part (actually everything up to the first
         * ":") is ignored in the comparison. If aDomainNeutral is ETrue, does
         * domain-neutral comparison with the following logic:
         *   - If only one of aId1 or aId2 (but not both) contains domain,
         *     domain-neutral comparison is done
         *   - If both or none of aId1 and aId2 contain domain, ordinary
         *     wv-neutral comparison is done
         * If aDomainNeutral is EFalse, then falls back to the old way of
         * comparing, i.e. wv-neutral only
         * @param aId1 The first id to compare (network-given)
         * @param aId2 The second id to compare
         * @param aDomainNeutral Do domain neutral comparison (ETrue, default)
         * @return -1, 0 or +1, like CompareC.
         * @since 2.5
         */
        static TInt NeutralCompare( const TDesC& aId1, const TDesC& aId2, TBool
                                    aDomainNeutral = ETrue );
        /**
         * Process userid/groupid/listid so that the "wv:" part is hidden,
         * depending on the branded setting coming from the UI.
         * @param aId The id to process
         * @return The id with the "wv:" (userid, groupid) or "wv:user" (listid)
         * part hidden
         */
        static TPtrC DisplayId( const TDesC& aId, TBool aListHiding = EFalse );

        /**
         * Generates a valid Wireless Village Id by removing all
         * characters that cannot appear in Wireless Village Ids.
         * @param aId The Id to process
         * @param aNum if within range (1...9999) the number is appended to the
         *             string. Appending might replace some part of the string
         *             if number doesn't fit otherwise.
         * @return The Id without invalid characters. Onwership is transferred.
         */
        static HBufC* GenerateIdLC( const TDesC& aId, TInt aNum = 0 );

        /**
         * Externalize buffer to stream
         * @param aBuffer Buffer to externalize
         * @param aStream stream to externalize to
         */
        static void ExternalizeBufferToStreamL( const TDesC& aBuffer,
                                                RWriteStream& aStream );
        static void ExternalizeBufferToStreamL( const TDesC8& aBuffer,
                                                RWriteStream& aStream );

        /**
         * Internalize buffer from stream
         * @param aStream stream to internalize from
         * @return Internalized buffer.
         */
        static HBufC* InternalizeBufferFromStreamLC( RReadStream& aStream );
        static HBufC* InternalizeBufferFromStreamL( RReadStream& aStream );
        static HBufC8* InternalizeBuffer8FromStreamLC( RReadStream& aStream );
        static HBufC8* InternalizeBuffer8FromStreamL( RReadStream& aStream );

        /**
         * Checks wether the user id is correct or not
         * @return ETrue if valid id
         */
        static TBool ValidLoginIdL( const TDesC& aPresenceId );

        /**
         * Adds the removed "wv:" and "@something" parts
         * @param aOrigId Original user id, i.e. "wv:user@domain.com"
         * @param aUserId User entered id, i.e. "otheruser"
         * @return The reconstructed id, i.e. "wv:otheruser@domain.com"
         * The ownership is transferred to caller!
         */
        static HBufC* ReconstructIdL( const TDesC& aOrigId, const TDesC& aUserId );

        /**
         * Find if user is on userlist using NeutralCompare
         * @param aUserList list of wvid's
         * @param aUserId   user id to find
         * @return position of founding, KErrNotFound if not found
         */
        static TInt NeutralFind( const MDesCArray& aUserList, const TDesC& aUserId );

        /**
         * This routine is used to convert between arabic-indic digits and european digits
         * based on existing language setting. So it'll convert any digit from the string
         * to use either european digits or arabic-indic digits based on current settings.
         *
         * This method can be also called in european release. The method is required
         * to do the correct thing with all the languages.
         *
         * Never store the converted string as unicode.
         *
         * @since S60 v3 dot 1
         * @param aDes Descriptor data to be converted.
         */
        static void LanguageSpecificNumberConversion( TDes& aDes );
        /**
        *  UI CR  ID: 101-39727
        *  This function capitalises the first letter of the
        *  contactlist.
        *
        *
        *
        * @param aListname Descriptor(Listname) data to be converted.
        */
        static HBufC* CapitalizeListNameL( const TDesC& aListname );

        /**
         *  UI CR  ID: 101-39727
         *  This function returns ETrue if capitalization of Lists is enabled,
         *  EFalse if disabled.
         *
         * The value is stored in the storage manager.
         *

         */
        static TBool CapitalizingEnabled();

        /**
        *  UI CR  ID: 101-39727
        *  This function capitalises the first letter of the
        *  contactlistID.
        *
        *
        *
        * @param aListname Descriptor(Listname) data to be converted.
        */
        static HBufC* CapitalizeListIdL( const TDesC& aId );
    };

#endif      // __CAUTILS_H__

// End of File
