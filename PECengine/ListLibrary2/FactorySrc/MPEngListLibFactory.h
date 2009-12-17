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
* Description:  List Library factory abstract interface
*
*/

#ifndef __MPENGLISTLIBFACTORY_H__
#define __MPENGLISTLIBFACTORY_H__

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class   CPEngAttributeListManager;
class   CPEngAttributeListTransactionManager;
class   CPEngContactListManager;
class   CPEngContactListTransactionManager;
class   CPEngAuthorizationManager;
class   CPEngAuthorizationTransFactory;

// CLASS DECLARATION


/**
 * List Lib factory abstract interface
 *
 * @since 3.0
 */
class MPEngListLibFactory
    {
    public: // New functions

        /**
         * Methods for removing different manager objects
         * from list of existing ones.
         *
         * @since 3.0
         * @param aManager Manager object to remove.
         */
        virtual void RemoveAttrListManager(
            CPEngAttributeListManager* aManager ) = 0;

        virtual void RemoveAttrListTransManager(
            CPEngAttributeListTransactionManager* aManager ) = 0;


        virtual void RemoveCntListManager(
            CPEngContactListManager* aManager ) = 0;


        virtual void RemoveCntListTransManager(
            CPEngContactListTransactionManager* aManager ) = 0;


        virtual void RemoveAuthorizationManager(
            CPEngAuthorizationManager* aManager ) = 0;


        virtual void RemoveAuthorizationTransManager(
            CPEngAuthorizationTransFactory* aManager ) = 0;


    protected:

        /**
         * Virtual inline destructor.
         * Protected to prohibite deletion through interface.
         */
        virtual ~MPEngListLibFactory() {};
    };

#endif      // __MPENGLISTLIBFACTORY_H__

// End of File
