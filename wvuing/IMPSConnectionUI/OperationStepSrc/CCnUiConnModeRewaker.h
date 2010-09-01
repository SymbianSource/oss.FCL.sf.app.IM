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
* Description:  Connection mode re-waker.
*
*/

#ifndef __CCNUICONNMODEREWAKER_H
#define __CCNUICONNMODEREWAKER_H

//  INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>
#include "MCnUiConnModeRewaker.h"



//FORWARD DECLARATION
class MCnUiClientPlugin;
class CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
 * Connection mode re-waker.
 * Re-wakes given client plug-ins.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiConnModeRewaker ) : public CTimer,
        public MCnUiConnModeRewaker
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiConnModeRewaker* NewL();


    /**
     * Destructor.
     */
    virtual ~CCnUiConnModeRewaker();


private:

    /**
     * C++ constructor.
     */
    CCnUiConnModeRewaker();


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: // Functions from MCnUiConnModeRewaker


    /**
     * From MCnUiConnModeRewaker.
     * @see MCnUiConnModeRewaker.
     * @since 2.1
     */
    void AddToAAModeRewakeListL( MCnUiClientPlugin& aClientPlugin,
                                 CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * From MCnUiConnModeRewaker.
     * @see MCnUiConnModeRewaker.
     * @since 2.1
     */
    void GetAARewakeListL( RArray< TIMPSConnectionClient >& aAARewakedClients );


    /**
     * From MCnUiConnModeRewaker.
     * @see MCnUiConnModeRewaker.
     * @since 2.1
     */
    void ClearAARewakeList();


    /**
     * From MCnUiConnModeRewaker.
     * @see MCnUiConnModeRewaker.
     * @since 2.1
     */
    void RemoveFromAARewakeList( TIMPSConnectionClient aClient );


public: // New functions

    /**
     * Suspends the existing rewake operation.
     *
     * Suspended rewake is resumed automaticly when the
     * suspend is PopAndDestroy():ed from the CleanupStack.
     * Note! PopAndDestroy() must be used!!
     *
     * While the rewaker is suspended, the rewake list contents
     * can be changed trough MCnUiConnModeRewaker methods.
     * The list changes are taken into account when rewaker
     * is resumed again.
     *
     * @since 2.1
     */
    void SuspendLC();


    /**
     * Resumes the suspended rewaker.
     *
     * This is issued automaticly through
     * the CleanupStack after using the SuspendLC();
     *
     * @since 2.1
     */
    void ResumeRewake();


protected: // Functions from CTimer


    /**
     * RunL().
     */
    void RunL();


    /**
     * RunError().
     *
     * @since 2.1
     * @param aError The leave code from RunL leave.
     * Stores this as iStatus member.
     * @return Always KErrNone.
     */
    TInt RunError( TInt aError );



private: // New functions


    /**
     * Rewakes all registered clients.
     *
     * @since 2.1
     */
    void RewakeRegisteredClientsL();


    /**
     * Checks is the client already
     * registered or not.
     *
     * @since 2.1
     * @param aClient The Client to check.
     * @return ETrue if the client is already registered for
     * rewake. Else EFalse.
     */
    TBool ClientAlreadyReqistered( TIMPSConnectionClient aClient );


private: // data

    ///<Clients to re-wake. owned
    RPointerArray< MCnUiClientPlugin >       iAAClntsToRewake;

    };

#endif      //  __CCNUICONNMODEREWAKER_H

//  End of File

