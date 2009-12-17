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
* Description:  imlaunchertest test module.
*
*/


#ifndef IMLAUNCHERTESTCLASS_H
#define IMLAUNCHERTESTCLASS_H

#include <imlauncherplugin.h>

//const TUid KTestAppUid (TUid::Uid(0x101FB0CF));

//#define KTestAppUid "defaultdata"
//Forward declaration
class CImLaunchertestclass;

/* class CImTestShutDownTimer : public CTimer
    {
    public:
        static CImTestShutDownTimer* NewL( CImLaunchertestclass* aLauncher, TInt aPriority );
   */
/**
* Destructor
*/
//virtual ~CImTestShutDownTimer();

//protected:

// From base class
//void RunL();

/**
* Constructor
* @param aServer WV engine server
* @param aPriority Active objects priority
*/
/* CImTestShutDownTimer( CImLaunchertestclass* aLauncher, TInt aPriority );

void ConstructL();

protected:
CImLaunchertestclass*    ilauncher;
    };*/

/**
*  Client application plugin loader
*
*  @since 2.6
*/
class CImLaunchertestclass : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CImLaunchertestclass* NewL();

        /**
        * Destructor.
        */
        virtual ~CImLaunchertestclass();

    public: // New functions

        /**
        * Loads the client application pluging and start the application
        * @since 2.6
        * @param aParams (Application ID, SAP URL and User ID)
        */
        TInt StartL();

        TInt ThreadFunction();

        TInt CancelApplication();

        TInt StartPluginL();

    public: //Data Member
        TBool iCanceled; //Will be set to TRUE if Cancel Testcase is called.




    protected:  // New functions

        /**
        * Custom panic funtion
        * @since 2.6
        * @param aPanicCode
        */
        //static void Panic(TLauncherPanic aPanicCode);


        /**
        * TimeOut handler function
        * @since 2.6
        */
        //void TimedOut();

    protected:  // Functions from base classes
        /**
        * From CActive
        */
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CImLaunchertestclass();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        CImLauncherPlugin*      iPlugin; // client application ECom plugin
        //CImTestShutDownTimer*       iTimer; // shut down timer
        TInt                    iReturnValue; // retval to the thread starter

        //static CImLaunchertestclass *launcher;  //own pointer
    };



#endif //IMLAUNCHERTESTCLASS_H

