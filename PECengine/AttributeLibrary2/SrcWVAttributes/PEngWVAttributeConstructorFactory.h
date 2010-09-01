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
* Description:  Wireless Village attribute constructor factory.
*
*/

#ifndef PENGWVATTRIBUTECONSTRUCTORFACTORY_H
#define PENGWVATTRIBUTECONSTRUCTORFACTORY_H

//  INCLUDES
#include <E32std.h>


// FORWARD DECLARATIONS
class CPEngAttrConstructorCon;
class MPEngPresenceAttrModel2;
class MPEngPresenceAttrManager;



// CLASS DECLARATION
/**
 * Wireless Village attribute constructor factory.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( PEngWVAttributeConstructorFactory )
    {
public: // New functions

    /**
     * Creates Wireless Village attribute constructors and
     * adds the to provided constructor container.
     *
     * @since 3.0
     * @param aModelPrototypes The array where to generate
     * model prototypes.
     */
    static void CreateConstructorsL( CPEngAttrConstructorCon& aConstructors );


    /**
     * Creates a attribute instance of those Wireless Village
     * attributes that need session tracing
     *
     * @since 3.0
     * @param aSessionAttributes On the return contains the
     * session tracing attributes. Ownership is returned
     * to caller.
     * @param aAttributeManager The Attribute manager to use
     * to create session tracing attributes.
     */
    static void LoadSessionTracingAttributesL( RPointerArray< MPEngPresenceAttrModel2 >& aSessionAttributes,
                                               MPEngPresenceAttrManager& aAttributeManager );


protected:  // Prohibited constructor & destructor

    PEngWVAttributeConstructorFactory();
    ~PEngWVAttributeConstructorFactory();
    };

#endif      // PENGWVATTRIBUTECONSTRUCTORFACTORY_H

//  End of File

