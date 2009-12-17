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
* Description:  Process Component for Settings
*
*/



#ifndef CCASETTINGSPC_H
#define CCASETTINGSPC_H

//  INCLUDES
#include    <e32base.h>
#include    "MCASettingsPC.h"

// FORWARD DECLARATIONS
class CCAEngine;
class CIMPSSAPSettings;
class MCASettings;


// CLASS DECLARATION

/**
*  CCASettingsPC
*  Process Component for Settings
*  since 3.2 lib
*/

class CCASettingsPC : public CBase, public MCASettingsPC
    {
    public:

        /**
         * Two-phased constructor.
         */
        static CCASettingsPC* NewL( CCAEngine& aEngine );


        /**
         * Destructor
         */
        virtual ~CCASettingsPC();

    public:
        /**
         * Sets the default sap
         */

        void SetDefaultSapL( CIMPSSAPSettings* aSap ) ;

        /**
         * Retrieves value.
         * @param aSetting specifies the wanted setting.
         * @return Boolean value.
         */
        TBool GetBoolValuePC( TEnumsPC::TCASettingValues aSetting,
                              CIMPSSAPSettings* aSap = NULL );


        /**
         * Retrieves value.
         * @param aSetting specifies the wanted setting.
         * @return Integer value.
         */
        TInt GetIntValuePC( TEnumsPC::TCASettingValues aSetting,
                            CIMPSSAPSettings* aSap = NULL );


        /**
         * Retrieves value..
         * @return Bool value.
         */
        TBool IsAlphabeticalPC( CIMPSSAPSettings* aSap = NULL );


        /**
         * Retrieves value.
         * Ownership is transferred to caller!!!
         * @param aSetting specifies the wanted setting.
         * @return String value.
         */
        HBufC* GetSapSettingValuePCL( TEnumsPC::TCASettingStrings aSetting,
                                      CIMPSSAPSettings* aSap  = NULL );
        /**
         * Sets a value.
         * @param aSetting specifies the wanted setting.
         * @param aValue specifies the value we want to store.
         */
        void SetValueL( TEnumsPC::TCASettingValues aSetting, TBool aValue ) ;
        /**
         * Sets a value.
         * @param aSetting specifies the wanted setting.
         * @param aValue specifies the value we want to store.
         * @param aIgnoreRollback Don't perform rollback operation on these
         *                        errors
         *        NULL if rollback is wanted in every error situation.
         */
        void SetBoolValuePCL( TEnumsPC::TCASettingValues aSetting, TBool aValue,
                              RArray<TInt>* aIgnoreRollback  = NULL,
                              CIMPSSAPSettings* aSap  = NULL );

        /**
         * Sets a value.
         * @param aSetting specifies the wanted setting.
         * @param aValue specifies the value we want to store.
         * @param aIgnoreRollback Don't perform rollback operation on these
         *                        errors
         *        NULL if rollback is wanted in every error situation.
         */
        void SetIntValuePCL( TEnumsPC::TCASettingValues aSetting, TInt aValue,
                             RArray<TInt>* aIgnoreRollback, /* = NULL*/
                             CIMPSSAPSettings* aSap /* = NULL*/ );


        /**
        * Sets a value.
        * @param aSetting specifies the wanted setting.
        * @param aValue specifies the value we want to store.
        * @param aIgnoreRollback Don't perform rollback operation on
        *                        these errors
        *        NULL if rollback is wanted in every error situation.
        */
        void SetSapSettingValuePCL( TEnumsPC::TCASettingStrings aSetting, const TDesC& aValue,
                                    RArray<TInt>* aIgnoreRollback = NULL ,
                                    CIMPSSAPSettings* aSap = NULL );

        /**
         * Flushes data from memory to persistent storage
         */
        virtual void FlushDataPC();


    private:

        /**
         * ConstructL
         */
        void ConstructL( CIMPSSAPSettings* aSap );

        /**
         * Default Constructor.
         */
        CCASettingsPC( CCAEngine& aEngine );

    private: //data

        //Does not own,Reference to Engine
        CCAEngine& iEngine;

        //Does not own,pointer to Engine Settings
        MCASettings* iSettings;


    };

#endif      // CCASETTINGSPC_H

// End of File
