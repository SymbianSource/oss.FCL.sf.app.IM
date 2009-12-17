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
* Description:  Contact list transaction manager
*
*/

#ifndef __CPENGCONTACTLISTTRANSACTIONMANAGER_H__
#define __CPENGCONTACTLISTTRANSACTIONMANAGER_H__


//  INCLUDES
#include "MPEngTransactionFactory.h"
#include "CPEngContactListManagerBase.h"
#include "MPEngContactListTransactionManager.h"
#include "PEngWVCspVersion.h"
#include <e32base.h>


// CLASS DECLARATION
/**
 * Contact List transaction manager
 * This class is used to get transaction handler
 * to update content of the contact list with WV server
 *
 * @lib PEngListLib2
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListTransactionManager ) :
        public CPEngContactListManagerBase,
        public MPEngTransactionFactory,
        public MPEngContactListTransactionManager
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngContactListTransactionManager* NewLC(
        MPEngListLibFactory& aFactory,
        const CPEngSessionSlotId& aSessionSlot,
        TPEngWVCspVersion& aCSPVersion  );

    /**
     * Destructor.
     */
    virtual ~CPEngContactListTransactionManager();


public:  // From MPEngTransactionFactory

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



public: // From MPEngContactListTransactionManager

    /**
     * Complete enviroment sync
     * @see <MPEngContactListTransactionManager.h>
     */
    void CompleteEnviromentSyncL( CDesCArray& aExistingLists,
                                  const TDesC& aDefaultList,
                                  TBool aListsUnsubscribed );


    /**
     * Domain of the active session if it available.
     * @see <MPEngContactListTransactionManager.h>
     */
    const TDesC& UserDomain();


    /**
     * Default contact list settings or NULL if no
     * default list defined.
     * @see <MPEngContactListTransactionManager.h>
     */
    CPEngContactListSettings* DefaultCntListSettingsOrNull();


    /**
     * Complete contact list deletion.
     * @see <MPEngContactListSettingsManager.h>
     */
    void CompleteCntListDeletionL( const TDesC& aContactList );



private:  // new functions


    /**
     * Get outgoing transaction handlers
     * @since 3.0
     * @param aData packed store Ids
     * @param aTransactionHandlers array to store those new handlers
     * @param aOperationId The operation Id
     */
    void GetOutGoingTransactionHandlersL(
        const TDesC& aData,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TInt aOperationId );

    /**
      * Creates contact list update handlers
      * @since 3.0
      * @param aContactList The contact list which needs to be updated
      * @param aTransactionHandlers array to store those new handlers
      * @param aOperationId The operation Id
      */
    void GetContactListsUpdateHandlersL(
        CPEngContactListModBase& aContactList,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TInt aOperationId );

    /**
     * Creates contact lists sync handlers
     *
     * @since 3.0
     * @param aContactList The contact list which needs to be synhronized
     * @param aTransactionHandlers array to store those new handlers
     * @param aOperationId The operation Id
     */
    void GetContactListsSyncHandlersL(
        CPEngContactListModBase& aContactList,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TInt aOperationId );

    /**
     * Create contact list delete handlers
     *
     * @since 3.0
     * @param aContactList The contact list which needs to be deleted
     * @param aTransactionHandlers array to store those new handlers
     * @param aOperationId The operation Id
     */
    void GetDeleteCntListHandlerL(
        CPEngContactListModBase& aModel,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TInt aOperationId );

    /**
     * Get contact list update properties handlers
     *
     * @since 3.0
     * @param aContactList The contact list which needs to be updated
     * @param aTransactionHandlers array to store those new handlers
     * @param aOperationId The operation Id
     */
    void GetUpdateCntListPropertiesHandlerL(
        CPEngContactListModBase& aModel,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TInt aOperationId );

    /**
     * Get Contact list update handlers for existing contact list
     *
     * @since 3.0
     * @param aContactList The contact list which needs to be updated
     * @param aTransactionHandlers array to store those new handlers
     * @param aOperationId The operation Id
     */
    void GetUpdateHandlersForExistingCntListL(
        CPEngContactListModBase& aModel,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TInt aOperationId );

    /**
     * Get Contact list update handlers for not existing contact list
     *
     * @since 3.0
     * @param aContactList The contact list which needs to be updated
     * @param aTransactionHandlers array to store those new handlers
     * @param aOperationId The operation Id
     */
    void GetUpdateHandlersForNotExistingCntListL(
        CPEngContactListModBase& aModel,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TInt aOperationId );

    /**
     * Get Watcher list update handler
     *
     * @since 3.0
     * @param aContactList The watcher contact list
     * @param aTransactionHandlers array to store those new handlers
     * @param aOperationId The operation Id
     */
    void GetWatcherListHandlerL(
        CPEngContactListModBase& aModel,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TInt aOperationId );

    /**
     * Get One contact list synchronize handler
     *
     * @since 3.0
     * @param aContactList The contact list which needs to be synhronized
     * @param aTransactionHandlers array to store those new handlers
     * @return ETrue if there was hanler/s created, else EFalse.
     */
    void GetSynchronizeOneCntListHandlerL(
        CPEngContactListModBase& aModel,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TInt aOperationId );

    /**
     * Get transaction handlers to update subscription of the contact list
     *
     * @since 3.0
     * @param aContactList The contact list which needs to be synhronized
     * @param aTransactionHandlers array to store those new handlers
     * @return ETrue if there was hanler/s created, else EFalse.
     */
    void GetSubscriptionUpdateHandlerL(
        CPEngContactListModBase& aModel,
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TInt aOperationId );


    /**
     * Templated function to create hander with one template parameter.
     */
    template < class THandlerClass,
    class TParam1 >
    void CreateHandler1ParamL(
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TParam1 aParam1,
        TPEngWVCspVersion& aParam2,
        TInt aParam3 );

    /**
     * Templated function to create hander with two template parameter.
     */
    template < class THandlerClass,
    class TParam1,
    class TParam2 >
    void CreateHandler2ParamL(
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TParam1 aParam1,
        TParam2 aParam2,
        TPEngWVCspVersion& aParam3,
        TInt aParam4 );

    /**
     * Templated function to create hander with three template parameter.
     */
    template < class THandlerClass,
    class TParam1,
    class TParam2,
    class TParam3 >
    void CreateHandler3ParamL(
        RPointerArray<MPEngOutgoingTransactionHandler>& aHandlers,
        TParam1 aParam1,
        TParam2 aParam2,
        TParam3 aParam3,
        TPEngWVCspVersion& aParam4,
        TInt aParam5 );


    /**
     * Initializes plain user id.
     * @since 3.0
     */
    void InitializePlainUserId();


    /**
     * Clean contact list settings for creation.
     * @since 3.0
     */
    void CleanContactListSettingsForListCreationL(
        CPEngContactListSettings& aSettings );


private: // constructors

    /**
     * C++ constructor.
     */
    CPEngContactListTransactionManager( MPEngListLibFactory& aFactory,
                                        TPEngWVCspVersion& aCSPVersion );

    /**
     * Symbian constructor.
     */
    void ConstructL( const CPEngSessionSlotId& aSessionSlot );

private: // Data

    /// OWN: Plain user name
    TPtrC                        iPlainUserName;

    /// REF: CSP Version
    TPEngWVCspVersion&           iCSPVersion;

    };

#endif      //  __CPENGCONTACTLISTTRANSACTIONMANAGER_H__


//  End of File

