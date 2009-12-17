/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Instance of this class catches key inputs when it is activated
*
*/



#ifndef CCAINPUTABSORBER_H
#define CCAINPUTABSORBER_H

//  INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CEikAppUi;

// CLASS DECLARATION

/**
*  Instance of this class catches key events when instance is activated
*  Activation is handled by pushing control to view stack
*
*  @lib chat.app
*  @since 1.2
*/
class CCAInputAbsorber : public CCoeControl
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAInputAbsorber* NewL();

        /**
        * Destructor.
        */
        virtual ~CCAInputAbsorber();

    public: // New functions

        /**
        * Starts capturing events
        */
        void CaptureEventsL();

        /**
        * Stops capturing events
        */
        void Release();

        /**
        * Are events captured now
        * @return Are events captured now
        */
        TBool IsCaptured() const;

    public: // Functions from base classes

        /**
        * @see CCoeControl
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
         * @see CCoeControl
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    private:

        /**
        * C++ default constructor.
        */
        CCAInputAbsorber();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: //data
        // Doesn't own. Handle to EikAppUi
        CEikAppUi* iAppUi;
        // Event capturing status
        TBool iIsCaptured;

    };

#endif      // CCAINPUTABSORBER_H   

// End of File
