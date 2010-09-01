/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contact list base settings.
*
*/

#ifndef __TPENGCONTACTLISTBASESETTINGS_H
#define __TPENGCONTACTLISTBASESETTINGS_H

//  INCLUDES
#include <E32Def.h>


//  DATA TYPES

enum TPEngContactListType
    {
    /**
     * Contact List is stored in memory, its content is removed
     * when PEC engine is logged of. Contact list cannot have any
     * permanent Data, which should be persistent over to the
     * session.
     *
     * This Type of the contact list is intented mainly for
     * contact lists, which master copy is held by the network server.
     *
     * For such list, MPEngContactItem permanent extension values
     * cannot be used, and cached extension values should be used
     * instead. Scope of cached extension values ends at the end
     * of the presence session.
     */
    EPEngCachedContactList = 0,


    /**
     * Contact list is permanently stored on the device disc.
     * Its content is persistent over the session, however set of contact
     * IDs of the contact list, can be updated after it synchronization
     * based on synchronization rules.
     *
     * This type of the contact list is intented mainly for contact lists
     * which master copy is held by the device. However they can be used
     * also for contact list, which master copy is held by the network
     * server if there is a need to store some permanent extension data
     * along the list and its contacts.
     *
     * With this type of the contact list, permanent MPEngContactItem extension
     * values can be used. Permanent extension values are persistent over
     * the sessions. When certain contact ID is removed from the list
     * during the contact list synchronization, contact permanent extension
     * values are lost.
     */
    EPEngPermanentlyStoredContactList = 1
    };




//  CLASS DEFINITION

/**
 * Contact list base settings.
 * Base settings to be used in contact list creation.
 *
 * @since 3.0
 */
class TPEngContactListBaseSettings
    {
    public:  // Constructor

        /**
         *  Default constructor
         */
        inline TPEngContactListBaseSettings();

        inline TPEngContactListBaseSettings(
            const TPEngContactListBaseSettings& aSettings );
    public: //Data

        //OWN: Contact list type
        TPEngContactListType   iContactListType;



        //OWN: Contact list name auto update needed
        // - ETrue if contact list name should be updated when
        //   user name and the domain are known
        // - EFalse if no list name auto update needed
        TBool					iContactListNameAutoUpdate;


    public: //Reserved for future

        //OWN: Reserved
        TUint8  iReserved[ 20 ];

    };





//=========== TPEngContactListBaseSettings MEMBER FUNCTIONS ===========

// C++ default constructor can NOT contain any code, that
// might leave.
//
inline TPEngContactListBaseSettings::TPEngContactListBaseSettings()
    {
    Mem::FillZ( this, sizeof( TPEngContactListBaseSettings ) );
    }

inline TPEngContactListBaseSettings::TPEngContactListBaseSettings(
    const TPEngContactListBaseSettings& aSettings )
        : iContactListType( aSettings.iContactListType ),
        iContactListNameAutoUpdate( aSettings.iContactListNameAutoUpdate )
    {
    }

#endif //  __TPENGCONTACTLISTBASESETTINGS_H


//End of file


