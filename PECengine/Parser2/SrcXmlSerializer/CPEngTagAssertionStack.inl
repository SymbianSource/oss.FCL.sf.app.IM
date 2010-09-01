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
* Description:  Assertion Stack implementation.
*
*/

// -----------------------------------------------------------------------------
// CPEngTagAssertionStack::NewL()
// Constructor.
// -----------------------------------------------------------------------------
//
inline CPEngTagAssertionStack* CPEngTagAssertionStack::NewL( TInt aGranularity )
    {
    CPEngTagAssertionStack* self = new (ELeave) CPEngTagAssertionStack;
    CleanupStack::PushL( self );
    self->iStack = new (ELeave) CPtrC8Array( aGranularity ); //construct stack here inline,
                                                             //avoiding need of ConstructL()
    CleanupStack::Pop(); //self

    return self;
    }


// -----------------------------------------------------------------------------
// CPEngTagAssertionStack::CPEngTagAssertionStack()
// Constructor.
// -----------------------------------------------------------------------------
//
inline CPEngTagAssertionStack::CPEngTagAssertionStack()
    {
    }


// -----------------------------------------------------------------------------
// CPEngTagAssertionStack::~CPEngTagAssertionStack()
// Destructor.
// -----------------------------------------------------------------------------
//
inline CPEngTagAssertionStack::~CPEngTagAssertionStack()
    {
    delete iStack;
    }


// -----------------------------------------------------------------------------
// CPEngTagAssertionStack::ReserveOneL()
// Reserves space for one pushed element.
// -----------------------------------------------------------------------------
//
inline void CPEngTagAssertionStack::ReserveOneL()
    {
    iStack->SetReserveL( iStack->Count() + 1 ); //reserve space for one new element
    }


// -----------------------------------------------------------------------------
// CPEngTagAssertionStack::PushL()
// Pushes a tag to stack.
// -----------------------------------------------------------------------------
//
inline void CPEngTagAssertionStack::PushL( const TPtrC8& aTag )
    {
    iStack->AppendL( aTag );
    }



// -----------------------------------------------------------------------------
// CPEngTagAssertionStack::Pop()
// Pops the topmost tag from stack.
// -----------------------------------------------------------------------------
//
inline void CPEngTagAssertionStack::Pop()
    {
    if( !Empty() )
        {
        iStack->Delete( iStack->Count() - 1 );  //delete the top item (thus -1)
        }
    }


// -----------------------------------------------------------------------------
// CPEngTagAssertionStack::Top()
// Gets the topmost tag from the stack.
// -----------------------------------------------------------------------------
//
inline TPtrC8 CPEngTagAssertionStack::Top() const
    {
    if( !Empty() )
        {
        return (*iStack)[ iStack->Count() - 1 ]; //get top item (thus -1)
        }
    else
        {
        return TPtrC8( NULL, 0 );
        }
    }


// -----------------------------------------------------------------------------
// CPEngTagAssertionStack::Count()
// Gets the count of tags in the stack.
// -----------------------------------------------------------------------------
//
inline TInt CPEngTagAssertionStack::Count() const
    {
    return iStack->Count();
    }


// -----------------------------------------------------------------------------
// CPEngTagAssertionStack::Empty()
// Test is the stack empty.
// -----------------------------------------------------------------------------
//
inline TBool CPEngTagAssertionStack::Empty() const
    {
    return ( iStack->Count() == 0 ); //stack is empty if the count is zero...
    }



// -----------------------------------------------------------------------------
// CPEngTagAssertionStack::PopTo()
// Pops topmost elements until the stack count is required one.
// -----------------------------------------------------------------------------
//
inline void CPEngTagAssertionStack::PopTo( TInt aCount )
    {
    TInt popCount = iStack->Count() - aCount; // how many to pop
    if( popCount > 0 )
        {
        TInt startIndex = iStack->Count() - popCount;
        iStack->Delete( startIndex, popCount );
        }
    }


// End of file

