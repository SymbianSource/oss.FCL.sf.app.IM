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
* Description:  Interface for settings related operations
*
*/


#ifndef MCASETTINGSPC_H
#define MCASETTINGSPC_H


// INCLUDES
#include <e32base.h>
#include "MCASettings.h"
#include "TEnumsPC.h"


// FORWARD DECLARATION
class CIMPSSAPSettings;



//Class Declaration
/**
 *  Interface for internal settings handler.
 *  @lib CAEngine.lib
 *  @since 3.2
 */

class MCASettingsPC
    {

    public:

        /**
         * Retrieves value.
         * @param aSetting specifies the wanted setting.
         * @param aSAPSettings - SAP Settings.
         * @return Boolean value.
         */
        virtual TBool GetBoolValuePC( TEnumsPC::TCASettingValues aSetting,
                                      CIMPSSAPSettings* aSAPSettings /* = NULL*/ ) = 0;

        /**
         * Retrieves value.
         * @param aSetting specifies the wanted setting.
         * @param aSAPSettings - SAP Settings.
         * @return Integer value.
         */
        virtual TInt GetIntValuePC( TEnumsPC::TCASettingValues aSetting,
                                    CIMPSSAPSettings* aSAPSettings /* = NULL*/ ) = 0;

        /**
         * Retrieves value..
         * @return Bool value.
         */
        virtual TBool IsAlphabeticalPC( CIMPSSAPSettings* aSAPSettings /* = NULL */ ) = 0;

        /**
         * Retrieves value.
         * Ownership is transferred to caller!!!
         * @param aSetting specifies the wanted setting.
         * @param aSAPSettings - SAP Settings.
         * @return String value.
         */
        virtual HBufC* GetSapSettingValuePCL( TEnumsPC::TCASettingStrings aSetting,
                                              CIMPSSAPSettings* aSAPSettings  = NULL ) = 0;

        /**
         * Sets a value.
         * @param aSetting specifies the wanted setting.
         * @param aValue specifies the value we want to store.
         * @param aIgnoreRollback Don't perform rollback operation on these
         * @ aSAPSettings - SAP Settings.
         *                        errors
         *        NULL if rollback is wanted in every error situation.
         */
        virtual void SetBoolValuePCL( TEnumsPC::TCASettingValues aSetting,
                                      TBool aValue, RArray<TInt>* aIgnoreRollback  = NULL,
                                      CIMPSSAPSettings* aSAPSettings  = NULL ) = 0;

        /**
         * Sets a value.
         * @param aSetting specifies the wanted setting.
         * @param aValue specifies the value we want to store.
         * @param aIgnoreRollback Don't perform rollback operation on these
         *                        errors
         *        NULL if rollback is wanted in every error situation.
         * @param aSAPSettings - SAP Settings.
         */
        virtual void SetIntValuePCL( TEnumsPC::TCASettingValues aSetting, TInt aValue,
                                     RArray<TInt>* aIgnoreRollback, /* = NULL*/
                                     CIMPSSAPSettings* aSAPSettings /* = NULL*/ ) = 0;


        /**
         * Sets a value.
         * @param aSetting specifies the wanted setting.
         * @param aValue specifies the value we want to store.
         * @param aIgnoreRollback Don't perform rollback operation on
         *                        these errors
         *        NULL if rollback is wanted in every error situation.
         * @param aSAPSettings - SAP Settings.
         */
        virtual void SetSapSettingValuePCL( TEnumsPC::TCASettingStrings aSetting,
                                            const TDesC& aValue, RArray<TInt>* aIgnoreRollback = NULL ,
                                            CIMPSSAPSettings* aSAPSettings = NULL   ) = 0;



        /**
        * Flushes data from memory to persistent storage
        */
        virtual void FlushDataPC( ) = 0;

        /**
        * Sets the default Sap
        * @param aSAPSettings - SAP Settings.
        */
        virtual void SetDefaultSapL( CIMPSSAPSettings* aSAPSettings ) = 0 ;

        /**
        * Sets a value.
        * @param aSetting specifies the wanted setting.
        * @param aValue specifies the value we want to store.
        */
        virtual void SetValueL( TEnumsPC::TCASettingValues aSetting, TBool aValue ) = 0;

        /**
         * Destructor
         */

        virtual ~MCASettingsPC() {};


    };


#endif // MCASETTINGSPC_H

// End of File
