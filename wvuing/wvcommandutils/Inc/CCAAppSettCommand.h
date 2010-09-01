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
* Description:  Command to get the Settings Interface for getting/setting tonefilename etc
*
*/



#ifndef CCAAPPSETTCOMMAND_H
#define CCAAPPSETTCOMMAND_H

//  INCLUDES
#include "MCACommand.h"
#include "MCASettingsCommand.h"
#include "MCASettingSapExt.h"
#include "CCACommandSettingSapExtCmd.h"
#include "MCASettingsPc.h"
#include "CIMPSSAPSettings.h"
#include "MCAProcessManager.h"
#include "CCASessionHandlerCmd.h"
#include "CCAEngine.h"
#include "MCAUiSettingsDialogCmdCB.h"


#include <e32base.h>


// CLASS DECLARATION
class CProcessManager;
class MCAChatAppSettCommand;
class MCAUiBaseCmdCB;
class MCAProcessManager;
class CIMPSSAPSettings;
class CPEngNWSessionSlotID2;
class MCAUiSettingsDialogCmdCB;



/**
*  Command base class.
*
*  @lib N/A
*
*/
NONSHARABLE_CLASS( CCAAppSettCommand ) : public CBase,
        public MCASettingsCommand
    {
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aProcessManager Reference to the process manager component
    * @param aSetting Reference to the UI Settings Callback interface
    * @to get the required parameters from UI side
    */

    static CCAAppSettCommand* NewL( MCAProcessManager& aProcessManager,
    MCAUiSettingsDialogCmdCB& aSetting );

    /**
    * Two-phased constructor.
    * @param aProcessManager Reference to the process manager component
    * @param aSetting Reference to the interface
    * @to get the required parameters from UI side
    */

    static CCAAppSettCommand* NewLC( MCAProcessManager& aProcessManager,
                                     MCAUiSettingsDialogCmdCB& aSetting );

    /*
    Destructor.
    */
    virtual ~CCAAppSettCommand();

public:

    /**
    * Set command observer.
    *
    * @param aObserver Command observer
    */
    void SetObserver( MCACommandObserver& aObserver );

    /**
    * Cancel command.
    *
    */
    void CancelCommand();
    /**
    * Executes the command.
    *
    */
    void ExecuteCommandL();

    /**
    *
    * @return ETrue if the command is asynchronous, otherwise EFalse.
    */
    TBool IsAsynchronous();


    /*
    * @GetAppSettingsSAPExt returns the interface
    * @used for getting/setting Application Settings
    */
    MCASettingSapExt*  GetAppSettingsSAPExt();

protected:

    /**
    * Complete command.
    * @param aError Commend completed with this error code
    */
    void CompletedL( TInt aError );

protected:

    /**
    * C++ default constructor.
    */
    CCAAppSettCommand( MCAProcessManager& aProcessManager,
                       MCAUiSettingsDialogCmdCB& aSetting );

private:
    // Ref: command observer
    MCACommandObserver* iCommandObserver;

    MCAProcessManager& iProcessManager;

    CIMPSSAPSettings* iSAPSettings;

    MCAUiSettingsDialogCmdCB& iAppSetting;

    MCASettingSapExt* iSapExtension;


    };

#endif      // CCAAPPSETTCOMMAND_H
// End of File
