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
* Description:  SAP Settings Store proxy.
*
*/

#ifndef __CCNUISAPSTOREPROXY_H
#define __CCNUISAPSTOREPROXY_H

//  INCLUDES
#include <E32Base.h>


//FORWARD DECLARATION
class CIMPSSAPSettingsStore;


// CLASS DECLARATION
/**
 * SAP Settings Store proxy.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiSapStoreProxy ) : public CBase
    {
public:  // constructor and destructor
    /**
     * C++ constructor.
     */
    CCnUiSapStoreProxy( CIMPSSAPSettingsStore* aSapStore );


    /**
     * Destructor.
     */
    virtual ~CCnUiSapStoreProxy();



public: // New functions

    /**
     * Gets a reference to SAP store owned by the proxy.
     *
     * @since 2.1
     * @return Reference CIMPSSAPSettingsStore instance.
     */
    CIMPSSAPSettingsStore& SapStoreL();


    /**
     * Pushes CIMPSSAPSettingsStore release
     * on the cleanupstack.
     *
     * @since 2.1
     */
    void PushOwnedSapStoreReleaseLC();


    /**
     * Releases owned SAP settings store.
     * After this, any previously obtained
     * CIMPSSAPSettingsStore reference is invalid.
     *
     * @since 2.1
     */
    void ReleaseOwnedSapStore();


private: // data

    ///<Used CIMPSSAPSettingsStore object.
    //If iOwnsSapStore bool is ETrue, then owned, else not owned.
    CIMPSSAPSettingsStore*          iSapStore;

    ///<SAP store owning flag, owned
    TBool                         iOwnsSapStore;

    };

#endif      //  __CCNUISAPSTOREPROXY_H

//  End of File

