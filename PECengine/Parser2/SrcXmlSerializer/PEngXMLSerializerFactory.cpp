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
* Description:  XML serializer factory implementation.
*
*/


// INCLUDE FILES
#include <E32Std.h>
#include "CPEngXmlSerializer.h"

// ================= FACTORY FUNCTIONS ====================
EXPORT_C MPEngXMLSerializer* CreateXmlSerializerL( TDes8& aBuffer )
    {
    return CPEngXmlSerializer::NewL( aBuffer );
    }


EXPORT_C MPEngXMLSerializer* CreateXmlSerializerLC( TDes8& aBuffer )
    {
    return CPEngXmlSerializer::NewLC( aBuffer );
    }



//  End of File




