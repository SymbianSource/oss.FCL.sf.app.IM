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
* Description:  Class is used for mapping engine errors to correct error notes
*
*/



#ifndef IMNOTEMAPPER_H
#define IMNOTEMAPPER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Class is used for mapping engine errors to correct error notes
*
*  @lib chat.app
*  @since
*/
class IMNoteMapper
    {
    public: // New functions
        /**
        * Shows specified error note for aErrorCode (UI specification)
        * @param aErrorCode Error code
        * @param aAdditionalData Additional data to show.
        * @return Did class have information about given error or not
        */
        IMPORT_C static void ShowNoteL( const TInt aErrorCode,
                                        const TDesC& aAdditionalData = KNullDesC );
    };

#endif      // IMNOTEMAPPER_H

// End of File
