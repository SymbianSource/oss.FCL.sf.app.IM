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
* Description:  Interface for Presence Engine Plugins.
*
*/

#ifndef __CPEngPluginInterface2_INL__
#define __CPEngPluginInterface2_INL__


inline CPEngPluginInterface2* CPEngPluginInterface2::NewL( TUid aImplementationUid )
    {
    TAny* ptr = REComSession::CreateImplementationL( aImplementationUid, _FOFF( CPEngPluginInterface2, iDtor_ID_Key ) );
    CPEngPluginInterface2* impl = reinterpret_cast<CPEngPluginInterface2*>( ptr );
    return impl;
    }

inline CPEngPluginInterface2::~CPEngPluginInterface2()
    {
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

inline CPEngPluginInterface2::CPEngPluginInterface2()
    {
    }


#endif // __CPEngPluginInterface2_INL__

// End of File
