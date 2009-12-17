/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Utility class which used in logging adapter for counting time delay when calling functions
*
*/



// INCLUDE FILES
#include    "TCADelay.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TCADelay::TCADelay
// C++ default constructor can NOT contain any code, that
// might leave.
// Constructor registers the time of construction.
// -----------------------------------------------------------------------------
//
TCADelay::TCADelay()
    {
    iStart.HomeTime();
    }

// -----------------------------------------------------------------------------
// TCADelay::StopAndReturn
// Calculates time difference between construction and a call to this function.
// Return TInt in microseconds.
// -----------------------------------------------------------------------------
//
TInt TCADelay::StopAndReturn()
    {
    iStop.HomeTime();
    return iStop.MicroSecondsFrom( iStart ).Int64().GetTInt();
    }


//  End of File
