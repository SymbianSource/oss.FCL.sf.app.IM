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
* Description:  Core XML defines collected in one place.
*
*/

#ifndef __PENGXMLDEFS_H__
#define __PENGXMLDEFS_H__

//XML tag parts
_LIT8( KXmlTagStart, "<" );
_LIT8( KXmlEndTagStart, "</" );
_LIT8( KXmlTagEnd, ">" );
_LIT8( KXmlEmptyTagEnd, "/>" );




//XML special characters
_LIT8( KXmlEqualSign, "=" );
_LIT8( KXmlSlash, "/" );
_LIT8( KXmlApostrophe, "'" );
_LIT8( KXmlDoubleQuote, "\"" );
_LIT8( KXmlWhiteSpace, " " );


//XML entity references.
_LIT8( KEntityAmpersand, "&amp;" );
_LIT8( KEntityGreaterThan, "&gt;" );
_LIT8( KEntityLowerThan, "&lt;" );
_LIT8( KEntityDoubleQuote, "&quot;" );
_LIT8( KEntityApostrophe, "&apos;" );


//Special characters
_LIT16( KPercentSign16, "%" );
_LIT8( KPercentSign8, "%" );


#endif      // __PENGXMLDEFS_H__