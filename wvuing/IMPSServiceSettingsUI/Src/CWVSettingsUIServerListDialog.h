/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Serverlist dialog
*
*/



#ifndef CWVSETTINGSUISERVERLISTDIALOG_H
#define CWVSETTINGSUISERVERLISTDIALOG_H

//  INCLUDES
#include "CWVSettingsUIDefineNewServerDialog.h"
#include "CWVSettingsUIDialogImp.h"

#include <eiklbo.h>
#include <MIMPSSAPObserver.h>

// FORWARD DECLARATIONS
class CIMPSSAPSettingsStore;
class CWVSettingsUIServersArray;
class CAknSingleStyleListBox;
class MWVSettingsObserver;
class CWVSettingsUIDialogTitlePaneHandler;
class CIMPSPresenceConnectionUi;
class CIMPSSAPSettingsList;

// CLASS DECLARATION

/**
*  Server list dialog
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/
NONSHARABLE_CLASS( CWVSettingsUIServerListDialog ) : public CAknDialog,
        public MEikListBoxObserver,
        public MIMPSSAPObserver
    {
public:  // Constructors and destructor

    /**
    * C++ default constructor.
    * @param aSAPSettingsStore Pointer to SAP settings store
    * @param aCaller Pointer to the dialog observer
    * @param aTitlePaneHandler Pointer to status-pane handler
    * @param aDialog Pointer to the service settings ui dialog
    * @param aConnectionUi pointer to connection ui
    */
    CWVSettingsUIServerListDialog(	CIMPSSAPSettingsStore& aSAPSettingsStore,
    MWVSettingsObserver& aCaller,
    CWVSettingsUIDialogTitlePaneHandler& aTitlePaneHandler,
    CWVSettingsUIDialogImp& aDialog,
    CIMPSPresenceConnectionUi* aConnectionUi );

    /**
    * Destructor.
    */
    virtual ~CWVSettingsUIServerListDialog();

public: // New functions

    /**
     * Refreshes title
     * @since 2.1
     */
    void RefreshTitleL();

    /**
     * Help key support - GetHelpContext is called by the framework on
     * event EAknCmdHelp
     * --------------------------------------------------------------------------------
     * CWVSettingsUIDefineNewServerDialog::GetHelpContext
     * @param 	aContext	The context dependent on the current view is passed by the framework.
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

private: //New functions

    /**
    * Displays server editor
    * @since 2.1
    * @param aEditMode Edit mode
    */
    TWVSettingsDialogReturnValue DisplayServerEditorDialogL(
        CWVSettingsUIDefineNewServerDialog::TEditMode aEditMode );

    /**
    * Asks needed confirmations and deletes selected server
    * @since 2.1
    */
    void DeleteSelectedServerL();

    /**
    * Asks for confirmation about logout and performs it if accepted
    * @since 2.1
    * @return ETrue if logged out, EFalse if we stayed logged in
    */
    TBool ConfirmLogoutL();

    /**
    * Executes confirmation dialog
    * @since 2.1
    * @param aPrompt Text in dialog
    * @return Dialog return value
    */
    TInt ExecuteConfirmationDialogL( const TDesC& aPrompt );

    /**
    * Return SAP settings of given index
    * Ownership is transferred to caller
    * @since 2.1
    * @param aIndex Index of the server in array
    * @return SAP settings, NOTE can return NULL!
    */
    CIMPSSAPSettings* GetSAPL( TInt aIndex );

    /**
    * Refreshes internal model
    * @since 2.1
    */
    void UpdateModelL();

    /**
    * Loads protected server icon bitmap.
    * @since 2.1
    */
    void LoadListboxIconArrayL();

    /**
    * Launches help
    * @param aContext, the help context to be shown
    * @since 2.1
    */
    void LaunchHelpL( const TDesC& aContext );

    /**
    * Show brand protection note
    * @since 2.1
    */
    void ShowBrandProtectionNoteL();

    /**
     * Stores the given command and launches correct dialog
     * asynchronously
     * @param aCommand command to execute
     */
    void LaunchCorrectDialog( TInt aCommand );

    /**
     * Static TCallBack method to handle async dialog launching
     * This will call DoLaunchCorrectDialogL.
     * @see TCallBack
     */
    static TInt LaunchDlgCallback( TAny* aDlg );

    /**
     * This method will launch the correct dialog
     * determined by iDlgCommand.
     * Do not call this directly. Use LaunchCorrectDialog;
     */
    void DoLaunchCorrectDialogL();

    /**
     *
     * set the iIsOpenedTargettDlg value.
     * aFlag is ETrue while try to open target dialog, or is EFalse after target was closed.
     */
    void SetTargetDlgFlag( TBool aFlag );

    /**
     *
     * get the flag value.
     */
    TBool GetTargetDlgFlag();

private:	//From MEikCommandObserver

    /**
    * From MEikCommandObserver Command handler
    * @param aCommand Command that was initiated
    */
    void ProcessCommandL( TInt aCommand );

private: // Functions from MEikListBoxObserver

    /**
    * From MEikListBoxObserver Callback for listbox events
    * @param aListBox Originating listbox
    * @param aEventType Event type
    */
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

private:
    /**
     * From CCoeControl. Handles the keyevents
     * @param aKeyEvent The key event.
     * @param aType The type of key event: EEventKey, EEventKeyUp or EEventKeyDown.
     * @return Indicates whether or not the key event was used by this control
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    /**
    * From Uikon framework. Dynamically initialises a menu pane.
    * @param aResourceId Resource ID identifying the menu pane to initialise
    * @param aMenuPane The in-memory representation of the menu pane
    */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

private: // From MIMPSSAPObserver

    /**
    * From MIMPSSAPObserver
    * @see MIMPSSAPObserver for more information
    */
    void HandleSAPEvent( TSAPEvent aEvent );

private:

    /**
    * The function to fetch the setting value given the ID, which could be in CENREP or Variation file
    */
    TInt IntResourceValueL( TInt aResourceId );

protected:  // Functions from CEikDialog

    /**
    * From CEikDialog @see CEikDialog for more information
    */
    void PreLayoutDynInitL();

    /**
    * From CEikDialog @see CEikDialog for more information
    */
    TInt MappedCommandId( TInt aButtonId );

    /**
     * @see CEikDialog
     */
    TBool OkToExitL( TInt aButtonId );

private:    // Data

    // Doesn'w own. sap settings storage
    CIMPSSAPSettingsStore* iSAPSettingsStore;
    // Doesn't own. Listbox owns this data
    CWVSettingsUIServersArray* iServerListAdapter;
    // Doesn't own. This is responsibility of dialog
    CAknSingleStyleListBox* iListBox;
    // Doesn't own the user of the dialog
    MWVSettingsObserver* iObserver;

    // Owns. Title pane text
    HBufC* iTitle;

    // array of protected servers
    RArray<TInt> iProtectedServers;
    // Doesn't own. Title pane handler. Changes the title pane text
    CWVSettingsUIDialogTitlePaneHandler* iTitlePaneHandler;

    // Doesn't own, the "base"-dialog (imp)
    CWVSettingsUIDialogImp* iDialog;

    // Doesn't own: Pointer to a presence connection UI
    CIMPSPresenceConnectionUi* iConnectionUI;

    // Owns: sap settings
    CIMPSSAPSettings* iSAP;

    // Tells if the editor dialog is active
    TBool iEditorActive;

    // the uid of the selected server
    TUint32 iCurrent;

    // Tells if a SAP has possibly been added to the list
    TBool iAdded;

    // the uid of the added server
    TUint32 iNewUid;

    // Async command to execute
    TInt iDlgCommand;

    // Async dialog launcher
    CAsyncCallBack* iDlgLauncher;


    // to avoid twice calling LaunchCorrectDialog()
    TBool iIsOpenedTargettDlg;
    };

#endif      // CWVSETTINGSUISERVERLISTDIALOG_H

// End of File
