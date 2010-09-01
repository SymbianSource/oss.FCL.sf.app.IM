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



#ifndef CCACONTACTLISTMODEL_H
#define CCACONTACTLISTMODEL_H

//  INCLUDES
#include "MCAContactListModel.h"
#include "MCASettingsObserver.h"


// FORWARD DECLARATIONS
class MCAStoredContacts;
class MCASettings;

// CLASS DECLARATION

/**
 *  Contact list model for UI.
 *  Wrapper between UI and storage, hides multiple contact list handling logic.
 *
 *  @lib CAEngine.lib
 *  @since Series 60 3.0
 */
class CCAContactListModel : public CBase,
            public MCAContactListModel,
            public MCASettingsObserver
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aSettingsInterface Settings interface
         */
        static CCAContactListModel* NewL( MCASettings& aSettingsInterface );

        /**
         * Destructor.
         */
        virtual ~CCAContactListModel();


    protected: // Functions from MCAContactListModel

        /**
         * @return Count of items
         */
        TInt Count() const;

        /**
         * Returns List item at given index.
         * @param aIndex
         * @return List item
         */
        SItem Item( TInt aIndex ) const;

        /**
         * Should the model show "All" item in beginning
         * of each contact list.
         * @param aShow If ETrue, first item in every contact list is "All"
         */
        void ShowAll( TBool aShow );

        /**
         * @see MCAContactListModel
         */
        TInt IndexOfContact( MCAStoredContact* aContact ) const;

        /**
         * @see MCAContactListModel
         */
        TInt IndexOfList( MCAContactList* aList,
                          TBool aIgnoreOwnItem /*= EFalse*/,
                          TBool aIgnoreEmptyLists /*= ETrue*/ ) const;

        /**
         * Set selection mode on/off
         * @param aSelectionMode ETrue if on. EFalse if off. Default is off.
         */
        void SetSelectionMode( TBool aSelectionMode );

        /**
         * Set primary contact lists in use
         */
        void SetPrimaryContactLists( TBool aPrimaryInUse );

        /**
         * Resets default filter.
         */
        void ResetDefaultFilter();

        /**
         * Sets default filter.
         * @param aFilter filter set to be default.
         */
        void SetDefaultFilter( TStorageManagerGlobals::TFilterType aFilter );

        /**
         * Sets filter.
         * @param aFilter filter set.
         */
        void SetFilter( TStorageManagerGlobals::TFilterType aFilter );

        /**
         * @return Filter in use.
         */
        TStorageManagerGlobals::TFilterType Filter() const;

        /**
        * Force the model in a state where every list is virtually
        * expanded. List's real expand/collapse state is kept intact.
        * @param aAllExpanded   ETrue: All list's are expanded
        *                       EFalse: Default behaviour
        */
        void ForceExpanded( TBool aAllExpanded );

        /**
        * Return the state of forced expand.
        * @return ETrue if force expand is active, EFalse othewise.
        */
        TBool IsForceExpanded() const;

        /**
         * @see MCAContactListModel
         */
        void MapContactListPositions( RArray<TInt>& aPositionArray );

        /**
         * Sets the correct sorting method to storage
         * according to current setting value
         */
        void SetSort();

    protected: // Functions from MCASettingsObserver

        /**
         * Handles setting changes.
         * @param aChangedSettingEnum specifies the changed setting.
         */
        void HandleSettingsChangeL( TInt aChangedSettingEnum );

    protected:  // Functions from MDesCArray

        /**
        * @see MDesCArray
        */
        TInt MdcaCount() const;

        /**
        * @see MDesCArray
        */
        TPtrC MdcaPoint( TInt aIndex ) const;


    private: // new functions



    private:

        /**
         * C++ default constructor.
         * @param aSettingsInterface Settings interface
         */
        CCAContactListModel( MCASettings& aSettingsInterface );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:    // Data

        // doesn't own, storage interface for contacts
        MCAStoredContacts* iContacts;

        // reference to settings interface
        MCASettings& iSettings;

        // is "all" item displayed in beginning of each list
        TBool iShowAll;

        /// selection-mode on/off
        TBool iSelectionMode;

        // type of filtering contacts
        TStorageManagerGlobals::TFilterType iFilter;

        // type of filtering contacts, can be restored
        TStorageManagerGlobals::TFilterType iDefaultFilter;

        // Are all the contact lists forced to expanded state.
        // List's original states are preserved and they are only shown
        // expanded
        TBool iExpandAll;
    };

#endif      // CCAContactListModel_H

// End of File
