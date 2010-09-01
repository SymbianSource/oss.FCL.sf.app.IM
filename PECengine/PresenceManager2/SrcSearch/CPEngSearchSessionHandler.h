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
* Description: this class handles the search session
*
*/

#ifndef CPENGSEARCHSESSIONHANDLER_H
#define CPENGSEARCHSESSIONHANDLER_H

//  INCLUDES
#include <E32Base.h>
#include <ImpsClient.h>
#include <ImpsAccessCli.h>



// FORWARD DECLARATIONS
class CPEngNWSessionSlot2;
class CIMPSSAPSettings;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlot2;



/**
 *
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSearchSessionHandler ) : public CBase,
        public MImpsAccessHandler2,
        public MImpsErrorHandler2
    {

private: //enumerations

    /**
     *
     */
    enum TPEngAccessOperation
        {
        EPEngAccOp_Idle = 0, //Idle state, no operation running
        EPEngAccOp_LogIn = 1,
        EPEngAccOp_LogOut = 2,
        };


public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngSearchSessionHandler* NewL( TInt aPriority,
                                            const CPEngNWSessionSlotID2& aNWSessionSlotID );

    /**
     * Destructor.
     */
    virtual ~CPEngSearchSessionHandler();


private:

    /**
     * C++ default constructor.
     */
    CPEngSearchSessionHandler();

    /**
     * Symbian OS constructor.
     */
    void ConstructL( TInt aPriority,
                     const CPEngNWSessionSlotID2& aNWSessionSlotID );




public: //New services

    /**
     *
     *
     *
     */
    void OpenCspSessionL( TRequestStatus& aStatus );


    /**
     *
     *
     *
     */
    void CloseCspSessionL( TRequestStatus& aStatus );


    /**
     *
     *
     *
     */
    RImpsEng& ImpsEngine();



    /**
     *
     *
     *
     */
    TBool IsRequestPending( TInt aId );



private: //From MImpsAccessHandler2


    /**
     * Observer method for Login and Open event.
     * Login may be initiated by other application.
     * @param aId operation id received in LoginL. If this is 0, then the
     *        login was initiated by other application or SAP.
     * @param aCspId CSP session identifier
     */
    void HandleLoginL( TInt aId,
                       TImpsCspIdentifier& aCspId );

    /**
     * Observer method for Logout event.
     * @param aId operation id received in LogoutL. If this is 0, then the
     *        logout was initiated by other application or caused by a serious
     *        network error. SAP initiated logouts are identified with -1.
     * @param aCspId CSP session identifier
     */
    void HandleLogoutL( TInt aId,
                        TImpsCspIdentifier& aCspId );

    /**
     * Observer method for CancelLogin event.
     * @param aCancelledOpId operation id of the canceled login.
     * @param aCspId CSP session identifier
     */
    void HandleLoginCancelL( TInt aCancelledOpId,
                             TImpsCspIdentifier& aCspId );


    /**
     * Observer for NumberOfSessions and NumberOfSubSessions response
     * @param aId operation id
     * @param aNbr number of sessions
     * @param aCspId CSP session identifier
     */
    void HandleNbrSessionsL( TInt aId,
                             TInt aNbr,
                             TImpsCspIdentifier& aCspId );


private: // From MImpsErrorHandler2

    /**
     * Observer method for error messages.
     * This provides data for partial error responses too.
     * Each service API MUST register this observer by RegisterErrorObserverL
     * immediately after Register method.
     * @param aStatus error code.
     * @param aOpId operation id to map responses to the requests.
     * @param aDescripton error description
     * @param aDetailedRes detailed error description
     * @param aCspId CSP session identifier
     */
    void HandleErrorL( TInt aStatus,
                       TInt aOpId,
                       const TDesC* aDescription,
                       const CImpsDetailed* aDetailedRes,
                       TImpsCspIdentifier& aCspId );



private: //Data


    CIMPSSAPSettings*       iImpsSap;
    RImpsEng                iImpsEng;
    RImpsAccessClient2      iImpsAccClient;

    TInt iOpId;
    TPEngAccessOperation    iOperation;
    TRequestStatus*         iRequest;

    };

#endif  // CPENGSEARCHSESSIONHANDLER_H



