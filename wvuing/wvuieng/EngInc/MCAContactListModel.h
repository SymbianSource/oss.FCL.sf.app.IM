/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contact list model for UI
*
*/



#ifndef MCACONTACTLISTMODEL_H
#define MCACONTACTLISTMODEL_H

//  INCLUDES
#include "TStorageManagerGlobals.h"

#include <e32base.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class MCAContactList;
class MCAStoredContact;

// CLASS DECLARATION

/**
 *  Contact list model for UI.
 *  Wrapper between UI and storage, hides multiple contact list handling logic.
 *
 *  @lib CAEngine.lib
 *  @since Series 60 3.0
 */
class MCAContactListModel : public MDesCArray
    {

    public:	// Enumerations and typedefs

        enum TItemType
            {
            EInvalid,
            EOwnStatus,
            EContactList,
            EContactItem,
            EAll
            };

        struct SItem
            {
            TItemType iType;
            MCAContactList* iContactList;
            MCAStoredContact* iContact;
            };

    public: // New functions

        /**
         * @return Count of items
         */
        virtual TInt Count() const = 0;

        /**
         * Returns List item at given index.
         * @param aIndex
         * @return List item
         */
        virtual SItem Item( TInt aIndex ) const = 0;

        /**
         * Should the model show "All" item in beginning
         * of each contact list.
         * @param aShow If ETrue, first item in every contact list is "All"
         */
        virtual void ShowAll( TBool aShow ) = 0;

        /**
         * Get index for contact.
         * @param aContact. Contact item to be found.
         * @return Index of contact. If contact not found returns KErrNotFound
         */
        virtual TInt IndexOfContact( MCAStoredContact* aContact ) const = 0;

        /**
         * Get index for list
         * @param aList. Contact item to be found.
         * @param aIgnoreOwnItem. If ETrue own item is not taken into account
         *                        when resolving index.
         * @param aIgnoreEmptyLists If ETrue, empty lists are ignored.
         * @return Index of list. If list not found returns KErrNotFound
         */
        virtual TInt IndexOfList( MCAContactList* aList,
                                  TBool aIgnoreOwnItem = EFalse,
                                  TBool aIgnoreEmptyLists = ETrue ) const = 0;

        /**
         * Set selection mode of contact list model.
         * @param aSelectionMode. ETrue selection mode on. EFalse -> off.
         */
        virtual void SetSelectionMode( TBool aSelectionMode ) = 0;

        /**
         * Set primary contact list properties in use. Secondary is used
         * for example with selection mode.
         * @param aPrimaryInUse. ETrue primary. EFalse secondary.
         */
        virtual void SetPrimaryContactLists( TBool aPrimaryInUse ) = 0;

        /**
         * Resets default filter.
         */
        virtual void ResetDefaultFilter() = 0;

        /**
         * Sets default filter.
         * @param aFilter filter set to be default.
         */
        virtual void SetDefaultFilter( TStorageManagerGlobals::TFilterType aFilter ) = 0;

        /**
         * Sets filter.
         * @param aFilter filter set.
         */
        virtual void SetFilter( TStorageManagerGlobals::TFilterType aFilter ) = 0;

        /**
         * @return Filter in use.
         */
        virtual TStorageManagerGlobals::TFilterType Filter() const = 0;

        /**
        * Force the model in a state where every list is virtually
        * expanded. List's real expand/collapse state is kept intact.
        * @param aAllExpanded   ETrue: All list's are expanded
        *                       EFalse: Default behaviour
        */
        virtual void ForceExpanded( TBool aAllExpanded ) = 0;

        /**
        * Return the state of forced expand.
        * @return ETrue if force expand is active, EFalse othewise.
        */
        virtual TBool IsForceExpanded() const = 0;

        /**
         * Construct an array of contact list positions in model.
         * aPositionArray is cleared before appending positions!
         * @param aPositionArray array in which to store positions
         * @return none
         */
        virtual void MapContactListPositions( RArray<TInt>& aPositionArray ) = 0;

        /**
         * Sets the correct sorting method to storage
         * according to current setting value
         */
        virtual void SetSort() = 0;

    public:  // From MDesCArray

        /**
        * @see MDesCArray
        */
        TPtrC MdcaPoint( TInt aIndex ) const = 0;

        /**
        * @see MDesCArray
        */
        TInt MdcaCount() const = 0;


    protected:	// prevent deletion through this interface

        /**
         * Destructor.
         */
        virtual ~MCAContactListModel() {};

    };

#endif      // MCAContactListModel_H

// End of File
