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
* Description:  Shared data factory
*
*/

#ifndef __CWVSETTINGSUISHAREDDATAFACTORY_H
#define __CWVSETTINGSUISHAREDDATAFACTORY_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MIMPSSharedDataObserver;
class MIMPSSharedData;
class MIMPSSystemNotifier;
class MIMPSSystemNotifierObserver;

// CLASS DECLARATION

/**
*  Shared data factory
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.6
*/
NONSHARABLE_CLASS( CIMPSSharedDataFactory ) : public CBase
    {

public: // New functions
    /**
    * Creates a permanent key handler
    * Returns the correct implementation.
    */
    static MIMPSSharedData* CreatePermanentKeyHandlerL( MIMPSSharedDataObserver* aObserver,
    const TUid aUid );

    /**
    * Creates a permanent key handler
    * Returns the correct implementation.
    */
    static MIMPSSharedData* CreateTemporaryKeyHandlerL( MIMPSSharedDataObserver* aObserver,
                                                        const TUid aUid );

    /**
    * Creates a system notify handler
    * Returns the correct implementation.
    */
    static MIMPSSystemNotifier* CreateSystemNotifyHandlerL( MIMPSSystemNotifierObserver& aObserver,
                                                            const TUid aKey );

private:

    /**
    * C++ default constructor.
    */
    CIMPSSharedDataFactory();

    /**
    * Destructor.
    */
    virtual ~CIMPSSharedDataFactory();

    };

#endif      // __CWVSETTINGSUISHAREDDATAFACTORY_H

// End of File
