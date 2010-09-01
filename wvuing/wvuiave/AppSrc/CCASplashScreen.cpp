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


// INCLUDE FILES
#include  "CCASplashScreen.h"
#include  "CCASplashScreenControl.h"
#include  "CCAAppUi.h"

CCASplashScreen* CCASplashScreen::NewL( CCAAppUi* aAppUi )
    {
    CCASplashScreen* self = CCASplashScreen::NewLC( aAppUi );
    CleanupStack::Pop(); //self

    return self;

    }

CCASplashScreen* CCASplashScreen::NewLC( CCAAppUi* aAppUi )
    {
    CCASplashScreen* self = new ( ELeave ) CCASplashScreen( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

CCASplashScreen::CCASplashScreen( CCAAppUi* aAppUi )
        : CActive( EPriorityStandard ), iSplashControl( NULL ), iAppUi( aAppUi )
    {
    CActiveScheduler::Add( this );
    }

void CCASplashScreen::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    }

CCASplashScreen::~CCASplashScreen()
    {
    Cancel();
    iTimer.Close();

    iSplashScreenListener = NULL;
    SplashScreenDismissed();

    }

void CCASplashScreen::ShowSplashScreenL( MCASplashScreenObserver* aSplashScreenListener,
                                         const TInt& aTime,
                                         CGulIcon* aSplashImage,
                                         /*MNcdSkinData* aSkinData,*/
                                         TBool aDismissable  )
    {
    Cancel();		// Cancel any request, just to be sure

    iSplashScreenListener = aSplashScreenListener;

    if ( !iSplashControl )
        {
        CleanupStack::PushL( aSplashImage );

        CCASplashScreenControl* tmpSplashControl =
            CCASplashScreenControl::NewLC( *this );

        iAppUi->AddToStackL( tmpSplashControl );

        CleanupStack::Pop( tmpSplashControl );

        CleanupStack::Pop( aSplashImage );

        iSplashControl = tmpSplashControl;
        }

    iSplashControl->ShowSplashScreen( aSplashImage,/* aSkinData,*/ aDismissable );

    // after aSplashTime go to RunL and remove splash screen from the view
    iTimer.After( iStatus, aTime * 1000000 );
    SetActive();	// Tell scheduler a request is active
    /*
        // get current time (microseconds since 0AD nominal Gregorian)
    	TTime time;
    	time.HomeTime();

    	// add 3 seconds to the time
    	TTimeIntervalSeconds timeIntervalSeconds(3);
    	time += timeIntervalSeconds;

    	TRequestStatus timerStatus;
    	// issue and wait
    	iTimer.At(timerStatus,time);
    	User::WaitForRequest(timerStatus);
    */
    }

void CCASplashScreen::RunL()
    {
    SplashScreenDismissed();
    }

TInt CCASplashScreen::RunError( TInt aError )
    {
    return aError;
    }

void CCASplashScreen::DoCancel()
    {
    iTimer.Cancel();
    }

void CCASplashScreen::SplashScreenDismissed()
    {
    // remove splashscreen by destroying the control
    if ( iSplashControl )
        {
        iSplashControl->MakeVisible( EFalse );
        iAppUi->RemoveFromStack( iSplashControl );
        delete iSplashControl;
        iSplashControl = NULL;
        }

    // Inform the listener.
    if ( iSplashScreenListener )
        {
        iSplashScreenListener->SplashScreenShowed();
        iSplashScreenListener = NULL;
        }
    }

void CCASplashScreen::DismissSplashScreen()
    {
    // Cancel outstanding requests
    Cancel();

    // Make a request and complete it
    // Call is made to RunL() and splash screen will be taken care of
    iStatus = KRequestPending;
    TRequestStatus* tmp = &iStatus;
    SetActive();
    User::RequestComplete( tmp, KErrNone );
    iAppUi->HandleCommandL( EAknCmdExit );
    }

// End of File
//
//
