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
* Description:  Presence Attribute Default Value API specific CenRep keys.
*
*/

#ifndef __PRESENCEATTRIBUTEDEFVALUECRKEYS_H
#define __PRESENCEATTRIBUTEDEFVALUECRKEYS_H

// Presence Attribute Default Value API
// Presence Engine Presence Attribute's default value keys.
const TUid KCRUIDPresenceAttributeDefValue = {0x101F879E};



// Value to be used for user own Wireless Village ClientInfo presence attribute.
// Name of the device manufacture. Free text, e.g "MyCompanyX".
const TUint32 KPAWVClientInfoDevManufacturer = 0x00000001;


// Value to be used for user own Wireless Village ClientInfo presence attribute.
// Model of the client device. Free text, e.g. "1.2.3.x"
const TUint32 KPAWVClientInfoModel = 0x00000002;


// Value to be used for user own Wireless Village ClientInfo presence attribute.
// Name of client application producer. Free text, e.g "MyCompanyY"
const TUint32 KPAWVClientInfoClientProducer = 0x00000003;


// Value to be used for user own Wireless Village ClientInfo presence attribute.
// Version of the client application. Free text, e.g. "1.2.3.y"
const TUint32 KPAWVClientInfoClientVersion = 0x00000004;


// Value to be used for user own Wireless Village ClientInfo presence attribute.
// Language setting of the client device.
// Enumerated String: ISO 639-2/T (three letter language code) or empty value.
const TUint32 KPAWVClientInfoLanguage = 0x00000005;




#endif      // __PRESENCEATTRIBUTEDEFVALUECRKEYS_H


//  End of File


