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
* Description:  Presence Engine Contact List extensions.
*
*/

#ifndef __PENGCONTACTLISTEXTENSIONS2_H
#define __PENGCONTACTLISTEXTENSIONS2_H

//  INCLUDES
#include    <E32Std.h>


// FORWARD DECLARATIONS
class CContactIdArray;



/**
 * Key for MPEngContactItem extension.
 *
 * Mapped contact DB contact ID's are stored under
 * this key for each succesfully mapped contact
 * list contact. Mapped IDs are available from
 * MPEngContactItem::GetCachedValue(). Descriptor
 * contents can be unpacked as CContactIdArray with
 * PEngContactItemExtensionCodec2::UnpackContactIdsL().
 *
 * If automatic contact mapping to contact DB IDs
 * is wanted for certain contact list, contact list
 * property: KPEngContactToContactDbMapping on level
 * KPEngCntLstPropertyExternalPermanent should be
 * put on (value 1). When mapping isn't more needed,
 * property can be set off (value 0).
 *
 * @since 3.0
 */
_LIT(       KPEngCntItemExtensionContactDbIds, "CntDbIds" );
const TUint KPEngCntItemExtensionMappedContactDbIds = 0x0003001;


/**
 * Contact Item TInt property name.
 * Property level: KPEngCntLstPropertyExternalPermanent
 * @see <MPEngContactItem.h>
 *
 * Key which value Janitor mapper will delete when
 * Contact Database mapping for such a contact was
 * updated.
 * Data which are stored under this key are defined
 * by the user of the contact list. Janitor mapper
 * only cleans content under this key once mapping
 * of corresponding contact Id is updated.
 */
const TUint KPEngCntItemExtensionMappingCleaning = 0x0003002;

/**
 * Contact list TInt property name.
 * Property level: KPEngCntLstPropertyExternalPermanent
 *
 * Flag if background contact id mapping to contact DB
 * contacts is wanted for the contacts in the contact list.
 *
 * Supported property values:
 *  - 0: (Default) No contact id mapping needed for the coctact list.
 *  - 1: Contact id mapping is needed for the coctact list.
 */
const TUint KPEngContactToContactDbMapping = 0x0003001;






// CLASS DECLARATION

/**
 * Presence Engine Contact item extension codec.
 *
 * @lib PEngManager2.lib
 * @since 3.0
 */
class PEngContactItemExtensionCodec2
    {
    public: // Login data packing

        /**
         * Packs Contact ID array as flat HBufC.
         *
         * @since 3.0
         * @param aContactIds The ContactIds to pack.
         * @return Contact ID package as HBufC8.
         * Buffer ownership is returned to caller.
         */
        IMPORT_C static HBufC8* PackContactIdsLC(
            const CContactIdArray& aContactIds );


        /**
         * Extracts Contact ID array contents from descriptor.
         *
         * @since 3.0
         * @param aContactIdsPckg The ContactIds packet.
         * @param aContactIds The ContactIds array to store
         * extracted contact Ids. Array is reseted before adding
         * new contents.
         */
        IMPORT_C static void UnpackContactIdsL(
            const TDesC8& aContactIdsPckg,
            CContactIdArray& aContactIds );



    private: // Prohibited constructors

        PEngContactItemExtensionCodec2();
        ~PEngContactItemExtensionCodec2();

    };



#endif      // __PENGCONTACTLISTEXTENSIONS2_H

// End of File


