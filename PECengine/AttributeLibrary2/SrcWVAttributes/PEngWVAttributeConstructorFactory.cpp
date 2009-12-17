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
* Description:  Wireless Village attribute constructor factory.
*
*/

// INCLUDE FILES
#include "PEngWVAttributeConstructorFactory.h"
#include "CPEngAttrConstructorCon.h"
#include "PresenceDebugPrint.h"

#include "CPEngWVAttributeConstructBase.h"
#include "CPEngWVOnlineStatusConstruct.h"
#include "CPEngWVUserAvailabilityConstruct.h"
#include "CPEngWVStatusTextConstruct.h"
#include "CPEngWVClientInfoConstruct.h"
#include "CPEngWVCommCapConstruct.h"

#include "CPEngWVStatusContentConstruct.h"
#include "CPEngWVThumbnailConstruct.h"
#include "CPEngWVAliasConstruct.h"
#include "CPEngWVInfoLinkConstruct.h"
#include "PEngInfoLinkSupport.h"

#include "MPEngPresenceAttrModel2.h"
#include "MPEngPresenceAttrManager.h"

#include <E32Base.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PEngWVAttributeConstructorFactory::CreateConstructorsL()
// -----------------------------------------------------------------------------
//
void PEngWVAttributeConstructorFactory::CreateConstructorsL(
    CPEngAttrConstructorCon& aConstructors )
    {
    TPEngWVCspVersion cspVer = PEngWVCspVersion::SelectVersionL();
    MPEngPresenceAttrConstructorTypeImp* typeConstructor = NULL;


    //WV CSP Attributes supported in all versions (WV CSP 1.1 and 1.2)
    typeConstructor = new ( ELeave ) CPEngWVOnlineStatusConstruct( cspVer );  // CSI: 35 #
    aConstructors.AddConstructorL( typeConstructor,
                                   KUidPrAttrOnlineStatus,
                                   EPEngClientServerOriginated );


    typeConstructor = new ( ELeave ) CPEngWVUserAvailabilityConstruct( cspVer );  // CSI: 35 #
    aConstructors.AddConstructorL( typeConstructor,
                                   KUidPrAttrUserAvailability,
                                   EPEngClientOriginated );


    typeConstructor = new ( ELeave ) CPEngWVStatusTextConstruct( cspVer );  // CSI: 35 #
    aConstructors.AddConstructorL( typeConstructor,
                                   KUidPrAttrStatusText,
                                   EPEngClientOriginated );


    typeConstructor = CPEngWVClientInfoConstruct::NewL( cspVer );           // CSI: 35 #
    aConstructors.AddConstructorL( typeConstructor,
                                   KUidPrAttrClientInfo,
                                   EPEngClientOriginated );


    typeConstructor = new ( ELeave ) CPEngWVCommCapConstruct( cspVer );     // CSI: 35 #
    aConstructors.AddConstructorL( typeConstructor,
                                   KUidPrAttrCommCap,
                                   EPEngClientServerOriginated );


    typeConstructor = new ( ELeave ) CPEngWVStatusContentConstruct( cspVer ); // CSI: 35 #
    aConstructors.AddConstructorL( typeConstructor,
                                   KUidPrAttrStatusContent,
                                   EPEngClientOriginated );


    typeConstructor = new ( ELeave ) CPEngWVThumbnailConstruct( cspVer );   // CSI: 35 #
    aConstructors.AddConstructorL( typeConstructor,
                                   KUidPrAttrMyLogoThumbnail,
                                   EPEngLocalAttribute );


    typeConstructor = new ( ELeave ) CPEngWVAliasConstruct( cspVer );       // CSI: 35 #
    aConstructors.AddConstructorL( typeConstructor,
                                   KUidPrAttrAlias,
                                   EPEngClientOriginated );



    //WV CSP Attributes supported only in WV CSP 1.2
    if ( ( cspVer == EWVCspV12 ) && PEngInfoLinkSupport::InfoLinkSupportedL() )
        {
        typeConstructor = new ( ELeave ) CPEngWVInfoLinkConstruct( cspVer );  // CSI: 35 #
        aConstructors.AddConstructorL( typeConstructor,
                                       KUidPrAttrInfoLink,
                                       EPEngClientOriginated );
        }
    }



// -----------------------------------------------------------------------------
// PEngWVAttributeConstructorFactory::LoadSessionTracingAttributesL()
// -----------------------------------------------------------------------------
//
void PEngWVAttributeConstructorFactory::LoadSessionTracingAttributesL(
    RPointerArray< MPEngPresenceAttrModel2 >& aSessionAttributes,
    MPEngPresenceAttrManager& aAttributeManager )
    {
    MPEngPresenceAttrModel2* model = NULL;
    aAttributeManager.LoadAttributeLC( model,
                                       KNullDesC,
                                       KUidPrAttrOnlineStatus,
                                       EPEngUserAttribute | EPEngStorableModel );
    aSessionAttributes.AppendL( model );
    CleanupStack::Pop(); //model
    }



//  End of File



