/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Custom listbox component for the contact list
*
*/


#ifndef CCACONTACTLISTBOXMODEL_H
#define CCACONTACTLISTBOXMODEL_H

#include <aknutils.h>

class MCAMainViewArrayPC;

/**
 *  Custom listbox model for the contact list
 *
 *  Enables custom filtering (with find box) for the listbox,
 *  so it's possible to show also contact list names for the
 *  filtered contacts.
 *
 *  @lib chat.exe
 *  @since S60 v3.0
 */
class CCAContactListBoxModel : public CAknFilteredTextListBoxModel
    {

    public: // Constructor and Destructor


        /**
        * Default C++ constructor
        */
        CCAContactListBoxModel();

        /**
        * Default C++ destructor
        */
        ~CCAContactListBoxModel();


    public: // from CTextListBoxModel

        /**
        * Returns the amount of filtered (searched) contacts and their contact lists
        * @return Total amount of filtered (searched) contacts and their contact lists
        */
        TInt NumberOfItems() const;

        /**
        * Returns the text of the specified item.
        * @param aItemIndex Index to the item.
        * @return The text of item in given index.
        */
        TPtrC ItemText( TInt aItemIndex ) const;


    public: // from MEikTextListBoxModel

        /**
        * Provides the text array for the find box to search for.
        * @return Pointer to the text array.
        */
        const MDesCArray* MatchableTextArray() const;


    public: // From MDesCArray

        /**
        * @see MDesCArray
        */
        TInt MdcaCount() const;

        /**
        * @see MDesCArray
        */
        TPtrC MdcaPoint( TInt aIndex ) const;


    public: // New methods

        /**
        * For setting the contactlist model to have a direct access to real model
        * @param aContactListModel Model
        */
        void SetContactListModel( MCAMainViewArrayPC& iMainViewArrayPC );

        /**
        * Return the real index of item which is currently visible
        * @param aVisibleItemIndex Currently visible item index
        * @return index of the item in the non-filtered list
        */
        TInt FilteredItemIndex( TInt aVisibleItemIndex ) const;

        /**
        * Return the current index of item in the non-filtered list
        * @param aOriginalIndex Index of the item in non-filtered list
        * @return Index of the item in filtered list. KErrNotFound if not visible
        */
        TInt VisibleItemIndex( TInt aOriginalIndex ) const;

    protected:  // New methods

        /**
        * Calculates the currently visible items.
        * SLOW
        * @return count of visible items
        */
        TInt CountItems() const;

        /**
        * Fetch the item text a given position
        * SLOW
        * @param aIndex Position of the item
        * @return Item text
        */
        TPtrC ItemAt( TInt aIndex ) const;

        /**
        * Check if filter is active.
        * @return ETrue: active, EFalse: deactive
        */
        TBool IsFilterActive() const;

        /**
        * Check if item at given index should be visible contact list.
        * Meaning that it has to be a contact list and it needs
        * to have at least 1 visible contact in it.
        * @param aIndex Position on item
        * @param aPosArray array of contacts lists positions
        * @return ETrue: visible contact list, EFalse: something else
        */
        TBool IsContactList( TInt aIndex, RArray<TInt>& aPosArray ) const;


    private: // Data

        // Not own. Contact list model
        MCAMainViewArrayPC* iMainViewArrayPC;
    };


#endif // CCACONTACTLISTBOXMODEL_H
