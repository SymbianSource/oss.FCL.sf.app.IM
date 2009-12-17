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

#ifndef __CPENGSESSIONMANAGER_H__
#define __CPENGSESSIONMANAGER_H__

//  INCLUDES
#include <E32Base.h>
#include <ImpsClient.h>

#include "MPEngSessionManager.h"


// FORWARD DECLARATIONS
class MPEngAccessHandler;
class CPEngAccessHandler;
class MPEngPureDataHandler;
class CPEngPureDataHandler;
class MPEngTransAdapFactory;
class CPEngSessionSlotId;

/**
 * NWSessionSlotID implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSessionManager ) :
        public CObject,
        public MPEngSessionManager
    {
public: /* Construction */


    /**
     * Instantiates CPEngSessionManager object.
     *
     * @return New CPEngSessionManager instance.
     */
    static CPEngSessionManager* NewLC( MPEngTransAdapFactory& aFactory,
    const CPEngSessionSlotId& aSesssId );


    /**
     * Destructor.
     */
    ~CPEngSessionManager();

    //-----------------------------------------------------------------------
public: // New function of the MPEngSessionManager

    /**
     *  Close Access handler reference, deleted it
     *
     *  @since 3.0
     *
     *  @param aHandler to be closed
     */
    void CloseAccessHandler( );

    /**
     *  Close Access handler
     *
     *  @since 3.0
     *  @param aHandler handler to be closed
     */
    void ClosePureDataHandler( );


    /**
     * Open reference
     * @since 3.0
     */
    void OpenRef();

    //-----------------------------------------------------------------------
public: // New functions

    /**
     *  Access handler
     *
     *  @since 3.0
     *  @return Access handler
     */
    MPEngAccessHandler* AccessHandlerL();

    /**
     *  Pure Data handler
     *
     *  @since 3.0
     *  @return Pure data handler
     */
    MPEngPureDataHandler* PureDataHandlerL();

    /**
     *  Session Id
     *
     *  @since 3.0
     *  @return session Id class
     */
    const CPEngSessionSlotId& SessionId() const;

protected: // construction

    /**
     * C++ constructor. protected
     */
    CPEngSessionManager( MPEngTransAdapFactory& aFactory );

    /**
     * Symbian Second phase constructor
     */
    void ConstructL( const CPEngSessionSlotId& aSesssId );

private: //Data

    /// REF: Session manager Factory
    MPEngTransAdapFactory&  iFactory;

    /// OWN: Session Id
    CPEngSessionSlotId*     iSessionId;

    /// OWN: WV Session main client
    RImpsEng                iImpsClient;

    /// OWN: Access handler
    CPEngAccessHandler*     iAccessHandler;

    /// OWN: Pure data handler
    CPEngPureDataHandler*   iPureDataHandler;

    };

#endif  //__CPENGSESSIONMANAGER_H__

// End of File
