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
* Description:  Contact lists settings manager abstract interface
*
*/

#ifndef __MPENGCONTACTLISTSETTINGSMANAGER_H__
#define __MPENGCONTACTLISTSETTINGSMANAGER_H__

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class CPEngContactListSettings;
class MDesCArray;
class MPEngContactListSettingsObserver;
class CPEngContactListModBase;


// CLASS DECLARATION

/**
 * Contact lists settings manager abstract interface.
 *
 * @since 3.0
 */
class MPEngContactListSettingsManager
    {
    public:

        /**
         * Get Contact list settings
         *
         * @since 3.0
         *
         * @param aContactList name of the contact list
         * @return contact list settings or NULL if no settings
         *         for given list.
         */
        virtual CPEngContactListSettings* ContactListSettingsOrNull(
            const TDesC& aContactList ) = 0;


        /**
         * Add contact list settings observer
         *
         * @since 3.0
         *
         * @param aObserver contact list settings observer
         */
        virtual void AddContactListSettingsObserverL(
            const MPEngContactListSettingsObserver* aObserver ) = 0;


        /**
         * Remove contact list settings observer
         *
         * @since 3.0
         *
         * @param aObserver contact list settings observer
         */
        virtual void RemoveContactListSettingsObserver(
            const MPEngContactListSettingsObserver* aObserver ) = 0;


        /**
         * Remove model from the array of loaded models.
         *
         * @since 3.0
         * @param aModel contact list model to be removed
         */
        virtual void RemoveModel( CPEngContactListModBase* aModel ) = 0;


        /**
         * Get domain of the logged user,
         *
         * KNullDesC if domain was not defined by user nor
         * neither provided by the server
         *
         * @since 3.0
         * @return The user domain or empty descriptor.
         */
        virtual const TDesC& UserDomain() const = 0;


        /**
         * Store contact list settings
         *
         * @since 3.0
         */
        virtual void StoreSettingsL() = 0;


        /**
         * Gets reference to the store entry size
         * Size can be modified through this reference
         * @since 3.0
         * @return store size reference
         */
        virtual TInt& StoreSize( ) = 0;


    protected:
        /**
         * Virtual inline destructor.
         * Protected to prohibite deletion through interface.
         */
        virtual ~MPEngContactListSettingsManager() {};
    };

#endif //   __MPENGCONTACTLISTSETTINGSMANAGER_H__

// End of File

