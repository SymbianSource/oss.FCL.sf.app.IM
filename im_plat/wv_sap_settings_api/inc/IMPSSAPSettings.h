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
* Description:  IMPSSAPSettingsStore common definitions
*
*/

#ifndef __IMPSSAPSETTINGS_H
#define __IMPSSAPSETTINGS_H


//  INCLUDES
#include <e32std.h>

//CONSTS
const TInt KIMPSSAPProtectionNULLOperation = 0;
const TInt KIMPSSettingsNullUid = 0;

//DATA TYPES

/**
 * SAP protection levels.
 *
 * This enumeration declares currently supported
 * SAP protection levels. SAP protection is used
 * to protect certain SAP data fields from altering
 * and also to forbid SAP deletion from the store.
 *
 * Current protection levels and their effects:
 * - ESAPNoProtection
 *      - SAP isn't protected any way. All SAP fields
 *        can be edited and SAP can be deleted.
 *      - The default protection level.
 *
 * - ESAPBrandProtection:
 *      - Protection level used with branded default SAP's
 *      - Forbids the altering of following SAP fields:
 *          - SAP name
 *
 * @since 3.0
 */
enum TIMPSSAPProtection
    {
    ESAPNoProtection = 0x0,      ///<No protection, default
    ESAPBrandProtection = 0x1    ///<Branding protection
    };

/**
 * Access groups.
 *
 * This enumeration declares currently supported
 * access groups. The access group defines which
 * SAPs are accessible to the client. The enumeration
 * acts as a bitmask.
 *
 * No access group. This is used for:
 *
 * - Error situations
 * - Initialisations
 * - Cannot be used for storing or accessing SAPs
 *
 *  - EIMPSNoAccessGroup
 *
 * PEC & IM access groups. These can be used for:
 *
 * - storing new SAPs to either group.
 * - accessing the data to get SAP's from desired group.
 *
 * 	- EIMPSIMAccessGroup: (mask = 01)
 *
 * 	- EIMPSPECAccessGroup: (mask = 10)
 *
 * Access filter
 *
 * - For listing and accessing SAPs from all groups
 *
 * 	- EIMPSAccessFilterAll: (mask = 11)
 *
 * @since 3.0
 */
enum TIMPSAccessGroup
    {
    EIMPSNoAccessGroup = 0,
    EIMPSIMAccessGroup = 1,
    EIMPSPECAccessGroup = 2,
    EIMPSAccessFilterAll = EIMPSIMAccessGroup | EIMPSPECAccessGroup
    };

// Enumeration for defining high level services.

enum TIMPSHighLevelServices
    {
    EIMPSServicesUnknown = 0, // 000
    EIMPSServicesIM = 1, // 001
    EIMPSServicesPEC = 2, //010
    EIMPSServicesGroups = 4 // 100
    };

// Enumeration for defining
// authorization mode to be used with the SAP

enum TIMPSAuthorizationMode
    {
    EIMPSAuthModeUnspecified = 0x00,
    EIMPSAuthModeProactive  = 0x01,
    EIMPSAuthModeReactive = 0x02
    };

#endif      // __IMPSSAPSETTINGS_H
//  End of File


