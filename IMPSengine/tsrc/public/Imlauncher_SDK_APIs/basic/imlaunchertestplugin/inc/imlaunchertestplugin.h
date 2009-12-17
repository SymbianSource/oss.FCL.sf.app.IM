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
* Description:  imlaunchertest test plugin implementation module.
*
*/
//

#ifndef IMLAUNCHERTESTPLUGIN_H
#define IMLAUNCHERTESTPLUGIN_H

#include <imlauncherplugin.h>

// An implementation of the CExampleInterface definition
class CTestPlugin : public CImLauncherPlugin
    {
    public:
        CTestPlugin();
        void ConstructL();

        static CTestPlugin* NewL();

        // Destructor
        virtual ~CTestPlugin();

        // Implementation of CImLauncherPlugin
        void StartApplicationL(
            TRequestStatus& aStatus,
            const TDesC& aSap,
            const TDesC& aUserId );

        void  CancelStartApplication();

    public: //Data member
        //RProcess p;

    private:
        TRequestStatus*    iStatus;
    };

#endif
