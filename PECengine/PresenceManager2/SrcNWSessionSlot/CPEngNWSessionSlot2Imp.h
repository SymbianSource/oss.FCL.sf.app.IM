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
* Description:  NWSessionSlot handle implementation.
*
*/

#ifndef CPENGNWSESSIONSLOT2IMP_H
#define CPENGNWSESSIONSLOT2IMP_H

//  INCLUDES
#include "MPEngNWSessionOperationOwner.h"

#include <E32Base.h>
#include <BaDesCa.h>
#include <PEngPresenceEngineConsts2.h>



//FORWARD DECLARATIONS
class MPEngNWSessionOperationObserver2;
class CPEngNWSessionSlot2;
class CPEngNWSessionSlotID2;
class CPEngNWSessionOperation;
class CIMPSSAPSettings;
class CPEngNWSessionSlotStorageProxy;
class CPEngNWSessionProxy;



/**
 * NWSessionSlot handle implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngNWSessionSlot2Imp ) : public CBase,
        public MPEngNWSessionOperationOwner

    {
public: /* Construction */


    /**
     * Instantiates CPEngNWSessionSlot2Imp object.
     *
     * @return New CPEngNWSessionSlot2Imp instance.
     */
    static CPEngNWSessionSlot2Imp* NewL(
        CPEngNWSessionSlot2& aInterface,
        TInt aPriority,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    ~CPEngNWSessionSlot2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngNWSessionSlot2Imp( CPEngNWSessionSlot2& aInterface,
                            TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* NWSessionSlot handle implementation. */

    /**
     * Implementation.
     * @since 3.0
     */
    TInt GetNWSessionSlotID( CPEngNWSessionSlotID2& aNWSessionSlotID ) const;

    TInt GetOption( TUint aOptionName, TUint aOptionLevel, TDes8& aOption );

    TBool IsOpenNWPresenceSessionActive() const;
    TInt OpenNWPresenceSession( const CIMPSSAPSettings& aSAP,
                                MPEngNWSessionOperationObserver2& aObserver );
    void CancelOpenNWPresenceSession();


    TBool IsCloseNWPresenceSessionActive() const;
    TInt CloseNWPresenceSession( MPEngNWSessionOperationObserver2& aObserver,
                                 TBool aForceClose );
    void CancelCloseNWPresenceSession();
    TInt OpenNWPresenceSessionOwnership();
    void CloseNWPresenceSessionOwnership();


    TInt StoreNWPresenceSessionOwnership( const TDesC16& aOwnerID );
    TInt RestoreNWPresenceSessionOwnership( const TDesC16& aOwnerID );

    TInt GetNWPresenceSessionSap( CIMPSSAPSettings& aSAP ) const;
    TInt GetNWSessionSlotState( TPEngNWSessionSlotState& aState ) const;



private: //from MPEngNWSessionOperationOwner

    /**
     * Notifies from session slot operation destruction.
     * @since 3.0
     * @param aOperation The dying operation.
     */
    void HandleNWSessionOperationDestruction( CPEngNWSessionOperation* aOperation );


private: //Helpers

    void DoGetNWPresenceSessionSapL( CIMPSSAPSettings& aSAP ) const;


private: //Data

    //REF: Implemented interface
    CPEngNWSessionSlot2& iInterface;

    //OWN: The notifier priority
    TInt    iActivePriority;

    //OWN: The Storage proxy for used slot
    CPEngNWSessionSlotStorageProxy* iUsedSlot;

    //OWN: Network session proxy
    CPEngNWSessionProxy*    iNWSessionProxy;

    //OWN:
    CPEngNWSessionOperation* iSessionOpenOp;
    CPEngNWSessionOperation* iSessionCloseOp;

    };

#endif  //CPENGNWSESSIONSLOT2IMP_H

// End of File
