/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  UI Call Back Interface for login,logout
*
*/



#ifndef MCAUILOGINCMDCB_H
#define MCAUILOGINCMDCB_H

//  INCLUDES
#include <e32std.h>


// FORWARD DECLARATIONS
class MIMPSConnProcessUi;
class TPEngWVCspServicesTree2;
// CLASS DECLARATION

/**
*  UI Call Back Interface for login,logout
*/
class MCAUiLoginCmdCB
    {
    public:// Enums

        /**
         * Use to ask if cmd is for login ,logout or exit
         * ELogin:  Login
         * ELogout: Logout
         * EExit: Exit
         */

        enum TLoginParams
            {
            ELogin = 0,
            ELogout,
            EQueryLogin,
            EExit

            };


    public:
        /**
        * Used by the command to check if it is a login,logout or exit command
        * @param aLoginParams is set with enum value for login or logout or exit
        */
        virtual void GetParameters( TLoginParams& aLoginParams ) = 0;

        /**
        * Updates status pane icon according to session state
        * @param aLandscapeOnly If ETrue, set the icons only in landscape mode
        */
        virtual void SetStatusPaneIconsL( TBool aLandscapeOnly = EFalse ) = 0;

        /**
        * Changes UI brand for selected logged in server
        * @since 2.1
        * @param aRefreshView Refresh current view after changing brand.
        *                     This closes dialogs so make sure that no dialogs
        *                     are open when this is called with ETrue
        * @param aServerName  If given, then branding will be done based on
                              given server, otherwise current server will be
                              retrieved from connectionUI/SAPSettingsStore
        * return Did branding actually change anything.
        */
        virtual TBool BrandUIL( TBool aRefreshView = ETrue,
                                const TDesC& aServerName = KNullDesC ) = 0;

        /**
         * Shows Post Login Notes if not always online case
         * @param aCurrentLoginIsFirst is true if the current login is first login
         * @param aManualLogin is true if login is manual else false
         */
        virtual void ShowPostLoginNotesL ( TBool aCurrentLoginIsFirst,
                                           TBool aManualLogin ) = 0;

        /**
         * Asks alias and updates presence, etc.
         */
        virtual void AskAliasL() = 0;

        /**
        * Refreshes current view
        * @since 2.1
        * @return ETrue, if refresh view launched. EFalse if not.
        */
        virtual TBool RefreshCurrentViewL() = 0;


        /**
        * Checks if there is default server information available, and launches
        * server-query, if not.
        * @param aShowNote If ETrue, show an error note in error situations
        * @return Tells if server information is available after call
        */
        virtual TBool CheckCurrentServerDataL(
            TBool aShowNote = ETrue ) = 0;

        /**
        * Notifes observer from SAP access events.
        * (Connection UI is logging into or logging
        * out from certain network server.)
        * @see MIMPSConnProcessUi.
        *
        * @since 2.1
        * @param aServerName The accessed server name.
        * @param aServerURL The accessed server URL.
        * @param aConnProcessUi Connection process UI.
        * If connection process is executed in environment
        * which doesn't have UI, aConnProcessUi is NULL.
        * Else aConnProcessUi is a valid pointer to
        * Connection Process UI interface.
        */
        //Callback from Command Session Handler

        virtual void HandleSapAccessEventEventL( const TDesC& aServerName,
                                                 const TDesC& aServerURL,
                                                 MIMPSConnProcessUi* aConnProcessUi ) = 0;
        /**
         * @return iCancelLogin value
         */
        virtual TBool CancelLoginValue() = 0;


        /**
        * Clears Universal Indication Pane's IM-icon
        */
        virtual void IMUIPIConForcedClear() = 0;

        /**
         * Closes all open dialogs on forced logout.
         */
        virtual void ShutDialogsL() = 0;

        /**
         * Capture events if needed
         */
        virtual void CaptureEventsL() = 0;

        /**
         * Releases events capturing
         */
        virtual void ReleaseCapturingL() = 0;

        /**
         * Refresh View After Login
         */
        virtual void RefreshViewAfterLoginL() = 0;

        /**
         * Shows wait dialog. Remember to call CleanupStack::Pop and DismissWaitDialogL
         * after this! Call CleanupStack::Pop first. Leaves one item in cleanup stack.
         * @param aTextResource Resource ID of text to be displayed
         * @param aShowImmediately Should the dialog be shown immediately
         */
        virtual void ShowProcessingNoteLC() = 0;


        /**
         * DismissProcessingNoteL . Remember to call DismissWaitDialogL
         * @param aError error to show
         */
        virtual void DismissProcessingNoteL( TInt aError, TBool aValue ) = 0 ;

        /**
         * Finalize construction of chat
         * @since 3.0
         */
        virtual void FinalizeChatConstructionL() = 0;

        /**
         * Sets the ImpsServices on UISessionManager by SessionHandler Cmd
         * @param aImpsServices - the Supported IMPS services
         * @param aBrandUIFlag - if true BrandUIL fn would be called else not
         * @param aRefreshFlag - if true refreshing of the view would be done in BrandUIL() else not
         */
        virtual void SetIMPSServices( TPEngWVCspServicesTree2 aImpsServices,
                                      TBool aBrandUIFlag = EFalse,
                                      TBool aRefreshFlag = EFalse ) = 0 ;




    };

#endif      // MCAUILOGINCMDCB_H

// End of File
