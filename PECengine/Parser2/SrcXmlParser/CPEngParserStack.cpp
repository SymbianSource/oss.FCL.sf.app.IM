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
* Description:  Stack to hold the data start and end markers
*
*/

// INCLUDE FILES
#include "CPEngParserStack.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngParserStack::CPEngParserStack
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngParserStack::CPEngParserStack()
    {
    }

// -----------------------------------------------------------------------------
// CPEngParserStack::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngParserStack* CPEngParserStack::NewL()
    {
    CPEngParserStack* self = new( ELeave ) CPEngParserStack;
    return self;
    }


// Destructor
CPEngParserStack::~CPEngParserStack()
    {
    iStartArray.Reset();
    iEndArray.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngParserStack::PushStartL
// Pushes a start element to the stack
// -----------------------------------------------------------------------------
//
TInt CPEngParserStack::PushStart( TUint aItem )
    {
    return iStartArray.Append( aItem );
    }

// -----------------------------------------------------------------------------
// CPEngParserStack::PushEndL
// Pushes an end element to the stack
// -----------------------------------------------------------------------------
//
TInt CPEngParserStack::PushEnd( TUint aItem )
    {
    return iEndArray.Append( aItem );
    }

// ---------------------------------------------------------
// CPEngParserStack::PopStart
// Pops a start element from the stack
// ---------------------------------------------------------
//
TUint CPEngParserStack::PopStart()
    {
    TUint pos( iStartArray.Count() - 1 ); // take the last one
    TUint returnable( iStartArray[pos] );

    iStartArray.Remove( pos );

    return returnable;
    }

// ---------------------------------------------------------
// CPEngParserStack::PopEnd
// Pops an end element from the stack
// ---------------------------------------------------------
//
TUint CPEngParserStack::PopEnd()
    {
    TUint pos( iEndArray.Count() - 1 ); // take the last one
    TUint returnable( iEndArray[pos] );

    iEndArray.Remove( pos );

    return returnable;
    }

// ---------------------------------------------------------
// CPEngParserStack::Pop
// Pops the start&end markers pair from the stack.
// ---------------------------------------------------------
//
TPoint CPEngParserStack::Pop()
    {
    TPoint returnablePoint;

    if ( ( iStartArray.Count() > 0 ) &&
         ( iEndArray.Count() > 0 ) )
        {
        returnablePoint.iX = PopStart();
        returnablePoint.iY = PopEnd();
        }
    else    // something wrong with the array, set negative values
        {   // shouldn't happen, because the WV engine checks the XML
        returnablePoint.iX = KErrCorrupt;
        returnablePoint.iY = KErrCorrupt;
        }

    return returnablePoint;
    }

// ---------------------------------------------------------
// CPEngParserStack::Reset
// Resets the element arrays
// ---------------------------------------------------------
//
void CPEngParserStack::Reset()
    {
    iEndArray.Reset();
    iStartArray.Reset();
    }

//  End of File
