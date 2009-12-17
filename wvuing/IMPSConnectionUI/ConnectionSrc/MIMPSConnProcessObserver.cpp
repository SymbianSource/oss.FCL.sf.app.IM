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
* Description:  Default implementations for MIMPSConnProcessObserver.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <mimpsconnprocessobserverng.h>



// -----------------------------------------------------------------------------
// MIMPSConnProcessObserver::HandleSapAccessEventEventL()
//
// Default empty implementation.
// -----------------------------------------------------------------------------
//
EXPORT_C void MIMPSConnProcessObserver::HandleSapAccessEventEventL( const TDesC& /*aServerName*/,
                                                                    const TDesC& /*aServerURL*/,
                                                                    MIMPSConnProcessUi* /*aConnProcessUi*/ )
    {
    }


// -----------------------------------------------------------------------------
// MIMPSConnProcessObserver::ReservedObservMethod_1L()
//
// Empty reserved virtual function.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt MIMPSConnProcessObserver::ReservedObservMethod_1L()
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// MIMPSConnProcessObserver::ReservedObservMethod_2L()
//
// Empty reserved virtual function.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt MIMPSConnProcessObserver::ReservedObservMethod_2L()
    {
    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// MIMPSConnProcessObserver::ReservedObservMethod_3L()
//
// Empty reserved virtual function.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt MIMPSConnProcessObserver::ReservedObservMethod_3L()
    {
    return KErrNotSupported;
    }


//  End of File

