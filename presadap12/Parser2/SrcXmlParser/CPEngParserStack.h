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

#ifndef CPENGPARSERSTACK_H
#define CPENGPARSERSTACK_H

//  INCLUDES
#include <e32base.h>

/**
 * Helper class to hold the parsing intervals in a stack
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngParserStack ): public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngParserStack* NewL();

    /**
     * Destructor.
     */
    virtual ~CPEngParserStack();

public: // New functions

    /**
     * Pushes an item marking the start of the data to the stack
     * @since 2.1
     * @return Append return value KErrNone or some error
     * @param aItem Item to be pushed
     */
    TInt PushStart( TUint aItem );

    /**
     * Pushes an item marking the end of the data to the stack
     * @since 2.1
     * @return Append return value KErrNone or some error
     * @param aItem Item to be pushed
     */
    TInt PushEnd( TUint aItem );

    /**
     * Pops both, the start marker and the end marker out of the stack. Both
     * is needed so the stack remains consistent.
     * @since 2.1
     * @return TPoint A popped item
     */
    TPoint Pop();

    /**
     * Resets the stack.
     */
    void Reset();

private:

    /**
     * Pops an item marking the start of the data to the stack
     * @since 2.1
     * @return TUint A popped item
     */
    TUint PopStart();

    /**
     * Pops an item marking the end of the data to the stack
     * @since 2.1
     * @return TUint A popped item
     */
    TUint PopEnd();

private:

    /**
     * C++ default constructor.
     */
    CPEngParserStack();


private:    // Data
    // The array of start markers
    RArray<TUint> iStartArray;

    // The array of end markers
    RArray<TUint> iEndArray;
    };

#endif      // CPENGPARSERSTACK_H

// End of File
