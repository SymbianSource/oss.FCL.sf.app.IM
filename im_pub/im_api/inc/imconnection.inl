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
* Description:  IM Connection inline functions
*
*/

inline CImConnection::CImConnection()
	{
	}

inline CImConnection::~CImConnection()
	{
	// Destroy any instance variables and then
	// inform the framework that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);

    }

inline CImConnection* CImConnection::NewL(const TDesC& aClientId)
    {
    // Set up the interface find for the default resolver.
    
    TUid KDefaultImplemantationUid = {0x101FB0CA};
    TAny* ptr = REComSession::CreateImplementationL(
        KDefaultImplemantationUid, 
        _FOFF(CImConnection,iDtor_ID_Key),
        (TAny*) &aClientId);
    
    
    return REINTERPRET_CAST(CImConnection*, ptr);
    }

