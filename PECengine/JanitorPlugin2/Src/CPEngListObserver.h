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
* Description:  Contact Database observer
*
*/



#ifndef __CPENGLISTOBSERVER_H__
#define __CPENGLISTOBSERVER_H__

//  INCLUDES
#include <e32base.h>
#include <MPEngContactListObserver2.h>


// FORWARD DECLARATIONS

class MPEngContactMapper;
class CPEngNWSessionSlotID2;
class CContactIdArray;
class CPEngContactListStore2;
class CPEngContactListNotifier2;
class MPEngContactList2;
class MPEngContactItem;

// CLASS DECLARATION
/**
 *  Contact List observer
 *  Observes contact list changes changes in given session slot
 *
 *  @lib PEngJanitor2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngListObserver )
        : public CBase,
        public MPEngContactListObserver2
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CPEngListObserver* NewLC(
        MPEngContactMapper& aMapper,
        const CPEngNWSessionSlotID2& aSessionId );

    /**
    * Destructor.
    */
    virtual ~CPEngListObserver();

public: // Function from MPEngContactListObserver2
    /**
     * Contact list change handler.
     *
     * @see <MPEngContactListObserver2.h>
     * @since 3.0
     */
    void HandleContactListChangeL(
        CPEngContactListNotifier2& aNotifier,
        const TDesC& aContactListName );


    /**
     * Notification failure handler.
     *
     * @see <MPEngContactListObserver2.h>
     * @since 3.0
     */
    void HandleContactListError(
        TInt aError,
        CPEngContactListNotifier2& aNotifier );

public: // new public function

    /*
     * Update mapping for given WV Id
     *
     * @since 3.0
     * @param aWVId
     * @param aContactIds
     */
    void UpdateMappingL( const TDesC& aWVId,
                         const CContactIdArray* aContactIds );


    /**
     *  Do full re-mapping of all contact list
     *  Called when contact db done unspecified change
     *
     *  @since 3.0
     */
    void ReMappAllContactsL();


    /**
     *  Session slot Id
     *
     *  @since 3.0
     *  @return session slot id
     */
    const CPEngNWSessionSlotID2& SessionId() const;

private:
    /**
     *  Build list of supported contact lists
     *
     *  @since 3.0
     */
    void RefreshSupportedListsL();

    /**
     *  Find Contact list
     *
     *  @since 3.0
     *  @param aContactList contact list name
     *  @return contact list model
     */
    MPEngContactList2& ContactListL( const TDesC& aContactList );

    /**
     *  Remap whole contact list
     *
     *  @since 3.0
     *  @param aContactList contact list name
     *  @param aForceUpdate force update flag
     */
    void RemapContactListL( MPEngContactList2& aContactList,
                            TBool aForceUpdate = EFalse );


    /**
     *  Remap one Contact Item with given contact db ids
     *
     *  @since 3.0
     *  @param aContactItem contact item to store mapping
     *  @param aContactIds contact db ids array
     */
    void RemapContactItemL( MPEngContactItem& aContactItem,
                            const CContactIdArray* aContactIds );


private:

    /**
    * C++ default constructor.
    */
    CPEngListObserver( MPEngContactMapper& aMapper );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( const CPEngNWSessionSlotID2& aSessionId );


private:    // Data

    /// REF: Janitor observer, not owned
    MPEngContactMapper& 				iContactMapper;

    /// OWN: Session slot id
    CPEngNWSessionSlotID2*				iSessionId;

    /// OWN: Contact list store
    CPEngContactListStore2*				iListStore;

    /// OWN: contact list notifier
    CPEngContactListNotifier2*			iListNotifier;

    /// REF: list of supported contact lists
    RPointerArray<MPEngContactList2>	iContactLists;


    };

#endif      // __CPENGLISTOBSERVER_H__

// End of File
