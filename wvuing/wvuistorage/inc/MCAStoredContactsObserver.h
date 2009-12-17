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
* Description:  Contact data container observer interface
*
*/



#ifndef MCASTOREDCONTACTSOBSERVER_H
#define MCASTOREDCONTACTSOBSERVER_H

#include <e32std.h>
#include "TStorageManagerGlobals.h"

class MCAStoredContact;
class MCAContactList;

// CLASS DECLARATION

/**
 *  Interface for contact data container observer
 *
 *  @lib CAStorage.dll
 *  @since 3.0
 */
class MCAStoredContactsObserver
    {
    public: // New functions

        /**
         * Called when a contact is deleted.
         * @param aId The ID of the deleted contact
         * @param aFromPEC, ETrue, if event is coming from PEC
         */
        virtual void HandleContactDelete( const TDesC& aContactId ) = 0;

        /**
         * Called when a contact is added.
         * @param aList. List for contact that was added.
         * @param aContact The contact which was added
         * @param aFromPEC, ETrue, if event is coming from PEC
         */
        virtual void HandleAddition( MCAContactList& aList, MCAStoredContact& aContact ) = 0;

        /**
         * Called when a contact or list is about to change, or has been changed.
         * @param aList List that is changed. Can be NULL if contact is changed.
         * @param aContact Contact that is changed. Can be NULL if list is changed
         * @param aEventType The type of event: EStorageEventPreChange before
         * the change (contact still has the old information), or
         * EStorageEventPostChange after the change took place
         * (contact has new information)
         */
        virtual void HandleChange( MCAContactList* aList, MCAStoredContact* aContact,
                                   TStorageManagerGlobals::TCAObserverEventType aEventType,
                                   TBool aUserIdChanged ) = 0;

        /**
         * Virtual destructor
         */
        virtual ~MCAStoredContactsObserver() {};
    };

#endif      // MCASTOREDCONTACTSOBSERVER_H

// End of File
