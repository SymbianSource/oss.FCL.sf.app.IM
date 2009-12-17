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
* Description:  Domain selection query pop-up
*
*/


#ifndef C_CCNUIDOMAINSELECTIONQUERYDIALOG_H
#define C_CCNUIDOMAINSELECTIONQUERYDIALOG_H

#include <aknlistquerydialog.h>

class CFbsBitmap;
class CIMPSSAPSettings;

/**
 *  Domain selection query pop-up
 *
 *  @lib IMPSConnectionUi.lib
 *  @since S60 v3.1
 */
class CCnUiDomainSelectionQueryDialog : public CAknListQueryDialog
    {

    public:

        /**
         * Symbian constructor.
         * @param aIndex    After query is dismissed, aIndex will hold
         *                  the value of selected item.
         * @param aSap      Current SAP in use, can be NULL.
         */
        static CCnUiDomainSelectionQueryDialog* NewL(
            TInt* aIndex,
            CIMPSSAPSettings* aSap = NULL );

        /**
         * Destructor.
         */
        virtual ~CCnUiDomainSelectionQueryDialog();

    public:

// from base class CAknListQueryDialog

        /**
         * Sets item text array to listbox.
         * @since S60 v3.1
         * @param aItemTextArray    Item text array to be added
         */
        void SetItemTextArray( MDesCArray* aItemTextArray );

        /**
         * Sets ownership type of item array.
         * @since S6 v3.1
         * @param aOwnershipType    Type of ownership.
         */
        void SetOwnershipType( TListBoxModelItemArrayOwnership aOwnershipType );

    protected:

// from base class CAknListQueryDialog

        /**
         * Handle key events.
         * @since S60 v3.1
         * @param aKeyEvent Key event.
         * @param aType     Type of the event.
         * @return          Indicates whether or not the key event was used by this control.
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
         * This function is called by application framework just
         *      before the dialog is activated.
         * @since S60 v3.1
         */
        void PreLayoutDynInitL();

        /**
         * This function is called by tapplication framework when user activates button.
         * @since S60 v3.1
         * @param aButtonId ID of button that was activated.
         * @return          Should return ETrue if dialog should exit,
         *                  and EFalse if it should not.
         */
        TBool OkToExitL( TInt aButtonId );

    private:

        /**
         * Constructor.
         * @param aIndex After query is dismissed, aIndex will hold
         *               the value of selected item.
         */
        CCnUiDomainSelectionQueryDialog( TInt* aIndex,
                                         CIMPSSAPSettings* aSap );

        /**
         * Symbian second phase constructor.
         */
        void ConstructL();

        /**
         * Update radio button in query.
         * @since S60 v3.1
         * @param aSelectedIndex index of currently selected item.
         */
        void UpdateRadioButtonL( TInt aSelectedIndex, TBool aForceUpdate = EFalse );

        /**
        * Calls ResetAndDestroy to given array
        * @since 3.1
        * @param aObject Array
        */
        static void DestroyCloseArray( TAny* aObject );

    private: // data

        /**
         * Bitmaps for list icons.
         * Owns.
         */
        CFbsBitmap* iRadioButton;
        CFbsBitmap* iRadioButtonMask;
        CFbsBitmap* iEmpty;
        CFbsBitmap* iEmptyMask;

        /**
         * Item text array.
         * Owned by default, ownership can be dropped by
         * calling SetOwnershipType with parameter ELbmDoesNotOwnItemArray.
         */
        MDesCArray* iItemTextArray;

        /**
         * Ownership of item text array.
         * ELbmOwnsItemArray by default.
         */
        TListBoxModelItemArrayOwnership iOwnershipType;

        /**
         * Ownership indicated by iSapOwned.
         * Pointer to current SAP, can be NULL.
         */
        CIMPSSAPSettings* iSap;

        /**
         * Indicates wether iSap is owned or not.
         */
        TBool iSapOwned;
    };

#endif // C_CCNUIDOMAINSELECTIONQUERYDIALOG_H
