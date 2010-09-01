/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Resource reader
*
*/


#ifndef __CIMPSRESOURCEREADER_H
#define __CIMPSRESOURCEREADER_H


//  INCLUDES
#include <E32Base.h>
#include <barsc.h>

// CLASS DECLARATION
NONSHARABLE_CLASS( CIMPSResourceReader ) : public CBase
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CIMPSResourceReader* NewL();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CIMPSResourceReader* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CIMPSResourceReader();

public:

    /**
    * Reads boolean value from the resource
    * @return value of resource flag for  warning
    */
    IMPORT_C TBool IsRoamingWarningRequiredL() ;

protected: //protected to allow derivation

    /**
     * C++ constructor.
     */
    CIMPSResourceReader();

    /**
     * Symbian OS constructor.
     */
    void ConstructL();

private : // New functions

    /**
     * Reads integer resource value from resources
     * @param aResourceId resource id which is read
     * @return value of wanted integer resource flag
     */
    TInt IntResourceValueL( TInt aResourceId );

private: // private data

    // File server session
    RFs iFs;

    // IMPS resource file, owned
    RResourceFile iResFile;
    };

#endif // __CIMPSRESOURCEREADER_H