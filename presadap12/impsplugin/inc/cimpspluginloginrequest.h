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
* Description:  IMPS Protocol implementation for Presence Framework
*
*/


#ifndef __CIMPSPLUGINLOGINREQUEST_H__
#define __CIMPSPLUGINLOGINREQUEST_H__

//  INCLUDES
#include <e32base.h>
#include <ximpbase.h>
#include "impsdebugprint.h"

// FORWARD DECLARATIONS
class MImpsPrtPluginConnectionManager;


/**
 * CImpsPluginLoginRequest
 *
 * Issue Login Request to the Network Server
 *
 * @lib ?library
 * @since S60 v4.0
 */
NONSHARABLE_CLASS(  CImpsPluginLoginRequest ) : public CActive


    {

public:

    static CImpsPluginLoginRequest* NewLC( MImpsPrtPluginConnectionManager& aConnMan,
    TXIMPRequestId aRequestId );

    static CImpsPluginLoginRequest* NewL( MImpsPrtPluginConnectionManager& aConnMan,
                                          TXIMPRequestId aRequestId );
    virtual ~CImpsPluginLoginRequest();


private:

    CImpsPluginLoginRequest( MImpsPrtPluginConnectionManager& aConnMan,
                             TXIMPRequestId aRequestId  );
    void ConstructL();


private: // from CActive

    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );

public:

    void IssueLoginRequestL();

private: // data

    /**
     * Request Id
     */

    TXIMPRequestId iRequestId;

    /**
     * ?description_of_pointer_member
     * Own.  *** Write "Own" if this class owns the object pointed to; in
                 other words, if this class is responsible for deleting it.
     */


    /**
     * Imps Protocol Plugin Connection Manager
     * Not own.  *** Write "Not own" if some other class owns this object.
     */
    MImpsPrtPluginConnectionManager& iConnMan;
    };


#endif // __CIMPSPLUGINLOGINREQUEST_H__

