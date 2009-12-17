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
* Description:  NWSessionSlot notifier API implementation
*
*/

#ifndef CPENGNWSESSIONSLOTNOTIFIER2IMP_H
#define CPENGNWSESSIONSLOTNOTIFIER2IMP_H

//  INCLUDES
#include <E32Base.h>
#include <PEngPresenceEngineConsts2.h>
#include "RGenericObserverArray.h"
#include "MPEngGlobalEventObserver.h"



//FORWARD DECLARATIONS
class CPEngNWSessionSlotNotifier2;
class CPEngNWSessionSlotEvent2;
class CPEngNWSessionSlotEvent2Copy;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotID2Imp;
class MPEngNWSessionSlotObserver2;


/**
 * NWSessionSlot notifier API implementation
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngNWSessionSlotNotifier2Imp ) : public CBase,
        public MPEngGlobalEventObserver

    {
public: /* Construction */


    /**
     * Instantiates CPEngNWSessionSlotNotifier2Imp object.
     *
     * @return New CPEngNWSessionSlotNotifier2Imp instance.
     */
    static CPEngNWSessionSlotNotifier2Imp* NewL(
        CPEngNWSessionSlotNotifier2& aInterface,
        TInt aPriority );


    /**
     * Destructor.
     */
    ~CPEngNWSessionSlotNotifier2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngNWSessionSlotNotifier2Imp( CPEngNWSessionSlotNotifier2& aInterface,
                                    TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: /* Session slot notifier implementation */

    /**
     * Implementation.
     * @since 3.0
     */
    void StartL( const CPEngNWSessionSlotID2Imp* aSlotIdFilter,
                 const TArray<TPEngNWSessionSlotEvent>* aEventFilter );

    void Stop();

    TBool IsActive() const;

    TInt PushNWSessionSlotStateToObservers( const CPEngNWSessionSlotID2& aMatchPattern );
    TInt GetNWSessionSlotState( const CPEngNWSessionSlotID2& aNWSessionSlotID,
                                TPEngNWSessionSlotState& aState ) const;



    TInt AddObserver( MPEngNWSessionSlotObserver2& aObserver );

    TInt RemoveObserver( MPEngNWSessionSlotObserver2& aObserver );



private: //Implementation

    /**
     * From MPEngGlobalEventObserver.
     */
    void HandleNewSessionSlotEventL( const CPEngSessionSlotEvent& aSessionEvent );
    void HandleObserverError( TInt aError );


    /**
     * Helpers.
     */
    void DoNotifyNWSessionSlotStatesL(
        const CPEngNWSessionSlotID2Imp& aSlotIdMatcher );

    void DeleteFilters();
    TBool IsEventAccepted( const CPEngNWSessionSlotID2& aNWSessionSlotID,
                           TPEngNWSessionSlotEvent aEvent ) const;
    void MediateNotify( TInt aError, CPEngNWSessionSlotEvent2& aEvent );




private: //Data

    //REF: Implemented interface
    CPEngNWSessionSlotNotifier2& iInterface;

    //OWN: The notifier priority
    TInt    iCActivePriority;

    //OWN: Observer array
    RGenericObserverArray< MPEngNWSessionSlotObserver2 >   iObsArray;

    //OWN: Slot ID filter
    CPEngNWSessionSlotID2Imp*   iSlotIdFilter;

    //OWN: Event filter
    RArray<TPEngNWSessionSlotEvent>* iEventFilter;

    //OWN: Event entry for notifications
    CPEngNWSessionSlotEvent2Copy* iEventEntry;

    //OWN: Flags describing the notifier state
    //     Flag contents are either EFalse or ETrue
    TInt8   iStarted;
    TInt8   iDying;
    TInt8   iSynchNotifyRunning;
    TInt8   iReserved2;

    };

#endif  //CPENGNWSESSIONSLOTNOTIFIER2IMP_H

// End of File
