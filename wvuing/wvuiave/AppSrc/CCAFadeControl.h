/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class encapsulates logic that is needed for text fading
*
*/



#ifndef CCAFADECONTROL_H
#define CCAFADECONTROL_H


//  INCLUDES
#include "MCAFadeTextObserver.h"
#include <aknview.h>
#include <aknutils.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MAknsControlContext;
class MCALayoutInfo;
// CLASS DECLARATION

/**
*  This class declares text fader component
*
*  @lib chat.app
*  @since 1.2s
*/
class CCAFadeControl : public CCoeControl,
            public MCoeControlObserver
    {
    public:

        /*
        *	Used to control fading timer
        */
        enum TTimerStates
            {
            ETimerStopped = 0,
            ETimerActivated
            };

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aRect TRect
        * @return instance of class
        */
        static CCAFadeControl* NewL( const TRect& aRect );

        /**
         * Two-phased costructor. Leaves object into stack
         * @param aRect TRect
         * @return instance of class
         */
        static CCAFadeControl* NewLC( const TRect& aRect );


        /**
        * Destructor.
        */
        ~CCAFadeControl();

    public: //New Functions

        /**
         * Starts fading text
         * @since 2.1
         */
        void Start();

        /**
         * Stops fading
         * @since 2.1
         */
        void Stop();

        /**
         * Registers observer
         * @since 2.1
         * @param aObserver Observer instance
         */
        void AddObserver( MCAFadeTextObserver* aObserver );

        /**
         * Sets the fade message
         * @since 2.1
         * @param aMsg Descriptor containing message
         */
        void SetMessage( const TDesC& aMsg );

        /**
         * Returns message
         * @since 2.1
         * @return descriptor that contains message
         */
        TPtrC Message() const;

        /**
         * Tells if fading is active
         * @since 2.1
         * @return ETrue if active else EFalse
         */
        TBool IsActive();

        /**
         * Resets the faded string
         * @since 2.1
         */
        void Reset();

        /**
         * Does the actual changing of text color
         * @since 2.1
         * @param aAnyPtr pointer of this object
         * @return ETrue if this should be called again
         */
        static TInt FadeText( TAny* aAnyPtr );

        /**
         * Starts the timer
         * @param aDelayInternal delay
         * @since 2.1
         */
        void StartTicker( const TTimeIntervalMicroSeconds32 aDelayInternal );

        /**
         * Stops the timer
         * @since 2.1
         */
        void StopTicker();

        /**
         * Reports that this CoeControl has changed
         * @since 2.1
         */
        void ReportChange();

    private: // Functions from base classes

        /**
         * From CCoeControl
         * @see CCoeControl
         */
        void SizeChanged();

        /**
         * From CCoeControl
         * @see CCoeControl
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl
         * @see CCoeControl
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * From CCoeControl
         * @see CCoeControl
         */
        void Draw( const TRect& aRect ) const;

        /**
         * From CCoeControl
         * @see CCoeControl
         */
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );


    private:

        /**
        * C++ default constructor.
        */
        CCAFadeControl();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aRect the rect of this component
        */
        void ConstructL( const TRect& aRect );

        /*
        * Error handling method
        * @param aError Error id
        */
        void HandleError( TInt aError );

        /**
        * Updates clipped text so that it fits to the current
        * the navi pane
        * @return None
        */
        void UpdateClippedMessage();


    private: //data

        /*
        *	Periodic timer is used to drive fade control. Owns
        */
        CPeriodic* iTimer;

        /*
        *	Index to color array.
        */
        TInt iColorIndex;

        /*
        *	Timer interval.
        */
        TInt iTimerTick;

        /*
        *	Font that is used
        */
        const CFont* iFadeFont;
        //View rectangle
        TRect iViewRect;
        //Observer
        MCAFadeTextObserver* iObserver;
        //Observer array
        CArrayPtrSeg<MCAFadeTextObserver>* iObserverArray;
        //Layouts for texts
        RArray<TAknLayoutText> iTextLayouts;

        TAknLayoutRect iNavipaneRect;

        MAknsControlContext* iBgContext;

        /*
        *	Message to fade
        */
        HBufC* iIMessage;

        /*
        *	Owned. Message which will fit the screen.
        */
        HBufC* iClippedMessage;

        /*
        *	State of timer
        */
        TTimerStates iTimerState;

        //does not own
        MCALayoutInfo* iLayoutInfo;
    };

#endif  // CCAFADECONTROL_H
