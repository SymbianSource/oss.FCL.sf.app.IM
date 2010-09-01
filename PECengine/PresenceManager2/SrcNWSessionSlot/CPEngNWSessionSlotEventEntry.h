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
* Description:  NWSessionSlotEvent entry.
*
*/

#ifndef CPENGNWSESSIONSLOTEVENTENTRY_H
#define CPENGNWSESSIONSLOTEVENTENTRY_H

//  INCLUDES
#include <E32Base.h>
#include <PEngPresenceEngineConsts2.h>


//FORWARD DECLARATIONS
class CPEngNWSessionSlotEvent2;
class CPEngSessionSlotEvent;
class CPEngNWSessionSlotID2;
class CPEngSessionSlotId;



/**
 * NWSessionSlotEvent entry.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngNWSessionSlotEventEntry ) : public CBase
    {

    //-----------------------------------------------------------------------
public: /* Construction */

    /**
     * Instantiates CPEngNWSessionSlotEventEntry object.
     *
     * @return New CPEngNWSessionSlotEventEntry instance.
     */
    static CPEngNWSessionSlotEventEntry* NewL();


    /**
     * Destructor.
     * Virtual by CBase.
     */
    ~CPEngNWSessionSlotEventEntry();


    /**
     * Copies CPEngNWSessionSlotEventEntry data from given source.
     *
     * @param aSource The CPEngNWSessionSlotEventEntry event source.
     */
    void CopyL( const CPEngNWSessionSlotEventEntry& aSource );



protected:  // Constructor

    /**
     * C++ default constructor.
     */
    CPEngNWSessionSlotEventEntry();



    //-----------------------------------------------------------------------
public: /* Interface */


    /**
     *
     *
     * @since 3.0
     *
     */
    void SetFullEventReferenceL( const CPEngSessionSlotEvent& aInternalEvent );


    /**
     *
     *
     * @since 3.0
     *
     */
    void SetPseudoEventReferenceL( const CPEngSessionSlotId& aBasePart,
                                   const TDesC& aAppId,
                                   TPEngNWSessionSlotState aAppState,
                                   TPEngNWSessionSlotState aCSPState );


    /**
     *
     *
     * @since 3.0
     *
     */
    void ResetReferences();



private: //Data


    /**
     * Interface is friend.
     * @since 3.0
     */
    friend class CPEngNWSessionSlotEvent2;      // CSI: 36 #


    //OWN: Exported class interface
    CPEngNWSessionSlotEvent2*   iInterface;

    //OWN: The NWSessionSlotID
    CPEngNWSessionSlotID2*      iNWSessionSlotID;

    //OWN: The event
    TPEngNWSessionSlotEvent     iEvent;

    //OWN: The NWSessionSlot state
    TPEngNWSessionSlotState     iNWSessionSlotState;

    //OWN: The CSPSessionState state
    TPEngNWSessionSlotState     iCSPSessionState;

    //OWN: The additional integer
    TInt                        iEventInt;

    //OWN: The additional descriptor
    TPtrC                       iEventDesc;

    //OWN: The additional descriptor value buffer for clones
    HBufC*                      iEventDescBuf;

    };

#endif      //CPENGNWSESSIONSLOTEVENTENTRY_H



