/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Servers-array wrapper
*
*/



#ifndef CCASERVERSARRAY_H
#define CCASERVERSARRAY_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include "CWVSettingsUIDefs.h"


// CLASS DECLARATION

/**
*  Servers-array wrapper
*  Wraps real array handling to MDesCArray protocol
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/
NONSHARABLE_CLASS( CWVSettingsUIServersArray ) : public CBase, public MDesCArray
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aServerList List of servers
    * @param aProtectedServers a list of protected servers
    */
    static CWVSettingsUIServersArray* NewL( MDesCArray& aServerList,
    RArray<TInt>& aProtectedServers );

    /**
    * Destructor.
    */
    virtual ~CWVSettingsUIServersArray();

public: // Functions from base classes (MDesCArray)
    /**
    * From MDesCArray, Returns the number of descriptor elements in
    * a descriptor array.
    * @return Count of items
    */
    TInt MdcaCount() const;

    /**
    * From MDesCArray, Indexes into a descriptor array.
    * Return listbox-formatted data
    * @param aIndex The position of the descriptor element within a
    * descriptor array
    * @return A 16 bit non-modifiable pointer descriptor representing
    * the descriptor element located at position aIndex within a
    * descriptor array.
    */
    TPtrC16 MdcaPoint( TInt aIndex ) const;

public: // New methods

    /**
    * Returns raw unformatted data
    * @since 2.1
    * @param aIndex The position of the descriptor element within a
    * descriptor array
    * @return A 16 bit non-modifiable pointer descriptor representing
    * the descriptor element located at position aIndex within a
    * descriptor array.
    */
    TPtrC16 RawDataMdcaPoint( TInt aIndex ) const;

    /**
    * Replaces adapter model
    * @since 2.1
    * @param aServerList New model
    */
    void ReplaceModelL( MDesCArray* aServerList );

private:
    /**
    * C++ default constructor.
    * @param aServerList List of servers
    */
    CWVSettingsUIServersArray( MDesCArray& aServerList, RArray<TInt>& aProtectedServers );

    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();

private:    // Data
    // Doesn't own. Handle to engines contact-list array
    MDesCArray* iServerList;
    // Owns. server name
    HBufC* iData;
    // doesn't own. Array of protected servers
    RArray<TInt>* iProtectedServers;
    };

#endif      // CCASERVERSARRAY_H

// End of File
