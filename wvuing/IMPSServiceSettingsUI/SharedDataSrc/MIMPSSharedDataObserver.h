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
* Description:  Shared data observer
*
*/

#ifndef __MIMPSSHAREDDATAOBSERVER_H
#define __MIMPSSHAREDDATAOBSERVER_H

//  INCLUDES
#include "MIMPSSharedData.h"

// CLASS DECLARATION

/**
*  Shared data handler
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.6
*/
NONSHARABLE_CLASS( MIMPSSharedDataObserver )
    {
public: // New functions

    /**
    * Handles notify from permanent keys' changes
    * @param aUid uid of the dll
    * @param aKey the shared data key
    * @param aValue the value of the key
    */
    virtual void HandlePermanentKeyNotifyL( const TUid aUid,
                                            const TIMPSSharedKeys aKey ) = 0;

    /**
    * Handles notify from temporary keys' changes
    * @param aUid uid of the dll
    * @param aKey the shared data key
    * @param aValue the value of the key
    */
    virtual void HandleTemporaryKeyNotifyL( const TUid aUid,
                                            const TIMPSSharedKeys aKey ) = 0;

protected:

    /**
    * Destructor
    */
    virtual ~MIMPSSharedDataObserver() {};

    };

#endif      // __MIMPSSHAREDDATAOBSERVER_H

// End of File
