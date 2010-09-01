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

#ifndef __MCNUISIGNALLER_H
#define __MCNUISIGNALLER_H

//  INCLUDES
#include <e32base.h>


//FORWARD DECLARATION
class MCnUiSignaller;


/**
 * Global factory method to create concrete
 * signaller.
 *
 * @since 2.1
 * @return New MCnUiSignaller instance.
 */
GLREF_D MCnUiSignaller* CreateConnUiSignallerL();
GLREF_D MCnUiSignaller* CreateConnUiSignallerLC();




// CLASS DECLARATION
/**
 * Connection UI signaller.
 *
 * Interface for Connection UI interprocess signalling.
 * Connection Ui signaller is used to signal between
 * different Connection Ui client processies.
 *
 * Signaller offers methods to signal from running
 * operations, raised errors etc.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiSignaller )
    {
public: // New operation signalling related functions.

    /**
     * Sets operation signal "on".
     *
     * Sets operation signal "on". Setted signal is "on" untill
     * the signal is cancelled, or the signaller object
     * is destroyed. There can be globally only one
     * operation signal "on" at time. If there exist already
     * other operation signal on "on" state when executing
     * this SignalOperationL(), returns KErrInUse.
     * Else returns KErrNone. If issuing the signal
     * fails due out of resources, leaves with KErrNoMemory.
     *
     * @since 2.1
     * @param aDetailDesc The detail descriptor to hold with the operation signal.
     */
    virtual TInt SignalOperationL( const TDesC& aDetailDesc ) = 0;


    /**
     * Cancels previously by this object issued operation signal.
     *
     * Cancels previously by this object issued operation signal.
     * If no operation signal is issued, does nothing.
     *
     * @since 2.1
     */
    virtual void CancelOperationSignal() = 0;


    /**
     * Tests the signal state.
     *
     * Tests wether the operation signal is "on" or "off".
     *
     * @since 2.1
     * @return ETrue if the signal was on. Else EFalse.
     */
    virtual TBool OperationRunning() = 0;


    /**
     * Gets the operation signal details.
     *
     * Tests wether the operation signal is "on" or "off".
     * If signal is off, returns empty buffer.
     * Else returns the value previously stored by the
     * SignalOperationL().
     *
     * Ownership of the created buffer is returned to
     * caller.
     *
     * @since 2.1
     * @param aDetailDesc The detail descriptor holded with the signal.
     */
    virtual void OperationDetailsL( HBufC*& aDetailDesc ) = 0;


public:  //Destructor

    /**
     * Virtual inline destructor.
     *
     * Concrete object can be destructed
     * using this interface.
     *
     * Destroying signaller cancels any previously
     * by this signaller object raised operation signal.
     * (Effectively done by calling the
     * CancelOperationSignal().)
     */
    virtual ~MCnUiSignaller() {};
    };


#endif      //  __MCNUISIGNALLER_H

//  End of File

