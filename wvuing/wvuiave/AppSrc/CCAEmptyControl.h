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
* Description:  Container for all controls in main view
*
*/



#ifndef CCAEMPTYCONTROL_H
#define CCAEMPTYCONTROL_H

// INCLUDES
#include "MCALayoutChangeObserver.h"
#include <aknview.h>
#include <eiklbo.h>
#include <bldvariant.hrh>

// FORWARD DECLARATIONS
class CAknSingleLargeStyleListBox;
class MCAViewSwitcher;
class CCAAppUi;
class CCAIconLoader;

// CLASS DECLARATION

/**
*  Container for all controls in main view
*
*  @lib chatng.app
*/
class CCAEmptyControl : public CCoeControl
    {
    public: // Constructors and destructor

        /**
         * Constructor
         * @since 2.1
         * @param aIconLoader Icon loader
         */
        CCAEmptyControl();

        /**
         * Symbian two phased constructor
         * @since 2.1
         * @param aRect Frame rectangle for container.
         * @param aViewSwitcher Reference to view switcher
         * @param aIconLoader Icon loader for load icons.
         */
        static CCAEmptyControl* NewL( const TRect& aRect );
        /**
        * Symbian OS default constructor.
        * @since 1.2
        * @param aRect Frame rectangle for container.
        * @param aViewSwitcher Reference to view switcher
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        virtual ~CCAEmptyControl();
    };

#endif // CCAEMPTYCONTROL_H

// End of File
