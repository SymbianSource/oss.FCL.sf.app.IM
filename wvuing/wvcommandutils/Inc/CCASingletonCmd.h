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
* Description:  Singleton base class
*
*/



#ifndef CCASINGLETONCMD_H
#define CCASINGLETONCMD_H

//  INCLUDES
#include <e32base.h>


// FUNCTION PROTOTYPES
/**
* Create singleton instance.
*
* @return pointer to singleton instance
*/
TAny* ConstructSingletonInstanceL();

// CLASS DECLARATION

/**
*  Singleton base class.
*
*  @lib N/A
*
*/
NONSHARABLE_CLASS( CCASingletonCmd ) : public CBase
    {
public:  // Constructors and destructor


    /**
    * Destructor.
    */
    virtual ~CCASingletonCmd();

protected:


    /**
    * C++ default constructor.
    */
    CCASingletonCmd();

protected: // New functions

    /**
    * Return instance.
    *
    * @return pointer to singleton instance
    */
    static TAny* BaseInstanceL();

    };

#endif      // CCASINGLETONCMD_H

// End of File
