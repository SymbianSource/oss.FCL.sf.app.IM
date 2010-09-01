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
* Description:  Publish & Subscribe property change observer
*
*/


#ifndef IMPSPROPERTYOBSERVERAPI_H
#define IMPSPROPERTYOBSERVERAPI_H


#include <e32base.h>
#include <e32property.h>

class MImpsPropertyObserver
    {

    public:

        virtual void HandlePropertyChangeL( TInt aValue ) = 0;
    };

#endif      // IMPSPROPERTYOBSERVERAPI_H  

// End of File
