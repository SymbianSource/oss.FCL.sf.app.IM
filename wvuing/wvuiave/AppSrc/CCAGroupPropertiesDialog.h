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
* Description:  Editing and  viewing class for chat group properties
*
*/



#ifndef CCAGROUPPROPERTIESDIALOG_H
#define CCAGROUPPROPERTIESDIALOG_H

//  INCLUDES
#include <aknform.h>
#include <eiklbo.h>
#include <bldvariant.hrh>

#include "TEnumsPC.h"
// FORWARD DECLARATIONS
class CAknSettingItemArray;
class CCAStatusPaneHandler;
class CAknSettingStyleListBox;
class CCAAppUi;
class MCAGroupPC;
class MCAGroupPropertiesPC;
// CLASS DECLARATION

/**
 *  Dialog class for setting and editing group properties
 *
 *  @lib chatng.app
 *  @since 3.2
 */
class CCAGroupPropertiesDialog : public CAknDialog,
            public MEikListBoxObserver
    {
    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         *
         * @param aTitleBar The in-memory representation of the
         *                  statuspanehandler.
         * @param aEditMode ETrue if editing else group properties are set
         *
         * @param aUserList List of users
         * @param aAdmins List of administrators
         * @param aBannedUsers List of banned users
         *
         * @param aGroupId Group id
         * @param aAreItemsEdited ETrue if items are edited
         */
        CCAGroupPropertiesDialog( CCAStatusPaneHandler& aTitleBar,
                                  TBool aEditMode,
                                  CDesCArray& aUserList,
                                  CDesCArray& aAdmins,
                                  CDesCArray& aBannedUsers,
                                  const TDesC& aGroupId,
                                  TBool& aAreItemsEdited );

        /**
         * Destructor
         */
        virtual ~CCAGroupPropertiesDialog();

        /**
         * 2nd level constructor.
         * @param aMenuResource The menu resource
         */
        void ConstructL( TInt aMenuResource );

    private: // New functions

        /**
         * This method launches access list dialog
         * @return 1 if dialog is exit by ok else 0
         */
        TInt LaunchAccessListDialogL();

        /**
         * This method launches editing rights list dialog
         * @return Dialog exit code
         */
        TInt LaunchEditingRightsListDialogL();

        /**
         * This method launches banned users list dialog
         * @return Dialog exit code
         */
        TInt LaunchBannedUsersListDialogL();

        /**
         * This method changes CBA labels
         */
        void HandleCBAChangeL();

        /**
         * Helper to launch a popup and handle Cancel
         * correctly with various list dialogs. Needed for AVKON
         * quirks.
         * @param aPopupResourceId The resource id for the popup texts
         * @param aSettingResourceId The radiobutton settings page resource id
         * @param aSelectionIndex user`s selection
         * @param aCallBack Pointer to member to invoke under conditions of
         * aBoolPtr and aInvokeOnFalse
         * @param aInvokeOnFalse ETrue means to invoke aCallBack when *aBoolPtr
         * is EFalse
         * @since 2.common
         * @return The aCallBack return value
         */
        TInt LaunchDialogHelperL(
            TInt aPopupResourceId,
            TInt aSettingResourceId,
            TInt& aSelectionIndex,
            TInt ( CCAGroupPropertiesDialog::* aCallBack )(),
            TBool aInvokeOnFalse = EFalse );

        TInt LaunchDialogHelperL(
            TInt aPopupResourceId,
            TInt aSettingResourceId,
            TInt& aSelectionIndex,
            TInt ( CCAGroupPropertiesDialog::* aCallBack )(),
            TEnumsPC::TImpsPropertyBoolPC  aInvokeOnTrue );
    private: //New methods

        /**
         * This method checks if items are edited
         * @since 2.1
         * @return ETrue if edited
         */
        TBool AreItemsEdited();

        /**
         * Adds text setting item to setting item array
         * @param aSettingPageResourceId Setting page resoure
         * @param aTitleResourceId Resource id of title
         * @param aOrdinal Ordinal id
         * @param aEmptyText Empty item text
         * @param aModifiedBuffer Descriptor that contains modified value
         */
        void AddTextSettingItemL( TInt aSettingPageResourceId,
                                  TInt aTitleResourceId,
                                  TInt aOrdinal,
                                  TDesC& aEmptyText,
                                  TDes& aModifiedBuffer );

        /**
         * Adds integer setting item to setting item array
         * @param aSettingPageResourceId Setting page resource Id
         * @param aTitleResourceId Title resource id
         * @param aOrdinal Ordinal id
         * @param aModigiedValue Contains modified value after change
         */
        void AddIntegerSettingItemL( TInt aSettingPageResourceId,
                                     TInt aTitleResourceId,
                                     TInt aOrdinal,
                                     TInt& aModifiedValue );

        /**
         * Adds binary setting item to setting item array
         * @param aSettingPageResourceId Setting page resource Id
         * @param aTitleResourceId Title resource id
         * @param aAssociatedResourceId Associated resource id
         * @param aOrdinal Ordinal id
         * @param aModigiedValue Contains modified value after change
         */
        void AddBinarySettingItemL( TInt aSettingPageResourceId,
                                    TInt aTitleResourceId,
                                    TInt aAssociatedResourceId,
                                    TInt aOrdinal,
                                    TEnumsPC::TImpsPropertyBoolPC& aModifiedValue );

        /**
        * Launches a edit dialog from currently focused listbox item.
        * Note! Do NOT call this method directly. Use LaunchCorrectDialogL.
        * @return if dialog was launched (ETrue) else (EFalse)
        */
        TBool DoLaunchCorrectDialogL();

        /**
         * Sets up the async dialog launching.
         * Calling this will method will eventually launch
         * DoLaunchCorrectDialogL, which should do all the work.
         * Async launching is done to release the pointer- and keyevents.
         */
        void LaunchCorrectDialog();

        /**
         * Static TCallBack method to launch dialogs
         * This will call DoLaunchCorrectDialogL
         * @param aDlg pointer to calling dialog (this)
         * @return TInt return value needed for TCallBack
         * @see DoLaunchCorrectDialogL
         */
        static TInt LaunchDlgCallBack( TAny* aDlg );

    public:	//From MEikCommandObserver

        /**
         * Processes user commands
         * @param ID of the command to respond to
         */
        void ProcessCommandL( TInt aCommand );

    public: // Functions from MEikListBoxObserver

        /**
         * From MEikListBoxObserver Handles listbox events
         * @param aListBox In memory representation of listbox
         * @param aEventType Event type od listbox
         */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );

    public:

        /**
         * Dynamically initialises a menu pane. The Uikon framework calls this
         * function, if it is implemented in a menu’s observer, immediately
         * before the menu pane is activated.
         * @param aResourceId Resource ID identifying the menu bar to
         *                    initialise
         * @param The in-memory representation of the menu bar
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    public:     // from CCoeControl

        /**
        * @see CCoeControl.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );



        /**
         * Help Key Support
         * This function is called by the Framework to get the context to launch
         * Help
         *
         * @param aContext	The context that contains the appid and the help id.
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
    protected:  // Functions from CEikDialog

        /**
         * From CEikDialog Sets the dialog layout when dialog is started
         * @see CEikDialog
         */
        void PreLayoutDynInitL();

        /**
         * From CEikDialog. Called when dialog is dismissed
         * @see CEikDialog
         */
        TBool OkToExitL( TInt aButtonId );

        /**
         * This method stores the values of setting items
         */
        void StoreValuesL();

    private:    // enumerations for setting item ids

        enum TSettingItemId
            {
            ESetGroupName = 0,
            ESetGroupId,
            ESetGroupTopic,
            ESetGroupWelcomeNote,
            ESetGroupSize,
            ESetGroupAllowSearch,
            ESetGroupAdminSelected,
            ESetGroupIsOpen,
            ESetGroupBannedUsers,
            ESetGroupAllowWhisper
            };

    private:    // Data

        //Doesn't own.
        CCAStatusPaneHandler& iTitlePane;

        TBool iEditMode;

        //Doesn't own.
        CDesCArray& iUserList;

        //Doesn't own.
        CDesCArray& iAdmins;

        //Doesn't own
        CDesCArray& iBannedUsers;

        //Group id . Doesnt own
        const TDesC& iGroupId;

        //Whether the properties of the group has been changed by the user
        TBool& iAreItemsEdited;

        //name od the group
        TPtr iGroupName;

        //Name of the group before editting. Doesnt own
        HBufC* iOldGroupName;

        //Group topic. Doesnt own
        TPtr iTopic;

        //Topic of the group before editting. Doesnt own
        HBufC* iOldTopic;

        //Welcome note to the group. Doesnt own
        TPtr iWelcomeNote;

        //Welcome note before editting. Doesnt own
        HBufC* iOldWelcomeNote;

        //Owns
        CAknSettingItemArray* iSettingItemArray;

        //Owns
        HBufC* iGroupNameBuf;

        //Owns
        HBufC* iTopicBuf;

        //Owns
        HBufC* iWelcomeNoteBuf;

        //Size of the group. Doesnt own
        TInt iSize;

        //Size property of the group before editting
        TInt iOldSize;

        //Allow search propery
        TEnumsPC::TImpsPropertyBoolPC iAllowSearch;

        //Allow search property before editting
        TEnumsPC::TImpsPropertyBoolPC iOldAllowSearch;

        //Allow whisper messages property
        TEnumsPC::TImpsPropertyBoolPC iAllowWhisper;

        //Allow whisper messages property before editting
        TEnumsPC::TImpsPropertyBoolPC iOldAllowWhisper;

        //Is group open
        TEnumsPC::TImpsPropertyBoolPC iIsOpen;

        //Propery of group before editting
        TEnumsPC::TImpsPropertyBoolPC iOldIsOpen;

        //To launch banned users list
        TInt iIsBannedUsers;
        //Flag to check for banned users list before editting
        TBool iOldIsBannedUsers;

        //To launch admin privileges list
        TInt iOldAdminSelected;

        //Flag for admin privileges list before editting
        TInt iAdminSelected;

        //Flag to check if the properties have been editted for a group
        TBool iIsEdited;

        //To launch the member acces list dialog again, after modifying it
        TBool iIsMemberAccessModified;

        //To launch the member admin privileges list again, after modifying it
        TBool iIsAdminStatusModified;

        //To launch the member banned list again, after modifying it
        TBool iIsBannedListModified;

        // Doesn't own. pointer to dialogs listbox.
        CAknSettingStyleListBox* iListbox;

        // Dialog launcher
        CIdle* iDialogLaunchCallback;

        //doesnt own. pointer to the app UI instance
        CCAAppUi* iAppUi;

        //Doesnt Own.
        MCAGroupPropertiesPC* iGroupPropertiesPC;

    };

#endif      // CCAGROUPPROPERTIESDIALOG_H   

// End of File
