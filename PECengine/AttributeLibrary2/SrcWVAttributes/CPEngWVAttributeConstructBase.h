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
* Description:  Base class for WV attribute constructors.
*
*/

#ifndef CPENGWVATTRIBUTECONSTRUCTBASE_H
#define CPENGWVATTRIBUTECONSTRUCTBASE_H

//  INCLUDES
#include <E32Base.h>
#include "PEngWVCspVersion.h"
#include "MPEngPresenceAttrConstructorTypeImp.h"


//DATA TYPES

/**
 * Attribute WV CSP name entry.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( TWVCspAttributeNameEntry )
    {
public: //Data

    //OWN: CSP version id
    TPEngWVCspVersion   iCspVersion;

    //OWN: Attribute name for CSP version
    const TDesC8*   iName;

    //OWN: Attribute namespace for CSP version
    const TDesC8*   iNameSpace;
    };


/**
 * Helper macro for initializing
 * TWVCspAttributeNameEntry with standard _LIT8.
 *
 * @since 3.0
 */
#define LIT_AS_DESC8_PTR(aLit) (reinterpret_cast<TDesC8 const*>(&aLit.iTypeLength))





// Class definition

/**
 * Base class for WV attribute constructors.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngWVAttributeConstructBase ) : public CBase,
        public MPEngPresenceAttrConstructorTypeImp
    {
public:  // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CPEngWVAttributeConstructBase();


    /**
     * C++ default constructor.
     */
    CPEngWVAttributeConstructBase( TPEngWVCspVersion aCurrentCspVer,
                                   const TWVCspAttributeNameEntry* aNameTable,
                                   TInt aTableCount );



public: // Functions from MPEngPresenceAttrConstructorTypeImp

    /**
     * From MPEngPresenceAttrConstructorTypeImp.
     *
     * @since 3.0
     */
    TInt GetProperty( TUint aPropertyName,
                      TUint aPropertyLevel,
                      TInt& aProperty ) const;
    TInt GetProperty( TUint aPropertyName,
                      TUint aPropertyLevel,
                      TDes8& aProperty ) const;
    TInt GetPropertyPtr( TUint aPropertyName,
                         TUint aPropertyLevel,
                         TPtrC16& aProperty ) const;

    TAny* TypeExtension( TUint32 aType );

    TInt GetCurrentCspName( TPtrC8& aAttributeName,
                            TPtrC8& aAttributeNameSpace ) const;


private: // New helpers

    /**
     * Gets the CSP name table index for given protocol version.
     *
     * @since 3.0
     * @param aCspVer The CSP version to get.
     * @return Valid index or KErrNotFound if
     * requested protocol isn't supported.
     */
    TInt ProtocolNameTableIndex( TPEngWVCspVersion aCspVer ) const;



private: //Data

    //OWN: Current WV CSP version.
    const TPEngWVCspVersion iCurrentCspVer;

    //OWN: CSP protocol name table
    const TWVCspAttributeNameEntry*    iCspNameTable;

    //OWN:CSP protocol name table entry count
    const TInt  iCspNameTableCount;

    };

#endif      //CPENGWVATTRIBUTECONSTRUCTBASE_H

//  End of File

