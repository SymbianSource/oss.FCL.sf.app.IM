/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef CCAListBoxCreator_H
#define CCAListBoxCreator_H

#include <aknlists.h>

//forward declarations

class MCAListBox;
class CCAContactListBox;
/**
*  Class for creating list box
*
*  @lib chat.app
*  @since 2.1
*/
// This is not a C-class although it begins with a CA
class CCAListBoxCreator  // CSI: 51 # see above
    {
    public:

        /**
         * Create just the List box based on type id
         * @param aAppUi For view creation parameters.
         * @since 3.0
         */
        static MCAListBox* CreateListBoxL( TInt aTypeID );
        static CCAContactListBox* CreateSingle2GraphicListBoxL( TInt aTypeID );


    };

#endif      // CCAListBoxCreator_H

// End of File
