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
* Description: Presence Engine AO dialog status notifier
 *
*/




#ifndef MPENGAODIALOGSTATUSNOTIFIER_H
#define MPENGAODIALOGSTATUSNOTIFIER_H

//  INCLUDES
#include <e32base.h>

/**
*  MPENGAODIALOGSTATUSNOTIFIER_H
*
*/
class MPEngAODialogStatusNotifier
    {
    public:
        /**
        * pure virtual method.
        */
        virtual void CommonDialogDismissedL( const TInt aStatusCode ) = 0;
    };

#endif // MPENGAODIALOGSTATUSNOTIFIER_H

// End of File
//
//
