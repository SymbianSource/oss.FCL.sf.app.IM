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
* Description:  Access handler of the network session
*
*/

#ifndef __CPENGTRANSADAPFACTORY_H__
#define __CPENGTRANSADAPFACTORY_H__

//  INCLUDES
#include <E32Base.h>
#include "MPEngTransAdapFactory.h"


// FORWARD DECLARATIONS
class MPEngAccessHandler;
class MPEngPureDataHandler;
class CPEngSessionSlotId;
class CPEngSessionManager;

/**
 * NWSessionSlotID implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngTransAdapFactory ) :
        public CObject,
        public MPEngTransAdapFactory
    {
public: /* Construction */

    /**
     *  Tryes to get instance from the Thread local storage
     *  if it does not exists there, it is constructed and stored
     *  there
     *
     *  @since 3.0
     *  @return CPEngTransAdapFactory instance
     */
    static CPEngTransAdapFactory* InstanceLC();

    /**
     * Destructor.
     */
    ~CPEngTransAdapFactory();

    //-----------------------------------------------------------------------
public: //new functions of the MPEngTransAdapFactory

    /**
     *  Add session reference
     *
     *  @since 3.0
     */
    void OpenSessionCount();

    /**
     *  Close session manager
     *
     *  @since 3.0
     *  @param aSessManager session manager
     */
    void CloseSession( CPEngSessionManager* aSessManager );

    //-----------------------------------------------------------------------
public: //

    /**
     *  Get Access handler for defined session
     *
     *  @since 3.0
     *  @param aSessionId
     *  @return access handler
     */
    MPEngAccessHandler* AccessHandlerL(
        const CPEngSessionSlotId& aSessionId );

    /**
     *  Get Pure data handler for defined session Id
     *
     *  @since 3.0
     *  @param
     *  @return
     */
    MPEngPureDataHandler* PureDataHandlerL(
        const CPEngSessionSlotId& aSessionId );

private: //

    /**
     *  Get Session manager
     *
     *  @since 3.0
     *  @param aSessionId session id to find
     *  @return session manager
     */
    CPEngSessionManager& SessionManagerLC(
        const CPEngSessionSlotId& aSessionId );

protected: // construction

    /**
     * C++ constructor. protected
     */
    CPEngTransAdapFactory();

private: //Data

    /// OWN: Access count
    TInt                                    iAccessCount;

    /// OWN: Active managers,
    ///      elements OWNED
    RPointerArray<CPEngSessionManager>      iSessManagers;

    };

#endif  //__CPENGTRANSADAPFACTORY_H__

// End of File
