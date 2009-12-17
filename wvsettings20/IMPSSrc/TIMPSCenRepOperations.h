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
* Description:  Central Repository operation handler.
*
*/


#ifndef TIMPSCENREPOPERATIONS_H
#define TIMPSCENREPOPERATIONS_H

//  INCLUDES
#include <e32base.h>
#include "impssapsettings.h"
#include "IMPSSAPSettingsStoreDefinitions.h"

// FORWARD DECLARATIONS

class CRepository;

// CLASS DECLARATION
/**
 * Central Repository operation handler.
 *
 * @lib WVSAPSettingsStore.dll
 * @since 3.0
 **/
NONSHARABLE_CLASS( TIMPSCenRepOperations )
    {
public: // Constructors and destructor

    TIMPSCenRepOperations( TUint32 aUid, CRepository& aRepository );

public: // New functions

    template <typename T>
    inline void CreateL( TFixedSAPSetting aSetting, const T& aSettingData )
        {
        User::LeaveIfError( iRepository.Create( iMaskBase + aSetting, aSettingData ) );
        }

    template <typename T>
    inline void GetL( TFixedSAPSetting aSetting, T& aSettingData )
        {
        User::LeaveIfError( iRepository.Get( iMaskBase + aSetting, aSettingData ) );
        }

    template <typename T>
    inline void SetL( TFixedSAPSetting aSetting, const T& aSettingData )
        {
        User::LeaveIfError( iRepository.Set( iMaskBase + aSetting, aSettingData ) );
        }

private: // Data

    CRepository& iRepository; // Handle to cenrep session. Does not own.
    TUint32 iMaskBase; // Base for cenrep masks.
    };

#endif	// TIMPSCENREPOPERATIONS_H

