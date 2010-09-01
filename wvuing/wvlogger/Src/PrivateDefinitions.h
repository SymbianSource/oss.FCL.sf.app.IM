/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Constants for CALogger module
*
*/


#ifndef PRIVATEDEFINITIONS_H
#define PRIVATEDEFINITIONS_H

class MCALoggerWriteInterface;
class MCALoggerMessageHeader;

// History folder
_LIT( KCAHistoryDirectory, "IMHistory\\" );
_LIT( KCAHistoryTocFile, "IMHistory\\toc" );
_LIT( KCAHistoryTocTempFile, "IMHistory\\tocTemp" );
_LIT( KCAHistoryDataDirectory, "IMHistory\\data\\" );
_LIT( KCADataDirectory, "data\\" );
_LIT( KCALoggerModuleName, "WV Logger" );

struct TFileRelation
    {
    MCALoggerWriteInterface* iWriter;
    MCALoggerMessageHeader* iFile;
    };

_LIT( KLineEnd, "\n" );
const TInt KWVUserIDMaxLength       = 50;
const TInt KWVMaxLengthOfIntString  = 10;

#ifdef _UNICODE
const TInt KCharacterLengthInBytes = 2;
#else
const TInt KCharacterLengthInBytes = 1;
#endif

#endif	// PRIVATEDEFINITIONS_H
//end of file

