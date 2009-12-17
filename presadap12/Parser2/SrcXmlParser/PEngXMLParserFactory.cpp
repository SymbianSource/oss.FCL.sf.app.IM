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
* Description:  XML parser factory implementation.
*
*/


// INCLUDE FILES
#include <E32Std.h>
#include "CPEngXMLParser.h"


// ================= FACTORY FUNCTIONS ====================
EXPORT_C MPEngXMLParser* CreateXMLParserL()
    {
    CPEngXMLParser* parser = CPEngXMLParser::NewLC();
    CleanupStack::Pop(); // parser
    return parser;
    }


EXPORT_C MPEngXMLParser* CreateXMLParserLC()
    {
    return CPEngXMLParser::NewLC();
    }



//  End of File




