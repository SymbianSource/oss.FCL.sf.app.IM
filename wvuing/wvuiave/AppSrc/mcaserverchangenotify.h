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
* Description:  This interface describes structure of server change notify services
*
*/



#ifndef MCASERVERCHANGENOTIFY_H
#define MCASERVERCHANGENOTIFY_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  An interface for listening server changes
*
*  @since Series 60 3.2
*/
class MCAServerChangeNotify
    {
    public:
        enum TServerNotify
            {
            ENoServer = 0,
            EOtherChange
            };
    public:

        virtual ~MCAServerChangeNotify() {};

    public: // New functions

        virtual void HandleServerChangeL( TServerNotify aKey ) = 0;
    };

#endif      // MCASERVERCHANGENOTIFY_H   

// End of File

