/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Tag assertion stack.
*
*/

#ifndef __CPENGTAGASSERTIONSTACK_H__
#define __CPENGTAGASSERTIONSTACK_H__

//INCLUDE FILES
#include <E32Base.h>
#include <Badesca.h>


/**
 * Tag assertion stack.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngTagAssertionStack ) : public CBase
    {
public: //Two phased constructor & destructor

    /**
     * Two phased constructor.
     *
     * @since 2.1
     */
    static inline CPEngTagAssertionStack* NewL( TInt aGranularity );


    /**
     * Destructor.
     *
     * Frees all resources owned by the
     * assetions stack instance.
     *
     * @since 2.1
     */
    inline ~CPEngTagAssertionStack();


public: //New methods

    /**
     * Reserves space for one pushed element.
     * After a call to this, next PushL() won't fail.
     *
     * @since 2.1
     */
    inline void ReserveOneL();

    /**
     * Pushes a tag to stack.
     *
     *
     * @since 2.1
     * @param aTag Tag to push.
     */
    inline void PushL( const TPtrC8& aTag );

    /**
     * Pops the topmost tag from stack.
     *
     * @since 2.1
     */
    inline void Pop();

    /**
     * Gets the topmost tag from the stack.
     * (Just gets the tag, doesn't affect to
     * count of tags in the stack.)
     *
     * @since 2.1
     * @return The topmost tag.
     */
    inline TPtrC8 Top() const;

    /**
     * Gets the count of tags in the stack.
     *
     * @since 2.1
     * @return The stack count.
     */
    inline TInt Count() const;

    /**
     * Test is the stack empty.
     *
     * @since 2.1
     * @return ETrue if the stack is empty, else EFalse.
     */
    inline TBool Empty() const;

    /**
     * Pops topmost elements until the stack count is
     * required one.
     *
     * @since 2.1
     * @param aCount Required stack end count.
     */
    inline void PopTo( TInt aCount );


private: //Constructor
    inline CPEngTagAssertionStack();

private: //Data
    CPtrC8Array* iStack;        ///<Actual stack, owned
    };


//Get inline implementation
#include "CPEngTagAssertionStack.inl"


#endif // __CPENGTAGASSERTIONSTACK_H__
