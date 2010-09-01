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


#ifndef __CIMPSPLUGINREQUESTBASE_H__
#define __CIMPSPLUGINREQUESTBASE_H__


#include <e32base.h>
#include <ximpbase.h>

#include "impsdebugprint.h"


class MImpsPrtPluginConnectionManager;


/**
 * ?one_line_short_description
 *
 * ?more_complete_description
 *
 * @lib ?library
 * @since S60 v4.0
 */

class CImpsPluginRequestBase: public CActive


    {

    public:

        static CImpsPluginRequestBase* NewL( MImpsPrtPluginConnectionManager& aConnMan,
                                             TXIMPRequestId aRequestId );
        virtual ~CImpsPluginRequestBase();


    private:

        CImpsPluginRequestBase( MImpsPrtPluginConnectionManager& aConnMan,
                                TXIMPRequestId aRequestId  );
        void ConstructL();


    private: // from CActive

        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    public:

        virtual void ManufactureXMLMessageL() = 0;
        virtual void ParseXMLMessageL() = 0;


    private: // data

        /**
         * ?description_of_member
         */

        TXIMPRequestId iRequestId;

        /**
         * ?description_of_pointer_member
         * Own.  *** Write "Own" if this class owns the object pointed to; in
                     other words, if this class is responsible for deleting it.
         */


        /**
         * ?description_of_pointer_member
         * Not own.  *** Write "Not own" if some other class owns this object.
         */
        MImpsPrtPluginConnectionManager& iConnMan;
    };


#endif // __CIMPSPLUGINREQUESTBASE_H__

