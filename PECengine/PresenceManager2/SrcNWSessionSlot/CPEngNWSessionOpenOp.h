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
* Description:  Operation handler for opening the network session.
*
*/

#ifndef CPENGNWSESSIONOPENOP_H
#define CPENGNWSESSIONOPENOP_H


//  INCLUDES
#include <E32Base.h>
#include "CPEngNWSessionOperation.h"


// FORWARD DECLARATIONS
class MPEngNWSessionOperationObserver2;
class CIMPSSAPSettings;
class CPEngNWSessionProxy;



/**
 * Operation handler for opening the network session.
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngNWSessionOpenOp ) : public CPEngNWSessionOperation
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngNWSessionOpenOp* NewL( TInt aPriority,
    CPEngNWSessionProxy& aNWSessionProxy,
    CPEngNWSessionSlot2& aInterface,
    const CIMPSSAPSettings& aImpsSap,
    MPEngNWSessionOperationObserver2& aObserver );

    /**
     * Destructor.
     */
    virtual ~CPEngNWSessionOpenOp();


private:

    /**
     * C++ default constructor.
     */
    CPEngNWSessionOpenOp( TInt aPriority,
                          CPEngNWSessionProxy& aNWSessionProxy,
                          CPEngNWSessionSlot2& aInterface,
                          MPEngNWSessionOperationObserver2& aObserver );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CIMPSSAPSettings& aImpsSap );



public: //New services


    /**
     *
     *
     * @since 3.0
     */
    void IssueNWSessionOpen();



private: //From CActive

    /**
     *
     *
     * @since 3.0
     */
    void DoCancel();


protected:  // Template methods from base

    void DoHandleOpCompleted( TInt aError );



private: //helper

    void CheckNetworkAccessAllowedL();



private: //Data

    //OWN:  The login data buffer
    HBufC8*     iLoginData;

    };

#endif  //CPENGNWSESSIONOPENOP_H




