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
* Description:  Event notifier implementation.
*
*/

#ifndef __CIMPSCONNUIPRESEVENTNOTIFIERIMP_H
#define __CIMPSCONNUIPRESEVENTNOTIFIERIMP_H


//  INCLUDES
#include <E32Base.h>
#include "CIMPSConnUiPresEventNotifierImpBase.h"
#include <MPEngNWSessionSlotObserver2.h>


// FORWARD DECLARATIONS
class MIMPSEventNotifierObserver;
class CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
 * CIMPSConnUiPresEventNotifierImp.
 * Implements the CIMPSConnUiPresEventNotifier
 * features. Uses CIMPSConnUiPresEventNotifierImpBase
 * services in implementation.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CIMPSConnUiPresEventNotifierImp ) : public CIMPSConnUiPresEventNotifierImpBase,
        public MPEngNWSessionSlotObserver2
    {
public:  // Two-phased constructor and destructor


    /**
     * Two-phased constructor.
     * Takes a pointer reference to implemented interface.
     * This interface is given to notified observers.
     *
     * @param aInterface The implemented interface.
     * No ownership taken.
     */
    static CIMPSConnUiPresEventNotifierImp* NewL( CIMPSConnUiPresEventNotifier& aInterface );


    /**
     * Destructor.
     */
    ~CIMPSConnUiPresEventNotifierImp();

private:

    /**
     * C++ constructor.
     */
    CIMPSConnUiPresEventNotifierImp( CIMPSConnUiPresEventNotifier& aInterface );

    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public:  // Methods from CIMPSConnUiPresEventNotifierImpBase


    /**
     * Implements start notifying as described
     * in CIMPSConnUiPresEventNotifier.
     * @see CIMPSConnUiPresEventNotifier
     * @since 2.1
     * @param aEvents Events to request.
     */
    void DoStartL( const TArray<TPEngNWSessionSlotEvent>& aEvents );


    /**
     * Implements stop notifying as described
     * in CIMPSConnUiPresEventNotifier.
     * @see CIMPSConnUiPresEventNotifier
     * @since 2.1
     */
    void DoStop();


    /**
     * Implements service status get as described
     * in CIMPSConnUiPresEventNotifier.
     * @see CIMPSConnUiPresEventNotifier
     * @since 2.1
     */
    const CPEngNWSessionSlotID2* AppServiceStatusL( TIMPSConnectionClient aClient );



public:  // Methods from MPEngEventNotifierObserver

    /**
     * From MPEngEventNotifierObserver.
     * Converts event to application enumeration and
     * forwards it to observers.
     *
     * @since 3.0
     */
    void HandleNWSessionSlotChangeL(
        CPEngNWSessionSlotNotifier2& aNotifier,
        CPEngNWSessionSlotEvent2& aEvent );

    /**
     * From MPEngEventNotifierObserver.
     *
     * Forwards error to observers.
     *
     * @since 3.0
     */
    void HandleNWSessionSlotError(
        TInt aError,
        CPEngNWSessionSlotNotifier2& aNotifier );

private: //  New helper methods

    /**
    * Calls ResetAndDestroy to given array
    * @since 3.0
    * @param aObject Array (RPointerArray<CPEngNWSessionSlotID2>)
    */
    static void DestroyCloseModelArray( TAny* aObject );

private: //Data

    ///< PEC Engine event notifier, owned
    CPEngNWSessionSlotNotifier2*    iEventNotifier;
    CPEngNWSessionSlotID2*          iSessionSlotID;

    };

#endif      //__CIMPSCONNUIPRESEVENTNOTIFIERIMP_H
//  End of File


