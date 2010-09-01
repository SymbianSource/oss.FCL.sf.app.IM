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
* Description:  Notifier API to listen presence contact list changes.
*
*/

#ifndef CPENGCONTACTLISTNOTIFIER2IMP_H
#define CPENGCONTACTLISTNOTIFIER2IMP_H

//  INCLUDES
#include <E32Base.h>
#include <BamDescA.h>
#include <PEngPresenceEngineConsts2.h>
#include "RGenericObserverArray.h"
#include "MPEngSIDChangeObserver.h"



//FORWARD DECLARATIONS
class CPEngContactListNotifier2;
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotID2Imp;
class CPEngNWSessionSlotStorageProxy;
class MPEngStorageManagerWatcher;
class MPEngContactListObserver2;




/**
 * Contact list change notifier implementation
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngContactListNotifier2Imp ) : public CBase,
        public MPEngSIDChangeObserver

    {
public: /* Construction */


    /**
     * Instantiates CPEngContactListNotifier2Imp object.
     *
     * @return New CPEngContactListNotifier2Imp instance.
     */
    static CPEngContactListNotifier2Imp* NewL(
        CPEngContactListNotifier2& aInterface,
        TInt aPriority,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );

    /**
     * Destructor.
     */
    ~CPEngContactListNotifier2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngContactListNotifier2Imp( CPEngContactListNotifier2& aInterface,
                                  TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Session slot notifier implementation */

    /**
     * Implementation.
     * @since 3.0
     */

    TBool IsActive() const;
    TInt Start( const MDesCArray& aContactListNames );
    void Stop();



    TInt Add( const TDesC& aContactListName );
    TInt Remove( const TDesC& aContactListName );

    TInt AddObserver( MPEngContactListObserver2& aObserver );
    TInt RemoveObserver( MPEngContactListObserver2& aObserver );


private: //from MPEngSIDChangeObserver


    /**
     * Handles change of the Storage IDs.
     *
     * @param aChangedSIDs The changed SIDs.
     */
    void HandleSIDsChangeL( CPtrCArray& aChangedSIDs );


    /**
     * Called when SIDs notifier fails.
     *
     * @param aError is the error code.
     */
    void HandleSIDNotifyError( TInt aError );


private: //Implementation

    void MediateNotify( TInt aError, const TDesC& aContactListName );



private: //Data

    //REF: Implemented interface
    CPEngContactListNotifier2& iInterface;

    //OWN: The notifier priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy* iUsedSlot;

    //OWN: Storage watcher
    MPEngStorageManagerWatcher* iWatcher;

    //OWN: Observer array
    RGenericObserverArray< MPEngContactListObserver2 >   iObsArray;

    //OWN: Flags describing the notifier state
    //     Flag contents are either EFalse or ETrue
    TInt8   iStarted;
    TInt8   iDying;
    TInt8   iReserved1;
    TInt8   iReserved2;

    };

#endif  //CPENGCONTACTLISTNOTIFIER2IMP_H

// End of File
