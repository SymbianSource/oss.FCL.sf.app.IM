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
* Description:  NWSessionOperation base class.
*
*/

#ifndef CPENGNWSESSIONOPERATION_H
#define CPENGNWSESSIONOPERATION_H

//  INCLUDES
#include <E32Base.h>



// FORWARD DECLARATIONS
class MPEngNWSessionOperationOwner;
class CPEngNWSessionSlot2;
class MPEngNWSessionOperationObserver2;
class CPEngNWSessionSlotID2;
class CPEngTransactionStatus;
class CPEngNWSessionProxy;



// CLASS DECLARATION

/**
 * NWSessionOperation base class.
 *
 * Provides:
 * - adding to scheduler
 * - protection against nested canceling
 * - operation owning services
 *
 *  @lib
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngNWSessionOperation ) : public CActive
    {
public:  // destructor

    /**
     * Destructor.
     */
    virtual ~CPEngNWSessionOperation();

protected: //Constructor

    /**
     * Constructor.
     */
    CPEngNWSessionOperation( TInt aPriority,
                             CPEngNWSessionProxy& aNWSessionProxy,
                             CPEngNWSessionSlot2& aInterface,
                             MPEngNWSessionOperationObserver2& aObserver );

    /**
     * Symbian OS base class constructor.
     */
    void BaseConstructL();


public:  // New external services

    /**
     * Sets the operation owner.
     * Owner can be set just once.
     */
    void SetOwner( MPEngNWSessionOperationOwner& aOperationOwner );

    /**
     * Cancels the operation if operation active
     * and not yet cancelled.
     *
     * Deletes also the operation object.
     */
    void CancelOpD();


protected:  // Services for derived implementations


    /**
     *
     */
    void HandleOperationIssued( TInt aError, TInt aNWSessionSlotOp );

    /**
     *
     */
    void NotifyObserverFromCancel();




protected:  // Template methods for derived implementations

    /**
     * Notifies derived implementation from
     * operation complete or failure with error.
     */
    virtual void DoHandleOpCompleted( TInt aError ) = 0;




private:  // Implementation

    void RunL();
    TInt RunError( TInt aError );
    //DoCancel() is implemented by the derived classes


protected:  // Data

    //REF:
    CPEngNWSessionProxy&    iNWSessionProxy;



private:  // Data

    //REF: The implemented interface
    CPEngNWSessionSlot2&     iInterface;

    //REF: The observer
    MPEngNWSessionOperationObserver2&  iObserver;

    //OWN: Is cancel call running?
    TBool   iCancelling;

    //REF: Operation owner
    MPEngNWSessionOperationOwner*   iOperationOwner;

    //OWN: Transaction status container for result
    CPEngTransactionStatus*     iTransactionStatus;

    //OWN: Session Slot operation identifier
    TInt    iNWSessionSlotOp;
    };


#endif  //CPENGNWSESSIONOPERATION_H
