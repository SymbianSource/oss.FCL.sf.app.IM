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
* Description:  Build definitions
 *
*/



#ifndef IMPSBUILDDEFINITIONS_H
#define IMPSBUILDDEFINITIONS_H

// variated features

// For Testing
// Used for Testing
//#define CHAT_UNIT_TEST
// debugging

// Enables debug print
//#define _ENABLE_DEBUG_PRINT
// Enabled file debug (Remember to create c\Logs\Chat folder!!!)
//#define _ENABLE_FILE_DEBUG

// Enables layout debug print
// #define _ENABLE_LAYOUT_DEBUG_PRINT
// Enabled layout file debug (Remember to create c\Logs\Chat folder!!!)
// #define _ENABLE_LAYOUT_FILE_DEBUG

// Enables sending not supported content.
//#define RD_SEND_NOT_SUPPORTED_CONTENT

// Enables settings facelift
#define RD_SETTINGS_FACELIFT

// Enables background fetching of contact lists.
#define IMPS_CONTACT_FETCH_BACKGROUND

// Enables service enhancements
#define RD_SERVICE_ENHANCEMENTS

// build-time variation flags

// "IM.Block.Query"
#define IMPS_BLOCK_AFTER_DELETE

// Enables send to many functionality
#define RD_SEND_TO_MANY

// Enables message indication layout change
#define RD_MESSAGE_INDICATION_NEW

// Enables new message indication in chat group list view and main view
#define RD_CHAT_GROUP_MESSAGE_INDICATION_NEW

// IM.Collapse.Option.Focusing.Contact
// Remove 'Collapse' option from the Options menu, when focus is on a contact
#define RD_REMOVE_COLLAPSE_OPTION

// This enables the IM to be compiled in S60 v3 0 platform
// Disables S60 Common features
// - Disabled MSK support
// - Disables Touch support
//
// NOTE: DO NOT ENABLE IN CCB
//
//#include "im30compatibility.h"

#endif      // IMPSBUILDDEFINITIONS_H

// End of File

