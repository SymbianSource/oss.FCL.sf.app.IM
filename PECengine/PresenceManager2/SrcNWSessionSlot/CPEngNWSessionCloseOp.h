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
* Description:  Operation handler for closing the network session.
*
*/

#ifndef CPENGNWSESSIONCLOSEOP_H
#define CPENGNWSESSIONCLOSEOP_H


//  INCLUDES
#include <E32Base.h>
#include "CPEngNWSessionOperation.h"


// FORWARD DECLARATIONS



/**
 * Operation handler for closing the network session.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngNWSessionCloseOp ) : public CPEngNWSessionOperation
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngNWSessionCloseOp* NewL( TInt aPriority,
    CPEngNWSessionProxy& aNWSessionProxy,
    CPEngNWSessionSlot2& aInterface,
    MPEngNWSessionOperationObserver2& aObserver,
    TBool aForceClose );

    /**
     * Destructor.
     */
    virtual ~CPEngNWSessionCloseOp();


private:

    /**
     * C++ default constructor.
     */
    CPEngNWSessionCloseOp( TInt aPriority,
                           CPEngNWSessionProxy& aNWSessionProxy,
                           CPEngNWSessionSlot2& aInterface,
                           MPEngNWSessionOperationObserver2& aObserver,
                           TBool aForceClose );

    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public: //New services


    /**
     *
     *
     * @since 3.0
     */
    void IssueNWSessionClose();



private: //From CActive

    /**
     *
     *
     * @since 3.0
     */
    void DoCancel();



protected:  // Template methods from base

    void DoHandleOpCompleted( TInt aError );





private: //Data

    //OWN:
    TBool   iForceClose;


    };

#endif  //CPENGNWSESSIONCLOSEOP_H




