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
* Description: Presence Publish Level Policy which offers publish level changing and initializing
*
*/

#ifndef __IMPSPUBLISHLEVELPOLICY_H
#define __IMPSPUBLISHLEVELPOLICY_H

//  INCLUDES
#include <E32Std.h>
#include <PECAppSharedData.h>


//FORWARD DECLARATION
class CPEngAttributeListStore2;
class MPEngAttributeList2;
class CPEngNWSessionSlotID2;

// TYPE DEFINITIONS
/**
 * Attribute list mutator function type definition.
 **/
typedef void ( *TSetPresenceL )( MPEngAttributeList2& );



// CLASS DECLARATION

/**
* Presence Publish Level Policy. Offers
* publish level changing and initializing.
*
*  @lib IMPSuilib.dll
*  @since 2.1
*/
NONSHARABLE_CLASS( IMPSPublishLevelPolicy )

    {
public: // New functions

    /**
     * Initialize settings of Publish level, based on the settings
     * from the settings stored in the shared data.
     * Function is synchronous and creates only attribute lists for
     * contact lists based on the publish level
     *
     * @param aNWSessionSlotID the network session slot ID
     * @since 2.1
     */
    static void InitializePublishLevelL( CPEngNWSessionSlotID2& aNWSessionSlotID );


    // New set functions

    /**
     *  Update settings of the attribute lists, so it reflects publishinglevel passed
     *  as parameter, no network asynchronous operations are performed, all changes
     *  are done only on local machine
     *
     * @since 2.1
     * @param aPublishLevel publish level to
     * @return
     */
    static void UpdateAttributeListsL( TInt aPublishLevel,
                                       CPEngNWSessionSlotID2& aNWSessionSlotID );

private:
    /**
     *  Set Attribute lists so it reflects private and Public publish level
     *  as parameter, no network asynchronous operations are performed, all changes
     *  are done only on local machine
     *
     * @since 2.1
     * @param   aAttributeListFactory attribute lists factory instance, function does not take
     *          ownership
     * @return
     */
    static void SetPrivatePublicPublishingL( CPEngAttributeListStore2& aAttributeListFactory );


    /**
     *  Set Attribute lists so it reflects only private publishing
     *  as parameter, no network asynchronous operations are performed, all changes
     *  are done only on local machine
     *
     * @since 2.1
     * @param   aAttributeListFactory attribute lists factory instance, function does not take
     *          ownership
     * @return
     */
    static void SetOnlyPrivatePublishingL( CPEngAttributeListStore2& aAttributeListFactory );


    /**
     *  Set Attribute lists so it reflects only Public publishing
     *  as parameter, no network asynchronous operations are performed, all changes
     *  are done only on local machine
     *
     * @since 2.1
     * @param   aAttributeListFactory attribute lists factory instance, function does not take
     *          ownership
     * @return
     */
    static void SetOnlyPublicPublishingL( CPEngAttributeListStore2& aAttributeListFactory );


    /**
     *  Set Attribute lists so it reflects all publishing OFF
     *  as parameter, no network asynchronous operations are performed, all changes
     *  are done only on local machine
     *
     * @since 2.1
     * @param   aAttributeListFactory attribute lists factory instance, function does not take
     *          ownership
     * @return
     */
    static void SetPublishOffL( CPEngAttributeListStore2& aAttributeListFactory );


    /**
     *  Set Attribute lists so it reflects all publishing ON
     *  as parameter, no network asynchronous operations are performed, all changes
     *  are done only on local machine
     *
     * @since 2.1
     * @return
     */
    static void SetPublishAllOnL();

    /**
     * Attribute list mutator.
     *
     * Updates given PEC contact lists attribute list
     * to given publish level. (Publish level is declared
     * trough the given function pointer.)
     *
     * @since 2.1
     * @param   aAttrLstStore Attribute list store to use to mutate attribute list.
     * @param   aContactList The contact list which attribute list to change.
     * @return  aPresenceCall The attribute list mutator function to use to
     * alter the attribute list.
     */
    static void UpdateAttributeListL(	CPEngAttributeListStore2& aAttrLstStore,
                                      const TDesC& aContactList,
                                      TSetPresenceL aPresenceCall );

    /**
     * Attribute list mutator.
     *
     * Updates default attribute list to given publish level.
     * (Publish level is declared trough the given function pointer.)
     *
     * @since 2.1
     * @param   aAttrLstStore Attribute list store to use to mutate attribute list.
     * @return  aPresenceCall The attribute list mutator function to use to
     * alter the attribute list.
     */
    static void UpdateDefaultAttrListL( CPEngAttributeListStore2& aAttrLstStore,
                                        TSetPresenceL aPresenceCall );


    /**
     * Attribute list mutator.
     * Sets PEC "private presence" attributes on the
     * given attribute list.
     *
     * @since 2.1
     * @param aAttrListModel Attribute list to alter.
     */
    static void SetPrivatePresenceL( MPEngAttributeList2& aAttrListModel );


    /**
     * Attribute list mutator.
     * Sets PEC "public presence" attributes on the
     * given attribute list.
     *
     * @since 2.1
     * @param aAttrListModel Attribute list to alter.
     */
    static void SetPublicPresenceL( MPEngAttributeList2& aAttrListModel );


    /**
     * Attribute list mutator.
     * Sets PEC "no presence" attributes on the
     * given attribute list.
     *
     * @since 2.1
     * @param aAttrListModel Attribute list to alter.
     */
    static void SetNoPresence( MPEngAttributeList2& aAttrListModel );



protected: //prohibited constructor / destructor

    ~IMPSPublishLevelPolicy();
    IMPSPublishLevelPolicy();

    };

#endif      // __IMPSPUBLISHLEVELPOLICY_H

// End of File
