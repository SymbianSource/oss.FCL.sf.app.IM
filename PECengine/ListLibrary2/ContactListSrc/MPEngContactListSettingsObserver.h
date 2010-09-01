/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Abstract interface of the contact list setting observer
*
*/

#ifndef __MPENGCONTACTLISTSETTINGSOBSERVER_H__
#define __MPENGCONTACTLISTSETTINGSOBSERVER_H__


/**
 * Abstract interface of the contact list setting observer
 *
 * It is used to signal that contact list settings were refreshed
 * from storage and references to settings must be restored
 *
 * @since 3.0
 */
class MPEngContactListSettingsObserver
    {
    public:

        /**
         * Notifies observer to refresh contact list
         * settings reference.
         * @since 3.0
         */
        virtual void RefreshSettingsReference() = 0 ;

    protected:

        /**
         * Virtual inline destructor.
         * Protected to prohibite deletion through interface.
         */
        virtual ~MPEngContactListSettingsObserver() {};
    };

#endif //   __MPENGCONTACTLISTSETTINGSOBSERVER_H__

// End of File

