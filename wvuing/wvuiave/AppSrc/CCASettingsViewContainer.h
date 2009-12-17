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
* Description:  Container class settings view
*
*/



#ifndef CCASETTINGSVIEWCONTAINER_H
#define CCASETTINGSVIEWCONTAINER_H

//  INCLUDES
#include "MCALayoutChangeObserver.h"
#include "impsbuilddefinitions.h"

#include <aknlists.h>
#include <eiklbo.h>	// MEikListBoxObserver
#include <bldvariant.hrh>
#include <MWVSettingsObserverng.h>
#include "MCASettingsPC.h"
#include "MCAProcessManager.h"

// CLASS DECLARATION
class MCAViewSwitcher;
class CCAEngine;
class CIMPSSAPSettingsStore;
class CWVSettingsUIDialog;
class MCAOpBrandVariant;
class CCAAppUi;

// CLASS DECLARATION

/**
 *  Container for class settings view
 *
 *  @lib chat.app
 *  @since 1.2
 */
class CCASettingsViewContainer : public CCoeControl,
            public MEikListBoxObserver,
            public MWVSettingsObserver,
            public MCALayoutChangeObserver
    {
    public: // enums
        enum TSettingItemLayout // enum values must match index in listbox.
            {
            EApplicationSettings = 0,
            EServiceSettings
            };

    public:  // Constructors and destructor
        /**
        * Default constructor
        * @param aViewSwitcher Handle to view-switcher
        * @param aSAPSettingsStore Pointer to SAP settings store
        * @param aSettings Pointer to settings API
        */
        CCASettingsViewContainer( MCAViewSwitcher& aViewSwitcher,
                                  MCASettingsPC& aSettings  );
        /**
        * By default Symbian OS constructor is private.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        virtual ~CCASettingsViewContainer();

    public: // New functions

        /**
        * Returns Pointer to listbox
        * @return Listbox pointer
        */
        CAknSettingStyleListBox* Listbox() const;

        /**
        * Launches application settings dialog
        */
        void LaunchAppSettingsDialogL();

        /**
        * Launches server settings dialog
        */
        void LaunchServSettingsDialogL( TBool aForcedLaunch = EFalse );

#ifdef RD_SERVICE_ENHANCEMENTS
        /**
         * Returns ETrue if ServSettingsDialog should
         * be launched next time settings view is activated.
         * Used when forced login made in SercSettings dialog,
         * login causes view switch between refresh view and settings view
         * -> dialogs are closed.
         * @since S60 v3.2
         */
        TBool IsServSettingsDialogActive() const;
#endif  // RD_SERVICE_ENHANCEMENTS

    private: // Functions from MEikListBoxObservers

        /**
        * @see MEikListBoxObservers
        */
        virtual void HandleListBoxEventL( CEikListBox* aListBox,
                                          TListBoxEvent aEventType );

    private: //From CCoeControl

        /**
        * @see CCoeControl
        */
        TInt CountComponentControls() const;

        /**
        * @see CCoeControl
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * @see CCoeControl
        */
        void SizeChanged();

        /**
        * @see CCoeControl
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent,
                                     TEventCode aType );

        /**
        * @see CCoeControl
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
         * From CCoeControl.
         * Handle focus event change.
         * @see CCoeControl
         */
        void FocusChanged( TDrawNow aDrawNow );

    private: // From MWVSettingsObserver

        /**
         * @see MWVSettingsObserver
         */
        void ServerChangedL ( const TDesC& aNewServerName, TBool aDefaultServerChanged = EFalse );

        /**
         * @see MWVSettingsUICaller
         */
        HBufC* CurrentServerNameLC();

    private: // From MCALayoutChangeObserver

        /**
         * Called when layout/skins change
         * @since 2.1
         */
        void LayoutChangedL( TInt aType );

    private:    // Data

        // Doesn't own. View switcher
        MCAViewSwitcher& iViewSwitcher;

        // Owns. Listbox.
        CAknSettingStyleListBox* iListBox;

        // Owns. SAP settings store
        CIMPSSAPSettingsStore* iSAPSettingsStore;

        // Doesn't own. Handle to application settings.
        MCASettingsPC& iSettingsPC;

        // Does settings view need to be refreshed
        TBool iNeedRefresh;

        // Doesn't own (destroys itself).
        CWVSettingsUIDialog* iServSettingsDialog;

        // Doesn't own (singleton).
        MCAOpBrandVariant* iOpBrand;

        //Doesn't own
        CCAAppUi* iAppUi;

#ifdef RD_SERVICE_ENHANCEMENTS
        // ETrue while service settings dialog launched
        TBool iServSettingsDialogActive;
#endif  // RD_SERVICE_ENHANCEMENTS
    };

#endif      // CCASETTINGSVIEWCONTAINER_H

// End of File
