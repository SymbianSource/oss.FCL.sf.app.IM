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
* Description:  Connection UI interprocess signaller.
*
*/

#ifndef __CCNUISIGNALLER_H
#define __CCNUISIGNALLER_H

//  INCLUDES
#include <E32Base.h>
#include "MCnUiSignaller.h"
#include "CCnUiGroupChannel.h"


//FORWARD DECLARATION



// CLASS DECLARATION
/**
 * Connection UI signaller.
 *
 * Connection UI interprocess signalling implementation.
 * Connection UI signaller is used to do IPC signal between
 * different Connection Ui client process.
 *
 * Implements the MCnUiSignaller interface.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiSignaller ) : public CBase,
        public MCnUiSignaller
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiSignaller* NewL();

    /**
     * Destructor.
     */
    virtual ~CCnUiSignaller();


private:

    /**
     * C++ constructor.
     */
    CCnUiSignaller();


    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public: // Functions from MCnUiSignaller


    /**
     * From MCnUiSignaller.
     *
     * @see MCnUiSignaller.
     * @since 2.1
     */
    TInt SignalOperationL( const TDesC& aDetailDesc );


    /**
     * From MCnUiSignaller.
     *
     * @see MCnUiSignaller.
     * @since 2.1
     */
    void CancelOperationSignal();


    /**
     * From MCnUiSignaller.
     *
     * @see MCnUiSignaller.
     * @since 2.1
     */
    TBool OperationRunning();


    /**
     * From MCnUiSignaller.
     *
     * @see MCnUiSignaller.
     * @since 2.1
     */
    void OperationDetailsL( HBufC*& aDetailDesc );


private: // data

    ///<Flag for holding the operation signal notification state, owned
    TBool                   iOperationSignalRequested;

    ///<Operation signal channel, owned
    CCnUiGroupChannel*      iOperationSignalChannel;
    };

#endif      //  __CCNUISIGNALLER_H

//  End of File

