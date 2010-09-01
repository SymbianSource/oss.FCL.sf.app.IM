/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Active implementation of A/H features.
*
*/



#ifndef CCAAHVARIANT20_H
#define CCAAHVARIANT20_H

//  INCLUDES
#include "MCAAHVariant.h"
#include <AknTabObserver.h>	// A/H support


// FORWARD DECLARATIONS
class MAknTabObserver;
// CLASS DECLARATION

/**
* Active implementation of A/H features.
* See base class MCAAHVariant.h for method comments.
*
*  @lib chat.app
*  @since 1.2s
*/
class CCAAHVariant20 : public CBase,
            public MCAAHVariant,
            public MAknTabObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAAHVariant20* NewL();

        /**
        * Destructor.
        */
        virtual ~CCAAHVariant20();


    public: // Functions from base classes

        /*
        * From MCAAHVariant
        * @since 2.0
        * Check method comments from MCAAHVariant.h
        */
        CAknNavigationDecorator* CreateTabGroupL(
            CAknNavigationControlContainer& aNaviPane,
            CCAAppUi& aAppUi,
            CCAStatusPaneHandler& aStatusPane );

        TBool HandleTabsL( const TKeyEvent& aKeyEvent,
                           TEventCode aType );

        /*
        * From MAknTabObserver, called when tabs change.
        * @since 2.0
        * @param aIndex, tab index.
        */
        void TabChangedL( TInt aIndex );

    private:

        /**
        * C++ default constructor.
        */
        CCAAHVariant20();

    private:    // Data

        // classes needed for responding to tab change
        CAknNavigationControlContainer* iNaviPane;
        CCAAppUi* iAppUi;
        CCAStatusPaneHandler* iStatusPane;
    };

#endif      // CCAAHVARIANT20_H

// End of File
