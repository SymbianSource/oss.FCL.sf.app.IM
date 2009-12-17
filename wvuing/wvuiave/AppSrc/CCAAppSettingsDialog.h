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
* Description:  Dialog for application local settings
 *
*/


#ifndef CCAAPPSETTINGSDIALOG_H
#define CCAAPPSETTINGSDIALOG_H

//  INCLUDES
#include <akndialog.h>
#include <bldvariant.hrh>
#include <eiklbo.h>
#include "MCAUiSettingsDialogCmdCB.h"
#include "MCASettingSapExt.h"
#include "MCASettingsPC.h"
#include "MCASettingsCommand.h"

// FORWARD DECLARATIONS
class CAknSettingItemArray;
class CCAStatusPaneHandler;
class CAknSettingStyleListBox;
class CCAUISessionManager;
class CAknSettingItem;
class CIMPSSAPSettings;
class CIMPSSAPSettingsStore;
class CRepository;

// CLASS DECLARATION

/**
 *  Application settings dialog
 *  Implements the dialog that is used with application settings
 *
 *  @lib chatng.app
 */
class CCAAppSettingsDialog : public CAknDialog,
            public MEikListBoxObserver,
            public MCAUiSettingsDialogCmdCB
    {
    private: // Enumerations

        //Setting items' indexes
        enum TSettingListLayout
            {
            ENickName = 0,
            EAuthorizePresence,
            EReceiveIMessages,
            EReceiveInvitations,
            EMessageFlow,
            EContactOrdering,
            EFriendsListUpdate,
            EContactsToBeReloaded,
            EAutoLogin,
            EAliasName,
            EShowOffline,
            EOwnColor,
            EOtherColor,
            EMessageTone,
            EShowTimeStamp,
            ELastItemMarker
            };

    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aTitleBar Reference to status-pane handler
         * @param aSettings Reference to settings API
         */
        CCAAppSettingsDialog( CCAStatusPaneHandler& aTitleBar,
                              MCASettingsPC& aSettingsPC,
                              CCAUISessionManager& aUISessionManager,
                              CIMPSSAPSettings* aServer );

        /**
         * Destructor.
         */
        virtual ~CCAAppSettingsDialog();

    public:     // New methods

        /**
        * Method which is called with async callback
        * @param aInstance  Object which is called
        * @return EFalse
        */
        static TInt CallBack( TAny* aInstance );

        /**
        * Real callback handler. Launches dialogs.
        * @return EFalse
        */
        TInt DoHandleCallBackL();

        void GetParameters( MCAUiSettingsDialogCmdCB::TSettingsDialogParams& aSettingsDialogParams );

        /**
         * Help Key Support
         * This function is called by the Framework to get the context to launch
         * Help
         *
         * @param aContext	The context that contains the appid and the help id.
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
         * Set the status of a dialog
         * @param aContext The status whether a dialog is shown
         */
        void SetIsTargetDialogOpened( TBool aBool );

        /**
         * Get the status of a dialog
         * @return Dialog status
         */
        TBool GetIsTargetDialogOpened();
    private:	//From MEikCommandObserver

        /**
         * From MEikCommandObserver Command handler
         * @since 1.2
         * @param aCommand Command that was initiated
         */
        void ProcessCommandL( TInt aCommand );

    private: // Functions from MEikListBoxObserver

        /**
         * From MEikListBoxObserver Callback for listbox events
         * @since 1.2
         * @param aListBox Originating listbox
         * @param aEventType Event type
         */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );

    private:    // Functions from CCoeControl

        /**
         * Called by framework when system wide resources are changed.
         * @since 3.1
         */
        void HandleResourceChange( TInt aType );

        /**
         * From CCoeControl
         * @see CCoeControl for more information
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
         * From CEikDialog
         * @see CEikDialog for more information
         */
        void DisplayMenuL();
    protected:  // Functions from CEikDialog

        /**
         * From MEikMenuObserver
         * @see MEikMenuObserver
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
         * From CEikDialog
         * @see CEikDialog for more information
         */
        void PreLayoutDynInitL();

        /**
         * From CEikDialog
         * @see CEikDialog for more information
         */
        TBool OkToExitL( TInt aButtonId );

    private: //New methods


        /**
         * Lauches text-setting-page
         * @since 1.2
         */
        void DisplayTextSettingPageL();


        /**
         * Adds binary-type item to settings array
         * @since 1.2
         * @param aBuffer Buffer for setting value
         * @param aId Resource id for the associated values and texts
         * @param aTitleResource Title text's resource id
         * @param aSettingPageResource  Setting page's resource id
         * @param aAssociatedResource Associated resource
         * @see CAknSettingItem for more information about parameters
         */
        void AddBinaryItemL( TBool& aBuffer,
                             TInt aId,
                             TInt aTitleResource,
                             TInt aSettingPageResource,
                             TInt aAssociatedResource );

        /**
         * Adds enumerated text-type item to settings array
         * @since 2.1
         * @param aBuffer Buffer for setting value
         * @param aId Resource id for the associated values and texts
         * @param aTitleResource Title text's resource id
         * @param aSettingPageResource  Setting page's resource id
         * @param aAssociatedResource Associated resource
         * @see CAknSettingItem for more information about parameters
         */
        void AddEnumTextItemL( TInt& aBuffer,
                               TInt aId,
                               TInt aTitleResource,
                               TInt aSettingPageResource,
                               TInt aAssociatedResource );

        /**
         * Adds colour setting item to settings array.
         * @since S60 v3.2
         * @param aKey Key for key-value pair in SAP settings,
                       used to get colour information from SAP settings.
         * @param aBuffer Buffer for setting value.
         * @param aId Resource id for the associated values and texts.
         * @param aTitleResource Title text's resource id.
         * @param aSettingPageResource  Setting page's resource id.
         * @see CAknSettingItem for more information about parameters.
         */
        void AddColourItemL(
            const TDesC& aKey,
            TBool& aBuffer,
            TInt aId,
            TInt aTitleResource );

        /**
         * Stores the values of setting items to persistent storage
         * @since 1.2
         */
        void StoreValuesL();

        /**
         * Handles the errorcodes, leaves on non-CSP error codes
         * @since 2.5
         */
        void HandleErrorL( TInt aErrorCode );

        /**
         * Externalizes the values of setting items' iternal buffers
         * @since 1.2
         */
        void StoreAllL() const;

        /**
         * Updates title-pane text with text found from given resource id
         * @param aResourceId Resource id for text
         * @since 1.2
         */
        void UpdateTitlePaneL( const TInt aResourceId );

        /**
         *  Handle CBA change.
         *  @param aResourceId. Id for resource to use.
         *  @since 2.8
         */
        void HandleCBAChangeL( const TInt aResourceId );

        /**
         * Adds tone setting item to setting item array
         */
        void AddToneSettingItemL( TBool aBranded );


    private:    // Data

        // Owns. Array for setting items
        CAknSettingItemArray* iSettingItemArray;

        // Doesn't own. Pointer to status-pane handler
        CCAStatusPaneHandler* iTitlePane;


        //Doesn't own
        CCAUISessionManager* iUISessionManager;
        // Doesn't own. Pointer to listbox in settings dialog
        CAknSettingStyleListBox* iListbox;

        // Flag for default nickname usage
        TBool iDefaultNick;
        TBool iOldDefaultNick;

        // Enum for authorizing IM presence
        TInt iAuthorizeIMPr;
        TInt iOldAuthorizeIMPr;

        // Enum for receiving instant messages
        TInt iReceiveIMessages;
        TInt iOldReceiveIMessages;

        // Enum for receiving invitations
        TInt iReceiveInvitations;
        TInt iOldReceiveInvitations;

        // Value for message flow
        TInt iMsgFlow;
        TInt iOldMsgFlow;

        // Flag for contact list ordering
        TBool iOrderAlphabetically;
        TBool iOldOrderAlphabetically;

        // Flag for automatic friends-list update
        TBool iAutoUpdFriends;
        TBool iOldAutoUpdFriends;

        // Owns. Buffer for default nickname
        HBufC* iDefaultNickName;
        HBufC* iOldDefaultNickName;


        //owns. Buffer for tone filename
        HBufC* iToneFileName;
        TPtr iToneFileNamePtr;
        TBool iToneFileInUse;
        //Tells if branded tones are in use
        TInt iBrandedTones;

        // "show offline contacts"
        TBool iShowOffline;
        TBool iOldShowOffline;

        // is own/other message color defined
        TBool iOwnColorDefined;
        TBool iOthersColorDefined;

        // Show timestamp settings item
        TBool iOldShowTimeStamp;
        TBool iShowTimeStamp;

        // Ordinal number that is used to layout the dialog correctly
        TInt iOrdinal;

        MCASettingSapExt* iSAPExtension;

        // Doesn't own. Pointer to settings API
        MCASettingsPC* iSettingsPC;
        // async callback for showing dialogs
        CAsyncCallBack* iAsyncCallBack;

        // settingitem index which will be edited with callback
        TInt iEditIndex;

        // Is edit dialog launched from menu (ETrue)
        TBool iIsPopUp;

        // Not owned. the server whose settings we show
        CIMPSSAPSettings* iServer;

        // Not owned. Pointer to message flow setting item
        CAknSettingItem* iFlowSettingItem;

        // owns.
        CRepository* iCenRep;
        MCASettingsCommand* iCommandSetings;

        TBool iIsEditItemFlag;

        TBool iIsTargetDialogOpened;

        TBool iSavingFlag;
    };

#endif      // CCAAPPSETTINGSDIALOG_H   

// End of File
