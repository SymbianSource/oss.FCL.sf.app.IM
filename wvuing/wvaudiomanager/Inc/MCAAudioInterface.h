/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Audio interface
*
*/

#ifndef MCCAAUDIOINTERFACE_H
#define MCCAAUDIOINTERFACE_H


#include <apgcli.h>
#include <mdaaudiosampleplayer.h>
#include <MdaAudioTonePlayer.h>
#include <bldvariant.hrh>

#include <centralrepository.h>
#include <CenRepNotifyHandler.h>
#include <barsc.h>
#include <Profile.hrh>

class MCAAudioManagerObserver;

class MCAAudioInterface

    {
    public:

        virtual void PlayL() = 0;

        virtual void Stop() = 0;

        virtual void SetVolume( TInt aVolume ) = 0;


        virtual void SetRingingType( TProfileRingingType aRingingType ) = 0;


        virtual void SetVibra( TBool aVibra ) = 0;

        virtual void AddObserverL( const MCAAudioManagerObserver* aObserver ) = 0;

        virtual void RemoveObserver( const MCAAudioManagerObserver* aObserver ) = 0;



        virtual ~MCAAudioInterface() {};


    };


#endif