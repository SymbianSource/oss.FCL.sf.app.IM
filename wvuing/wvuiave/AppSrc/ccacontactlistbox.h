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
* Description:  Custom listbox component for the contact list
*
*/


#ifndef CCACONTACTLISTBOX_H
#define CCACONTACTLISTBOX_H

#include <aknjavalists.h>

// FORWARD DECLARATIONS

class MCAMainViewArrayPC;

/**
 *  Custom listbox component for the contact list
 *
 *  Allows to change the model for the listbox.
 *  This is done because we have to customize the behaviour of the find pane,
 *  so that filtered list shows also the contact lists where the searched
 *  contacts exist.
 *
 *  @lib chat.exe
 *  @since S60 v3.0
 */
class CCAContactListBox : public CAknSingle2GraphicStyleListBox
    {

    public:

        /**
        * Creates the model
        * @see CAknColumnListBox
        */
        void CreateModelL();

    public: // New functions

        /**
        * Give listbox a direct access to contact list model.
        * @param aContactListModel Model of this listbox
        */
        void SetContactListModel( MCAMainViewArrayPC& aMainViewArrayPC );

    };


#endif // CCACONTACTLISTBOX_H
