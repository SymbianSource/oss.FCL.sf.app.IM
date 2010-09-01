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
* Description:  Presence search transactions.
*
*/

#ifndef CPENGSEARCHTRANSACTION2IMP_H
#define CPENGSEARCHTRANSACTION2IMP_H

//  INCLUDES
#include <E32Base.h>
#include <CPEngSearchTransaction2.h>


//FORWARD DECLARATIONS
class CPEngSearchTransaction2;
class CPEngNWSessionSlotID2;
class CPEngSearchControl;
class CPEngSearchCriteria2;
class CPEngNWSessionSlotStorageProxy;


/**
 * Presence attribute transactions.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSearchTransaction2Imp ): public CBase

    {
public: /* Construction */


    /**
     * Instantiates CPEngSearchTransaction2Imp object.
     *
     * @return New CPEngSearchTransaction2Imp instance.
     */
    static CPEngSearchTransaction2Imp* NewL(
        CPEngSearchTransaction2& aInterface,
        TInt aPriority,
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    ~CPEngSearchTransaction2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngSearchTransaction2Imp( CPEngSearchTransaction2& aInterface,
                                TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );



public: /* Seacrh transaction implementation */

    TBool IsSearchFromNetworkActive() const;

    TInt SearchFromNetwork( CPEngSearchCriteria2*& aCriteria,
                            TInt aSearchLimit,
                            MPEngSearchTransactionObserver2& aObserver );

    TInt SearchFromNetwork( RPointerArray< CPEngSearchCriteria2 >& aCriterias,
                            TInt aSearchLimit,
                            MPEngSearchTransactionObserver2& aObserver );

    TInt ContinueSearchFromNetwork( TInt aContinueIndex );

    TInt StopSearchFromNetwork();

    void HandleSearchDestruction( CPEngSearchControl* aSearch );

    CPEngSearchTransaction2& Interface();


private: //Helpers





private: //Implementation



private: //Data

    //REF: Implemented interface
    CPEngSearchTransaction2& iInterface;

    //OWN: The priority
    TInt    iCActivePriority;

    //OWN: Slot connection to bind to the storage.
    CPEngNWSessionSlotStorageProxy* iUsedSlot;

    //OWN: Cached slot id for search control
    CPEngNWSessionSlotID2* iUsedSlotId;

    //OWN: The search operation
    CPEngSearchControl*    iSearchCntrl;


    };

#endif  //CPENGSEARCHTRANSACTION2IMP_H

// End of File
