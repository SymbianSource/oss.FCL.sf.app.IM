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
* Description:  Contact editor / viewer
*
*/



#ifndef CCACONTACTEDITOR_H
#define CCACONTACTEDITOR_H

//  INCLUDES
#include <aknform.h>
#include "MCAUiContactEditCmdCB.h"
#include "TEnumsPC.h"

// FORWARD DECLARATIONS
//class MCAStoredContact;
class CCAStatusPaneHandler;
class CEikEdwin;
class CCAContactEditCmd;
class MCAContactEditPC;
class CCAAppUi;



// CLASS DECLARATION

/**
 *  Contact editor / viewer
 *
 *  @lib chatNG.app
 *  @since 3.2
 */
class CCAContactEditor : public CAknForm,
            public MCAUiContactEditCmdCB
    {
    public:  // Constructors and destructor

        /**
         * Symbian default constructor
         * @since 3.2
         * @param aIndex - Index of the contact to be edited in MainviewArryPC
         * @param aSaved - Ref to the Bool - Etrue denotes that the contact was edited
         * @param aContactPCInitialise - EFalse denotes that the Contact EditorsPC Component
         *			has been initialised by the caller. Hence ContactEditor should
         *			call initialise/release of the Process Component.
         */
        static CCAContactEditor* NewL( TInt aIndex,
                                       TBool& aSaved,
                                       const TBool aContactPCInitialise = ETrue );

        /**
         * Symbian default constructor
         * @since 3.2
         * @param aIndex - Index of the contact to be edited in MainviewArryPC
         * @param aSaved - Ref to the Bool - Etrue denotes that the contact was edited
         * @param aContactPCInitialise - EFalse denotes that the Contact EditorsPC Component
         *			has been initialised by the caller. Hence ContactEditor should
         *			call initialise/release of the Process Component.
         */
        static CCAContactEditor* NewLC(
            TInt aIndex,
            TBool& aSaved,
            const TBool aContactPCInitialise = ETrue );


        /**
         * Destructor.
         * @since 3.2
         */
        virtual ~CCAContactEditor();

    public:  // New functions

        /**
         * Launches contact editor.
         * @param aCreateNew ETrue if new contact should be created
         * @param aListId Id of the contact list, where the contact is to be
         *                stored
         * @param aWVID WVID to be inserted for new contact
         *              (can be set to NULL)
         * @param aNickName Name to be insterted for new contact (can be NULL)
         * @param aLoadFromStore Load the contact from storage
         * @param aFetchAlias Fetch alias for the provided wvid
         * @param aCursorToBeginning Move cursor of wv id editor to beginning
         * @return zero if it was the cancel button ELSE EAknSoftkeyDone
         * @since 3.2
         */
        TInt EditContactLD( TEnumsPC::TContactEditorMode aCreateNew,
                            const TDesC& aListId,
                            const TDesC& aWVID = KNullDesC,
                            const TDesC& aNickName = KNullDesC,
                            TBool aLoadFromStore = ETrue,
                            TBool aFetchAlias = ETrue,
                            TBool aCursorToBeginning = EFalse );

        /**
         * Launches contact editor, with focus on the given field.
         * Used by Contact Details Dialog.
         * @param aInitialFocus The initial focus line (command ID)
         * @see EditContactLD
         * @return zero if it was the cancel button ELSE EAknSoftkeyDone
         * @since 3.2
         */
        TInt EditContactLD(
            TInt aInitialFocus,
            TEnumsPC::TContactEditorMode aCreateNew,
            const TDesC& aListId );


        /**
         * Move specified contact to another list
         * @param aContactItemId Id of contact to be moved
         * @since 3.2
         */
        TBool MoveContactL( const TDesC& aListId, HBufC*& aMovedListId, TBool& aCollapsed );



    private: // Enumerations

        /**
        * Internal state transitions only
        * EAdd:  Edit mode for adding contact
        * EView: View-mode. User can select edit from there.
        * EEdit: Edit mode for editing contact
        * EEditAfterView: Edit selected from view-mode..
        * @since 3.2
        */
        enum TInternalStates
            {
            EAdd = 1,
            EView,
            EEdit,
            EEditAfterView,
            EMove
            };

    private:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since 3.2
        * @param aSaved - Ref to the Bool - Etrue denotes that the contact was edited
        * @param aContactPCInitialise - Etrue denotes that the Contact EditorsPC Component
        *			has been initialised by the caller. Hence ContactEditor should
        *			call initialise/release of the Process Component.
        */
        CCAContactEditor( TBool& aSaved, const TBool aContactPCInitialise );

        /**
         * 2nd level constructor for this class
         * @since 3.2
         */
        void ConstructL( TInt aIndex );

    private: // New functions

        /**
        * Loads all data that is displayed in form
        * @since 3.2
        */
        void LoadFormDataL();

        /**
        * Fetches handler to edwin control pointed by aId
        * @param aId Control's id
        * @return Pointer to Edwin
        * @since 3.2
        */
        CEikEdwin* GetFormEdwin( TInt aId );

        /**
        * Copies edwin's text to buffer and returns handle to it
        * Transfers ownership to caller!!!
        * Leaves on failure
        * @param aId Control's id
        * @return Buffer with text
        * @since 3.2
        */
        HBufC* GetEdwinTextBufferL( TInt aId );

        /**
        * Copies edwin's text to buffer and returns handle to it
        * Transfers ownership to caller!!!
        * Pushes pointer to cleanup stack, leaves on failure.
        * @param aId Control's id
        * @return Buffer with text
        * @since 3.2
        */
        HBufC* GetEdwinTextBufferLC( TInt aId );

        /**
        * Updates CBA-labels with resource given by aResourceId (RESOURCE CBA)
        * @param aResourceId Resource that is used to update labels
        * @since 3.2
        */
        void SetMenuBarTitleResourceL( TInt aResourceId );

        /**
        * Updates title bar according to current data edited
        * @since 3.2
        */
        void UpdateTitleBarL();

        /**
        * Hides fields that have no data
        * @since 3.2
        */
        void HideEmptyFieldsL();

        /**
        * Sets focus to field that is available
        * If currently focused is not, then try nick->real->wvid
        * @since 3.2
        */
        void SetFocusToAvailableFieldL();

    private: // Functions from base classes (CAknDialog)

        /**
        * From CAknDialog, Handles key events
        * @param aKeyEvent Key that caused the event
        * @param aType Event type
        * @return Response
        * @since 3.2
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From CAknForm, Set initial line to be in edit-state when edit
        * or add-state is chosen
        * @since 3.2
        */
        void SetInitialCurrentLine();


    private: // Functions from base classes

        /**
        * From MEikMenuObserver, Gets called before menu is displayed
        * @param aResourceId Id of a resource being built
        * @param aMenuPane Handle to menupane that is about to be shown
        * @since 3.2
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From CEikDialog, Gets called before form is displayed
        * @since 3.2
        */
        void PreLayoutDynInitL();

        /**
         * From CEikDialog, Gets called before form is displayed
         * @since 3.2
         */
        void PostLayoutDynInitL();

        /**
        * From CAknDialog, Gets called before form is dismissed
        * @param aButtonId Button that was pressed for dismissing dialog
        * @return Is it ok to exit or not
        * @since 3.2
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From CAknForm, Gets called when user chooses to save data
        * @return Was saving successful
        * @since 3.2
        */
        TBool SaveFormDataL();

        /**
        * From CAknForm, Gets called when user chooses not to save data
        * @since 3.2
        */
        void DoNotSaveFormDataL() ;

        /**
        * From CAknForm, Gets called when user chooses to save data,
        * we don't ask for confirmation
        * @return Save or not
        * @since 3.2
        */
        TBool QuerySaveChangesL() ;

    private:

        /**
         * Launch help dialog
         * @since 3.2
         */
        void LaunchHelpL( const TDesC& aContext );

        /**
         * Uses list query to get list for contact for editing and adding.
         * @param aId - current User Id in case of operation other than move
         *				- current listid in case the operation is move
         * @param aNoListsResource Resource id of text to be used if
         *                         there aren't any lists avalaible.
         *                         Use 0 to get qtn_chat_no_contact_lists
         *                         information note with OK softkey
         *                         and force list creation after the note.
         * @return ListId if success, NULL otherwise.
         * @since 3.2
         */
        HBufC* GetListIdL( const TDesC& aId, TInt aNoListsResource );

        /**
        * Check if given key would edit the text in focus.
        * @param aKeyEvent Key event to check.
        * @return ETrue if key would edit text.
        * @since 3.2
        */
        TBool IsEditingKey( const TKeyEvent &aKeyEvent );

        /**
        * Changes own alias to aAlias
        * @param aAlias New own alias
        * @return None
        * @since 3.2
        */
        void UpdateOwnAliasL( const TDesC& aAlias );

        /**
         * Deletes currently viewed contact.
         * @param aAskConfirmation If EFalse confirmation about
         *                         deletion is not shown and contact
         *                         is deleted.
         * @return General error code or KErrNone if successfull.
         * @since 3.0
         */
        TInt DeleteCurrentContactL( TBool aAskConfirmation = ETrue );

        /**
         * Focuses Edwin control.
         * Cursor is set in the end of text.
         * @since 3.2
         */
        void FocusEdwinL( TInt aId, TBool aHighlight = EFalse );

    public:
        /**
         * This is invoked by the command object to get
         * the current parameters before performing an operation

         * this function is called by the contact edit object
         * based on the iContactEditOperation - The Command object
         * gets the required data from UI
         * @since 3.2
         */
        virtual void GetParameters( MCAUiContactEditCmdCB::TContactEditParams& aParam ) ;

        /**
         * This is invoked by the command object to set
         * the result values after performing an operation
         * based on the iContactEditOperation - command object sets the
         * data required by UI.
         * The ownership of pointer varibles is also transferred to the UI.
         * Hence UI must delete the allocated data.
         * @since 3.2
         */
        virtual void SetParameters( MCAUiContactEditCmdCB::TContactEditParams& aParam ) ;

    private:    // Data

        // Doesn't own
        const TDesC* iWVUID;
        // Doesn't own
        const TDesC* iNickName;

        /// owns list id of saved contact
        HBufC* iListId;

        // owns user id of saved contact
        HBufC* iUserId;

        // owns. original user id
        HBufC* iOrigId;

        // owns. Identification of contact
        HBufC* iIdentification;

        //// owns. Nickname of contact
        HBufC* iNick;

        // owns. alias
        HBufC* iAlias;

        //owns the new user id thats is to be edited
        HBufC* iNewUserId;

        // if all fields are empty
        TBool iAllFieldsEmpty;

        // Tells editor in what mode it should be (view/add/edit...) internally
        TEnumsPC::TContactEditorMode iInternalMode;

        // if the contact was deleted from here
        TBool iContactDeleted;

        // ETrue if contact is saved
        TBool& iContactWasSaved;

        // ETrue, if support for alias is enabled
        TBool iAliasEnabled;

        // ETrue, if nick name is enabled in brand variation
        TBool iNicknameEnabled;

        // ETrue, if WVID fields should have textual input mode
        // (enabled through brand variation)
        TBool iTextualInputMode;

        // ETrue, if user id cannot be edited
        TBool iUserIDNotEditable;

        // ETrue if contact is own status
        TBool iOwnStatus;

        // initial focus (form control id)
        TInt iInitialFocus;

        TBool iAliasNotOk;

        //initial cursor position
        TBool iCursorToBeginning;

        //Owns - Ptr to CCAContactEditCmd object
        CCAContactEditCmd *iCCAContactEditCmd;

        //Does'nt own - process component object for contact editor
        MCAContactEditPC *iCCAContactEditPC;

        //holds the contact edit operation to be performed
        MCAUiContactEditCmdCB::TContactEditParams::TContactEditOperations iContactEditOperation;

        //holds the error after an operation on contact was performed.
        TInt iContactEditOperationError;

        // not owned, from CCoeEnv
        CCAAppUi* iAppUI;

        //ETrue - denotes that the PC Component
        //has been initialised by the ContactEditor.
        TBool iContactEditPCInitialised;

        TBool iSaveStarted;

    };

#endif      // CCACONTACTEDITOR_H

// End of File
