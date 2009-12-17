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
* Description:  Notifier implementation to listen presence changes.
*
*/

#ifndef CPENGPRESENCENOTIFIER2IMP_H
#define CPENGPRESENCENOTIFIER2IMP_H

//  INCLUDES
#include "RGenericObserverArray.h"
#include "MPEngSIDChangeObserver.h"
#include <E32Base.h>





//FORWARD DECLARATIONS
class CPEngPresenceNotifier2;
class CPEngNWSessionSlotStorageProxy;
class CPEngTrackedPresenceIDs2;
class CPEngTrackedPresenceIDCollection;
class CPEngNWSessionSlotID2;
class MPEngPresenceObserver2;
class MPEngContactList2;
class MPEngPresenceAttrManager;
class MPEngStorageManagerWatcher;


/**
 * Notifier API implementation for presence attributes.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngPresenceNotifier2Imp ): public CBase,
        public MPEngSIDChangeObserver
    {
public: /* Construction */


    /**
     * Instantiates CPEngPresenceNotifier2Imp object.
     *
     * @return New CPEngPresenceNotifier2Imp instance.
     */
    static CPEngPresenceNotifier2Imp* NewL(
        CPEngPresenceNotifier2& aInterface,
        TInt aPriority,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    ~CPEngPresenceNotifier2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngPresenceNotifier2Imp( CPEngPresenceNotifier2& aInterface,
                               TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Attribute notifier implementation */

    /**
     * Implementation.
     * @since 3.0
     */
    TBool IsActive() const;

    TInt Start( const MDesCArray& aPresenceIDs,
                const TArray<TUint32>& aTypes );

    void Stop();



    /**
     * Implementation.
     * @since 3.0
     */
    TInt Update( const MDesCArray& aPresenceIDs,
                 const TArray<TUint32>* aTypes,
                 TBool aKeepOldTrackeds );
    TInt Remove( const TDesC& aPresenceID );
    TInt Remove( TUint32 aType );

    CPEngTrackedPresenceIDs2& TrackedPresenceIDs();

    TInt AddObserver( MPEngPresenceObserver2& aObserver );
    TInt RemoveObserver( MPEngPresenceObserver2& aObserver );


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


    void GenerateSidsL( CDesCArray*& aGeneratedSids,
                        const MDesCArray& aPresenceIDs );
    void AddTrackedAttributesL( const TArray<TUint32>& aTypes );
    TBool RemoveTrackedAttributes( const TArray<TUint32>& aTypes );
    void MediateNotify( TInt aError );
    void UpdateWatcherL( CDesCArray*& aSidsList,
                         const MDesCArray& aPresenceIDs,
                         const TArray<TUint32>& aTypes,
                         TBool aKeepOldTrackeds );


    /**
     * Notify mediator.
     * @since 3.0
     */
    NONSHARABLE_CLASS( TPresenceNotifyMediator ) :
            public MGenObserverNotifyMediator< MPEngPresenceObserver2 >
        {
public: //Constructor
        TPresenceNotifyMediator( CPEngPresenceNotifier2& aNotifier,
        CPEngTrackedPresenceIDCollection& aPresenceIds );

private: //From MGenObserverNotifyMediator

        void MediateNotifyL( MPEngPresenceObserver2& aObserverToNotify );
        void MediateNotifyError( MPEngPresenceObserver2& aObserverToNotify,
                                 TInt aLeaveError );

private: //Data

        //REF: The notifier argument
        CPEngPresenceNotifier2& iNotifier;

        //REF: Tracked presence Ids argument
        CPEngTrackedPresenceIDCollection& iPresenceIds;
        };


private: //Data

    //REF: Implemented interface
    CPEngPresenceNotifier2& iInterface;

    //OWN: The notifier priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy* iUsedSlot;

    //OWN: Attribute manager
    MPEngPresenceAttrManager*   iAttrManager;

    //OWN: Storage watcher
    MPEngStorageManagerWatcher* iWatcher;

    //OWN: Collection of tracked presence IDs
    CPEngTrackedPresenceIDCollection*  iTrackedPresenceIDs;

    //OWN: Tracked attribute types
    RArray< TUint32 >   iTrackedAttributes;

    //OWN: Observer array
    RGenericObserverArray< MPEngPresenceObserver2 >   iObsArray;

    //OWN: Flags describing the notifier state
    //     Flag contents are either EFalse or ETrue
    TInt8   iStarted;
    TInt8   iDying;
    TInt8   iReserved1;
    TInt8   iReserved2;

    };

#endif  //CPENGPRESENCENOTIFIER2IMP_H

// End of File
