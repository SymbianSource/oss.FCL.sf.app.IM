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
* Description:  This class is used to store/retrieve plugin implementation
*		 specific shared data keys and enumerations.
*
*/


#ifndef __TIMPSAOKEYS_H
#define __TIMPSAOKEYS_H

//  INCLUDES
#include "MIMPSSharedData.h"    // for TIMPSSharedKeys

#include <e32std.h>

/**
*  Keeps plugin implementation type related shared data keys and enumerations
*
*  @lib PEngAOPlugin.lib
*  @since 3.1
*/
class TIMPSAOKeys
    {

    public: // constructors
        /**
         * Constructor
         */
        TIMPSAOKeys();

    public: // function for get enumerations

        /**
         * @return Minimum login type enum
         */
        TInt MinLoginTypeEnum();

        /**
         * @return Manual login enum
         */
        TInt LoginManualEnum();

        /**
         * @return ApplicationLaunch login enum
         */
        TInt LoginApplicationLaunchEnum();

        /**
         * @return AutoAlways login enum
         */
        TInt LoginAutoAlwaysEnum();

        /**
         * @return AutoInHomeNW login enum
         */
        TInt LoginAutoInHomeNWEnum();

        /**
         * @return Maximum Login type enum
         */
        TInt MaxLoginTypeEnum();

    public: // functions for get keys

        /**
        * Used for logged client type shared data key
        * @return connection days shared data key
        */
        const TIMPSSharedKeys& LoginTypeKey();

        /**
        * Used for getting connection days shared data key
        * @return connection days shared data key
        */
        const TIMPSSharedKeys& ConnDaysKey();

        /**
        * Used for getting connection start time shared data key
        * @return connection start time shared data key
        */
        const TIMPSSharedKeys& ConnStartKey();

        /**
        * Used for getting connection end time shared data key
        * @return connection end time shared data key
        */
        const TIMPSSharedKeys& ConnEndKey();

        /**
        * Used for getting logintype changed shared data key
        * @return logintype changed shared data key
        */
        const TIMPSSharedKeys& LoginTypeChangedKey();

        /**
        * Used for getting schedule changed shared data key
        * @return schedule changed shared data key
        */
        const TIMPSSharedKeys& ScheduleChangedKey();


    private: // member variables

        /// stored enumerations
        TInt iMinLoginTypeEnum;
        TInt iLoginManualEnum;
        TInt iLoginApplicationLaunchEnum;
        TInt iLoginAutoAlwaysEnum;
        TInt iLoginAutoInHomeNWEnum;
        TInt iMaxLoginTypeEnum;

        /// stored keys
        TIMPSSharedKeys iLoginTypeKey;
        TIMPSSharedKeys iConnDaysKey;
        TIMPSSharedKeys iConnStartKey;
        TIMPSSharedKeys iConnEndKey;
        TIMPSSharedKeys iLoginTypeChangedKey;
        TIMPSSharedKeys iScheduleChangedKey;


    };

#endif // __TIMPSAOKEYS_H
