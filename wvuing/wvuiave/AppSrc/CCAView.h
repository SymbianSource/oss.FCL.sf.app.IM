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
* Description:  Base class for all views
*
*/


#ifndef CCAVIEW_H
#define CCAVIEW_H

//  INCLUDES
#include "CCAAppUi.h"
#include "CCAUISessionManager.h"
#include <aknview.h>


// FORWARD DECLARATIONS
class CCAAppUi;
class MCAViewSwitcher;
class CCAEngine;
class CAknNavigationDecorator;
class CCAFadeControl;
class CAknIndicatorContainer;
class CAknNavigationControlContainer;

// CLASS DECLARATION

/**
*  Base class for chat application's view-classes
*  Capsulates common methods
*
*  @lib chat.app
*  @since 1.2
*/
class CCAView : public CAknView
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CCAView();


        /**
        * Constructs CCAView-object
        * @param aAvkonViewResourceId Id of a resource that is used to constuct
        * this view
        * @param aViewId View id for view to be constructed
        * @return Pointer to CCAAppUi
        */
        void BaseConstructL( TInt aAvkonViewResourceId, TUid aViewId );

        /**
        * Destructor.
        */
        virtual ~CCAView();


    public: // Functions from base classes

        /**
        * From CAknView Returns the view's id
        * @return View-id
        */
        TUid Id() const;

    public: // New functions

        /**
        * Fetches pointer to ChatClient-application's AppUi (CCAAppUi).
        * @return Pointer to CCAAppUi
        */
        CCAAppUi* CAAppUi() const;

        /**
        * Fetches pointer to ChatClient-application's view switcher
        * @return Pointer to view switcher
        */
        MCAViewSwitcher* CAViewSwitcher() const;

        /**
        * Return pointer to application engine
        * @return Pointer to application engine
        */
        //CCAEngine& CAModel() const;

        /**
        * Return pointer to UI Session Manager
        * @return Pointer to UI Session Manager
        */
        CCAUISessionManager& UISessionManager() const;

        /**
         * Returns IMessage indicator
         * @return handle to imessage indicator
         */
        CCAFadeControl* IMessageIndicator() const;

        /**
         * Initialises instant messaging navipane indicator. This is called
         * when view is activated.
         */
        void InitMessageIndicatorL();

        /**
         * Hides fade text
         */
        void HideFadeText();

        /**
         * Destroys instant messaging indicators. This method is called when
         * view is deactivated.
         */
        void DestroyMessageIndicator();

        /**
         * Pushes fade text control to navistack
         */
        void PushFadeTextL();

        /**
         * Shows fade text
         * @param aTextToFade Descriptor that contains message to fade
         */
        void ShowFadeText( const TDesC& aTextToFade );

        /**
         * Shows navipane indicators
         */
        void ShowNavipaneIndicators();

        /**
         * Hides navi pane message info
         */
        void HideNavipaneIndicators();

    protected:  // New functions

        /**
         * Starts fade operation
         */
        void RunFadeText();

        /**
         * Resets the text to fade
         */
        void ResetFadeText();

        /**
         * Launches help application
         */
        void LaunchHelpL();

        /**
         * From CCoeControl
         * @see CCoeControl
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );


        /**
         * Checks if function is supported and if not "not supported" note is
         * displayed
         * @param aSupportedFeature
         * @return ETrue if supported, else EFalse
         */
        TBool IsFunctionSupported(
            CCAUISessionManager::TSupportedFeature aSupporterFeature );

    private: //New functions

        /**
         * Creates new IMessage indicator
         * @return new IMessageIndicator
         */
        CAknNavigationDecorator* CreateIMessageIndicatorL();

    private:     // Data

        // View's UID
        TUid iViewId;

        //Owns.
        CAknNavigationDecorator* iDecoratedFadeText;

        //Doesn't own
        CAknNavigationControlContainer* iNaviPane;
    };

#endif      // CCAVIEW_H

// End of File
