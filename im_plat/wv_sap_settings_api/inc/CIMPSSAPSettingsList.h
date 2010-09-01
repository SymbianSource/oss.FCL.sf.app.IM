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
* Description:  "Light weight" SAP list.
*
*/

#ifndef __CIMPSSAPSETTINGSLIST_H
#define __CIMPSSAPSETTINGSLIST_H

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>
#include <cimpssapsettingslistitem.h>


// CLASS DECLARATION
/**
 * List of CIMPSSAPSettingsListItem's. List owns its list items.
 *
 * CIMPSSAPSettingsList has all CArrayPtrSeg's functionality and also couple
 * useful shortcuts to list items data. CIMPSSAPSettingsList also implements
 * MDesCArray interface and on MdcaPoint() method, it returns SAP's name.
 *
 * @lib WVSAPSettingsStore.dll
 * @since 3.0
 */

class CIMPSSAPSettingsList : public CArrayPtrSeg< CIMPSSAPSettingsListItem >,
            public MDesCArray
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since 3.0
         */
        IMPORT_C static CIMPSSAPSettingsList* NewL();

        /**
         * Two-phased constructor.
         *
         * @since 3.0
         */
        IMPORT_C static CIMPSSAPSettingsList* NewLC();


        /**
         * Destructor.
         * Destroys list items.
         */
        IMPORT_C virtual ~CIMPSSAPSettingsList();


    private:  // Constructor

        /**
         * C++ default constructor.
         */
        CIMPSSAPSettingsList();


    public:  // New methods
        /**
         * Returns a certain list items index in the array.
         * Indexation is similar as in MDesCArray.
         *
         * @param aUid Uid which index to get.
         * @return Desired listItems index. If given uid isn't found from the
         *         array, returns KErrNotFound.
         *
         * @since 3.0
         */
        IMPORT_C TInt IndexForUid( TUint32 aUid ) const;


        /**
         * Returns a list items uid value.
         * Indexation is similar as in MDesCArray.
         *
         * @param aIndex Index which uid to get.
         *        If index is out of bounds, function raises
         *        E32USER-CBase 21 panic.
         * @return Desired listItems uid.
         *
         * @since 3.0
         */
        IMPORT_C TUint32 UidForIndex( TInt aIndex ) const;


        /**
         * Gets pointer to the list item having a specified aUid.
         *
         * Note!! If item having a such uid isn't found from list,
         * returns NULL.
         *
         * @param aUid Uid of item to get.
         * @return Pointer to the item having specified uid, or NULL.
         * ListItem ownership remains on the list.
         *
         * @since 3.0
         */
        IMPORT_C const CIMPSSAPSettingsListItem* ListItemForUid( TUint32 aUid ) const;


        /**
         * Finds the position of an item within the array, based on the items name.
         * If the array has items with duplicate names, the function cannot
         * guarantee which element, with the given name, it returns.
         *
         * @param aName The name to find.
         * @param aIndex If the item is found, the reference is set to
         *               the position of that element within the array. The position
         *               is relative to zero, (i.e. the first element in the array
         *               is at position 0). If the element is not found and the
         *               array is not empty, then the reference is set to the position
         *               of the first element in the array with a name which is greater
         *               than the given name. If the element is not found and
         *               the array is empty, then the reference is set to zero.
         *
         * @return Zero, if the item with the specified name is found.
         *         Non-zero, if the item with the specified name isn't found.
         *
         * @since 3.0
         */
        IMPORT_C TInt FindNameL( const TDesC& aName, TInt& aIndex ) const;


        /**
         * Delete elements by position.
         *
         * Deletes element, frees its resource reservations and also
         * removes element from the list.
         *
         * Deleting elements from the array does not cause the array buffer to
         * be automatically compress. Call CArrayFixBase::Compress() to return
         * excess space to the heap.
         *
         * @param aIndex  The element to be deleted. Indexation is relative
         * to zero; i.e. zero implies the first element in the list.
         * Index must not be negative and must not be greater than the
         * number of elements currently in the array, otherwise the
         * function raises a panic.
         *
         * @since 3.0
         */
        IMPORT_C void Delete( TInt aIndex );


        /**
         * Delete elements by position.
         *
         * Similar as Delete( TInt aIndex ) except deletes aCount
         * elements instead of just one.
         *
         * @param aIndex  The position within the array from where
         * deletion of elements is started. Indexation is relative
         * to zero; i.e. zero implies the first element in the list.
         * Index must not be negative and must not be greater than the
         * number of elements currently in the array, otherwise the
         * function raises a panic.
         *
         * @param aCount  The number of contiguous elements to
         * delete from the array. The value must not be negative, and
         * index + count must not be greater than the number of elements
         * currently in the array. Otherwise the function raises
         * a panic.
         *
         * @since 3.0
         */
        IMPORT_C void Delete( TInt aIndex, TInt aCount );


    public:  // From MDesCArray

        /**
         * From MDesCArray.
         *
         * Returns lists element count.
         * @return Count of list items.
         *
         * @since 3.0
         */
        IMPORT_C TInt MdcaCount() const;

        /**
         * From MDesCArray.
         *
         * Returns a list items name.
         * @return Desired listItems name value.
         *
         * @since 3.0
         */
        IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

    };

#endif  // __CIMPSSAPSETTINGSLIST_H


//  End of File



