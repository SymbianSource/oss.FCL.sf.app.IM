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
* Description:  Dialog for defining new server
*
*/


#ifndef CWVSETTINGSUIDEFINENEWSERVERDIALOG_H
#define CWVSETTINGSUIDEFINENEWSERVERDIALOG_H

//  INCLUDES
#include "CWVSettingsUINGDialog.h"
#include <eiklbo.h>
#include <akndialog.h>
#include <aknsettingpage.h>

// FORWARD DECLARATIONS
class CAknSettingItemArray;
class CAknSettingStyleListBox;
class CIMPSSAPSettingsStore;
class CIMPSSAPSettings;
class CCommsDatabase;
class CApUtils;
class CApSettingsHandler;
class MWVSettingsUICaller;
class CWVSettingsUIDialogTitlePaneHandler;
class CWVSettingsUIDialogImp;
class CIMPSPresenceConnectionUi;
class CPEngNWSessionSlotID2;
// CLASS DECLARATION

/**
*  New server definition dialog
*  Implements the dialog, that is used when user is creating new server
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/

NONSHARABLE_CLASS( CWVSettingsUIDefineNewServerDialog ) :
        public CAknDialog,
        public MEikListBoxObserver,
        public MAknSettingPageObserver
    {
public: // Enumerations

    enum TEditMode
        {
        ENewServer = 1,
        EEditServer
        };

private: // Enumerations

    enum TItemType
        {
        ETextItem = 1,
        EPasswordItem
        };

    // TSettingsListLayout is used for indexing items to array, so first
    // enumeration's value must be 0, and next must be 1 and so on...
    enum TSettingsListLayout
        {
        EServerName = 0,
        ESAP,
        EURL,
        EWVId,
        EPassword,
        ELastItemMarker // This has to be the last one!!!!
        };

    // TCompValues is used for telling which settings values
    // are compulsory
    enum TCompValues
        {
        EWVSettingsViewServerName = 1,
        EWVSettingsViewIAPName = 2,
        EWVSettingsViewURL = 4,
        EWVSettingsViewWVUserId = 8,
        EWVSettingsViewPassword = 16
        };

    // TValueOrder is an index for the settings items
    // this is used when editing different values
    enum TValueOrder
        {
        EWVSettingsViewServerNameIndex = 0,
        EWVSettingsViewIAPNameIndex,
        EWVSettingsViewURLIndex,
        EWVSettingsViewWVUserIdIndex,
        EWVSettingsViewPasswordIndex
        };

public:  // Constructors and destructor

    /**
    * C++ default constructor.
    * @param aExitReason This will hold the exit reason after this dialog
    * has been dismissed
    * @param aSAPSettingsStore Pointer to SAP settings store
    * @param aEditMode Edit mode
    * @param aCaller Pointer to the dialog observer
    * @param aTitlePaneHandler Pointer to the title pane handler
    * @param aDialog Pointer to the service settings UI dialog
    * @param aNewUid Uid of a created server
    * @param aConnectionUi pointer to connection ui
    * @param aServer Server, that is being edited. This is NULL in case of
    * creating new server
    */
    CWVSettingsUIDefineNewServerDialog( TInt& aExitReason,
                                        CIMPSSAPSettingsStore& aSAPSettingsStore,
                                        TEditMode aEditMode,
                                        MWVSettingsObserver& aObserver,
                                        CWVSettingsUIDialogTitlePaneHandler& aTitlePaneHandler,
                                        CWVSettingsUIDialogImp* aDialog,
                                        TUint32& aNewUid,
                                        CIMPSPresenceConnectionUi* aConnectionUi,
                                        CIMPSSAPSettings* aServer = NULL );


    /**
    * Destructor.
    */
    virtual ~CWVSettingsUIDefineNewServerDialog();

private: //New functions

    /**
    * Launches IAP selector
    * @since 2.1
    * @return Return flags of the dialog, to check if Exit was chosen
    */
    TInt EditSapSettingsL();

    /**
    * Stores all items to persistent storage
    * @since 2.1
    */
    void StoreAllItemsL();

    /**
    * Adds setting item to setting-items array
    * @since 2.1
    * @param aBuffer Buffer that is used to store the text in item
    * @param aId Resource identifier for this setting item
    * @param aTitleResource Resource for title text
    * @param aDlgResource Settings editor resource
    * @param aCompulsory Is this compulsory item
    * @param aItemType Setting item type
    */
    void AddItemL( TDes& aBuffer,
                   TInt aId,
                   TInt aTitleResource,
                   TInt aDlgResource,
                   TBool aCompulsory = EFalse,
                   TBool aIconNeeded = EFalse,
                   TItemType aItemType = ETextItem );

    /**
    * Loads given servers information to internal buffers
    * @since 2.1
    */
    void LoadExistingItemL();

    /**
    * Checks if all the compulsory items are filled correctly
    * @since 2.1
    * @return Are compulsory items filled correctly
    */
    TBool CompulsoryItemsFilled();

    /**
    * Externalizes all items internal states
    * @since 2.1
    */
    void SignalStoreLToAllL() const;

    /**
    * Creates a unique server name to internal buffer
    * @since 2.1
    * @return Did it succeed
    */
    TBool FormUniqueServerNameL();

    /**
    * Created a unique server name from given data
    * @since 2.1
    * @param aExistingNames an array of existing names
    * @param aNameToForm a reference to the name to be formed
    * @return Did it succeed
    */
    TBool FormUniqueNameL( const CDesCArray& aExistingNames, TDes& aNameToForm );

    /**
    * Transfers the focus to invalid field
    * @since 2.1
    */
    void ChangeFocusToInvalidCompulsoryField() const;

    /**
    * Generates safe flag value from given seed (enum)
    * @since 2.1
    * @param aSeed Seed value
    * @return Safe flag value
    */
    TInt GenerateFlagValue( TInt aSeed ) const;

    /**
    * Get server-list
    * Ownership is transferred to caller!!!
    * @since 2.1
    * @return List of servers available
    */
    CDesCArrayFlat* ServerListL() const;

    /**
    * Creates unique server name from internal server-list and places it
    * to internal buffer
    * @since 2.1
    */
    void FormUniqueServerL();

    /**
    * Edits the server name settings item
    * @since 2.1
    * @param Index of the setting item
    */
		TInt EditServerNameL( TInt aIndex );

    /**
    * Edits the user id settings item
    * @since 2.1
    * @param Index of the setting item
    */
    void EditUserIdL ( TInt aIndex );

    /**
    * Edits the password settings item
    * @since 2.1
    * @param Index of the setting item
    */
    void EditPasswordL ( TInt aIndex );

    /**
    * Returns the compulsory setting items
    * @since 2.1
    * @return The compulsory setting items
    */
    TInt GetCompulsoryItems () const;

    /**
    * Called when editing user ID and the length of user ID is zero.
    * Checks if login type is automatic and shows an error note if so
    * @since 2.1
    * @return ETrue if login type is automatic
    */
    TBool CheckAutoLoginValuesL();

    /**
    * Asks for confirmation about logout and performs it if accepted
    * @since 2.1
    * @return ETrue if logged out, EFalse if we stayed logged in
    */
    TBool ConfirmLogoutL();

    /**
    * Loads icon array
    * @since 3.0
    */
    void LoadIconArrayL();

    /**
     * Help key support - GetHelpContext is called by the framework on
     * event EAknCmdHelp
     * --------------------------------------------------------------------------------
     * CWVSettingsUIDefineNewServerDialog::GetHelpContext
     * @param 	aContext	The context dependent on the current view is passed by the framework.
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

private: // From CAknDialog

    /**
    * From CAknDialog, Handles menu commands
    * @param aCommandId Command event received
    */
    void ProcessCommandL( TInt aCommand );



private: // From MEikListBoxObserver

    /**
    * From MEikListBoxObserver Callback for listbox events
    * @since 1.2
    * @param aListBox Originating listbox
    * @param aEventType Event type
    */
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

private: // From MAknSettingPageObserver

    /**
     * Used to observe editor state when domain selection
     * query is variated on.
     * @since S60 v3.2
     * @see MAknSettingPageObserver
     */
    void HandleSettingPageEventL( CAknSettingPage* aSettingPage,
                                  TAknSettingPageEvent aEventType );

public: // From Uikon framework

    /**
    * From Uikon framework. Dynamically initialises a menu pane.
    * @param aResourceId Resource ID identifying the menu pane to initialise
    * @param aMenuPane The in-memory representation of the menu pane
    */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

private: // from CCoeControl
    /**
    * From CCoeControl. Handles the keyevents
    * @param aKeyEvent The key event.
    * @param aType The type of key event: EEventKey, EEventKeyUp or EEventKeyDown.
    * @return Indicates whether or not the key event was used by this control
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );


private:
    /**
     * The function to fetch the setting value given the ID, which could be in CENREP or Variation file
       */
    TInt IntResourceValueL( TInt aResourceId );
		/**
		 * Static TCallBack method
		 * to async exit dialog 
		 * @see TCallBack
		 */
		static TInt LaunchDlgCallback( TAny* aDlg );
		
		/**
		 * close dialog using callback function
		 * to async exit dialog 
		 * @see TCallBack
		 */
		void CloseDialogL();
protected: // From CEikDialog

    /**
    * From CEikDialog @see CEikDialog for more information
    */
    void PreLayoutDynInitL();

    /**
    * From CEikDialog @see CEikDialog for more information
    */
    void PostLayoutDynInitL();

    /**
    * From CEikDialog @see CEikDialog for more information
    */
    TBool OkToExitL( TInt aButtonId );

    /**
    * From CEikDialog @see CEikDialog for more information
    */
    TInt MappedCommandId( TInt aButtonId );


private:    // Data

    // an exit reason for the dialog
    TInt& iExitReason;

    // Doesn't own.
    CIMPSSAPSettingsStore* iSAPSettingsStore;

    // edit mode for the dialog, new server or edit old
    TEditMode iEditMode;

    // owns: wvsapsettings for a server
    CIMPSSAPSettings* iSAP;

    // internet access point name pointer
    TPtr iIAPNamePtr;
    // server name pointer
    TPtr iServerNamePtr;
    // wvuserid pointer
    TPtr iWVUserIdPtr;
    // password pointer
    TPtr iPasswordPtr;
    // url pointer
    TPtr iURLPtr;

    // caller type of the dialog
    // Doesn't own
    MWVSettingsObserver* iObserver;

    // access point settings handler
    // Owns.
    CApSettingsHandler* iAPSettingsHandler;
    // comms database
    // Owns.
    CCommsDatabase* iCommsDb;
    // access point utilities
    // Owns.
    CApUtils* iAPUtils;
    // settings listbox
    // Doesn't own
    CAknSettingStyleListBox* iListbox;
    // Owns: settings item array
    CAknSettingItemArray* iSettingItemArray;
    // title pane handler, changes the text
    // Doesn't own
    CWVSettingsUIDialogTitlePaneHandler* iTitlePaneHandler;

    // ordinal of the setting items added
    TInt iOrdinal;

    // index of the array of servers
    TInt iServerArrayIndex;

    // internet access point name
    // Owns.
    HBufC* iIAPName;
    // server name
    // Owns.
    HBufC* iServerName;
    // wvuserid
    // Owns.
    HBufC* iWVUserId;
    // password
    // Owns.
    HBufC* iPassword;
    // url
    // Owns.
    HBufC* iURL;
    // tells if the internet access point setting has a value or not
    TBool iIAPHasValue;
    // internet access point id
    TUint32 iIAP;
    // invalid data in these setting items
    TInt iFailedFields;

    // user id before editing
    // owns.
    HBufC* iOldUserId;

    // the base dialog of service settings view
    // Doesn't own
    CWVSettingsUIDialogImp* iDialog;

    // tells if we should store old user id or not when editing
    TBool iStoreOldId;

    // pointer to the old user id
    TPtr iOldUserIdPtr;

    // Owns: old password
    HBufC* iOldPassword;

    // tells if we should save the SAP or not
    TBool iEdited;

    // the uid of the new server
    TUint32& iNewUid;

    TBool iProtected;

    // Pointer to a presence connection UI, owned if
    // iConnectionUIOwned is ETrue
    CIMPSPresenceConnectionUi* iConnectionUI;

    // iConnectionUI owhership
    TBool iConnectionUIOwned;

    // Is this dialog already deleted
    TBool iIsDeleted;

    ///<Cached pointer to control environment, not owned
    CCoeEnv* iCoeEnv;

    // previous server name (which was active before loading brand for
    // editable server), owned
    HBufC* iOldServerName;

    TBool iDomainSelectionQuery;
        // Async dialog launcher
        CAsyncCallBack* iDlgLauncher;
        
        TBool isDisplayedSubDialog;
    };

#endif      // CWVSETTINGSUIDEFINENEWSERVERDIALOG_H

// End of File
