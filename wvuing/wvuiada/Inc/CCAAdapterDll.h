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
* Description:  Dll access point.
*
*/



#ifndef CCAADAPTERDLL_H
#define CCAADAPTERDLL_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MCAImpsFactory;

// FUNCTION PROTOTYPES

IMPORT_C MCAImpsFactory* CreateImpsFactoryL();

IMPORT_C MCAImpsFactory* CurrentImpsFactoryInstance();

IMPORT_C void ReleaseAdapterL();

#endif      // CCAADAPTERDLL_H

// End of File
