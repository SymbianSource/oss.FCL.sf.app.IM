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
* Description:  Shared basic implementation for client plug-ins.
*
*/

#ifndef __CCNUICLIENTPLUGINBASE_H
#define __CCNUICLIENTPLUGINBASE_H

//  INCLUDES
#include <E32Base.h>
#include <badesca.h>
#include <MPEngAttributeTransactionObserver2.h>
#include <MPEngContactListTransactionObserver2.h>
#include <MPEngAttributeListTransactionObserver2.h>
#include <PEngWVServices2.h>

#include "MCnUiClientPlugin.h"

const TInt KCollationLevel = 0;

// FORWARD DECLARATIONS

class CPEngAttributeStore2;
class CPEngAttributeTransaction2;
class CPEngAttributeListStore2;
class CPEngAttributeListTransaction2;
class CPEngContactListStore2;
class CPEngContactListTransaction2;
class CPEngNWSessionSlotID2;



// CLASS DECLARATION
/**
 * Shared basic implementation for client plug-ins.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiClientPluginBase ) : public CBase,
        public MCnUiClientPlugin,
        public MPEngContactListTransactionObserver2,
        public MPEngAttributeListTransactionObserver2,
        public MPEngAttributeTransactionObserver2



    {
protected: //Enumerations

    /**
     * Sub processing steps ID's.
     * @since 2.1
     */
    enum TStepId
        {
        ECntListPublish = 1,
        EAttribListPublish = 2,
        EAttributePublish = 3,
        EBaseCntListSynch = 4
        };


    /**
     * Sub processing types.
     * @since 2.1
     */
    enum TStepProcessType
        {
        EExplicitPostLoginSync = 1,
        EPreLogoutPublish = 2
        };


protected: //constructors and destructor

    /**
     * C++ constructor.
     */
    CCnUiClientPluginBase( TIMPSConnectionClient aClientID );


    /**
     * Destructor.
     */
    ~CCnUiClientPluginBase();


    /**
     * Symbian OS constructor.
     */
    void ConstructL( CPEngNWSessionSlotID2& aNWSessionSlotID );


public: //From MCnUiClientPlugin


    /**
     * From MCnUiClientPlugin
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    TIMPSConnectionClient ClientID();


    /**
     * From MCnUiClientPlugin
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    void ReleaseDynamicResources();


    //New connection open related methods
    /**
     * From MCnUiClientPlugin
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    void ConnectionOpenInitL( TBool aSapConnectionOpen,
                              const CIMPSSAPSettings& aSap );


    /**
     * From MCnUiClientPlugin
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    TBool VerifySapCapabilitiesL();


    /**
    * From MCnUiClientPlugin
    * @see MCnUiClientPlugin
    * @since 2.1
    */
    void DoPostLoginProsessingL( TRequestStatus& aStatus );


    /**
    * From MCnUiClientPlugin
    * @see MCnUiClientPlugin
    * @since 2.1
    */
    void CancelPostLoginProsessing();


    //New connection close related methods
    /**
    * From MCnUiClientPlugin
    * @see MCnUiClientPlugin
    * @since 2.1
    */
    void ConnectionCloseInitL( const CIMPSSAPSettings& aSap );


    /**
    * From MCnUiClientPlugin
    * @see MCnUiClientPlugin
    * @since 2.1
    */
    void DoPreLogoutProsessingL( TRequestStatus& aStatus );


    /**
    * From MCnUiClientPlugin
    * @see MCnUiClientPlugin
    * @since 2.1
    */
    void CancelPreLogoutProsessing();

    /**
     * From MCnUiClientPlugin
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    void LoginCancelledL();


private: // From MPEngContactListTransactionObserver2


    /**
     * From MPEngContactListTransactionObserver2
     * @see MPEngContactListTransactionObserver2
     */
    void HandleContactListTransactionError(
        TInt aError,
        CPEngContactListTransaction2& aAttributeTransaction,
        TInt aTransactionOperation );

    /**
     * From MPEngContactListTransactionObserver2
     * @see MPEngContactListTransactionObserver2
     */
    void HandleContactListTransactionCompleteL(
        MPEngTransactionStatus2& aStatus,
        CPEngContactListTransaction2& aContactListTransaction,
        TInt aTransactionOperation );


private: // From MPEngAttributeListTransactionObserver2

    /**
     * From MPEngAttributeListTransactionObserver2
     * @see MPEngAttributeListTransactionObserver2
     */
    void HandleAttributeListTransactionError(
        TInt aError,
        CPEngAttributeListTransaction2& aAttributeTransaction,
        TInt aTransactionOperation );

    /**
     * From MPEngAttributeListTransactionObserver2
     * @see MPEngAttributeListTransactionObserver2
     */
    void HandleAttributeListTransactionCompleteL(
        MPEngTransactionStatus2& aStatus,
        CPEngAttributeListTransaction2& aAttributeListTransaction,
        TInt aTransactionOperation );




private: //From MPEngAttributeTransactionObserver2

    /**
     * From MPEngAttributeTransactionObserver2
     * @see MPEngAttributeTransactionObserver2
     * @since 2.1
     */
    void HandleAttributeTransactionError(
        TInt aError,
        CPEngAttributeTransaction2& aAttributeTransaction,
        TInt aTransactionOperation );

    /**
     * From MPEngAttributeTransactionObserver2
     * @see MPEngAttributeTransactionObserver2
     * @since 2.1
     */
    void HandleAttributeTransactionCompleteL(
        MPEngTransactionStatus2& aStatus,
        CPEngAttributeTransaction2& aAttributeTransaction,
        TInt aTransactionOperation );



protected: // New template methods & helpers to derived classes

    /**
     * Template method for concrete client plug-ins
     * to do their client specific capability checks.
     *
     * @since 2.1
     * @param aCurrenServices The Current NW Server capabilities
     * expressed as IMPS Engine TImpsServices.
     * @return ETrue if network server capabilities are OK for
     * client. Else EFalse.
     */
    virtual TBool DoVerifySapCapabilities( TPEngWVCspServicesTree2 aCurrenServices ) = 0;

    /**
     * Template method for concrete client
     * plug-ins to enumerate contact list used by those.
     *
     * @since 2.1
     * @return An array containing known contact list names.
     */
    virtual CDesCArray* DoClientKnownCntListsLC() = 0;


    /**
     * Template method for concrete client
     * plug-ins to start their explicit post login
     * synchronization processing.
     *
     * Client plug-in can use PENG publishers holded by this base
     * to do the sub steps. If client starts processing,
     * it has to signal to base with SetStepProcessingActive()
     *
     * @since 2.1
     */
    virtual void DoStartExplicitPostLoginSyncL() = 0;


    /**
     * Template method for concrete client
     * plug-ins to get notifications from post login
     * processing step completions and handle those
     * as needed.
     *
     * @since 2.1
     * @param aStepId The step ID which completed.
     * @param aStepStatus The completion status.
     */
    virtual void DoHandleExplicitPostLoginSyncStepCompleteL( TStepId aStepId,
                                                             TInt& aStepStatus ) = 0;

    /**
     * Template method for concrete client
     * plug-ins to start their explicit pre logout
     * publishing.
     *
     * Client plug-in can use PENG publishers holded by this base
     * to do the sub steps. If client starts processing,
     * it has to signal to base with SetStepProcessingActive()
     *
     * @since 2.1
     */
    virtual void DoStartPreLogoutPublishL() = 0;


    /**
     * Template method for concrete client
     * plug-ins to get notifications from pre logout
     * processing step completions and handle those
     * as needed.
     *
     * @since 2.1
     * @param aStepId The step ID which completed.
     * @param aStepStatus The completion status.
     */
    virtual void DoHandlePreLogoutPublishStepCompleteL( TStepId aStepId,
                                                        TInt& aStepStatus ) = 0;

    /**
     * Signals to base that there were started a
     * sub processing step.
     *
     * @since 2.1
     */
    void SetStepProcessingActive();



private: //Private helpers


    /**
     * Helper to handle sub step completions.
     *
     * @since 2.1
     * @param aStepId The step ID which completed.
     * @param aStepStatus The completion status.
     */
    void HandleStepComplete( TStepId aStepId, TInt aStepStatus );


    /**
     * Helper to cancel all PENG publishers and
     * complete the original request.
     * This method is protected against partial
     * construction, so it can be used from destructor.
     *
     * @since 2.1
     */
    void CancelAllProcessingAndCompleteIfNeeded();


    /**
     * Helper to cancel all PENG publishers.
     * This method is protected against partial
     * construction, so it can be used from destructor.
     *
     * @since 2.1
     */
    void CancelAllPEngPublishers();


protected : // Protected data available to real clients

    ///<PENG contact list handlers, owned.
    CPEngContactListStore2*               iPEngCntListStore;
    CPEngContactListTransaction2*          iPEngCntListPublisher;

    ///<PENG attribute list handlers, owned.
    CPEngAttributeListStore2*             iPEngAttributeListStore;
    CPEngAttributeListTransaction2*        iPEngAttributeListPublisher;

    ///<PENG attribute handlers, owned.
    CPEngAttributeStore2*                 iPEngAttributeStore;
    CPEngAttributeTransaction2*            iPEngAttributePublisher;

    // the state of this state machine
    TStepId                              iStepId;

    // owns: network session slot ID
    CPEngNWSessionSlotID2*               iNWSessionSlotID;

private: // Private data

    ///<Client ID, owned
    const TIMPSConnectionClient          iClientID;

    ///<Is explicit sync needed flag, owned
    TBool                                iExplicitSyncNeeded;

    ///<Is step processing active, owned
    TBool                                iStepProcessingActive;

    ///<Step processing type, owned
    TStepProcessType                     iStepProcessType;

    ///<External request status complete after step precessing, owned
    TRequestStatus*                      iRequest;


    };

#endif      //__CCNUICLIENTPLUGINBASE_H
//  End of File


