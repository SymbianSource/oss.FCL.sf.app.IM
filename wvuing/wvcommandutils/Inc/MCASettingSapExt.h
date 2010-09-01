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
* Description: Interface to get the Settings Interface for getting/setting tonefilename etc
*
*/
#ifndef MCASETTINGSAPEXT_H
#define MCASETTINGSAPEXT_H

#include <badesca.h>
#include <e32std.h>

#include "MCASettingsPc.h"

//#include"MCASettings.h"
//class MCASettings;
class CIMPSSAPSettings;
class MCASettingsPc;


class MCASettingSapExt
    {
    public:

        /*

        */
        virtual void SetIntValueL( const TDesC& aKey, TInt aValue ) = 0;

        /*

        */
        virtual TInt IntValueL( const TDesC& aKey ) = 0;

        /*

        */
        virtual void SetToneFileNameL( const TDesC& aToneFileName ) = 0;

        /*

        */
        virtual void GetToneFileNameL( TDes& aToneFileName ) = 0;

        /*

        */
        virtual  CIMPSSAPSettings* CurrentSAPLC() = 0;
        /*

        */
        virtual  CIMPSSAPSettings* StoredSAPLC() = 0;



        /*
          Destructor
        */

        virtual ~MCASettingSapExt() {};


    };

#endif
