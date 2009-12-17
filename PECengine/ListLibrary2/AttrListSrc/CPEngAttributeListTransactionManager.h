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
* Description:  Attribute list transaction manager
*
*/

#ifndef __CPENGATTRIBUTELISTTRANSACTIONMANAGER_H__
#define __CPENGATTRIBUTELISTTRANSACTIONMANAGER_H__


//  INCLUDES

#include "CPEngAttributeListManager.h"
#include "MPEngTransactionFactory.h"
#include "MPEngAttributeListTransactionManager.h"
#include "PEngWVCspVersion.h"
#include <e32base.h>

// DATA TYPES
typedef void ( CPEngAttributeListItem::*CommitCall )( const TDesC& );
typedef TBool ( CPEngAttributeListItem::*ItemDefaultSet )( void ) const;



// FORWARD DECLARATIONS
class CPEngContactListTransactionManager;
class MPEngListLibFactory;


// CLASS DECLARATION
/**
 * Attribute list transaction manager
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttributeListTransactionManager ):
        public CPEngAttributeListManager,
        public MPEngTransactionFactory,
        public MPEngAttributeListTransactionManager
    {
public:
    /**
     * Two-phased constructor.
     */
    static CPEngAttributeListTransactionManager* NewL(
        MPEngListLibFactory& aFactory,
        const CPEngSessionSlotId& aSessionSlot,
        TPEngWVCspVersion& aCSPVersion );

    /**
     * Two-phased constructor.
     */
    static CPEngAttributeListTransactionManager* NewLC(
        MPEngListLibFactory& aFactory,
        const CPEngSessionSlotId& aSessionSlot,
        TPEngWVCspVersion& aCSPVersion );

    /**
     * Destructor.
     */
    ~CPEngAttributeListTransactionManager();



public: // Functions from MPEngTransactionFactory

    /**
     * Clean up stack support for the factory
     * @see <MPEngTransactionFactory.h>
     */
    void Close();


    /**
     * Factory method to create handler for incoming data
     * @see <MPEngTransactionFactory.h>
     */
    MPEngIncomingTransactionHandler* IncomingTransactionHandlerL(
        const TDesC8& aIncomingRequest );

    /**
     * Factory method to create outgoing handler(s)
     * @see <MPEngTransactionFactory.h>
     */
    void OutgoingTransactionHandlerL(
        TInt aOperation,
        const TDesC& aData,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers );



public: // From MPEngAttributeListTransactionManager

    /**
     * Commit Attribute list creation
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void AttributeListCreated( const RArray<TUint32>& aAttrList );


    /**
     * Attribute list synchronized with the server
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void AttributeEngineSynchronizedL();


    /**
     * Commit successful creation of the attribute list for contact list
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void CommitContactListL( const TDesC& aContactList );


    /**
     * Commit successful creation of the attribute list for contact ID
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void CommitContactIdL( const TDesC& aContactId );


    /**
     * Rollback unsuccessful creation of the attribute list
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void RollBackContactListL( const TDesC& aContactList );


    /**
     * Rollback unsuccessful creation of the attribute list
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void RollBackContactIdL( const TDesC& aContactId );


    /**
     * Commit creation of the attribute list
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void CommitDefaultL();


    /**
     * Roll back creating of the default attribute list
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void RollBackDefaultL();


    /**
     * Commit contacts, creating of the attribute list was successful
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void CommitDefinedContactsL( const CDesCArray* aContactLists,
                                 const CDesCArray* aContactIds );

    /**
     * RollBack contacts
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void RollBackDefinedContactsL( const CDesCArray* aContactLists,
                                   const CDesCArray* aContactIds );


    /**
     * Store changes of the attribute list to the store
     * @see <MPEngAttributeListTransactionManager.h>
     */
    void StoreChangesToStoreL();





private:    // new function of the base class

    /**
     * Create synchronization Handlers
     *
     * @since 3.0
     * @param aTransactionHandlers array to append handlers
     */
    void GetSyncHandlersL(
        RPointerArray<MPEngOutgoingTransactionHandler>& TransactionHandlers,
        TInt aOperationId );

    /**
     * Get attribute lists update handler
     *
     * @since 3.0
     * @param aTransactionHandlers array to append handlers
     * @param aOperationId operation Id
     */
    void GetAttributeListsUpdateHandlersL(
        RPointerArray<MPEngOutgoingTransactionHandler>& TransactionHandlers,
        TInt aOperationId );

    /**
     * Get attribute lists update handlers for defined contact lists.
     *
     * @since 3.0
     * @param aData packed Storage Ids of the contact lists
     * @param aTransactionHandlers array to append handlers
     * @param aOperationId operation Id
     */
    void GetLazyContLstInitializeAttrLstHandlersL(
        const TDesC& aData,
        RPointerArray<MPEngOutgoingTransactionHandler>& TransactionHandlers,
        TInt aOperationId );

    /**
     * Get attribute lists update handlers for to be deleted attr lists
     *
     * @since 3.0
     * @param aTransactionHandlers array to append handlers
     * @param aOperationId operation Id
     */
    void GetDeleteAttributeListsHandlersL(
        RPointerArray<MPEngOutgoingTransactionHandler>& aTransactionHandlers,
        TInt aOperationId );


    /**
     * Compress array of attribute lists
     *
     * @since 3.0
     */
    void CompressArrListArray();

    /**
     * Append descriptors from the original array to the new array
     *
     * @since 3.0
     * @param aOriginalDesArray original descriptor array
     * @param aNewArray new descriptor array
     */
    void AppendDesArrayContentL(
        const CDesC16Array& aOriginalDesArray,
        CDesC16Array& aNewDesArray );


    /**
     * Check if attribute list is valid on the server
     *
     * @since 3.0
     * @param aAttributeList attribute list
     * @return ETrue if attribute list is valid
     */
    TBool CheckAttributeListServerValidityCurrent(
        const CPEngAttributeListItem& aAttributeList );

    /**
     * Check if attribute list needs to be created on the network server.
     *
     * @since 3.0
     * @param aAttributeList attribute list
     * @return ETrue if attribute list is valid
     */
    TBool CheckAttributeListCreateNewValidity(
        const CPEngAttributeListItem& aAttributeList );

    /**
     * Check if attribute list needs to be deleted from the server
     *
     * @since 3.0
     * @param
     * @return ETrue if attribute list is valid
     */
    TBool CheckAttributeListDeleteValidity();

    /**
     * Commit whole content of the attribute list
     *
     * @since 3.0
     * @param aAttributeList attribute list
     */
    void CommitAttributeListL(
        const CPEngAttributeListItem& aAttributeList );

    /**
     * Commit all attribute lists
     *
     * @since 3.0
     */
    void CommitAllAttributeListL();

    /**
     * Commit attribute lists for deletion
     *
     * @since 3.0
     */
    void CommitDeleteAttributeListL();

    /**
     * Commit successful creation of the attribute list for contact
     *
     * @since 3.0
     * @param aContact Contact to commit
     * @param aCommitCall call back function to use for commit
     * @param aDeleteArray delete array to be used for commiting
     */
    void CommitContactL( const TDesC& aContact,
                         CommitCall aCommitCall,
                         CDesCArray& aDeleteArray );

    /**
      * Rollback not successful creation of the attribute list for contact
      *
      * @since 3.0
      * @param aContact Contact to rollback
      * @param aNewCommitArray array of the attr list item to use to rollback
      * @param aDeleteArray delete array to use to rollback
      */
    void RollBackOneContactL( const TDesC& aContact,
                              CPEngAttributeListItem::TPEngAttrListItemArrays aNewCommitArray,
                              CDesCArray& aDeleteArray );
    /**
     * Check if attribute list is validity
     *
     * @since 3.0
     * @param aAttributeList attribute list
     * @param aIDsArray id array to use
     * @param aCntListArray cnt list array to use
     * @param aDefaultSetCall call function pointer
     * @param aSynchronizeFlagIncluded synchronization flag included
     * @return ETrue if attribute list is valid
     */
    TBool CheckAttributeListValidity(
        const CPEngAttributeListItem& aAttributeList,
        CPEngAttributeListItem::TPEngAttrListItemArrays aIDsArray,
        CPEngAttributeListItem::TPEngAttrListItemArrays aCntListArray,
        ItemDefaultSet aDefaultSetCall,
        TBool aSynchronizeFlagIncluded );

private: // constructors

    /**
     * By default Symbian constructor is private.
     */
    void ConstructL( const CPEngSessionSlotId& aSessionSlot );

    /**
     * C++ default constructor.
     */
    CPEngAttributeListTransactionManager( MPEngListLibFactory& aFactory,
                                          TPEngWVCspVersion& aCSPVersion );

private:    // Data

    // REF: Contact list transaction manager
    CPEngContactListTransactionManager*     iContactListTransManager;

    /// REF: CSP Version
    TPEngWVCspVersion&                      iCSPVersion;

    /// OWN: Count of created sync handlers
    TInt									iSyncHandlerCount;

    };

#endif  //   __CPENGATTRIBUTELISTTRANSACTIONMANAGER_H__

// End of File

