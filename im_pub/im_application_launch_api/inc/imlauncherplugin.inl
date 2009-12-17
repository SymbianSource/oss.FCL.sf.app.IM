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
* Description:  Interface for the IM application launching
*
*/

inline CImLauncherPlugin::CImLauncherPlugin()
    {
    }

inline CImLauncherPlugin::~CImLauncherPlugin()
    {
    // Destroy any instance variables and then
    // inform the framework that this specific 
    // instance of the interface has been destroyed.
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

inline CImLauncherPlugin* CImLauncherPlugin::NewL(const TDesC8& aApplicationId)
    {
    // Set up the interface find for the default resolver.
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aApplicationId);
    resolverParams.SetWildcardMatch(EFalse);
    TAny* ptr = REComSession::CreateImplementationL(
                                        KImLauncherPluginUid,
                                        _FOFF(CImLauncherPlugin,iDtor_ID_Key),
                                        resolverParams);

    return REINTERPRET_CAST(CImLauncherPlugin*, ptr);
    }
