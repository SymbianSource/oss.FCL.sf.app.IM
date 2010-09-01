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
* Description:  Notifier API to listen presence reactive authorization changes.
*
*/

#ifndef CPENGREACTAUTHNOTIFIERIMP_H
#define CPENGREACTAUTHNOTIFIERIMP_H

//  INCLUDES
#include <E32Base.h>
#include "RGenericObserverArray.h"
#include "MPEngAuthorizationObserver.h"




//FORWARD DECLARATIONS
class CPEngReactAuthNotifier;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotID2Imp;
class CPEngNWSessionSlotStorageProxy;
class MPEngReactAuthObserver;
class MPEngAuthorizationManager;




/**
 * Reactive authorization change notifier implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngReactAuthNotifierImp ) : public CBase,
        public MPEngAuthorizationObserver

    {
public: /* Construction */


    /**
     * Instantiates CPEngReactAuthNotifierImp object.
     *
     * @return New CPEngReactAuthNotifierImp instance.
     */
    static CPEngReactAuthNotifierImp* NewL(
        CPEngReactAuthNotifier& aInterface,
        TInt aPriority,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );

    /**
     * Destructor.
     */
    ~CPEngReactAuthNotifierImp();



protected:

    /**
     * C++ constructor.
     */
    CPEngReactAuthNotifierImp( CPEngReactAuthNotifier& aInterface,
                               TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Reactive authorization notifier implementation */

    /**
     * Implementation.
     * @since 3.0
     */
    TBool IsActive() const;
    TInt Start( TInt aOptions );
    void Stop();

    TInt AddObserver( MPEngReactAuthObserver& aObserver );
    TInt RemoveObserver( MPEngReactAuthObserver& aObserver );


private: /* From MPEngAuthorizationObserver */


    /**
     * Authorization handler.
     * @since 3.0
     */
    void HandleNewPendingAuthorization(
        MPEngAuthorizationRequest& aNewPendingAuthorization );

    /**
     * Authorization handler.
     * @since 3.0
     */
    void HandlerNewAuthorizationStatus(
        const MPEngAuthorizationStatus& aNewAuthorizationStatus );

    /**
     * Authorization engine update handler.
     */
    void HandleAuthorizationEngineUpdate();


    /**
     * Notification failure handler.
     */
    void HandleAuthorizationEngineError( TInt aError );



private: //Data

    //REF: Implemented interface
    CPEngReactAuthNotifier& iInterface;

    //OWN: The notifier priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy* iUsedSlot;

    //OWN: Observer array
    RGenericObserverArray< MPEngReactAuthObserver >   iObsArray;

    //OWN: Flags describing the notifier state
    //     Flag contents are either EFalse or ETrue
    TInt8   iStarted;
    TInt8   iDying;
    TInt8   iReserved1;
    TInt8   iReserved2;

    MPEngAuthorizationManager*  iAuthManager;

    };

#endif  //CPENGREACTAUTHNOTIFIERIMP_H

// End of File
