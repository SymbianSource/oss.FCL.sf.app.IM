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
* Description:  Lookup key container.
*
*/

#ifndef __CIMPSSAPLOOKUPKEYVALUEPAIR_H
#define __CIMPSSAPLOOKUPKEYVALUEPAIR_H

//  INCLUDES
#include "CIMPSSAPKeyValuePair.h"


// CLASS DECLARATION
/**
 * Lookup key container.
 *
 * Lookup key holder to be able to do linear order
 * based key-value pair lookups without memory allocation.
 *
 * @since 3.0
 **/
NONSHARABLE_CLASS( CIMPSSAPLookupKeyValuePair ): public CIMPSSAPKeyValuePair
    {
public:  // Constructors and destructor

    /**
     * Two phased constructor.
     *
     * @since 3.0
     */
    static CIMPSSAPLookupKeyValuePair* NewL();

    /**
     * Destructor.
     */
    virtual ~CIMPSSAPLookupKeyValuePair();



private:  // Constructor

    /**
     * C++ default constructor.
     */
    CIMPSSAPLookupKeyValuePair();



public:  //Data access

    /**
     * Key accessor.
     *
     * Gets the look up key associated with
     * this key-value object. Overwritten
     * from base.
     *
     * @since 3.0
     * @return The lookup key.
     */
    const TDesC& Key() const;


    /**
     * Lookup key setter.
     *
     * Sets the look up key associated with
     * this key-value object.
     *
     * @since 3.0
     * @param aLookUpKey The lookup key to use.
     */
    void SetLookupKey( const TDesC& aLookUpKey );


    /**
     * Lookup key reset.
     *
     * Resets the look up key to empty one.
     *
     * @since 3.0
     */
    void ResetLookupKey();


private:    // Data

    TPtrC   iLookupKey;     //OWN: Lookup key

    };

#endif      // __CIMPSSAPLOOKUPKEYVALUEPAIR_H


//  End of File

