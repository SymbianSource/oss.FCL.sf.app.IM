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
* Description:  Login data refresher.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <CIMPSSAPSettings.h>
#include "CnUiSapLoginDataRefresher.h"
#include "MCnUiUiFacade.h"


//CONSTS
/**
 * Constants to use.
 * For future: These could be merged with IMPS Common UI
 */
const TInt KUsernameMaxLength = 50;
const TInt KPasswordMaxLength = 50;



// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CnUiSapLoginDataRefresher::RefreshLoginDataL()
// -----------------------------------------------------------------------------
//
TBool CnUiSapLoginDataRefresher::RefreshLoginDataL( MCnUiUiFacade& aUi,
                                                    CIMPSSAPSettings& aSap,
                                                    TBool aReEdit )
    {
    HBufC* dataBuf = HBufC::NewLC( KUsernameMaxLength );
    HBufC* codeBuf = HBufC::NewLC( KPasswordMaxLength );
    TPtr data( dataBuf->Des() );
    TPtr code( codeBuf->Des() );

    data.Copy( aSap.SAPUserId().Left( KUsernameMaxLength ) );
    code.Copy( aSap.SAPUserPassword().Left( KPasswordMaxLength ) );


    TCnUiLoginDataQueryMode mode = ECnUiNormal;
    if ( aReEdit )
        {
        mode = ECnUiReEdit;
        }


    TBool queryAccepted = aUi.LoginQueryL( data, code, aSap, mode );
    if ( queryAccepted )
        {
        aSap.SetSAPUserIdL( *dataBuf );
        aSap.SetSAPUserPasswordL( *codeBuf );
        }

    CleanupStack::PopAndDestroy( 2 ); //dataBuf, codeBuf
    return queryAccepted;
    }


//  End of File
