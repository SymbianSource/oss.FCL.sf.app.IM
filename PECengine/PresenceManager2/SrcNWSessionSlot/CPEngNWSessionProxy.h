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
* Description: this class handles the network session operation
*
*/

#ifndef CPENGNWSESSIONPROXY_H
#define CPENGNWSESSIONPROXY_H

//  INCLUDES
#include "RPEngManagerClient.h"
#include "RPEngSessionHolder.h"
#include <E32Base.h>



//FORWARD DECLARATIONS
class CPEngNWSessionSlotID2Imp;
class CPEngNWSessionSlotID2;
class TPEngWVCspServicesTree2;



/**
 *
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngNWSessionProxy ) : public CBase
    {

    //-----------------------------------------------------------------------
public: /* Construction */

    /**
     * Instantiates CPEngNWSessionProxy object.
     *
     * @return New CPEngNWSessionProxy instance.
     */
    static CPEngNWSessionProxy* NewL( const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     * Virtual by CBase.
     */
    ~CPEngNWSessionProxy();


protected:  // Constructor

    /**
     * C++ default constructor.
     */
    CPEngNWSessionProxy();

    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     *
     */
    TInt InitializeConnection();


public:

    void Close();


public:

    TInt OpenSession( const TDesC8& aLogInData, TRequestStatus& aStatus );
    void CancelOpenSession();

    TInt CloseSession( TRequestStatus& aStatus );
    void CancelCloseSession();

    TInt ForceCloseSession( TRequestStatus& aStatus );
    void CancelForceCloseSession();

    TInt OpenOwnership();

    TInt StoreOwnership( const TDesC16& aOwnerID );
    TInt RestoreOwnership( const TDesC16& aOwnerID );

    TInt GetServiceTree( TPEngWVCspServicesTree2& aCspTree );
    TInt GetLoginData( HBufC8*& aLogInData );




private: //Data

    RPEngManagerClient  iMainClient;
    RPEngSessionHolder  iSessionClient;

    CPEngNWSessionSlotID2Imp* iNWSessionSlotID;

    TBool   iInitialized;

    };

#endif      //CPENGNWSESSIONPROXY_H



