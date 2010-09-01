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
* Description:  API implementation to manage NWSessionSlots.
*
*/

#ifndef CPENGNWSESSIONSLOTMANAGER2IMP_H
#define CPENGNWSESSIONSLOTMANAGER2IMP_H

//  INCLUDES
#include <E32Base.h>
#include <bamdesca.h>
#include <PEngPresenceEngineConsts2.h>


//FORWARD DECLARATIONS
class CPEngNWSessionSlotManager2;
class CPEngSessionSlotId;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotID2Imp;


/**
 * API implementation to manage NWSessionSlots.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngNWSessionSlotManager2Imp ) : public CBase
    {
public: /* Construction */


    /**
     * Instantiates CPEngNWSessionSlotManager2Imp object.
     *
     * @return New CPEngNWSessionSlotManager2Imp instance.
     */
    static CPEngNWSessionSlotManager2Imp* NewL(
        CPEngNWSessionSlotManager2& aInterface,
        TInt aPriority );


    /**
     * Destructor.
     */
    ~CPEngNWSessionSlotManager2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngNWSessionSlotManager2Imp( CPEngNWSessionSlotManager2& aInterface,
                                   TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: /* NWSessionSlot management implementation */

    /**
     * Implementation.
     * @since 3.0
     */
    TInt CreateNWSessionSlot(
        const CPEngNWSessionSlotID2& aNWSessionSlotID );

    TInt RemoveNWSessionSlot(
        const CPEngNWSessionSlotID2& aNWSessionSlotID );

    void GetNWSessionSlotsL( RPointerArray<CPEngNWSessionSlotID2>& aNWSessionSlotIDs,
                             const CPEngNWSessionSlotID2* aMatchPattern,
                             TPEngNWSessionSlotState* aMatchState ) const;

    void ExistsNWSessionSlotL( const CPEngNWSessionSlotID2& aNWSessionSlotID ) const;


private: //Implementation



    void DoGenerateMatchingSlotIdsL( RPointerArray<CPEngNWSessionSlotID2>& aNWSessionSlotIDs,
                                     CPEngSessionSlotId& aSessionSlotBaseId,
                                     const MDesC16Array& aRegApps,
                                     const MDesC16Array& aActiveApps,
                                     const CPEngNWSessionSlotID2* aSlotIdMatcher,
                                     TPEngNWSessionSlotState* aSlotStateMatcher ) const;





private: //Data

    //REF: Implemented interface
    CPEngNWSessionSlotManager2& iInterface;

    //OWN: The priority
    TInt    iCActivePriority;


    };

#endif  //CPENGNWSESSIONSLOTMANAGER2IMP_H

// End of File
