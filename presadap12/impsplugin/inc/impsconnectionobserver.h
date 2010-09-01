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
* Description:  This is a connection observer class
*
*/

#ifndef __M_IMPSCONNECTIONOBSERVER_H__
#define __M_IMPSCONNECTIONOBSERVER_H__


/**
 * ?one_line_short_description
 *
 * ?more_complete_description
 *
 * @lib ?library
 * @since S60 v4.0
 */
class MImpsConnectionObserver
    {

    public:

        /**
         * ?description
         *
         * @since S60 ?S60_version
         * @param ?arg1 ?description
         * @param ?arg2 ?description
         * @return ?description
         */
        virtual void NotifyOperationComplete( TInt aResult ) = 0;

    protected:

        /**
         * Protected destructor.
         * Instancies can't be destroyed via this interface.
         */
        virtual ~MImpsConnectionObserver() {}

    };


#endif // __M_IMPSCONNECTIONOBSERVER_H__


