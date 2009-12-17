/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Editor indicator controller
*
*/


// INCLUDE FILES

#include	"CCAEditIndicator.h"
#include	"CCAMessageEditor.h"

// CONSTANTS
// 3000000(3 sec) is good enough time to view the edit indicator.
// if the indicator display has to be more this value can be increased.
const TInt KMsgIndicatorTimeOut = 3000000;

// -----------------------------------------------------------------------------
// CCAEditIndicator::CCAEditIndicator()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCAEditIndicator::CCAEditIndicator( CCAMessageEditor& aEditor )
        : CActive( 0 ),
        iEditor( aEditor )
    {
    iTimer.CreateLocal();
    CActiveScheduler::Add( this );

    }


// -----------------------------------------------------------------------------
// CCAEditIndicator::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCAEditIndicator* CCAEditIndicator::NewL( CCAMessageEditor& aEditor )

    {
    CCAEditIndicator* self = new( ELeave ) CCAEditIndicator( aEditor );
    return self;
    }



// -----------------------------------------------------------------------------
// CCAEditIndicator::DoCancel()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAEditIndicator::DoCancel()
    {
    iTimer.Cancel();
    }



// -----------------------------------------------------------------------------
// CCAEditIndicator::RunL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAEditIndicator::RunL()
    {
    iEditor.HideIndicator();

    iEditor.ReSetActiveObject();
    iTimer.Cancel();
    }



// -----------------------------------------------------------------------------
// CCAEditIndicator::~CCAEditIndicator()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCAEditIndicator::~CCAEditIndicator()
    {
    iEditor.ReSetActiveObject();
    Cancel();
    iTimer.Close();
    }


// -----------------------------------------------------------------------------
// CCAEditIndicator::StartTimer()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAEditIndicator::StartTimer()
    {
    iEditor.SetActiveObject( this );
    iEditor.ShowIndicator();

    // 3000000(3 sec) is good enough time to view the edit indicator.
    // if the indicator display has to be more this value can be increased.
    iTimer.After( iStatus, TTimeIntervalMicroSeconds32( KMsgIndicatorTimeOut ) );
    if ( !IsActive() )
        {
        SetActive();
        }
    }



// -----------------------------------------------------------------------------
// CCAEditIndicator::IsIndicatorActive()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAEditIndicator::IsIndicatorActive()
    {
    return IsActive();
    }









