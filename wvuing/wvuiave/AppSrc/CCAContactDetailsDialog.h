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
* Description:  Declares contact details dialog
 *
*/


#ifndef CCACONTACTDETAILSDIALOG_H
#define CCACONTACTDETAILSDIALOG_H

//  INCLUDES
#include <aknform.h>
#include <eiklbo.h>
#include <bldvariant.hrh>

// FORWARD DECLARATIONS
class CCAStatusPaneHandler;
class CAknDoubleStyleListBox;
class MCAContactEditPC;

// CLASS DECLARATION
/**
 *  This class declares dialog for contact details
 *  @lib chatNG.app
 *  @since 3.2
 */
class CCAContactDetailsDialog : public CAknDialog,
            public MEikListBoxObserver
    {

    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         * @since 3.2
         * @see CCAContactDetailsDialog::NewL
         */
        CCAContactDetailsDialog(
            TBool& aWasSaved,
            const HBufC* aListId,
            const TDesC& aNickname,
            TInt aIndex );

        /**
         * Destructor
         */
        virtual ~CCAContactDetailsDialog();

        /**
         * Symbian default constructor.
         * @param aWasSaved ETrue, if the contact was saved (if edited)
         * otherwise show the contact details
         * @param aIndex The index of the contact to edit/view
         * @param aListId The list id (must be zero length, but valid string, if no list)
         * @param aNickname Nickname (optional)
         * @since 3.2
         */
        static CCAContactDetailsDialog* NewL(
            TBool& aWasSaved,
            TInt aIndex,
            const TDesC& aListId,
            const TDesC& aNickname = KNullDesC );

        /**
         * 2nd level constructor for this class
         * @since 3.2
         * @see CCAContactDetailsDialog::NewL
         */
        void ConstructL();

    public: // new methods

        /**
         * Invoke the dialog.
         * Just a wrapper for ExecuteLD with proper resource ID,
         * to make it simpler for callers.
         * @since 3.2
         */
        void ShowLD();

        /**
         * Help Key Support
         * This function is called by the Framework to get the context to launch
         * Help
         *
         * @param aContext	The context that contains the appid and the help id.
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    private: //new functions

        /**
         * Update the internal item text array with the data from the current
         * contact
         * @since 3.2
         */
        void UpdateItemTextArrayL();

        /**
         * Helper for UpdateItemTextArrayL.
         * Appends correctly formatted text to the given item text array.
         * @param aItemTextArray The item text array
         * @param aStringResource The string resource for the label
         * @param aText The text of the entry
         * @since 3.2
         */
        void AppendToItemArrayL(
            CDesCArray& aItemTextArray,
            TInt aStringResource,
            const TDesC& aText );

        /**
         * Maps the given list box item index to the Contact Editor control id.
         * If the currently selected list box item index is unavailable in the
         * control editor, the method will find the next suitable one.
         * @param aItemIndex The listbox item index
         * @return The control id in the editor
         * @since 3.2
         */
        TInt MapIndexToControlId( TInt aItemIndex );

        /**
         * Update the title bar text and icon
         * @since 3.2
         */
        void UpdateTitleBarL();

        /**
         * Deletes currently viewed contact.
         * @return General error code or KErrNone.
         * @since 3.2
         */
        TInt DeleteCurrentContactL();

        /**
        * Launch contact editor with the currently focused item.
        * @since 3.2
        * @return None
        */
        void LaunchContactEditorL();


    private:	//From MEikCommandObserver

        /**
         * Processes user commands.
         * @see MEikCommandObserver
         */
        void ProcessCommandL( TInt aCommand );

    private: //from MEikMenuObserver

        /**
         * Dynamically initialises menu pane
         * @see MEikMenuObserver
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    protected:  // Functions from CEikDialog

        /**
         * From CEikDialog Sets the layout for dialog
         */
        void PreLayoutDynInitL();

        /**
         * From CEikDialog. This method is called when buttons are pressed
         * @param aButtonId Button id, if OK-button then dialog is dismissed
         * @return EFalse if cancel is pressed
         */
        TBool OkToExitL( TInt aButtonId );

        /**
         * From CCoeControl. Handles the keyevents
         * @param aKeyEvent The key event.
         * @param aType The type of key event: EEventKey,
         *              EEventKeyUp or EEventKeyDown.
         * @return Indicates whether or not the key event
         *                   was used by this control
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
         * From MEikListBoxObserver, Handles event's generated by listbox
         * @param aListBox Pointer to listbox from where this event originated
         * @param aEventType Type of event received.
         */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

        /**
        * @see CAknDialog
        * @since S60 v3.1
        */
        void SetSizeAndPosition( const TSize& aSize );

    private:    // Data

        //Handle to titlepane. doesn't own
        CCAStatusPaneHandler* iTitlePane;

        // Owns. Contact's list ID
        const HBufC* iListId;

        // nickname
        const TDesC& iNickname;

        // owned
        CDesCArrayFlat* iItemTextArray;

        // ETrue if contact was saved by invocation of Contact Editor,
        // used by caller to optimize refresh
        TBool& iWasSaved;

        // variated functionality
        // alias support
        TBool iAliasEnabled;
        // nickname support
        TBool iNicknameEnabled;

        // viewing own data (ETrue)
        TBool iOwnData;

        // Does contact still exist
        TBool iValidContact;

        // Owns. Contacts id
        HBufC* iContactId;

        // map listbox index to given akn form control id.
        // always contains "all" list box indexes which
        // the details dialog can show. empty items are
        // marked with EChatEditDummy.
        RArray<TInt> iIndexToControlIdMapping;

        //Does'nt own - process component object for contact
        MCAContactEditPC *iCCAContactEditPC;

        //index of the contact chosen w.r.t MainViewArrayPC
        //the contact to edit.
        TInt iIndexOfContact;
        
        //the contact is deleting
        TBool iMovingFlag;
    };

#endif      // CCACONTACTDETAILSDIALOG_H   

// End of File
