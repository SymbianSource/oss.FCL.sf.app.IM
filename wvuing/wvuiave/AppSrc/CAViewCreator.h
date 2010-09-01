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
* Description:  Creator class for views.
*
*/


#ifndef CAVIEWCREATOR_H
#define CAVIEWCREATOR_H

// FORWARD DECLARATIONS
class CCAAppUi;

// CLASS DECLARATION

/**
*  Class for creating views for application UI.
*
*  @lib chatng.app
*/
// This is not a C-class although it begins with a CA
class CAViewCreator // CSI: 51 # see above
    {
    public:

        /**
         * Create just the main view
         * @param aAppUi For view creation parameters.
         */
        static void CreateMainViewL( CCAAppUi& aAppUi );

        /**
         * Create the rest of the views
         * @param aAppUi For view creation parameters.
         */
        static void CreateOtherViewsL( CCAAppUi& aAppUi );

    };

#endif      // CAVIEWCREATOR_H

// End of File
