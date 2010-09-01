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
* Description: SAP settings observer test cases
*
*/


//  Include Files
#include "WVSAPSettingsObserverTestCases.h"
#include "CIMPSSAPSettingsStore.h"
#include "CIMPSSAPSettings.h"
#include "CIMPSSAPSettingsList.h"
#include "IMPSSAPSettingsStoreTestUtils.h"
#include "CObserverWait.h"

#include <e32err.h>

enum TThreadOps
    {
    EThreadOp_AccessStore,
    EThreadOp_QueryDefault,
    EThreadOp_AddSAP,
    EThreadOp_SeveralTransactions,
    EThreadOp_ComplexTransactionsEncapsulated,
    EThreadOp_ComplexTransactionsEncapsulatedRollBack
    };


// CONSTRUCTION
// Static constructor
WVSAPSettingsObserverTestCases* WVSAPSettingsObserverTestCases::NewLC()
    {
    WVSAPSettingsObserverTestCases* self = new( ELeave ) WVSAPSettingsObserverTestCases;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

WVSAPSettingsObserverTestCases* WVSAPSettingsObserverTestCases::NewL()
    {
    WVSAPSettingsObserverTestCases* self = WVSAPSettingsObserverTestCases::NewLC();
    CleanupStack::Pop();
    return self;
    }

// Destructor (virtual by CBase)
WVSAPSettingsObserverTestCases::~WVSAPSettingsObserverTestCases()
    {
    delete iStore;

    delete iSettingsEmpty;
    delete iSettingsFilled;
    delete iDefaultSAP;
    delete iLoggedInSAP;


    delete iObsWait;
    }

// Second phase construct
void WVSAPSettingsObserverTestCases::ConstructL()
    {
    iSettingsEmpty = CIMPSSAPSettings::NewL();

    iSettingsFilled = CIMPSSAPSettings::NewL();
    IMPSSAPSettingsStoreTestUtils::FillContainerL( *iSettingsFilled, _L( "SettingsFilled" ) );

    iDefaultSAP = CIMPSSAPSettings::NewL();
    IMPSSAPSettingsStoreTestUtils::FillContainerL( *iDefaultSAP, _L( "DefaultSAP" ) );

    iLoggedInSAP = CIMPSSAPSettings::NewL();
    IMPSSAPSettingsStoreTestUtils::FillContainerL( *iLoggedInSAP, _L( "LoggedInSAP" ) );

    iObsWait = CObserverWait<TBool, TInt>::NewL();

    }



WVSAPSettingsObserverTestCases::WVSAPSettingsObserverTestCases()
    {
    iSAPNameIndex = 0;
    }




//---------------------------------------------------------------
//----------------- TEST CASE METHODS ---------------------------
//---------------------------------------------------------------
//Observer Test cases to add SAP and Remove SAP.

TInt WVSAPSettingsObserverTestCases::TestNotify_AddSapL()
    {
    ResetTheObsState();

    /* IMPSSAPSettingsStoreTestUtils::UnigueSAPNameL( iSAPNameIndex,
                                             *iSettingsFilled ); */

    //Create the Store
    iStore = CIMPSSAPSettingsStore::NewL();

    IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    //Add the observer
    iStore->AddObserverL( this, EIMPSPECAccessGroup );

    IMPSSAPSettingsStoreTestUtils::UnigueSAPNameL( iSAPNameIndex,
                                                   *iSettingsFilled );

    //Do the operation for store
    iStore->StoreNewSAPL( iSettingsFilled, EIMPSPECAccessGroup );

    //Start the timer, expect HandleSAPEvent to be called.
    iObsWait->Wait( 100000 ); //wait or timeout

    RDebug::Print(  _L( "First event: %d" ), iObsWait->Value2() );

    iObsWait->Value1();
    if ( iObsWait->Value2() != ESAPSettingChanged )
        {
        //There was an error, just return from the function.
        return KErrArgument;
        }

    WaitQuedEvents( 1 );

    //Get the SAP count after deletion of SAPs
    TInt count = iStore->SAPCountL( EIMPSPECAccessGroup );

    //Delete the observer
    iStore->RemoveObserver( this );

    //Check the count of SAPs added :
    if ( count == 1 )
        {
        //StoreNewSAPL operation was successfull
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }

    }


TInt WVSAPSettingsObserverTestCases::TestNotify_RemoveSapL()
    {
    ResetTheObsState();

    //Create the Store
    iStore = CIMPSSAPSettingsStore::NewL();

    // Add the observer
    iStore->AddObserverL( this, EIMPSPECAccessGroup );

    //Delete the previuosly existing SAP settings
    IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    //Add the SAP
    IMPSSAPSettingsStoreTestUtils::AddSAPSettingsL( *iStore,
                                                    2,
                                                    iExistingUid1,
                                                    iExistingUid2,
                                                    EIMPSPECAccessGroup );
    //Delete the SAPs added
    iStore->DeleteSAPL( iExistingUid1 );
    iObsWait->Wait( 100000 ); //wait or timeout

    RDebug::Print(  _L( "First event: %d" ), iObsWait->Value2() );

    iObsWait->Value1();
    if ( iObsWait->Value2() != ESAPSettingChanged )
        {
        //There was an error, return from the function.
        return KErrArgument;
        }

    WaitQuedEvents( 1 );

    //Get the SAP count after deletion of SAPs
    TInt count = iStore->SAPCountL( EIMPSPECAccessGroup );

    //Delete the observer
    iStore->RemoveObserver( this );

    if ( count != 1 ) //deletion did not happen.
        //we created 2 SAPs and deleted one SAP, the count of number of SAPs on the store must be 1.
        {
        return KErrGeneral ;
        }
    else
        {
        return KErrNone;
        }
    }

//Implementation of the HandleSAPEvent function, the observer interface

void WVSAPSettingsObserverTestCases::HandleSAPEvent( TSAPEvent aEvent )
    {
    iObsWait->SetValue1( ETrue ); //Event is really called
    iObsWait->SetValue2( aEvent );
    iObsWait->Completed();//now the wait will return
    RDebug::Print( _L( "SAP Event occurred!" ) );
    }

//Implementation of the private helper functions
void WVSAPSettingsObserverTestCases::ResetTheObsState()
    {
    iObsWait->SetValue1( EFalse );
    iObsWait->SetValue2( KErrNotFound );
    }


void WVSAPSettingsObserverTestCases::WaitQuedEvents( TInt aEventCountStart )
    {
    FOREVER
        {
        ResetTheObsState();
        if ( iObsWait->Wait( 5000000 ) )
            {
            RDebug::Print(  _L( "Queued event[%d]: %d" ), aEventCountStart,
                            iObsWait->Value2() );
            aEventCountStart++;
            }
        else
            {
            return;
            }
        }
    }

TInt WVSAPSettingsObserverTestCases::CountQueuedEvents( TInt aEventCountStart )
    {
    FOREVER
        {
        ResetTheObsState();
        if ( iObsWait->Wait( 1000000 ) )
            {
            RDebug::Print(  _L( "Queued event[%d]: %d" ), aEventCountStart,
                            iObsWait->Value2() );
            aEventCountStart++;
            }
        else
            {
            return aEventCountStart;
            }
        }
    }

//  End of File

