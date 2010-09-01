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
* Description:  "Light weight" list item for SAP settings list.
*
*/


#ifndef __CIMPSSAPSETTINGSLISTITEM_H
#define __CIMPSSAPSETTINGSLISTITEM_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include "impssapsettings.h"

// CLASS DECLARATION
/**
* CIMPSSAPSettingsListItem is "light weight" representation of
* SAP settings. It contains only SAP name, UID, protection level,
* and access group.
*
* CIMPSSAPSettingsList uses CIMPSSAPSettingsListItems to represent
* stored SAP settings.
*
* @lib WVSAPSettingsStore.dll
* @since 3.0
*/
class CIMPSSAPSettingsListItem : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * Defaults the SAP protection level to
         * ESAPNoProtection.
         *
         * @param aSAPName The SAP name.
         * @param aUid The SAP uid.
         * @param aGroup The SAP access group
         *
         * @since 3.0
         */

        IMPORT_C static CIMPSSAPSettingsListItem* NewLC( const TDesC& aSAPName,
                                                         TUint32 aUid,
                                                         TIMPSAccessGroup aGroup );

        //for BC
        IMPORT_C static CIMPSSAPSettingsListItem* NewLC( const TDesC& aSAPName,
                                                         TUint32 aUid );

        /**
         * Two-phased constructor.
         * Overload method accepting also the protection level.
         * If the passed protection level isn't one of the
         * defined in TIMPSSAPProtection, leaves with KErrArgument.
         *
         * @param aSAPName The SAP name.
         * @param aUid The SAP uid.
         * @param aAccessGroup The SAP access group
         * @param aProtection The SAP protection.
         *
         * @since 3.0
         */
        IMPORT_C static CIMPSSAPSettingsListItem* NewLC( const TDesC& aSAPName,
                                                         TUint32 aUid,
                                                         TIMPSAccessGroup aGroup,
                                                         TIMPSSAPProtection aProtection );

        //for BC
        IMPORT_C static CIMPSSAPSettingsListItem* NewLC( const TDesC& aSAPName,
                                                         TUint32 aUid,
                                                         TIMPSSAPProtection aProtection );

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIMPSSAPSettingsListItem();


    private:  // Constructors and destructor


        /**
         * C++ default constructor.
         */
        CIMPSSAPSettingsListItem();



    public: // New functions


        /**
         * Gets the SAP settings name.
         * @return The name.
         *
         * @since 3.0
         */
        IMPORT_C const TDesC& Name() const;

        /**
         * Gets the SAP settings UID.
         * @return The UID.
         *
         * @since 3.0
         */
        IMPORT_C TUint32 Uid() const;

        /**
         * Gets the SAP settings protection level.
         * @return The protection level.
         *
         * @since 3.0
         */
        IMPORT_C TIMPSSAPProtection Protection() const;

        /**
         * Gets the SAP settings access group.
         * @return The access group of which the SAP belongs to.
         *
         * @since 3.0
         */
        IMPORT_C TIMPSAccessGroup AccessGroup() const;

    private:    // Data


        TUint32             iUid;           //OWN: Uid
        HBufC*              iSAPName;       //OWN: SAP name
        TIMPSSAPProtection  iProtection;    //OWN: Protection
        TIMPSAccessGroup	iGroup;			//OWN: Access group

        //reserved extensions
        TAny*  iReserved1;
        TAny*  iReserved2;

    };

#endif	// __CIMPSSAPSETTINGSLISTITEM_H
