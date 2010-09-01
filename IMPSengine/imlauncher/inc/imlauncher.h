/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Client application plugin loader.
*
*/



#ifndef CIMLAUNCHER_H
#define CIMLAUNCHER_H

//  INCLUDES
#include <e32base.h>
#include    "impsservercommon.h"

// CONSTANTS
_LIT( KLauncherPanic, "IM Launcher Panic" );

// MACROS

// DATA TYPES
enum TLauncherPanic
    {
    ECreateTrapCleanup = 1
    };


// FORWARD DECLARATIONS
class CImLauncherPlugin;
class CImLauncher;

// CLASS DECLARATION

class CImShutDownTimer : public CTimer
    {
    public:
        static CImShutDownTimer* NewL( CImLauncher* aLauncher, TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImShutDownTimer();

    protected:

        // From base class
        void RunL();

        /**
        * Constructor
        * @param aServer WV engine server
        * @param aPriority Active objects priority
        */
        CImShutDownTimer( CImLauncher* aLauncher, TInt aPriority );

        void ConstructL();

    protected:
        CImLauncher*    iLauncher;
    };



/**
*  Client application plugin loader
*
*  @since 2.6
*/
class CImLauncher : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CImLauncher* NewL();

        /**
        * Destructor.
        */
        virtual ~CImLauncher();

    public: // New functions

        /**
        * Loads the client application pluging and start the application
        * @since 2.6
        * @param aParams (Application ID, SAP URL and User ID)
        */
        void StartL( TImpsLaunchParams& aThreadParams );

        static TInt ThreadFunction( TImpsLaunchParams& aThreadParams );

    protected:  // New functions

        /**
        * Custom panic funtion
        * @since 2.6
        * @param aPanicCode
        */
        static void Panic( TLauncherPanic aPanicCode );


        /**
        * TimeOut handler function
        * @since 2.6
        */
        void TimedOut();

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
        CImLauncher();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        CImLauncherPlugin*      iPlugin; // client application ECom plugin
        CImShutDownTimer*       iTimer; // shut down timer
        TInt                    iReturnValue; // retval to the thread starter

        friend class CImShutDownTimer;
    };

#endif      // CIMLAUNCHER_H   

// End of File
