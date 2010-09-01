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
* Description:  Interface class for Arabic/Hebrew variant.
*
*/



#ifndef MCAAHVARIANT_H
#define MCAAHVARIANT_H

//  INCLUDES
#include <e32base.h>
#include <w32std.h>

// FORWARD DECLARATIONS
class CAknNavigationControlContainer;
class CCAAppUi;
class CCAStatusPaneHandler;
class CAknNavigationDecorator;


// CLASS DECLARATION

/**
*	Interface class for A/H variant.
*  @lib chat.app
*  @since 2.0
*/
class MCAAHVariant
    {
    public: // New functions

        /**
        * Creates tabgroup.
        * @since 1.2s
        * @return, CAknNavigationDecorator*, created object.
        */
        virtual CAknNavigationDecorator* CreateTabGroupL(
            CAknNavigationControlContainer& aNaviPane,
            CCAAppUi& aAppUi,
            CCAStatusPaneHandler& aStatusPane ) = 0;
        /**
        * Handles changing of tabs.
        * @return ETrue if A/H tab handling was used, otherwise EFalse.
        * @since 1.2s
        */
        virtual TBool HandleTabsL( const TKeyEvent& aKeyEvent,
                                   TEventCode aType ) = 0;
    };

#endif      // MCAAHVARIANT_H

// End of File
