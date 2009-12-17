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
* Description:  IMPS Protocol presence features implementation for Presence Framework
*
*/



#ifndef __CIMPSPROTOCOLPRESENCEFEATURES_H__
#define __CIMPSPROTOCOLPRESENCEFEATURES_H__

//  INCLUDES
#include <e32base.h>
#include <protocolpresencefeatures.h>
#include <protocolpresencewatching.h>
#include <protocolpresencepublishing.h>
#include <protocolpresentitygroups.h>
#include <protocolpresenceauthorization.h>
#include <cimpspluginconnectionmanager.h>
class CImpsProtocolPresenceWatching;
class CImpsProtocolPresencePublishing;
class CImpsProtocolPresentityGroups;
class CImpsProtocolPresenceAuthorization;

/**
 * CImpsProtocolPresenceFeatures
 *
 * Imps Protocol Connection Object. Protocol Connection Object has connection Manager.
 *
 * @lib ?library
 * @since S60 v4.0
 */
class CImpsProtocolPresenceFeatures :
            public CBase,
            public MProtocolPresenceFeatures
    {

    public:

        static CImpsProtocolPresenceFeatures* NewL( CImpsPluginConnectionManager& aConnMan );


        ~CImpsProtocolPresenceFeatures();

    private:

        CImpsProtocolPresenceFeatures();


        void ConstructL( CImpsPluginConnectionManager& aConnMan );

    public: // from MXIMPBase

        TAny* GetInterface( TInt32 aInterfaceId,
                            TIfGetOps  aOptions );

        const TAny* GetInterface( TInt32 aInterfaceId,
                                  TIfGetOps  aOptions ) const;


        TInt32 GetInterfaceId() const;

    public :

        // From Access to MProtocolPresenceFeatures
        MProtocolPresenceWatching& PresenceWatching();
        MProtocolPresencePublishing& PresencePublishing();
        MProtocolPresentityGroups& PresentityGroups();
        MProtocolPresenceAuthorization& PresenceAuthorization();

    private : //data members

        CImpsProtocolPresenceWatching*	iWatching;
        CImpsProtocolPresencePublishing*	iPublishing;
        CImpsProtocolPresentityGroups*	iPresentityGroups;
        CImpsProtocolPresenceAuthorization*	iAuthrozation;

    };

#endif //__CIMPSPROTOCOLPRESENCEFEATURES_H__