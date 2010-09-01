/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  imlaunchertest test module.
*
*/


#ifndef TIMERTOOL_H
#define TIMERTOOL_H

class CTimerTool : public CActive
    {
    public: //member functions
        static CTimerTool* NewL();
        CTimerTool();
        ~CTimerTool();
        void ConstructL();
        void Start( TTimeIntervalMicroSeconds32 aInterval );
        TInt Stop();


    public: //data members
        TInt error;


    protected:
        void DoCancel();
        void RunL();

    private:
        RTimer			 	iTimer;

    };

#endif //TIMERTOOL_H