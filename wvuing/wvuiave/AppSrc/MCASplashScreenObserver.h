/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  AppUi class of application
*  Since			 : 3.2
*
*/


#ifndef M_MCASPLASHSCREENOBSERVER_H
#define M_MCASPLASHSCREENOBSERVER_H

/**
 *  Splash screen observer
 *
 *  Used to inform its inherited classes about events in the
 *	splash screen showing process
 *
 *  @since S60 v3.1
 */
class MCASplashScreenObserver
    {

    public:

        /**
         * SplashScreenShowed.
         *
         * @since S60 ?S60_version
         * @param ?arg1 ?description
         * @param ?arg2 ?description
         * @return ?description
         */
        virtual void SplashScreenShowed() = 0;

    };


#endif // M_NCDSPLASHSCREENOBSERVER_H
