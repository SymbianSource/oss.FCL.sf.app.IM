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
* Description:  Multi/single contact selection dialog
*
*/



#ifndef CCACONTACTSELECTIONDIALOG_H
#define CCACONTACTSELECTIONDIALOG_H

//  INCLUDES
#include "MCAMainViewObserverPC.h"
#include "MCAMainViewArrayPC.h"

#include "TEnumsPC.h"
#include "CCAAppUi.h"
#include <akndialog.h>
#include <eiklbo.h>


// FORWARD DECLARATIONS
class CAknSingle2GraphicStyleListBox;
class CAknSearchField;
class MCASkinVariant;
class CAknNavigationControlContainer;
class CCAStatusPaneHandler;
class CAknNavigationDecorator;
class CCAContactListBox;
class MCASettingsPC;

// CLASS DECLARATION

/**
 *  Multi/single contact selection dialog
 *  Used for selecting contact(s)
 *
 *  @lib chat.exe
 *  @since Series 60 3.0
 */



class CCAContactSelectionDialog : public CAknDialog,
            public MEikListBoxObserver,
            public MCAMainViewObserverPC
    {
    public:  // Constructors and destructor

        /**
          * static method for displaying the dialog
          * @param Array to be filled by on pcside and returned
          * with selectedcontacts,
          * @param aSelectMultiple ETrue if multiplecontacts
          *                        need to be selected(selection
          *                        list to be displayed)
          *                        else EFalse
          * @param aFilter If defined it is used, otherwise
          *                filtering is based on variation
          *                and settings.
          * @param aDisplayFilter If not defined display
          *                filtering is based on iFilter.
         */
        static TBool ShowDialogL(
            CDesCArray& aSelectedContacts,
            MCAMainViewArrayPC& aMainViewArrayPC,
            MCASkinVariant& aSkinVariant,
            MCASettingsPC& aSettings,
            TDesC& aMbmPath,
            TEnumsPC::TListSelectionType aSelectMultiple,
            TInt aResourceId,
            CCAStatusPaneHandler& aStatusPaneHandler,
            CDesCArray* aSelectedIdentifications,
            TEnumsPC::TFilterType aFilter
            = TEnumsPC::EFilterNotDefined,
            TEnumsPC::TFilterType aDisplayFilter
            = TEnumsPC::EFilterNotDefined,
            TBool aBlockingSelection = EFalse,
            TInt aSelectedListIndex = 0,
            TBool* aShutDialogFromFSW = NULL );

    protected:  // Functions from MCAMainViewObserverPC

        /**
          * From MCAMainViewObserverPC, called when a contact is deleted.
          * @since 2.1
          * @see MCAMainViewObserverPC
          */
        virtual void HandleDelete( const TDesC& aContactId ,
                                   TEnumsPC::TItem aType );

        /**
         * From MCAMainViewObserverPC, called when a contact is added.
         * @since 2.1
         * @see MCAMainViewObserverPC
         */
        virtual void HandleAddition( TEnumsPC::TItem aType, TInt aIndex );

        /**
         * From MCAMainViewObserverPC, called when a contact is changed.
         * @since 2.1
         * @see MCAMainViewObserverPC
         */
        virtual void HandleChange( TEnumsPC::TItem aType, TInt aIndex,
                                   TEnumsPC::TChange changeType );

        /**
         * Handles setting changes.
         * @param aChangedSettingEnum specifies the changed setting.
         */
        virtual void HandleSettingsChangeL( TInt aChangedSettingEnum );

        /**
         * From MCAMainViewObserverPC, called when a contact is changed.
         * @since 2.1
         * @see MCAMainViewObserverPC
         */
        virtual void HandleWatcherEvent( const TDesC &  aString , TEnumsPC::TOnlineStatus aOnlineStatus );

        /**
         * From MCAMainViewObserverPC, called when a invitation is received.
         * @since 2.1
         * @see MCAMainViewObserverPC
         */
        virtual void HandleInvitationEventL( TEnumsPC::TInviteType /* aInviteType */,
                                             const TDesC& aUserID,
                                             const TDesC& aMessage );

        /**
         * From MCAMainViewObserverPC, called when a invitation is received.
         * @since 2.1
         * @see MCAMainViewObserverPC
         */
        virtual void HandleInvitationResponse( TBool /*aAcceptance*/,
                                               const TDesC& /*aUserId*/,
                                               const TDesC&  /*aGroupName*/,
                                               const TDesC& /*aResponse*/ );
    protected:	// Functions from MEikListBoxObserver

        /**
         * Handles list box event
         */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );

    protected:  // Functions from MCoeControlObserver

        /**
         * Handles events from findpane
         */
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );


    protected:  // Functions from CAknDialog

        /**
         * @see CCoeControl
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
        * @see CCoeControl
        */
        TInt CountComponentControls() const;

        /**
        * @see CCoeControl
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * @see MEikMenuObserver
         */
        void DynInitMenuPaneL( TInt aResourceId,
                               CEikMenuPane* aMenuPane );

        /**
         * @see CEikDialog
         */
        void PreLayoutDynInitL();

        /**
           * @see CEikDialog
         */
        void PostLayoutDynInitL();

        /**
         * @see CEikDialog
         */
        TBool OkToExitL( TInt aButtonId );

        /**
         * @see CAknDialog
         */
        void ProcessCommandL( TInt aCommandId );

        /**
         * @see CAknDialog
         */
        void Draw( const TRect &aRect ) const;

        /**
         * @see CAknDialog
         * @since S60 v3.1
         */
        void SetSizeAndPosition( const TSize& aSize );

        /**
         * Creates custom controls for dialog:
         *      - Search field
         *      - List box, CCAContactListBox
         * @see CAknDialog
         * @since S60 v3.1
         */
        SEikControlInfo CreateCustomControlL( TInt aControlType );

        /**
         * @see CEikDialog
         */
        void HandleDialogPageEventL( TInt aEventID );

    private:    // new functions

        /**
         * Current index
         * @return current index
         */
        TInt CurrentIndex();

        /**
         * Set new navi label for current situation.
         */
        void SetNaviLabelL();

        /**
         * Reset the search box.
         * @since 3.0
         */
        void ResetFindBoxL();

        /**
         * Update the filter when collapsing the list.
         * @since 3.0
         */
        void UpdateFilterL();

        /**
         * Return search field from dialog.
         * @since S60 v3.1
         * @return CAknSearchField
         */
        CAknSearchField* FindBox() const;

        /**
         * Return list box from dialog.
         * @since S60 v3.1
         * @return CEikListBox, can be casted to CCAContactListBox
         */
        CEikListBox* ListBox() const;

        /**
         * Updates CBA.
         * @since S60 v3.1
         */
        void UpdateCbaL();

    private:

        /**
          * C++ default constructor.
           */
        CCAContactSelectionDialog(
            CDesCArray& aSelectedContacts,
            MCAMainViewArrayPC& aMainViewArrayPC,
            MCASettingsPC& aSettings,
            MCASkinVariant& aSkinVariant,
            TDesC& aMbmPath,
            TEnumsPC::TListSelectionType aSelectMultiple,
            CCAStatusPaneHandler& aStatusPaneHandler,
            CDesCArray* aSelectedIdentifications,
            TEnumsPC::TFilterType aFilter,
            TEnumsPC::TFilterType aDisplayFilter,
            TBool* iShutDialogFromFSW,
            TBool aBlockingSelection );

        /**
         * By default Symbian 2nd phase constructor is private.
          */
        TBool ConstructL( TInt aSelectedListIndex = 0 );

        /**
        * Layouts current components according to AVKON LAF
        */
        void SetLayout();

        /**
         * Destructor.
          */
        virtual ~CCAContactSelectionDialog();

    private:    // Data

        // Doesn't own.
        CDesCArray& iSelectedContacts;

        // Doesn't own.interface to MainViewArrayPC
        MCAMainViewArrayPC& iMainViewArrayPC;

        // Doesn't own. interface to SettingsPC
        MCASettingsPC& iSettings;

        // Doesn't own. interface to skinvariant for loading bitmaps
        MCASkinVariant& iSkinVariant;

        // Doesn't own. path to bitmap file
        TDesC& iMbmPath;

        // Can we select multiple contacts
        TEnumsPC::TListSelectionType iSelectMultiple;

        // Doesn't own, interface to navipane
        CAknNavigationControlContainer* iNaviPane;

        // Owns. Navi pane text.
        CAknNavigationDecorator* iNaviPaneText;

        // Do we have pushed navipane in stack
        TBool iNaviPanePushed;

        // Do we need to remove the cleared navepane from stack
        TBool iNaviPaneCleared;

        // Owns. old titlepane text
        HBufC* iOldTitle;

        // selection counter
        TInt iSelectionCount;

        // set this to ETrue before calling TryExitL
        TBool iExiting;

        // filter that should be used with contact list model
        TEnumsPC::TFilterType iFilter;

        // filter that was previously used
        TEnumsPC::TFilterType iDisplayFilter;

        // status pane handler
        CCAStatusPaneHandler& iStatusPaneHandler;

        // Doesn't own.
        CDesCArray* iSelectedIdentifications;

        TBool iBlockingSelection;

        CCAAppUi* iAppUi;

        // if success remove from cleanupstack in destructor
        TBool iDialogCreationSuccess;

        // ETrue if user try to exit IM from FSW (Fast Swap Window), else always is EFalse.
        TBool* iShutDialogFromFSW;
    };

#endif      // CCACONTACTSELECTIONDIALOG_H

// End of File
