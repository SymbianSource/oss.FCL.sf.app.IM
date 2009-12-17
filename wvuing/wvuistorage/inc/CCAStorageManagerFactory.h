/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header for contact and group list interface factory
 *
*/



#ifndef CCASTORAGEMANAGERFACTORY_H
#define CCASTORAGEMANAGERFACTORY_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CCAStorageManager;
class MCAStoredContacts;
class MCAStoredGroups;

// CLASS DECLARATION

/**
 *  Factory to create contact and group list interfaces.
 *
 *  @lib CCAStorage.dll
 *  @since 2.1
 */
class CCAStorageManagerFactory : public CBase
    {
    public: // New functions

        /**
         * Caller must NOT delete the returned pointer, it is a Singleton.
         * @return Interface for contact list
         */
        IMPORT_C static MCAStoredContacts* ContactListInterfaceL();

        /**
         * Caller must NOT delete the returned pointer, it is a Singleton.
         * @return Interface for group list
         */
        IMPORT_C static MCAStoredGroups* GroupListInterfaceL();

        /**
         * Releases ContactListInterface and GroupListInterface.
         * Actually those two interfaces are currently implemented
         * in one same singleton. So releasing this singleton release
         * both interfaces.
         */
        IMPORT_C static void ReleaseL();
    };

#endif      // CCASTORAGEMANAGERFACTORY_H

// End of File
