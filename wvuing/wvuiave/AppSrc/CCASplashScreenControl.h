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


#ifndef __CCASPLASHSCREENCONTROL_H__
#define __CCASPLASHSCREENCONTROL_H__

#include <coecntrl.h>

//class CNcdDialogNotifier;
//class MNcdSkinData;

class CCASplashScreenControl : public CCoeControl,
            public MCoeControlObserver
    {
    public:

        // Two-phase Constructors
        static CCASplashScreenControl* NewL( CCASplashScreen &aParent );

        static CCASplashScreenControl* NewLC( CCASplashScreen &aParent );

        // destructor
        ~CCASplashScreenControl();

    public: // Functions from base classes

        /**
         * From CoeControl,SizeChanged.
         */
        void SizeChanged();

        /**
         * From CoeControl,CountComponentControls.
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl,ComponentControl.
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * From CCoeControl,Draw.
         */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl,OfferKeyEventL.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From MCoeControlObserver, HandleControlEventL.
        */
        // event handling section
        // e.g Listbox events
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

        /**
        * ShowSplashScreen. Views the splash screen for a certain time.
        */
        void ShowSplashScreen( CGulIcon *aSplashImage,
                               /*MNcdSkinData* aSkinData,*/
                               TBool aDismissable );

        void HandleResourceChange( TInt aType );

    protected:

        // default constructor
        CCASplashScreenControl( CCASplashScreen &aParent );

        //Constructor method
        void ConstructL();

        TBool LandscapeOrientation() const;


    private: // dat

        CCASplashScreen &iParent;

        CGulIcon* iSplashImage;
//    MNcdSkinData* iSkinData;

        TBool iDismissable;

        TBool iSplashDismissed;

    };

#endif // __CCASPLASHSCREENCONTROL_H__
