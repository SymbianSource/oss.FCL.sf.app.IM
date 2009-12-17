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
* Description:  Declares accesslist dialog
 *
*/


#ifndef CCAACCESSLISTDIALOG_H
#define CCAACCESSLISTDIALOG_H

//  INCLUDES
#include <aknform.h>
#include <eiklbo.h>
#include <bldvariant.hrh>

// FORWARD DECLARATIONS
class CCAStatusPaneHandler;
class CCAAccessArray;
class CCAAppUi;
class MCAGroupPropertiesPC;


// CLASS DECLARATION
/**
 *  This class declares dialog for accesslist
 *  @lib chatng.app
 */
class CCAAccessListDialog : public CAknDialog,
            public MEikListBoxObserver
    {
    public: // Definitions

        /// Enumeration for list types
        enum TListType
            {
            EAdminList = 0,
            EAccessList,
            EBannedList
            };

    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aTitlePane Handle to title pane
         * @param aUserList List of users
         * @param aIsMemberAccessModified Will be set to ETrue when member's
         *                                access is modified
         * @param aFirstItemCanBeEdited The first item can be edited or not
         * @param aRemoveAllShouldExitDialog Removing all members should exit
         *                                   the dialog after removal
         * @param aTitleResource The title resource of the dialog
         * @param aFilterList users that should not be put on the aUserList
         * @param aFilterList2 users that should not be put on the aUserList
         */
        CCAAccessListDialog( CCAStatusPaneHandler& aTitlePane,
                             CDesCArray& aUserList,
                             TBool& aIsMemberAccessModified,
                             TBool aFirstItemCanBeEdited,
                             TBool aRemoveAllShouldExitDialog,
                             TInt aTitleResource,
                             TListType aListType,
                             CDesCArray* aFilterList = NULL,
                             CDesCArray* aFilterList2 = NULL
                           );

        /**
         * Destructor
         */
        virtual ~CCAAccessListDialog();
        /**
         * Symbian OS Second Phased constructor
         * @param aMenuBarResourceId Menu items to be shown in options menu
         */
        void ConstructL( TInt aMenuBarResourceId );


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
         * This method displays data query for WVID
         */
        void DisplayManualDataQueryL();

        /**
         * This method launches popup list that contains items from friends list
         */
        void DisplayPopupFriendsListL();

        /**
         * This method shows confirmation dialog when item is removed from list
         */
        void DisplayRmvConfirmationL();

        /**
         * Displays confirmation if all items are deleted from list
         */
        void DisplayRmvAllConfirmationL();

        /**
         * This method displays pop up menu
         */
        void DisplayPopupMenuL();

        /**
         * Checks if given user id is in given list
         * and checks if it should be removed from other.
         * @param aUserList List of users
         * @param aUserId User id
         * @return ETrue if on list EFalse if not
         */
        TBool CanUserBeAddedToListL( const CDesCArray& aUserList, const TDesC& aUserId );

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

    protected: // MEikListBoxObserver

        /**
         * @see MEikListBoxObserver
         */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

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
         * From CEikDialog. @see CEikDialog for more details.
         */
        TInt MappedCommandId( TInt aButtonId );

    private:    // Data

        //Handle to titlepane. doesn't own
        CCAStatusPaneHandler* iTitlePane;

        //Access array. owns
        CCAAccessArray* iItemArray;

        //UserId. owns
        HBufC* iWVUserId;

        //UserList. doesn't own
        CDesCArray& iUserList;

        //Pop-up menu. Owns
        CEikMenuBar* iPopupMenu;

        //Indicates if member access is modified
        TBool& iIsMemberAccessModified;

        // whether the first item can be edited or not
        TBool iFirstItemCanBeEdited;

        // Indicates whether selecting the "remove all"-option
        // should exit the dialog or not
        TBool iRemoveAllShouldExitDialog;

        //Title of the dialog
        TInt iTitleResource;

        // These variables are used for menubar and prompts
        TInt iMenuBarResourceId;
        TInt iDeletePromptResourceId;
        TInt iDeleteAllPromptResourceId;

        // Owns the resource
        HBufC* iEmptyText;

        // Topic for help launcher, not owned, just points to constant help topic
        const TDesC* iHelpTopic;

        /**
        * Not owned just referenced. The Filters are used to identify whether a
        * contact has to be removed from say the
        		* banned list when added to access list
        		* access list when added to banned list
        		* banned list when added to admin privileges list
        		* admin privileges list when added to banned list
        * The contact will be searched in these filters passed on from the goup
        * properties dialog.
        */
        CDesCArray* iFilterList;

        /**
        * Not owned just referenced. The Filters are used to identify whether a
        * contact has to be removed from say the
        		* banned list when added to access list
        		* access list when added to banned list
        		* banned list when added to admin privileges list
        		* admin privileges list when added to banned list
        * The contact will be searched in these filters passed on from the goup
        * properties dialog.
        */
        CDesCArray* iFilterList2;

        /// List type.
        TListType iListType;

        // EFalse, if numeric input for WVID fields is wanted
        TBool iTextualInputMode;

        //Reference to the app ui instance
        CCAAppUi* iAppUi;

        //Pointer to group properties dialog pc
        MCAGroupPropertiesPC* iGroupPropertiesPC;
    };

#endif      // CCAACCESSLISTDIALOG_H   

// End of File
