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
* Description:  Service Settings UI
*
*/



#ifndef CWVSETTINGSUIDIALOG_H
#define CWVSETTINGSUIDIALOG_H


//  INCLUDES
#include <e32base.h>
#include <akndialog.h>

// DATA TYPES

enum TWVSettingsDialogReturnValue
    {
    EWVSettingsViewOk,
    EWVSettingsViewExitCalled,
    EWVSettingsViewForceExit
    };


enum TWVSettingsDialogType
    {
    EWVSettingsNormal,
    EWVSettingsDefineServer
    };

enum TExitReasons
    {
    EServerSaved = 0,
    EMissingCompulsory, // server not saved
    EApplicationExit,
    ENotSaved,
    EForcedExit
    };


// FORWARD DECLARATIONS
class CIMPSSAPSettingsStore;
class MWVSettingsObserver;
class MWVSettingsCASettings;

// CLASS DECLARATION

/**
*  Server settings dialog
*  Basicly this dialog gives that two links to other dialogs, to current
*  server selection and to server list
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/
class CWVSettingsUIDialog : public CAknDialog
    {

    public: // New functions

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWVSettingsUIDialog* NewL();

        /**
        * Runs the dialog.
        * @param aSAPSettingsStore Pointer to SAP settings store
        * @param aObserver Pointer to the UI observer
        * @param aResourceFile The path to a branded resource file wanted to be used
        * @param aResourceVariationFile	 The variation file name that contains the settings
        * @param aExitReason The exit reason of the dialog
        * @param aWhatToRun Tells whether to run the whole dialog
        *                   or just the new server definition dialog
        */
        virtual TWVSettingsDialogReturnValue RunDialogLD (
            CIMPSSAPSettingsStore& aSAPSettingsStore,
            MWVSettingsObserver* aObserver,
            const TDesC& aResourceFile,
            const TDesC& aResourceVariationFile,
            TInt& aExitReason,
            MWVSettingsCASettings* aCAServerSettings,
            CWVSettingsUIDialog** aSelfPtr,
            TBool aForcedLaunch = EFalse,
            TWVSettingsDialogType aDialogType = EWVSettingsNormal ) = 0;

        /**
        * Changes the resource file used
        * @param aResourceFileName The path to the resource file wanted to be used
        * @param aResourceVariationFile	The file name of the variation file to be changed.
        */
        virtual void ChangeResourceFileL ( const TDesC& aResourceFileName,
                                           const TDesC& aResourceVariationFile ) = 0;

    protected:

        /**
        * C++ default constructor.
        */
        CWVSettingsUIDialog();

        /**
        * Destructor.
        */
        virtual ~CWVSettingsUIDialog();

    };

#endif      // CWVSETTINGSUIDIALOG_H

// End of File
