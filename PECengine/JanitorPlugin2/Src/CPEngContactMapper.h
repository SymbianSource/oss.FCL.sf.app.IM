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
* Description:  Janitor's plugin main class. Implements most of the Janitor logic.
*
*/



#ifndef __CPENGCONTACTMAPPER_H__
#define __CPENGCONTACTMAPPER_H__

//  INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <cntdbobs.h>
#include <MPEngNWSessionSlotObserver2.h>

#include "MPEngContactMapper.h"


// FORWARD DECLARATIONS
class   CPEngContact;
class   CContactDatabase;
class   CContactChangeNotifier;
class   CPEngListObserver;
class   CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
*  Janitor plugIn main class
*  It does the mapping of the contact id to the wv ids
*
*  @lib PEngJanitor2
*  @since 3.0
*/
NONSHARABLE_CLASS( CPEngContactMapper )
        : public CBase,
        public MPEngContactMapper,
        public MContactDbObserver,
        public MPEngNWSessionSlotObserver2
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngContactMapper* NewL();

    /**
     * Two-phased constructor.
     */
    static CPEngContactMapper* NewLC();

    /**
     * Destructor.
     */
    virtual ~CPEngContactMapper();

public: // new from MPEngContactMapper

    /**
     *  Map wv id to the contact db id
     *
     *  @see MPEngContactMapper
     *  @since 3.0
     */
    CContactIdArray* MapWvIdToContactIdLC( const TDesC& aWVId );


public: // new from MContactDBObserver

    /**
     * Handle Contact Database event
     *
     * @since 3.0
     * @param aEvent contact database event
     * @return
     */
    void HandleDatabaseEventL( TContactDbObserverEvent aEvent );

public: // new from MPEngNWSessionSlotObserver2

    /**
     * NWSessionSlot change handler.
     *
     * @see MPEngNWSessionSlotObserver2
     * @since 3.0
     */
    void HandleNWSessionSlotChangeL(
        CPEngNWSessionSlotNotifier2& aNotifier,
        CPEngNWSessionSlotEvent2& aEvent );


    /**
     * Notification failure handler.
     *
     * @see MPEngNWSessionSlotObserver2
     * @since 3.0
     */
    void HandleNWSessionSlotError(
        TInt aError,
        CPEngNWSessionSlotNotifier2& aNotifier );

private:

    /**
     *  Inserts new contact to the contact db cache
     *
     *  @since 3.0
     *  @param aContactId Contact Database Item Id
     *  @return KErrNone if successful, otherwise system wide error
     */
    void AddContactL( TContactItemId& aContactId );

    /**
     *  Remove contact from contact db cache
     *
     *  @since 3.0
     *  @param aContactId Contact Database Item Id
     *  @return KErrNone if successful, otherwise system wide error
     */
    void RemoveContactL( TContactItemId& aContactId );

    /**
     *  Update content of the contact in  contact db cache
     *
     * @since 3.0
     * @param aContactId Contact Database Item Id
     */
    void UpdateContactL( TContactItemId& aContactId );

    /**
     *  More changes happened in the contact db
     *
     *  @since 3.0
     */
    void HandleMultipleChangesL();

    /**
     *  Notify all list observers about wv Ids mapping change
     *
     *  @since 3.0
     *  @param aWVId wv id to update mapping
     *  @param aContactIds contact ids mapped to the wv id
     */
    void UpdateMappingOnContactListsL(
        const TDesC& aWVId,
        const CContactIdArray* aContactIds );


    /**
      * Rebuild contact db cache
      *
      * @since 3.0
      */
    void RebuildContactDbCacheL();


    /**
     *  Find Contact in contact cache
     *
     *  @since 3.0
     *  @param contact db Id
     *  @return index in array or KErrNotFound
     */
    TInt FindContact( TContactItemId& aContactId ) const;


    /**
     *  Remap given wv Ids
     *
     *  @since 3.0
     *  @param aWVIds wv ids to be remaped
     */
    void RemapWvIdsL( const MDesCArray& aWVIds );


    /**
     *  Find Contact list observer
     *
     *  @since 3.0
     *  @param aSessionId session id of desired list manager
     *  @return index in the array if found or KErrNotFound if not found
     */
    TInt FindListObserver(
        const CPEngNWSessionSlotID2& aSessionId ) const;
private:

    /**
    * C++ default constructor.
    */
    CPEngContactMapper();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private:    // Data


    // OWN: Contact database
    CContactDatabase*                           iCntDatabase;

    // OWN: Contact Database Notifier
    CContactChangeNotifier*                     iCntNotifier;

    /// OWN: Session notifier
    CPEngNWSessionSlotNotifier2*                iSessionNotifier;

    /// OWN:  cached contacts
    ///       Elements OWNed
    RPointerArray<CPEngContact>                 iContacts;

    /// OWN: Contact list observers
    ///       Element OWNed
    RPointerArray<CPEngListObserver>            iListObservers;

    };

#endif      // CPEngContactMapper_H

// End of File
