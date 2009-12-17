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


#ifndef __CCNUISIGNINCONTROL_H__
#define __CCNUISIGNINCONTROL_H__

#include <coecntrl.h>
#include <eiklabel.h>
#include <gulicon.h>
#include <ICLAnimationDataProvider.h>
#include <BasicAnimation.h>
#include <f32file.h>    // RFs

//class CNcdDialogNotifier;
//class MNcdSkinData;
class MCnUiWaitNoteObserver;

class CCnUiSignInControl : public CCoeControl,
            public MCoeControlBackground,
            public MEikCommandObserver
    {
    public:

        // Two-phase Constructors
        static CCnUiSignInControl* NewL( const TDesC& aLabelText, MCnUiWaitNoteObserver* aObserver );

        static CCnUiSignInControl* NewLC( const TDesC& aLabelText, MCnUiWaitNoteObserver* aObserver );

        // destructor
        ~CCnUiSignInControl();

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
        * From MCoeControlBackground, Draw.
        */
        // event handling section
        void Draw( CWindowGc& aGc, const CCoeControl& aControl, const TRect& aRect ) const;

        /**
        * From MEikCommandObserver.
        */
        // event handling section
        void ProcessCommandL( TInt aCommandId );

        void HideStatus();
        void StopAnimation();
        void AssignSignInText( const TDesC& aLabelText );

        void HandleResourceChange( TInt aType );

    private:
        // Fix codescanner warning: Pointer access in destructors
        void MakeCbaInvisible();
    protected:

        // default constructor
        CCnUiSignInControl( MCnUiWaitNoteObserver* aObserver );

        //Constructor method
        void ConstructL( const TDesC& aLabelText );

        TBool LandscapeOrientation() const;


    private: // dat
        ///<The observer, not owned, can be NULL
        MCnUiWaitNoteObserver*      iObserver;

//	CGulIcon* iSplashImage;
        //CCnUiInfoLabel* iLabel;
        CEikLabel* iLabel;
//	CEikLabel* iCancelLabel;
//	MAknsControlContext* iBackground;
        CEikButtonGroupContainer* iPopoutCba;
        CGulIcon* iImage;
        CBasicAnimation* iGifImage;
        RFs iFs;
    };

#endif // __CCNUISIGNINCONTROL_H__
