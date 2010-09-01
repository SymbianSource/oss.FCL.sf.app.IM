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
* Description:  Abstract factory for creating command objects.
*
*/



#ifndef MCAUISETTINGSDIALOGCMDCB_H
#define MCAUISETTINGSDIALOGCMDCB_H

//  INCLUDES
#include <e32std.h>
//#include "MCACommandObserver.h"


// FORWARD DECLARATIONS
class CIMPSSAPSettings;

// CLASS DECLARATION

/**
*  Interface for CallBack Method for Settings command component,
*  to retreive Settings Parameters from the UI Component.
*  @lib
*  @since 2.6
*/
class MCAUiSettingsDialogCmdCB
    {
    public:
        class TSettingsDialogParams
            {
            public:
                TSettingsDialogParams()
                    {

                    }
                TBool iPreserveSessionIdInExit;
                CIMPSSAPSettings* iSap;
            };

    public:
        /**
        * Executes the command.
        * @param aSettingsDialogParams - Retreive by reference from UI Component
        * @since 2.6
        */
        virtual void GetParameters( TSettingsDialogParams& aSettingsDialogParams ) = 0;

        virtual ~MCAUiSettingsDialogCmdCB()
            {

            }

    };

#endif      // MCAUILOGINCMDCB_H
// End of File
