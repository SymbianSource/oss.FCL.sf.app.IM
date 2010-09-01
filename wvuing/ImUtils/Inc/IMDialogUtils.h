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
* Description:  This class provides some useful methods to launch different dialogs.
 *
*/



#ifndef IMDIALOGUTILS_H
#define IMDIALOGUTILS_H

//  INCLUDES
#include <eiklbm.h>
#include <avkon.rsg> // for R_AVKON_SOFTKEYS_EMPTY
#include <aknnotedialog.h>
// CLASS DECLARATION

/**
 *  This class provides some useful methods to launch different dialogs.
 *
 *  @lib chat.app
 *  @since
 */
class IMDialogUtils
    {
    public: // Enumerations

        enum TLeftSoftkey
            {
            EDefault = 0,
            ESend,
            EOk
            };

        enum TInitialSelectionMode
            {
            ESelectAll = 0, // Default mode
            ESelectNone
            };

    public: // New functions

        /**
         * Displays information note with given text
         * @since
         * @param aText Text to show in note
         */
        IMPORT_C static void DisplayInformationNoteL( const TDesC& aText );

        /**
         * Displays information note with given text
         * @since
         * @param aTextResource Resource for information text ()
         * @param aParameter Parameter data. If not given, then text is loaded
         * with just AllocReadResourceLC
         */
        IMPORT_C static void DisplayInformationNoteL( TInt aTextResource,
                                                      const TDesC& aParameter = KNullDesC );

        /**
         * This method displays popuplist
         * @param aItemList Item list for dialog
         * @param aTitleResourceId Resource id of title
         * @param aOwnerShipType Ownershiptype
         * @param aEmptyListPrompt Prompt of empty list
         */
        IMPORT_C static void DisplayPopupListL( MDesCArray* aItemList,
                                                const TInt aTitleResourceId,
                                                TListBoxModelItemArrayOwnership aOwnerShipType,
                                                const TInt aEmptyListPrompt = KErrNotFound );

        IMPORT_C static void DisplayPopupListL( TInt& aSelectedIndex, MDesCArray* aItemList,
                                                const TDesC& aTitle,
                                                TListBoxModelItemArrayOwnership aOwnerShipType,
                                                const TInt aEmptyListPrompt  /*=-1*/	);
        /**
         * This method displays Text Query Dialog
         * @param aDataText Descriptor whom written data is stored
         * @param aTitleResourceId Resource ID of title text
         * @param aDialogResource Resource of dialog
         * @param aPredictiveInput Enable T9 predictive input (when ETrue)
         * @param aLeftSoftkeyAlwaysVisible Is LSK always visible, or visible
         *                                  only when there is text in the editor.
         * @param aLeftSoftkey Left soft key of dialog.
         * @param aSelectionMode Initial selection mode in dialog editor.
         * @param aDomainSelectionQuery Is domain selection variated on or not,
         *                              ETrue causes dialog to exit when editor field
         *                              is cleared by user.
         * @return KeyCode (e.g. EAknSoftKeyOk)
         */
        IMPORT_C static TInt DisplayTextQueryDialogL( TDes& aDataText,
                                                      const TInt aTitleResourceId,
                                                      const TInt aDialogResourceId,
                                                      TBool aPredictiveInput = EFalse,
                                                      TBool aLeftSoftkeyAlwaysVisible = EFalse,
                                                      TLeftSoftkey aLeftSoftkey = EDefault,
                                                      TInitialSelectionMode aSelectionMode = ESelectAll,
                                                      TBool aDomainSelectionQuery = EFalse );

        /**
         * Displays message query dialog
         * @param aDialogResourceId Dialog resource
         * @param aMsgText Message text (empty by default)
         * @param aHeaderText Header text (empty by default)
         * @param aPredictiveInput Turn on T9 support if ETrue (off by default)
         * @return status
         */
        IMPORT_C static TInt DisplayMessageQueryDialogL( const TInt aDialogResourceId,
                                                         const TDesC& aMsgText = KNullDesC,
                                                         const TDesC& aHeaderText = KNullDesC,
                                                         TBool aPredictiveInput = EFalse );

        /**
         * Displays multiselection list query dialog.
         * Returns NULL if aItemList is empty or if user cancels the query!
         *
         * @param aItemList List of items in selection list
         * @param aTitle Dialog title
         * @return Selected items, NOTE THAT THE OWNERSHIP IS TRANSFERRED TO CALLER!
         */
        IMPORT_C static CDesCArray* MultiselectionListQueryDialogL(
            const MDesCArray& aItemList,
            const TDesC& aTitle );

        /**
         * This method displays multiselection List Query Dialog.
         * NOTE that it is the responsibility of the caller to use
         * cleanupstack for aItemList (ownership is NOT transferred to the
         * list box).
         * @param aIndexArray Array of selections that are selected in dialog
         * @param aItemList Array of list items (must be formatted)
         * @param aDialogResourceId Resource ID of dialog
         * @param aArrayOfSelectionIndexes Array of predefined indexes (not mandatory)
         * @param aListEmptyText Text to be shown if the list is empty
         * @param aAlternativeTitle Alternative title text
         * @param aIsForwardQuery The dialog is for forwarding the message (ETrue)
         * @return KeyCode (e.g. EAknSoftKeyOk)
         */
        IMPORT_C static TInt DisplayListQueryDialogL( CArrayFix< TInt >* aIndexArray,
                                                      MDesCArray* aItemList,
                                                      const TInt aDialogResourceId,
                                                      CArrayFix< TInt >* aArrayOfSelectionIndexes = NULL,
                                                      const TDesC& aListEmptyText = KNullDesC,
                                                      const TDesC& aAlternativeTitle = KNullDesC );

        /**
         * This method displays singleselection List Query Dialog
         * NOTE that it is the responsibility of the caller to use
         * cleanupstack for aItemList (ownership is NOT transferred to the
         * list box).
         * @param aSelectedIndex Index of selected item
         * @param aItemList List of Items (Must be formatted)
         * @param aDialogResourceId Resource ID of dialog
         * @param aIsForwardQuery The dialog is for forwarding the message (ETrue)
         * @return KeyCode (e.g. EAknSoftKeyOk)
         */
        IMPORT_C static TInt DisplayListQueryDialogL( TInt* aSelectedIndex,
                                                      MDesCArray* aItemList,
                                                      const TInt aDialogResourceId,
                                                      const TDesC& aAlternativeTitle = KNullDesC,
                                                      const TDesC& aListEmptyText = KNullDesC,
                                                      const TBool aIsForwardQuery = EFalse,
                                                      const TBool aIsOnlySelect = EFalse  );

        /**
         * This method displays note dialogs e.g. confirmation dialog
         * Note dialog's type is defined in resource file
         * @param aResourceId Resource ID of dialog
         * @return KeyCode (e.g. EAknSoftKeyOk)
         */
        IMPORT_C static TInt DisplayNoteDialogL( const TInt aResourceId );

        /**
         * This method displays note dialogs e.g. confirmation dialog
         * Note dialog's type is defined in resource file
         * @param aText Prompt text of dialog
         * @param aButtonsResourceId tells buttons that are shown with note, default none
         * @param aTimeout timeout when note showing stops
         * @return KeyCode (e.g. EAknSoftKeyOk)
         */
        IMPORT_C static TInt DisplayNoteDialogL( const TDesC& aText,
                                                 const TInt aButtonsResourceId = R_AVKON_SOFTKEYS_EMPTY,
                                                 const CAknNoteDialog::TTimeout& aTimeout = CAknNoteDialog::EShortTimeout );

        /**
         * This method displays error notes for corresponding resource Id
         * @param aResourceId Id for resource to be shown
         * @return KeyCode
         */
        IMPORT_C static TInt DisplayErrorNoteL( const TInt aResourceId );

        /**
         * This method displays error notes for corresponding resource Id
         * @param aText Prompt text of dialog
         * @param aSoftkeyOk Should there be OK softkey
         * @return KeyCode
         */
        IMPORT_C static TInt DisplayErrorNoteL( const TDesC& aText, TBool aSoftkeyOk = EFalse );

        /**
        * Displays query dialog with given data
        * @param aDialogResourceId Dialogs resource id
        * @param aTextResourceId Resource id of the text
        * @return Dialog dismiss key
        */
        IMPORT_C static TInt DisplayQueryDialogL( TInt aDialogResourceId,
                                                  TInt aTextResourceId );

        /**
         * Displays query dialog with given data
         * @param aDialogResourceId Dialogs resource id
         * @param aPrompt Prompt text
         * @return Dialog dismiss key
         */
        IMPORT_C static TInt DisplayQueryDialogL( TInt aDialogResourceId,
                                                  const TDesC& aPrompt = KNullDesC );

        /**
        * Displays YES/NO query dialog with given text prompt
        * @param aTextResourceId Resource id of the text
        * @return Dialog dismiss key
        */
        IMPORT_C static TInt DisplayYesNoConfirmationDialogL( TInt aTextResourceId );

        /**
         * Displays YES/NO query dialog with given text prompt
         * @param aPrompt Prompt text
         * @return Dialog dismiss key
         */
        IMPORT_C static TInt DisplayYesNoConfirmationDialogL( const TDesC& aPrompt );

        /**
         * Displays Error notification with OK softkey and given text prompt
        * @param aTextResourceId Resource id of the text
        * @return Dialog dismiss key (EAknSoftKeyOk)
        */
        IMPORT_C static TInt DisplayErrorConfirmationNoteL( TInt aTextResourceId );

        /**
         * Displays Error notification with OK softkey and given text prompt
         * @param aPrompt Prompt text
         * @return Dialog dismiss key (EAknSoftKeyOk)
         */
        IMPORT_C static TInt DisplayErrorConfirmationNoteL( const TDesC& aPrompt );

        /**
         * Displays confirmation notification with given text prompt and
         * without softkeys
         * @param aPrompt Prompt text
         * @return Dialog dismiss key (EAknSoftKeyOk)
         */
        IMPORT_C static TInt DisplayConfirmationNoteL( TInt aTextResourceId );
    };

#endif      // IMDIALOGUTILS_H   

// End of File
