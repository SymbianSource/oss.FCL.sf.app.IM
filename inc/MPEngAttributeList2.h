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
* Description:  Attribute list model interface & related definitions.
*
*/

#ifndef __MPENGATTRIBUTELIST2_H__
#define __MPENGATTRIBUTELIST2_H__

//  INCLUDES
#include <E32Std.h>



//FORWARD DECLARATIONS

class MPEngAttributeListExt2;
class MPEngAdvancedAttributeList2;

// DATA TYPES

/**
 * Attribute list set enumeration.
 *
 * @since
 */
enum TPEngAttributeListsSet2
    {
    /**
     * The set of attribute lists which are on the network server.
     */
    EPEngNetworkAttributeLists = 1,


    /**
     * The set of the attribute lists locally on the terminal.
     */
    EPEngLocalAttributeLists = 2
    };





// CLASS DECLARATION

/**
 * Presence Attribute Lists interface
 *
 * This interface is used by the client to manage one attribute list.
 *
 * After attribute list was created and edited, attribute list must
 * to be attached to the particular contact list or presence contact.
 *
 * @since 3.0
 */
class MPEngAttributeList2
    {
    public: //Mutators

        /**
         * Adds a presence attribute into the attribute list.
         *
         * @since 3.0
         * @param aPresenceAttribute The presence attribute which should
         *        be added into the attribute list. Attribute types known
         *        by Presence Engine are listed in PEngWVPresenceAttributes2.h.
         */
        virtual void AddPresenceAttributeL(
            TUint32 aPresenceAttribute ) = 0;


        /**
         * Removes the presence attribute from the attribute list.
         *
         * @since 3.0
         * @param aPresenceAttribute The presence attribute to remove
         *        from the attribute list. Attribute types known
         *        by Presence Engine are listed in PEngWVPresenceAttributes2.h.
         * @return KErrNone if operation was successful. Else a system wide error code.
         */
        virtual TInt RemovePresenceAttribute(
            TUint32 aPresenceAttribute ) = 0;


        /**
         * Removes all presence attributes from the attribute list
         *
         * @since 3.0
         */
        virtual void RemoveAllAttributes() = 0;



    public: //Accessors

        /**
         * Checks is the presence attribute valid and can it be added
         * into the attribute list.
         *
         * @since 3.0
         * @param aPresenceAttribute The presence attribute to check.
         *        Attribute types known by Presence Engine are listed in
         *        PEngWVPresenceAttributes2.h.
         * @return KErrNone if attribute is valid. Else a system wide error code.
         */
        virtual TInt ConfirmPresenceAttribute(
            TUint32 aPresenceAttribute ) const = 0 ;


        /**
         * Current set of presence attributes in the attribute list.
         *
         * @since 3.0
         * @return The list of the presence attributes
         * in the attribute list. Attribute types known
         * by Presence Engine are listed in PEngWVPresenceAttributes2.h.
         */
        virtual TArray<TUint32> ListOfAttributes() const = 0;




    private: //Extensions interfaces


        /**
         * Generic attribute list extension interface.
         *
         * @since 3.0
         * @return Attribute list extension.
         */
        virtual MPEngAttributeListExt2* Extension() = 0;



    public: //Presence Engine internal interface access

        /**
         * Presence Engine internal attribute list interface.
         *
         * @since 3.0
         * @return Attribute list internal interface.
         */
        virtual const MPEngAdvancedAttributeList2* Advanced() const = 0;
        virtual MPEngAdvancedAttributeList2* Advanced() = 0;



    public: //Cleanup support & destructor

        /**
         * Support for attribute list cleanup.
         *
         * When this method is called, concrete
         * attribute list instance is deleted and all
         * it runtime resources are freed.
         * ==> Attribute lists can be pushed on the
         * CleanupStack using CleanupClosePushL().
         *
         * @since 3.0
         */
        virtual void Close() = 0;


        /**
         * Destructor.
         */
        virtual ~MPEngAttributeList2() {};

    };

#endif //   __MPENGATTRIBUTELIST2_H__

// End of File


