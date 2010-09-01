/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: A class for CImpsServer inline function
* 
*/


inline TBool CImpsServer::IsConAllowed() const
    {
    return iConAllowed;
    }
          
inline TImpsCspVersion CImpsServer::CspVersion()
    {	
    return iCSPVersion;
    }	
	
inline TBool CImpsServer::IsShuttingDown()
    {
    return iOperation == EImpsSrvShuttingDown ? ETrue : EFalse;
    }
    
inline const TImpsSrvSettings* CImpsServer::Settings() const
    {
    return &iSettings;
    }
    


    

