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
* Description:  Server settings dialog implementation
*
*/



#ifndef CWVSETTINGSUIDIALOGIMP_H
#define CWVSETTINGSUIDIALOGIMP_H


//  INCLUDES
#include "MWVSettingsObserverng.h"
#include "CWVSettingsUINGDialog.h"
#include "MIMPSSharedDataObserver.h"
#include "MIMPSSharedData.h"

#include "impscommonuibuilddefinitions.h"

#include <aknform.h>
#include <eiklbo.h>
#include <MIMPSSAPObserver.h>


//DATA TYPES


// FORWARD DECLARATIONS
class CAknSettingItemArray;
class CAknSettingStyleListBox;
class CIMPSSAPSettingsStore;
class CWVSettingsUITlsHolder;
class CWVSettingsUIDialogTitlePaneHandler;
class CIMPSPresenceConnectionUi;
class CAknSettingItem;
class CWVSettingsUISettingItemEditor;
class CSelectionItemList;
class CSelectableItem;
class CIMPSSAPSettingsList;
class CIMPSSAPSettings;
class CAknIconArray;
class CWVSettingsUIServerListDialog;
// CLASS DECLARATION

/**
*  Server settings dialog
*  Basically this dialog gives that two links to other dialogs, to current
*  server selection and to server list
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/
NONSHARABLE_CLASS( CWVSettingsUIDialogImp ) : public CWVSettingsUIDialog,
        public MEikListBoxObserver,
        public MIMPSSAPObserver,
        public MIMPSSharedDataObserver
    {

public: // datatypes

    // listbox items' indexes
    enum TListboxItemIndex
        {
        EWVSettingsCASettings = 0,
        EWVSettingsServers,
        EWVSettingsServerInUse,
        EWVSettingsIMLoginType,
#ifdef RD_MULTIPLE_DRIVE
        EWVSettingsSaveChatsTo,
#endif
        EWVSettingsIMConnDays,
        EWVSettingsIMConnHours
        };

    enum TConnHoursPopUpValue
        {
        EWVSettingsAllDay,
        EWVSettingsBetweenHours
        };

private: // datatypes

    enum TConnHoursSetting
        {
        EWVSettingsConnStart,
        EWVSettingsConnEnd
        };

public:  // Constructors and destructor
    /**
    * Destructor.
    */
    virtual ~CWVSettingsUIDialogImp();

    /**
    * Two-phased constructor.
    */
    static CWVSettingsUIDialogImp* NewL( );

protected:
    /**
    * Runs the dialog.
    * @since 2.1
    * @param aSAPSettingsStore Pointer to SAP settings store
    * @param aObserver Pointer to the UI observer
    * @param aResourceFile The path to a branded resource file wanted to be used
    * @param aExitReason The exit reason of the dialog
    * @param aSelfPtr pointer to callers dialog pointer.
    * @param aWhatToRun Tells whether to run the whole dialog or just the new server definition dialog
    */
    TWVSettingsDialogReturnValue RunDialogLD(	CIMPSSAPSettingsStore& aSAPSettingsStore,
                                              MWVSettingsObserver* aObserver,
                                              const TDesC& aResourceFile,
                                              const TDesC& aResourceVariationFile,
                                              TInt& aExitReason,
                                              MWVSettingsCASettings* aCAServerSettings,
                                              CWVSettingsUIDialog** aSelfPtr,
                                              TBool aForcedLaunch /*= EFalse */,
                                              TWVSettingsDialogType aDialogType );

    /**
    * Changes the resource file used
    * @since 2.1
    * @param aResourceFileName The path to the resource file wanted to be used
    * @param aResourceVariationFile	The path to the variation file which contains the settings.
    */
    void ChangeResourceFileL( const TDesC& aResourceFileName,
                              const TDesC& aResourceVariationFile );

    /**
    * C++ default constructor.
    */
    CWVSettingsUIDialogImp();

public: // New functions

    /**
    * Tells the current Chat login type
    * @since 2.1
    * @return Chat login type
    */
    TInt GetChatLoginType() const;
    
    void ChangeToManualL();

    void GetHelpContext( TCoeHelpContext& aContext ) const;
private: // New functions

    /**
    * Updates "server in use"-item from listbox to current value
    * @since 2.1
    */
    void UpdateServerInUseItemL();

    /**
    * Launches new server query
    * @since 2.1
    * @param aExitReason indicate if required to exit from IM application
    */
    void QueryNewServerL( TBool *aExitReason = NULL );

    /**
    * Launches server-list dialog
    * @since 2.1
    * @return a value from the enumeration
    */
    TWVSettingsDialogReturnValue ExecuteServerListDialogL();

    /**
    * Refreshs title
    * @since 2.1
    */
    void RefreshTitleL();

    /**
    * Launches a correct dialog
    * @since 2.1
    */
    void LaunchCorrectDialogL();

    /**
    * Launches a correct dialog
    * @since 3.1
    */
    void LaunchCASettingsL();

    /**
     * Activates currently highlighted setting item
     * @since 3.2
     */
    void ActivateCurrentItemL();

    /**
    * Launches the dialog for defining a new server
    * @since 2.1
    * @param aSAPSettingsStore Pointer to SAP settings store
    * @param aCaller Pointer to the dialog observer
    * @param aTitlePaneHandler Pointer to the title pane handler
    * @param aExitReason This will hold the exit reason after this dialog
    * has been dismissed
    */
    TInt LaunchDefineNewServerDialogL( CIMPSSAPSettingsStore& aSAPSettingsStore,
                                       MWVSettingsObserver& aObserver,
                                       CWVSettingsUIDialogTitlePaneHandler& aTitlePaneHandler,
                                       TInt& aExitReason ) const;

    /**
    * Sets IM login type in the listbox
    * @param aLoadedValue loaded login type
    * @since 2.1
    */
    void SetIMLoginTypeL( TInt aLoadedValue );

    /**
    * Creates a list of selected days for scheduling
    * @param aSelectionItemList selection item list
    * @since 2.5
    */
    void CreateSelectedDaysArrayL( CSelectionItemList& aSelectionItemList );

    /**
    * Creates a selectable item
    * @param aResourceId resource id for the text in the selectable item
    * @since 2.5
    * @return selectable item
    */
    CSelectableItem* CreateSelectableItemLC( TInt aResourceId );

    /**
    * Sets connection days setting to CSelectionItemList array
    * @param aLoadedValue loaded login type
    * @param aSelectionItemList selection item list
    * @since 2.5
    */
    void SetConnectionDaysL( TInt aLoadedValue, CSelectionItemList& aSelectionItemList );

    /**
    * Gets connection days setting from CSelectionItemList array
    * @param aSelectionItemList selection item list
    * @since 2.5
    * @return integer with the correct bits set
    */
    TInt GetConnectionDaysL( CSelectionItemList& aSelectionItemList );

    /**
    * Updates the connection days setting item value text in listbox
    * @param aSelectionItemList selection item list
    * @param aItem item identifier
    * @since 2.5
    */
    void UpdateConnectionDaysL( CSelectionItemList& aSelectionItemList );

    /**
    * Updates the connection hours text in the listbox
    * @param aItem item identifier
    * @param aAllDay all day or not
    * @since 2.5
    */
    void UpdateConnectionHoursTextL( TBool aAllDay );

    /**
    * Queries new connection times from user
    * @param aItem item identifier
    * @since 2.5
    */
    void QueryConnectionHoursL();

    /**
    * Changes login type to manual
    * @since 2.5
    */
    void ChangeLoginTypeToManualL();

    /**
    * Updates the texts in the listbox
    * @since 2.6
    */
    void UpdateListboxL();

    /**
    * Copies a selected days array
    * @param aSelectedDaysArray an array to be copied
    * @since 2.6
    */
    CSelectionItemList* CopySelectedDaysArrayL( CSelectionItemList* aSelectedDaysArray );

    /**
    * Returns the hour part of the saved time as integer.
    * Returns the given default if parsing fails.
    * @param aTime time to get the hour from
    * @param aDefault default hour as integer
    * @since 2.6
    */
    TInt Hour( const TDesC& aTime, TInt aDefault );

    /**
    * Returns the minute part of the saved time as integer
    * Returns the given default if parsing fails.
    * @param aTime time to get the minute from
    * @param aDefault default minute as integer
    * @since 2.6
    */
    TInt Minute( const TDesC& aTime, TInt aDefault );

    /**
    * Return current server's name
    * @since 2.1
    * Ownership is transferred to caller!!!
    * @return Name, NOTE returns NULL if no default SAP
    */
    HBufC* CurrentServerNameL();

    /*
    * Updates necessary parts when resource file is changed.
    * @since 2.1
    */
    void RefreshDialogL();

    /**
    * Loads the settings
    * @since 2.1
    */
    void LoadSettingsL();

    /**
    * Loads a settings value
    * @since 2.1
    * @param aSettingsValueToLoad the setting value to load
    * @param aNameKey the key name of the value
    */
    void LoadSettingValueL( TInt& aSettingValueToLoad, const TIMPSSharedKeys aNameKey );

    /**
    * Loads a string settings value
    * @since 2.5
    * @param aNameKey the key name of the value
    * @return the loaded value
    */
    HBufC* LoadStringSettingValueL( const TIMPSSharedKeys aNameKey, TConnHoursSetting aSetting );

#ifdef RD_MULTIPLE_DRIVE

    /**
    * Loads a memory drive string settings value
    * @since 2.5
    */
    void LoadMemoryDriveSettingValueL();
#endif

    /**
    * Loads connection hours settings values
    * @since 2.5
    */
    void LoadConnectionHoursSettingValuesL();

    /**
    * Saves the settings
    * @since 2.1
    */
    void SaveSettingsL();

    /**
    * Stores a settings value
    * @since 2.1
    * @param aSettingValueToStore the setting value to be saved
    * @param aNameKey the key name of the value
    */
    void StoreSettingValueL( TInt aSettingValueToStore, const TIMPSSharedKeys aNameKey );

    /**
    * Stores a string settings value
    * @since 2.5
    * @param aSettingValueToStore the setting value to be saved
    * @param aNameKey the key name of the value
    */
    void StoreStringSettingValueL( const TDesC& aSettingValueToStore, const TIMPSSharedKeys aNameKey );


    /**
    * Updates the login types in the listbox
    * @since 2.1
    */
    void UpdateLoginTypesL ();

    /**
     * Gets server list and index which to highlight
     * in selection list.
     *
     * @since 3.1
     * @param aIndexToHighlight On the return contains the
     * index that should be high lighted by default in the
     * selection list, or KErrNotFound if default index should
     * be ignored.
     * @return SAP settings list. Ownership of list returned to client.
     */
    CIMPSSAPSettingsList* ServerListLC( TInt& aIndexToHighlight );

    /**
     * Loads the used login SAP based the SAP Settings list
     * and index of selected server.
     *
     * @since 3.1
     * @param aList Server list.
     * @param aIndexOfSelectedServer The index of selected server.
     * @param aSap
     */
    void LoadSapByServerListL( const CIMPSSAPSettingsList& aList,
                               TInt aIndexOfSelectedServer,
                               CIMPSSAPSettings& aSap );

    /**
     *
     * @since 3.1
     */
    TBool ServerToUseQueryL( MDesCArray& aServers,
                             TInt aIndexToHighlight,
                             TInt& aSelectedIndex );

    /**
     * Loads the branded server icons to iIconArray
     * @param aServers List of the server names
     * @since 3.1
     */
    void LoadServerIconsL( MDesCArray& aServers );

    /**
    * Constructs complete path from parts and checks for correctness.
    * @param aResult, the complete path.
    * @param aModuleId, dll's (or app) id. For example "IM".
    * @param aAlternative, id for alternative (for example operator "Oper1" ) files.
    * @param aFileName, filename of the complete path
    * @return ETrue if path was found from C or MMC card, otherwise false
    * @since 3.1
    */
    TBool ConstructCompletePathAndCheck( TDes& aResult,
                                         const TDesC& aModuleId,
                                         const TDesC& aAlternative,
                                         const TDesC& aFileName );

    /**
    * Returns MMC drive.
    * @param aDrive, the result.
    */
    void MMCDrive( TDes& aDrive ) const;

    /**
     * Updates CBA.
     * @since S60 v3.2
     */
    void UpdateCbaL();

    /**
     * Static TCallBack method which will call ActivateCurrentItemL when
     * launched
     * @see TCallBack
     */
    static TInt LaunchDlgCallback( TAny* aDlg );

    /**
     * Used for launching dialogs asynchronously.
     */
    void LaunchCorrectDialog();

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

    /**
     *
     * Check whether there are stored servers, otherwise show add new server query.
     * @return ETrue there is at least one stored server.
     * @return EFalse there is no stored server.
     */
    TBool CheckAnyStoredServers();
private:
    /**
    * From MEikListBoxObserver Handles listbox events.
    * @param aListBox Listbox's pointed
    * @param aEventType Eventtype of listbox
    */
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

    /**
    * Handles notify from permanent keys' changes
    * @param aUid uid of the dll
    * @param aKey the shared data key
    * @param aValue the value of the key
    */
    void HandlePermanentKeyNotifyL( const TUid aUid,
                                    const TIMPSSharedKeys aKey );

    /**
    * Handles notify from temporary keys' changes
    * @param aUid uid of the dll
    * @param aKey the shared data key
    * @param aValue the value of the key
    */
    void HandleTemporaryKeyNotifyL( const TUid aUid,
                                    const TIMPSSharedKeys aKey );
    /**
    * From MEikCommandObserver Command handler
    * @since 1.2
    * @param aCommand Command that was initiated
    */
    void ProcessCommandL( TInt aCommand );

    /**
    * From MEikMenuObserver, Gets called before menu is displayed
    * @param aResourceId Id of a resource being built
    * @param aMenuPane Handle to menupane that is about to be shown
    */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
    * From CCoeControl. Handles the keyevents
    * @param aKeyEvent The key event.
    * @param aType The type of key event: EEventKey, EEventKeyUp or EEventKeyDown.
    * @return Indicates whether or not the key event was used by this control
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

private: // From MIMPSSAPObserver

    /**
    * From MIMPSSAPObserver
    * @see MIMPSSAPObserver for more information
    */
    void HandleSAPEvent( TSAPEvent aEvent );

private:

    // This function gets the setting value from the Cenrep or variation file, given the resource ID
    TInt IntResourceValueL( TInt aResourceId );

protected:  // Functions from CEikDialog

    /**
    * From CEikDialog, Gets called before form is displayed
    */
    void PreLayoutDynInitL();

    /**
    * From CEikDialog
    * Gets called before the dialog is exited
    * @param aKeyCode the key code used to exit the dialog
    */
    TBool OkToExitL( TInt aKeycode );

private:    // Data
    // Doesn't own: pointer to the sap settings store
    CIMPSSAPSettingsStore* iSAPSettingsStore;

    // server in use
    TPtr iServerInUsePtr;

    // Doesn't own: the user of this dialog
    MWVSettingsObserver* iObserver;

    // Owns: Settings items
    CAknSettingItemArray* iSettingItemArray;

    // Owns.: server in use
    HBufC* iServerInUse;

    // Doesn't own: listbox
    CAknSettingStyleListBox* iListbox;

    // Owns: the text on the titlepane
    HBufC* iTitle;

    // Owns: tls holder
    CWVSettingsUITlsHolder* iTlsHolder;

    // offset of the resource file
    TInt iResourceFileOffset;

    //offset of the variation based resource file
    TInt iResourceVariationFileOffset;

    // tells if the chat login type is edited
    TBool iImLoginTypeEdited;
    // chat login type
    TInt iImLoginType;

    // Owns: title pane handler. changes the title pane text
    CWVSettingsUIDialogTitlePaneHandler* iTitlePaneHandler;

    // Owns:: Pointer to a presence connection UI
    CIMPSPresenceConnectionUi* iConnectionUI;

    // Owns: Setting item editor
    CWVSettingsUISettingItemEditor* iSettingItemEditor;

    // IM Scheduling settings
    // owns: Selection item list for scheduling support
    CSelectionItemList* iSelectedDaysIM;
    // Owns: connection days setting item value in listbox
    HBufC* iConnDaysIM;
    // Pointer to connection days setting item value in listbox
    TPtr iConnDaysIMPtr;
    // Owns: connection hours setting item value in listbox
    HBufC* iConnHoursIM;
    // Pointer to connection hours setting item value in listbox
    TPtr iConnHoursIMPtr;
    // The value of the connection hours pop-up setting item
    TInt iConnHoursIMInt;
    // Owns: connection start time setting item value
    HBufC* iConnStartIM;
    // pointer to connection start time setting item value
    TPtr iConnStartIMPtr;
    // Owns: connection end time setting item value
    HBufC* iConnEndIM;
    // pointer to connection end time setting item value
    TPtr iConnEndIMPtr;

#ifdef RD_MULTIPLE_DRIVE
    // Owns.: memory drive selection
    HBufC* iMemorySelection;

    // memory drive selection
    TPtr iMemorySelectionPtr;
#endif // RD_MULTIPLE_DRIVE

    // tells if scheduling is supported
    TBool iSchedulingSupported;

    TDay iStartDayOfWeek;

    // Own: shared keys interface for permanent keys
    MIMPSSharedData *iSharedData;

    // Own: shared keys interface for temporary keys
    MIMPSSharedData *iSettingsChangeNotifier;

    // as a result of parsing problems of time values when changing
    // time/date separators we must have these here
    // connection end hour and minute values for PEC
    TInt iConnEndHourPEC;
    TInt iConnEndMinutePEC;
    // connection start hour and minute values for IM
    TInt iConnStartHourIM;
    TInt iConnStartMinuteIM;
    // connection end hour and minute values for IM
    TInt iConnEndHourIM;
    TInt iConnEndMinuteIM;

    // Chat server settings
    MWVSettingsCASettings* iCAServerSettings;

    // Icon array for server list query, owned
    CAknIconArray* iIconArray;

    // Self pointer. This pointer can be used to inform the
    // caller that dialog is destroyed.
    CWVSettingsUIDialog** iSelfPtr;

    // Async dialog launcher
    CAsyncCallBack* iDlgLauncher;

    // Is forced login made before activating automatic login mode
    TBool iForcedLoginMade;

    // Is dialog re-launched after login procedure
    TBool iForcedLaunch;

    // Not owned. Valid pointer only when
    // server list dialog is active.
    CWVSettingsUIServerListDialog* iServListDialog;

    //
    // to avoid twice calling LaunchCorrectDialog()
    TBool iIsOpenedTargettDlg;
    };

#endif      // CWVSETTINGSUIDIALOGIMP_H

// End of File
