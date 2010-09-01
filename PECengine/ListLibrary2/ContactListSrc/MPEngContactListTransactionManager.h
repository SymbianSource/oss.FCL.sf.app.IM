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
* Description:  Contact lists transaction manager abstract interface
*
*/

#ifndef __MPENGCONTACTLISTTRANSACTIONMANAGER_H__
#define __MPENGCONTACTLISTTRANSACTIONMANAGER_H__


// INCLUDES
#include <e32base.h>
#include <badesca.h>


// FORWARD DECLARATIONS
class MPEngOutgoingTransactionHandler;
class MDesCArray;
class CPEngContactListSettings;


// CLASS DECLARATION
/**
 * Contact lists transaction manager abstract interface
 *
 * @lib PEngListLib2
 * @since 3.0
 */
class MPEngContactListTransactionManager
    {
    public:

        /**
         * Handles completion of contact list
         * environment synhronization.
         *
         * Non-existing contact lists are removed and new contact
         * lists are added to the contact list manager
         * Sets contact list manager as synchronized.
         *
         * @since 3.0
         * @param aExistingLists List of lists existing on the server.
         * @param aDefaultList Id of the default list.
         */
        virtual void CompleteEnviromentSyncL( CDesCArray& aExistingLists,
                                              const TDesC& aDefaultList,
                                              TBool aListsUnsubscribed ) = 0;

        /**
         * User domain for the active session.
         * KNUllDesC if domain isn't available.
         *
         * @since 3.0
         * @return User domain
         */
        virtual const TDesC& UserDomain() = 0;


        /**
         * Gets settinsg for default contact list.
         * If there is not default contact list, NULL pointer is returned
         *
         * @since 3.0
         * @return default contact list settings or NULL.
         */
        virtual CPEngContactListSettings* DefaultCntListSettingsOrNull() = 0;


        /**
         * Complete contact list deletion
         *
         * This function is called when contact list is deleted from
         * from network server
         *
         * @since 3.0
         * @param aContactList Id of the deleted contact list
         */
        virtual void CompleteCntListDeletionL( const TDesC& aContactList ) = 0;


    protected:

        /**
         * Virtual inline destructor.
         * Protected to prohibite deletion through interface.
         */
        virtual ~MPEngContactListTransactionManager() {};

    };

#endif //   __MPENGCONTACTLISTTRANSACTIONMANAGER_H__

// End of File

