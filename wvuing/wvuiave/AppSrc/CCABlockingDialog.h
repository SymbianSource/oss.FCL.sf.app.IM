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
* Description:  Container class for search view
 *
*/


#ifndef CCABLOCKINGDIALOG_H
#define CCABLOCKINGDIALOG_H


/* Dialogs for selection service implementation.
 *
 */

#include <aknform.h>
#include <aknview.h>
#include <aknutils.h>
#include <aknpopup.h>
#include <avkon.rsg>

#include "impsbuilddefinitions.h"

// FORWARD DECLARATIONS
class CCAStatusPaneHandler;

/** CCABlockingDialog is the actual interface to the applications.
 *
 * Use this class to get a full screen list.
 *
 * This class only works in whole main pane. Do not try to use
 * this directly for other places.
 *
 * resource definition for this class:
 * RESOURCE DIALOG r_res_id_for_a_dialog
 *   {
 *   flags = EAknDialogSelectionList;
 *   buttons = R_AVKON_SOFTKEYS_OPTIONS_BACK;
 *   items =
 *	{
 *	DLG_LINE
 *	    {
 *	    type = EAknCtSingleGraphicListBox;
 *	    id = ESelectionListControl;
 *	    control = LISTBOX
 *		{
 *		flags = EAknListBoxSelectionList;
 *		};
 *	    }
 *     // the next dlg line is optional.
 *	,
 *	DLG_LINE
 *	    {
 * 	    itemflags = EEikDlgItemNonFocusing;
 *	    id = EFindControl;
 *	    type = EAknCtSelectionListFixedFind;
 *	    }
 *	};
 *   }
 *
 * The listbox type can be one of the following: (avkon.hrh and aknlists.h)
 *   EAknCtSingleListBox                (See CAknSingleStyleListBox)
 *   EAknCtSingleNumberListBox          (See CAknSingleNumberStyleListBox)
 *   EAknCtSingleHeadingListBox         (See CAknSingleHeadingStyleListBox)
 *   EAknCtSingleGraphicListBox         (See CAknSingleGraphicStyleListBox)
 *   EAknCtSingleGraphicHeadingListBox  (See CAknSingleGraphicHeadingStyleListBox)
 *   EAknCtSingleNumberHeadingListBox   (See CAknSingleNumberHeadingStyleListBox)
 *   EAknCtSingleLargeListBox           (See CAknSingleLargeStyleListBox)
 *   EAknCtDoubleListBox                (See CAknDoubleStyleListBox)
 *   EAknCtDoubleNumberListBox          (See CAknDoubleNumberStyleListBox)
 *   EAknCtDoubleTimeListBox            (See CAknDoubleTimeStyleListBox)
 *   EAknCtDoubleLargeListBox           (See CAknDoubleLargeStyleListBox)
 *   EAknCtDoubleGraphicListBox         (See CAknDoubleGraphicStyleListBox)
 *
 * The type field while defining find can be one of the following:
 *   EAknCtSelectionListFixedFind
 *   EAknCtSelectionListPopupFind
 *
 * The menubar you give for selection list dialog should have
 * one of the following as one of its menu panes: (avkon.hrh)
 *    R_AVKON_MENUPANE_SELECTION_LIST
 *    R_AVKON_MENUPANE_SELECTION_LIST_WITH_FIND_POPUP
 *
 * RESOURCE MENU_BAR r_res_id_for_a_menubar
 * {
 * titles =
 *     {
 *     MENU_TITLE { menu_pane = R_AVKON_MENUPANE_SELECTION_LIST; }
 *     };
 * };
 *
 * C++ Usage:
 *    TInt openedItem = 0;
 *    MDesCArray *array = ...;
 *    CCABlockingDialog *dialog = CCABlockingDialog::NewL(openedItem, array, R_RES_ID_FOR_A_MENUBAR);
 *    TInt result = dialog->ExecuteLD(R_RES_ID_FOR_A_DIALOG);
 *    if (result) { ...use openeditem here... } else { ...canceled... }
 *
 * Alternatively, you can use:
 *    TInt openedItem = 0;
 *    MDesCArray *array = ...;
 *    CCABlockingDialog *dialog = CCABlockingDialog::NewL(openedItem, array, R_RES_ID_FOR_A_MENUBAR);
 *    dialog->PrepareLC(R_RES_ID_FOR_A_DIALOG);
 *    // do some operations here, for example fill icon array
 *    TInt result = dialog->RunLD();
 *    if (result) { ... } else { ... }
 *
 * Often it is also useful to derive from CCABlockingDialog and implement OkToExitL(), constructors and the NewL() methods.
 * OkToExitL() implementation helps with providing navigation with other dialogs; OkToExitL() is ideal place to launch
 * new dialogs when a list item is selected. This way when backstepping, the state of the first dialog is preserved...
 */
class CCABlockingDialog : public CAknDialog, public MEikListBoxObserver
    {

    public: // operands for selection lists
        /** CCABlockingDialog::NewL()
         *
         * aOpenedItem   Variable to be modified when user selects a list item.
         * aArray        Content of list items; A tab-separated string with texts and indexes to icon array
         * aMenuBarResourceId Menu items to be shown in options menu
         * aCommand      Callback for state changes. EAknCmdOpen command is send by listbox. Options menu commands come here too.
         * @param aDontShowTabGroup For navipane control.
         */
        static CCABlockingDialog *NewL( TInt &aOpenedItem,
                                        MDesCArray *aArray,
                                        TInt aMenuBarResourceId,
                                        TInt aOkMenuBarResourceId,
                                        MEikCommandObserver *aCommand = 0,
                                        TBool aDontShowTabGroup = EFalse );
        static CCABlockingDialog *NewLC( TInt &aOpenedItem,
                                         MDesCArray *aArray,
                                         TInt aMenuBarResourceId,
                                         TInt aOkMenuBarResourceId,
                                         MEikCommandObserver *aCommand = 0,
                                         TBool aDontShowTabGroup = EFalse );

    public: // different options
        enum TFindType
            {
            ENoFind,
            EFixedFind,
            EPopupFind
            };
        /** SetupFind() provides a way to enable and disable find and find popup
         *  on runtime.
         *
         * You still need entry with id EFindControl to resource file for the find, this is only for disabling
         * existing find element.
         */
        void SetupFind( TFindType aType );


        /* GetFindBoxTextL() returns the text in the FindPane, if the FindPane is empty
         * returns KNullDesC;
         * @return const TDesC&
         */
        const TDesC& GetFindBoxTextL() ;

        /** Icons: IconArray(), SetIconArrayL()
         *
         * Icons, images and thumbnails are in this array.
         *
         * The list items are tab separated strings with fixed format. Some of the numbers
         * in the list item strings are indexes to this array.
         *
         * This array can be modified by MDesCArray::MdcaPoint() method or at construction of
         * dialog; after PrepareLC() call.
         */
        CArrayPtr<CGulIcon>* IconArray() const;
        void SetIconArrayL( CArrayPtr<CGulIcon>* aIcons );

        /**
         * refresh the block dialog title
         */
        void RefreshDialogTitleL();

        /**
         * To handle the deletiontem to the list box
         */
        void HandleItemRemovalL();

        /**
         * To handle the addition of an item to the list box
         */
        void HandleItemAdditionL();

        /**
         * Returns pointer to the listbox being used.
         */
        virtual CEikListBox *ListBox() const;

        // Destructor
        ~CCABlockingDialog();

        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /*
         * Set the iExitEnable a new value
         * @param aValue The new value to be set
         */
        void SetExitEnabled( TBool aValue );

    protected:
        CCABlockingDialog( TInt &aIndex,
                           MDesCArray *aArray,
                           TInt aMenuBarResourceId,
                           TInt aOkMenuBarResourceId,
                           MEikCommandObserver *aCommand,
                           TBool aDontShowTabGroup );
    public: // Customisation from derived classes
        /** SelectionListProcessCommandL() handles selection list and markable list
         *  default commands.
         *
         *  For markable lists, this method handles EAknCmdMark, EAknCmdUnmark, EAknMarkAll, EAknUnmarkAll
         *  defined in options menu pane R_AVKON_MENUPANE_MARKABLE_LIST.
         */
        virtual void SelectionListProcessCommandL( TInt aCommand );

    protected:
        /** IsAcceptableListType(): Detection of list and grid layouts
         *
         * The CCCABlockingDialog only works with certain list and grid
         * layouts.
         *
         * You will get Panic() if you use your own list/grid layouts and
         * you do not have this method implemented!
         *
         * If you add new list layouts, you should implement this method to
         * publish the type of the layout using this method. This is especially
         * the case where you use CAknSelectionGrid with your own grid layout.
         * (as there are no predefined grid layouts, you need to do this
         * every time you use a selection grid)
         *
         * The following aControlTypes are already implemented and requires
         * no action:
         * EAknCtSingleListBox
         * EAknCtSingleNumberListBox
         * EAknCtSingleHeadingListBox
         * EAknCtSingleGraphicListBox
         * EAknCtSingleGraphicHeadingListBox
         * EAknCtSingleNumberHeadingListBox
         * EAknCtSingleLargeListBox
         * EAknCtDoubleListBox
         * EAknCtDoubleNumberListBox
         * EAknCtDoubleTimeListBox
         * EAknCtDoubleLargeListBox
         * EAknCtDoubleGraphicListBox
         * EAknCtSettingListBox
         * EAknCtSettingNumberListBox
         *
         * Any other layout requires you to inherit from CCCABlockingDialog
         * and implement the following methods:
         * TBool IsAcceptableListBoxType(TInt aControlType, TBool &aIsFormattedCellList)
         * {
         *   if (aControlType == EMyCtGridLayout)
         *   {
         *      // CAknGrid is-a formattedcelllistbox.
         *      aIsFormattedCellList = ETrue;
         *      return ETrue;
         *   }
         *   else
         *      return EFalse;
         * }
         *
         * See also CreateCustomControlL(). IsAcceptableListType() and CreateCustomControlL() forms a pair that should
         * be implemented together.
         */
        virtual TBool IsAcceptableListBoxType( TInt aControlType, TBool &aIsFormattedCellList ) const;

        /**
         * Second phase constructor.
         */
        void ConstructL( TInt aMenuBarResourceId );

    protected:

        /**
         * Processes user commands.
         * @see MEikCommandObserver
         */
        void ProcessCommandL( TInt aCommandId );

        /**
         * From CEikDialog : Prepare for opening dialog.
         */
        void PreLayoutDynInitL();

        /**
         * @see CAknDialog
         * @since S60 v3.1
         */
        void SetSizeAndPosition( const TSize &aSize );

        /**
         * @see CAknDialog
         * @since S60 v3.1
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

        /**
         * From CEikDialog. Called when dialog is dismissed
         * @see CEikDialog
         */
        TBool OkToExitL( TInt aButtonId );

        /**
         * Handles list box event
         */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

        /**
         * From CoeControl, Returns the number of control contained by this
           * class.
         * @see CCoeControl
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl, Returns handle to control pointed by aIndex
         * @param aIndex Wanted control's index [0..n]
         * @return Handle to wanted control
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * From CCoeControl. Handles the keyevents
         * @param aKeyEvent The key event.
         * @param aType The type of key event: EEventKey,
         *              EEventKeyUp or EEventKeyDown.
         * @return Indicates whether or not the key event
         *                   was used by this control
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /** CreateCustomControlL() creates own list or grid layouts.
         *
         * Normal implementation of this method is:
         * SEikControlInfo CreateCustomControlL(TInt aControlType)
         * {
         *     CCoeControl *control = NULL;
         *     if (aControlType == EMyCtGridLayout)
         *     {
         *        // CMyOddStyleGrid should be derived from CAknFormattedCellListBox or CEikColumnListBox.
         *        control = new(ELeave)CMyOddStyleGrid;
         *     }
         *     SEikControlInfo info = {control,0,0};
         *     return info;
         * }
         * See also IsAcceptableListType(). CreateCustomControlL() and IsAcceptableListType() forms a pair that should
         * be implemented together.
         */
        SEikControlInfo CreateCustomControlL( TInt aControlType );
    
    /**
     * From CoeControl. Called when focus changed
     * @see CCoeControl
     */
    void FocusChanged(TDrawNow aDrawNow); 
    
    private:
        /**
         * ResetFindBoxL : clear all character from findbox
         */
        void ResetFindBoxL();

        /**
        * Layouts current components according to AVKON LAF
        */
        void SetLayout();

    protected:

        // Returns the type do findbox being used.
        CAknSearchField *FindBox() const;

        TBool IsFormattedCellListBox() const;

        void Draw( const TRect& ) const;

        /**
         * From MEikMenuObserver, Called by framework before
         * constructing menupane
         * @param aResourceId Resource to be constructed
         * @param aMenuPane Pointer to menupane being constructed
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
         * Show context sensitive menubar
         */
        void ShowContextMenuL();

    protected:
        TBool iEnterKeyPressed;
    private:

        TFindType iFindType;
        TInt *iSelectedItem;

        //Does not own
        MDesCArray *iArray;

        //Does not own
        MEikCommandObserver *iCmdObserver;

        TInt iDialogResourceId;
        TInt iMenuBarResourceId;
        TInt iOkMenuBarResourceId;

        //Handle to titlepane. doesn't own
        CCAStatusPaneHandler* iTitlePane;

        // ETrue if findbox is enabled
        // else EFalse
        TBool iFindBoxEnabled;

        /// ETrue if we can not show tabgroup after desctruction of this class.
        /// EFalse if we can show.
        TBool iDontShowTabGroup;

        //Holds the text entered in the FindPane
        //Owns - has to be freed
        HBufC* iFindPaneText;

        // ETrue if we can shut the dialog.
        // EFalse if we can not.
        TBool iExitEnabled;

        // ETrue if touch event is allowed
        // EFalse if touch event is not allowed
        TBool iAllowPenEvent;
    };



#endif
