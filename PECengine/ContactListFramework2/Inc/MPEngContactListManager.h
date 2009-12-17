/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contact List Manager abstract interface
*
*/

#ifndef __MPENGCONTACTLISTMANAGER_H__
#define __MPENGCONTACTLISTMANAGER_H__

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>
#include "TPEngContactListBaseSettings.h"

// FORWARD DECLARATIONS
class MPEngContactListAdvance;
class MPEngContactListProperties;

// CLASS DECLARATION
/**
 *  Abstract interface of the contact list manager
 *
 *  It can be used to create, maintain, and delete contact lists
 *
 *  @lib PEngListLib2
 *  @since 3.0
 */
class MPEngContactListManager
    {
    public: // New functions

        /**
         *  Close Reference to the contact list manager
         *
         *  @since 3.0
         */
        virtual void Close() = 0;

        /**
         *  Retrieves contact list model
         *
         *  Contact list is identified by its name defined while
         *  it was created or by the name how it exists on the Network
         *  server if such a contact list is unknown to the PEC engine
         *
         *  Contact list model can be retrieved only if it has been
         *  synchronized withing active session.
         *  If contact list is not synchronized yet, function will
         *  leave with KErrAccessDenied
         *  If contact list does not exist, function will leave with
         *  KErrNotFound
         *
         *  Contact List model is Singleton.
         *  If client want to release model from memory refer:
         *      ReleaseCntListModel();
         *
         *  @since 3.0
         *  @param aContactList name of the contact list to retrieve
         *  @return a contact list model
         */
        virtual MPEngContactListAdvance& ContactListL(
            const TDesC& aContactList ) = 0;

        /**
         *  List of all contact lists. Contains both Presence Engine
         *  native contact lists and unknown server contact lists.
         *
         *  @since 3.0
         *  @return List of all contact lists names. Contains both unknown
         *         server contact lists and Presence Engine local contact
         *         lists.
         */
        virtual const MDesCArray& AllContactListsL() const = 0;

        /**
         *  Active contact list
         *
         *  Contact list which content has been synchronized between
         *  network and local client. Contact list models of such
         *  a contact lists can be retrieved by the client and used.
         *  This group of the contact list consist from the contact lists
         *  of the native contact lists of the PEC engine and unknown
         *  contact lists of the Network server.
         *
         *  @since 3.0
         *  @return a list of active contact lists
         */
        virtual const MDesCArray& SynchronizedContactLists( ) const = 0;

        /**
         *  Contact list settings model
         *
         *  Contact list settings models allows to view details of
         *  the contact list and modify them if needed.
         *  Even though settings of the contact list can be retrieved
         *  even contact list hasn't been synchronized, not all
         *  details are available. Particularly Display  name of the
         *  server unknown contact lists is unavailable.
         *
         *  @since 3.0
         *  @param aContactList name of the contact list
         *  @return contact list settings
         */
        virtual MPEngContactListProperties& ContactListSettingsL(
            const TDesC& aContacList ) = 0;

        /**
         *  Set Default contact list
         *
         *  This function sets default contact list.
         *  There can be only one default contact list in one session.
         *  Therefore to make settings valid, update of the contact list
         *  is required.
         *
         *  @since 3.0
         *  @param aContactList contact list to be set as default contact
         *                  list
         */
        virtual void SetAsDefaultCntListL( const TDesC& aContactList ) = 0;

        /**
         *  Create new contact list.
         *
         *  Creates new contact list.
         *  New contact list is added to the PEC engine. Function returns
         *  reference to the new contact list settings, which should be
         *  used by the client to defined all detailed settings of
         *  new contact list if it is required. If not, default values
         *  for the contact list will be used.
         *  Depends on the nature of the contact list, but after its
         *  synchronization it can be used by the client freely.
         *
         *  @since 3.0
         *  @param aName name of new contact list
         *  @param aMainProperty main property of the new contact list
         *  @return new contact list settings reference
         */
        virtual MPEngContactListProperties& CreateContactListL(
            const TDesC& aContactList,
            TPEngContactListBaseSettings& aBaseSettings ) = 0;

        /**
         *  Delete contact list
         *
         *  Contact list is deleted from the PEC engine, and after
         *  calling of the Update to the contact list, it is also
         *  removed from the network server.
         *  Delete can be called to all contact list, native and unknown.
         *
         *  @since 3.0
         *  @param aContactList name of the contact list to be deleted
         */
        virtual void DeleteContactListL( const TDesC& aContactList ) = 0;

        /**
         *  Change contact list type
         *
         *  Contact list type is updated. Client does not have to retrieve
         *  again contact list model if it was retrieved before. Change
         *  is imidiate.
         *
         *  @since 3.0
         *  @param aContactList name of the contact list to change type
         *  @param aCntListType new type of the contact list.
         */
        virtual void ChangeContactListTypeL(
            const TDesC& aContactList,
            TPEngContactListType aContactListType ) = 0;



        /**
         *  Release contact list model from the memory
         *
         *  Releases contact list model from the client side memory
         *  Contact list can be still used by other clients.
         *  If client wants to use same contact list again, its model
         *  will be again created once client retrieves it by calling
         *  ContactListL() function
         *
         *  @since 3.0
         *  @param aContactList contact list to be released
         *          if contact list does not exists, function is ignored
         */
        virtual void ReleaseCntListModel( const TDesC& aContactList ) = 0;

    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Public destructor to allow deletion trough interface.
         */
        virtual ~MPEngContactListManager() {};

    };
#endif      //  __MPENGCONTACTLISTMANAGER_H__

//  End of File

