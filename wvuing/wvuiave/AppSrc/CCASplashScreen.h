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


#ifndef __CCASPLASHSCREEN_H__
#define __CCASPLASHSCREEN_H__

#include <e32base.h>
#include <e32std.h>
#include <COEAUI.H>
#include <EIKENV.H>

#include  "MCASplashScreenObserver.h"

class CCAAppUi;
class CCASplashScreenControl;
//class CNcdDialogNotifier;
//class MNcdSkinData;

class CCASplashScreen : public CActive
    {
    public:

        // Two-phase Constructors
        static CCASplashScreen* NewL( CCAAppUi* aAppUi );

        static CCASplashScreen* NewLC( CCAAppUi* aAppUi );

        // destructor
        ~CCASplashScreen();

    public: // from CActive
        /**
         * Executed when request is completed
         */
        void RunL();

        /**
         * Cancels outstanding requests
         */
        void DoCancel();

        TInt RunError( TInt aError );

    public:	// Added functions

        // Takes ownership of aSplashImage
        void ShowSplashScreenL( MCASplashScreenObserver* aSplashScreenListener,
                                const TInt& aTime,
                                CGulIcon* aSplashImage,
                                /*MNcdSkinData* aSkinData,*/
                                TBool aDismissable );

        // Cancels the timer and makes an asynchronous call that
        // is completed immediately.
        // This can be used to close the splashscreen from the splash control.
        //
        // Asynchronous call is needed for some reason to make
        // the disclaimer dialog show.
        void DismissSplashScreen();

    protected:

        // default constructor
        CCASplashScreen( CCAAppUi* aAppUi );

        //Constructor method
        void ConstructL( );

        void SplashScreenDismissed();

    private: // data

        CCASplashScreenControl* iSplashControl;

        CCAAppUi* iAppUi;


        /**
         * A listener for this class - e.g. CNcdStartup instantiates this.
         * A class for this pointer is given as a parameter of ShowSplashScreenL().
         */
        MCASplashScreenObserver* iSplashScreenListener;

        RTimer iTimer;
    };

#endif // __CCASPLASHSCREEN_H__


