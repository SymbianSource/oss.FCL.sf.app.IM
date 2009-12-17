/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common UID constants for wvuing.
*
*/


// uids for all component

#ifndef     __CHATNGCOMMONUIDS_H
#define     __CHATNGCOMMONUIDS_H

// MACROS
//Uid3:s part of wvservice setting
#define KWVSERVICESETINGDLLUID2 		0x1000008d
#define KWVSERVICESETINGDLLUID3 		0x10282E14

// part of imindicatorplugin dll
#define KECOMRECOGNITIONUID2            0x10009d8d
#define KIMINDICATORPLUGINUID3          0x102823BD
// There is only one implementation for IM indicator plugin
#define KIMINDICATORPLUGINIMPLEMENTATIONUIDVALUE    KIMINDICATORPLUGINUID3
#define KIMINDICATORPLUGININTERFACEUID  0x101FB0CC

// uids part of ImLauncher
#define KIMLAUNCHERUID2		                0x10009D8D
#define KIMLAUNCHERUID3                     0x102824CC
// There is only one implementation for IM application launcher
#define KIMLAUNCHERIMPLEMENTATIONUIDVALUE   KIMLAUNCHERUID3
#define KIMLAUNCHERINTERFACEUID             0x101FB0CC

// Uid2:s
// uids part of impsconnectionuing
#define KUNICODEAPPLICATIONUID2         0x100039ce 		// CSI: 8 # No need for uppercase
#define KSTATICINTERFACEDLLUID2         0x1000008d 		// CSI: 8 # No need for uppercase
#define KIMPSCONNECTIONUIMULTICSPUID3 	0x10282E13   	// CSI: 8 # No need for uppercase

// Uid3:s part of always online plugin
#define KIMPSCONNECTIONUIUID3           0x10005A37 		// CSI: 8 # No need for uppercase
#define KIMPSALWAYSONLINEUID2			0x10009D8D 	   // CSI: 8 # No need for uppercase
#define KIMPSALWAYSONLINEUID3			0x10282E12	   // CSI: 8 # No need for uppercase


//uids part of imutils
#define KIMUTILSDLLUID1			 	0x1000008d
#define KIMUTILSDLLUID2				0x10282460

//uids part of audiomanager
#define KAUDIOMANAGERUIDS1			0x1000008d
#define KAUDIOMANAGERUIDS2			0x1028243C

//uids part of wvcommandutils dll
#define KCOMMANDUTILSUIDS1			0x1000008d
#define KCOMMANDUTILSUIDS2          0x1028243F

//uid part of logger dll
#define KLOGGERDLLUIDS1             0x1000008d
#define KLOGGERDLLUIDS2 			0x10282441

// uids part of adpter
#define KADAPTERDLLUIDS1 			0x1000008d
#define KADAPTERDLLUIDS2  			0x10282440

// uids part of ave component
#define KWVUIAVEEXEUIDS1  			0x100039CE
#define KWVUIAVEEXEUIDS2			0x101F4673

//uids part of ui engine component
#define KWVUIENGINEIDS1             0x1000008d
#define KWVUIENGINEIDS2			    0x1028243A

// uids part of wvui presence component
#define KWVUIPRESENCEUIDS1          0x1000008d
#define KWVUIPRESENCEUIDS2			0x1028243B

// uids part od process component
#define KWVUIPROCESSUIDS1 			0x1000008d
#define KWVUIPROCESSUIDS2 			0x10282442

//uid s part of wv storage component
#define KWVUISTORAGEUIDS1           0x1000008d
#define KWVUISTORAGEUIDS2 			0x1028243D

//uids part of wvvariant component
#define KWVUIVAIANTUIDS1    		0x1000008d
#define KWVUIVAIANTUIDS2			0x1028243E

#endif // __CHATNGCOMMONUIDS_H

//  End of File


